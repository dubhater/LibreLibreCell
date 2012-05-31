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


//#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFile>
#include <QFontMetrics>
#include <QIODevice>
#include <QLabel>
#include <QLatin1Char>
#include <QObject>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QTextOption>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>

#include "about.h"

#ifndef LIBRELIBRECELL_VERSION
#define LIBRELIBRECELL_VERSION "unknown"
#endif

About::About(QWidget * parent, Qt::WindowFlags f)
   : QDialog(parent, f)
{
   // stuff
   setWindowTitle("About LibreLibreCell");

   QLabel *versionLabel = new QLabel(QString("Git revision: %1").arg(LIBRELIBRECELL_VERSION));
   QPlainTextEdit *aboutBox = new QPlainTextEdit;
   QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

   QVBoxLayout *vLayout = new QVBoxLayout;
   vLayout->addWidget(versionLabel);
   vLayout->addWidget(aboutBox, 1);
   vLayout->addSpacing(10);
   vLayout->addWidget(buttonBox);

   setLayout(vLayout);


   aboutBox->setReadOnly(true);
   aboutBox->setTabChangesFocus(true);
   aboutBox->setWordWrapMode(QTextOption::NoWrap);
   aboutBox->setLineWrapMode(QPlainTextEdit::NoWrap);
   aboutBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   aboutBox->setFont(QFont("Monospace", 10));

   QFile readme(":/README.txt");
   if (readme.open(QIODevice::ReadOnly))
   {
      QTextStream stream(&readme);
      aboutBox->setPlainText(stream.readAll());
      readme.close();
   }
   else
   {
      aboutBox->setPlainText("Could not open README.txt.");
   }
   QFontMetrics fm(aboutBox->font());
   aboutBox->setMinimumSize(fm.width(QLatin1Char('m')) * 76, 450);

   connect(buttonBox, SIGNAL(accepted()),
               this, SLOT(accept()));
}
