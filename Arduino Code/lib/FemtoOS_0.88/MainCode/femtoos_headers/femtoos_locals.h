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

#ifndef FEMTOOS_LOCALS_H
#define FEMTOOS_LOCALS_H


/* The context save makes use of the OS to store the background variables. In order to
 * facilitate this, and simplify code over there, we define here the addresses needed.
 * This must be done for the OS stack as well as the ISR stack. Of course these definitions
 * depend on the direction of the stack growth.
 * Be ware, the endianness is relevant too. These values are for little endian encoding,
 * and are to be adjusted when we have big endian encoding. I have no big endian hardware
 * to test, so it makes little sense to add that much untested code. In fact i don't even
 * have hardware with cfgSysStackGrowthUp so that code must be faulty too.
 * Depending on the device the size of an address can be two or three bytes large. We must
 * distinguish between the two. FirstByte,SecondByte and ThirdByte refer to the bytes
 * as they are peeled of one stack to store on an other. StackInit refers to the absolute
 * beginning of the stack, whereas StactStart provides the location of the new stackpointer,
 * after the address have been transplanted (place of the stack pointer upon return to the OS.)
 * In case there is only a two byte program counter the 'ThirdByte' macros are not used.
 */

#if (cfgSysStackGrowthUp == cfgTrue)
  #define OSstackFirstByte             (1 + defThreeByteStackAdd)
  #define OSstackSecondByte            (0 + defThreeByteStackAdd)
  #define OSstackThirdByte             (0)
  #define OSstackStart                 (2 + defThreeByteStackAdd)
  #define OSstackInit                  (0)
#else
  #define OSstackFirstByte             (StackSizeOS - 2 - defThreeByteStackAdd)
  #define OSstackSecondByte            (StackSizeOS - 1 - defThreeByteStackAdd)
  #define OSstackThirdByte             (StackSizeOS - 1)
  #define OSstackStart                 (StackSizeOS - 3 - defThreeByteStackAdd)
  #define OSstackInit                  (StackSizeOS - 1)
#endif

/* When switching to an ISR we need a place where to temporarily store current
 * stackpointer. We could have defined a special variable for this, but it
 * seems more appropriate to use the ISR stack itself for this purpose. Therefore
 * not all of the ISR stack can be used by the ISR itself.
 * ISRstackStart defines the place of the stack pointer when starting the isr.
 */

#if (cfgSysStackGrowthUp == cfgTrue)
  #define ISRstackSPH                  (0)
  #define ISRstackSPL                  (1)
  #define ISRstackFirstByte            (3 + defThreeByteStackAdd)
  #define ISRstackSecondByte           (2 + defThreeByteStackAdd)
  #define ISRstackThirdByte            (1 + defThreeByteStackAdd)
  #define ISRstackStart                (4 + defThreeByteStackAdd)
  #define ISRstackLastByte             (StackSizeISR-1)
  #define ISRstackInit                 (0)
#else
  #define ISRstackSPH                  (StackSizeISR - 1)
  #define ISRstackSPL                  (StackSizeISR - 2)
  #define ISRstackFirstByte            (StackSizeISR - 4 - defThreeByteStackAdd)
  #define ISRstackSecondByte           (StackSizeISR - 3 - defThreeByteStackAdd)
  #define ISRstackThirdByte            (StackSizeISR - 2 - defThreeByteStackAdd)
  #define ISRstackStart                (StackSizeISR - 5 - defThreeByteStackAdd)
  #define ISRstackLastByte             (0)
  #define ISRstackInit                 (StackSizeISR-1)
#endif


/**
 * Using one switch it is possible to make several methods (particularly
 * those who use a lot of stack space) inline.
 */
#if (cfgSysInline == cfgTrue)
  #define defConditionalInline  __attribute__ ( ( always_inline ) )
#else
  #define defConditionalInline
#endif

/**
 * For sleep time calculations we need to know the tick frequency, This is found
 * from the ClockFrequency (assume it has been correctly defined), the Prescaler
 * value and the number of subticks per tick. The cfgNumSleepThreshold is given
 * in milliseconds so we must calculate how may tickblocks that is, taking into
 * account that it may never exceed defSleepMaximum. A sleepbit is used in the
 * devAuxSysReg to indicate that the device was wkoken by the watchdog interrupt, and
 * should continue its sleep.
 *
 * Exact calculation of the time the Watchdog may sleep based on the requested number
 * of tickblocks that is available for sleep is possible, but requires 16 bit
 * division which is extremely lengthy on the AVR. Therefore we do a lot of rounding
 * and accept less accurate results. See also the documentation on cfgUseLowPowerOnDelay.
 * We keep it down to one 8 bit division, and if you choose your tickfrequency such
 * that the real time of 256 ticks divide the choosen sleeptime of the watchdog, no
 * integer division is needed, but this can be coded as a shift operation.
 * (The compiler does that for you)
 */
#define defTickFrequency              (devClockFrequency / cfgSysClockDivider / cfgSysSubTickDivider/ cfgSysSubTicksPerFullTick )
#define defSleepInfinite              0xFF
#define defSleepMaximum               0xFE
#define defSleepTickBlockTime         (32000 / (defTickFrequency/8))
#define defSleepThreshold             ((((cfgNumSleepThreshold + (defSleepTickBlockTime/2)) / 256) < defSleepInfinite) ? ((cfgNumSleepThreshold + (defSleepTickBlockTime/2)) / 256) : defSleepMaximum)
#define defSleepDivider               ( (((2*defSleepRealPeriod) / defSleepTickBlockTime)+1) / 2 )
/**
 * This holds the absolute minimum of bytes the task stack must contain. In the present setup
 * this number equals 4 because the stack must at least accommodate two return addresses. Although
 * the last address is not stored in the context save, it is temporarily needed.
 */
#define defTaskStackMinimum            4

/**
 *  The number of bytes taken by saving one (return) address and the status register.
 */
#define defTaskStackGround             (3 + defThreeByteStackAdd)

/**
 * This holds the absolute minimum of bytes the OS stack must contain. In the present setup
 * this number equals the size of the background variables structure.
 */
#define defOsStackMinimum              (sizeof(TtaskSave))

/**
 * The maximum of the subtick counter.
 */
#define defSubTicksMax                 0xFF

/**
 * The maximum number of the priorities. The range is 0..7. It is stored
 * in three bits in the task status.
 */
#define defNumberOfPriorities          8

/**
 * defUseIsrStack indicates if a separate stack for the isr is used. This is the case
 * if we have some interrupt routine which is not utilizing the OS stack.
 */
#if (cfgIntUserDefined == cfgTrue) && (((includeIsrBegin == cfgTrue)) || (cfgIntSwitchUsesOSstack == cfgFalse))
  #define defUseIsrStack               cfgTrue
#else
  #define defUseIsrStack               cfgFalse
#endif

/**
 * defNonSwitchingCallPresent indicates if any call is used which does not switch to OS.
 * If so, we must include some code to enter/exit system critical regions. If you add a
 * function to the Femto OS api, make sure you include it in the conditionals.
 * Usually these routines contain privEnterGlobalCritical() or privEnterSysCritical()
 * TODO: This seems not the most maintainable way to solve this
 */
#if ( (includeGenAddtoTickCount == cfgTrue) || \
      (includeGenCountEventBlocks == cfgTrue) || \
      (includeGenFireEvent  == cfgTrue) || \
      (includeGenFireEventSet == cfgTrue) || \
      (includeGenGetLastWakeTime == cfgTrue) || \
      (includeGenGetPriority == cfgTrue) || \
      (includeGenGetTaskname == cfgTrue) || \
      (includeGenGetTickCount == cfgTrue) || \
      (includeGenLogOs == cfgTrue) || \
      (includeGenLogTask == cfgTrue) || \
      (includeGenLogTask == cfgTrue) || \
      (includeGenQueuClear == cfgTrue) || \
      (includeGenQueuEmpty == cfgTrue) || \
      (includeGenQueuFull == cfgTrue) || \
      (includeGenQueuPeek == cfgTrue) || \
      (includeGenQueuRead == cfgTrue) || \
      (includeGenQueuReadable == cfgTrue) || \
      (includeGenQueuWrite == cfgTrue) || \
      (includeGenQueuWriteable == cfgTrue) || \
      (includeGenResume == cfgTrue) || \
      (includeGenSetPriority == cfgTrue) || \
      (includeGenSuspend == cfgTrue) || \
      (includeGenTrace == cfgTrue) || \
      (includeGenWaitRelease == cfgTrue) || \
      (includeTaskFeedWatchdog == cfgTrue) || \
      (includeTaskFileAppendByte == cfgTrue) || \
      (includeTaskFileFormat == cfgTrue) || \
      (includeTaskFileGetSize == cfgTrue) || \
      ((includeTaskFileReadBuffer == cfgTrue) && (cfgCheckTrace == cfgTrue)) || \
      (includeTaskFileReadByte == cfgTrue) || \
      ((includeTaskFileReadPipe == cfgTrue) && (cfgCheckTrace == cfgTrue)) || \
      (includeTaskFileSetSize == cfgTrue) || \
      ((includeTaskFileWriteBuffer == cfgTrue) && (cfgCheckTrace == cfgTrue)) || \
      (includeTaskFileWriteByte == cfgTrue) || \
      ((includeTaskFileWritePipe == cfgTrue) && (cfgCheckTrace == cfgTrue)) || \
      (includeTaskKillWatchdog == cfgTrue) || \
      (includeTaskProtectSwitchCritical == cfgTrue) || \
      (includeTaskProtectSwitchTasks == cfgTrue) )
  #define defNonSwitchingCallPresent   cfgTrue
#else
  #define defNonSwitchingCallPresent   cfgFalse
#endif

/**
 * defStackClean is true if we want to clean or initialize the stack before use. If this
 * indeed is the case we must consider the following:
 * (1) Due to the section clear .bss all stacks are automatically filled by zero
 * (2) Gcc expects r1 to be cleared at all times, and does not check it any further
 * (3) Filling the stack up with other values (than zero) is sensible only for
 *     rigorous watermark checking, but dangerous due to the fact we must have r1 zero.
 *     Thus, make sure defStackInitByte == 0 or know what you are doing.
 * (4) The tasks do not have startup parameters which must be taken into account.
 * (5) If we re-initialize the stack it is not clean and so we cannot rely on .bss and
 *     must clean manually
 */
#if ( (cfgUseTaskWatchdog == cfgTrue) || ( includeTaskRestart == cfgTrue ) || (defStackInitByte != 0x00) )
  #define defStackClean cfgTrue
#else
  #define defStackClean cfgFalse
#endif

/* Just to regulate the return value */
#define defUseBoolReturns cfgUseTimeout

/* Definitions to find the current task */
#define defCurrentTaskMask             0x80



/* Definition of the task status byte
 * 8 bit structure: smbdpppr
 * s:    stopstate   (go/halt)
 * m:    mode:       (suspend/sleep/file/sync)
 * b     blockstate: (blocked/free)
 * d:    delaystate
 * ppp:  priority
 * r:    dressstate  (runable/done, slot/event, read,write)
 *
 * The first three bits can be coupled to the states as follows:
 *
 * 0001 terminated (suspended blocked) never delayed!
 * 001x suspended (suspended unblocked) can be delayed
 * 010x sleeping blocked on sync (but not on fs, this is forbidden) (LSB: slot/event) can be delayed
 * 011x sleeping free can be delayed
 * 100x running blocked on fs (LSB:read/write) , can be delayed
 * 101x running free but otherwise stopped state [unused!]
 * 110x running blocked (LSB: slot/event) , can be delayed
 * 111x running free (LSB: runable/done) , can be delayed
 *
 * The delay bit is orthogonal to these states, and without meaning in the
 * terminated state. A delay in any other blocked state may end and
 * depending on the state may wakeup the state.
 */


/* Definitions of the masks you need to read a specific state */
#define defBaseStopStateGetMask        0x80
#define defBaseModeGetMask             0x40
#define defBaseBlockStateGetMask       0x20
#define defBaseDelayStateGetMask       0x10
#define defBasePrioGetMask             0x0E
#define defBaseDressGetMask            0x01
#define defBaseSuspendedGetMask        0xE0
#define defBaseTerminatedGetMask       0xF0
#define defBaseSleepingGetMask         0xC0
#define defBaseRunningGetMask          0xF0
#define defBaseNoBlocksGetMask         0xE0
#define defBaseSyncBlockedGetMask      0xE0
#define defBaseFileBlockedGetMask      0xE0
#define defBaseUnusedBlockedGetMask    0xE0

/* Definitions of the masks you need to write a specific state */
#define defBaseStopStateSetMask        (~defBaseStopStateGetMask)
#define defBaseModeSetMask             (~defBaseModeGetMask)
#define defBaseBlockStateSetMask       (~defBaseBlockStateGetMask)
#define defBaseDelayStateSetMask       (~defBaseDelayStateGetMask)
#define defBasePrioSetMask             (~defBasePrioGetMask)
#define defBaseDressSetMask            (~defBaseDressGetMask)
#define defBaseSuspendedSetMask        (~defBaseSuspendedGetMask)
#define defBaseTerminatedSetMask       (~defBaseTerminatedGetMask)
#define defBaseSleepingSetMask         (~defBaseSleepingGetMask)
#define defBaseRunningSetMask          (~defBaseRunningGetMask)
#define defBaseNoBlocksSetMask         (~defBaseNoBlocksGetMask)
#define defBaseSyncBlockedSetMask      (~defBaseSyncBlockedGetMask)
#define defBaseFileBlockedSetMask      (~defBaseFileBlockedGetMask)
#define defBaseUnusedBlockedSetMask    (~defBaseUnusedBlockedGetMask)

/* Definitions of the states */
#define defBaseStopStateGo             0x80
#define defBaseStopStateHalt           0x00
#define defBaseModeSuspend             0x00
#define defBaseModeSleep               0x40
#define defBaseModeFile                0x00
#define defBaseModeSync                0x40
#define defBaseBlockStateFree          0x20
#define defBaseBlockStateBlocked       0x00
#define defBaseDelayStateWake          0x10
#define defBaseDelayStateDelayed       0x00
#define defBaseDressRunable            0x01
#define defBaseDressDone               0x00
#define defBaseDressSlot               0x01
#define defBaseDressEvent              0x00
#define defBaseDressRead               0x01
#define defBaseDressWrite              0x00

#define defBaseRunningTask             0xF0
#define defBaseNoBlocksTask            0xE0
#define defBaseSyncBlockedTask         0xC0
#define defBaseFileBlockedTask         0x80
#define defBaseSleepingTask            0x40
#define defBaseSuspendedTask           0x20
#define defBaseTerminatedTask          0x10

/* Definition of the shift values for the numerics */
#define defBasePrioShift               1


/* Definitions for the uiFsStatus byte. The whole state of the Femto File System is
 * kept in one byte. 8 bit structure: rwqbtttt
 * r: read mode, reading is possible, all blocking FS tasks are write request.
 * w: write mode, one task has access, further tasks are blocked.
 * q: request bit, if set, write is requested, no further reads are allowed.
 * b: burn block. As long as this bit is set, writing is prohibited.
 * tttt: number of the task currently writing the FS in case of writemode
 * tttt: number of tasks holding a readlock in case of readmode
 */

/* Definitions for the File System */

#define defFsReadBlockGetMask          0x80
#define defFsWriteBlockGetMask         0x40
#define defFsSleepRequestGetMask       0x20
#define defFsBurnBlockGetMask          0x10
#define defFsWriteNumberGetMask        0x0F
#define defFsReadCountGetMask          0x0F
#define defFsFreeGetMask               (~defFsSleepRequestGetMask)

#define defFsReadBlockSetMask          (~defFsReadBlockGetMask)
#define defFsWriteBlockSetMask         (~defFsWriteBlockGetMask)
#define defFsSleepRequestSetMask       (~defFsSleepRequestGetMask)
#define defFsBurnBlockSetMask          (~defFsBurnBlockGetMask)
#define defFsWriteNumberSetMask        (~defFsWriteNumberGetMask)
#define defFsReadCountSetMask          (~defFsReadCountGetMask)
#define defFsFreeSetMask               (~defFsFreeGetMask)

#define defFsReadBlockActive           0x80
#define defFsReadBlockReleaseRequest   0x80
#define defFsWriteBlockActive          0x40
#define defFsSleepRequestActive        0x20
#define defFsBurnBlockActive           0x10
#define defFsReadBlockClear            0x00
#define defFsWriteBlockClear           0x00
#define defFsSleepRequestClear         0x00
#define defFsBurnBlockClear            0x00
#define defFsFree                      0x00

#define defFsSetReleaseRequest         (defFsReadBlockReleaseRequest | defFsWriteBlockActive | defFsBurnBlockActive)
#define defFsReadInc                   0x01

#define defFsField                     uiTaskMonitor
#define defFsReadGetMask               0x80
#define defFsWriteGetMask              0x40
#define defFsReadSetMask               (~defFsReadGetMask)
#define defFsWriteSetMask              (~defFsWriteGetMask)
#define defFsReadActive                0x80
#define defFsReadClear                 0x00
#define defFsWriteActive               0x40
#define defFsWriteClear                0x00


/* Definition of the use of the delay */

#if (cfgUseDelay == cfgTrue) || ((cfgUseSynchronization != cfgSyncNon) && (cfgUseTimeout == cfgTrue))
  #define defUseDelay                  cfgTrue
#else
  #define defUseDelay                  cfgFalse
#endif

#if ((cfgUseTaskWatchdog == cfgTrue) || (includeTaskRestart == cfgTrue))
  #define defReUseTaskInit             cfgTrue
#else
  #define defReUseTaskInit             cfgFalse
#endif

#if (cfgUseFileSystem == cfgTrue) && ((defReUseTaskInit == cfgTrue) || (cfgUseLowPowerSleep == cfgTrue) || (includeTaskKillTask == cfgTrue) || (cfgCheckMethodUse == cfgTrue))
  #define defUseFsField                cfgTrue
#else
  #define defUseFsField                cfgFalse
#endif

/* Definition of the Task Monitor byte
 * 8 bit structure: rw..wwrr
 * rw:    read,write mode for the filesystem (if needed)
 * ..:    unused
 * ww:    watchdog countdown
 * rr:    return state (if a return is needed, the value true/false)
 * Note: the T status and the return state must be located on the same
 * byte sine they are used to the context save for processing.
 */
#if (defUseFsField == cfgTrue) || (cfgUseTaskWatchdog == cfgTrue) || (defUseBoolReturns == cfgTrue)
  #define defUseTaskMonitor            cfgTrue
#else
  #define defUseTaskMonitor            cfgFalse
#endif


#if (cfgUseSynchronization  != cfgSyncNon)
  #define defUseTaskSlot               cfgTrue
#else
  #define defUseTaskSlot               cfgFalse
#endif


/* Definition of the fields. Note, you cannot Change the field of the SemaField just
 * like that. Since it is part of the slot stack, separately address in the operations
 * on the SlotStack */
#define defDogField                    uiTaskMonitor
#define defRetField                    uiTaskMonitor


/* Definitions of the masks you need to read a specific state.  Note
 * that the position of the Ret bits is not totally free. If changed,
 * also consider the effects upon the xOS.pxSave.uiReturn background
 * variable and coupled assembly statements, and the value of the
 * defParaRetToRetShift parameter.  */
#define defDogGetMask                  0x0C
#define defRetGetMask                  0x03

/* Definitions of the masks you need to write a specific state.*/
#define defRetSetMask                  (~defRetGetMask)
#define defDogSetMask                  (~defDogGetMask)


/* Definitions of the states. Note these are 'counted' or shiftable states,
 * so do not change the order or values. Counted states are states the are
 * changed from one value to an other with a counter. Shiftable states are
 * states that are coupled to other states. These values cannot be changed
 * without detailed inspection of the code. defRetLoop is not used at the
 * moment and will not lead to a returned value.
 * The RetNone value is coupled to 0x03 and not to 0x00 so we can easily
 * use the result directly as output when it must be true (0x01) or false
 * (0x00). */
#define defDogDead                     0x00
#define defDogBark                     0x04
#define defDogWake                     0x08
#define defDogSleep                    0x0C
#define defRetFalse                    0x00
#define defRetTrue                     0x01
#define defRetLoop                     0x02
#define defRetNon                      0x03

/* Definition of the shift values for the numerics */
#define defRetShift                    0
#define defDogShift                    2

/* Definition of the occupied bits for the return value */
#define defRet0                        0
#define defRet1                        1

/* Definition of the count values for the numerics */
#define defDogDec                      0x04


#if (cfgUseNestedCriticals == cfgTrue)
  #define defUseTaskLevels             cfgTrue
#else
  #define defUseTaskLevels             cfgFalse
#endif



/* Definition of the Task Levels byte
 *
 * 8 bit structure: sssttggg (width's are variable)
 * ggg:   counter for levels of global critical sections
 * tt:    counter for levels of tick critical sections
 * sss:   counter for levels of switch critical sections
 */

#if (cfgIntGlobalOnly == cfgTrue)
  #define defNumGlobWidth              (cfgNumGlobWidth + cfgNumTickWidth)
  #define defNumTickWidth              (0)
  #define defNumSwitchWidth            (cfgNumSwitchWidth)
#else
  #define defNumGlobWidth              (cfgNumGlobWidth)
  #define defNumTickWidth              (cfgNumTickWidth)
  #define defNumSwitchWidth            (cfgNumSwitchWidth)
#endif

#define defGlobField                   uiTaskLevels
#define defGlobShift                   (0)
#define defGlobGetMask                 (( (1 << defNumGlobWidth) - 1) << defGlobShift)
#define defGlobSetMask                 (~defGlobGetMask)
#define defGlobInc                     (1 << defGlobShift)
#define defGlobMax                     (defGlobGetMask)
#define defGlobMin                     (0x00)

#define defTickField                   uiTaskLevels
#define defTickShift                   (defNumGlobWidth)
#define defTickGetMask                 (( (1 << defNumTickWidth) - 1) << defTickShift)
#define defTickSetMask                 (~defTickGetMask)
#define defTickInc                     (1 << defTickShift)
#define defTickMax                     (defTickGetMask)
#define defTickMin                     (0x00)

#define defSwitchField                 uiTaskLevels
#define defSwitchShift                 (defNumGlobWidth + defNumTickWidth)
#define defSwitchGetMask               (( (1 << defNumSwitchWidth) - 1) << defSwitchShift)
#define defSwitchSetMask               (~defSwitchGetMask)
#define defSwitchInc                   (1 << defSwitchShift)
#define defSwitchMax                   (defSwitchGetMask)
#define defSwitchMin                   (0x00)



/* Initializer for the Task Monitor field */
#if (defUseBoolReturns == cfgTrue)
  #define defTaskMonitorInit           ( defFsReadClear | defFsWriteClear | defDogDead | defRetNon )
#else
  #define defTaskMonitorInit           ( defFsReadClear | defFsWriteClear | defDogDead )
#endif

/* Initializer for the Task levels field */
#define defTaskLevelsInit              ( defGlobMin | defSwitchMin )


/**
 * Defined if we make use of queus.
 */
#if (defined(SN_01) && (SlotUse(SN_01) == cfgUseAsQueu)) || (defined(SN_02) && (SlotUse(SN_02) == cfgUseAsQueu)) || (defined(SN_03) && (SlotUse(SN_03) == cfgUseAsQueu)) || (defined(SN_04) && (SlotUse(SN_04) == cfgUseAsQueu)) || (defined(SN_05) && (SlotUse(SN_05) == cfgUseAsQueu)) || (defined(SN_06) && (SlotUse(SN_06) == cfgUseAsQueu)) || (defined(SN_07) && (SlotUse(SN_07) == cfgUseAsQueu)) || (defined(SN_08) && (SlotUse(SN_08) == cfgUseAsQueu)) || (defined(SN_09) && (SlotUse(SN_09) == cfgUseAsQueu)) || (defined(SN_10) && (SlotUse(SN_10) == cfgUseAsQueu)) || (defined(SN_11) && (SlotUse(SN_11) == cfgUseAsQueu)) || (defined(SN_12) && (SlotUse(SN_12) == cfgUseAsQueu)) || (defined(SN_13) && (SlotUse(SN_13) == cfgUseAsQueu)) || (defined(SN_14) && (SlotUse(SN_14) == cfgUseAsQueu)) || (defined(SN_15) && (SlotUse(SN_15) == cfgUseAsQueu))
  #define defUseQueus                  cfgTrue
#else
  #define defUseQueus                  cfgFalse
#endif

/**
 * Defined if we make use of mutexes.
 */
#if (defined(SN_01) && (SlotUse(SN_01) == cfgUseAsMutex)) || (defined(SN_02) && (SlotUse(SN_02) == cfgUseAsMutex)) || (defined(SN_03) && (SlotUse(SN_03) == cfgUseAsMutex)) || (defined(SN_04) && (SlotUse(SN_04) == cfgUseAsMutex)) || (defined(SN_05) && (SlotUse(SN_05) == cfgUseAsMutex)) || (defined(SN_06) && (SlotUse(SN_06) == cfgUseAsMutex)) || (defined(SN_07) && (SlotUse(SN_07) == cfgUseAsMutex)) || (defined(SN_08) && (SlotUse(SN_08) == cfgUseAsMutex)) || (defined(SN_09) && (SlotUse(SN_09) == cfgUseAsMutex)) || (defined(SN_10) && (SlotUse(SN_10) == cfgUseAsMutex)) || (defined(SN_11) && (SlotUse(SN_11) == cfgUseAsMutex)) || (defined(SN_12) && (SlotUse(SN_12) == cfgUseAsMutex)) || (defined(SN_13) && (SlotUse(SN_13) == cfgUseAsMutex)) || (defined(SN_14) && (SlotUse(SN_14) == cfgUseAsMutex)) || (defined(SN_15) && (SlotUse(SN_15) == cfgUseAsMutex))
  #define defUseMutexes                cfgTrue
#else
  #define defUseMutexes                cfgFalse
#endif

/**
 * Defined if we make use of mutexes.
 */
#if (defined(SN_01) && (SlotUse(SN_01) == cfgUseAsWait)) || (defined(SN_02) && (SlotUse(SN_02) == cfgUseAsWait)) || (defined(SN_03) && (SlotUse(SN_03) == cfgUseAsWait)) || (defined(SN_04) && (SlotUse(SN_04) == cfgUseAsWait)) || (defined(SN_05) && (SlotUse(SN_05) == cfgUseAsWait)) || (defined(SN_06) && (SlotUse(SN_06) == cfgUseAsWait)) || (defined(SN_07) && (SlotUse(SN_07) == cfgUseAsWait)) || (defined(SN_08) && (SlotUse(SN_08) == cfgUseAsWait)) || (defined(SN_09) && (SlotUse(SN_09) == cfgUseAsWait)) || (defined(SN_10) && (SlotUse(SN_10) == cfgUseAsWait)) || (defined(SN_11) && (SlotUse(SN_11) == cfgUseAsWait)) || (defined(SN_12) && (SlotUse(SN_12) == cfgUseAsWait)) || (defined(SN_13) && (SlotUse(SN_13) == cfgUseAsWait)) || (defined(SN_14) && (SlotUse(SN_14) == cfgUseAsWait)) || (defined(SN_15) && (SlotUse(SN_15) == cfgUseAsWait))
  #define defUseWaits                 cfgTrue
#else
  #define defUseWaits                 cfgFalse
#endif

/**
 * Defined if we make use of some facility that reports an error.
 */
#if (cfgCheckOsStack == cfgTrue) || (cfgCheckTaskStack == cfgTrue) || (cfgCheckIsrStack == cfgTrue) || (cfgCheckRegisters == cfgTrue) || (cfgCheckTiming == cfgTrue) || (cfgCheckQueuFilling == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
  #define defCheckReportingError       cfgTrue
#else
  #define defCheckReportingError       cfgFalse
#endif


/**
 * Define how the CPU status must be setup.
 */
#define defInitCPUStatusRegister   (portInitCPUStatusRegister & ~((1 << portInitModeInterruptLoc) | (1 << portInitGlobalInterruptLoc) | (1 << portInitTickInterruptLoc)) )

/* Definitions for the Queu subfield. Note that the Write- and ReadEmpty bytes are, and
 * must be, 0x00. This is because upon initial use we want to make use of the .bss cleaning,
 * to save code. The values of the StateFull/StateNotFull must be defined accordingly.
 * A special state for the empty queu cannot be defined if we demand that there is no
 * initialization before first use of the queu mechanism. Note that for emptying a queu
 * it suffices to write the Empty values to the queu filling variable. */
#define defQueuFillingWriteEmpty       0x00
#define defQueuFillingReadEmpty        0x00
#define defQueuFillingGetMask          0x80
#define defQueuFillingSetMask          (~defQueuFillingGetMask)
#define defQueuFillingStateFull        0x80
#define defQueuFillingStateNotFull     0x00


/* Definitions for the uiOsStatus byte. The whole state of the Femto OS is kept in one byte.
 * 8 bit structure: ccnstttt
 * cc:   context state of the OS: This indicates in which state the OS is at the moment.
 * n:    presence of a near wake: one or more tasks have to waked with 256 ticks.
 * s:    switch block,  if set no task switch can be performed.
 * tttt: number of the current task: Number of the task that is currently running.
 */

/* Definitions of the masks you need to read a specific state */
#define defContextGetMask              0xC0
#define defNearWakePresentGetMask      0x20
#define defSwitchAllowGetMask          0x10
#define defTaskNumberGetMask           0x0F

/* Definitions of the masks you need to write a specific state */
#define defContextSetMask              (~defContextGetMask)
#define defNearWakePresentSetMask      (~defNearWakePresentGetMask)
#define defSwitchAllowSetMask          (~defSwitchAllowGetMask)
#define defTaskNumberSetMask           (~defTaskNumberGetMask)

/* Definition for the context state determination.
 * We defined two bits to mark in which situation the system is. That can be sleeping,
 * isr, idle, task or OS. These are five states, but since we can never mixup a
 * sleep state with an isr state we may choose the same id for both states, and thus
 * save a one bit. So this can be coded using 2 bits.
 * In the portSaveContext the defContextStateSaveBit definition is used to test the
 * particular bit. If it is set, the saveContext itself is skipped, since it is assumed
 * we come from an idle or sleep state. Make sure this definition relates to the other
 * defContext definitions.
 * Note you cannot just change these definitions to you liking, assembly code has to
 * be adapted as well.
 */
#define defContextStateSaveBit         7
#define defContextStateSleep           0xC0
#define defContextStateIsr             0xC0
#define defContextStateIdle            0x80
#define defContextStateTask            0x40
#define defContextStateOs              0x00

/* Definitions for the nearwake and switch state. */
#define defNearWakeStatePresent        0x20
#define defNearWakeStateAbsent         0x00
#define defSwitchStateBlocked          0x10
#define defSwitchStateRunning          0x00

/* Definition of the shift values for the numerics */
#define defOsTaskNumberShift           0
#define defFsTaskNumberShift           0
#define defFsCounterShift              0

/* These are results mask for the privSwitchContext() method. Make sure all defs are
 * bitwise unique. It tells the Femto OS what action to take, and if the Watchdog
 * has barked.
 */
#define defAssignmentSleep             0x80
#define defAssignmentIdle              0x40
#define defAssignmentTask              0x00
#define defAssignmentWatchdog          0x10
#define defAssignmentAbsent            0x00

/* Definitions for parameter passing in the synchronization sections. Note that the
 * SyncRetState patterns are coupled to the defRetXXX bit patterns. Do not alter just
 * like that. Msb is still free for use.*/
#define defParaLockMask                0x40
#define defParaLockStateUnlock         0x40
#define defParaLockStateKeep           0x00
#define defParaRetMask                 0x30
#define defParaRetStateNon             0x30
#define defParaRetStateLoop            0x20
#define defParaRetStateTrue            0x10
#define defParaRetStateFalse           0x00
#define defParaRetToRetShift           4

/* Definitions below are used by the preprocessor to convert configuration constants
 * into the task status byte of slot depth byte initial definition. Bit 4 is still
 * free for other startup information. */
#define defInterruptStartMask          0x11
#define defInterruptAbsent             0x00
#define defStopStartMask               0xE0
#define defStopAbsent                  0x00
#define defSlotDepthMask               0x07
#define defSlotDepthAbsent             0x00

/* In privOperateSlotStack we make use control bits derived from the task status.
 * Referencing is done only to make sure we have no bit collisions, and does
 * not have a further meaning, except for the block state (defSlotLoopBlockMask)
 * which is internally derived in privOperateSlotStack*/
#define defSlotLoopBlockMask           defBaseBlockStateGetMask
#define defSlotLoopSearchMask          defBaseDelayStateGetMask
#define defSlotLoopBlockBlocked        defBaseBlockStateBlocked
#define defSlotLoopBlockNon            defBaseBlockStateFree
#define defSlotLoopSearchActive        defBaseDelayStateWake
#define defSlotLoopSearchNon           defBaseDelayStateDelayed

/* Operations below must occupy mutual exclusive bits */
#define defSlotOperateIncreaseMask     0x10
#define defSlotOperateDecreaseMask     0x20
#define defSlotOperateSQMMask          0x40
#define defSlotOperateSFreeMask        0x80
#define defSlotOperateIncreaseAction   0x10
#define defSlotOperateDecreaseAction   0x20
#define defSlotOperateNoAction         0x00
#define defSlotOperateSearchDefault    0x00
#define defSlotOperateQMAbsent         0x40
#define defSlotOperateSearchFree       0x80

/* There are two slots per byte */
#define defSlotLeftGetMask             0xF0
#define defSlotRightGetMask            0x0F
#define defSlotLeftSetMask             (~defSlotLeftGetMask)
#define defSlotRightSetMask            (~defSlotRightGetMask)
#define defSlotLeftShift               4
#define defSlotRightShift              0

/* This is the free slot */
#define defSlotFree                    0x00
#define defSlotLeftFree                0x00
#define defSlotRightFree               0x00

/* Definitions for the handling of the checks on the stack
 * (avr specific, thought through the portable section usable for other
 * purposes. These cannot be changed since their structure is 'hardcoded'
 * in the portable sections of the code. Check before you change! */
#define defCheckStackBit               7
#define defCheckStackGetMask           0x80
#define defCheckStackStateError        0x80
#define defCheckStackStateOK           0x00

/* Definitions for switching and ticking */
#define defActionNone                  0x00
#define defActionTaskGetMask           0x01
#define defActionTickGetMask           0x02
#define defActionTaskStateSwitch       0x01
#define defActionTaskStateKeep         0x00
#define defActionTickStateTick         0x02
#define defActionTickStateNone         0x00

/* Definition used to check slot numbers */
#define defCheckSlotTypeError          0xF0

/* Definition to fire all events */
#define defAllEventsSet                0xFF
#define defAllEventsReset              0x00


/* Definitions for initialization */
#define defInitLockMask                0x01
#define defInitStatusMask              0x02
#define defInitWatermarksMask          0x04
#define defInitLockRelease             0x01
#define defInitLockKeep                0x00
#define defInitStatusRenew             0x02
#define defInitStatusKeep              0x00
#define defInitWatermarksRenew         0x04
#define defInitWatermarksKeep          0x00

/* Definitions for the error handling */
#define errSignLeft                    0x80
#define errSignRight                   0x40
#define errTaskStateCurrent            0x80
#define errTaskStateNon                0x00
#define errTaskStateInfo               0x00

#define errOsStateSwitch               0x40
#define errOsStateReturn               0x00
#define errOsMask                      0x40

#define errFatalError                  0x20
#define errNoInfo                      0x00
#define errCurrentTask                 0x00
#define errTaskNumberShift             0
#define errInfoNumberShift             4
#define errTraceErrorBase              0xC0
#define errTaskMask                    0x80
#define errMessageMask                 0x3F
#define errTaskNumberGetMask           0x0F
#define errInfoNumberGetMask           0xF0
#define errSignGetMask                 0xC0
#define errTaskNumberSetMask           (~errTaskNumberGetMask)
#define errInfoNumberSetMask           (~errInfoNumberGetMask)
#define errSignSetMask                 (~errSignGetMask)


/* All system internal calls on operation on the interrupt status
 * are routed trough privXXX. The mapping is given below, and depends
 * on the setting of the configuration parameters. */

#define privEnableGlobalInterrupts()       portEnableGlobalInterrupts()
#define privDisableGlobalInterrupts()      portDisableGlobalInterrupts()
#define privEnableTickInterrupts()         portEnableTickInterrupts()
#define privDisableTickInterrupts()        portDisableTickInterrupts()
#define privEnterGlobalCritical()          portEnterGlobalInterrupts()
#define privExitGlobalCritical()           portExitGlobalInterrupts()

#if (cfgIntGlobalOnly == cfgTrue)
  #define privEnterSysCritical()           portEnterGlobalInterrupts()
  #define privExitSysCritical()            portExitGlobalInterrupts()
#elif (cfgIntOsProtected == cfgTrue)
  #define privEnterSysCritical()           portEnterGlobalInterrupts()
  #define privExitSysCritical()            portExitGlobalInterrupts()
#else
  #define privEnterSysCritical()           portEnterTickInterrupts()
  #define privExitSysCritical()            portExitTickInterrupts()
#endif


/* Re-routing for event tracking */
#if (cfgCheckTrace == cfgTrue)
  #define privTrace(Event)                  portTrace(Event)
  #define privShowReset()                   do { portTrace(eventReset); portShowReset(); } while(false)
#else
  #define privTrace(Event)
  #define privShowReset()                   portShowReset()
#endif


/* Test if all stacks fit on page zero */
#if ((defStackSpaceTotal + devRAMbegin)> 0xFF) || (defSysZeroPageStack != cfgTrue)
  #define defStackSpaceExceedsOneByte cfgTrue
#else
  #define defStackSpaceExceedsOneByte cfgFalse
#endif

/* Stack manipulation definitions. */
#if (defStackSpaceExceedsOneByte == cfgTrue)
  #define privInitStack portSetFullStack
  #define privSetStack  portSetFullStack
  #define privGetStack  portGetFullStack
#else
  #define privInitStack portSetFullStack
  #define privSetStack  portSetReducedStack
  #define privGetStack  portGetReducedStack
#endif


#endif /* FEMTOOS_LOCALS_H */
