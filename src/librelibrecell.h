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


#ifndef LIBRELIBRECELL_H
#define LIBRELIBRECELL_H

#include <QAction>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QSettings>
#include <QString>

#include "card.h"
#include "column.h"
#include "cornerwidget.h"


class LibreLibreCell : public QMainWindow
{
   Q_OBJECT

   public slots:
      void onGameNew();
      void onGameSelect();
      void onGameRestart();
      void onGamePreferences();
      void onGameUndo();
      void onHelpAbout();
      void onHelpAboutQt();

      void onCornerClicked(int corner);
      void onCornersCurrentChanged(int index, int number);

      void onColumnLeftClicked(int column, Qt::KeyboardModifiers modifs);
      void onColumnLeftDoubleClicked(int column);
      void onColumnMouseEntered(int column);
      void onColumnMouseLeft(int column);

      void onCardMouseRightDown(int type);
      void onCardMouseRightUp(int);

   public:
      LibreLibreCell();

   private:
      QSettings *m_settings;

      int m_spacing;
      QFont m_messageFont;
      QString m_pathToCardSets;
      QString m_cardSet;
      bool m_cardSetIsInternal;

      QString m_appName;
      int m_gameNumber;

      Card *m_cards[52];
      Column *m_columns[8];

      QList<int> m_randoms;

      CornerWidget *m_corners[8];
      QHBoxLayout *m_hLayoutBottom, *m_hLayoutTop;

      QPlainTextEdit *m_messages;

      QLabel *m_cardsLeftLabel;

      bool m_enableEvents;

      enum UndoMoveTypes
      {
         CornerToCorner = 0,
         CornerToColumn,
         ColumnToColumn,
         ColumnToCorner
      };

      // Holds one move that can be undone.
      struct UndoMove
      {
         // Corner to corner, corner to column,
         // column to column, column to corner.
         UndoMoveTypes type;
         // Number of cards moved (for column to column type).
         int numCards;
         // Where the cards came from.
         int source;
         // Where the cards went.
         int dest;
      };
      // Only one undo step, because that's what freecell does.
      // One undo step can hold any number of moves
      // (due to cards jumping on their own).
      QList<UndoMove> m_undoStep;

      QAction *m_gameUndo;


      void initRandoms(bool useSeed, int seed);
      void initCards();
      void deleteAllCards();

      int selectedColumn();
      int selectedCorner();

      QList<int> cardSequenceFromColumn(int col);
      bool cardGoesUnder(int topCard, int bottomCard);
      int moveCardSequence(QList<int> sequence, int source, int target);

      int canMoveNCards();
      int emptyColumns();
      int emptySpaces();

      void makeCardsJump();

      void checkGameOver();

      void clearUndo(bool disableMenu);
      void addUndoMove(UndoMoveTypes type, int numCards, int source, int dest, bool clearStepFirst);

      void keyPressEvent(QKeyEvent * event);
      void closeEvent(QCloseEvent * event);

      QString cardTypeToFilename(int type);
      bool isCardSetComplete(QString path);

      void  readSettings();
      void writeSettings();
};

#endif
