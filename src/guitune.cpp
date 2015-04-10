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

#include <QtWidgets>

#include "osziview.h"
#include "logview.h"
#include "guitune.h"
#include "resources.h"
#include "audio_base.h"
#include "audio_oss.h"
#include "audio_alsa.h"

//globally
double KAMMERTON, KAMMERTON_LOG;

GuiTune::GuiTune(QWidget *parent, int argc, char **argv) : QWidget(parent), audio(NULL)
{
  int sampnr;
  double sampfreq;
  
  KAMMERTON     = KAMMERTON_NORM;
  KAMMERTON_LOG = KAMMERTON_LOG_NORM;
  
  sampnr = 1024;
  sampfreq = 11048;
  
  CurrentDriver = OSSDriver;
  audio = new AudioOSS(QString("/dev/dsp"));
  audio->init_audio();
  
  oszi = new OsziView(parent);

  audio->setSampleFreq(sampfreq);
  oszi->setSampleFreq(sampfreq);
  audio->setSampleNr(sampnr);
  oszi->setSampleNr(sampnr);
  
  logview = new LogView(parent);
  logview->setFrameStyle(QFrame::Box | QFrame::Sunken);
  logview->setLineWidth(2);
  
  freqview = new QLCDNumber(parent);
  freqview->setDigitCount(9);
  freqview->setFrameStyle(QFrame::NoFrame);
  freqview->setSegmentStyle(QLCDNumber::Filled);
  QLabel* freqviewLabel = new QLabel(tr("Freq.:"), parent);
  freqviewLabel->setBuddy(freqview);
  QHBoxLayout* freqviewLayout = new QHBoxLayout(this);
  QWidget* freqviewWidget = new QWidget(parent);
  freqviewLayout->addWidget(freqviewLabel);
  freqviewLayout->addWidget(freqview);
  freqviewWidget->setLayout(freqviewLayout);
  
  nfreqview = new QLCDNumber(parent);
  nfreqview->setDigitCount(9);
  nfreqview->setFrameStyle(QFrame::NoFrame);
  nfreqview->setSegmentStyle(QLCDNumber::Filled);
  QLabel* nfreqviewLabel = new QLabel(tr("Note:"), parent);
  nfreqviewLabel->setBuddy(nfreqview);
  QHBoxLayout* nfreqviewLayout = new QHBoxLayout(this);
  QWidget* nfreqviewWidget = new QWidget(parent);
  nfreqviewLayout->addWidget(nfreqviewLabel);
  nfreqviewLayout->addWidget(nfreqview);
  nfreqviewWidget->setLayout(nfreqviewLayout);
  
  sampfreq_input = new QSpinBox();
  sampfreq_input->setMinimum(1000);
  sampfreq_input->setMaximum(48000);
  sampfreq_input->setSingleStep(50);
  sampfreq_input->setValue(sampfreq);
  QLabel* sampfreq_inputLabel = new QLabel(tr("Sample Freq.:"), parent);
  sampfreq_inputLabel->setBuddy(sampfreq_input);
  QHBoxLayout* sampfreq_inputLayout = new QHBoxLayout(this);
  QWidget* sampfreq_inputWidget = new QWidget(parent);
  sampfreq_inputLayout->addWidget(sampfreq_inputLabel);
  sampfreq_inputLayout->addWidget(sampfreq_input);
  sampfreq_inputWidget->setLayout(sampfreq_inputLayout);
  
  connect(sampfreq_input, SIGNAL(valueChanged(int)), this, SLOT(setSampleFreq(int)));
  
  sampnr_input = new QSpinBox();
  sampnr_input->setMinimum(32);
  sampnr_input->setMaximum(11048);
  sampnr_input->setSingleStep(32);
  sampnr_input->setValue(sampnr);
  QLabel* sampnr_inputLabel = new QLabel(tr("Sample #:"), parent);
  sampnr_inputLabel->setBuddy(sampnr_input);
  QHBoxLayout* sampnr_inputLayout = new QHBoxLayout(this);
  QWidget* sampnr_inputWidget = new QWidget(parent);
  sampnr_inputLayout->addWidget(sampnr_inputLabel);
  sampnr_inputLayout->addWidget(sampnr_input);
  sampnr_inputWidget->setLayout(sampnr_inputLayout);
  
  connect(sampnr_input, SIGNAL(valueChanged(int)), this, SLOT(setSampleNr(int)));
  
  trigger_input = new QSpinBox();
  trigger_input->setMinimum(0);
  trigger_input->setMaximum(100);
  trigger_input->setSingleStep(10);
  trigger_input->setValue((int)(getTrigger()*100.0+0.5));
  QLabel* trigger_inputLabel = new QLabel(tr("Trig. (% of max):"), parent);
  trigger_inputLabel->setBuddy(trigger_input);
  QHBoxLayout* trigger_inputLayout = new QHBoxLayout(this);
  QWidget* trigger_inputWidget = new QWidget(parent);
  trigger_inputLayout->addWidget(trigger_inputLabel);
  trigger_inputLayout->addWidget(trigger_input);
  trigger_inputWidget->setLayout(trigger_inputLayout);
  
  connect(trigger_input, SIGNAL(valueChanged(int)), this, SLOT(setTriggerPercent(int)));
  
  QVBoxLayout* infoLayout = new QVBoxLayout(this);
  QWidget* infoWidget = new QWidget(parent);
  infoLayout->addWidget(freqviewWidget);
  infoLayout->addWidget(nfreqviewWidget);
  infoLayout->addWidget(sampfreq_inputWidget);
  infoLayout->addWidget(sampnr_inputWidget);
  infoLayout->addWidget(trigger_inputWidget);
  infoWidget->setLayout(infoLayout);

  QHBoxLayout* guiupperLayout = new QHBoxLayout(this);
  QWidget* guiupperWidget = new QWidget(parent);
  guiupperLayout->addWidget(oszi);
  guiupperLayout->addWidget(infoWidget);
  guiupperWidget->setLayout(guiupperLayout);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(guiupperWidget);
  mainLayout->addWidget(logview);
  
  // 'this' is the main widget
  this->setLayout(mainLayout);

  // Start the audio processing
  audio->start();
}

GuiTune::~GuiTune() 
{
  audio->quit();
}

void GuiTune::setTuningNorm()
{
  KAMMERTON     = KAMMERTON_NORM;
  KAMMERTON_LOG = KAMMERTON_LOG_NORM;
  update();
}

void GuiTune::setTuningWien()
{
  KAMMERTON     = KAMMERTON_WIEN;
  KAMMERTON_LOG = KAMMERTON_LOG_WIEN;
  update();
}

void GuiTune::setTuningPhys()
{
  KAMMERTON     = KAMMERTON_PHYS;
  KAMMERTON_LOG = KAMMERTON_LOG_PHYS;
  update();
}

void GuiTune::setTuningEqui()
{
  logview->nat_tuning_on = false;
  update();
}

void GuiTune::setTuningNat()
{
  logview->nat_tuning_on = true;
  update();
}

void GuiTune::setScaleUS()
{
  logview->setScale(LogView::us_scale);
}

void GuiTune::setScaleUSAlt()
{
  logview->setScale(LogView::us_scale_alt);
}

void GuiTune::setScaleGE()
{
  logview->setScale(LogView::german_scale);
}

void GuiTune::setScaleGEAlt()
{
  logview->setScale(LogView::german_scale_alt);
}

void GuiTune::showLogView()
{
  logview->show();
}

void GuiTune::hideLogView()
{
  logview->hide();
}

void GuiTune::showOszi()
{
  oszi->show();
}

void GuiTune::hideOszi()
{
  oszi->hide();
}

double GuiTune::getTrigger()
{
  return (oszi->getTrigFact());
}

void GuiTune::setSampleFreq(int f)
{
  audio->setSampleFreq(f);
  oszi->setSampleFreq(f);
  
  emit signalSampFreqChanged();
}


void GuiTune::setSampleNr(int nr)
{
  audio->setSampleNr(nr);
  oszi->setSampleNr(nr);
  
  emit signalSampNrChanged();
}


void GuiTune::setTrigger(double trig)
{
  oszi->setTrigFact(trig);
  
  emit signalTriggerChanged();
}


void GuiTune::setTriggerPercent(int trig)
{
  setTrigger((double)trig/100.0);
}

void GuiTune::setDSPName(QString name)
{
  audio->setDSPName(name);
}

OsziView* GuiTune::getOsziPtr()
{
  return oszi;
}

LogView* GuiTune::getLogviewPtr()
{
  return logview;
}

QLCDNumber* GuiTune::getFreqviewPtr()
{
  return freqview;
}

QLCDNumber* GuiTune::GetNFreqviewPtr()
{
  return nfreqview;
}

void GuiTune::setAudioDriver(DriverType _dt)
{
  audio->quit();
  delete audio;
  CurrentDriver = _dt;
  if (_dt == OSSDriver)  audio = new AudioOSS(QString("/dev/dsp"));
  if (_dt == ALSADriver) audio = new AudioALSA(QString("hw:0,0"));
  
  audio->init_audio();
  
  //audio->setSampleFreq(sampfreq);
  //oszi->setSampleFreq(sampfreq);
  //audio->setSampleNr(sampnr);
  //oszi->setSampleNr(sampnr);

  audio->start();
}
