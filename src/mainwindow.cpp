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

#include <QtGui>

#include "mainwindow.h"
#include "guitune.h"

MainWindow::MainWindow(QWidget *parent, QString name, int argc, char **argv) : QMainWindow(parent)
{
  mw = new GuiTune(this, argc, argv);
  mw->setMinimumSize(380,240);
  
  setCentralWidget(mw);
  setWindowTitle(name);

  FileExitAction = new QAction(tr("E&xit"), parent);
  connect(FileExitAction, SIGNAL(triggered()), this, SLOT(close()));
  filemenu = menuBar()->addMenu(tr("&File"));
  filemenu->addAction(FileExitAction);
    
  optmenu    = menuBar()->addMenu(tr("&Options"));
  tuningmenu = optmenu->addMenu(tr("&Tuning"));
  scalemenu  = optmenu->addMenu(tr("&Note Scale"));

  TuningNormalAction      = new QAction(tr("&Normal"), parent);
  TuningWienAction        = new QAction(tr("&Wien"), parent);
  TuningPhysicalAction    = new QAction(tr("&Physical"), parent);
  TuningEquidistantAction = new QAction(tr("&Equidistant"), parent);
  TuningNaturalAction     = new QAction(tr("N&atural"), parent);
  
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
  
  tuningmenu->addAction(TuningNormalAction);
  tuningmenu->addAction(TuningWienAction);
  tuningmenu->addAction(TuningPhysicalAction);
  tuningmenu->addSeparator();
  tuningmenu->addAction(TuningEquidistantAction);
  tuningmenu->addAction(TuningNaturalAction);

  ViewUSAction      = new QAction(tr("&US"), parent);
  ViewUSaAction     = new QAction(tr("U&S-Alternate"), parent);
  ViewGermanAction  = new QAction(tr("&German"), parent);
  ViewGermanaAction = new QAction(tr("G&erman-Alt."), parent);
  
  ViewUSAction->setCheckable(true);
  ViewUSaAction->setCheckable(true);
  ViewGermanAction->setCheckable(true);
  ViewGermanaAction->setCheckable(true);
  
  connect(ViewUSAction,      SIGNAL(triggered()), this, SLOT(m_scale_US()));
  connect(ViewUSaAction,     SIGNAL(triggered()), this, SLOT(m_scale_USa()));
  connect(ViewGermanAction,  SIGNAL(triggered()), this, SLOT(m_scale_GE()));
  connect(ViewGermanaAction, SIGNAL(triggered()), this, SLOT(m_scale_GEa()));
  
  scalemenu->addAction(ViewUSAction);
  scalemenu->addAction(ViewUSaAction);
  scalemenu->addAction(ViewGermanAction);
  scalemenu->addAction(ViewGermanaAction);
  
  drivermenu = menuBar()->addMenu(tr("&Drivers"));
  OSSDriverAction  = new QAction(tr("&OSS"), parent);
  ALSADriverAction = new QAction(tr("&ALSA"), parent);
  
  OSSDriverAction->setCheckable(true);
  ALSADriverAction->setCheckable(true);
  
  connect(OSSDriverAction,  SIGNAL(triggered()), this, SLOT(m_driver_OSS()));
  connect(ALSADriverAction, SIGNAL(triggered()), this, SLOT(m_driver_ALSA()));
  
  drivermenu->addAction(OSSDriverAction);
  drivermenu->addAction(ALSADriverAction);

  LoadSettings();
  
  this->adjustSize();
  this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  this->setFocusPolicy(Qt::ClickFocus);
  this->show();
}

MainWindow::~MainWindow()
{
}

void MainWindow::m_tuning_norm()
{
  mw->setTuningNorm();
  TuningNormalAction->setChecked(true);
  TuningWienAction->setChecked(false);
  TuningPhysicalAction->setChecked(false);
}

void MainWindow::m_tuning_wien()
{
  mw->setTuningWien();
  TuningNormalAction->setChecked(false);
  TuningWienAction->setChecked(true);
  TuningPhysicalAction->setChecked(false);
}

void MainWindow::m_tuning_phys()
{
  mw->setTuningPhys();
  TuningNormalAction->setChecked(false);
  TuningWienAction->setChecked(false);
  TuningPhysicalAction->setChecked(true);
}

void MainWindow::m_tuning_equi()
{
  mw->setTuningEqui();
  TuningEquidistantAction->setChecked(true);
  TuningNaturalAction->setChecked(false);
}

void MainWindow::m_tuning_nat()
{
  mw->setTuningNat();
  TuningEquidistantAction->setChecked(false);
  TuningNaturalAction->setChecked(true);
}

void MainWindow::m_scale_US()
{
  mw->setScaleUS();
  ViewUSAction->setChecked(true);
  ViewUSaAction->setChecked(false);
  ViewGermanAction->setChecked(false);
  ViewGermanaAction->setChecked(false);
}

void MainWindow::m_scale_USa()
{
  mw->setScaleUSAlt();
  ViewUSAction->setChecked(false);
  ViewUSaAction->setChecked(true);
  ViewGermanAction->setChecked(false);
  ViewGermanaAction->setChecked(false);
}

void MainWindow::m_scale_GE()
{
  mw->setScaleGE();
  ViewUSAction->setChecked(false);
  ViewUSaAction->setChecked(false);
  ViewGermanAction->setChecked(true);
  ViewGermanaAction->setChecked(false);
}

void MainWindow::m_scale_GEa()
{
  mw->setScaleGEAlt();
  ViewUSAction->setChecked(false);
  ViewUSaAction->setChecked(false);
  ViewGermanAction->setChecked(false);
  ViewGermanaAction->setChecked(true);
}

void MainWindow::m_driver_OSS()
{
  mw->setAudioDriver(GuiTune::OSSDriver);
  OSSDriverAction->setChecked(true);
  ALSADriverAction->setChecked(false);
}

void MainWindow::m_driver_ALSA()
{
  mw->setAudioDriver(GuiTune::ALSADriver);
  OSSDriverAction->setChecked(false);
  ALSADriverAction->setChecked(true);
}

void MainWindow::LoadSettings()
{
  QSettings settings("AudioApp", "QtGuiTune");
  
  if (settings.value("ViewUSAction",      true).toBool())  m_scale_US();
  if (settings.value("ViewUSaAction",     false).toBool()) m_scale_USa();
  if (settings.value("ViewGermanAction",  false).toBool()) m_scale_GE();
  if (settings.value("ViewGermanaAction", false).toBool()) m_scale_GEa();
  
  if (settings.value("TuningEquidistantAction", true).toBool())  m_tuning_equi();
  if (settings.value("TuningNaturalAction",     false).toBool()) m_tuning_nat();
  
  if (settings.value("TuningNormalAction",   true).toBool())  m_tuning_norm();
  if (settings.value("TuningWienAction",     false).toBool()) m_tuning_wien();
  if (settings.value("TuningPhysicalAction", false).toBool()) m_tuning_phys();
  
  if (GuiTune::OSSDriver == (GuiTune::DriverType)settings.value("AudioDriver", GuiTune::OSSDriver).toInt()) m_driver_OSS();
  if (GuiTune::ALSADriver == (GuiTune::DriverType)settings.value("AudioDriver", GuiTune::OSSDriver).toInt()) m_driver_ALSA();
}

void MainWindow::SaveSettings()
{
  QSettings settings("AudioApp", "QtGuiTune");
  
  settings.setValue("ViewUSAction",      ViewUSAction->isChecked());
  settings.setValue("ViewUSaAction",     ViewUSaAction->isChecked());
  settings.setValue("ViewGermanAction",  ViewGermanAction->isChecked());
  settings.setValue("ViewGermanaAction", ViewGermanaAction->isChecked());
  
  settings.setValue("TuningEquidistantAction", TuningEquidistantAction->isChecked());
  settings.setValue("TuningNaturalAction",     TuningNaturalAction->isChecked());
  
  settings.setValue("TuningNormalAction",   TuningNormalAction->isChecked());
  settings.setValue("TuningWienAction",     TuningWienAction->isChecked());
  settings.setValue("TuningPhysicalAction", TuningPhysicalAction->isChecked());
  
  settings.setValue("AudioDriver", mw->CurrentDriver);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  SaveSettings();
  event->accept();
}
