#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//
//    guitune - program for tuning instruments (actually an oscilloscope)
//    Copyright (C) 1999  Florian Berger
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

#include <QtGui>

class GuiTune;

class MainWindow : public QMainWindow
{
  Q_OBJECT
 public:
  MainWindow(QWidget *parent, QString name, int argc, char **argv);
  ~MainWindow();
  
 private:
  GuiTune* mw;
  QMenu* filemenu;
  QMenu* optmenu;
  QMenu* tuningmenu;
  QMenu* scalemenu;
  
  QAction* FileExitAction;
  QAction* TuningNormalAction;
  QAction* TuningWienAction;
  QAction* TuningPhysicalAction;
  QAction* TuningEquidistantAction;
  QAction* TuningNaturalAction;
  QAction* ViewUSAction;
  QAction* ViewUSaAction;
  QAction* ViewGermanAction;
  QAction* ViewGermanaAction;
    
 private slots:
  void m_tuning_norm();
  void m_tuning_wien();
  void m_tuning_phys();
  void m_tuning_equi();
  void m_tuning_nat();
  void m_scale_US();
  void m_scale_USa();
  void m_scale_GE();
  void m_scale_GEa();
};
#endif
