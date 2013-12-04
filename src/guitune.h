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

#include <QtGui>

class OsziView;
class LogView;

class MainWidget : public QWidget
{
 Q_OBJECT
 public:
  MainWidget(QWidget *parent, const char *name , int argc, char **argv);
  ~MainWidget();
  int    init_audio();
  int    sampnr;
  int    sampfreq;
  double sampfreq_exact;
  int    blksize;
  int    audio;
  int    trig1;
  int    trig2;
  int    note_0t;
  int    note_ht;
  double freq_0t;
  double freq_ht;
  double lfreq_0t;
  double lfreq_ht;
  void   setTuningNorm();
  void   setTuningWien();
  void   setTuningPhys();
  void   setTuningEqui();
  void   setTuningNat();
  void   setScaleUS();
  void   setScaleUSAlt();
  void   setScaleGE();
  void   setScaleGEAlt();
  void   showOszi();
  void   hideOszi();
  void   showLogView();
  void   hideLogView();
  double getTrigger();
  
 public slots:
  void setSampFreq(int f);
  void setSampNr(int nr);
  void setTrigger(double trig);
  void setTriggerPercent(int trig);
  void setDSPName(const char* name);

 signals:
  void signalSampFreqChanged();
  void signalSampNrChanged();
  void signalTriggerChanged();

 protected:
  void paintEvent(QPaintEvent *);
  void resizeEvent(QResizeEvent *);

 private:
  OsziView   *oszi;
  LogView    *logview;
  QLCDNumber *freqview;
  QLCDNumber *nfreqview;
  QTimer     *timer;
  QMenuBar   *menu;
  QMenu      *filemenu;
  QMenu      *optmenu;
  QMenu      *viewmenu;
  QSpinBox   *sampfreq_input;
  QSpinBox   *sampnr_input;
  QSpinBox   *trigger_input;
  //   FreqView *freqview;
  int    oszi_height;
  unsigned char sample[64000];
  double freqs[12];
  double lfreqs[12];
  int    processing_audio;
  char   dsp_devicename[100];

 private slots:
  void proc_audio();
};
