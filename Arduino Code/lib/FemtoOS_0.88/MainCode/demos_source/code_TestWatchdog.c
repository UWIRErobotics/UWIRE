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

#define delay07  1600U
#define delay06   800U
#define delay05   400U
#define delay04   200U
#define delay03  1600U
#define delay02   800U
#define delay01   400U
#define delay00   200U

static Tbool bSync = false;

void appBoot(void)
{ devLedDRR  |= 0xFF;
  devSwitchDRR  &= 0xFC;
  devLedPORT |= 0xFF; }

/* here you don't need protection since the devLedPORT operations are sbi/cbi atomair operations! */
#define setLed(lednr,state)  if (state) { devLedPORT |= (1 << lednr); } else { devLedPORT &=  ~(1 << lednr); }


#if (preTaskDefined(LEDtask0))

void appLoop_LEDtask0(void)
{ Tuint08 led  = 0;
  Tuint08 count = 32;
  while (count--)
  { taskFeedWatchdog();
    led = 1 - led;
    setLed(0,led);
    taskDelayFromNow(delay00); }
   while (1); }

/* Als de watchdog afgaat wordt bark aangeroepen een daarna de taak opnieuw
 * geinitialiseerd en gestart. Maar Init wordt NIET meer aangeroepen, dat kan
 * vanuit hier gebeuren indien nodig, maar Init bevat typisch alleen echt
 * eenmalige startcode. De rest gaat voor de oneindige loop in Loop0X
 */
void appBark_LEDtask0(void)
{ devLedPORT |= 0x01;
  Tuint08 lastPrio = genGetPriorityOnName(LEDtask0);
  if(lastPrio>0)
  { --lastPrio;
    genSetPriorityOnName(LEDtask0,lastPrio); }
  else
  { genSuspendOnName(LEDtask0); } }

#endif


#if (preTaskDefined(LEDtask1))

void appLoop_LEDtask1(void)
{ Tuint08 led  = false;
  while (true)
  { led = !led;
    setLed(1,led);
    taskDelayFromNow(delay01);
    led = !led;
    setLed(1,led);
    taskDelayFromNow(delay01); } }

#endif


#if (preTaskDefined(LEDtask2))

void appLoop_LEDtask2(void)
{ Tuint08 led  = false;
  while (true)
  { led = !led;
    setLed(2,led);
    taskDelayFromNow(delay02);
    led = !led;
    setLed(2,led);
    taskDelayFromNow(delay02); } }

#endif


#if (preTaskDefined(LEDtask3))

void appLoop_LEDtask3(void)
{ Tuint08 led  = false;
  while (true)
  { led = !led;
    setLed(3,led);
    taskDelayFromNow(delay03);
    led = !led;
    setLed(3,led);
    taskDelayFromNow(delay03);
    if (bSync)
    { taskWaitForOtherTaskOnName(TrefPunt);
      bSync = false; } } }

#endif


#if (preTaskDefined(LEDtask4))

void appLoop_LEDtask4(void)
{ Tuint08 led  = false;
  while (true)
  { led = !led;
    setLed(4,led);
    taskDelayFromNow(delay04);
    led = !led;
    setLed(4,led);
    taskDelayFromNow(delay04); } }

#endif


#if (preTaskDefined(LEDtask5))

void appLoop_LEDtask5(void)
{ Tuint08 led  = false;
  while (true)
  { led = !led;
    setLed(5,led);
    taskDelayFromNow(delay05);
    led = !led;
    setLed(5,led);
    taskDelayFromNow(delay05); } }

#endif


#if (preTaskDefined(LEDtask6))

void appLoop_LEDtask6(void)
{ Tuint08 led  = false;
  while (true)
  { led = !led;
    setLed(6,led);
    taskDelayFromNow(delay06);
    led = !led;
    setLed(6,led);
    taskDelayFromNow(delay06);  } }

#endif


#if (preTaskDefined(LEDtask7))

void appLoop_LEDtask7(void)
{ Tuint08 led  = false;
  while (true)
  { led = !led;
    setLed(7,led);
    taskDelayFromNow(delay07);
    led = !led;
    setLed(7,led);
    taskDelayFromNow(delay07);
    if (bSync)
    { taskWaitForOtherTaskOnName(TrefPunt);
      bSync = false; } } }

#endif


#if (preTaskDefined(Synchronize))

void appLoop_Synchronize(void)
{ Tuint08 button  = devSwitchPIN & 01;
  Tuint08 lastbutton  = button;
  while (true)
  { button = devSwitchPIN & 01;
    if (button != lastbutton)
    { bSync = true;
      asm("mov r1,r24");
    }
    taskDelayFromNow(100); } }

#endif



#if (preTaskDefined(Release))

void appLoop_Release(void)
{ Tuint08 button  = devSwitchPIN & 02;
  Tuint08 lastbutton  = button;
  while (true)
  { button = devSwitchPIN & 02;
    if (button != lastbutton)
    { genSetPriorityOnName(LEDtask0,3);
      genResumeOnName(LEDtask0); }
    taskDelayFromNow(100); } }

#endif

