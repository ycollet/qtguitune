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

#include <QtWidgets>

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "resources.h"
#include "osziview.h"

OsziView::OsziView(QWidget *parent) : QWidget(parent)
{
  QPalette Pal(palette());

  Pal.setColor(QPalette::Background, QColor(Qt::white));
  setAutoFillBackground(true);
  setPalette(Pal);
  
  trigfact = 0.6;
}

OsziView::~OsziView() {};

void OsziView::paintSample(void)
{
  int i,x1,x2,y1,y2;
  //erase();
  
  // Qt4 setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
  
  QPainter paint(this);
  
  paint.eraseRect(xscr, yscr, wscr + 1, hscr + 1);
  paint.setPen(Qt::gray);
  paint.drawLine(xscr, yscr + 128 * hscr / 256, xscr + wscr - 1, yscr + 128 * hscr / 256);
  paint.setPen(Qt::green);
  
  for(i=1; i<sampnr; i++) {
    x1 = xscr + (i - 1) * wscr / sampnr;
    x2 = xscr + i * wscr / sampnr;
    y1 = yscr + (samp[i-1]) * hscr / 256;
    y2 = yscr + (samp[i]) * hscr / 256;
    paint.drawLine(x1, y1, x2, y2);
  }

  paint.end();
}

double OsziView::getfreq(void)
{
  // in units of sampfreq
  int endpoint, tc, i;
  double freq;
  
  // Qt4 setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
  
  endpoint = 0;
  
  for(i=1, tc=0; i<sampnr; i++)
    if (POSTRIG(samp,i)) {
      endpoint = i;
      tc++;
    }
  
  freq = (double)tc / (double)endpoint;
  
  QPainter paint(this);
  
  paint.setPen(qRgb(200,0,0));
  paint.drawLine(xscr + endpoint * wscr / sampnr, yscr,
		 xscr + endpoint * wscr / sampnr, yscr + hscr);
  paint.end();
  
  return (freq);
}

double OsziView::getfreq2(void)
{
  // in units of sampfreq
  int endpoint, startpoint, t1, t2, A1, A2, tc, i, schmitt_triggered;
  double freq;
  
  // Qt4 setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
  
  for(i=0, A1=0; i<sampnr; i++)
    if (A1 < Abs(samp[i] - 128) && samp[i] - 128 > 0) A1 = Abs(samp[i] - 128);
  
  for(i=0, A2=0; i<sampnr; i++)
    if (A2 < Abs(samp[i] - 128) && samp[i] - 128 < 0) A2 = Abs(samp[i] - 128);

  t1 = 128 + (int)(A1 * trigfact + 0.5);
  t2 = 128 - (int)(A2 * trigfact + 0.5);
  
  startpoint = 0;
  for(i=1; !LEVTRIG(samp,i,t1) && i<sampnr; i++);
  
  startpoint = i;
  schmitt_triggered = NO;
  endpoint = startpoint + 1;
  tc = 0;
  for(i=startpoint, tc=0; i<sampnr; i++) {
    if (!schmitt_triggered)
      schmitt_triggered = (samp[i]<=t2);
    else if (LEVTRIG(samp,i,t1)) {
      endpoint = i;
      tc++;
      schmitt_triggered = NO;
    }
  }

  if (endpoint==startpoint) endpoint++;
  freq = (double)tc / (double)(endpoint - startpoint);
  
  if (freq<1e-15) freq = 1e-15;
  
  QPainter paint(this);
  
  paint.setPen(qRgb(200,0,0));
  paint.drawLine(xscr + endpoint * wscr / sampnr, yscr,
		 xscr + endpoint * wscr / sampnr, yscr + hscr);
  paint.drawLine(xscr + startpoint * wscr / sampnr, yscr,
		 xscr + startpoint * wscr / sampnr, yscr + hscr);
  
  paint.setPen(qRgb(0,170,170));
  paint.drawLine(xscr, yscr + t1 * hscr / 256, xscr + wscr, yscr + t1 * hscr / 256);
  paint.drawLine(xscr, yscr + t2 * hscr / 256, xscr + wscr, yscr + t2 * hscr / 256);
  
  paint.end();
  
  return (freq);
}

void OsziView::setSamplePtr(unsigned char *s)
{
  samp = s; 
}

void OsziView::setSampleFreq(double f)
{ 
  sampfreq = f;
  repaint();
}

void OsziView::setSampleNr(int nr)
{ 
  sampnr = nr; 
  repaint();
}

void OsziView::setTrigFact(double fact)
{
  trigfact = fact;
  repaint();
}

double OsziView::getTrigFact() 
{ 
  return (trigfact);
}

void OsziView::drawScale(void)
{
  double i, j;
  double di, dj;
  double dim; //mantissa of di
  char str[100];
  QPainter paint(this);
  
  // Qt4 setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
  
  //QRect r(rect()); //YC
  //paint.eraseRect(r); //YC
  
  di = (double)sampnr / sampfreq / 10.0;
  
  for(dim = di; dim >= 10.0; dim /= 10.0);
  for( ; dim < 1.0; dim *= 10.0);
  if (dim <= 5.0) { 
    di  = di / dim * 5.0;
    dim = 5.0;
  } else {
    di  = di / dim * 10.0;
    dim = 10.0;
  }
  
  dj = di / 10.0;
  
  paint.setPen(qRgb(100,100,100));
  for(j=0.0; j<(double)sampnr / sampfreq; j += dj) {
    paint.drawLine(xscr + (j * sampfreq) * wscr / sampnr, yscr + hscr,
		   xscr + (j * sampfreq) * wscr / sampnr, yscr + hscr + 3);
  }
  
  if (dim != 5.0) {
    dj = di/2.0;
    paint.setPen(qRgb(200,200,200));
    for(j=0.0; j<(double)sampnr / sampfreq; j += dj) {
      paint.drawLine(xscr + (j * sampfreq) * wscr / sampnr, yscr + hscr,
		     xscr + (j * sampfreq) * wscr / sampnr, yscr + hscr + 5);
    }
  }
  
  paint.setPen(qRgb(200,200,200));
  paint.setFont(QFont("System",8));
  for(i=0.0; i<(double)sampnr / sampfreq; i += di) {
    paint.drawLine(xscr + (i * sampfreq) * wscr / sampnr, yscr + hscr,
		   xscr + (i * sampfreq) * wscr / sampnr, yscr + hscr + 7);
    
    sprintf(str,"%.0f",i*1000); // in ms
    paint.drawText(xscr + (i * sampfreq) * wscr / sampnr - paint.fontMetrics().width(str) / 2,
		   yscr + hscr + 7 + paint.fontMetrics().ascent(),
		   str);
  }
  
  paint.end();
}

void OsziView::paintEvent(QPaintEvent *)
{
  //YC erase();
  drawScale();
  
  update();
}

void OsziView::resizeEvent(QResizeEvent *)
{
  xscr = 20;
  yscr = 5;
  wscr = width() - xscr * 2;
  hscr = height() - yscr - 20;
}
