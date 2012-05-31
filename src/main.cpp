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


#include <QApplication>

#include "librelibrecell.h"


int main(int argv, char **args)
{
   QApplication app(argv, args);
   app.setOrganizationName("librelibrecell");
   app.setApplicationName("librelibrecell");

   LibreLibreCell cell;

   cell.show();

   return app.exec();
}
