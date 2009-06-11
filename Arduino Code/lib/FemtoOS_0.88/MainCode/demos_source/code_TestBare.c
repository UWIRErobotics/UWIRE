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

void appEnterIdle(void)
{ /* Using a register saves 2 bytes ;-) */
  Tuint08 newPortB = devSwitchPORT--;
  /* Start counting on all i/o lines */
  if (newPortB == 0x00) { devLedPORT--; } }

void appBoot(void)
{ /* Make sure all lines are output */
  devLedDRR = 0xFF;
  devSwitchDRR = 0xFF;  }

