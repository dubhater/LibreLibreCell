/*
 *
 * Copyright (C) 2012 dubhater
 *
 * This file is part of LibreLibreCell.
 *
 * LibreLibreCell is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 *
 */


#include <QFrame>
#include <QMouseEvent>
#include <QObject>

#include "card.h"
#include "cornerwidget.h"


CornerWidget::CornerWidget(int number)
{
   m_number = number;
   m_selected = false;

   setFrameStyle(QFrame::Box | QFrame::Plain);

   connect(this, SIGNAL(currentChanged(int)),
           this, SLOT(onCurrentChanged(int)));
}


void CornerWidget::mousePressEvent(QMouseEvent * event)
{
   switch (event->button())
   {
      case Qt::LeftButton:
      {
         emit clicked(m_number);
         event->accept();
         break;
      }
      default:
      {
         event->ignore();
         break;
      }
   }
}


bool CornerWidget::isSelected()
{
   return m_selected;
}


void CornerWidget::setSelected(bool sel)
{
   if (m_selected != sel)
   {
      m_selected = sel;
      if (count() > 0)
      {
         ((Card*)widget(count()-1))->setInverted(m_selected);
      }
   }
}


void CornerWidget::onCurrentChanged(int index)
{
   emit currentChangedWithNumber(index, m_number);
}
