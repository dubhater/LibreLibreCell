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


#ifndef CORNERWIDGET_H
#define CORNERWIDGET_H

#include <QMouseEvent>
#include <QObject>
#include <QStackedWidget>


class CornerWidget : public QStackedWidget
{
   Q_OBJECT

   public:
      CornerWidget(int number);

      bool isSelected();
      void setSelected(bool sel);
      
   signals:
      void clicked(int number);
      void currentChangedWithNumber(int index, int number);

   private slots:
      void onCurrentChanged(int index);

   private:
      void mousePressEvent(QMouseEvent * event);

      int m_number;
      bool m_selected;
};

#endif
