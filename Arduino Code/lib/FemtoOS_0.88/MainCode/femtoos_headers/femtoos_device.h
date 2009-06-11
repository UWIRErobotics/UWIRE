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

/* Port check, see a list of all possible devices at:
 * http://www.nongnu.org/avr-libc/user-manual/using_tools.html */

#if defined(__AVR_ATmega128__)

  /* port is present and tested */
  #include "femtoos_ATmega128.asm"

#elif defined(__AVR_ATmega1280__)

  /* port is present and tested */
  #include "femtoos_ATmega1280.asm"

#elif defined(__AVR_ATmega1281__)

  /* port is present and tested */
  #include "femtoos_ATmega1281.asm"

#elif defined(__AVR_ATmega1284P__)

  /* port is present but untested on the actual device. */
  #warning "Port for this device (ATmega1284P) is untested"
  #include "femtoos_ATmega1284P.asm"

#elif defined(__AVR_ATmega16__)

  /* port is present and tested */
  #include "femtoos_ATmega16A.asm"

#elif defined(__AVR_ATmega164P__)

  /* port is present and tested */
  #include "femtoos_ATmega164P.asm"

#elif defined(__AVR_ATmega168__)

  /* port is present and tested */
  #include "femtoos_ATmega168.asm"

#elif defined(__AVR_ATmega168P__)

  /* port is present but untested on the actual device. */
  #warning "Port for this device (ATmega168P) is untested"
  #include "femtoos_ATmega168P.asm"

#elif defined(__AVR_ATmega2560__)

  /* port is present and tested */
  #include "femtoos_ATmega2560.asm"

#elif defined(__AVR_ATmega2561__)

  /* port is present and tested */
  #include "femtoos_ATmega2561.asm"

#elif defined(__AVR_ATmega32__)

  /* port is present and tested */
  #include "femtoos_ATmega32A.asm"

#elif defined(__AVR_ATmega324P__)

  /* port is present and tested */
  #include "femtoos_ATmega324P.asm"

#elif defined(__AVR_ATmega328P__)

  /* port is present and tested */
  #include "femtoos_ATmega328P.asm"

#elif defined(__AVR_ATmega48__)

  /* port is present and tested */
  #include "femtoos_ATmega48.asm"

#elif defined(__AVR_ATmega48P__)

  /* port is present and tested */
  #include "femtoos_ATmega48P.asm"

#elif defined(__AVR_ATmega64__)

  /* port is present and tested */
  #include "femtoos_ATmega64.asm"

#elif defined(__AVR_ATmega640__)

  /* port is present and tested */
  #include "femtoos_ATmega640.asm"

#elif defined(__AVR_ATmega644__)

  /* port is present and tested */
  #include "femtoos_ATmega644.asm"

#elif defined(__AVR_ATmega644P__)

  /* port is present and tested */
  #include "femtoos_ATmega644P.asm"

#elif defined(__AVR_ATmega8__)

  /* port is present and tested */
  #include "femtoos_ATmega8.asm"

#elif defined(__AVR_ATmega88__)

  /* port is present and tested */
  #include "femtoos_ATmega88.asm"

#elif defined(__AVR_ATmega88P__)

  /* port is present and tested */
  #include "femtoos_ATmega88P.asm"

#elif defined(__AVR_ATtiny24__)

  /* port is present and tested */
  #include "femtoos_ATtiny24.asm"

#elif defined(__AVR_ATtiny25__)

  /* port is present and tested */
  #include "femtoos_ATtiny25.asm"

#elif defined(__AVR_ATtiny261__)

  /* port is present and tested */
  #include "femtoos_ATtiny261.asm"

#elif defined(__AVR_ATtiny44__)

  /* port is present and tested */
  #include "femtoos_ATtiny44.asm"

#elif defined(__AVR_ATtiny45__)

  /* port is present and tested */
  #include "femtoos_ATtiny45.asm"

#elif defined(__AVR_ATtiny461__)

  /* port is present and tested */
  #include "femtoos_ATtiny461.asm"

#elif defined(__AVR_ATtiny84__)

  /* port is present and tested */
  #include "femtoos_ATtiny84.asm"

#elif defined(__AVR_ATtiny85__)

  /* port is present and tested */
  #include "femtoos_ATtiny85.asm"

#elif defined(__AVR_ATtiny861__)

  /* port is present and tested */
  #include "femtoos_ATtiny861.asm"

#else

  /* ports are not present. */
  #error "Port for this device is absent"

#endif

