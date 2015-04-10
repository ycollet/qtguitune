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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <iostream>

#include <QtWidgets>

#include "audio_alsa.h"
#include "resources.h"
#include "guitune.h"
#include "osziview.h"
#include "logview.h"

extern double KAMMERTON, KAMMERTON_LOG;

AudioALSA::AudioALSA(QString audio_name) : AudioBase(audio_name), audio(0)
{
  // KAMMERTON: the reference A note (440 Hz for standard tuning = KAMMERTON_NORM
  freqs[0]  = KAMMERTON; // The reference frequency
  lfreqs[0] = KAMMERTON_LOG; // The log of the reference frequency
  
  // Produce an octave
  for(int i=1; i<12; i++) {
    freqs [i] = freqs [i-1] * D_NOTE;
    lfreqs[i] = lfreqs[i-1] + D_NOTE_LOG;
  }
}

AudioALSA::~AudioALSA() 
{
  int ret = close_audio();
}

void AudioALSA::setDSPName(QString name)
{
  audio = close_audio();
  dsp_devicename = QString(name);
  audio = init_audio();
}

int AudioALSA::close_audio()
{
  snd_pcm_close(capture_handle);
  return 0;
}

int AudioALSA::init_audio()
{
  int err = 0, dir = 1;
  unsigned int tmp_sampfreq = sampfreq;
  std::cout << qPrintable(tr("initializing audio at ")) << qPrintable(dsp_devicename) << std::endl;

  if ((err = snd_pcm_open(&capture_handle, dsp_devicename.toStdString().c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
    std::cerr << "cannot open audio device " << qPrintable(dsp_devicename) << " (" << snd_strerror(err) << ")." << std::endl;
    exit (1);
  }
  
  if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
    std::cerr << "cannot allocate hardware parameter structure (" << snd_strerror(err) << ")." << std::endl;
    exit (1);
  }
				 
  if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0) {
    std::cerr << "cannot initialize hardware parameter structure (" << snd_strerror(err) << ")." << std::endl;
    exit (1);
  }
  
  if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
    std::cerr << "cannot set access type (" << snd_strerror(err) << ")." << std::endl;
    exit (1);
  }
  
  if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_U8)) < 0) {
    std::cerr << "cannot set sample format (" << snd_strerror(err) << ")." << std::endl;
    exit (1);
  }
  
  if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &tmp_sampfreq, &dir)) < 0) {
    std::cerr << "cannot set sample rate (" << snd_strerror(err) << ")." << std::endl;
    exit (1);
  }
  sampfreq = tmp_sampfreq;
  
  if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, 1)) < 0) {
    std::cerr << "cannot set channel count (" << snd_strerror(err) << ")." << std::endl;
    exit (1);
  }
  
  if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0) {
    std::cerr << "cannot set parameters (" << snd_strerror(err) << ")." << std::endl;
    exit (1);
  }
  
  snd_pcm_hw_params_free(hw_params);
  
  if ((err = snd_pcm_prepare(capture_handle)) < 0) {
    std::cerr << "cannot prepare audio interface for use (" << snd_strerror(err) << std::endl;
    exit (1);
  }

  blksize = 256;

  return 1;
}

void AudioALSA::proc_audio()
{
  int i,j,n,trig,trigpos;
  int err = 0;
  static int k = 0;
  unsigned char *c = NULL;
  double ldf,mldf;
 
  processing_audio = 1;
  trigpos = 0;
  c = sample;
  
  if ((err = snd_pcm_readi(capture_handle, c, blksize)) != blksize) {
    std::cerr << "read from audio interface failed (" << snd_strerror(err) << ")." << std::endl;
    exit (1);
  }
  n = blksize;
  
  // We search a 0 crossing value (beware: unsigned char !)
  for(i=0; i<n && Abs(c[i]-128)<2; i++);

  j = 0;
  trig = 0;
  
  if (i<n) do {
      for( ; i<n-1; i++)   /* n-1 because of POSTRIG uses i+1 */
	if (POSTRIG(c,i)) {
	  trig    = 1;
	  trigpos = i;
	}
      if (trig==0) {
	if ((err = snd_pcm_readi(capture_handle, c, blksize)) != blksize) {
	  std::cerr << "read from audio interface failed (" << snd_strerror(err) << ")." << std::endl;
	  exit (1);
	}
	n = blksize;
	j++;
	i=0;
      }
      // We loop over the samples until we have found a new 0 crossing value
    } while((!trig) && j<NO_TRIG_LIMIT);

  if (trig) {
    for(i=n-trigpos; i<sampnr; i+=n) {
      c += n;
      
      if ((err = snd_pcm_readi(capture_handle, c, blksize)) != blksize) {
	std::cerr << "read from audio interface failed (" << snd_strerror(err) << ")." << std::endl;
	exit (1);
      }
      n = blksize;
    }

    // Plot the measured values
    mw->getOsziPtr()->setSamplePtr(&sample[trigpos]);
    mw->getOsziPtr()->paintSample();
    
    freq_0t  = (double)sampfreq * mw->getOsziPtr()->getfreq2();
    lfreq_0t = log(freq_0t);
    
    while(lfreq_0t <  lfreqs[0]         - D_NOTE_LOG/2.0) lfreq_0t += LOG_2;
    while(lfreq_0t >= lfreqs[0] + LOG_2 - D_NOTE_LOG/2.0) lfreq_0t -= LOG_2;
    
    mldf    = D_NOTE_LOG; 
    note_0t = 0;
    
    for(i=0; i<12; i++) {
      ldf = fabs(lfreq_0t - lfreqs[i]);
      if (ldf < mldf) {
	mldf    = ldf; 
	note_0t = i;
      }
    }
    
    // Display the frequency
    update_lfreq();
    
    double nfreq_0t = freqs[note_0t];
    while(nfreq_0t/freq_0t > D_NOTE_SQRT) nfreq_0t /= 2.0;
    while(freq_0t/nfreq_0t > D_NOTE_SQRT) nfreq_0t *= 2.0;
    
    // Display the note
    update_nfreq(nfreq_0t);
  }
  
  k++;
  processing_audio = 0;
}

void AudioALSA::setSampleFreq(int f)
{
  sampfreq = f;
  audio = close_audio();
  audio = init_audio();
}
