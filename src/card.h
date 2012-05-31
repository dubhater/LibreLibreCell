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


#ifndef CARD_H
#define CARD_H

#include <QLabel>
#include <QObject>
#include <QString>
#include <QMouseEvent>


class Card : public QLabel
{
   Q_OBJECT

   public:

      // Constructor.
      Card(int type, QString filename);
      ~Card();

      // Get/set the type of card we're displaying.
      int getType();
      void setType(int newType);

      bool isInverted();

      // Invert the image.
      void setInverted(bool inv);

   signals:
      void rightMouseDown(int type);
      void rightMouseUp(int type);

   private:

      // Type of card.
      int m_cardType;

      bool m_inverted;

      // Returns the file name where the card is stored.
      QString cardTypeToFilename(int type);

      // Needs to send a clicked() signal
      void mousePressEvent(QMouseEvent * event);
      void mouseReleaseEvent(QMouseEvent * event);
};

#endif
