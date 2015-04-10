#ifndef GUITUNE_H
#define GUITUNE_H

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

#include <QtWidgets>

class OsziView;
class LogView;
class AudioBase;

class GuiTune : public QWidget
{
 Q_OBJECT

 public:
  enum DriverType {OSSDriver, ALSADriver};
  
  GuiTune(QWidget *parent, int argc, char **argv);
  ~GuiTune();
  void   setTuningNorm();
  void   setTuningWien();
  void   setTuningPhys();
  void   setTuningEqui();
  void   setTuningNat();
  void   setScaleUS();
  void   setScaleUSAlt();
  void   setScaleGE();
  void   setScaleGEAlt();
  void   setAudioDriver(DriverType);
  void   showOszi();
  void   hideOszi();
  void   showLogView();
  void   hideLogView();
  double getTrigger();
  OsziView   *getOsziPtr();
  LogView    *getLogviewPtr();
  QLCDNumber *getFreqviewPtr();
  QLCDNumber *GetNFreqviewPtr();
  DriverType CurrentDriver;

 public slots:
  void setSampleFreq(int f);
  void setSampleNr(int nr);
  void setTrigger(double trig);
  void setTriggerPercent(int trig);
  void setDSPName(QString name);

 signals:
  void signalSampFreqChanged();
  void signalSampNrChanged();
  void signalTriggerChanged();

 private:
  OsziView   *oszi;
  LogView    *logview;
  QLCDNumber *freqview;
  QLCDNumber *nfreqview;
  QMenuBar   *menu;
  QMenu      *filemenu;
  QMenu      *optmenu;
  QMenu      *viewmenu;
  QSpinBox   *sampfreq_input;
  QSpinBox   *sampnr_input;
  QSpinBox   *trigger_input;
  AudioBase  *audio;
};
#endif
