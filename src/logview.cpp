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

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "logview.h"
#include "resources.h"

QString note_ger[12]     = {"A","B", "H","C","Db","D","Eb","E","F","Gb","G","Ab"};
QString note_us[12]      = {"A","Bb","B","C","Db","D","Eb","E","F","Gb","G","Ab"};
QString note_ger_alt[12] = {"A","B", "H","C","C#","D","D#","E","F","F#","G","G#"};
QString note_us_alt[12]  = {"A","A#","B","C","C#","D","D#","E","F","F#","G","G#"};

QString *note = NULL;

char * tuning_nat[23] = {"25/24","16/15","9/8",  "7/6",  "75/64","6/5",  "5/4","32/25","125/96",
			 "4/3",  "25/18","45/32","36/25","3/2",  "25/16","8/5","5/3",  "125/72",
			 "16/9",  "9/5", "15/8", "48/25","125/64"};

LogView::LogView(QWidget *parent, QString name, Qt::WindowFlags f, bool allowLines) : QFrame(parent,f)
										      //QFrame(parent,name,f,allowLines)
{
  int r, g, b, gray;
  QPalette Pal(palette());

  setWindowTitle(name);
  
  grundton = 0;
  nat_tuning_on = false;

  note = note_us;

  lfreq = 1.0;
  x0 = 10;
  y0 = 26;
  h  = 10;
  r = Pal.color(QPalette::Background).red();
  g = Pal.color(QPalette::Background).green();
  b = Pal.color(QPalette::Background).blue();
  gray = r + g + b;
  r = r * 3 * 0xD0 / gray; if (r > 0xFF) r = 0xFF;
  g = g * 3 * 0xD0 / gray; if (g > 0xFF) g = 0xFF;
  b = b * 3 * 0xD0 / gray; if (b > 0xFF) b = 0xFF;

  // set black background
  Pal.setColor(QPalette::Background, QColor(qRgb(r,g,b)));
  setAutoFillBackground(true);
  setPalette(Pal);

  r = Pal.color(QPalette::Background).red();
  g = Pal.color(QPalette::Background).green();
  b = Pal.color(QPalette::Background).blue();
  gray = r + g + b;
  r = r *3 * 0x80 / gray; if (r > 0xFF) r = 0xFF;
  g = g *3 * 0x80 / gray; if (g > 0xFF) g = 0xFF;
  b = b *3 * 0x80 / gray; if (b > 0xFF) b = 0xFF;
  arrow_bgcol.setRgb(r,g,b);
}

LogView::~LogView() {};

int LogView::lfreq_pos(double lfreq)
{
  return (int)((double)x0 + (double)(width() - 2*x0) * (double)(lfreq - lfreq0) / LOG_2 + 0.5);
}

int LogView::pos_note(int pos)
{
  return ((pos - x0) * 12 / (width() - 2*x0));
}

void LogView::drawarrow(QColor col)
{
  int x, y;
  QPainter p(this);
  
  setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
  
  qDebug() << "LogView: paintEvent2 - begin";
  
  p.setPen(Qt::white);
  x = lfreq_pos(lfreq);
  y = y0;
  p.fillRect(x - 6, y + 1, 13, h - 1, col);
  p.drawLine(x, y + 1, x, y + h - 1);
  p.end();
  qDebug() << "LogView: paintEvent2 - end";
  update();
}

void LogView::deletearrow(QColor col)
{
  int x, y;
  QPainter p(this);
  
  setAttribute(Qt::WA_PaintOutsidePaintEvent, true);
  qDebug() << "LogView: paintEvent3 - begin";
  
  p.setPen(col);
  p.setPen(arrow_bgcol);
  //        x=(int)( x0+(lfreq-lfreq0)*(width()-20)/LOG_2+0.5 );
  x = lfreq_pos(lfreq);
  y = y0;
  p.fillRect(x - 6, y + 1, 13, h - 1, arrow_bgcol);
  p.end();
  qDebug() << "LogView: paintEvent3 - end";
  update();
}

void LogView::change_lfreq(double freq)
{
  double mldf,ldf;
  int i,note;
  int r,g,b;
  QPalette Pal(palette());
  
  r = Pal.color(QPalette::Background).red();
  g = Pal.color(QPalette::Background).green();
  b = Pal.color(QPalette::Background).blue();
  deletearrow(QColor(qRgb(r,g,b)));
  
  lfreq = freq;
  
  while(lfreq > lfreq1) lfreq -= LOG_2;
  while(lfreq < lfreq0) lfreq += LOG_2;
  
  mldf = D_NOTE_LOG;
  note = 0;
  
  for(i=0; i<12; i++) {
    ldf = fabs(lfreq - lfreqs[i]);
    if (ldf<mldf) {
      mldf = ldf;
      note = i;
    }
  }
  
  drawarrow(qRgb(int(0x90 + 0x6F * mldf / D_NOTE_LOG * 2),
		 int(0xFF - 0x6F * mldf / D_NOTE_LOG * 2),
		 0x60));
}

void LogView::setScale(int scale)
{
  switch(scale) {
  case us_scale:
    note = note_us;
    break;
  case german_scale:
    note = note_ger;
    break;
  case us_scale_alt:
    note = note_us_alt;
    break;
  case german_scale_alt:
    note = note_ger_alt;
    break;
  }
  
  update();
}


void LogView::paintEvent(QPaintEvent *)
{
  int i,k,x,y,x2;
  char  str[100];
  double nat_freq[23];
  double nat_lfreq[23];
  int z,n;
  int r,g,b;
  QPalette Pal(palette());
  
  qDebug() << "LogView: paintEvent - begin";
  
  r = Pal.color(QPalette::Background).red();
  g = Pal.color(QPalette::Background).green();
  b = Pal.color(QPalette::Background).blue();

  lfreq0 = KAMMERTON_LOG - D_NOTE_LOG/2.0;
  lfreq1 = KAMMERTON_LOG - D_NOTE_LOG/2.0 + LOG_2;

  //YC erase();

  drawarrow(QColor(qRgb(r,g,b)));

  for(i=0; i<12; i++) {
    freqs [i] = KAMMERTON * pow(D_NOTE,i);
    lfreqs[i] = KAMMERTON_LOG + (double)i * D_NOTE_LOG;
  }
  
  for(i=0; i<23; i++) {
    sscanf(tuning_nat[i],"%d/%d",&z,&n);
    nat_freq[i]  = freqs[grundton] * (double)z / (double)n;
    nat_lfreq[i] = log(nat_freq[i]);
    if (nat_lfreq[i] > lfreq1) {
      nat_lfreq[i] -= LOG_2;
      nat_freq[i]  /= 2.0;
    }
  }
  
  QPainter p(this);
  QRect re(rect()); //YC
  p.eraseRect(re); //YC
  
  p.setPen(qRgb(0x50,0x50,0x50));
  
  for(k=0; k<120; k++) {
    x2 = lfreq_pos(KAMMERTON_LOG + (double)(k - 5) / 10.0 * D_NOTE_LOG);
    p.drawLine(x2, y0,    x2,y0 - ((k%5==0)?5:3));
    p.drawLine(x2, y0 + h,x2,y0 + h +((k%5==0)?5:3));
  }

  if (nat_tuning_on) {
    // draw natural tuning
    p.setFont(QFont("System",8));
    p.setPen(Qt::white);
    
    x2 = lfreq_pos(lfreqs[grundton]);
    p.drawLine(x2, y0 + h, x2, y0 + h + 10 + fontMetrics().height() * 2);
    p.setPen(Qt::black);
    p.drawText(x2 - p.fontMetrics().width("Key") / 2 + 1,
	       y0 + h + 10 + fontMetrics().ascent() + fontMetrics().height() * 2 + 1,
	       "Key");
    
    p.setPen(Qt::white);
    p.drawText(x2 - p.fontMetrics().width("Key") / 2,
	       y0 + h + 10 + fontMetrics().ascent() + fontMetrics().height() * 2,
	       "Key");
    
    for(k=0; k<23; k++) {
      x2 = lfreq_pos(nat_lfreq[k]);
      p.setPen(Qt::red);
      p.drawLine(x2, y0 + h, x2, y0 + h + 10 + fontMetrics().height() * (k%3));
    }
    
    for(k=0; k<23; k++) {
      x2 = lfreq_pos(nat_lfreq[k]);
      p.setPen(Qt::black);
      p.drawText(x2 - p.fontMetrics().width(tuning_nat[k]) / 2 + 1,
		 y0 + h + 10 + fontMetrics().ascent() + fontMetrics().height() * (k%3) + 1,
		 tuning_nat[k]);
      
      p.setPen(Qt::white);
      p.drawText(x2 - p.fontMetrics().width(tuning_nat[k]) / 2,
		 y0 + h + 10 + fontMetrics().ascent() + fontMetrics().height() * (k%3),
		 tuning_nat[k]);
    }
  }
  
  p.fillRect(x0, y0, width() - 10 - x0, h, arrow_bgcol);
  
  p.setPen(Qt::white);
  p.drawLine(x0, y0, width() - 10, y0);
  p.drawLine(x0, y0 + h,width() - 10, y0 + h);
  for(i=0; i<12; i++) {
    p.setFont(QFont("System",12));
    x = lfreq_pos(lfreqs[i]);
    y = y0;
    p.drawLine(x, y, x, y - 7);
    if (!nat_tuning_on)
      p.drawLine(x, y + h, x, y + h + 7);

    p.setPen(Qt::black);
    p.drawText(x - p.fontMetrics().width(note[i]) / 2 + 1, y - 10 + 1, note[i]);
    p.setPen(Qt::white);
    p.drawText(x - p.fontMetrics().width(note[i]) / 2, y - 10, note[i]);
    
    p.setFont(QFont("System",8));
    
    if (!nat_tuning_on)
      for(k=0; k<4; k++) {
	sprintf(str, "%.2f", (double)(freqs[i] / 8.0 * (1<<k)));
	p.setPen(Qt::black);
	p.drawText(x - p.fontMetrics().width(str) / 2 + 1,
		   y + h + 10 + p.fontMetrics().ascent() + 1 + p.fontMetrics().height() * k, 
		   str);
	p.setPen(Qt::white);
	p.drawText(x - p.fontMetrics().width(str) / 2,
		   y + h + 10 + p.fontMetrics().ascent() + p.fontMetrics().height() * k,
		   str);
      }
  }
  
  p.end();
  qDebug() << "LogView: paintEvent - end";
  update();
}

void LogView::mousePressEvent(QMouseEvent *mouse)
{
  grundton = pos_note(mouse->x());
  update();
}
