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

void appBoot(void)
{ devLedDRR    |= 0xFF;
  devSwitchDRR &= 0xFE;
  devLedPORT   |= 0xFF; }

Tuint08 uiCounter;
Tuint08 uiSelector;

void appTick00(void)
{ if (uiSelector == 0)
  { devLedPORT = ~(uiCounter++); } }

void appTick08(void)
{ if (uiSelector == 1)
  { devLedPORT = ~(uiCounter++); } }

void appTick16(void)
{ if (uiSelector == 2)
  { devLedPORT = ~(uiCounter++); } }


#if (preTaskDefined(Select))

void appLoop_Select(void)
{ Tuint08 button  = devSwitchPIN & 01;
  Tuint08 lastbutton  = button;
  genTraceInfo(button);
  while (true)
  { button = devSwitchPIN & 01;
    if (button != lastbutton)
    { if ((++uiSelector) == 3) { uiSelector = 0; } }
    taskDelayFromNow(100); } }

#endif

