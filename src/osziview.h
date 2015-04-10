#ifndef OSZIVIEW_H
#define OSZIVIEW_H

//
//    osziview.h
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

class OsziView : public QWidget
{
 public:
  OsziView(QWidget *parent);
  ~OsziView();

  void   paintSample(void);
  double getfreq(void);
  double getfreq2(void);
  void   setSamplePtr(unsigned char *s);
  void   setSampleFreq(double f);
  void   setSampleNr(int nr);
  void   setTrigFact(double fact);
  double getTrigFact();
  void   drawScale();

 protected:
  void paintEvent(QPaintEvent *);
  void resizeEvent(QResizeEvent *);

 private:
  unsigned char *samp;
  int    sampnr;
  double sampfreq;
  int    wscr, hscr, xscr, yscr;
  double trigfact;
};
#endif
