/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * This file has been copied from Nokia's Qt Examples, with minor modifications
 * made available under the LGPL version 2.1, as the original file was licensed
 *
 ****************************************************************************
 ****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include <QtGui>
#include <QApplication>

#include "codeeditor.h"

//![constructor]

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent), completer(this)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)),
            this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)),
            this, SLOT(updateLineNumberArea(QRect,int)));

    updateLineNumberAreaWidth(0);

    QObject::connect(this, SIGNAL(cursorPositionChanged()),
                     this, SLOT(cursorMoved()));
    completer.hide();
    settings.beginGroup("CodeEditor");
}

//![constructor]

//![extraAreaWidth]

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

//![extraAreaWidth]

//![slotUpdateExtraAreaWidth]

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{ 
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

//![slotUpdateExtraAreaWidth]

//![slotUpdateRequest]

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

//![slotUpdateRequest]

void CodeEditor::cursorMoved()
{
    /* Closing the completer if the cursor has moved out of its bounds */
    if(completer.isVisible())
    {
        if(textCursor().position() < tagBegin
           || textCursor().position() > tagEnd)
        {
            completer.hide();
        }
    }
}

//![resizeEvent]

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                      lineNumberAreaWidth(), cr.height()));
}

//![resizeEvent]

void CodeEditor::keyPressEvent(QKeyEvent *event)
{

    if(!settings.value("completeSyntax", false).toBool())
    {
        QPlainTextEdit::keyPressEvent(event);
        return;
    }

    if(completer.isVisible())
    {
        /* Handling the completer */
        if(event->key() == Qt::Key_Up)
        {
            /* Up/down arrow presses get sent right along to the completer
             * to navigate through the list
             */
            if(completer.currentIndex().row() > 0)
                QApplication::sendEvent(&completer, event);
        }
        else if(event->key() == Qt::Key_Down)
        {
            if(completer.currentIndex().row()
                < completer.topLevelItemCount() - 1)
                QApplication::sendEvent(&completer, event);
        }
        else if(event->key() == Qt::Key_Backspace)
        {
            tagEnd--;
            QPlainTextEdit::keyPressEvent(event);
        }
        else if(event->key() == Qt::Key_Escape)
        {
            /* Escape hides the completer */
            completer.hide();
            QPlainTextEdit::keyPressEvent(event);
        }
        else if(event->key() == Qt::Key_Enter)
        {
            /* The enter key inserts the currently selected tag */
        }
        else if(event->key() == Qt::Key_Question)
        {
            /* The question mark doesn't filter the list */
            tagEnd++;
            QPlainTextEdit::keyPressEvent(event);
        }
        else if(event->key() == Qt::Key_Left
                || event->key() == Qt::Key_Right)
        {
            /* Left and right keys shouldn't affect tagEnd */
            QPlainTextEdit::keyPressEvent(event);
        }
        else
        {
            /* Otherwise, we have to filter the list */
            tagEnd++;
            QPlainTextEdit::keyPressEvent(event);

            QString filterText = "";
        }
    }
    else
    {
        /* Deciding whether to show the completer */
        QPlainTextEdit::keyPressEvent(event);
        if(event->key() == Qt::Key_Percent)
        {
            tagBegin = textCursor().position();
            tagEnd = textCursor().position();
            completer.filter("");
            completer.move(cursorRect().left(), cursorRect().bottom());
            if(completer.frameGeometry().right() > width())
                completer.move(width() - completer.width(), completer.y());
            if(completer.frameGeometry().bottom() > height())
                completer.move(completer.x(),
                               cursorRect().top() - completer.height());
            completer.show();
        }
    }

}

//![extraAreaPaintEvent_0]

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

//![extraAreaPaintEvent_0]

//![extraAreaPaintEvent_1]
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
//![extraAreaPaintEvent_1]

//![extraAreaPaintEvent_2]
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            /* Drawing an error circle if necessary */
            if(errors.contains(blockNumber + 1))
            {
                painter.fillRect(QRect(0, top, lineNumberArea->width(),
                                       fontMetrics().height()), errorColor);
            }
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(),
                             fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
//![extraAreaPaintEvent_2]

