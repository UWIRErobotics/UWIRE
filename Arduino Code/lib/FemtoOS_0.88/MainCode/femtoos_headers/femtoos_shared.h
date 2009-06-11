/*
 * Femto OS v 0.88 - Copyright (C) 2008  Ruud Vlaming
 *
 * This file is part of the Femto OS distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Please note that, due to the GPLv3 license, for application of this
 * work and/or combined work in embedded systems special obligations apply.
 * If these are not to you liking, please know the Femto OS is dual
 * licensed. A commercial license and support are available.
 * See http://www.femtoos.org/ for details.
 */

#ifndef FEMTOOS_SHARED_H
#define FEMTOOS_SHARED_H

#include "femtoos_types.h"


/* This file contains the variables and declarations shared between the implementation
 * files of the different part of the Femto OS. It should not be included in application
 * code.
 */

extern Ttickcount  uxTickCount;
extern Tuint08     uiOsStatus;
extern TosData     xOS;
extern Tuint08     uiFsStatus;

extern Tuint16     uiOsLoadCollect;
extern Tuint16     uiOsLoadTotal;
extern Tuint16     uiIdleLoadCollect;
extern Tuint16     uiIdleLoadTotal;
extern Tuint08     uiIsrLoadTemp;
extern Tuint16     uiIsrLoadCollect;
extern Tuint16     uiIsrLoadTotal;

extern Tuint08     uiOsStackMax;
#if (StackSizeISR > 0xFF)
  extern Tuint16     uiIsrStackMax;
#else
  extern Tuint08     uiIsrStackMax;
#endif

extern Tchar       StackISR[StackSizeISR];

extern void appTick00(void);
extern void appTick08(void);
extern void appTick16(void);
extern void appTickSleep(void);
extern void appEnterIdle(void);
extern void appEnterSleep(void);
extern void appExitSleep(void);
extern void appBoot(void);


extern const Tbyte * pxTCBlist[defNumberOfTasks];
extern const TtaskDefinitionBlock * pxTDBlist[defNumberOfTasks];
extern const fpInitTask pxInitlist[defNumberOfTasks];
extern const fpLoopTask pxLooplist[defNumberOfTasks];
extern const fpBarkTask pxBarklist[defNumberOfWatchdogTasks];
extern const Tbyte * pxQueulist[defNumberOfQueus];
extern const Tuint08 uiQueuSize[defNumberOfQueus];
extern const Tuint08 uiFileSpace[defFsNumberOfPreFiles];
extern const Tuint08 uiCapabilities[defNumberOfTasks];

extern Tuint08 uiQueuWrite[defNumberOfQueus];
extern Tuint08 uiQueuRead[defNumberOfQueus];


#endif /* FEMTOOS_SHARED_H */
