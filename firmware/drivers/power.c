/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2002 by Linus Nielsen Feltzing
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#include "config.h"
#include "cpu.h"
#include <stdbool.h>
#include "adc.h"
#include "kernel.h"
#include "system.h"
#include "power.h"
#include "hwcompat.h"

#ifdef HAVE_CHARGE_CTRL
bool charger_enabled;
#endif


#ifdef CONFIG_TUNER

static int fmstatus = 0;

void radio_set_status(int status)
{
#ifdef IRIVER_H100
    fmstatus = status;
#else
    fmstatus = status;
#ifdef HAVE_TUNER_PWR_CTRL
    if (status)
    {
        and_b(~0x04, &PADRL); /* drive PA2 low for tuner enable */
        sleep(1); /* let the voltage settle */
    }
    else
        or_b(0x04, &PADRL); /* drive PA2 high for tuner disable */
#endif
#endif
}

int radio_get_status(void)
{
    return fmstatus;
}

#endif /* #ifdef CONFIG_TUNER */

#ifndef SIMULATOR

void power_init(void)
{
#ifdef IRIVER_H100
    GPIO1_OUT |= 0x00080000;
    GPIO1_ENABLE |= 0x00080000;
    GPIO1_FUNCTION |= 0x00080000;
    
    GPIO_ENABLE |= 0x80000000;
    GPIO_FUNCTION |= 0x80000000;
#else
#ifdef HAVE_CHARGE_CTRL
    or_b(0x20, &PBIORL); /* Set charging control bit to output */
    charger_enable(false); /* Default to charger OFF */
#endif
#ifdef HAVE_TUNER_PWR_CTRL
    PACR2 &= ~0x0030;  /* GPIO for PA2 */
    or_b(0x04, &PADRL); /* drive PA2 high for tuner disable */
    or_b(0x04, &PAIORL); /* output for PA2 */
#endif
#endif
}

bool charger_inserted(void)
{     
#ifdef IRIVER_H100
    return (GPIO1_READ & 0x00400000)?true:false;
#elif defined(GMINI_ARCH)
    return (P7 & 0x80) == 0;
#else
#ifdef HAVE_CHARGING
#ifdef HAVE_CHARGE_CTRL
    /* Recorder */
    return adc_read(ADC_EXT_POWER) > 0x100;
#else
#ifdef HAVE_FMADC
    /* FM or V2, can also charge from the USB port */
    return (adc_read(ADC_CHARGE_REGULATOR) < 0x1FF) ||
        (adc_read(ADC_USB_POWER) < 0x1FF);
#else
    /* Player */
    return (PADR & 1) == 0;
#endif /* HAVE_FMADC */
#endif /* HAVE_CHARGE_CTRL */
#else
    /* Ondio */
    return false;
#endif /* HAVE_CHARGING */
#endif
}

void charger_enable(bool on)
{
    (void)on;
#ifdef HAVE_CHARGE_CTRL
    if(on) 
    {
        and_b(~0x20, &PBDRL);
        charger_enabled = 1;
    } 
    else 
    {
        or_b(0x20, &PBDRL);
        charger_enabled = 0;
    }
#endif
}

#ifndef HAVE_MMC
void ide_power_enable(bool on)
{
    (void)on;

#ifdef IRIVER_H100
    if(on)
        GPIO_OUT &= ~0x80000000;
    else
        GPIO_OUT |= 0x80000000;
#elif defined(GMINI_ARCH)
    if (on)
        P1 |= 0x04;
    else
        P2 &= ~0x04;
#else
    bool touched = false;
#ifdef NEEDS_ATA_POWER_ON
    if(on)
    {
#ifdef ATA_POWER_PLAYERSTYLE
        or_b(0x10, &PBDRL);
#else
        or_b(0x20, &PADRL);
#endif
        touched = true;
    }
#endif
#ifdef HAVE_ATA_POWER_OFF
    if(!on)
    {
#ifdef ATA_POWER_PLAYERSTYLE
        and_b(~0x10, &PBDRL);
#else
        and_b(~0x20, &PADRL);
#endif
        touched = true;
    }
#endif

/* late port preparation, else problems with read/modify/write 
   of other bits on same port, while input and floating high */
    if (touched)
    {
#ifdef ATA_POWER_PLAYERSTYLE
        or_b(0x10, &PBIORL); /* PB4 is an output */
        PBCR2 &= ~0x0300; /* GPIO for PB4 */
#else
        or_b(0x20, &PAIORL); /* PA5 is an output */
        PACR2 &= 0xFBFF; /* GPIO for PA5 */
#endif
    }
#endif
}
#endif /* !HAVE_MMC */


bool ide_powered(void)
{
#ifdef IRIVER_H100
    return (GPIO_OUT & 0x80000000)?false:true;
#elif defined(GMINI_ARCH)
    return (P1 & 0x04?true:false);
#else
#if defined(NEEDS_ATA_POWER_ON) || defined(HAVE_ATA_POWER_OFF)
#ifdef ATA_POWER_PLAYERSTYLE
    /* This is not correct for very old players, since these are unable to
     * control hd power. However, driving the pin doesn't hurt, because it
     * is not connected anywhere */
    if ((PBCR2 & 0x0300) || !(PBIORL & 0x10)) /* not configured for output */
        return false; /* would be floating low, disk off */
    else
        return (PBDRL & 0x10) != 0;
#else
    if ((PACR2 & 0x0400) || !(PAIORL & 0x20)) /* not configured for output */
        return true; /* would be floating high, disk on */
    else
        return (PADRL & 0x20) != 0;
#endif /* ATA_POWER_PLAYERSTYLE */
#else
    return true; /* pretend always powered if not controlable */
#endif
#endif
}


void power_off(void)
{
    set_irq_level(HIGHEST_IRQ_LEVEL);
#ifdef IRIVER_H100
    GPIO1_OUT &= ~0x00080000;
#elif defined(GMINI_ARCH)
    P1 &= ~1;
    P1CON &= ~1;
#else
#ifdef HAVE_POWEROFF_ON_PBDR
    and_b(~0x10, &PBDRL);
    or_b(0x10, &PBIORL);
#elif defined(HAVE_POWEROFF_ON_PB5)
    and_b(~0x20, &PBDRL);
    or_b(0x20, &PBIORL);
#else
    /* Disable the backlight */
    and_b(~0x40, &PAIORH);

    and_b(~0x08, &PADRH);
    or_b(0x08, &PAIORH);
#endif
#endif
    while(1);
}

#else

bool charger_inserted(void)
{
    return false;
}

void charger_enable(bool on)
{
    (void)on;
}

void power_off(void)
{
}

void ide_power_enable(bool on)
{
   (void)on;
}

#endif /* SIMULATOR */
