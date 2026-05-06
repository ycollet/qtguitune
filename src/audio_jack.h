#ifndef AUDIO_JACK_H
#define AUDIO_JACK_H

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

#ifdef ENABLE_JACK

#include <jack/jack.h>
#include <jack/ringbuffer.h>

#include "audio_base.h"

// ~1.5 seconds of float samples at 88200 Hz
#define JACK_RINGBUF_SIZE 131072

class AudioJACK : public AudioBase
{
  Q_OBJECT
 public:
  AudioJACK(QString client_name);
  virtual ~AudioJACK();
  virtual int  init_audio();
  virtual int  close_audio();
  virtual void setDSPName(QString);
  virtual void setSampleFreq(int);
  virtual void proc_audio();

 private:
  jack_client_t     *client;
  jack_port_t       *input_port;
  jack_ringbuffer_t *ringbuffer;
  int                blksize;
  double             freqs[12];
  double             lfreqs[12];
  int                processing_audio;
  int                note_0t;

  static int jack_process_cb(jack_nframes_t nframes, void *arg);
  int        process(jack_nframes_t nframes);
};

#endif // ENABLE_JACK
#endif // AUDIO_JACK_H
