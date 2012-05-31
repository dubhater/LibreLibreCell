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


#include <QComboBox>
//#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFont>
#include <QFontDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QString>
#include <QVBoxLayout>

#include "preferences.h"


Preferences::Preferences(QWidget * parent, Qt::WindowFlags f)
   : QDialog(parent, f)
{
   setWindowTitle("Preferences");

   QLabel *spacingLabel = new QLabel("S&pacing:");
   m_spacingBox = new QSpinBox;
   m_spacingBox->setRange(10, 100);
   m_spacingBox->setSuffix(" px");
   spacingLabel->setBuddy(m_spacingBox);
   QVBoxLayout *spacingVLayout = new QVBoxLayout;
   spacingVLayout->setSpacing(2);
   spacingVLayout->addWidget(spacingLabel);
   spacingVLayout->addWidget(m_spacingBox);
   QHBoxLayout *spacingHLayout = new QHBoxLayout;
   spacingHLayout->addLayout(spacingVLayout);
   spacingHLayout->addStretch(1);

   QLabel *messageFontLabel = new QLabel("Message &font:");
   m_messageFontButton = new QPushButton;
   messageFontLabel->setBuddy(m_messageFontButton);
   QVBoxLayout *messageFontVLayout = new QVBoxLayout;
   messageFontVLayout->setSpacing(2);
   messageFontVLayout->addWidget(messageFontLabel);
   messageFontVLayout->addWidget(m_messageFontButton);
   QHBoxLayout *messageFontHLayout = new QHBoxLayout;
   messageFontHLayout->addLayout(messageFontVLayout);
   messageFontHLayout->addStretch(1);

   QLabel *cardSetsPathLabel = new QLabel("Path to a&dditional card sets:");
   m_cardSetsPathEdit = new QLineEdit;
   m_cardSetsPathEdit->setReadOnly(true);
   cardSetsPathLabel->setBuddy(m_cardSetsPathEdit);
   QPushButton *cardSetsPathBrowse = new QPushButton("&Browse");
   QHBoxLayout *cardSetsPathHLayout = new QHBoxLayout;
   cardSetsPathHLayout->addWidget(m_cardSetsPathEdit, 1);
   cardSetsPathHLayout->addSpacing(5);
   cardSetsPathHLayout->addWidget(cardSetsPathBrowse, 0);
   QVBoxLayout *cardSetsPathVLayout = new QVBoxLayout;
   cardSetsPathVLayout->setSpacing(2);
   cardSetsPathVLayout->addWidget(cardSetsPathLabel);
   cardSetsPathVLayout->addLayout(cardSetsPathHLayout);
   cardSetsPathVLayout->addStretch(1);

   QLabel *cardSetLabel = new QLabel("Card &set:");
   m_cardSetComboBox = new QComboBox;
   cardSetLabel->setBuddy(m_cardSetComboBox);
   QVBoxLayout *cardSetVLayout = new QVBoxLayout;
   cardSetVLayout->setSpacing(2);
   cardSetVLayout->addWidget(cardSetLabel);
   cardSetVLayout->addWidget(m_cardSetComboBox);
   QHBoxLayout *cardSetHLayout = new QHBoxLayout;
   cardSetHLayout->addLayout(cardSetVLayout);
   cardSetHLayout->addStretch(1);

   QDialogButtonBox *buttonBox = new QDialogButtonBox(
         QDialogButtonBox::Ok
       | QDialogButtonBox::Cancel
   );

   QVBoxLayout *vLayout = new QVBoxLayout;
   vLayout->addLayout(spacingHLayout);
   vLayout->addLayout(messageFontHLayout);
   vLayout->addLayout(cardSetsPathVLayout);
   vLayout->addLayout(cardSetHLayout);
   vLayout->setSpacing(12);
   vLayout->addSpacing(10);
   vLayout->addStretch(1);
   vLayout->addWidget(buttonBox);

   setLayout(vLayout);

   readSettings();


   connect(m_spacingBox,         SIGNAL(valueChanged(int)),
                           this, SLOT(onSpacingChanged(int)));
   connect(m_messageFontButton,  SIGNAL(clicked()),
                           this, SLOT(onChooseMessageFont()));
   connect(cardSetsPathBrowse,   SIGNAL(clicked()),
                           this, SLOT(onChooseCardSetPath()));
   connect(m_cardSetComboBox,    SIGNAL(currentIndexChanged(int)),
                           this, SLOT(onChooseCardSet(int)));
   connect(buttonBox,            SIGNAL(accepted()),
                           this, SLOT(accept()));
   connect(buttonBox,            SIGNAL(rejected()),
                           this, SLOT(reject()));
}


// Reads the settings and initialises the relevant widgets.
void Preferences::readSettings()
{
   QSettings settings;

   m_spacing = settings.value("spacing", 20).toInt();
   m_messageFont.fromString(settings.value("message font", QFont("Monospace", 10).toString()).toString());
   m_pathToCardSets = settings.value("path to card sets", "").toString();
   m_cardSet = settings.value("card set", "").toString();
   m_cardSetIsInternal = settings.value("card set is internal", true).toBool();

   m_spacingBox->setValue(m_spacing);
   m_messageFontButton->setText(QString("%1 %2").arg(m_messageFont.family()).arg(m_messageFont.pointSize()));
   m_cardSetsPathEdit->setText(m_pathToCardSets);


   // while (true) codeDuplication++;
   //
   addInternalCardSets(":/cards/");
   addExternalCardSets(m_pathToCardSets);


   // Fuck it. Let the code in initCards() worry about incomplete
   // card sets.
}


void Preferences::addInternalCardSets(QString path)
{
   if (!path.isEmpty())
   {
      m_prefixDir.setPath(path);
      m_prefixDir.setSorting(QDir::Name);

      m_cardSetList.append(m_prefixDir.entryList());
      m_cardSetComboBox->addItems(m_cardSetList);
      // Add empty string for potential separator.
      m_cardSetList.append("");
   }
   
   m_internalCardSets = m_cardSetList.count()-1;
}


void Preferences::addExternalCardSets(QString path)
{
   // Clear the external card sets already on the list.
   for (int i = m_cardSetList.count()-1; i >= m_internalCardSets; i--)
   {
      m_cardSetComboBox->removeItem(i);
      if (i == m_internalCardSets)
      {
         break;
      }
      else
      {
         m_cardSetList.removeAt(i);
      }
   }

   if (!path.isEmpty())
   {
      QDir externalSetsDir(path);
      externalSetsDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
      if (externalSetsDir.entryList().count())
      {
         m_cardSetComboBox->insertSeparator(m_cardSetComboBox->count());
         m_cardSetComboBox->addItems(externalSetsDir.entryList());
         m_cardSetList.append(externalSetsDir.entryList());
      }
   }

   if (m_cardSetIsInternal)
   {
      m_cardSetComboBox->setCurrentIndex(m_cardSetList.indexOf(m_cardSet));
   }
   else
   {
      // If the card set was external, we need to check if it exists in the
      // newly chosen folder.
      int index = m_cardSetList.indexOf(m_cardSet, m_internalCardSets);
      if (index == -1)
      {
         index = 0;
      }
      m_cardSetComboBox->setCurrentIndex(index);
   }
}


void Preferences::writeSettings()
{
   QSettings settings;

   settings.setValue("spacing", m_spacing);
   settings.setValue("message font", m_messageFont.toString());
   settings.setValue("path to card sets", m_pathToCardSets);
   settings.setValue("card set", m_cardSet);
   settings.setValue("card set is internal", m_cardSetIsInternal);
}


void Preferences::accept()
{
   writeSettings();
   QDialog::accept();
}


void Preferences::onSpacingChanged(int i)
{
   m_spacing = i;
}


void Preferences::onChooseMessageFont()
{
   // pop up a QFontDialog
   m_messageFont = QFontDialog::getFont(0, m_messageFont, this);
   m_messageFontButton->setText(QString("%1 %2").arg(m_messageFont.family()).arg(m_messageFont.pointSize()));
}


void Preferences::onChooseCardSetPath()
{
   m_pathToCardSets = QFileDialog::getExistingDirectory(this, "", m_pathToCardSets, QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
   m_cardSetsPathEdit->setText(m_pathToCardSets);

   addExternalCardSets(m_pathToCardSets);
}


void Preferences::onChooseCardSet(int index)
{
   m_cardSet = m_cardSetList[index];
   m_cardSetIsInternal = index < m_internalCardSets;
}
