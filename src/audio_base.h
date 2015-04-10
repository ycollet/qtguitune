#ifndef AUDIO_BASE_H
#define AUDIO_BASE_H

//
//    guitune - program for tuning instruments (actually an oscilloscope)
//    Copyright (C) 1999  Florian Berger
//    Copyright (C) 2013-2015  Yann Collette
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

class GuiTune;

#define TIMER_TIME    1000
#define NO_TRIG_LIMIT 10

class AudioBase : public QThread
{
 public:
  AudioBase(QString);
  virtual ~AudioBase();
  virtual int init_audio() = 0;
  virtual int close_audio() = 0;
  virtual void setDSPName(QString) = 0;
  virtual void setSampleFreq(int) = 0;
  virtual void proc_audio() = 0;
  void setSampleNr(int);
  void setGuiPtr(GuiTune *);
  void update_lfreq();
  void update_nfreq(double);
  
 protected:
  virtual void run();
  
 public:
  GuiTune* mw;
  QString dsp_devicename;
  int     sampnr;
  int     sampfreq;
  unsigned char sample[64000];
  double  freq_0t;
  double  lfreq_0t;
};
#endif
