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

#ifndef FEMTOOS_GLOBALS_H
#define FEMTOOS_GLOBALS_H

/* DISCUSSION
 * Unfortunately we must include here some hardware dependent files. avr/io.h could
 * probably be moved to port.h, but this does not hold true for the avr/pgmspace.h.
 * Therefore we kept them together. The point is, storage in flash is not so hardware
 * independent as it may seem. Thus, if you want to port this OS, this is one point
 * to look at.
 */

#include  <avr/io.h>

/* These includes are not compatible with assembler language. But since we need the
 * globals in the startup code, we must explicitly exclude them here.
 */
#ifndef __ASSEMBLER__
  #include  <avr/pgmspace.h>
  #include  <string.h>
#endif

/**
 * Identification string for this OS. Make sure it has 12 bytes.
 */
#define SystemID   "FemtoOS 0.88"

/**
 * Our standard 'bools'.
 */
#define true       1
#define false      0


/* DISCUSSION
 * We don't use primitive data types in the code anywhere. All types are related
 * to the types below. We compile with the option -mint8, otherwise gcc promotes
 * a lot of 8 bit operations to 16 bit. Although this is standard behavior, and
 * can be suppressed with the right casts, to my opinion this has been the wrong
 * choice. C is a machine language and should respect the true lengths of the
 * defined variables. Note that it seems that GCC does not honor the bit lengths
 * defined; so why are the present anyway? With -mint8 int32_t becomes a 16 bit
 * integer. Why? This is totally confusing.
 */
#define Tchar      unsigned char
#define Tbool      Tuint08
#define Taddress   Tchar *
#define Tbyte      uint8_t
#define Tsint08    int8_t
#define Tuint08    uint8_t
#define Tsint16    int16_t
#define Tuint16    uint16_t
#define Tdouble    double
#define Tenum      uint8_t

/** <!--0601-->
 * The highest delay time you may use.
 *
 * The highest delay time you may use. If your tick time is 1ms, and since
 * we have a 16 bit tick counter, the maximum delay is about 65 seconds. In my
 * opinion, this is long enough. If you want to delay tasks longer it is
 * better to get a real time signal somewhere.
 */
#define defMaxDelayTime        0xFEFF

/** <!--0602-->
 * Use in a synchronizing method to specify it may not timeout
 *
 * Value you can use to indicate that a lock need not to timeout. Use this
 * rather than supply 0 to make your intentions clear.
 */
#define defLockBlockInfinite   0x00

/** <!--0603-->
 * Use in a synchronizing method to say it must return immediately
 *
 * Value you can use to indicate that a lock need timeout immediately, thus
 * just check if you can obtain the lock, but do not wait for it. Use this
 * rather than supply 1 to make your intentions clear.
 */
#define defLockDoNotBlock      0x01

/** <!--0604-->
 * Value with which the stack is filled.
 *
 * For testing purposes you can fill the stack with other values that 0x00.
 * This may be handy to see if you are not misusing a register. For the shortest
 * code, leave it to 0x00, so the compiler can rely upon the .bss section cleaning.
 * For a discussion how to use this setting see also cfgSysRegisterCheckByte.
 * Also see the discussion at defStackClean.
 */
#define defStackInitByte       0x00

/** <!--0605-->
 * Value to specify the current task.
 *
 * In all (external) methods accepting the uiTaskNumber parameter you can specify
 * defCurrentTaskNumber to indicate you want to use it for the current task, if
 * you do not know that number (for example when code is run from different
 * tasks).
 * Please be absolutely sure you are inside a real task. If you are inside an
 * isr, inside init or bark routines, you are not in task space and the current
 * task may not be what you expect. However, the system cannot warn you for this
 * situation. Thus, only to be used inside code that was called from appLoop_XXX.
 */
#define defCurrentTaskNumber   0x80

/** <!--0606-->
 * Literal value to specify the current task.
 *
 * In all (external) methods accepting the TaskName parameter you can specify
 * defCurrentTask to indicate you want to use it for the current task, if
 * you do not know that name (for example when code is run from different
 * tasks). Never use it as a number in your code
 * Please be absolutely sure you are inside a real task. If you are inside an
 * isr, inside init or bark routines, you are not in task space and the current
 * task may not be what you expect. However, the system cannot warn you for this
 * situation. Thus, only to be used inside code that was called from appLoop_XXX.
 */
#define defCurrentTaskName     defCurrentTaskName

/** <!--0607-->
 * Literal value to state to start at the beginning of a file.
 *
 * In most file operation calls you can specify an offset. Instead of using zero, you
 * may always use this constant to make your intention clear that you want to start
 * reading or writing from the file start.
 */
#define defFromFileStart        0x00

/** <!--0608-->
 * Literal value to state to start at the end of a file.
 *
 * In most file operation calls you can specify an offset. Instead of using the
 * real file length (which you must know or obtain), you
 * may use this constant to start writing the new bytes at the end of the file.
 * This constant is only available if cfgUseFileSystemMaintainFAT is activated.
 */
#define defFromFileEnd          0xFF

/** <!--0609-->
 * Literal value to state you want to run up to the end of a file.
 *
 * In most file operation calls you can specify an size. Instead of using the
 * real file length (which you must know or obtain), you
 * may use this constant to keep reading bytes up to the end of the file.
 * This constant is only available if cfgUseFileSystemMaintainFAT is activated.
 */
#define defUntilFileEnd         0xFF

/** <!--0610-->
 * Literal value to state to open the file system in read only mode.
 *
 * If cfgUseFileSystemConcurrentRead is activated, it is required to specify if you
 * want to open the file system in read only mode, or in read/write mode. Although, you
 * can just use a boolean there, you may want to use this constant to make your
 * intentions mode explicit.
 */
#define defReadOnly             0x01

/** <!--0611-->
 * Literal value to state to open the file system in read/write mode.
 *
 * If cfgUseFileSystemConcurrentRead is activated, it is required to specify if you
 * want to open the file system in read only mode, or in read/write mode. Although, you
 * can just use a boolean there, you may want to use this constant to make your
 * intentions mode explicit.
 */
#define defReadWrite            0x00

/** <!--0612-->
 * Literal value to fire all events in the genFireEventSet method.
 *
 * If you need to unblock all waiting tasks by firing all all events use this
 * constant in the method genFireEventSet. It makes your intentions clear.
 */
#define defAllEvents            0xFF

/* DISCUSSION
 * Errors are divided into two categories, fatal and non fatal errors. Production code
 * should of course generate no error at all, but in testing situations, the non fatal
 * errors try to stop the offending task, and continue (but do not release any locks
 * the task was holding). Note this is only possible if
 * the stacks are not damaged, after the error has been reported. Normally a SaveContext
 * will not corrupt the task stack is the context does not fit (it will skip the save in
 * test mode) but reporting the error itself takes quite some stack space and can also
 * corrupt a stack. Fatal errors are reported indefinitely.
 * The error consists off 2x 8bit, with the following format:
 * SSfx xxxx ssss tttt
 *     SS: signbits, used to blink, cleared in the call
 *  xxxxx: error number
 *      f: fatal error
 *   tttt: task number
 *   ssss: slot number / other information
 * Errors should, in the current implementation not exceed number 0x2F, since the
 * upper two bits are used for other purposes (blink leds and show info about the
 * type of error. This could reduced to one bit, thereby freeing the numbers until 0x4F.
 */

/**
 * The stack of the OS is under the required minimum. It may not be lower as the space taken by
 * the background variables.
 */
#define  fatOsStackUnderMinimum           0x21

/**
 * During use the size of the OS stack became to small, other data may be overwritten.
 */
#define  fatOsStackOverflowed             0x22

/**
 * You tried to fire an event that is located on the auxiliary bit of the Femto OS.
 */
#define  fatIllegalEvent                  0x23

/**
 * The stack of one task has overflown, possible damaging other data.
 */
#define  fatTaskStackOverflowed           0x24

/**
 * The stack of the isr has overflown, possible damaging other data.
 */
#define  fatIsrStackOverflowed            0x25

/**
 * The OS takes more then 50% of the processor time regularly. For equidistant timing model
 * risking immediate timer interrupts after context switch. Note that incidental high loads
 * should not be a problem, for instance if a watchdog barks, Or at heavy interrupt load.
 */
#define  fatOsTickRateTooHigh             0x26

/**
 * A task is too long, or the OS took to long  in the variable timing model, so that it
 * may overflow the subtick timer. This may lead to missed ticks. Increase the time a
 * subtick takes, i.e. lower the tick frequency.
 */
#define  fatTaskSubtickOverflow           0x27

/**
 * A function call used a non existent task number.
 */
#define  fatTaskIllegalTaskNumber         0x28

/**
 * A function tried to store a lock while the slot stack was full
 */
#define  fatSlotStackOverflow             0x29

/**
 * A 'genXXXX' function was used from an isr when while cfgIntOsProtected was not set.
 * This is forbidden, and may lead to rare and hard to trace weird bugs.
 */
#define  fatIllegalCallfromISR            0x2A


/**
 * The size of the stack is under the minimum value, which is currently 4 bytes.
 * The task cannot be started.
 */
#define  errTaskStackUnderMinimum         0x01

/**
 * The size of the stack is under the minimum required to save the context
 * The task cannot be started.
 */
#define  errTaskStackSetupTooSmall        0x02

/**
 * The stack of the task will become too small during execution. Femto OS calculated
 * it will not be possible to save the context on the particular moment it tried to
 * do so. That stack has not really overflown, but the task cannot be restarted. Other
 * tasks should be fine.
 */
#define  errTaskStackWillOverflow         0x03

/**
 * A register is being used which is not saved on the context. This might be an incorrect
 * error. Femto OS cannot determine with 100% certainty that a modified register is actually
 * used, it assumes it is. If you are absolutely certain the register is not used, exclude
 * it from the register check. The slot bits are used to report the highest register block in
 * violation numbered from 0: r0r1r2r3 up to 7: r28r29r30r31.
 */
#define  errTaskIllegalRegisterUse        0x04

/**
 * The task might take so long that subtick overflow may occur, leading to missed ticks.
 * Reduce the time slice for that particular task.
 *
 */
#define  errTaskTakesTooLong              0x05

/**
 * Overflow of the number of nesting levels of one of the critical sections. The number
 * is a power of two minus one depending on the number of bits reserved for the counting.
 * Counting is per type, being one of global, switch or tick.
 */
#define  errTaskNestingOverflowed         0x06

/**
 * Underflow of the number of nesting levels of one of the critical sections. You tried
 * to exit a critical section while not inside one.
 * Counting is per type, being one of global, switch or tick.
 */
#define  errTaskNestingUnderflowed        0x07

/**
 * Unused error.
 */
#define  errUnused_0x08                   0x08

/**
 * Unused error.
 */
#define  errUnused_0x09                   0x09

/**
 * Trying to call an synchronization primitive on a task that was not configured
 * not to hold any slots (Slotsize == 0). This is an error.
 */
#define  errTaskHoldsNoSlots              0x0A

/**
 * On Waits double slots are never allowed. The same applies for mutexes and queu's
 * if cfgSyncDoubleBlock is no activated. This generates this error.
 */
#define  errSlotDoubleUsed                0x0B

/**
 * Trying to write data on an already full queu. This is not an error in strict sense,
 * since bytes put on a full queu are simply ignored. However, you asked to be noted
 * when this situation occurs. This error occurred inside a task (not an isr)
 */
#define  errQueuOverrun                   0x0C

/**
 * Trying to read data from an already empty queu. This is not an error in strict sense,
 * since bytes put on a full queu are simply ignored. However, you asked to be noted
 * when this situation occurs. This error occurred inside a task (not an isr)
 */
#define  errQueuUnderrun                  0x0D

/**
 * Tried to set the priority to a value above 7
 */
#define  errTaskPriorityToHigh            0x0E

/**
 * The delay may not be any longer as FF00 after addition. Internally a to high delay
 * time arose.
 */
#define  errTaskDelayTooLong              0x0F

/**
 *  In a delayFromWake the task is not de-activated, because the wake time lies before the current time.
 */
#define  errTaskDelayTooShort             0x10

/**
 *  Slot with number zero is being used, this is forbidden.
 */
#define  errSlotZeroUsed                  0x11

/**
 * Slot type mismatch, a function tried to use a slot for an other
 * purpose as it was defined.
 */
#define  errSlotTypeMismatch              0x12

/**
 * Twin slots. You cannot ask a double lock on two identical slot numbers.
 */
#define  errSlotTwins                     0x13

/**
 * Trying to write more data on a queu than the size of that queu. This call can never
 * succeed and is therefore an error.
 */
#define  errQueuWriteLimit                0x15

/**
 * Trying to read more data from a queu than the size of that queu. This call can never
 * succeed and is therefore an error.
 */
#define  errQueuReadLimit                 0x16

/**
 * Trying to remove a slot not present on the slot stack.
 * before u use the next.
 */
#define  errSlotDecreaseFail              0x17

/**
 * Trying to put more slots on the stack than room for.
 */
#define  errSlotIncreaseFail              0x18

/**
 * Trying to open a file that is already open, to write in read-only mode, to read,
 * write or close a file that is not open. Or you tried to put a task to sleep
 * with a open file in write mode.
 */
#define  errFileOpenMode                  0x19

/**
 * Trying operations on a file which does not exist.
 */
#define  errFileInvalidFileNumber         0x1A

/**
 * Trying to read or write outside the file.
 */
#define  errFileOutOfRange                0x1B

/**
 * Trying to perform an operation in a task which does not have sufficient
 * capabilities to act. The info number represents the failing capability.
 * cfgCapWatchdog         1
 * cfgCapEvent            2
 * cfgCapCritical         3
 * cfgCapSynchronization  4
 * cfgCapFileSystem       5
 * cfgCapDelay            6
 * cfgCapTimeout          7
 */
#define  errInsufficientCapabilities      0x1C


/**
 * General trace events. Use these in your trace application
 */
#define  traceNull                                    0x00  /* Nullbyte is not used, and may used as separation byte.           */
#define  traceMarker                                  0x01  /* Mark a special point in the tracing.                             */
#define  traceTaskStart                               0x10  /* switched to the task number 0x00 - 0x0F by 0x10 - 0x1F           */
#define  traceOsStart                                 0x20  /* switched to the OS                                               */
#define  traceIdleStart                               0x21  /* switched to the idle task                                        */
#define  traceSleepStart                              0x22  /* switched to low power sleep                                      */
#define  traceTickInterrupt                           0x23  /* a tick interrupt has occurred                                    */
#define  traceWatchdog                                0x24  /* the watchdog barked, see following TaskInit for number           */
#define  traceLockAcquire                             0x25  /* the slot is free and the task may continue                       */
#define  traceLockReject                              0x26  /* the slot has been taken and the task will be blocked             */
#define  traceReset                                   0x27  /* a reset occured                                                  */
#define  traceISR                                     0x28  /* switched to one of 8 interrupts 0x28 -- 0x2F, define yourself.   */
#define  traceSlotLock                                0x30  /* lock a slot, 2 bytes:   (0x30 | tasknumber), (slot1<<4 | slot2)  */
#define  traceSlotUnlock                              0x40  /* unlock a slot, 2 bytes: (0x40 | tasknumber), (slot1<<4 | slot2)  */
#define  traceFireEvent                               0x50  /* Fire of event number 0..7 given by 0x50 ... 0.57                 */
#define  traceREGISTERLEAK_5A                         0x5A  /* If this event occurs, you probably have a register leak          */
#define  traceBurnLock                                0x5B  /* File system not ready for burning                                */
#define  traceBurnFree                                0x5C  /* File system ready for burning                                    */
#define  traceTaskInit                                0x60  /* Initialized task number 0x00 - 0x0F by 0x60 - 0x6F               */
#define  traceTaskRelease                             0x70  /* A blocked (Sync,File,Event) task may continue 0x70..0x7F         */
#define  traceAPIcall0                                0x80  /* 0x80 -- 0xBF api called (64 functions's)                         */
#define  traceAPIcall1                                0x90  /* See below for definitions                                        */
#define  traceAPIcall2                                0xA0  /*                                                                  */
#define  traceAPIcall3                                0xB0  /*                                                                  */
#define  traceStandardError1                          0xC0  /* 0xD0 -- 0xDF standard error number 00-0F                         */
#define  traceStandardError2                          0xD0  /* 0xE0 -- 0xDF standard error number 10-1F                         */
#define  traceFatalError                              0xE0  /* 0xE0 -- 0xEF fatal error number 00-0F                            */
#define  traceUserCatch                               0xF0  /* 0xF0 -- 0xFF reserved for the user                               */
#define  traceUserByteMessage                         0xF1  /* Used to pass a payload information byte                          */

/**
 * API trace events. Use these to see which method has been called.
 */
#define  traceAPIcallTaskYield                        0x80
#define  traceAPIcallTaskDelayFromNow                 0x81
#define  traceAPIcallTaskDelayFromWake                0x82
#define  traceAPIcallTaskSuspendMe                    0x83
#define  traceAPIcallTaskSleep                        0x84
#define  traceAPIcallTaskSleepAll                     0x85

#define  traceAPIcallTaskFeedWatchdog                 0x87
#define  traceAPIcallTaskKillWatchdog                 0x88
#define  traceAPIcallTaskRestart                      0x89
#define  traceAPIcallTaskKillTask                     0x8A
#define  traceAPIcallTaskWaitForTasks                 0x8B
#define  traceAPIcallTaskSyncRequest                  0x8C
#define  traceAPIcallTaskSyncRelease                  0x8D
#define  traceAPIcallTaskEnableGlobalInterrupts       0x8E
#define  traceAPIcallTaskDisableGlobalInterrupts      0x8F
#define  traceAPIcallTaskEnableSwitchTask             0x90
#define  traceAPIcallTaskDisableSwitchTask            0x91
#define  traceAPIcallTaskEnterGlobalCritical          0x92
#define  traceAPIcallTaskExitGlobalCritical           0x93
#define  traceAPIcallTaskEnterSwitchCritical          0x94
#define  traceAPIcallTaskExitSwitchCritical           0x95
#define  traceAPIcallTaskWaitForEvent                 0x96
#define  traceAPIcallTaskFileOpen                     0x97
#define  traceAPIcallTaskFileClose                    0x98
#define  traceAPIcallTaskFileFormat                   0x99
#define  traceAPIcallTaskFileGetSize                  0x9A
#define  traceAPIcallTaskFileSetSize                  0x9B
#define  traceAPIcallTaskFileReadByte                 0x9C
#define  traceAPIcallTaskFileWriteByte                0x9D
#define  traceAPIcallTaskFileAppendByte               0x9E
#define  traceAPIcallTaskFileReadPipe                 0x9F
#define  traceAPIcallTaskFileWritePipe                0xA0
#define  traceAPIcallTaskFileReadBuffer               0xA1
#define  traceAPIcallTaskFileWriteBuffer              0xA2

#define  traceREGISTERLEAK_A5                         0xA5

#define  traceAPIcallGenSuspend                       0xA8
#define  traceAPIcallGenResume                        0xA9
#define  traceAPIcallGenSetPriority                   0xAA
#define  traceAPIcallGenGetPriority                   0xAB
#define  traceAPIcallGenGetTickCount                  0xAC
#define  traceAPIcallGenGetLastWakeTime               0xAD
#define  traceAPIcallGenGetTasknameChar               0xAE
#define  traceAPIcallGenFireEventSet                  0xAF
#define  traceAPIcallGenReboot                        0xB0
#define  traceAPIcallGenLogTask                       0xB1
#define  traceAPIcallGenLogOs                         0xB2
#define  traceAPIcallGenWaitRelease                   0xB3
#define  traceAPIcallGenQueuWrite                     0xB4
#define  traceAPIcallGenQueuRead                      0xB5
#define  traceAPIcallGenQueuPeek                      0xB6
#define  traceAPIcallGenQueuReadable                  0xB7
#define  traceAPIcallGenQueuWriteable                 0xB8
#define  traceAPIcallGenQueuFull                      0xB9
#define  traceAPIcallGenQueuEmpty                     0xBA
#define  traceAPIcallGenQueuClear                     0xBB
#define  traceAPIcallGenAddtoTickCount                0xBC
#define  traceAPIcallGenPassFlashString               0xBD

#define  traceAPIcallGenCountEventBlocks              0xBE


/**
 * Communication bytes. Use this in for example a shell. They take less space than
 * full strings. In you shell app on a pc you can translate them to string
 * messages. All request/response bytes have predefined length for the associated
 * data streams.
 */
#define  defResponseOK                                0x01  /* datalength: 0   */
#define  defResponseUnknownCommand                    0x02  /* datalength: 0   */
#define  defResponseError                             0x03  /* datalength: 0   */
#define  defResponseInvalidTask                       0x04  /* datalength: 0   */
#define  defResponseInvalidPriority                   0x05  /* datalength: 0   */
#define  defResponseFemtoOs                           0x10  /* datalength: 12  */
#define  defResponseLogTask                           0x11  /* datalength: 16  */
#define  defResponseLogOs                             0x12  /* datalength: 10  */
#define  defResponseTickCount                         0x13  /* datalength: 2   */
#define  defResponseDump                              0x14  /* datalength: 2   */
#define  defResponseUptime                            0x15  /* datalength: 5   */
#define  defResponseFile                              0x16  /* datalength: x   */
#define  defResponseInvalidFileNumber                 0x17  /* datalength: x   */

#define  defRequestFemtoOs                            0x90  /* datalength: 0   */
#define  defRequestProcessList                        0x91  /* datalength: 0   */
#define  defRequestTickCount                          0x93  /* datalength: 0   */
#define  defRequestReboot                             0x94  /* datalength: 0   */
#define  defRequestSuspend                            0x95  /* datalength: 1   */
#define  defRequestResume                             0x96  /* datalength: 1   */
#define  defRequestKill                               0x97  /* datalength: 1   */
#define  defRequestPriority                           0x98  /* datalength: 2   */
#define  defRequestRestart                            0x99  /* datalength: 2   */
#define  defRequestDump                               0x9A  /* datalength: 2   */
#define  defRequestStore                              0x9B  /* datalength: x   */
#define  defRequestRecall                             0x9C  /* datalength: x   */
#define  defRequestUptime                             0x9D  /* datalength: 0   */
#define  defRequestTest                               0x9E  /* datalength: 1  */

#define defDumpRAM                                    0x01
#define defDumpEEPROM                                 0x02
#define defDumpFLASH                                  0x03

#endif /* FEMTOOS_GLOBALS_H */
