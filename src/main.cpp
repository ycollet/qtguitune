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

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, QString name, int argc, char **argv) : QMainWindow(parent)
{
  mw = new MainWidget(this, "MainWidget", argc, argv);
  setCentralWidget(mw);
  mw->setMinimumSize(380,240);
  setWindowTitle(name);

  QAction* FileExitAction = new QAction("E&xit", parent);
  connect(FileExitAction, SIGNAL(triggered()), this, SLOT(close()));
  filemenu = menuBar()->addMenu("&File");
  filemenu->addAction(FileExitAction);
    
  optmenu    = menuBar()->addMenu("&Options");
  tuningmenu = optmenu->addMenu("&Tuning");
  scalemenu  = optmenu->addMenu("&Note Scale");

  QAction* TuningNormalAction      = new QAction("&Normal", parent);
  QAction* TuningWienAction        = new QAction("&Wien", parent);
  QAction* TuningPhysicalAction    = new QAction("&Physical", parent);
  QAction* TuningEquidistantAction = new QAction("&Equidistant", parent);
  QAction* TuningNaturalAction     = new QAction("N&atural", parent);
  TuningNormalAction->setCheckable(true);
  TuningWienAction->setCheckable(true);
  TuningPhysicalAction->setCheckable(true);
  TuningEquidistantAction->setCheckable(true);
  TuningNaturalAction->setCheckable(true);
  connect(TuningNormalAction,      SIGNAL(triggered()), this, SLOT(m_tuning_norm()));
  connect(TuningWienAction,        SIGNAL(triggered()), this, SLOT(m_tuning_wien()));
  connect(TuningPhysicalAction,    SIGNAL(triggered()), this, SLOT(m_tuning_phys()));
  connect(TuningEquidistantAction, SIGNAL(triggered()), this, SLOT(m_tuning_equi()));
  connect(TuningNaturalAction,     SIGNAL(triggered()), this, SLOT(m_tuning_nat()));
  TuningNormalAction->setChecked(true);
  TuningEquidistantAction->setChecked(true);
  
  tuningmenu->addAction(TuningNormalAction);
  tuningmenu->addAction(TuningWienAction);
  tuningmenu->addAction(TuningPhysicalAction);
  tuningmenu->addSeparator();
  tuningmenu->addAction(TuningEquidistantAction);
  tuningmenu->addAction(TuningNaturalAction);
  tuningmenu->menuAction()->setCheckable(true);

  QAction* ViewUSAction      = new QAction("&US", parent);
  QAction* ViewUSaAction     = new QAction("U&S-Alternate", parent);
  QAction* ViewGermanAction  = new QAction("&German", parent);
  QAction* ViewGermanaAction = new QAction("G&erman-Alt.", parent);
  ViewUSAction->setCheckable(true);
  ViewUSaAction->setCheckable(true);
  ViewGermanAction->setCheckable(true);
  ViewGermanaAction->setCheckable(true);
  connect(ViewUSAction,      SIGNAL(triggered()), this, SLOT(m_scale_US()));
  connect(ViewUSaAction,     SIGNAL(triggered()), this, SLOT(m_scale_USa()));
  connect(ViewGermanAction,  SIGNAL(triggered()), this, SLOT(m_scale_GE()));
  connect(ViewGermanaAction, SIGNAL(triggered()), this, SLOT(m_scale_GEa()));
  ViewUSAction->setChecked(true);
  
  scalemenu->addAction(ViewUSAction);
  scalemenu->addAction(ViewUSaAction);
  scalemenu->addAction(ViewGermanAction);
  scalemenu->addAction(ViewGermanaAction);
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent *)
{
  centralWidget()->resize(width(), height() - menuBar()->height());
}

void MainWindow::m_tuning_norm()
{
  mw->setTuningNorm();
  tuningmenu->menuAction()->setChecked(true);
  tuningmenu->menuAction()->setChecked(false);
  tuningmenu->menuAction()->setChecked(false);
}

void MainWindow::m_tuning_wien()
{
  mw->setTuningWien();
  tuningmenu->menuAction()->setChecked(false);
  tuningmenu->menuAction()->setChecked(true);
  tuningmenu->menuAction()->setChecked(false);
}

void MainWindow::m_tuning_phys()
{
  mw->setTuningPhys();
  tuningmenu->menuAction()->setChecked(false);
  tuningmenu->menuAction()->setChecked(false);
  tuningmenu->menuAction()->setChecked(true);
}

void MainWindow::m_tuning_equi()
{
  mw->setTuningEqui();
  tuningmenu->menuAction()->setChecked(true);
  tuningmenu->menuAction()->setChecked(false);
}

void MainWindow::m_tuning_nat()
{
  mw->setTuningNat();
  tuningmenu->menuAction()->setChecked(false);
  tuningmenu->menuAction()->setChecked(true);
}

void MainWindow::m_scale_US()
{
  mw->setScaleUS();
  scalemenu->menuAction()->setChecked(true);
  scalemenu->menuAction()->setChecked(false);
  scalemenu->menuAction()->setChecked(false);
  scalemenu->menuAction()->setChecked(false);
}

void MainWindow::m_scale_USa()
{
  mw->setScaleUSAlt();
  scalemenu->menuAction()->setChecked(false);
  scalemenu->menuAction()->setChecked(true);
  scalemenu->menuAction()->setChecked(false);
  scalemenu->menuAction()->setChecked(false);
}

void MainWindow::m_scale_GE()
{
  mw->setScaleGE();
  scalemenu->menuAction()->setChecked(false);
  scalemenu->menuAction()->setChecked(false);
  scalemenu->menuAction()->setChecked(true);
  scalemenu->menuAction()->setChecked(false);
}

void MainWindow::m_scale_GEa()
{
  mw->setScaleGEAlt();
  scalemenu->menuAction()->setChecked(false);
  scalemenu->menuAction()->setChecked(false);
  scalemenu->menuAction()->setChecked(false);
  scalemenu->menuAction()->setChecked(true);
}

int main(int argc, char *argv[])
{
  QApplication a(argc,argv);
  MainWindow mainwin(0, "MainWin", argc, argv);
  
  mainwin.resize(500,281);
  mainwin.show();
  mainwin.resize(500,280);
  
  return (a.exec());
}
