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


#include <QLabel>
#include <QLayout>
#include <QLayoutItem>
#include <QRect>
#include <QSize>
#include <QWidget>

#include "columnlayout.h"


ColumnLayout::ColumnLayout(int hSpacing, int vSpacing)
{
   m_hSpacing = hSpacing;
   m_vSpacing = vSpacing;

   m_lastChildWidth = 0;
   m_lastChildHeight = 0;
}


ColumnLayout::~ColumnLayout()
{
   QLayoutItem *item;
   while ((item = takeAt(0)))
   {
      delete item;
   }
}


int ColumnLayout::count() const
{
   return list.size();
}


QLayoutItem *ColumnLayout::itemAt(int index) const
{
   return list.value(index);
}


QLayoutItem *ColumnLayout::takeAt(int index)
{
   if (index >= 0 && index < list.size())
   {
      return list.takeAt(index);
   }
   else
   {
      return 0;
   }
}


void ColumnLayout::addItem(QLayoutItem *item)
{
   list.append(item);
}


void ColumnLayout::addWidget2(QWidget * w, Qt::Alignment alignment)
{
   addWidget(w);
   setAlignment(w, alignment);
   
   // Technically it's a Card, but Card is subclass of QLabel so whatever
   m_lastChildWidth  = ((QLabel*)w)->pixmap()->width();
   m_lastChildHeight = ((QLabel*)w)->pixmap()->height();
}


void ColumnLayout::setGeometry(const QRect &r)
{
   QLayout::setGeometry(r);

   if (list.size() == 0)
      return;

   int w = r.width() - list.count() * m_hSpacing;
   int h = r.height() - (list.count()-1) * m_vSpacing;
   int i = 0;
   while (i < list.size()) {
      QLayoutItem *o = list.at(i);
      QRect geom(r.x() + i * m_hSpacing, r.y() + i * m_vSpacing, w, h);
      o->setGeometry(geom);
      ++i;
   }
}


QSize ColumnLayout::sizeHint() const
{
   QSize s(m_lastChildWidth,m_lastChildHeight);
   int n = list.count();
   int i = 0;
   while (i < n) {
      QLayoutItem *o = list.at(i);
      s = s.expandedTo(o->sizeHint());
      ++i;
   }
   if (n == 0)
      return s;
   else
      return s + (n-1)*QSize(m_hSpacing, m_vSpacing);
}


QSize ColumnLayout::minimumSize() const
{
   QSize s(m_lastChildWidth,m_lastChildHeight);
   int n = list.count();
   int i = 0;
   while (i < n) {
      QLayoutItem *o = list.at(i);
      s = s.expandedTo(o->minimumSize());
      ++i;
   }
   if (n == 0)
      return s;
   else
      return s + (n-1)*QSize(m_hSpacing, m_vSpacing);
}


Qt::Orientations ColumnLayout::expandingDirections() const
{
   return Qt::Vertical;
}


int ColumnLayout::horizontalSpacing()
{
   return m_hSpacing;
}


int ColumnLayout::verticalSpacing()
{
   return m_vSpacing;
}
