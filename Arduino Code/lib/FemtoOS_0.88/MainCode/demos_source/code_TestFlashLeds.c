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

/* This file is solely for demonstration purposes. */

#include "femtoos_code.h"


#if cfgSysDebug == cfgTrue
  #define delay07   9U
  #define delay06   9U
  #define delay05   8U
  #define delay04   7U
  #define delay03   6U
  #define delay02   5U
  #define delay01   4U
  #define delay00   3U
#else
  #define delay07 25000U
  #define delay06 13661U
  #define delay05  7465U
  #define delay04  4079U
  #define delay03  2229U
  #define delay02  1218U
  #define delay01   666U
  #define delay00   364U
#endif

#define testWithPermanentTasks  cfgFalse

void appBoot(void)
{ devLedDRR  |= 0xFF;
  devLedPORT |= 0xFF; }


#if true
  /* here you don't need protection since the devLedPORT operations are sbi/cbi which are atomic operations */
  #define setLed(lednr,state)  if (state) { devLedPORT |= (1 << lednr); } else { devLedPORT &=  ~(1 << lednr); }
#else
 /* If you use the code below make sure to include critical  protection and if needed critical sections.*/
  static void setLed(Tuint08 lednr, Tbool state)
  { taskEnterTickCritical();
    if (state) { devLedPORT &=  ~(1 << lednr); }  else { devLedPORT |= (1 << lednr); }
    taskExitTickCritical(); }
#endif

#if (preTaskDefined(LEDtask0))

void appLoop_LEDtask0(void)
{ while (true)
  { setLed(0,true);
    taskDelayFromNow(delay00);
    setLed(0,false);
    taskDelayFromNow(delay00); } }

#endif


#if (preTaskDefined(LEDtask1))

void appLoop_LEDtask1(void)
{ while (true)
  { setLed(1,true);
    taskDelayFromNow(delay01);
    setLed(1,false);
    taskDelayFromNow(delay01); } }

#endif


#if (preTaskDefined(LEDtask2))

void appLoop_LEDtask2(void)
{ while (true)
  { setLed(2,true);
    taskDelayFromNow(delay02);
    setLed(2,false);
    taskDelayFromNow(delay02); } }

#endif


#if (preTaskDefined(LEDtask3))

void appLoop_LEDtask3(void)
{ while (true)
  { setLed(3,true);
    taskDelayFromNow(delay03);
    setLed(3,false);
    taskDelayFromNow(delay03); } }

#endif


#if (preTaskDefined(LEDtask4))

void appLoop_LEDtask4(void)
{ while (true)
  { setLed(4,true);
    taskDelayFromNow(delay04);
    setLed(4,false);
    taskDelayFromNow(delay04); } }

#endif


#if (preTaskDefined(LEDtask5))

void appLoop_LEDtask5(void)
{ while (true)
  { setLed(5,true);
    taskDelayFromNow(delay05);
    setLed(5,false);
    taskDelayFromNow(delay05); } }

#endif


#if (preTaskDefined(LEDtask6))

void appLoop_LEDtask6(void)
{ while (true)
  { setLed(6,true);
    taskDelayFromNow(delay06);
    setLed(6,false);
    taskDelayFromNow(delay06); } }

#endif


#if (preTaskDefined(LEDtask7))

void appLoop_LEDtask7(void)
{ while (true)
  { setLed(7,true);
    taskDelayFromNow(delay07);
    setLed(7,false);
    taskDelayFromNow(delay07); } }

#endif


