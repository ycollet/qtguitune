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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>

#include <iostream>

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <QtGui>

#include "osziview.h"
#include "logview.h"
#include "guitune.h"
#include "resources.h"

//globally
double KAMMERTON, KAMMERTON_LOG;

int close_unistd(int fd)
{
  return (close(fd));
}
// unistd - close is not visible to widget because of own close function

void MainWidget::setTuningNorm()
{
  KAMMERTON     = KAMMERTON_NORM;
  KAMMERTON_LOG = KAMMERTON_LOG_NORM;
  update();
}

void MainWidget::setTuningWien()
{
  KAMMERTON     = KAMMERTON_WIEN;
  KAMMERTON_LOG = KAMMERTON_LOG_WIEN;
  update();
}

void MainWidget::setTuningPhys()
{
  KAMMERTON     = KAMMERTON_PHYS;
  KAMMERTON_LOG = KAMMERTON_LOG_PHYS;
  update();
}

void MainWidget::setTuningEqui()
{
  logview->nat_tuning_on = false;
  update();
}

void MainWidget::setTuningNat()
{
  logview->nat_tuning_on = true;
  update();
}

void MainWidget::setScaleUS()
{
  logview->setScale(LogView::us_scale);
}

void MainWidget::setScaleUSAlt()
{
  logview->setScale(LogView::us_scale_alt);
}

void MainWidget::setScaleGE()
{
  logview->setScale(LogView::german_scale);
}

void MainWidget::setScaleGEAlt()
{
  logview->setScale(LogView::german_scale_alt);
}

void MainWidget::showLogView()
{
  logview->show();
}

void MainWidget::hideLogView()
{
  logview->hide();
}

void MainWidget::showOszi()
{
  oszi->show();
}

void MainWidget::hideOszi()
{
  oszi->hide();
}

double MainWidget::getTrigger()
{
  return (oszi->getTrigFact());
}

void MainWidget::setSampFreq(int f)
{
  sampfreq = f;
  timer->stop();
  close_unistd(audio);
  audio = init_audio();
  timer->start(0);
  oszi->setSampleFreq(sampfreq_exact);
  
  emit signalSampFreqChanged();
}


void MainWidget::setSampNr(int nr)
{
  sampnr = nr;
  oszi->setSampleNr(nr);
  
  emit signalSampNrChanged();
}


void MainWidget::setTrigger(double trig)
{
  oszi->setTrigFact(trig);
  
  emit signalTriggerChanged();
}


void MainWidget::setTriggerPercent(int trig)
{
  setTrigger((double)trig/100.0);
}

void MainWidget::setDSPName(const char *name)
{
  timer->stop();
  close_unistd(audio);
  strcpy(dsp_devicename,name);
  audio = init_audio();
  timer->start(0);
}

int MainWidget::init_audio()
{
  std::cout << qPrintable(tr("initializing audio at ")) << dsp_devicename << std::endl;

  audio = open(dsp_devicename, O_RDONLY);
  if (audio == -1) {
    perror(dsp_devicename);
    exit(1);
  }
  fcntl(audio,F_SETFD,FD_CLOEXEC);

  if (strcmp(dsp_devicename,"/dev/stdin")==0) {
    std::cout << qPrintable(tr("reading data from stdin")) << std::endl;
    blksize = 32;
    std::cout << "  blocksize = " << blksize  << std::endl;
    std::cout << "  sampfreq  = " << sampfreq << std::endl;
    sampfreq_exact=sampfreq;

  } else {
    ioctl(audio, SNDCTL_DSP_SETDUPLEX, 0);
    {
      int caps;
      ioctl(audio, SNDCTL_DSP_GETCAPS, &caps);
      std::cout << "OSS-Version "  << int(caps & DSP_CAP_REVISION) << std::endl;
      std::cout << "  DUPLEX   = " << int(caps & DSP_CAP_DUPLEX)   << std::endl;
      std::cout << "  REALTIME = " << int(caps & DSP_CAP_REALTIME) << std::endl;
      std::cout << "  BATCH    = " << int(caps & DSP_CAP_BATCH)    << std::endl;
      std::cout << "  COPROC   = " << int(caps & DSP_CAP_COPROC)   << std::endl;
      std::cout << "  TRIGGER  = " << int(caps & DSP_CAP_TRIGGER)  << std::endl;
      std::cout << "  MMAP     = " << int(caps & DSP_CAP_MMAP)     << std::endl;
    }

    blksize = 8; // 2^8 = 256
    ioctl(audio, SNDCTL_DSP_SETFRAGMENT, &blksize);

    ioctl(audio, SNDCTL_DSP_GETBLKSIZE, &blksize);
    std::cout << "blocksize = " << blksize << std::endl;

    ioctl(audio, SNDCTL_DSP_SYNC, NULL);
    int a_sampsize = AFMT_U8;
    ioctl(audio, SNDCTL_DSP_SAMPLESIZE, &a_sampsize);
    int a_stereo = 0;
    ioctl(audio, SNDCTL_DSP_STEREO, &a_stereo);

    int a_speed = sampfreq;
    std::cout << "sampfreq = " << sampfreq << std::endl;
    ioctl(audio, SNDCTL_DSP_SPEED, &a_speed);
    ioctl(audio, SOUND_PCM_READ_RATE, &sampfreq);
    std::cout << "sampfreq = " << sampfreq << std::endl;
    sampfreq_exact = sampfreq;
  }

  return(audio);
}

MainWidget::MainWidget(QWidget *parent, int argc, char **argv) : QWidget(parent)
{
  KAMMERTON     = KAMMERTON_NORM;
  KAMMERTON_LOG = KAMMERTON_LOG_NORM;

  strcpy(dsp_devicename,"/dev/dsp");

  sampnr = 1024;
  sampfreq = 11048;
  processing_audio = 0;
  audio = init_audio();
  
  std::cout << qPrintable(tr("Audiodriver initialized")) << std::endl;

  freqs[0]  = KAMMERTON;
  lfreqs[0] = KAMMERTON_LOG;
  
  for(int i=1;i<12;i++) {
    freqs [i] = freqs [i-1] * D_NOTE;
    lfreqs[i] = lfreqs[i-1] + D_NOTE_LOG;
  }
  
  oszi = new OsziView(this);
  oszi->setSampleNr(sampnr);
  oszi->setSampleFreq(sampfreq_exact);
  
  logview = new LogView(this);
  logview->setFrameStyle(QFrame::Box | QFrame::Sunken);
  logview->setLineWidth(2);
  
  freqview = new QLCDNumber(parent);
  freqview->setNumDigits(9);
  freqview->setFrameStyle(QFrame::NoFrame);
  freqview->setSegmentStyle(QLCDNumber::Filled);
  
  nfreqview = new QLCDNumber(parent);
  nfreqview->setNumDigits(9);
  nfreqview->setFrameStyle(QFrame::NoFrame);
  nfreqview->setSegmentStyle(QLCDNumber::Filled);
  
  sampfreq_input = new QSpinBox();
  sampfreq_input->setMinimum(1000);
  sampfreq_input->setMaximum(48000);
  sampfreq_input->setSingleStep(50);
  sampfreq_input->setValue(sampfreq);
  
  connect(sampfreq_input, SIGNAL(valueChanged(int)), this, SLOT(setSampFreq(int)));
  
  sampnr_input = new QSpinBox();
  sampnr_input->setMinimum(32);
  sampnr_input->setMaximum(11048);
  sampnr_input->setSingleStep(32);
  sampnr_input->setValue(sampnr);
  
  connect(sampnr_input, SIGNAL(valueChanged(int)), this, SLOT(setSampNr(int)));
  
  trigger_input = new QSpinBox();
  trigger_input->setMinimum(0);
  trigger_input->setMaximum(100);
  trigger_input->setSingleStep(10);
  trigger_input->setValue((int)(getTrigger()*100.0+0.5));
  
  connect(trigger_input, SIGNAL(valueChanged(int)), this, SLOT(setTriggerPercent(int)));
  
  timer = new QTimer(this);
  
  connect(timer, SIGNAL(timeout()), this, SLOT(proc_audio()));
  
  timer->start(0);
}

void MainWidget::proc_audio()
{
  int i,j,n,trig,trigpos;
  static int k = 0;
  unsigned char *c = NULL;
  double ldf,mldf;
  char str[50];
  
  processing_audio = 1;
  trigpos = 0;
  c = sample;
  n = read(audio, c, blksize);
  
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
    } while((!trig) && j<100);

  if (trig) {
    for(i=n-trigpos; i<sampnr; i+=n) {
      c += n;
      n  = read(audio, c, blksize);
    }

    oszi->setSamplePtr(&sample[trigpos]);
    oszi->paintSample();
    
    freq_0t  = (double)sampfreq*oszi->getfreq2();
    lfreq_0t = log(freq_0t);
    
    while(lfreq_0t <  lfreqs[0] - D_NOTE_LOG/2.0) lfreq_0t += LOG_2;
    while(lfreq_0t >= lfreqs[0] + LOG_2 - D_NOTE_LOG/2.0) lfreq_0t -= LOG_2;
    mldf    = D_NOTE_LOG; 
    note_0t = 0;
    for(i=0; i<12; i++) {
      ldf = fabs(lfreq_0t-lfreqs[i]);
      if (ldf<mldf) {
	mldf    = ldf; 
	note_0t = i;
      }
    }
    
    logview->change_lfreq(lfreq_0t);
    sprintf(str,"%.3f",freq_0t);
    freqview->display(str);
    
    double nfreq_0t = freqs[note_0t];
    while(nfreq_0t/freq_0t > D_NOTE_SQRT) nfreq_0t /= 2.0;
    while(freq_0t/nfreq_0t > D_NOTE_SQRT) nfreq_0t *= 2.0;
    
    sprintf(str,"%.3f",nfreq_0t);
    nfreqview->display(str);
  }
  k++;
  processing_audio = 0;
}

void MainWidget::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  
  p.setFont(QFont("System",8));
  
  p.setPen(Qt::black);
  p.drawText(freqview->x() - p.fontMetrics().width(tr("Freq.:")) - 5 + 1,
	     freqview->y() + p.fontMetrics().ascent() + 1 + p.fontMetrics().height(),
	     tr("Freq.:"));

  p.setPen(Qt::white);
  p.drawText(freqview->x() - p.fontMetrics().width(tr("Freq.:")) - 5,
	     freqview->y() + p.fontMetrics().ascent() + p.fontMetrics().height(),
	     tr("Freq.:"));

  p.setPen(Qt::black);
  p.drawText(nfreqview->x() - p.fontMetrics().width(tr("Note:")) - 5 + 1,
	     nfreqview->y() + p.fontMetrics().ascent() + 1 + p.fontMetrics().height(),
	     tr("Note:"));
  
  p.setPen(Qt::white);
  p.drawText(nfreqview->x() - p.fontMetrics().width(tr("Note:")) - 5,
	     nfreqview->y() + p.fontMetrics().ascent() + p.fontMetrics().height(),
	     tr("Note:"));

  p.setPen(Qt::black);
  p.drawText(sampfreq_input->x() - p.fontMetrics().width(tr("Sample Freq.:")) - 5 + 1,
	     sampfreq_input->y() + p.fontMetrics().ascent() + 1 + p.fontMetrics().height() - 3,
	     tr("Sample Freq.:"));
  
  p.setPen(Qt::white);
  p.drawText(sampfreq_input->x() - p.fontMetrics().width(tr("Sample Freq.:")) - 5,
  	     sampfreq_input->y() + p.fontMetrics().ascent() + p.fontMetrics().height() - 3,
	     tr("Sample Freq.:"));

  p.setPen(Qt::black);
  p.drawText(sampnr_input->x() - p.fontMetrics().width(tr("Sample #:")) - 5 + 1,
	     sampnr_input->y() + p.fontMetrics().ascent() + 1 + p.fontMetrics().height() - 3,
	     tr("Sample #:"));
  
  p.setPen(Qt::white);
  p.drawText(sampnr_input->x() - p.fontMetrics().width(tr("Sample #:")) - 5,
	     sampnr_input->y() + p.fontMetrics().ascent() + p.fontMetrics().height() - 3,
	     tr("Sample #:"));

  p.setPen(Qt::black);
  p.drawText(trigger_input->x() - p.fontMetrics().width(tr("Trig.(% of max):")) - 5 + 1,
	     trigger_input->y() + p.fontMetrics().ascent() + 1 + p.fontMetrics().height() - 3,
	     tr("Trig.(% of max):"));
  
  p.setPen(Qt::white);
  p.drawText(trigger_input->x() - p.fontMetrics().width(tr("Trig.(% of max):")) - 5,
	     trigger_input->y() + p.fontMetrics().ascent() + p.fontMetrics().height() - 3,
	     tr("Trig.(% of max):"));
  
  p.end();
  update();  
}

void MainWidget::resizeEvent(QResizeEvent *)
{
  int fwidth = 150;

  oszi->setGeometry(0, 0, width() - fwidth, height() - 100);
  logview->setGeometry(0, (oszi->isVisible())?oszi->height():0, width(), 100);
  freqview->setGeometry(width() - fwidth + 40, 3, 100, 25);
  nfreqview->setGeometry(width() - fwidth + 40, 35, 100, 25);
  sampfreq_input->setGeometry(width() - 80, 65, 70, 20);
  sampnr_input->setGeometry(sampfreq_input->x(),
			    sampfreq_input->y() + sampfreq_input->height() + 5,
			    70,20);
  trigger_input->setGeometry(sampnr_input->x() + 20,
			     sampnr_input->y() + sampnr_input->height() + 5,
			     50,20);
}

MainWidget::~MainWidget()
{
  close_unistd(audio);
}
