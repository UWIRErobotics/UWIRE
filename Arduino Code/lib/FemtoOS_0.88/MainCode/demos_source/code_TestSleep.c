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

#define delay05 31000U
#define delay04 30500U
#define delay03 30000U
#define delay02  1500U
#define delay01  1000U
#define delay00   250U
#define delayFLASH 50U

/* If only two leds burn continuously you have not connected the button
 * to the correct interrupt pin. See the pin-configuration for your device
 * and connect the switch to INT0 */

#if (defSysGCCstartup != cfgReplace) && (devINT != cfgUndefined)
  #define IntPortSetup()   do { devSwitchDRR = 0x00; devEIR = preBitSet1(0x00,devINT); } while(false)
  void devSigExternalInt(void) __attribute__ ( ( signal, naked, used, externally_visible ) );
  void devSigExternalInt(void) { asm("reti"); }
#else
/* Games with interrupts are not possible when we use cfgReplace, since there is no interrupt
 * vector table. So pushing the button will not cause a wake-up in the demo. */
  #define IntPortSetup()
#endif

void appTickSleep(void)
{ devLedPORT &= ~0x40;
  portNop();
  portNop();
  portNop();
  devLedPORT |= 0x40; }

void appEnterSleep(void) { devLedPORT |= 0x80; }

void appExitSleep(void)  { devLedPORT &= ~0x80; }

void appBoot(void)
{ devLedDRR    = 0xFF;
  devLedPORT   = 0xFF;
  IntPortSetup(); }

/* here you don't need protection since the devLedPORT operations are sbi/cbi atomair operations! */
#define setLed(lednr,state)  if (state) { devLedPORT &=  ~(1 << lednr); } else { devLedPORT |= (1 << lednr); }

void flashLED(Tuint08 ledNR)
{ setLed(ledNR,1);
  taskDelayFromNow(delayFLASH);
  setLed(ledNR,0); }

#if (preTaskDefined(LEDtask0))

void appLoop_LEDtask0(void)
{ while (true)
  { flashLED(0);
    taskDelayFromNow(delay02);
    flashLED(0);
    #if (cfgUseLowPowerOnDelay == cfgTrue)
      taskDelayFromNow(delay05);
    #else
      taskSleep();
    #endif
  } }

#endif

#if (preTaskDefined(LEDtask1))

void appLoop_LEDtask1(void)
{ while (true)
  { flashLED(1);
    taskDelayFromNow(delay01);
    flashLED(1);
    #if (cfgUseLowPowerOnDelay == cfgTrue)
      taskDelayFromNow(delay04);
    #else
      taskSleep();
    #endif
  } }

#endif

#if (preTaskDefined(LEDtask2))

void appLoop_LEDtask2(void)
{ while (true)
  { Tuint08 count = 8;
    while (count--)
    { flashLED(2);
      taskDelayFromNow(delay00); }
    taskSleep(); } }

#endif

