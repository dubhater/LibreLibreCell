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


#ifndef COLUMN_H
#define COLUMN_H

#include <QEvent>
#include <QFrame>
#include <QMouseEvent>
#include <QObject>

#include "columnlayout.h"


class Column : public QFrame
{
   Q_OBJECT

   public:
      Column(int number, int spacing);

      ColumnLayout *layout;

      bool isSelected();

   signals:
      void selected(int number, bool sel);
      void leftClicked(int number, Qt::KeyboardModifiers modifs);
      void rightClicked(int number);
      void leftDoubleClicked(int number);
      void mouseEntered(int number);
      void mouseLeft(int number);
   
   public slots:
      void setSelected(bool sel);

   private:
      void mousePressEvent(QMouseEvent * event);
      void mouseDoubleClickEvent(QMouseEvent * event);
      void enterEvent(QEvent *);
      void leaveEvent(QEvent *);

      bool m_selected;
      // The number of the column (0-7).
      int m_number;
};

#endif
