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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <iostream>

#include <QtWidgets>

#include "audio_oss.h"
#include "resources.h"
#include "guitune.h"
#include "osziview.h"
#include "logview.h"

extern double KAMMERTON, KAMMERTON_LOG;

AudioOSS::AudioOSS(QString audio_name) : AudioBase(audio_name), audio(0)
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

AudioOSS::~AudioOSS() 
{
  int ret = close_audio();
}

void AudioOSS::setDSPName(QString name)
{
  audio = close_audio();
  dsp_devicename = QString(name);
  audio = init_audio();
}

int AudioOSS::close_audio()
{
  return close(audio);
}

int AudioOSS::init_audio()
{
  std::cout << qPrintable(tr("initializing audio at ")) << qPrintable(dsp_devicename) << std::endl;

  audio = open(dsp_devicename.toStdString().c_str(), O_RDONLY);
  if (audio == -1) {
    perror(dsp_devicename.toStdString().c_str());
    exit(1);
  }
  fcntl(audio,F_SETFD,FD_CLOEXEC); // Set the 'audio' file descriptor (F_SETFD) to FD_CLOEXEC: if the FD_CLOEXEC flag in the third argument is 0,
                                   // the file shall remain open across the exec functions; otherwise, the file shall be closed upon successful
                                   // execution of one of the exec functions.

  if (dsp_devicename ==  QString("/dev/stdin")) {
    std::cout << qPrintable(tr("reading data from stdin")) << std::endl;
    blksize = 32;
    std::cout << "  blocksize = " << blksize  << std::endl;
    std::cout << "  sampfreq  = " << sampfreq << std::endl;
    sampfreq_exact = sampfreq;
  } else {
    // Get the capabilities of the audio stream
    int caps;
    ioctl(audio, SNDCTL_DSP_GETCAPS, &caps);
    std::cout << "OSS-Version "  << int(caps & DSP_CAP_REVISION) << std::endl;
    std::cout << "  DUPLEX   = " << int(caps & DSP_CAP_DUPLEX)   << std::endl;
    std::cout << "  REALTIME = " << int(caps & DSP_CAP_REALTIME) << std::endl;
    std::cout << "  BATCH    = " << int(caps & DSP_CAP_BATCH)    << std::endl;
    std::cout << "  COPROC   = " << int(caps & DSP_CAP_COPROC)   << std::endl;
    std::cout << "  TRIGGER  = " << int(caps & DSP_CAP_TRIGGER)  << std::endl;
    std::cout << "  MMAP     = " << int(caps & DSP_CAP_MMAP)     << std::endl;

    blksize = 8; // 2^8 = 256
    // Automatically set the buffer allocation
    ioctl(audio, SNDCTL_DSP_SETFRAGMENT, &blksize);

    // Get the block size after buffer allocation
    ioctl(audio, SNDCTL_DSP_GETBLKSIZE, &blksize);
    std::cout << "blocksize = " << blksize << std::endl;

    // Suspend the application until all samples have been played
    ioctl(audio, SNDCTL_DSP_SYNC, NULL);
    // Set the sample format to unsigned 8 bits
    int a_sampsize = AFMT_U8;
    ioctl(audio, SNDCTL_DSP_SETFMT, &a_sampsize);
    // Set mono mode
    int a_stereo = 0;
    ioctl(audio, SNDCTL_DSP_STEREO, &a_stereo);

    int a_speed = sampfreq;
    std::cout << "sampfreq = " << sampfreq << std::endl;
    // Set the sampling rate
    ioctl(audio, SNDCTL_DSP_SPEED, &a_speed);
    ioctl(audio, SOUND_PCM_READ_RATE, &sampfreq); // Obsolete !
    std::cout << "sampfreq = " << sampfreq << std::endl;
    sampfreq_exact = sampfreq;
  }

  return(audio);
}

void AudioOSS::proc_audio()
{
  int i,j,n,trig,trigpos;
  static int k = 0;
  unsigned char *c = NULL;
  double ldf,mldf;
 
  processing_audio = 1;
  trigpos = 0;
  c = sample;
  n = read(audio, c, blksize);
  
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
	n = read(audio, c, blksize);
	j++;
	i=0;
      }
      // We loop over the samples until we have found a new 0 crossing value
    } while((!trig) && j<NO_TRIG_LIMIT);

  if (trig) {
    for(i=n-trigpos; i<sampnr; i+=n) {
      c += n;
      n  = read(audio, c, blksize);
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

void AudioOSS::setSampleFreq(int f)
{
  sampfreq = f;
  audio = close_audio();
  audio = init_audio();
}

