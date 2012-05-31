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


#include <QLayout>
#include <QEvent>
#include <QMouseEvent>

#include "card.h"
#include "column.h"
#include "columnlayout.h"


Column::Column(int number, int spacing)
{
   // initialise
   m_number = number;
   m_selected = false;
   layout = new ColumnLayout(0, spacing);
   layout->setSizeConstraint(QLayout::SetMinimumSize);
   setLayout(layout);
   //setFrameStyle(QFrame::Box);
}


bool Column::isSelected()
{
   return m_selected;
}


void Column::setSelected(bool sel)
{
   if (sel != m_selected)
   {
      m_selected = sel;
      emit selected(m_number, m_selected);
      ((Card*)layout->itemAt(layout->count()-1)->widget())->setInverted(m_selected);
   }
}


void Column::mousePressEvent(QMouseEvent * event)
{
   switch (event->button())
   {
      case Qt::LeftButton:
      {
         emit leftClicked(m_number, event->modifiers());
         event->accept();
         break;
      }
      case Qt::RightButton:
      {
         emit rightClicked(m_number);
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


void Column::mouseDoubleClickEvent(QMouseEvent * event)
{
   switch (event->button())
   {
      case Qt::LeftButton:
      {
         emit leftDoubleClicked(m_number);
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


void Column::enterEvent(QEvent *)
{
   emit mouseEntered(m_number);
}


void Column::leaveEvent(QEvent *)
{
   emit mouseLeft(m_number);
}
