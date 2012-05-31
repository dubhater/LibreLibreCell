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


#ifndef COLUMNLAYOUT_H
#define COLUMNLAYOUT_H

#include <QLayout>
#include <QLayoutItem>
#include <QList>
#include <QRect>
#include <QSize>
#include <QWidget>


class ColumnLayout : public QLayout
{
   public:
      ColumnLayout(int hSpacing, int vSpacing);
      ~ColumnLayout();

      void addItem(QLayoutItem *item);
      void addWidget2(QWidget * w, Qt::Alignment alignment);

      int count() const;
      QLayoutItem *itemAt(int index) const;
      QLayoutItem *takeAt(int index);

      QSize sizeHint() const;
      QSize minimumSize() const;

      void setGeometry(const QRect &rect);
      Qt::Orientations expandingDirections() const;

      int horizontalSpacing();
      int verticalSpacing();

   private:
      QList<QLayoutItem*> list;

      int m_hSpacing, m_vSpacing;

      int m_lastChildWidth, m_lastChildHeight;
};

#endif
