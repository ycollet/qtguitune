/***************************************************************************
                          resources.h  -  description
                             -------------------
    begin                : Mon Jan 17 2000
    copyright            : (C) 2000 by Florian Berger
    email                : florian.berger@jk.uni-linz.ac.at
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


// positive flank triggereing
#define POSTRIG(c,i)      ((c)[(i)]<=128 && (c)[(i)+1]>128)
#define LEVTRIG(c,i,l)    ((c)[(i)]<=(l) && (c)[(i)+1]>(l))

//#define D_NOTE          pow(2.0,1.0/12.0)
#define D_NOTE          1.059463094

//#define D_NOTE_SQRT     pow(2.0,1.0/24.0)
#define D_NOTE_SQRT     1.029302237

//#define D_NOTE_LOG      log(D_NOTE)
#define D_NOTE_LOG      0.057762264

//#define LOG_2           log(2.0)
#define LOG_2           0.69314718

//#define KAMMERTON_PHYS  16.0/pow(2.0,3.0/12.0)*32.0
#define KAMMERTON_PHYS  430.5389646

#define KAMMERTON_WIEN  435.0

#define KAMMERTON_NORM  440.0

//#define KAMMERTON_LOG_PHYS   log(KAMMERTON_PHYS)
#define KAMMERTON_LOG_PHYS   6.06503783
//#define KAMMERTON_LOG_WIEN   log(KAMMERTON_WIEN)
#define KAMMERTON_LOG_WIEN   6.075346031
//#define KAMMERTON_LOG_NORM   log(KAMMERTON)
#define KAMMERTON_LOG_NORM   6.086774727

#define NO  0
#define YES 1

inline int Abs(int x) { return ((x>0)?x:-x);}
