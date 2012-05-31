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


#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QComboBox>
#include <QDialog>
#include <QDir>
#include <QLineEdit>
#include <QObject>
#include <QSpinBox>
#include <QStringList>


class Preferences : public QDialog
{
   Q_OBJECT

   public slots:
      void accept();
      void onSpacingChanged(int i);
      void onChooseMessageFont();
      void onChooseCardSetPath();
      void onChooseCardSet(int index);

   public:
      Preferences(QWidget * parent = 0, Qt::WindowFlags f = 0);

   private:
      int     m_spacing;
      QFont   m_messageFont;
      QString m_pathToCardSets;
      QString m_cardSet;
      bool    m_cardSetIsInternal;

      QString m_prefix;
      QDir    m_prefixDir;

      QStringList m_cardSetList;
      int m_internalCardSets;

      QSpinBox    *m_spacingBox;
      QPushButton *m_messageFontButton;
      QLineEdit   *m_cardSetsPathEdit;
      QComboBox   *m_cardSetComboBox;


      void  readSettings();
      void writeSettings();
      void addInternalCardSets(QString path);
      void addExternalCardSets(QString path);
};

#endif
