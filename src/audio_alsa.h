#ifndef AUDIO_ALSA_H
#define AUDIO_ALSA_H

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

#include <alsa/asoundlib.h>

#include "audio_base.h"

#define TIMER_TIME 1000
#define NO_TRIG_LIMIT 10

class AudioALSA : public AudioBase
{
 public:
  AudioALSA(QString);
  virtual ~AudioALSA();
  virtual int close_audio();
  virtual int init_audio();
  virtual void setDSPName(QString);
  virtual void setSampleFreq(int);
  virtual void proc_audio();
  
 private:
  snd_pcm_t *capture_handle;
  snd_pcm_hw_params_t *hw_params;
  int     audio;
  int     blksize;
  int     sampfreq;
  double  sampfreq_exact;
  double  freqs[12];
  double  lfreqs[12];
  int     processing_audio;
  int     trig1;
  int     trig2;
  int     note_0t;
  int     note_ht;
  double  freq_ht;
  double  lfreq_ht;
};
#endif
