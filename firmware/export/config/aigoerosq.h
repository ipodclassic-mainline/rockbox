/*
 * This config file is for the AIGO EROS Q / EROS K (and its clones)
 */

/* For Rolo and boot loader */
#define MODEL_NUMBER 113

#define MODEL_NAME   "AIGO Eros Q"

/* LCD dimensions */
#define LCD_WIDTH  320
#define LCD_HEIGHT 240
/* sqrt(240^2 + 320^2) / 2.0 = 200 */
#define LCD_DPI 200

#ifndef SIMULATOR
#define CONFIG_PLATFORM (PLATFORM_HOSTED)
#define PIVOT_ROOT "/mnt/sd_0"
#endif

#define HIBY_LINUX
#define HAVE_FPU

/* define this if you have a colour LCD */
#define HAVE_LCD_COLOR

#define HAVE_LCD_ENABLE

/* Define this if the LCD can shut down */
#define HAVE_LCD_SHUTDOWN

/* define this if you want album art for this target */
#define HAVE_ALBUMART

/* define this to enable bitmap scaling */
#define HAVE_BMP_SCALING

/* define this to enable JPEG decoding */
#define HAVE_JPEG

/* define this if you have access to the quickscreen */
#define HAVE_QUICKSCREEN

/* define this if you would like tagcache to build on this target */
#define HAVE_TAGCACHE

#define LCD_DEPTH  32
/* Check that but should not matter */
#define LCD_PIXELFORMAT XRGB8888

#define HAVE_BACKLIGHT
#define HAVE_BACKLIGHT_BRIGHTNESS

/* Main LCD backlight brightness range and defaults: the backlight driver
 * has levels from 0 to 255. But 0 is off so start at 1.
 */
#define MIN_BRIGHTNESS_SETTING      1
#define MAX_BRIGHTNESS_SETTING      255
#define BRIGHTNESS_STEP             5
#define DEFAULT_BRIGHTNESS_SETTING  70

/* Which backlight fading type? */
#define CONFIG_BACKLIGHT_FADING BACKLIGHT_FADING_SW_SETTING

/* define this if you have a real-time clock */
#define CONFIG_RTC APPLICATION

/* The number of bytes reserved for loadable codecs */
#define CODEC_SIZE 0x80000

/* The number of bytes reserved for loadable plugins */
#define PLUGIN_BUFFER_SIZE 0x100000

#define HAVE_HEADPHONE_DETECTION
#define HAVE_LINEOUT_DETECTION

/* KeyPad configuration for plugins */
#define CONFIG_KEYPAD EROSQ_PAD

/* define this if the target has volume keys which can be used in the lists */
#define HAVE_VOLUME_IN_LIST

#ifndef SIMULATOR
/* We have usb power and can detect usb but it is handled by Linux */
#define HAVE_USB_POWER

#endif

#define CONFIG_BATTERY_MEASURE VOLTAGE_MEASURE

/* Linux controlls charging, we can monitor */
#define CONFIG_CHARGING CHARGING_MONITOR

/* define this if the hardware can be powered off while charging */
#define HAVE_POWEROFF_WHILE_CHARGING

/* same dimensions as gigabeats */
#define CONFIG_LCD LCD_INGENIC_LINUX

/* Define this if you have a software controlled poweroff */
#define HAVE_SW_POWEROFF

/* Define this to the CPU frequency */
#define CPU_FREQ           108000000

/* No special storage */
#define CONFIG_STORAGE STORAGE_HOSTFS
#define HAVE_STORAGE_FLUSH

/* Battery */
#define BATTERY_TYPES_COUNT  1

/* Audio codec */
#define HAVE_EROSQ_LINUX_CODEC
/* Rockbox has to handle the volume level */
#define HAVE_SW_VOLUME_CONTROL

/* We don't have hardware controls */
#define HAVE_SW_TONE_CONTROLS

/* HW codec is flexible */
#define HW_SAMPR_CAPS SAMPR_CAP_ALL_192

/* Battery */
#define BATTERY_CAPACITY_DEFAULT 1300 /* default battery capacity */
#define BATTERY_CAPACITY_MIN 1300  /* min. capacity selectable */
#define BATTERY_CAPACITY_MAX 1500 /* max. capacity selectable */
#define BATTERY_CAPACITY_INC 0   /* capacity increment */

#define CURRENT_NORMAL 100      // 1.7mA * 60s
#define CURRENT_BACKLIGHT 180
#define CURRENT_MAX_CHG 500     // bursts higher if needed

/* ROLO */
#define BOOTFILE_EXT "erosq"
#define BOOTFILE     "rockbox." BOOTFILE_EXT
#define BOOTDIR      "/.rockbox"
