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

#include <QtGui>

#include "audio_oss.h"
#include "resources.h"
#include "guitune.h"
#include "osziview.h"
#include "logview.h"

AudioBase::AudioBase(QString audio_name) : mw(NULL), dsp_devicename(audio_name)
{
}

AudioBase::~AudioBase() 
{
}

void AudioBase::setGuiPtr(GuiTune* _mw)
{
  mw = _mw;
}

void AudioBase::run()
{
  proc_audio();
}

void AudioBase::update_lfreq()
{
  char str[50];
  
  mw->getLogviewPtr()->change_lfreq(lfreq_0t);
  sprintf(str,"%.3f",freq_0t);
  mw->getFreqviewPtr()->display(str);
}

void AudioBase::update_nfreq(double _nfreq_0t)
{
  char str[50];
  
  sprintf(str, "%.3f", _nfreq_0t);
  mw->GetNFreqviewPtr()->display(str);
}

void AudioBase::setSampleNr(int nr)
{
  sampnr = nr;
}
