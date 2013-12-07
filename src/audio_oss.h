#ifndef AUDIO_H
#define AUDIO_H

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
