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

static void setNumber(Tuint08 mask,Tuint08 number)
{ taskEnterSwitchCritical();
  devLedPORT = ~((~devLedPORT & (mask)) | (number));
  taskExitSwitchCritical(); }

static void invLed(Tuint08 lednr)
{ taskEnterSwitchCritical();
  devLedPORT ^= (1 << lednr);
  taskExitSwitchCritical(); }

void appInit_WriteTask(void)
{ devLedDRR  |= 0x0F;
  devLedPORT |= 0x0F; }

void appLoop_WriteTask(void)
{ Tuint08 count = 0;
  while (true)
  { if (taskQueuWriteRequestOnName(ThePipe,8,800))
    { for (count=0; count<8; count++)
      { genQueuWriteOnName(ThePipe,count);
        setNumber(0xF0,count);
        taskDelayFromNow(600); }
      taskQueuReleaseOnName(ThePipe); }
    else
    { for (count=0; count<10; count++)
      { invLed(3);
        taskDelayFromNow(80); } } } }


void appInit_ReadTask(void)
{ devLedDRR  |= 0xF0;
  devLedPORT |= 0xF0; }

void appLoop_ReadTask(void)
{ Tuint08 count = 0;
  Tuint08 Result;
  while (true)
  { if (taskQueuReadRequestOnName(ThePipe,6,800))
    { for (count=0; count<6; count++)
      { Result = genQueuReadOnName(ThePipe);
        setNumber(0x0F,Result<<4);
        taskDelayFromNow(600); }
      taskQueuReleaseOnName(ThePipe); }
    else
    { for (count=0; count<10; count++)
      { invLed(7);
        taskDelayFromNow(80); } } } }


#if (preTaskDefined(ResetTask))

void appInit_ResetTask(void)
{ devSwitchDRR = 0x00; }

void appLoop_ResetTask(void)
{ Tuint08 button  = devSwitchPIN & 01;
  Tuint08 lastbutton  = button;
  while (true)
  { button = devSwitchPIN & 01;
    if (button != lastbutton)
    { setNumber(0x88,0x88);
      taskQueuReadRequestOnName(ThePipe,0,defMaxDelayTime);
      Tuint08 uiLedState = devLedPORT;
      devLedPORT = 0x00;
      genQueuClearOnName(ThePipe);
      taskDelayFromNow(80);
      devLedPORT = uiLedState;
      taskQueuReleaseOnName(ThePipe);
      }
    taskDelayFromNow(100); } }

#endif

