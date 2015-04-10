//
//    guitune - program for tuning instruments (actually an oscilloscope)
//    Copyright (C) 1999  Florian Berger
//    Copyright (C) 2013  Yann Collette
//    Email: florian.berger@jk.uni-linz.ac.at
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License Version 2 as
//    published by the Free Software Foundation;
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <iostream>

#include <QtWidgets>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc,argv);
  QTranslator qtTranslator;
  qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app.installTranslator(&qtTranslator);

  QTranslator myappTranslator;
  if (myappTranslator.load("qtguitune_" + QLocale::system().name().toLower()))
    {
      app.installTranslator(&myappTranslator);
    }
  else
    {
      std::cerr << QT_TRANSLATE_NOOP("main","failed to load translation file ") << "qtguitune" << qPrintable(QLocale::system().name().toLower()) << "." << std::endl;
    }

  MainWindow* mainwin = new MainWindow(NULL, "QtGuiTune", argc, argv);
  
  mainwin->resize(500,280);
  mainwin->show();
  
  return (app.exec());
}
