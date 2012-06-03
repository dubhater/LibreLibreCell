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


#include <algorithm>

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QDateTime>
//#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QObject>
#include <QPlainTextEdit>
#include <QPoint>
#include <QSettings>
#include <QSpinBox>
#include <QStatusBar>
#include <QString>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>

#include <QtGlobal>

#include "about.h"
#include "card.h"
#include "column.h"
#include "cornerwidget.h"
#include "librelibrecell.h"
#include "preferences.h"


const char * const cardSuits[] =
{
   "spades",
   "hearts",
   "clubs",
   "diamonds"
};
const char * const cardNumbers[] =
{
   "Ace", "2", "3", "4", "5",
   "6", "7", "8", "9","10",
   "Jack", "Queen", "King"
};


LibreLibreCell::LibreLibreCell()
{
   m_appName = "LibreLibreCell";

   m_settings = new QSettings();

   // Set up the menus.
   QAction *gameNew         = new QAction("&New game",       this);
   QAction *gameSelect      = new QAction("&Select game...", this);
   QAction *gameRestart     = new QAction("&Restart game",   this);
   QAction *gamePreferences = new QAction("&Preferences...", this);
            m_gameUndo      = new QAction("&Undo",           this);
   QAction *gameQuit        = new QAction("&Quit",           this);

   QAction *helpAbout       = new QAction("&About",          this);
   QAction *helpAboutQt     = new QAction("About &Qt",       this);

   gameNew->setShortcut(QKeySequence("Ctrl+N"));
   gameSelect->setShortcut(QKeySequence("Ctrl+Shift+N"));
   gameRestart->setShortcut(QKeySequence("Ctrl+R"));
   gamePreferences->setShortcut(QKeySequence("Ctrl+P"));
   m_gameUndo->setShortcut(QKeySequence("Ctrl+Z"));
   gameQuit->setShortcut(QKeySequence("Ctrl+Q"));

   helpAbout->setShortcut(QKeySequence("F1"));

   connect(gameNew, SIGNAL(triggered()),
              this, SLOT(onGameNew()));
   connect(gameSelect, SIGNAL(triggered()),
                 this, SLOT(onGameSelect()));
   connect(gameRestart, SIGNAL(triggered()),
                  this, SLOT(onGameRestart()));
   connect(gamePreferences, SIGNAL(triggered()),
                      this, SLOT(onGamePreferences()));
   connect(m_gameUndo, SIGNAL(triggered()),
                 this, SLOT(onGameUndo()));
   connect(gameQuit, SIGNAL(triggered()),
               this, SLOT(close()));

   connect(helpAbout, SIGNAL(triggered()),
                this, SLOT(onHelpAbout()));
   connect(helpAboutQt, SIGNAL(triggered()),
                  this, SLOT(onHelpAboutQt()));

   QMenu *gameMenu = menuBar()->addMenu("&Game");
   gameMenu->addAction(gameNew);
   gameMenu->addAction(gameSelect);
   gameMenu->addAction(gameRestart);
   gameMenu->addSeparator();
   gameMenu->addAction(gamePreferences);
   gameMenu->addSeparator();
   gameMenu->addAction(m_gameUndo);
   gameMenu->addSeparator();
   gameMenu->addAction(gameQuit);

   QMenu *helpMenu = menuBar()->addMenu("&Help");
   helpMenu->addAction(helpAbout);
   helpMenu->addAction(helpAboutQt);


   m_messages = new QPlainTextEdit;
   m_messages->setReadOnly(true);
   m_messages->setFixedHeight(100);
   m_messages->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

   resize(m_settings->value("size", QSize(width(), height())).toSize());
   move(m_settings->value("position", QPoint(x(), y())).toPoint());

   // Initialise the list of unique random numbers.
   initRandoms(false, 0);

   // Set up the layout.
   m_hLayoutBottom = new QHBoxLayout;
   m_hLayoutTop = new QHBoxLayout;
   m_hLayoutBottom->addStretch(1);
   initCards();
   m_hLayoutTop->insertStretch(4, 1);
   m_hLayoutTop->setSpacing(1);
   m_hLayoutBottom->addStretch(1);
   
   QVBoxLayout *vLayout = new QVBoxLayout;
   vLayout->addLayout(m_hLayoutTop, 0);
   vLayout->addLayout(m_hLayoutBottom, 1);
   vLayout->addWidget(m_messages, 0);

   QHBoxLayout *hLayout = new QHBoxLayout;
   hLayout->addStretch(1);
   hLayout->addLayout(vLayout);
   hLayout->addStretch(1);

   QWidget *centralWidget = new QWidget();
   centralWidget->setLayout(hLayout);

   setCentralWidget(centralWidget);

   m_cardsLeftLabel = new QLabel("52 cards left");

   statusBar()->addPermanentWidget(m_cardsLeftLabel);

   m_enableEvents = true;
}


// Initialise the list of random numbers
void LibreLibreCell::initRandoms(bool useSeed, int seed)
{
   int random;
   unsigned int s = (unsigned int)QDateTime::currentMSecsSinceEpoch();
   qsrand(s);
   s = useSeed ? seed : qrand() % 100000;
   srand(s+2);

   QString title;
   title.append(m_appName);
   title.append(" - ");
   title.append(QString::number(s));
   setWindowTitle(title);

   m_gameNumber = s;

   m_randoms.clear();
   while (m_randoms.size() < 52)
   {
      random = rand() % 52;
      if (!m_randoms.contains(random))
      {
         m_randoms.append(random);
      }
   }
}


// Create the cards and add them to the columns.
void LibreLibreCell::initCards()
{
   m_gameUndo->setEnabled(false);

   readSettings();

   // A new card set could be loaded without restarting the game,
   // but whatever.
   //
   // Also, I hate this.
   //
   // Also, fuck you, QString::append().

   bool foundCompleteCardSet = false;
   QString tmp;
   QString prefix(":/cards/");

   QFileInfo cardSetFolder(QString(m_pathToCardSets).append("/").append(m_cardSet));
   if (!m_pathToCardSets.isEmpty() && cardSetFolder.isDir() && !m_cardSetIsInternal)
   {
      // If the configured card set is found in the configured location,
      // use that.
      tmp = cardSetFolder.absoluteFilePath();
      foundCompleteCardSet = isCardSetComplete(tmp);
   }

   if (!foundCompleteCardSet && !m_cardSet.isEmpty() && QDir(prefix).exists(m_cardSet))
   {
      if (!m_pathToCardSets.isEmpty() && !m_cardSetIsInternal)
      {
         m_messages->appendPlainText(QString("Could not find card set \"%1\" in %2. Attempting to find the card set among those embedded in the executable.").arg(m_cardSet).arg(m_pathToCardSets));
         m_pathToCardSets = "";
         m_cardSetIsInternal = true;
      }
      // Otherwise, try to use the configured card set from the resources.
      tmp = prefix;
      tmp.append(m_cardSet).append("/");
      foundCompleteCardSet = isCardSetComplete(tmp);
   }

   if (!foundCompleteCardSet)
   {
      if (!m_cardSet.isEmpty())
      {
         m_messages->appendPlainText(QString("Could not find card set \"%1\" anywhere. Trying one of the card sets embedded in the executable.").arg(m_cardSet));
      }
      for (unsigned int i = 0; i < QDir(prefix).count(); i++)
      {
         if ((foundCompleteCardSet = isCardSetComplete(tmp = QDir(prefix).entryList()[i].append("/").prepend(prefix))))
         {
            m_cardSet = QDir(prefix).entryList()[i];
            break;
         }
      }

      if (!foundCompleteCardSet)
      {
         m_messages->appendPlainText("Could not find any complete card sets embedded in the executable. Something must have gone very wrong during compilation.");
         m_cardSet = "";
      }
   }

   writeSettings();

   QString msg;
   msg.append("Starting game number ");
   msg.append(QString::number(m_gameNumber));
   msg.append(".");
   m_messages->appendPlainText(msg);

   for (int i = 0; i < 52; i++)
   {
      m_cards[i] = new Card(m_randoms[i], tmp + cardTypeToFilename(m_randoms[i]).prepend("/"));
      connect(m_cards[i], SIGNAL(rightMouseDown(int)),
                  this, SLOT(onCardMouseRightDown(int)));
      connect(m_cards[i], SIGNAL(rightMouseUp(int)),
                  this, SLOT(onCardMouseRightUp(int)));
   }

   for (int i = 0; i < 8; i++)
   {
      m_columns[i] = new Column(i, m_spacing);
      connect(m_columns[i], SIGNAL(leftClicked(int, Qt::KeyboardModifiers)),
                    this, SLOT(onColumnLeftClicked(int, Qt::KeyboardModifiers)));
      connect(m_columns[i], SIGNAL(leftDoubleClicked(int)),
                    this, SLOT(onColumnLeftDoubleClicked(int)));
      connect(m_columns[i], SIGNAL(mouseEntered(int)),
                    this, SLOT(onColumnMouseEntered(int)));
      connect(m_columns[i], SIGNAL(mouseLeft(int)),
                    this, SLOT(onColumnMouseLeft(int)));
      m_columns[i]->setMinimumSize(m_cards[0]->pixmap()->size());
      m_hLayoutBottom->addWidget(m_columns[i], 0, Qt::AlignTop);
   }

   for (int i = 0; i < 52; i++)
   {
      m_columns[i % 8]->layout->addWidget2(m_cards[i],
                                         Qt::AlignHCenter | Qt::AlignTop);
   }

   for (int i = 0; i < 8; i++)
   {
         m_corners[i] = new CornerWidget(i);
         m_corners[i]->setFixedSize(m_cards[0]->pixmap()->size() + QSize(2,2));
         connect(m_corners[i], SIGNAL(clicked(int)),
                       this, SLOT(onCornerClicked(int)));
         // For "game over" detection.
         connect(m_corners[i], SIGNAL(currentChangedWithNumber(int, int)),
                       this, SLOT(onCornersCurrentChanged(int, int)));
         m_hLayoutTop->insertWidget(i<4 ? i : i+1, m_corners[i], 0, 0);
   }
}


// Check if a given card set is complete.
bool LibreLibreCell::isCardSetComplete(QString path)
{
   path.append("/");
   for (int i = 0; i < 52; i++)
   {
      if (!QFile(path + cardTypeToFilename(i)).exists())
      {
         return false;
      }
   }
   return true;
}


// Delete all cards
void LibreLibreCell::deleteAllCards()
{
   for (int i = 0; i < 8; i++)
   {
      delete m_columns[i];
      delete m_corners[i];
   }
}


// Start a new game.
void LibreLibreCell::onGameNew()
{
   if (m_enableEvents == false)
      return;
   // Reinitialise the list of random numbers.
   initRandoms(false, 0);
   // Call onGameRestart() because it does everything else we need.
   onGameRestart();
}


// Start the game the user wants.
void LibreLibreCell::onGameSelect()
{
   if (m_enableEvents == false)
      return;

   bool ok;
   int n = QInputDialog::getInt(this,
                                "Select game",
                                "Choose a number\nbetween 0 and 99999:",
                                m_gameNumber,
                                0,
                                99999,
                                1,
                                &ok);

   if (ok)
   {
      initRandoms(true, n);
      onGameRestart();
   }
}


// Restart the current game.
void LibreLibreCell::onGameRestart()
{
   if (m_enableEvents == false)
      return;
   // Remove all cards.
   deleteAllCards();
   // Recreate the cards and add them to the columns.
   // The existing list of randoms is reused.
   initCards();
}


// Display Preferences dialog.
void LibreLibreCell::onGamePreferences()
{
   if (m_enableEvents == false)
      return;
   // Display the preferences dialog.
   Preferences prefs(this);
   prefs.exec();
   readSettings();
}


// Unde the last move.
void LibreLibreCell::onGameUndo()
{
   int selColumn = selectedColumn();
   int selCorner = selectedCorner();

   if (selColumn > -1)
   {
      m_columns[selColumn]->setSelected(false);
   }

   if (selCorner > -1)
   {
      m_corners[selCorner]->setSelected(false);
   }

   // Replay the moves in reverse.
   for (int i = m_undoStep.count()-1; i >= 0; i--)
   {
      switch (m_undoStep[i].type)
      {
         case CornerToCorner: // Move was from corner to corner.
         {
            // Move one card from its dest corner back to its source.
            // FIXME: Seriously, implement CornerWidget::takeAt() already.
            // Or takeLast()...
            QWidget *w = m_corners[m_undoStep[i].dest]->widget(m_corners[m_undoStep[i].dest]->count()-1);
            m_corners[m_undoStep[i].dest]->removeWidget(w);
            m_corners[m_undoStep[i].source]->addWidget(w);
            m_corners[m_undoStep[i].source]->setCurrentWidget(w);
            break;
         }
         case CornerToColumn: // Move was from corner to column.
         {
            // Move one card from its dest column back to its source.
            // This card could only come from the left corner, so set
            // the current index to 0 after putting it back.
            m_corners[m_undoStep[i].source]->addWidget(m_columns[m_undoStep[i].dest]->layout->takeAt(m_columns[m_undoStep[i].dest]->layout->count()-1)->widget());
            m_corners[m_undoStep[i].source]->setCurrentIndex(0);
            break;
         }
         case ColumnToColumn: // Move was from column to column.
         {
            // Move m_undoStep[i].numCards cards from their dest column
            // back to their source column.
            int destCardCount = m_columns[m_undoStep[i].dest]->layout->count();
            for (int j = 0; j < m_undoStep[i].numCards; j++)
            {
               // Fugly.
               m_columns[m_undoStep[i].source]->layout->addWidget2(m_columns[m_undoStep[i].dest]->layout->takeAt(destCardCount-m_undoStep[i].numCards)->widget(), Qt::AlignHCenter | Qt::AlignTop);
            }
            break;
         }
         case ColumnToCorner: // Move was from column to corner.
         {
            // Move one card from its dest corner back to its source.
            QWidget *w = m_corners[m_undoStep[i].dest]->widget(m_corners[m_undoStep[i].dest]->count()-1);
            m_corners[m_undoStep[i].dest]->removeWidget(w);
            m_columns[m_undoStep[i].source]->layout->addWidget2(w, Qt::AlignHCenter | Qt::AlignTop);
            w->show();
            break;
         }
      }

      repaint(rect());
   }

   // We're done with this undo step.
   clearUndo(true);
}


// Display About box.
void LibreLibreCell::onHelpAbout()
{
   About about(this);
   about.exec();
}


// Display About Qt box.
void LibreLibreCell::onHelpAboutQt()
{
   QMessageBox::aboutQt(this, "About Qt");
}


// Do stuff if a corner was clicked.
void LibreLibreCell::onCornerClicked(int corner)
{
   if (m_enableEvents == false)
      return;

   int selColumn = selectedColumn();
   int selCorner = selectedCorner();
   if (corner < 4) // Left corner.
   {
      if (selCorner > -1) // There is a corner space already selected.
      {
         // Deselect it no matter which corner space we clicked.
         m_corners[selCorner]->setSelected(false);
         if (selCorner != corner)
         {
            if (!m_corners[corner]->count())
            {
               // Empty corner space was clicked.
               // Move the card from selCorner to corner.
               QWidget *w = m_corners[selCorner]->widget(0);
               m_corners[selCorner]->removeWidget(w);
               m_corners[corner]->addWidget(w);
               addUndoMove(CornerToCorner, 1, selCorner, corner, true);
            }
         }
      }
      else // There is no corner space selected yet.
      {
         if (selColumn > -1)
         {
            m_columns[selColumn]->setSelected(false);
            // A column was selected before clicking in the corner.
            // Maybe move a card from the column to the corner.
            if (!m_corners[corner]->count())
            {
               // Column is selected and this corner is empty.
               // Move the card.
               m_corners[corner]->addWidget(m_columns[selColumn]->layout->takeAt(m_columns[selColumn]->layout->count()-1)->widget());
               m_corners[corner]->setCurrentIndex(1);
               addUndoMove(ColumnToCorner, 1, selColumn, corner, true);
            }
            makeCardsJump();
         }
         else
         {
            if (m_corners[corner]->count())
            {
               m_corners[corner]->setSelected(true);
            }
         }
      }
   }
   else // Right corner.
   {
      if (selCorner > -1)
      {
         // FreeCell deselects the card whether the move was successful
         // or not.
         m_corners[selCorner]->setSelected(false);
         Card *cornerWidget = (Card*)m_corners[selCorner]->widget(0);
         if (cornerWidget->getType()/13 == corner-4
          && cornerWidget->getType()%13 == m_corners[corner]->count())
         {
            // The selected card can be moved here.
            m_corners[selCorner]->removeWidget(cornerWidget);
            m_corners[corner]->addWidget(cornerWidget);
            m_corners[corner]->setCurrentWidget(cornerWidget);
            addUndoMove(CornerToCorner, 1, selCorner, corner, true);
            makeCardsJump();
         }
         else
         {
            m_messages->appendPlainText("That move is not allowed.");
         }
      }
      else if (selColumn > -1)
      {
         m_columns[selColumn]->setSelected(false);
         Card *columnWidget = (Card*)m_columns[selColumn]->layout->itemAt(m_columns[selColumn]->layout->count()-1)->widget();
         if (columnWidget->getType()/13 == corner-4
          && columnWidget->getType()%13 == m_corners[corner]->count())
         {
            m_columns[selColumn]->layout->removeWidget(columnWidget);
            m_corners[corner]->addWidget(columnWidget);
            m_corners[corner]->setCurrentWidget(columnWidget);
            addUndoMove(ColumnToCorner, 1, selColumn, corner, true);
         }
         else
         {
            m_messages->appendPlainText("That move is not allowed.");
         }
         makeCardsJump();
      }
   }
}

// Do stuff if a column was left clicked.
void LibreLibreCell::onColumnLeftClicked(int column, Qt::KeyboardModifiers modifs)
{
   if (m_enableEvents == false)
      return;

   int selColumn = selectedColumn();
   int selCorner = selectedCorner();
   if (selColumn > -1) // A column is already selected.
   {
      // If a column is selected, no corner space can be selected.
      if (selCorner > -1)
      {
         QString msg;
         QTextStream stream(&msg);
         stream << "Insanity detected: both column " << selColumn
            << " and corner space " << selCorner << " are selected.";
         m_messages->appendPlainText(msg);
      }
      // Deselect the selected column whether the same was clicked or not,
      // before potentially moving any cards.
      m_columns[selColumn]->setSelected(false);
      if (column != selColumn)
      {
         // If the user clicked a different column, attempt to move cards
         // from the selected column to the clicked column.
         QList<int> sequence = cardSequenceFromColumn(selColumn);
         if (m_columns[column]->layout->count() == 0)
         {
            statusBar()->clearMessage();
            if (modifs == Qt::ControlModifier)
            {
               // If the target is an empty column and ctrl was pressed,
               // move only one card.
               sequence = sequence.mid(0, 1);
            }
         }
         int movedCards = moveCardSequence(sequence, selColumn, column);
         if (movedCards > 0)
         {
            addUndoMove(ColumnToColumn, movedCards, selColumn, column, true);
            checkGameOver();
         }
         else if (movedCards == 0)
         {
            m_messages->appendPlainText("That move is not allowed.");
         }
         else if (movedCards < 0)
         {
            QString msg;
            QTextStream stream(&msg);
            stream << "You attempted to move " << -movedCards
               << " cards. You only have enough space to move "
               << canMoveNCards() << " cards.";
            m_messages->appendPlainText(msg);
         }
      }
      makeCardsJump();
   }
   else // No column is selected yet.
   {
      if (selCorner > -1)
      {
         m_corners[selCorner]->setSelected(false);
         // If a corner space is selected, attempt to move the card
         // to this column.
         Card *cornerWidget = (Card*)m_corners[selCorner]->widget(0);
         // Wow, this is ugly.
         if (!m_columns[column]->layout->count()
          || cardGoesUnder(cornerWidget->getType(), ((Card*)m_columns[column]->layout->itemAt(m_columns[column]->layout->count()-1)->widget())->getType()))
         {
            m_corners[selCorner]->removeWidget(cornerWidget);
            m_columns[column]->layout->addWidget2(cornerWidget,
                                                Qt::AlignHCenter
                                              | Qt::AlignTop);
            cornerWidget->show();
            addUndoMove(CornerToColumn, 1, selCorner, column, true);
         }
         else
         {
            m_messages->appendPlainText("That move is not allowed.");
         }
      }
      else
      {
         // Select the clicked column if there are cards in it.
         if (m_columns[column]->layout->count())
         {
            m_columns[column]->setSelected(true);
         }
      }
   }
}


// Do stuff if a column was left double clicked.
void LibreLibreCell::onColumnLeftDoubleClicked(int column)
{
   if (m_enableEvents == false)
      return;

   // Right. So, a double click first sends a normal click event,
   // then the double click event. No second normal click event
   // is generated.
   
   // Do nothing at all if there are no cards in the column.
   if (m_columns[column]->layout->count())
   {
      // First, deselect the column. If it wasn't selected,
      // this does nothing.
      m_columns[column]->setSelected(false);
      
      // Try to send the card to the left corner.
      // Do nothing if there are no empty spaces.
      for (int i = 0; i < 4; i++)
      {
         if (m_corners[i]->count() == 0)
         {
            m_corners[i]->addWidget(m_columns[column]->layout->takeAt(m_columns[column]->layout->count()-1)->widget());
            m_corners[i]->setCurrentIndex(1);
            addUndoMove(ColumnToCorner, 1, column, i, true);
            makeCardsJump();
            break;
         }
      }
   }
}


// Return the number of the selected column, or -1.
int LibreLibreCell::selectedColumn()
{
   // Hurr hurr, we should detect if there are multiple columns selected
   // and display an error message or something.
   for (int i = 0; i < 8; i++)
   {
      if (m_columns[i]->isSelected())
      {
         return i;
      }
   }
   // If we got this far, no column is selected.
   return -1;
}


// Return the number of the selected corner space, or -1.
int LibreLibreCell::selectedCorner()
{
   for (int i = 0; i < 4; i++)
   {
      if (m_corners[i]->isSelected())
      {
         return i;
      }
   }
   // If we got this far, no corner space is selected.
   return -1;
}


// Find the longest sequence of cards that can be moved from this column.
// Return it as a list of card types, topmost card first.
QList<int> LibreLibreCell::cardSequenceFromColumn(int col)
{
   QList<int> sequence;
   int cardCount = m_columns[col]->layout->count();

   // Return an empty list if there are no cards in the column
   // or if the column number if out of range.
   if (col < 0 || col > 7 || !cardCount)
   {
      return sequence;
   }
   
   // Iterate over the cards in the column.
   for (int i = cardCount-1; i >= 0; i--)
   {
      int currentType = ((Card*)m_columns[col]->layout->itemAt(i)->widget())->getType();
      // Always return the topmost card.
      if (i == cardCount-1)
      {
         sequence.append(currentType);
      }
      else
      {
         if (cardGoesUnder(sequence.value(sequence.count()-1), currentType))
         {
            // If the current card can go under the last card in the
            // sequence, add it to the sequence.
            sequence.append(currentType);
         }
         else
         {
            // If the current card can't go under the last one in the
            // sequence, there is no reason to continue.
            break;
         }
      }
   }
   return sequence;
}


// Returns true if the second card type can go under the first one
// according to the rules of freecell.
bool LibreLibreCell::cardGoesUnder(int topCard, int bottomCard)
{
   // The card numbers (0-12):
   // 0 - "A", 1 - "2", ..., 9 - "10", 10 - "J", 11 - "Q", 12 - "K".
   // The card types (0-51):
   // 0-12:  spades   (black) (0)
   // 13-25: hearts   (red)   (1)
   // 26-38: clubs    (black) (2)
   // 39-51: diamonds (red)   (3)
   // (cardType/13)%2 gives the colour, cardType%13 gives the number
   // Any odd colour can go with any even colour.
   
   // Empty space can go under any card.
   if (bottomCard == -1)
      return true;

   int topColour, topNumber, bottomColour, bottomNumber;

      topColour =    (topCard/13)%2;
   bottomColour = (bottomCard/13)%2;
      topNumber =     topCard%13;
   bottomNumber =  bottomCard%13;

   return (bottomColour != topColour) && (bottomNumber == topNumber+1);
}


// Move what you can from a card sequence to the target column.
// Return the number of cards moved or 0 if the target can't take them
// (incompatible cards), or the negated number of cards that would have
// been moved, if there is not enough free space.
int LibreLibreCell::moveCardSequence(QList<int> sequence, int source, int target)
{
   int sourceCardCount = m_columns[source]->layout->count();
   int targetCardCount = m_columns[target]->layout->count();
   int targetType;
   int cardsToMove;

   if (targetCardCount == 0)
   {
      // The target is an empty space.
      targetType = -1;
   }
   else
   {
      targetType = ((Card*)m_columns[target]->layout->itemAt(targetCardCount-1)->widget())->getType();
   }

   if (targetType > -1) // Target is a card.
   {
      int targetNumber = targetType % 13;
      // SourceEnd = the bottommost card in the sequence.
      int sourceEndNumber = sequence.value(sequence.count()-1) % 13;
      int distance = targetNumber - sourceEndNumber;
      if (distance > 1)
      {
         // Can't put a 7 on top of a 9, for example.
         return 0;
      }

      int targetColour = (targetType / 13) % 2;
      int sourceEndColour = (sequence.value(sequence.count()-1) / 13) % 2;
      // No logical XOR in C++ :/
      if   ((  distance % 2  && targetColour == sourceEndColour)
         || (!(distance % 2) && targetColour != sourceEndColour))
      {
         // If the distance is even, the colours must be identical.
         // If the distance is odd, the colours must be different.
         return 0;
      }

      int sourceStartNumber = sequence.value(0) % 13;
      // SourceStart = the topmost card in the sequence.
      if (sourceStartNumber >= targetNumber)
      {
         // Can't put a 7 on top of a 7 (or anything lower).
         return 0;
      }

      // At this point, part of the sequence definitely can go on top of
      // target - if there is enough free space.
      //
      // From the first card, move targetNumber-sourceStartNumber cards.
      cardsToMove = targetNumber - sourceStartNumber;
      if (cardsToMove > canMoveNCards())
      {
         // Too many cards, not enough space.
         return -cardsToMove;
      }
   }
   else
   {
      // Target is an empty column. Move as many cards as we can.
      cardsToMove = std::min(sequence.count(), emptySpaces()+1);
   }

   // Finally move the damned cards.
   for (int i = cardsToMove-1; i >= 0; i--)
   {
      m_columns[target]->layout->addWidget2(m_columns[source]->layout->takeAt(sourceCardCount-cardsToMove)->widget(), Qt::AlignHCenter | Qt::AlignTop);
   }
   repaint(rect());

   return cardsToMove;
}


// Return the number of empty columns.
int LibreLibreCell::emptyColumns()
{
   int n = 0;

   for (int i = 0; i < 8; i++)
   {
      if (m_columns[i]->layout->count() == 0)
      {
         n++;
      }
   }
   return n;
}


// Return the number of empty spaces.
int LibreLibreCell::emptySpaces()
{
   int n = 0;

   for (int i = 0; i < 4; i++)
   {
      if (m_corners[i]->count() == 0)
      {
         n++;
      }
   }
   return n;
}


// Return the maximum number of cards that we can move.
int LibreLibreCell::canMoveNCards()
{
   // (spaces+1)*(columns+1) seems like a reasonable assumption.
   return (emptySpaces()+1)*(emptyColumns()+1);
}


// Move unneeded cards to the right corner.
void LibreLibreCell::makeCardsJump()
{
   if (selectedColumn() > -1 || selectedCorner() > -1)
   {
      m_messages->appendPlainText("Insanity detected: makeCardsJump() was called while a column or corner space was selected.");
      return;
   }

   // I suspect this isn't /exactly/ what freecell does,
   // but I find it good enough™.
   //
   // codeDuplication++;
   
   // Disable handling of mouse events.
   m_enableEvents = false;
   
   bool movedCards;
   do
   {
      movedCards = false;
      // First, the columns (for no specific reason).
      for (int i = 0; i < 8; i++)
      {
         if (!m_columns[i]->layout->count())
         {
            // Skip empty columns.
            continue;
         }

         Card *card = (Card*)m_columns[i]->layout->itemAt(m_columns[i]->layout->count()-1)->widget();
         int cardType = card->getType();
         bool cardCanMove = false;
         if (cardType%13 == 0)
         {
            // Aces are moved unconditionally.
            cardCanMove = true;
         }
         else
         {
            // Other cards.
            if (cardType%13 == m_corners[cardType/13+4]->count())
            {
               if ((cardType/13)%2)
               {
                  // FIXME: That -1 isn't quite right. It allowed the 3 of
                  // diamonds to jump when the 2 of spades was still in a
                  // column. I added the -1 because some cards that should
                  // have jumped weren't doing so and it seemed like a
                  // good solution at the time.
                  //    No, that was fine. The 2 of spades would have
                  //    jumped out as soon as it became free so the 3 of
                  //    diamonds really wasn't necessary.
                  //
                  // Odd number. Check 0 and 2.
                  if (m_corners[0+4]->count() >= m_corners[cardType/13+4]->count()-1
                   && m_corners[2+4]->count() >= m_corners[cardType/13+4]->count()-1)
                  {
                     // Card can move.
                     cardCanMove = true;
                  }
               }
               else
               {
                  // Even number. Check 1 and 3.
                  if (m_corners[1+4]->count() >= m_corners[cardType/13+4]->count()-1
                   && m_corners[3+4]->count() >= m_corners[cardType/13+4]->count()-1)
                  {
                     // Card can move.
                     cardCanMove = true;
                  }
               }
            }
         }
         if (cardCanMove)
         {
            m_columns[i]->layout->removeWidget(card);
            m_corners[cardType/13+4]->addWidget(card);
            m_corners[cardType/13+4]->setCurrentWidget(card);
            movedCards = true;
            addUndoMove(ColumnToCorner, 1, i, cardType/13+4, false);
         }
      }

      // And now, the left corner.
      for (int i = 0; i < 4; i++)
      {
         if (!m_corners[i]->count())
         {
            // Skip empty corner spaces.
            continue;
         }

         Card *card = (Card*)m_corners[i]->widget(0);
         int cardType = card->getType();
         bool cardCanMove = false;
         if (cardType%13 == 0)
         {
            // Aces are moved unconditionally.
            cardCanMove = true;
         }
         else
         {
            // Other cards.
            if (cardType%13 == m_corners[cardType/13+4]->count())
            {
               if ((cardType/13)%2)
               {
                  // Odd number. Check 0 and 2.
                  if (m_corners[0+4]->count() >= m_corners[cardType/13+4]->count()-1
                   && m_corners[2+4]->count() >= m_corners[cardType/13+4]->count()-1)
                  {
                     // Card can move.
                     cardCanMove = true;
                  }
               }
               else
               {
                  // Even number. Check 1 and 3.
                  if (m_corners[1+4]->count() >= m_corners[cardType/13+4]->count()-1
                   && m_corners[3+4]->count() >= m_corners[cardType/13+4]->count()-1)
                  {
                     // Card can move.
                     cardCanMove = true;
                  }
               }
            }
         }
         if (cardCanMove)
         {
            m_corners[i]->removeWidget(card);
            m_corners[cardType/13+4]->addWidget(card);
            m_corners[cardType/13+4]->setCurrentWidget(card);
            movedCards = true;
            addUndoMove(CornerToCorner, 1, i, cardType/13+4, false);
         }
      }
      //usleep(50000);
      repaint(rect());
   }
   while (movedCards);
   // Enable the mouse events again.
   m_enableEvents = true;
}


// Do stuff every time the current card in the corners changes.
void LibreLibreCell::onCornersCurrentChanged(int index, int number)
{
   // Calculate the number of cards left and check if we've won the game.
   if (number >= 4)
   {
      bool gameFinished = true;
      int cardsLeft = 52;
      for (int i = 0; i < 4; i++)
      {
         cardsLeft -= m_corners[i+4]->count();
         if (m_corners[i+4]->count() != 13)
         {
            gameFinished = false;
            //break;
         }
      }

      QString msg;
      QTextStream stream(&msg);
      stream << cardsLeft << " card" << (cardsLeft == 1 ? " " : "s ")
         << "left";
      m_cardsLeftLabel->setText(msg);

      if (gameFinished)
      {
         m_messages->appendPlainText("Congratulations! You have successfully completed the game.");
         clearUndo(true);
         return;
      }
   }

   // Only if the space is not empty.
   if (index > -1)
   {
      checkGameOver();
   }
}


// Check if there are any legal moves left.
void LibreLibreCell::checkGameOver()
{
   m_enableEvents = false;

   if (emptySpaces() == 0 && emptyColumns() == 0)
   {
      // Now we need to go through the corner spaces and see if any
      // cards can go to the right corner or to a column.
      // If they can't go anywhere, go though the columns and see if
      // they can go to another column or to the right corner.
      // If all fails, the game is over.

      // Check left corner first.
      for (int i = 0; i < 4; i++)
      {
         int cardType = ((Card*)m_corners[i]->widget(0))->getType();

         // To right corner.
         if (m_corners[cardType/13+4]->count() == cardType%13)
         {
            // This card can go to the right corner. Stop looking.
            m_enableEvents = true;
            return;
         }

         // To columns.
         for (int j = 0; j < 8; j++)
         {
            if (cardGoesUnder(cardType, ((Card*)m_columns[j]->layout->itemAt(m_columns[j]->layout->count()-1)->widget())->getType()))
            {
               // This card can go to column j. Stop looking.
               m_enableEvents = true;
               return;
            }
         }
      }

      // Check the columns.
      for (int i = 0; i < 8; i++)
      {
         int cardTypeI = ((Card*)m_columns[i]->layout->itemAt(m_columns[i]->layout->count()-1)->widget())->getType();
         
         // To right corner.
         if (m_corners[cardTypeI/13+4]->count() == cardTypeI%13)
         {
            // Card from column i can go to the right corner.
            m_enableEvents = true;
            return;
         }

         for (int j = i+1; j < 8; j++)
         {
            int cardTypeJ = ((Card*)m_columns[j]->layout->itemAt(m_columns[j]->layout->count()-1)->widget())->getType();
            if (cardGoesUnder(cardTypeI, cardTypeJ)
             || cardGoesUnder(cardTypeJ, cardTypeI))
            {
               // Card from column i can go to column j,
               // or the other way around. Stop looking.
               m_enableEvents = true;
               return;
            }
         }
      }

      // At this point, there are no legal moves left.
      m_messages->appendPlainText("No legal moves left. You lose.");
      clearUndo(true);
   }

   m_enableEvents = true;
}


void LibreLibreCell::onColumnMouseEntered(int column)
{
   if (m_columns[column]->layout->count() == 0 && selectedColumn() > -1)
   {
      statusBar()->showMessage("Press Ctrl and click to move a single card.");
   }
}


void LibreLibreCell::onColumnMouseLeft(int column)
{
   if (m_columns[column]->layout->count() == 0 && selectedColumn() > -1)
   {
      statusBar()->clearMessage();
   }
}


void LibreLibreCell::onCardMouseRightDown(int type)
{
   QString msg;
   QTextStream stream(&msg);
   stream << cardNumbers[type%13] << " of " << cardSuits[type/13];
   statusBar()->showMessage(msg);
}


void LibreLibreCell::onCardMouseRightUp(int)
{
   statusBar()->clearMessage();
}


void LibreLibreCell::clearUndo(bool disableMenu)
{
   if (disableMenu)
   {
      m_gameUndo->setEnabled(false);
   }

   m_undoStep.clear();
}


void LibreLibreCell::addUndoMove(UndoMoveTypes type, int numCards, int source, int dest, bool clearStepFirst)
{
   if (clearStepFirst)
   {
      clearUndo(false);
   }

   if (m_undoStep.isEmpty())
   {
      m_gameUndo->setEnabled(true);
   }

   UndoMove move;
   move.type = type;
   move.numCards = numCards;
   move.source = source;
   move.dest = dest;

   m_undoStep.append(move);
}


// Reimplement keyPressEvent to allow controlling the cards
// with the keyboard.
void LibreLibreCell::keyPressEvent(QKeyEvent * event)
{
                     int key    = event->key();
   Qt::KeyboardModifiers modifs = event->modifiers();
   if (key >= Qt::Key_0 && key <= Qt::Key_9)
   {
      int selColumn = selectedColumn();
      int selCorner = selectedCorner();
      switch (key)
      {
         case Qt::Key_1:
         case Qt::Key_2:
         case Qt::Key_3:
         case Qt::Key_4:
         case Qt::Key_5:
         case Qt::Key_6:
         case Qt::Key_7:
         case Qt::Key_8:
         {
            // Clicks on columns are simple enough.
            onColumnLeftClicked(key - Qt::Key_0 - 1, modifs);
            break;
         }
         case Qt::Key_0:
         {
            if (selColumn > -1)
            {
               // If there is a column selected,
               // "click" the next free corner space.
               for (int i = 0; i < 4; i++)
               {
                  if (m_corners[i]->count() == 0)
                  {
                     onCornerClicked(i);
                     break;
                  }
               }
            }
            else if (emptySpaces() < 4)
            {
               // Make a list of occupied spaces.
               QList<int> occupiedSpaces;
               for (int i = 0; i < 4; i++)
               {
                  if (m_corners[i]->count() > 0)
                  {
                     occupiedSpaces.append(i);
                  }
               }
               if (selCorner == occupiedSpaces.last())
               {
                  // If the last occupied space is selected,
                  // deselect it.
                  m_corners[selCorner]->setSelected(false);
               }
               else if (selCorner == -1)
               {
                  // If no space is selected, select the first
                  // occupied one (there is at least one occupied).
                  m_corners[occupiedSpaces[0]]->setSelected(true);
               }
               else
               {
                  // Deselect the current one and select the next one.
                  m_corners[occupiedSpaces[occupiedSpaces.indexOf(selCorner)]]->setSelected(false);
                  m_corners[occupiedSpaces[occupiedSpaces.indexOf(selCorner)+1]]->setSelected(true);
               }
            }
            break;
         }
         case Qt::Key_9:
         {
            // This should also be simple.
            int cardType = -1;
            if (selColumn > -1)
            {
               cardType = ((Card*)m_columns[selColumn]->layout->itemAt(m_columns[selColumn]->layout->count()-1)->widget())->getType();
            }
            else if (selCorner > -1)
            {
               cardType = ((Card*)m_corners[selCorner]->widget(0))->getType();
            }

            if (cardType > -1)
            {
               onCornerClicked(cardType/13+4);
            }
            break;
         }
      }
   }
   else
   {
      // The documentation recommends we call this if we're not handling
      // the keypress.
      QMainWindow::keyPressEvent(event);
   }
}


void LibreLibreCell::closeEvent(QCloseEvent * event)
{
   writeSettings();
   event->accept();
}


QString LibreLibreCell::cardTypeToFilename(int type)
{
   // type is assumed to be between 0 and 51.
   
   QString name;
   int number = type % 13;

   QTextStream(&name) << cardSuits[type/13] << "-" << (number < 10 ? "0" : "") << number << ".png";

   return name;
}


void LibreLibreCell::readSettings()
{
   m_spacing = m_settings->value("spacing", 20).toInt();
   m_messageFont.fromString(m_settings->value("message font", QFont("Monospace", 10).toString()).toString());
   m_pathToCardSets = m_settings->value("path to card sets", "").toString();
   m_cardSet = m_settings->value("card set", "").toString();
   m_cardSetIsInternal = m_settings->value("card set is internal", true).toBool();

   m_messages->setFont(m_messageFont);
}


void LibreLibreCell::writeSettings()
{
   m_settings->setValue("size", size());
   m_settings->setValue("position", pos());
   m_settings->setValue("spacing", m_spacing);
   m_settings->setValue("message font", m_messageFont.toString());
   m_settings->setValue("path to card sets", m_pathToCardSets);
   m_settings->setValue("card set", m_cardSet);
   m_settings->setValue("card set is internal", m_cardSetIsInternal);
}
