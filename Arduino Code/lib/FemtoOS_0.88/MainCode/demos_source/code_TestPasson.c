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

#define taskMutexRelease(Slot)             taskSyncRelease(Slot)


void appBoot(void)
{ devLedDRR    = 0xFF;
  devLedPORT   = 0xFF;
  devSwitchDRR = 0x00; }

/* here you don't need protection since the devLedPORT operations are sbi/cbi atomair operations! */
#define setLed(lednr,state)  if (state) { devLedPORT |= (1 << lednr); } else { devLedPORT &=  ~(1 << lednr); }

#if (defSysOptimized == cfgTrue)
  /* Please note the use of "always_inline" attribute, if you remove this the register use explodes,
   * whereas with this trick the compiler can highly optimize your code (which of course gets longer). */
  static void HandOver(Tuint08 uiLed) __attribute__ ( ( naked, noreturn, always_inline ) );
#else
  static void HandOver(Tuint08 uiLed) __attribute__ ( ( naked, noreturn ) );
#endif

#if (cfgUseSynchronization == cfgSyncSingleBlock)

  #define taskMutexRequest(Slot,TicksToWait) taskSyncRequest(Slot,0,TicksToWait)

  static void HandOver(Tuint08 uiLed)
  { while (true)
    { Tuint08 uiSource = (uiLed & 0x07);
      Tuint08 uiTarget = ((uiLed+1) & 0x07);
      if (taskMutexRequest(uiSource+1,defLockDoNotBlock))
      { if (taskMutexRequest(uiTarget+1,defLockDoNotBlock))
        { taskMutexRequest(9,defLockBlockInfinite);
          Tuint08 uiBit = (devLedPORT >> uiSource) & 0x01;
          setLed(uiTarget,uiBit)
          taskMutexRelease(9);
          taskMutexRelease(uiTarget+1); }
        taskMutexRelease(uiSource+1); }
      taskDelayFromNow(300+(uiLed << 3)); } }

#elif (cfgUseSynchronization == cfgSyncDoubleBlock)

  #define taskMutexRequest(Slot,TicksToWait) taskSyncRequest(Slot,0,0,TicksToWait)

  static void HandOver(Tuint08 uiLed)
  { while (true)
    { Tuint08 uiSource = (uiLed & 0x07);
      Tuint08 uiTarget = ((uiLed+1) & 0x07);
      Tuint08 uiDoubleSlot = (((uiSource+1)<<4) | (uiTarget+1));
      if (taskMutexRequest(uiDoubleSlot,defLockDoNotBlock))
      { taskMutexRequest(9,defLockBlockInfinite);
        Tuint08 uiBit = (devLedPORT >> uiSource) & 0x01;
        setLed(uiTarget,uiBit)
        taskMutexRelease(9);
        taskMutexRelease(uiDoubleSlot); }
      taskDelayFromNow(300+(uiLed << 3)); } }

#else
  #error this is not possible
#endif

#if (preTaskDefined(LEDtask0))
  void appLoop_LEDtask0(void) { HandOver(0); }
#endif

#if (preTaskDefined(LEDtask1))
  void appLoop_LEDtask1(void) { HandOver(1); }
#endif

#if (preTaskDefined(LEDtask2))
  void appLoop_LEDtask2(void) { HandOver(2); }
#endif

#if (preTaskDefined(LEDtask3))
  void appLoop_LEDtask3(void) { HandOver(3); }
#endif

#if (preTaskDefined(LEDtask4))
  void appLoop_LEDtask4(void) { HandOver(4); }
#endif

#if (preTaskDefined(LEDtask5))
  void appLoop_LEDtask5(void) { HandOver(5); }
#endif

#if (preTaskDefined(LEDtask6))
  void appLoop_LEDtask6(void) { HandOver(6); }
#endif

#if (preTaskDefined(LEDtask7))
  void appLoop_LEDtask7(void) { HandOver(7); }
#endif


#if (preTaskDefined(Reset))

void appLoop_Reset(void)
{ Tuint08 button  = devSwitchPIN & 01;
  Tuint08 lastbutton  = button;
  while (true)
  { button = devSwitchPIN & 01;
    if (button != lastbutton)
    { taskMutexRequest(9,defLockBlockInfinite);
      devLedPORT = 0xFE;
      taskMutexRelease(9);
      }
    taskDelayFromNow(100); } }

#endif


#if (preTaskDefined(AutoStart))

void appLoop_AutoStart(void)
{ while(true)
  { taskMutexRequest(9,defLockBlockInfinite);
    Tuint08 uiLeds = devLedPORT;
    if ((uiLeds == 0x00) || (uiLeds == 0xFF)) { devLedPORT ^= 0x01; }
    taskMutexRelease(9);
    taskDelayFromNow(30000); } }

#endif
