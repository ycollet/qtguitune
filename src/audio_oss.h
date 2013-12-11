#ifndef AUDIO_H
#define AUDIO_H

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

#include <QtGui>

class GuiTune;

#define TIMER_TIME 1000
#define NO_TRIG_LIMIT 10

class AudioOSS : public QThread
{
 public:
  AudioOSS();
  ~AudioOSS();
  int init_audio();
  void setDSPName(QString);
  void setSampleFreq(int);
  void setSampleNr(int);
  void proc_audio();
  void setGuiPtr(GuiTune *);
  void update_lfreq();
  void update_nfreq(double);
  
 protected:
  virtual void run();
  
 private:
  GuiTune* mw;
  QString dsp_devicename;
  int     audio;
  int     blksize;
  int     sampnr;
  int     sampfreq;
  double  sampfreq_exact;
  unsigned char sample[64000];
  double  freqs[12];
  double  lfreqs[12];
  int     processing_audio;
  int     trig1;
  int     trig2;
  int     note_0t;
  int     note_ht;
  double  freq_0t;
  double  freq_ht;
  double  lfreq_0t;
  double  lfreq_ht;
};
#endif
