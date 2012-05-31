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


#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QObject>
#include <QWidget>


class About : public QDialog
{
   Q_OBJECT

   public:
      About(QWidget * parent = 0, Qt::WindowFlags = 0);
};

#endif
