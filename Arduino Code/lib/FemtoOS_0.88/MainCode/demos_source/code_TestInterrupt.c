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

/* This file is solely for demonstration purposes. Note that use for
 * different devices requires different hardwiring to the switches.
 */

#include "femtoos_code.h"

#define delay03  2229U
#define delay02  1218U
#define delay01   666U
#define delay00   364U

void appBoot(void)
{ devLedDRR  = 0xFF;
  devLedPORT = 0x0F;
  devSwitchDRR = 0x00;
  devEIR = preBitSet1(0x00,devINT); }

/* Different options to play with. Vary this and the cfgIntOsProtected too.
 * See the different results.
 */
#define smallISR  cfgTrue

static void setLed(Tuint08 lednr, Tbool state)
{ taskEnterGlobalCritical();
  if (state) { devLedPORT |= (1 << lednr); }  else { devLedPORT &=  ~(1 << lednr); }
  taskExitGlobalCritical(); }

void appLoop_LEDtask0(void)
{ Tuint08 led  = false;
  while (true)
  { led = !led;
    setLed(0,led);
    taskDelayFromNow(delay00); } }

void appLoop_LEDtask1(void)
{ Tuint08 led  = false;
  while (true)
  { led = !led;
    setLed(1,led);
    taskDelayFromNow(delay01); } }

void appLoop_LEDtask2(void)
{ Tuint08 led  = false;
  while (true)
  { led = !led;
    setLed(2,led);
    taskDelayFromNow(delay02); } }

void appLoop_LEDtask3(void)
{ Tuint08 led  = false;
  while (true)
  { led = !led;
    setLed(3,led);
    taskDelayFromNow(delay03); } }


#if (cfgIntOsProtected == cfgTrue)

  #if (smallISR == cfgTrue)

  void devSigExternalInt(void) __attribute__ ( ( signal, naked, used, externally_visible ) );
  void devSigExternalInt(void)
  { isrEnter();
    Tuint08 uiRunMask =  (~devLedPORT + 0x10);
    devLedPORT =  ~(uiRunMask);
    isrStackCheck(2);
    /* The line below should never be executed, but leave it there as a test */
    if (preBitIsSet(devAuxSysReg,devAuxSysRegBit)) { devLedPORT=~0xA5; while(true); }
    if ((uiRunMask & 0x10) == 0) { genSuspendOnName(LEDtask0); } else { genResumeOnName(LEDtask0); }
    if ((uiRunMask & 0x20) == 0) { genSuspendOnName(LEDtask1); } else { genResumeOnName(LEDtask1); }
    if ((uiRunMask & 0x40) == 0) { genSuspendOnName(LEDtask2); } else { genResumeOnName(LEDtask2); }
    if ((uiRunMask & 0x80) == 0) { genSuspendOnName(LEDtask3); } else { genResumeOnName(LEDtask3); }
    isrExit(); }

  #else

  /* The gcc warning is ok, we misuse the gcc facilities to make sure all registers are saved. */
  void HandlePinChange(void) __attribute__ ( ( signal ) );
  void HandlePinChange(void)
  { isrStackCheck(2);
    Tuint08 uiRunMask =  (~devLedPORT + 0x10);
    if ((uiRunMask & 0x10) == 0) { genSuspend(preTaskNumberOf(LEDtask0)); } else { genResume(preTaskNumberOf(LEDtask0)); }
    if ((uiRunMask & 0x20) == 0) { genSuspend(preTaskNumberOf(LEDtask1)); } else { genResume(preTaskNumberOf(LEDtask1)); }
    if ((uiRunMask & 0x40) == 0) { genSuspend(preTaskNumberOf(LEDtask2)); } else { genResume(preTaskNumberOf(LEDtask2)); }
    if ((uiRunMask & 0x80) == 0) { genSuspend(preTaskNumberOf(LEDtask3)); } else { genResume(preTaskNumberOf(LEDtask3)); }
    devLedPORT = ~uiRunMask; }

  void devSigExternalInt(void) __attribute__ ( ( signal, naked, used, externally_visible ) );
  void devSigExternalInt(void)
  { isrBegin();
    HandlePinChange();
    /* This is a dangerous trick, since from reti from HandlePinChange we need to be sure the cli comes
     * immediately. taskDisableGlobalInterrupts() may have other business (trace) */
    taskDisableGlobalInterrupts();
    isrEndYield(); }

  #endif


#else

/* The signal attribute makes sure all registers are properly restored.
 * Unfortunately it also stores registers never used, such as r0,r1.
 * Further it returns with a reti, where is should be a ret. Therefore
 * we must immediately disable interrupts again after return.
 * If you program this assembler all these problems are absent. ;-)
 * The compiler complains you misspelled your function, now did you?
 */

  void HandlePinChange(void) __attribute__ ( ( signal ) );
  void HandlePinChange(void)
  { devLedPORT =  ~(~devLedPORT + 0x10); }

  void devSigExternalInt(void) __attribute__ ( ( signal, naked, used, externally_visible ) );
  void devSigExternalInt(void)
  { isrBegin();
    /* make sure do not use any registers here, there is nothing to protect you from
     * bizarre results. */
    HandlePinChange();
    taskDisableGlobalInterrupts();
    isrEndReturn(); }

#endif

