#ifndef LOGVIEW_H
#define LOGVIEW_H

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

extern double KAMMERTON, KAMMERTON_LOG;

class LogView : public QFrame
{
 public:
  LogView(QWidget *parent);
  ~LogView();

  enum {us_scale, us_scale_alt, german_scale, german_scale_alt};

  bool nat_tuning_on;
  int  lfreq_pos(double lfreq);
  int  pos_note(int pos);
  void drawarrow(QColor col);
  void deletearrow(QColor col);
  void change_lfreq(double freq);
  void setScale(int scale);

 protected:
  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *);

 private:
  int    x0, y0, h;
  int    grundton;     // for natural tuning
  double lfreq0,lfreq1;
  double lfreq;
  double freqs[12];
  double lfreqs[12];
  QColor arrow_bgcol;
};
#endif
