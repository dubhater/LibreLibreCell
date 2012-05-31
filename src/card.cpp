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


#include <QImage>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QString>
#include <QTextStream>

#include "card.h"


Card::Card(int type, QString filename) : QLabel()
{
   setType(type);

   setPixmap(QPixmap(filename));
   
   m_inverted = false;
}


Card::~Card()
{
   //printf("Deleting card...\n");
}


int Card::getType()
{
   return m_cardType;
}


void Card::setType(int newType)
{
   m_cardType = newType;

   // Load the pixmap that corresponds to cardType.
   //setPixmap(QPixmap(cardTypeToFilename(cardType)));
}


void Card::setInverted(bool inv)
{
   if (m_inverted != inv)
   {
      m_inverted = inv;

      QImage img = pixmap()->toImage();
      img.invertPixels();
      setPixmap(QPixmap::fromImage(img));
   }
}


void Card::mousePressEvent(QMouseEvent * event)
{
   if (event->button() == Qt::RightButton)
   {
      emit rightMouseDown(m_cardType);
      event->accept();
   }
   else
   {
      event->ignore();
   }
}


void Card::mouseReleaseEvent(QMouseEvent * event)
{
   if (event->button() == Qt::RightButton)
   {
      emit rightMouseUp(m_cardType);
      event->accept();
   }
   else
   {
      event->ignore();
   }
}
