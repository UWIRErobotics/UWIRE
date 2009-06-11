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

#include "femtoos_core.h"
#include "femtoos_shared.h"


/* ========================================================================= */
/* FEMTO OS INTERNAL FUNCTIONS  ============================================ */
/* ========================================================================= */

/*
 * Body definitions. All switching methods are called via an assembler
 * redirect, making it possible to switch stack, save context and preserve
 * all registers (including the parameters) while doing so. Without this
 * trick gcc may push the parameter registers or construct a frame pointer
 * first thereby destroying other registers. Notice functions never return,
 * but are not 'noreturn'. They are naked or bikini.
 */

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Use this function to delay your task for the given number of ticks.
 * The delay time is measured from the point you call this function, which
 * does not make it a very reliable clock.
 */
#if (includeTaskDelayFromNow == cfgTrue)
  static void privDelayFromNowBody(Tuint16 uiTicksToWait) __attribute__((used)) defSysReduceProEpilogue;
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Use this function to delay your task for the given number of ticks.
 * The delay time is measured from the point the task was last woken. This
 * can be used to generate a fixed frequency, depending of the timing model
 * and other parameters used. See the discussion at cfgUseEquidistantTicks
 * and cfgUseCorrectWakeupTimes.
 */
#if (includeTaskDelayFromWake == cfgTrue)
  static void privDelayFromWakeBody(Tuint16 uiTicksToWait) __attribute__((used)) defSysReduceProEpilogue;
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Body function for taskRestart. See and use that function.
 */
#if (includeTaskRestart == cfgTrue)
  static void privRestartBody(Tuint08 uiTaskNumber) __attribute__((used)) defSysReduceProEpilogue;
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Body function for taskYield. See and use that function.
 */
#if (includeTaskYield == cfgTrue)
  static void privYieldBody(void) __attribute__((used)) defSysReduceProEpilogue;
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Body function for taskKillTask. See and use that function.
 */
#if (includeTaskKillTask == cfgTrue)
  static void privKillTaskBody(Tuint08 uiTaskNumber) __attribute__((used)) defSysReduceProEpilogue;
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Body function for taskSuspendMe. See and use that function.
 */
#if (includeTaskSuspendMe == cfgTrue)
  static void privSuspendMeBody(void) __attribute__((used)) defSysReduceProEpilogue;
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Body function for taskSleep. See and use that function.
 */
#if (cfgUseLowPowerSleep == cfgTrue) && (includeTaskSleep == cfgTrue)
  static void privSleepBody(void) __attribute__((used)) defSysReduceProEpilogue;
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Body function for taskSleepAll. See and use that function.
 */
#if (cfgUseLowPowerSleep == cfgTrue) && (includeTaskSleepAll == cfgTrue)
  static void privSleepAllBody(void) __attribute__((used)) defSysReduceProEpilogue;
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Body function for taskWaitForTasks. See and use that function.
 */
#if (cfgUseSynchronization != cfgSyncNon) && (includeTaskWaitForTasks == cfgTrue)
  #if (cfgUseTimeout == cfgTrue)
    static void privWaitForTasksBody(Tuint08 uiSlot, Tuint08 uiNumberOfTasks, Tuint16 uiTicksToWait) __attribute__((used)) defSysReduceProEpilogue;
  #else
    static void privWaitForTasksBody(Tuint08 uiSlot, Tuint08 uiNumberOfTasks) __attribute__((used)) defSysReduceProEpilogue;
  #endif
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Body function for taskSyncRequest See and use that function.
 */
#if (cfgUseSynchronization != cfgSyncNon) && ( (includeTaskQueu == cfgTrue) || (includeTaskMutex == cfgTrue) )
  #if (cfgUseTimeout == cfgTrue)
    #if (cfgUseSynchronization == cfgSyncDoubleBlock)
      static void privSyncRequestBody(Tuint08 uiSlotSlot, Tsint08 siFreeLeftFilling, Tsint08 siFreeRightFilling, Tuint16 uiTicksToWait) __attribute__((used)) defSysReduceProEpilogue;
    #else
      static void privSyncRequestBody(Tuint08 uiSlotSlot, Tsint08 siFreeRightFilling, Tuint16 uiTicksToWait) __attribute__((used)) defSysReduceProEpilogue;
    #endif
  #else
    #if (cfgUseSynchronization == cfgSyncDoubleBlock)
      static void privSyncRequestBody(Tuint08 uiSlotSlot, Tsint08 siFreeLeftFilling, Tsint08 siFreeRightFilling) __attribute__((used)) defSysReduceProEpilogue;
    #else
      static void privSyncRequestBody(Tuint08 uiSlotSlot, Tsint08 siFreeRightFilling) __attribute__((used)) defSysReduceProEpilogue;
    #endif
  #endif
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Body function for taskSyncRelease See and use that function.
 */
#if (cfgUseSynchronization != cfgSyncNon) && ( (includeTaskQueu == cfgTrue) || (includeTaskMutex == cfgTrue) )
  static void privSyncReleaseBody(Tuint08 uiSlotSlot) __attribute__((used)) defSysReduceProEpilogue;
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Body function for taskFileOpen See and use that function.
 */
#if (cfgUseFileSystem  ==  cfgTrue) && (includeTaskFileAccess == cfgTrue)
  #if (cfgUseTimeout == cfgTrue)
    #if (cfgUseFileSystemConcurrentRead == cfgTrue)
      static void privFileOpenBody(Tbool bReadOnly, Tuint16 uiTicksToWait) __attribute__((used)) defSysReduceProEpilogue;
    #else
      static void privFileOpenBody(Tuint16 uiTicksToWait) __attribute__((used)) defSysReduceProEpilogue;
    #endif
  #else
    #if (cfgUseFileSystemConcurrentRead == cfgTrue)
      static void privFileOpenBody(Tbool bReadOnly) __attribute__((used)) defSysReduceProEpilogue;
    #else
      static void privFileOpenBody(void) __attribute__((used)) defSysReduceProEpilogue;
    #endif
  #endif
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Body function for taskFileClose See and use that function.
 */
#if (cfgUseFileSystem  ==  cfgTrue) && (includeTaskFileAccess == cfgTrue)
  static void privFileCloseBody(void) __attribute__((used)) defSysReduceProEpilogue;
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Body function for WaitForFsAccessBody See and use that function.
 */
#if (cfgUseFileSystem  ==  cfgTrue)
  static void privWaitForFsAccessBody(void) __attribute__((used)) defSysReduceProEpilogue;
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Body function for taskWaitForEvent See and use that function.
 */
#if (cfgUseEvents == cfgTrue) && (includeTaskWaitForEvents == cfgTrue)
  #if (cfgUseTimeout == cfgTrue)
    static void privWaitForEventBody(Tuint08 uiEvent, Tuint16 uiTicksToWait) __attribute__((used)) defSysReduceProEpilogue;
  #else
    static void privWaitForEventBody(Tuint08 uiEvent) __attribute__((used)) defSysReduceProEpilogue;
  #endif
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Shows the errors produced internally to the user via a portShowError
 * method, and stops the current task is necessary. In case of a fatal
 * error, this method does not return.
 */
#if (defCheckReportingError == cfgTrue)
  #if (cfgCheckAlwaysFatal == cfgTrue)
    static void privShowError(Tuint08 uiMessage,Tuint08 uiInfoTask) __attribute__ (( noreturn ));
  #else
    static void privShowError(Tuint08 uiMessage,Tuint08 uiInfoTask);
  #endif
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Checks if the Stack of the OS is below or on its limit, generates
 * an error otherwise
 */
#if (cfgCheckOsStack == cfgTrue) || (cfgCheckWatermarks == cfgTrue)
  static void privCheckOsStack(void);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Sets up the context for initial use by a task
 */
#ifdef portInitContext
  static Taddress privInitContext(Taddress pTaskStart, Taddress pStackTop, Tuint08 uiRegisterCount, Tuint08 uiInterruptStart);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Returns a pointer to the task control block from the given task number.
 * In Femto OS we choose not to store the pointer of the tcb of the
 * current task itself, since that would require an two extra bytes of ram.
 * The downside is that we must call this method a lot.
 */
static TtaskControlBlock * privTcbList(Tuint08 uiTaskNumber) __attribute__ ((pure));

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * The method calculates how many registers are used for a particular
 * RegisterUse byte. Each bit set to one counts for four registers, so
 * in fact is adds the number of bits set to one in the byte, multiplied
 * by four. We only need if we cannot calculate the number of registers
 * used at compile time.
 */
#if (defRegisterUseConstant == cfgFalse)
  static Tuint08 privRegisterCount(Tuint08 uiRegisterUse) __attribute__ ((const));
#endif

/*DISCUSSION
 * Some methods use quite a bit of stack, i.e. they start by saving
 * say 10 registers on the context. This implies not only 10 more bytes
 * of ram use, but also 40 extra flash bytes just to store and retrieve
 * the bytes. This must be weighed against inlining those methods.
 * Less stack, more flash, but maybe not that much more. It really depends
 * which resource is limited.
 * Also that discussion cannot be made per function, since you cannot simply
 * add the stack use. Functions could be inlined if needed by adding
 *  __attribute__ ( ( always_inline ) );
 * to their definitions.
 */

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * The method initializes the context of a particular task. Its sets up the
 * stack, initializes the used fields, and clears the watchdog. The method is
 * used on three places. Called from main, when the watchdog bites, and from
 * task restart.
 */
static void privTaskInit(Tuint08 uiTaskNumber, Tuint08 uiInitControl) defConditionalInline;

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * This method is the start of the OS after the context has been saved, and
 * possible some handling has been done. It takes care of timing, switching
 * task, etc etc. It ends with starting a new task, starting the idle task
 * or bringing the device to sleep.
 */
static void privEnterOS(Tuint08 uiAction) defSysReduceProEpilogue;

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Called from priv only. It increments the tick counter one (or more!)
 * ticks, calls the necessary hooks, checks the watchdog and reactivates
 * delayed tasks.
 */
static void privIncrementTick(void);

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Checks if some tasks has deadlocked, switches to the next task and
 * determines if the device may be put to sleep.
 */
static Tuint08 privSwitchContext(void);


/**
 * Internal use only, not possible to call yourself from application code.
 *
 * If the device may be put to sleep, all preparations are made here,
 * such as, adjusting the status and reseting the stack. From here,
 * portSleep is called.
 */
#if (cfgUseLowPowerSleep == cfgTrue)
  static void privEnterSleep(Tuint08 uiTickMinDelay);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * If the OS has no runnable task but may not sleep, the idle 'tread'
 * may run. All preparations are made here, such as, adjusting
 * the status and reseting the stack. From here it jumps to portIdle.
 */
static void privEnterIdle(void) defSysReduceProEpilogue;

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * If the OS has a runnable task all preparations are made here,
 * such as, initializing the background variables, adjusting the status
 * and switching the stack. From here  it jumps  portRestoreContext.
 */
static void privEnterTask(void) defSysReduceProEpilogue;

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * If we monitor the load of the system and each task, the collected
 * values are copied to readable variables in this method.
 */
#if (cfgUseLoadMonitor == cfgTrue)
  static void privCopyLoad(void);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Used to reactivated a task when the delay timer expires or when the
 * timeout on a task is over.
 */
#if (defUseDelay == cfgTrue)
  static void privWakeupFromDelay(Tuint08 uiTaskNumber, TtaskControlBlock * taskTCB);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Makes the calculations needed to delay a particular task from
 * the current time (bFromNow == true) of from the last wake time
 * (bFromNow == false).
 */
#if (defUseDelay == cfgTrue)
  static void privDelayCalc(Tuint16 delayTime, Tbool bFromNow) defConditionalInline;
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Cleans the whole slotstack
 */
#if (cfgUseSynchronization != cfgSyncNon) && ((cfgUseTaskWatchdog == cfgTrue) || ( includeTaskRestart == cfgTrue ))
  static void privCleanSlotStack(TtaskExtendedControlBlock * taskTCB);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Defines operations on the slot stack. Using the controls you can add a lock
 * to the stack, remove a lock, or search for a particular lock.
 */
#if (cfgUseSynchronization != cfgSyncNon)
  static Tbool privOperateSlotStack(Tuint08 uiControlTaskNumber, Tuint08 uiSlotSlot);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Gets the size of a queue from flash.
 */
#if (cfgUseSynchronization != cfgSyncNon) && (defUseQueus == cfgTrue)
  static Tuint08 privGetQueuSize(Tuint08 uiQueuNumber) __attribute__ ((always_inline, const));
#endif


/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Unblock the task given by taskTCB, does the actual releasing, and sets the
 * return value. Only call this is you know the task is blocking, since it
 * only operates on the zeroth lock. It always unblocks, and unlocks upon request.
 * It cannot be used to unlock a non blocking lock. Also, supply the return state
 * for the unblocked task (true of false)
 */
#if (cfgUseSynchronization != cfgSyncNon) || (cfgUseFileSystem == cfgTrue)
  static void privUnblockTask(Tuint08 uiControlTaskNumber);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Implements priority lifting. It runs trough all tasks and lifts the
 * priority of the tasks holding the same slot to the level highest
 * priority in the list.
 */
#if (cfgUseSynchronization != cfgSyncNon) && (cfgUsePriorityLifting == cfgTrue)
  static void privLiftLocksOnSlot(Tuint08 uiSlot);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Set the priority of the released task back to its initial value.
 * (Note this may not be the value prior to lifting, since that value
 * is not stored.
 */
#if (cfgUseSynchronization != cfgSyncNon) && (cfgUsePriorityLifting == cfgTrue)
  static void privRestoreInitialPriority(Tuint08 uiTaskNumber);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Calculate the max number of bytes that can be read from the queue,
 * given at uiSlot, (siFreeFilling>0) or can be written to the
 * queue (siFreeFilling<0). Returns the queue size when siFreeFilling==0.
 */
#if (cfgUseSynchronization != cfgSyncNon) && (defUseQueus == cfgTrue)
  static Tuint08 privQueuTest(Tuint08 uiSlot, Tsint08 siFreeFilling);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Release the next task that is using the given slot. This method
 * runs through all tasks and picks the first, not necessarily the one
 * with the highest priority, so make use of priority lifting if needed.
 * Use to release a mutexes and queues.
 */
#if (cfgUseSynchronization != cfgSyncNon) && ((defUseMutexes == cfgTrue) || (defUseQueus == cfgTrue))
  static void privReleaseSyncBlockingTasks(void) defConditionalInline;
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Test if a slot is locked freely (it non blocking) in any of the tasks.
 * Returns true if the slot is absent, zero or only used as blocking slot.
 * If a slot was obtained by a task that later blocked on an other slot,
 * this is still considered free.
 */
#if (cfgUseSynchronization != cfgSyncNon) && ((defUseMutexes == cfgTrue) || (defUseQueus == cfgTrue))
  static Tuint08 privFreeLockAbsent(Tuint08 uiSlot);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Test if a queue on slot matches the freeFilling sites. If true, the queue
 * can store or produce at least the number of bytes given. Slot numbers
 * representing mutexes always return true, independent of the value of
 * siFreeFilling.
 */
#if (cfgUseSynchronization != cfgSyncNon) && (defUseQueus == cfgTrue)
  static Tbool privSizeFitsQueu(Tuint08 uiSlot, Tsint08 siFreeFilling);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * See if there are tasks that can be released for reading or writing
 * the file system. Tasks are blocked/released using a single write
 * multiple read synchronizer. This works independent of the slots.
 */
#if (cfgUseFileSystem  ==  cfgTrue)
  static void privReleaseFileBlocks(void);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * The sizes of all files are stored in flash. Use this routine to
 * calculate the starting point of a particular file in eeprom. You
 * specify an offset, which is added to the starting point.
 */
#if (cfgUseFileSystem  ==  cfgTrue)
  static Taddress privFileLocation(Tuint08 uiFileNumber, Tuint08 uiOffset);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Use this routine to put all tasks (that are not waiting on some
 * file action) to sleep.
 */
#if (cfgUseLowPowerSleep == cfgTrue) && (includeTaskSleepAll == cfgTrue)
  static void privPutAllTasksToSleep(void);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * This method handles closing of file operations for a task. In case
 * of reading, the block is released and new tasks are given an opportunity
 * to write if needed. In case of a write, it puts the task in a state
 * where it can wait for the burn lock to disappear.
 */
#if (cfgUseFileSystem  ==  cfgTrue)
  static void privPrepareFileClose(Tuint08 uiTaskNumber);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Checks if the given parameters are possible. It checks if the mode the system
 * is in (closed, open-read-only, open-read-write) corresponds to the call, checks
 * if the file number is valid, and check if the write operation does not exceed
 * file boundaries.
 */
#if (cfgUseFileSystem  ==  cfgTrue) && (cfgCheckMethodUse == cfgTrue)
  static void privCheckFileSpecsWriting(Tuint08 uiFileNumber, Tuint08 uiOffset, Tuint08 uiSize);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Checks if the given parameters are possible. It checks if the mode the system
 * is in (closed, open-read-only, open-read-write) corresponds to the call, checks
 * if the file number is valid, and check if the read operation does not exceed
 * the present content of file boundaries.
 */
#if (cfgUseFileSystem  ==  cfgTrue) && (cfgCheckMethodUse == cfgTrue)
  static void privCheckFileSpecsReading(Tuint08 uiFileNumber, Tuint08 uiOffset, Tuint08 uiSize);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Obtain the maximal size of a file. Takes into account if the file is predefined
 * or default.
 */
#if (cfgUseFileSystem  ==  cfgTrue) && ( ((cfgUseFileSystemMaintainFAT == cfgTrue) && (includeTaskFileAppendByte == cfgTrue)) || (cfgCheckMethodUse == cfgTrue) )
  static Tuint08 privFileSpace(Tuint08 uiFileNumber);
#endif

/**
 * Internal use only, not possible to call yourself from application code.
 *
 * Check if a function may be called for the particular task. Not all task support the capabilities
 * of the OS. This is defined in the configuration.
 */
#if (cfgCheckMethodUse == cfgTrue)
  #if (defCapabilitiesFull == cfgFalse)
    static void privCheckCapabilities(Tuint08 uiControlTaskNumber, Tuint08 uiTaskCaps) __attribute__ ((unused));
  #else
    #define privCheckCapabilities(X,Y)
  #endif
#endif


/* ========================================================================= */
/* FEMTO OS CORE IMPLEMENTATION ============================================ */
/* ========================================================================= */


#if (defCheckReportingError == cfgTrue)


static void privShowError(Tuint08 uiMessage,Tuint08 uiInfoTask)
{ /* We may arrive here from OS space, isr or task space. In the latter case is important we do
   * not get tick interrupts (only possible when coming from a task), since reporting an error
   * is usually a critical situation. But since we want the user to see the error it is best
   * we stop all interrupts.
   * Note that for tasks this method should not return to the point of calling but invoke a
   * context switch instead. When coming from an isr, we cannot stop that particular isr so
   * we should treat that situation as fatal. Only when coming here from the OS itself, we
   * may return if that was specifically asked for, or we must switch to a new task (usually
   * can a switching method was called from the task). Only when the possibility exists we
   * return to the point of origin we need to keep the stack. Thus, from
   *  isr  => make error fatal, renew stack, never return
   *  task => if fatal error, renew stack, never return
   *       => if non-fatal error, renew stack, terminate task, issue a switch-task
   *  os   => if fatal error, renew stack, never return
   *       => if non-fatal error with switch request, renew stack,  issue a switch-task
   *       => if non-fatal error with return request, keep stack, return
   * Of course, when cfgCheckAlwaysFatal is activated the error is ... always fatal.
   */
  privDisableGlobalInterrupts();
  /* Errors are traced first, to be sure that tracing is done in case the portError holds the system for ever. */
  privTrace(errTraceErrorBase | uiMessage);
  /* Strip the error type information from the message */
  Tuint08 uiBareMessage = (uiMessage & errMessageMask);
  /* First check if we have fatal errors per default */
  #if (cfgCheckAlwaysFatal == cfgTrue)
    /* If so, the matter is simple. All errors are fatal and we never return. */
    const Tbool bFatal = true;
    const Tbool bReturn = false;
  #else
    /* If not, we first calculate when we have a fatal error. That is the case is the error in itself is
     * fatal, or if we arrive here from an isr. */
    const Tbool bFatal = (uiBareMessage >= errFatalError) || ((uiOsStatus & defContextGetMask) == defContextStateIsr);
    /* We may normally return only in one special case. Of course the error may not be fatal, and we must come
     * here from the OS itself and there must be a special request to do so. The latter, since it is not possible
     * to return to an OS error in general. */
    const Tbool bReturn = !bFatal && ((uiOsStatus & defContextGetMask) == defContextStateOs) && ((uiMessage & errOsMask) == errOsStateReturn);
  #endif
  /* If we do not return we reset the stack to prevent stack overflow when handling the error code.
   * We don't need the present stack anymore. */
  if (!bReturn) { privSetStack(&xOS.StackOS[OSstackInit]); }
  /* We disable tick interrupts, needed to be deactivated in case we should return. We do that here, after a possible
   * stack transplant since this call may make use of the stack. It must be done before global interrupts are
   * activated again. */
  privDisableTickInterrupts();
  /* Now determine the task number. */
  Tuint08 uiTaskNumber;
  /* Test if we must use the given task number by uiInfoTask or the current one. This flag is used because
   * from a lot of places its much sorter to set the flag than to calculate the current task number. */
  if ((uiMessage & errTaskMask) == errTaskStateCurrent)
  { /* Deduce the current task number from the Status */
    uiTaskNumber = (uiOsStatus & defTaskNumberGetMask) >> defOsTaskNumberShift;
    /* Reconstruct the uiInfoTask byte, that will be send to the error method later on */
    uiInfoTask = (uiInfoTask & errTaskNumberSetMask) | (uiTaskNumber << errTaskNumberShift); }
  else
  { /* Read the task number from the given byte, probably the current task is not the one giving problems */
    uiTaskNumber = (uiInfoTask & errTaskNumberGetMask) >> errTaskNumberShift ; }
  /* Now, show the error,  if we had a fatal error it makes no sense to continue operations,
   * so the only thing we can do is show the error again*/
  do { portShowError(uiBareMessage,uiInfoTask); } while (bFatal);
  /* In case we had a normal error we want to continue, but since it is reasonable to assume that it
   * may have taken some time, the user has probably stop the timer, if he did not misused the timer
   * to make some leds blink. In any case its best to restart the timer. We have to live with the
   * fact that real time and timer ticks are out of sync anyway. Note that the user may use the timer,
   * but may not activate the interrupt. */
  portSetupTimerInterrupt();
  /* We return from this call with a setup timer, and with tick interrupts disabled. The global interrupts
   * are still disabled. If we have no os protection we must re-enable global interrupts again. */
  #if (cfgIntOsProtected == cfgFalse)
    privEnableGlobalInterrupts();
  #endif
  /* All errors not being fatal at least must stop the current or requested task and put it in error mode.
   * (This cannot be an isr anymore, and all non fatal Os errors are about some task.) */
  TtaskControlBlock * taskTCB = privTcbList(uiTaskNumber);
  /* Note that the task is terminated, but it's locks are not removed, since this is an error
   * condition, it is unclear what the result would be, so that this cannot be compared with
   * a normal terminate, in which case the user can take precautions for released blocks etc. */
  taskTCB->uiTaskStatus = defBaseTerminatedTask;
  /* If we are at a task, or at some operations within the OS that handle a specific task,
   * we can leave this method by asking for a task switch. This is not a normal return.
   * Note that in this case the AuxRegBit (if used) can be set. Here, that is not a problem
   * for there are no switching interrupts that can take place. */
  if (!bReturn) { privEnterOS(defActionTaskStateSwitch); }
  /* Otherwise we are done, and return to the OS operations that generated the error. */ }

#endif


#if (cfgCheckOsStack == cfgTrue) || (cfgCheckWatermarks == cfgTrue)

static void privCheckOsStack(void)
{ /* We are going to check if we overflowed the Os Stack. We should only arrive here if we are
   * in OS space, i.e. if we are sure the stack pointer actually should be inside the OS stack. */
  Taddress pStack;
  Tuint16 uiOsStackSize;
  /* Obtain the current stack pointer. */
  privGetStack(pStack);
  /* Calculate the Stacksize, we have to distinguish between the different stack types. Note we
   * assumed the stack pointer points to the byte that will be written next, i.e. the stack pointer
   * is post increment.  */
  #if (cfgSysStackGrowthUp == cfgTrue)
    uiOsStackSize = ((Tuint16) pStack - (Tuint16) &xOS.StackOS[0]);
  #else
    uiOsStackSize = ((Tuint16) &xOS.StackOS[StackSizeOS-1] - (Tuint16) pStack);
  #endif
  /* Now, we do not incorporate the StackSafety parameter into our calculation. That parameter is
   * used for task stacks only. And, besides that, the call to this function, i.e.. privCheckOsStack
   * costs two bytes extra anyway */
  #if (cfgCheckWatermarks == cfgTrue)
    if (uiOsStackMax<uiOsStackSize) uiOsStackMax=uiOsStackSize;
  #endif
  #if (cfgCheckOsStack == cfgTrue)
    if (uiOsStackSize>StackSizeOS) { privShowError((fatOsStackOverflowed | errTaskStateNon), errNoInfo); }
  #endif
}

#endif


#if (cfgCheckMethodUse == cfgTrue) && (defCapabilitiesFull == cfgFalse)

static void privCheckCapabilities(Tuint08 uiControlTaskNumber, Tuint08 uiTaskCaps)
{ /* Use this function to see if the task is capable of the required functions.
   * The function is not needed if all tasks have full capabilities, for there is nothing
   * to check in that case. */
  Tuint08 uiTaskNumber;
  /* Check if we are interested in the current task */
  if ((uiControlTaskNumber & defCurrentTaskMask) == defCurrentTaskNumber)
  { /* The current task has no meaning inside an isr, so there is nothing to check in that case */
    if ((uiOsStatus & defContextGetMask) == defContextStateIsr) { return; }
    /* If yes, extract the current task number. */
    uiTaskNumber = (uiOsStatus & defTaskNumberGetMask) >> defOsTaskNumberShift; }
  else
  { /* If no, deduce the task number from the given number. */
    uiTaskNumber = (uiControlTaskNumber & defTaskNumberGetMask); }
  /* Get the capabilities of the task*/
  Tuint08 uiDefinedCaps = portFlashReadByte(Tuint08,uiCapabilities[uiTaskNumber]);
  /* Check if the capabilities match, with ~ all non defined caps turn high, if it matches with a
   * required cap, this is an error. */
  Tuint08 uiViolation = (uiTaskCaps & ~uiDefinedCaps);
  /* Thus if the value equals zero all required capabilities are present, otherwise some are absent. */
  if (uiViolation != 0)
  { /* Determine the highest violation (binary log) */
    Tuint08 uiInfo = 8;
    /* Check if this is the violating bit */
    while ( (uiViolation & 0x80) == 0 )
    { /* If not it could be bit 6, so we decrease the counter one ... */
      uiInfo--;
      /*  and shift the 6th bit to the 7th */
      uiViolation <<= 1; }
    /* Report an error and stop the task if the requested capabilities are absent. The ControlTaskNumber
     * also contains the realm in which we currently are.  */
    privShowError((errInsufficientCapabilities | errTaskStateInfo | errOsStateSwitch), (uiInfo << errInfoNumberShift) | (uiTaskNumber << errTaskNumberShift)); } }


#endif


#ifdef portInitContext

static Taddress privInitContext(Taddress pTaskStart, Taddress pStackTop, Tuint08 uiRegisterCount, Tuint08 uiInterruptStart)
{ /* Arrive here to set up the initial stack. Of course this is machine dependent, but not very
   * strongly. For 8 bits cpu's it is fairly general. Just place the return address, the registers
   * and the status. Here the values are just cleaned. If needed we can add a portInitConext call
   * on the basis of an option. The initialization of the status register requires some care since
   * in this port we keep the state of the tick interrupt in the (unused) T bit of the avr, when storing
   * the context. Thus you may use the T bit only if no tick interrupt can take place, otherwise that
   * bit is expected to be zero. If you use an other mechanism to to store the state of the tick interrupt
   * you must redefine this method also.
   */
  Tuint16 uiStartAddress = (Tuint16) pTaskStart;
  /* At the bottom of the artificial stack the start address of each task is defined. This is a pointer
   * to your Loop code. */
  *(pStackTop--) = (Tuint08) uiStartAddress;
  *(pStackTop--) = (Tuint08) (uiStartAddress >> 8);
  /* If we have a program counter of more than 16 bit, call instructions push three bytes
   * into the stack. We must take that into account. Unfortunately, i believe, gcc is not
   * able to handle pointer larger as 64K words in the current setting. Thus this will
   * effectively be zero. */
  #if (defThreeByteAddress == cfgTrue)
    *(pStackTop--) = (Tuint08) (uiStartAddress >> 16);
  #endif
  /* We rely upon register cleaning done by the Femto OS, or the precleaning done by privTaskInit(). */
  pStackTop -= uiRegisterCount;
  /* The way the status register is setup depends on the state of the interrupts. These can be specified per task
   * or (easier, and shorter) in general. Note that the optimization is not needed in a strict sense, the variable
   * uiInterruptStart constrains the interrupt start information, even if this is constant for all tasks. But, of course,
   * the lower part generates shorter code. */
  #if (defInterruptStartConstant == cfgFalse)
    /* If we specify the CPU status register per task, we need to prepare them per task. uiInterruptStart contains
     * the information about which interrupts must be activated. The other bits of the status register cannot be
     * set individually (there is no need in general) */
    Tuint08 uiInitCPUStatusRegister = defInitCPUStatusRegister | (0 << portInitModeInterruptLoc);
    /* Test if global interrupts must be activated at the start, if so set the specified bit. */
    #if (cfgIntGlobalOnly ==  cfgTrue)
      /* In case we have a mapping from tick interrupts on global interrupts, we require both interrupts
       * to be activated before activating the global interrupt. */
      if ((uiInterruptStart & ((cfgGlobSet | cfgTickSet) & defInterruptStartMask)) == ((cfgGlobSet | cfgTickSet) & defInterruptStartMask)) { uiInitCPUStatusRegister |= (1 << portInitGlobalInterruptLoc); }
    #else
      /* Otherwise we just test the global interrupt setting. */
      if ((uiInterruptStart & (cfgGlobSet & defInterruptStartMask)) != defInterruptAbsent) { uiInitCPUStatusRegister |= (1 << portInitGlobalInterruptLoc); }
    #endif
    /* Test if tick interrupts must be activated at the start, if so set the specified bit. */
    #if (cfgIntTickTrack == cfgTrue)
      if ((uiInterruptStart & (cfgTickSet & defInterruptStartMask)) != defInterruptAbsent) { uiInitCPUStatusRegister |= (1 << portInitTickInterruptLoc); }
    #endif
    /* The status register is put at the end of the stack. (See portSaveContext for the reason why) */
    *(pStackTop--) = uiInitCPUStatusRegister;
  #else
    /* Set up the status register with the initial interrupt states: global set and tick set */
    #if (((defInterruptStartFixed) & cfgGlobSet) == cfgGlobSet) && (((defInterruptStartFixed) & cfgTickSet) == cfgTickSet)
      *(pStackTop--) = defInitCPUStatusRegister | (0 << portInitModeInterruptLoc) | (1 << portInitGlobalInterruptLoc) | (1 << portInitTickInterruptLoc);
    /* Set up the status register with the initial interrupt states: global set and tick clear */
    #elif (((defInterruptStartFixed) & cfgGlobSet) == cfgGlobSet) && (((defInterruptStartFixed) & cfgTickClear) == cfgTickClear)
      #if (cfgIntGlobalOnly == cfgTrue)
        /* In case we have a mapping from tick interrupts on global interrupts, global interrupts cannot be activated if tick interrupts are not activated */
        *(pStackTop--) = defInitCPUStatusRegister | (0 << portInitModeInterruptLoc) | (0 << portInitGlobalInterruptLoc) | (0 << portInitTickInterruptLoc);
      #else
        /* Default situation. */
        *(pStackTop--) = defInitCPUStatusRegister | (0 << portInitModeInterruptLoc) | (1 << portInitGlobalInterruptLoc) | (0 << portInitTickInterruptLoc);
      #endif
    /* Set up the status register with the initial interrupt states: global clear and tick set */
    #elif (((defInterruptStartFixed) & cfgGlobClear) == cfgGlobClear) && (((defInterruptStartFixed) & cfgTickSet) == cfgTickSet)
      *(pStackTop--) = defInitCPUStatusRegister | (0 << portInitModeInterruptLoc) | (0 << portInitGlobalInterruptLoc) | (1 << portInitTickInterruptLoc);
    /* Set up the status register with the initial interrupt states: global clear and tick clear */
    #elif (((defInterruptStartFixed) & cfgGlobClear) == cfgGlobClear) && (((defInterruptStartFixed) & cfgTickClear) == cfgTickClear)
      *(pStackTop--) = defInitCPUStatusRegister | (0 << portInitModeInterruptLoc) | (0 << portInitGlobalInterruptLoc) | (0 << portInitTickInterruptLoc);
    #elif (defNumberOfTasks == 0)
      /* without tasks there is nothing to set. */
    #else
      /* well, we never come here. */
      #error Parameter 'defInterruptStartFixed' is misspeld or incorrect (You should not arrive here)
    #endif
  #endif
  /* Done, let the caller know where the stack ended. */
  return pStackTop; }
#endif

static TtaskControlBlock * privTcbList(Tuint08 uiTaskNumber)
{ /* Most of the time, a call to this function represents the deepest use of the stack (this
   * was verified using a simulator). This it makes sense to issue a call to the os check
   * here. We must however test if we are indeed in os space, since this function is called
   * from the api also, thus being in task space. Note that this function is declared pure,
   * which in not true is the stack is overflown. The effect however of a pure function is
   * that the optimized may relocate is (even over volatile functions). */
  #if (cfgCheckOsStack == cfgTrue) || (cfgCheckWatermarks == cfgTrue)
    /* We check if we are in OS space here, and not in privCheckOsStack for if we are not
     * we do not want to load an extra two bytes on some task stack */
    if ((uiOsStatus & defContextGetMask) == defContextStateOs) { privCheckOsStack(); }
  #endif
  /* Check if we are interested in the current task */
  if ((uiTaskNumber & defCurrentTaskMask) == defCurrentTaskNumber)
  { /* If yes, replace the uiTaskNumber with the current task number. */
    uiTaskNumber = (uiOsStatus & defTaskNumberGetMask) >> defOsTaskNumberShift; }
  /* The location of the task control block is stored in flash, be careful, use the correct
   * instructions to retrieve it. */
  return portFlashReadWord(TtaskControlBlock *,pxTCBlist[uiTaskNumber]); }


#if (defRegisterUseConstant == cfgFalse)

static Tuint08 privRegisterCount(Tuint08 uiRegisterUse)
{ /* We want to count the number of register this uiRegisterUse byte represent. Each set bit
   * counts for four registers.  */
  Tuint08 uiLoop = 8;
  /* uiCount accumulates the number of set bits */
  Tuint08 uiCount = 0;
  /* Loop  through all bits of the uiRegisterUse byte.*/
  while (uiLoop--)
  { /* Increase the counter if the bit is set */
    uiCount += (uiRegisterUse & 0x01);
    /* Shift to the next bit */
    uiRegisterUse >>= 1; }
  /* Each bit counts for four registers, so multiply by four */
  uiCount <<= 2;
  return uiCount; }

#endif


static void privTaskInit(Tuint08 uiTaskNumber, Tuint08 uiInitControl)
{ /* Tasks are initialized in this routine. Note the fields:
   * uxDelay, siQueuLock, uiLoadCollect, uiLoadTotal, uiStackMax, uiRegisterUse
   * are not cleared when we re-initialize the task. Only about uxDelay there could
   * be debate since the would indicate the last wake time. But what is a last wake
   * time in this case. We could set it to the current time, but have not done so.  */
  /* Report that we are initializing this task. */
  privTrace(traceTaskInit | uiTaskNumber);
  TtaskControlBlock * taskTCB = privTcbList(uiTaskNumber);
  /* In case we might return here due to a watchdog bark or a restart or so. some old connections
   * must be closed when present. */
  #if (defReUseTaskInit == cfgTrue)
    /* And if asked for it. */
    if ((uiInitControl & defInitLockMask) == defInitLockRelease)
    { /* In case we use synchronization and the watchdog or want to be able to manually restart a task
       * it may be that the task that has to be re-initiated is holding some locks. These must be released properly
       * We need only to release locks on other tasks that are held by this task. Since waits cannot be non-blocking
       * it can safely be ignored here. Hmm, some other task may call restart on a blocking task on wait. So it seems
       * better to always clean when synchronization is used. */
      #if (cfgUseSynchronization != cfgSyncNon)
        /* we only need to check if the task contains a slot. */
        if (uiTaskNumber < defNumberOfTasksWithSlot)
        { /* We may only enter here if the particular task indeed has a slot stack, otherwise there is nothing clean
           * If we made use of priority lifting, we do not need to do anything. We can simply clean the slot stack. */
          privCleanSlotStack((TtaskExtendedControlBlock *) taskTCB );
          /* and any task that may run freely now will be release by the call below. Please note that is not possible
           * that is call releases the present task, since all its slots have been wiped. */
          #if ((defUseMutexes == cfgTrue) || (defUseQueus == cfgTrue))
            privReleaseSyncBlockingTasks();
          #endif
          }
      #endif
      /* At this point we must check if we have some kind of lock in the file system and we need
       * to release this and other tasks. Only include code if this is possible at all. */
      #if (cfgUseFileSystem == cfgTrue)
        /* We can simply try to close a file. If no lock is hold and no blocks are present this method returns
         * without any harm done. Of course it is more overhead, but we assume this part of the code is rarely executed
         * and therefore we do not want to invest the bytes. There are quite a number of situations in which the task
         * can be caught when killed. All these situations are handled by the method. */
        privPrepareFileClose(uiTaskNumber);
      #endif
    }
  #endif
 /* Read the priority the task will start with is contained in the lowest three bits of this number, the interrupt
  * information is contained in the highest bits.  */
  #if (defInitialStatusConstant == cfgTrue)
    Tuint08 uiInitialStatus = defInitialStatusFixed;
  #else
    Tuint08 uiInitialStatus = portFlashReadStruc(TtaskDefinitionBlock,pxTDBlist[uiTaskNumber],Tuint08,uiInitialStatus);
  #endif
 /* The defInitStatus indicates if we want to prepare a new TaskStatus. This is needed upon first use, as it
  * contains the initial run state, its priority etc. If we are here due to a watchdog reset, we do not want
  * to reset its status completely, this must be done by hand. Since we may, for instance use the watchdog to lower the
  * priority of the task. We know when we get here from a watchdog that the status was runnable, otherwise
  * the watchdog cannot bark, but we initialize in the state it started out. The location of the DelayState and
  * Dress state are used to store the interrupt starts, so these must be filtered out */
  Tuint08 uiIntialFilteredStatus = (uiInitialStatus & (defBaseDelayStateSetMask & defBaseDressSetMask) ) | (defBaseDelayStateWake | defBaseDressRunable);
  /* Depending on the fact if we are able to return here we can directly set its initial state, or
   * we must be more careful */
  #if (defReUseTaskInit == cfgTrue)
    if ((uiInitControl & defInitStatusMask) == defInitStatusRenew)
    { /* All tasks start in the state specified by the Include parameter in the configuration file. This information
       * is packed into the uiInitialStatus. Blocking and Delay cannot be predefined.*/
      taskTCB->uiTaskStatus = uiIntialFilteredStatus; }
    else
    { /* The bark routine is given the opportunity to change this state later on, but before the task loop is called.
       * Thus we can create the state just as it was at startup, but we leave the priority as is. */
      taskTCB->uiTaskStatus = (uiIntialFilteredStatus & defBasePrioSetMask) | (taskTCB->uiTaskStatus & defBasePrioGetMask); }
  #else
    taskTCB->uiTaskStatus = uiIntialFilteredStatus;
  #endif
  /* If we keep track of the watermarks ... */
  #if (cfgCheckWatermarks == cfgTrue)
    /* ... Initializing the task may (requested through the call) require resetting those levels. */
    if ((uiInitControl & defInitWatermarksMask) == defInitWatermarksRenew)
    { /* If so, clean the StackMax level and the use of the registers. */
      taskTCB->uiStackMax = 0;
      taskTCB->uiRegisterUse = 0; }
  #endif
  /* Fields uiTaskMonitor is initialized. Usually the init bytes are 0x00. If we
   * have no watchdog and no TaskRestart we can only arrive here one time. If the init fields are
   * 0x00 so we may rely upon the cleaning of the .bss section to reset these fields.
   * In other cases however we must explicitly clean/initialize those fields. */
  #if (defUseTaskMonitor == cfgTrue) && ((defReUseTaskInit == cfgTrue) || (defTaskMonitorInit != 0x00))
    taskTCB->uiTaskMonitor = defTaskMonitorInit;
  #endif
  /* Fields uiTasksLevels is initialized. Usually the init bytes are 0x00. If we
   * have no watchdog and no TaskRestart we can only arrive here one time. If the init fields are
   * 0x00 so we may rely upon the cleaning of the .bss section to reset these fields.
   * In other cases however we must explicitly clean/initialize those fields. */
  #if (defUseTaskLevels == cfgTrue) && ((defReUseTaskInit == cfgTrue) || (defTaskLevelsInit != 0x00))
    taskTCB->uiTaskLevels = defTaskLevelsInit;
  #endif
  /* pcStackOffset is a pointer to the beginning of the stack of this task, it is located in flash. */
  Taddress pcStackOffset = portFlashReadStruc(TtaskDefinitionBlock,pxTDBlist[uiTaskNumber],Taddress,pcStackOffset);
  /* We need the place where the task starts. This is placed on the context as return address. It is located in flash */
  Taddress pTask = portFlashReadWord(Taddress,pxLooplist[uiTaskNumber]);
  /* Register compression requires information on which registers must be saved. Every bit stands for
   * four bits to save. If we make use of Register Compression with variable registers per task we must
   * load the information from flash. otherwise we just may use the constant here.
   * Here we count how may registers will be saved on the stack. For each register one byte will be reserved.
   * If all RegisterUse parameters are equal, we can calculate the number at compile time. */
  #if (defRegisterUseConstant == cfgTrue)
    /* The constant defRegisterCount holds the number of registers used.*/
    Tuint08 uiCount = defRegisterCount;
  #else
    /* The register use must be read from flash.  */
    Tuint08 uiRegisterUse = portFlashReadStruc(TtaskDefinitionBlock,pxTDBlist[uiTaskNumber],Tuint08,uiRegisterUse);
    /* We have to calculate the number dynamically */
    Tuint08 uiCount = privRegisterCount(uiRegisterUse);
  #endif
  /* Initializing the slot may of course only be done after all locks have be neatly cleaned up. Here we
   * assume that has been done, or is not needed. */
  /* We need the StackSize if we must check if it is enough or if we must clean the stack before use  */
  #if ( (cfgCheckTaskStack == cfgTrue) || (defStackClean == cfgTrue) )
    /* Get the reserved stack space from flash, or use the constant if all stack sizes are identical Note that, if we
     * don't check, we don;t need the uiStackSize information. It is simply assumed there is enough. */
    #if (defStackSizeConstant == cfgTrue)
      Tstack uiStackSize = defStackSizeFixed;
    #else
      Tstack uiStackSize = portFlashReadStruc(TtaskDefinitionBlock,pxTDBlist[uiTaskNumber],Tstack,uiStackSize);
    #endif
  #endif
  /* conditional code if we want to check some stack parameters. */
  #if (cfgCheckTaskStack == cfgTrue)
    /* First check if that space is at least the minimum required space needed. This minimum consists of the
     * status byte and at the return address. Thus we need always some stack, even if we don't save any
     * registers. */
    if (uiStackSize < defTaskStackMinimum)
    { privShowError((errTaskStackUnderMinimum | errTaskStateInfo | errOsStateReturn), (uiTaskNumber << errTaskNumberShift) ); }
    /* Now, we can check if, given the number of registers needed to be saved and the safety we want to
     * take into account, there is enough room */
    else if (uiCount + defTaskStackGround + StackSafety > uiStackSize)
    { privShowError((errTaskStackSetupTooSmall | errTaskStateInfo | errOsStateReturn), (uiTaskNumber << errTaskNumberShift) ); }
    /* Now be careful, only if we succeed the tests above we will set up a context. There is no problem if we
     * failed, since the system will never activate the task, and there is no instruction to revive a error
     * task. We should not notice the absence of the context. But the C construction is a little awkward,
     * with this dangling else. */
    else
  #endif
  /* Clean the stack and set up the context. The method portInitContext makes room the return address.
   * the device status register and all registers used. */
  #if (cfgSysStackGrowthUp == cfgTrue)
    { /* First fill the whole stack with the defStackInitByte if needed */
      #if (defStackClean == cfgTrue)
        Taddress pStackTop = pcStackOffset;
        while (uiStackSize--) { *(pStackTop++) = defStackInitByte; }
      #endif
      /* Subsequently define the context. */
      taskTCB->pcStackLevel = (Tstack) ((Tuint16) portInitContext(pTask, pcStackOffset, uiCount, uiInitialStatus) - (Tuint16) pcStackOffset); }
  #else
    { /* First fill the whole stack with the defStackInitByte if needed */
      #if (defStackClean == cfgTrue)
        Taddress pStackTop = pcStackOffset;
        while (uiStackSize--) { *(pStackTop--) = defStackInitByte; }
      #endif
      /* Subsequently define the context. */
      taskTCB->pcStackLevel = (Tstack) ((Tuint16) pcStackOffset - (Tuint16) portInitContext(pTask, pcStackOffset, uiCount, uiInitialStatus)); }
  #endif
}


static void privEnterOS(Tuint08 uiAction)
{ /* This is the method to call to start the OS functions. Its is typically called from yield,
   * delay etc, but also from the switching api functions after the completed their task. The
   * action parameter states the intention of the caller. It contains whether or not we should
   * switch the task, and if a tick occurred.
   * Report that we are starting OS specific actions */
  privTrace(traceOsStart);
  /* First we check if no event has taken place. */
  #if (cfgUseEvents == cfgTrue)
  /* Handling events must take place in an protected environment. */
    #if (cfgIntOsProtected == cfgFalse)
      privDisableGlobalInterrupts();
    #endif
    /* Check if there are any event set */
    if (portEventRegister != defAllEventsReset)
    { /* OK, we have to handle the events and possibly deblock some tasks. If we invert the Event
       * register, we have '1' on all places that must be left alone, and '0' on those places where
       * the bits may be reset. */
      Tuint08 uiInvEvent = ~portEventRegister;
      /* Don't forget to reset the event register */
      portEventRegister = defAllEventsReset;
      /* Loop through all tasks possibly waiting on an event. */
      Tuint08 uiLoopTask;
      for (uiLoopTask=defTaskNumberEventBegin; uiLoopTask<defTaskNumberEventEnd; uiLoopTask++)
      { TtaskControlBlock * loopTCB = privTcbList(uiLoopTask);
        /* Test if the task is really waiting on events */
        if (loopTCB->uiTaskEvents != defAllEventsReset)
        { /* reset all events that are fired */
          loopTCB->uiTaskEvents &= uiInvEvent;
          /* Check if there are any events left. */
          if (loopTCB->uiTaskEvents == defAllEventsReset)
          { /* If not, unblock this task. */
            privUnblockTask(uiLoopTask | defParaLockStateUnlock | defParaRetStateTrue);
            /* If we have unblocked a task, we make sure we switch tasks asap */
            /* TODO v0.90: If we are returning a full byte, it must not be possible to
             * switch the task since there will be only one return value register. */
            uiAction |=  defActionTaskStateSwitch; } } } }
    /* Done, restore interrupts if needed */
    #if (cfgIntOsProtected == cfgFalse)
      privEnableGlobalInterrupts();
    #endif
  #endif
  /* If we do not want to switch, we are quickly done. */
  if ((uiAction & defActionTaskGetMask) == defActionTaskStateSwitch)
  { /* If we allow for tick tracking there is the possibility that interrupts are switched off
     * in all tasks. You may call that the full cooperative mode if you like. However, an tick
     * interrupt will never be executed and the tick counter will not be increased. Thus we must
     * test if this situation has arose, and schedule the idle timer to handle the tick. */
    #if (cfgIntTickTrack == cfgTrue)
      /* The check method returns true if a timer interrupt was due. The interrupt was also
       * cleaned and therefore we ask for a increase of the tick counter here. */
      if (portCheckTimer()) { uiAction |=  defActionTickStateTick; }
    #endif
    /* If we make use of a file system we must check if there is a task waiting for a burn lock
	   * to finish (this can only be one task at most. */
    #if (cfgUseFileSystem  ==  cfgTrue)
      /* Look if we have a burn lock active, if so there must be a write lock too, so we do not need to check that. */
      if ((uiFsStatus & defFsBurnBlockGetMask) == defFsBurnBlockActive)
      { /* If so we test if the file system is ready for a new byte to be written */
        if (portFSWriteReady())
        { /* Tell we may burn again. In this case we must clean the burn lock in the uiFsStatus, we do so below. */
          privTrace(traceBurnFree);
          /* Check if we are just waiting for a FS release. In this situation there is no task
           * actually writing, but one of the write tasks was somehow terminated, and the system
           * must wait with the release of new tasks until the burn block is gone. */
          if ((uiFsStatus & defFsReadBlockGetMask) == defFsReadBlockReleaseRequest)
          { /* Clear everything in the status register, except a possible request for sleep. */
            uiFsStatus = (uiFsStatus & defFsFreeSetMask) | defFsFree;
            /* There could be other tasks waiting to be released and perform file operations. Always, directly
             * after a task completed its file operations, it must call for a release of new tasks. There is
             * no automatic dispatch mechanism to activate the tasks independently. */
            privReleaseFileBlocks(); }
          else
          { /* We still have to clear the burn lock */
            uiFsStatus = ((uiFsStatus & defFsBurnBlockSetMask) | defFsBurnBlockClear);
            /* In this case we are in a simple file write operation, so extract the task
             * that was waiting for burning.  The Burn lock can not be active if no task is waiting,
             * which is essential since we have no task number indicating a 'free' state. We could check if that
             * task found has a write lock, but that should be the case */
            #if (defUseFsOnMultipleTasks == cfgTrue)
             /* In the case of multiple tasks, the task that is currently writing is in the status */
             Tuint08 uiLockTaskNumber =  (uiFsStatus & defFsWriteNumberGetMask) >> defFsTaskNumberShift;
            #else
             /* otherwise it is a fixed number determined by the preprocessor. */
             Tuint08 uiLockTaskNumber =  defUseFsSingleTaskNumber;
            #endif
            /* we must unblock the task, note that
             * this kind of block (burn block) cannot timeout, and thus the function does not return a value
             * to indicate this, we must leave the return register as it was. Naturally we keep the lock, for
             * there could be more bytes that must be written. It is not possible for a terminated task
             * to arrive here, since the taskKill routine will always try to close a lock on the file. */
            privUnblockTask(uiLockTaskNumber | defParaLockStateKeep | defParaRetStateNon); } } }
    #endif
    /* Well, let us first determine what kind of tick engine
     * we are running on. Since both types require quite a different approach the code is
     * completely separated. */
    #if (cfgUseEquidistantTicks == cfgTrue)
      /* We are running equidistant ticks. Thus the timer interrupt is set to a fixed value and
       * can only be read, but not changed. Also, the timer is of the type interrupt and clear, thereby
       * guaranteeing the most regular tick intervals. */
      #if ( (cfgCheckTiming == cfgTrue) || (cfgUseLoadMonitor == cfgTrue) )
        /* If we are checking timing issues or want to keep track of the load of the tasks we
         * must read the value timer. */
        Tuint08 uiOsStartTime = portReadTimer();
      #endif
      /* If we want to monitor the load, some calculations are needed. Please note that this facility
       * is only available when cfgIntOsProtected == true. Otherwise there is simply no way get the
       * data to the os, since during this calculation it may be interrupted also. */
      #if (cfgUseLoadMonitor == cfgTrue)
        Tuint08 uiTaskTime;
        /* We want to calculate how much time the last task took. The subbyte contains the
         * last saved value of portReadTimer.*/
        Tuint08 uiSubByte = uxTickCount.SubByte;
        /* It is possible that we had an interrupt since we saved the the subByte. We assume,
         * that in that case uiSubByte>uiOsStartTime. This may not be true if we had a very
         * long interrupt but that cannot be checked. We cannot check one full round of the timer,
         * since we do not want to spend bytes to keep track of that.
         * If, however the timer has an unhandled interrupt, we expect the portReadTimer() function
         * to return a timer value with cfgSysSubTicksPerFullTick added to the actual value. This
         * is 'easy' to implement. */
        if (uiSubByte<=uiOsStartTime)
        /* If we have a very slow timer, we could arrive in the same subtick here, thus the equality
         * should result in the addition of zero subticks instead of cfgSysSubTicksPerFullTick. */
        { uiTaskTime = uiOsStartTime - uiSubByte; }
        /* So here we past the cfgSysSubTicksPerFullTick barrier and must correct for the situation. */
        else
        { uiTaskTime = cfgSysSubTicksPerFullTick - uiSubByte + uiOsStartTime; }
        /* If we have other interrupts at hand, the system may have spend some time there as well. We should
         * correct the uiTaskTime accordingly. Note that we cannot distinguish between different interrupts
         * of succeeding interrupts.  */
        #if (cfgIntUserDefined == cfgTrue)
          /* uiIsrLoadTemp holds the time spend in isr */
          Tuint08 uiIsrTime = uiIsrLoadTemp;
          /* This time however was erroneously incorporated in the task time, so we must correct for that error,
           * again, we assume no multiple tick interrupts in between. The test in between it to make sure the
           * value is indeed deducible, which may not be the case in rare cases */
          if (uiTaskTime > uiIsrTime) { uiTaskTime -= uiIsrTime; }
          /* Add it to the LoadCollect */
          uiIsrLoadCollect += uiIsrTime;
          /* We have read out the value of uiIsrLoadTemp and must reset it for the next interrupt. */
          uiIsrLoadTemp = 0;
        #endif
        /* Which task was running lately? */
        TtaskControlBlock * oldTCB = privTcbList(defCurrentTaskNumber);
        /* OK, this was the last task that did run, but ... did it run the very last slice or was it the
         * somewhat longer ago and did the idle time run after that? We can see the difference by looking
         * at the RunState and running mode. We only count the time if the task is running and the state
         * is still runnable (may be delayed). Of course we miss the time of those tasks which just went
         * blocking. Since there is no other way to find out, we have to accept this (small) error. */
        if ((oldTCB->uiTaskStatus & (defBaseNoBlocksGetMask | defBaseDressGetMask)) == (defBaseNoBlocksTask | defBaseDressRunable) )
        { /* The RunState is still set, so indeed the calculated time was due to the running of that task */
          oldTCB->uiLoadCollect += uiTaskTime; }
        else
        { /* Now the last task was the idle task, so add the time to the idle collect. Btw we could
           * also have been sleeping that difference cannot be made. However, it does not matter, since the
           * time from wakeup until now could count for idle as well. The sleep time itself is lost.*/
          uiIdleLoadCollect += uiTaskTime; }
      #endif
      /* Here we arrive at the first instruction when all checks are off. We increment the tick counter
       * if a timer interrupt has taken place. This is indicated the the uiAction parameter. Most times
       * we arrive here it is namely due to a yield of delay or so, and we can skip that step. */
      if ((uiAction & defActionTickStateTick) == defActionTickStateTick) { privIncrementTick(); }
      /* DISCUSSION
       * Sometimes the GCC compiler uses a frame pointer when there is not real need. This costs a lot
       * of flash. With the measure below we try to convince gcc that a particular register is free.
       * This seems portable since it is a request the compiler may ignore.
       * See the explanation at the option for more information. */
      #if (cfgSysFramePointerCounterMeasures == cfgTrue)
        register volatile Tuint08 uiAssignmentStatus asm ("r6");
      #else
        Tuint08 uiAssignmentStatus;
      #endif
      /* The two most important steps of the scheduler are, incrementing the tick counter, and
       * determining which task has to run next. The former is done above, and this action possible
       * waked some tasks, the latter below. The switch context results in a new task to run, or
       * the idle task that may or, or the sleep mode that the device must be put in. */
      uiAssignmentStatus = privSwitchContext();
      /* First, we check if we are put to sleep, which is done in OS space btw. If we are to sleep,
       * we are not allowed to prepare a subbyte for timing measurement, since that variable is used
       * to pass the number of allowable tick blocks to sleep.*/
      #if (cfgUseLowPowerSleep == cfgTrue)
        if ((uiAssignmentStatus & defAssignmentSleep) == defAssignmentSleep)
        { /* We indeed have to go to low power sleep. Enter the sleep mode, we do not return from this call. */
          privEnterSleep(uxTickCount.SubByte); }
      #endif
      /* If we arrive here, we are left with two possibilities, either we go run a task or go to the
       * idle state. First however, we measure how long the OS took, since most tasks of the OS are
       * done by this time. */
      #if ((cfgCheckTiming == cfgTrue) || (cfgUseLoadMonitor == cfgTrue))
        Tuint08 uiOsTime;
        /* Read the timer again. */
        Tuint08 uiOsStopTime = portReadTimer();
        /* From the moment we filled uiOsStartTime until now, that is the time the
         * OS took to run. This may not even be one subtick, so the we must test for equality too. (i.e..
         * both times being equal probably means not even one subtick, contrary to a full round of the
         * subtick timer */
        if (uiOsStartTime <= uiOsStopTime)
        { uiOsTime = uiOsStopTime - uiOsStartTime; }
        else
        { /* Again, in this case the timer interrupt fired, but is not handled yet. Hmmm, I am not
           * even sure id this situation can happen, since this should be a hanging interrupt right?
           * Well anyway, it seems save to leave the calculation in anyway */
          uiOsTime = cfgSysSubTicksPerFullTick - uiOsStartTime + uiOsStopTime; }
        /* If you already used half of the tick here, there is probably not enough time to complete
         *  the following task. If we had a watchdog event however, we do not want to check, since these
         * context replacements may take a lot of time, and they are a one time event. */
        #if (cfgCheckTiming == cfgTrue)
          if ( (uiOsTime >= (cfgSysSubTicksPerFullTick / 2))  && ((uiAssignmentStatus & defAssignmentWatchdog) == defAssignmentAbsent) )
          { privShowError((fatOsTickRateTooHigh | errTaskStateNon) , errNoInfo ); }
        #endif
        #if (cfgUseLoadMonitor == cfgTrue)
          /* Having calculated the time spend in the OS, add it to the Collected times. */
          uiOsLoadCollect += uiOsTime;
          /* In order to make the next time measurement possible save the last value read
           * from the timer in the Subbyte. That will be used to calculate the time spend in
           * a particular task. Of course we make a small error because the next few instructions
           * are not really part of the task. However, the same applies to the instructions
           * after the context save so we assume (hope?) these errors cancel out. */
          uxTickCount.SubByte = uiOsStopTime;
        #endif
      #endif
      /* Now check if we must perform an idle task */
      if ((uiAssignmentStatus & defAssignmentIdle) == defAssignmentIdle)
      { /* if so, call for the EnterIdle, from which we do not return */
        privEnterIdle(); }
      /* Here we are done, the next thing is to enter the privEnterTask, in order to run
       * the task chosen */
    #else
      /* We are running in the true time slice mode. Good choice, now you are sure that no tasks
       * are being starved if more are running full swing in one priority. OK, so far for the
       * advertisements. The timer interrupt is variable in this mode, and reset every time a task,
       * or the os starts. Note that the interrupts may be (and are) irregular. Ticks, however,
       * are on average, in pace with the system clock.  */
      Tuint08 uiTaskTime;
      /* Determining how long the task took is a snap, since the timer was reset just before
       * the task started. And, we must also specify how long we want to wait before the next
       * timer interrupt. Now that does not matter much here, since (tick) interrupts are disabled
       * anyhow. */
      uiTaskTime = portReadAndResetTimer(defSubTicksMax);
      /* We assume the timer did not overflow. This may happen if you choose cfgSysSubTicksPerFullTick
       * too high. But you can easily check this, see below. In this timer model the subbyte collects
       * the subticks, from which the tick counter is driven. */
      uxTickCount.SubByte += uiTaskTime;
      /* If we keep track of the task times, the only thing to do is to add uiTaskTime to the collector */
      #if (cfgUseLoadMonitor == cfgTrue)
        /* If we have other interrupts at hand, the system may have spend some time there as well. We should
         * correct the uiTaskTime accordingly. Note that we cannot distinguish between different interrupts
         * of succeeding interrupts.  */
        #if (cfgIntUserDefined == cfgTrue)
          /* uiIsrLoadTemp holds the time spend in isr */
          Tuint08 uiIsrTime = uiIsrLoadTemp;
          /* This time however was erroneously incorporated in the task time, so we must correct for that error,
           * again, we assume no multiple tick interrupts in between. The test in between it to make sure the
           * value is indeed deducible, which may not be the case in rare cases */
          if (uiTaskTime > uiIsrTime) { uiTaskTime -= uiIsrTime; }
          /* Add it to the LoadCollect */
          uiIsrLoadCollect += uiIsrTime;
          /* We have read out the value of uiIsrLoadTemp and must reset it for the next interrupt. */
          uiIsrLoadTemp = 0;
        #endif
        /* Wich task was running lately? */
        TtaskControlBlock * oldTCB = privTcbList(defCurrentTaskNumber);
        /* OK, this was the last task that did run, but ... did it run the very last slice or was it the
         * somewhat longer ago and did the idle time run after that? We can see the difference by looking
         * at the RunState and running mode. We only count the time if the task is running and the state
         * is still runable (may be delayed). Of course we miss the time of those tasks which just went
         * blocking. Since there is no other way to find out, we have to accept this (small) error. */
        if ((oldTCB->uiTaskStatus & (defBaseNoBlocksGetMask | defBaseDressGetMask)) == (defBaseNoBlocksTask | defBaseDressRunable) )
        { /* The RunState is still set, so indeed the calculated time was due to the running of that task */
          oldTCB->uiLoadCollect += uiTaskTime; }
        else
        { /* Now the last task was the idle task, so add the time to the idle collect. Btw we could
           * also have been sleeping that difference cannot be made. However, it does not matter, since the
           * time from wakeup until now could count for idle as well. The sleep time itself is lost.*/
          uiIdleLoadCollect += uiTaskTime; }
      #endif
      /* in this timing model we always call privIncrementTick() since the ticks added to the tick counter
       * are deduced from the value of subbyte */
      privIncrementTick();
      /* DISCUSSION
       * Sometimes the GCC compiler uses a frame pointer when there is not real need. This costs a lot
       * of flash. With the measure below we try to convince gcc that a particular register is free.
       * This seems portable since it is a request the compiler may ignore.
       * See the explanation at the option for more information. */
      #if (cfgSysFramePointerCounterMeasures == cfgTrue)
        register volatile Tuint08 uiAssignmentStatus asm ("r6");
      #else
        Tuint08 uiAssignmentStatus;
      #endif
      /* The two most important steps of the scheduler are, incrementing the tick counter, and
       * determining which task has to run next. The former is done above, and this action possible
       * waked some tasks, the latter below. The switch context results in a new task to run, or
       * the idle task that may or, or the sleep mode that the device must be put in. */
      uiAssignmentStatus = privSwitchContext();
      /* First, we check if we are put to sleep, which is done in OS space btw. If we are to sleep,
       * we are not allowed to prepare a subbyte for time measurement, since that variable is used
       * to pass the number of allowable tick blocks to sleep.*/
      #if (cfgUseLowPowerSleep == cfgTrue)
        if ((uiAssignmentStatus & defAssignmentSleep) == defAssignmentSleep)
        { /* We indeed have to go to low power sleep. Enter the sleep mode, we do not return from this call. */
          privEnterSleep(uxTickCount.SubByte); }
      #endif
      /* If we arrive here, we are left with two possibilities, either we go run a task or go to the
       * idle state, Now check if we must perform an idle task .*/
      if ((uiAssignmentStatus & defAssignmentIdle) == defAssignmentIdle)
      { /* if so, first set the Timer with the right time for the idle task, which at the same time
         * returns the time spend so far in OS */
        Tuint08 uiOsTime = portReadAndResetTimer(TimeSliceIdleTime);
        /* This is of course time spend, so must be added to the tick counter */
        uxTickCount.SubByte += uiOsTime;
        /* If we monitor the load */
        #if (cfgUseLoadMonitor == cfgTrue)
          /* the time spend in OS must be added to the appropriate collector */
          uiOsLoadCollect += uiOsTime;
        #endif
        /* End we may enter the idle task. In this case we do not have the opportunity to check
         * upon the OS time. */
        privEnterIdle(); }
      /* Arriving here means we are going to perform a regular task. So we must prepare the timers
       * for that.*/
      Tuint08 uiTimeSlice;
      /* We have two options. Either we have specified the time slice for each task separately, or
       * we use the one fits all model. */
      #if (defTimeSliceConstant == cfgFalse)
        /* Get the task number we choose to run next */
        Tuint08 uiTaskNumber = (uiOsStatus & defTaskNumberGetMask) >> defOsTaskNumberShift;
        /* Get the value of the time slice from flash */
        uiTimeSlice   = portFlashReadStruc(TtaskDefinitionBlock,pxTDBlist[uiTaskNumber],Tuint08,uiTimeSlice);
      #else
        /* In the standard model, we give all tasks the same value, */
        uiTimeSlice = defTimeSliceFixed;
      #endif
      /* The time has come to determine how long the OS has run, reset the timer and set the new time slice */
      Tuint08 uiOsTime = portReadAndResetTimer(uiTimeSlice);
      /* The time spend in the OS must be added to the extended tick counter */
      uxTickCount.SubByte += uiOsTime;
      /* And, if we monitor the load ... */
      #if (cfgUseLoadMonitor == cfgTrue)
        /* to the collector of the OS load time as well */
        uiOsLoadCollect += uiOsTime;
      #endif
      /* Many things can go wrong with the timing, so the moment has come to check. */
      #if (cfgCheckTiming == cfgTrue)
        /* First, check if the time spend in the OS does not exceed one tick. Although it is not an error in the
         * strictest sense, it can do harm to the collected subticks in the subbyte, which may overflow, since
         * it cannot be cleaned on this point. This may happen if the time slice of the current task is very long.
         *  If we had a watchdog event however, we do not want to check, since these context replacements may
         * take a lot of time, and they are a one time event. */
        if ( (uiOsTime >= (cfgSysSubTicksPerFullTick)) && ((uiAssignmentStatus & defAssignmentWatchdog) == defAssignmentAbsent) )
        { /* thus report an error in this case, its fatal and you should slow down your tick timer/prescaler or
           * increase the value of cfgSysSubTicksPerFullTick. */
          privShowError((fatOsTickRateTooHigh | errTaskStateNon), errNoInfo); }
        /* Second, we test if there is enough room for the current time slice, i.e. if the subtick counter will
         * not overflow, even if the task runs its full length. The comparison seams awkward, but makes use
         * of the overflow of the unsigned integers arithmetic. */
        if (((Tuint08)(uiTimeSlice + uxTickCount.SubByte)) < uiTimeSlice)
        { /* if it does not fit report the error */
          privShowError((errTaskTakesTooLong | errTaskStateCurrent | errOsStateReturn), errCurrentTask  );
          /* We have a problem, since although the task is put in error mode it will still run, we cannot
           * stop it anymore on this point. What to do? Since our only concern was the length of the task,
           * we just start it for the last time, with a standard time slice. Of course we must add the subticks
           * already past */
          uxTickCount.SubByte += portReadAndResetTimer(cfgSysSubTicksPerFullTick); }
      #endif
      /* Here we are done, the next thing is to enter the privEnterTask, in order to run
       * the task chosen */
   #endif
  }
  /* make it so */
  privEnterTask(); }


static void privIncrementTick(void)
{ /* This the place where the tick counter is increased. This may be one or more ticks, depending of
   * the timing model in use. */
  #if (cfgUseEquidistantTicks == cfgFalse)
    /* Only in case we have true time slice model, more than one tick may be needed to add. For every
     * tick we fully loop the routine below. This is a little inefficient, but ensures we do not miss
     * a tick in the delay/wake section. If we do, we may miss to reactivate a delay task. */
    while (uxTickCount.SubByte >= cfgSysSubTicksPerFullTick)
  #endif
  { /* In case of true time slicing we use the SubByte to collect all subticks, otherwise there is
     * no need */
    #if (cfgUseEquidistantTicks == cfgFalse)
      uxTickCount.SubByte -= cfgSysSubTicksPerFullTick;
    #endif
    /* Every time we arrive here we must increase the tick counter by one. Or we arrived here because
     * subbyte overflowed the cfgSysSubTicksPerFullTick or we arrived here because we were send here
     * from a tick interrupt. */
    ++uxTickCount.LowByte;
    /* Call the tick hook. Please note that at this point the tick counter is incomplete, */
    #if (callAppTick00 == cfgTrue)
      appTick00();
    #endif

    /* if the low byte overflows ... */
    if (uxTickCount.LowByte == 0 )
    { /* ... Increase the high byte */
      ++uxTickCount.HighByte;
      /* every 256 ticks we must call the tick08 hook */
      #if (callAppTick08 == cfgTrue)
        if (uxTickCount.LowByte == 0) { appTick08(); }
      #endif
      /* every 256*256 ticks we must call the appTick16 hook */
      #if (callAppTick16 == cfgTrue)
        if (uxTickCount.HighByte == 0) { appTick16(); }
      #endif
      /* On every cross of the low byte boundary, new tasks may have entered the near wake
       * state, thus we must set the corresponding bit. */
      uiOsStatus = ((uiOsStatus & defNearWakePresentSetMask) | defNearWakeStatePresent);
      /* If we make use of the watchdog facility */
      #if (cfgUseTaskWatchdog == cfgTrue)
        /* we test if we already arrived at a watchdog decrease boundary. This test simple passes when
         * the HighByte ends with the number of zeros given by  cfgNumWatchdogDiv */
        if ( (uxTickCount.HighByte & (0xFF >> (8 - cfgNumWatchdogDiv))) == 0)
        { /* if so, now loop trough all tasks who might have a watchdog */
          Tuint08 uiLoopTask;
          for (uiLoopTask=defTaskNumberWatchdogBegin; uiLoopTask<defTaskNumberWatchdogEnd; uiLoopTask++)
          { TtaskControlBlock * loopTCB = privTcbList(uiLoopTask);
            /* We may only decrease the watchdog state of running tasks, If a task is delayed, suspended
             * or otherwise blocked, it can never feed its watchdog so we should not decrease its counter. */
            if (loopTCB->uiTaskStatus >= defBaseRunningTask)
            { /* Extract the value of the watchdog counter which can only take four states */
              Tuint08 uiWatchdog = loopTCB->defDogField & defDogGetMask;
              /* The value of zero (defDogDead) means we do not use the watchdog, the value one (defDogBark) means
               * it is already barking, so it should not be decreased further rendering it inactive,*/
              if (uiWatchdog > defDogBark)
              { /* so only on higher values, decrease its value, which may result in a barking watchdog */
                loopTCB->defDogField = (loopTCB->defDogField & defDogSetMask) | (uiWatchdog - defDogDec); } } } }
      #endif
      /* If we make use of the load monitor facility */
      #if (cfgUseLoadMonitor == cfgTrue)
        /* we test if we already arrived at a watchdog monitor boundary. This test simple passes when
         * the HighByte ends with the number of zeros given by  cfgNumMonitorDiv, we must
         * copy all values from the collectors to the totals. */
        if ( (uxTickCount.HighByte & (0xFF >> (8 - cfgNumMonitorDiv))) == 0) { privCopyLoad(); }
      #endif
    /* This concludes the activity on the high byte (256 ticks) boundary */
    }
    #if (defUseDelay == cfgTrue)
      /* Now we must check if we must wake some delayed tasks, this only is the case when we
       * have near wakes. The mechanism of near wakes is made in order not to have to check all
       * tasks on every tick */
      if ((uiOsStatus & defNearWakePresentGetMask) == defNearWakeStatePresent)
      { Tuint08 uiNearWakeCount = 0;
        Tuint08 uiLoopTask;
        /* we must find out which tasks are near wake, thus we must check all tasks */
        for (uiLoopTask=defTaskNumberDelayBegin; uiLoopTask<defTaskNumberDelayEnd; uiLoopTask++)
        { TtaskControlBlock * loopTCB = privTcbList(uiLoopTask);
          /* We check if the task is delayed. Note that it need not to be running. If the task was suspended or
           * sleeping while delayed, this delay simply ends, but the task remains suspended/sleeping. The same
           * applies for killed tasks. Tasks that are blocked may have a timeout, so this must be checked as well. */
          if ((loopTCB->uiTaskStatus & defBaseDelayStateGetMask) == defBaseDelayStateDelayed)
          { /* a task from which the high byte does not much is not near wake */
            if (loopTCB->uxDelay.HighByte == uxTickCount.HighByte)
            { /* if it matches, we assume that the task must be waked, since we do not allow so long delays that the
               * end time is, after overflow, just before the current time in the same block. (The need for this has
               * several reasons, one of which is that this allows us to skip ticks within a tick block. It is needed
               * for recovery from low power sleep too. The former facility is not really used right now */
              if (loopTCB->uxDelay.LowByte <= uxTickCount.LowByte)
              { /* We have a winner, we may give a wakeup call, but first we check synchronization */
                privWakeupFromDelay(uiLoopTask,loopTCB); }
              else
              { /* arriving here means the particular task has a wake time in the near future, thus we may
                 * increase the near wake counter. */
                ++uiNearWakeCount; } } } }
        /* If we had no more near wakes we may clear the near wake bit. If we do not do this, it remains set, as
         * it was for certain when arriving here. If we clear the bit will be set again when we enter the
         * next tick block. */
        if (uiNearWakeCount==0) { uiOsStatus = ((uiOsStatus & defNearWakePresentSetMask) | defNearWakeStateAbsent); } }
    #endif
  } }


static Tuint08 privSwitchContext(void)
{ /* We arrive here when we may want to do a context switch. If we do not want to switch, we are
   * always running a task, so that is the default return state. Here we fill in in, so we can
   * modify it to our needs. */
  Tuint08 uiContextResult = defAssignmentTask;
  /* If there is no possibility the SwitchAllow is modified, there is no need to check */
  #if (includeTaskProtectSwitchTasks == cfgTrue) || (includeTaskProtectSwitchCritical == cfgTrue)
   /* If the block switch bit is up we want to skip this apart, one task has complete access to
    * the device capacities. */
    if ((uiOsStatus & defSwitchAllowGetMask) == defSwitchStateRunning)
  #endif
  { /* DISCUSSION
     * Where to put he watchdog check was a matter of considerable inner debate of mine. Is the
     * privSwitchContext the right place? In any case, we must not do it after we determined
     * what the next task to run will be. We could have done it in the privInitOS, probably it
     * results in smaller code over there. On the other side, we want to keep privInitOS as lean as
     * possible by itself. We could have done it in privSwitchContext since there is no need
     * to check broken tasks after. In any case, a call to privTaskInit eats a lot of stack so
     * we want to do it at a place where the stack is fresh. Anyway i decided that this place
     * is not that bad. */

    /* ... determine which task was running */
    TtaskControlBlock * oldTCB = privTcbList(defCurrentTaskNumber);
    /* If we make use of the watchdog ... */
    #if (cfgUseTaskWatchdog == cfgTrue)
      /* check if the watchdog barked (in this way we make sure we only check tasks which
       * actually did run, so at least had a chance to feed the watchdog. A blocking task
       * therefore should not be barking. Exception is, if the task just called for some
       * unavailable slot, and went blocking as last action. Likewise, the task could also be
       * delayed as last action (the task normally is started with bark activated as last
       * action, since bark is only checked upon tasks that have run.)  Furthermore, a
       * terminated task can only issue a bark if it was not stuck but running without
       * watchdog feeding. This situation is extremely rare, but not impossible. Such a
       * task schould stay terminated. End of the story, we have to recheck if the task is
       * indeed still running. Test if the task is running and if the watchdog barks. */
      if ( (oldTCB->uiTaskStatus >= defBaseRunningTask) && ((oldTCB->defDogField & defDogGetMask) == defDogBark))
      { /* Report that the watchdog is barking. The task number can be deduced from the following TaskInit report. */
        privTrace(traceWatchdog);
        /* We need the task nunmber of the task that just did run. */
        Tuint08 uiOldTaskNumber = (uiOsStatus & defTaskNumberGetMask) >> defOsTaskNumberShift;
        /* Re-initialize the task, (but keep its priority). The rest of the state will be restored as if
         * the task was created for the first time. All hold locks will be released, blocking and delays
         * are removed, with out exception. The task could be in block waiting for the burnbock to release.
         * We want to keep the status partially (priority etc) and the watermarks. */
        privTaskInit(uiOldTaskNumber,(defInitStatusKeep | defInitLockRelease | defInitWatermarksKeep));
        /* If so get the pointer to the bark code from flash, and call it, if there is something to call */
        #if (callAppBark == cfgTrue)
          portFlashReadWord(fpBarkTask,pxBarklist[uiOldTaskNumber-defTaskNumberWatchdogBegin])();
        #endif
        /* In case we check timing we must inform the caller that we may have spend considerable time in
         * the watchdog routine. Since this is a one time event (or should be rare at least, is is better
         * if it not triggers an error. */
        #if (cfgCheckTiming == cfgTrue)
          uiContextResult |= defAssignmentWatchdog;
        #endif
      }
    #endif
    /* ... its state to done. If we previously run the idle task, the old task is already
     * done, but that does not matter. Since the dress runnable bit has only the runnable meaning
     * when the state is indeed not blocked (running, may be delayed) we may only set it to
     * done in that case. In blocking situations the bit is used for different purposes.
     * So there tasks must be skipped. We can save some bytes if there is no possibility for
     * blocking states. */
    /* TODO v0.90 Calls on the heap may block also */
    #if (cfgUseSynchronization != cfgSyncNon) || (cfgUseFileSystem  ==  cfgTrue)
      if (oldTCB->uiTaskStatus >= defBaseNoBlocksTask)
    #endif
    { oldTCB->uiTaskStatus = (oldTCB->uiTaskStatus & defBaseDressSetMask) | defBaseDressDone; }
    Tuint08 uiLoopTask;
    /* MaxTask is variable which will hold the task with the highest priority, Initialization of this variable
     * is strictly spoken not necessary, however the compiler does not understand this, so let us give
     * is some harmless value. */
    Tuint08 uiMaxTask = 0;
    /* Now this is the important variable. It is set to the highest priority of a non runnable task, the default
     * so to say. If it is not increased, we have no runnable task, thus we must run the idle task. By using this
     * approach we do not need a real idle task, with stack and so. */
    Tuint08 uiPrio = defBaseRunningTask - 1;
    /* Now loop trough all tasks and determine which has the highest priority. Since the
     * priority information is just before the bit 0, which determines the run state, all
     * tasks with equal priority are issued round robin. Tasks with a status below defRunableTask
     * will never be started, they are typically the blocked, delayed suspended, error or
     * sleeping tasks. */
    for (uiLoopTask=0; uiLoopTask<defNumberOfTasks; uiLoopTask++)
    {  TtaskControlBlock * loopTCB = privTcbList(uiLoopTask);
      /* The 'smaller sign' is important here, only if the priority exceeds the smallest possible
       * runnable task a replacement must take place. */
      if (uiPrio<loopTCB->uiTaskStatus)
      { /* this is the most important task so far */
        uiMaxTask=uiLoopTask;
        /* and this is its priority */
        uiPrio = loopTCB->uiTaskStatus;  } }

    /* OK, now it is possible that there are no runnable tasks. In that case we must run the
     * idle task (which is not really a task, but merely a state in OS space, it does not have
     * a task control block) or we must enter the sleep state. */
    if (uiPrio == (defBaseRunningTask - 1))
    { /* DISCUSSION
       * If we arrive here, all the tasks are terminated, suspended, sleeping, blocked or delayed.
       * (Thus, you cannot arrive here if there are running tasks, this is important for a correct
       *  understanding of the decisions made below.)
       * The first situation can never recover, the next three cannot recover on their own, thus
       * the delayed tasks are their only hope. Thus we may go to sleep directly, or, when the
       * the minimum delay is long enough, for that time at least. Interrupts can revive
       * locked tasks may do so through the sleep anyway. Note that we disallowed sleep in case
       * of any write-file system activity or coming activity.
       * So see if we allow for low power sleep. If there are no tasks we cannot decide to go to sleep
       * by looking at the tasks. In fact, we should not sleep, maybe the idle loop has something
       * sensible to do, otherwise we just as well switch off the power. */
      #if (cfgUseLowPowerSleep == cfgTrue) && (defNumberOfTasks > 0)
        /* Set the minimum delay to 255 block ticks, this is the largest value per default. */
        Tuint08 uiMinDelay = defSleepMaximum;
        /* If there is any fs activity we do not need to test any further, so sort this out. If any
         * writing is going on, we may not fall asleep. Reading is permitted (read tasks cannot block
         * because of reading solely, only in combination of an other task writing or an other synchronization
         * primitive). Since we know there are no running tasks when arriving here, we only need to check
         * if there is not write block present.*/
        #if (cfgUseFileSystem  ==  cfgTrue)
          /* If the file system is in write block, there are tasks writing to the FS and we cannot put the
           * system into a sleep mode. If there is a read block, this is no problem by itself.
           * If we have a burn block, there must be a write block too, so we do not need to check
           * for burning separately.*/
          if ((uiFsStatus & defFsWriteBlockGetMask) == defFsWriteBlockActive)
          { /* Setting uiLoopTask to zero blocks failing to sleep in the test below. */
            uiLoopTask = 0; }
          else
        #endif
        { /* Now loop through all tasks to test if a sleep is justified. */
          for (uiLoopTask=0; uiLoopTask<defNumberOfTasks; uiLoopTask++)
          { TtaskControlBlock * loopTCB = privTcbList(uiLoopTask);
            #if (cfgUseFileSystem  ==  cfgTrue)
              /* Test first if we do not have a FS block, if so, no sleep. For we must first handle any
               * task that is waiting for an opportunity to start writing. Task blocking for reading
               * operations should not hinder falling asleep. However, there cannot be a situation where
               * all tasks are solely blocked on reading. One read block implies an other task blocked
               * on writing. And even if that where possible we should not go to sleep since we cannot
               * store the blocked state. Tasks actually doing reading (but blocked or delayed) are
               * NOT FileBlocked, thus pass here undetected. When some tasks are actually writing
               * we never arrive here. End of the story is that any file block should hinder sleep.
               * The  defBaseDressGetMask is not important. */
              if ( (loopTCB->uiTaskStatus & defBaseFileBlockedGetMask) == defBaseFileBlockedTask ) { break; }
            #endif
            /* We can only arrive here if all tasks (until now) are terminated, suspended, sleeping of syncblocked.
             * (fs blocks are already filtered out). The task could be delayed or syncblocked, the latter with or
             * without delay. We cannot reach this place if there is any task that is able to run.
             * Here three situations are important. No delay, delay with UseLowPowerOnDelay or delay without UseLowPowerOnDelay.
             * First if delayed tasks are not possible then, there is nothing left to do. If we come here, this task
             * cannot hinder going to sleep anymore.
             * In the second situation, where we allow a sleep on (long) delays, we must determine the shortest delay among
             * all, where we only take tasks into account that will be reviving without the help from outside, i.e.
             * standard  delayed tasks, or blocked delayed tasks (not on FS).
             * Third, if we do not allow a sleep on (long) delays, we must determine if we really do not have any delayed
             * tasks, thus  check if there is a delay task, if so, we may not go to sleep. If there is no delayed
             * task, we could only arrive here (idle) if ALL tasks are stopped, suspended, sleeping of syncblocked.
             * Only an external event can revive any of them. We may go to sleep.
             * Note that it does  matter if blocked state is delayed, since a delayed blocked state may revive
             * earlier, but only with help from an other task or from the outside. But it will certainly be revived
             * when its timer expires. We thus must treat is as if it is a normal delayed task.
             * It must however not be a stopped delayed task, since even if the timer expires it will never restart. */
            #if (cfgUseDelay == cfgTrue)
              /* Test if we are in a delayed situation, that is not terminated or sleeping (fs is not possible any more)
               * but may be syncblocked. */
              if ( (loopTCB->uiTaskStatus & (defBaseStopStateGetMask | defBaseModeGetMask | defBaseDelayStateGetMask)) == ( defBaseStopStateGo |  defBaseModeSync | defBaseDelayStateDelayed ) )
                #if (cfgUseLowPowerOnDelay == cfgTrue)
                { /* Second situation, we must determine the delay. We do not care for the low byte, this 256 ticks vanish in
                   * the rounding. One tick block must at least be the delay.  The calculation is correct, even if
                   * loopTCB->uxDelay.HighByte < uxTickCount.HighByte do to the limited size of the type. */
                  Tuint08 uiDelayTime = loopTCB->uxDelay.HighByte - uxTickCount.HighByte;
                  /* If the high bytes are equal, there will be no delay, in fact that task will be waked
                   * [Is this possible at all, it seems not so, cause these tasks should have been waked earlier on ]
                   * Delays as long as 0xFF00 or more are forbidden. */
                  if (uiDelayTime == 0 ) { break; }
                  /* The time the device may be put to sleep is of course one tick block less otherwise, we could
                   * be to late for the task to wake, thus we subtract one. */
                  uiDelayTime--;
                  /* We have an option which specifies a lower bound on the sleep time. Sleep times below that
                   * are not thought to be useful, thus if we find such a time, we might as well wait here */
                  if (uiDelayTime < defSleepThreshold ) { break; }
                  /* We want to keep track of the smallest possible sleep time so far, cause that will be reported
                   * to the portSleep method. */
                  if (uiMinDelay>uiDelayTime) { uiMinDelay = uiDelayTime; } }
                #else
                { /* Third situation, a present delay must prohibit going to sleep. */
                  break; }
                #endif
            #endif
          }
        }
        /* No we must see if we did not break out of the loop. If we did not ... */
        if (uiLoopTask == defNumberOfTasks)
        { /* ... we indeed may go to sleep, the the caller so */
          uiContextResult |= defAssignmentSleep;
          /* and let the portSleep method know what the longest possible sleep time is, for
           * which no tasks will be missed (note the you may sleep longer if you want). The highest
           * value that can emerge from a delay is 0xFE (from 0xFExx) so that we know that a value of
           * defSleepMaximum (0xFF) must mean that no delay was present at all.
           * Now we can use the subbyte to pass this information, since if we are going to sleep, the
           * information will be useless anyway. The tick counter can be corrected manually, but
           * we cannot expect it to be accurate up to the subtick */
           uxTickCount.SubByte = uiMinDelay; }
        else
      #endif
    { /* If you arrive here, then or you did not allow for a sleep mode, or the conditions
       * to go go low power sleep are just not met. This implies running the idle task,
       * we are done, let the caller know. */
      uiContextResult |= defAssignmentIdle; } }
    else
    { /* If you arrive here, then there is a task to be run. But it could be that all tasks
       * in the current priority have already run, so that we selected a task in the state
       * defRunStateDone. If this is the case then we know vice versa that all tasks in
       * that priority have that state, for otherwise it would have been selected due to the
       * higher value of the status byte. */
      if ((privTcbList(uiMaxTask)->uiTaskStatus & defBaseDressGetMask) == defBaseDressDone)
      { /* In fact we need only to restore the run state of the current priority. It
         * is however more work to check the priority, and it does not matter that we
         * reset the run states of all lower priorities as well. [Well it could matter in special
         * cases where one particular lower priority task revives higher priority tasks, which,
         * when done, get to the same lower priority task every time, thereby skipping the other
         * lower priority tasks. But we do not have hierarchical round robin, thus we leave
         * this as is. ] However, we must take care  that only running tasks are handled.
         * For the blocking tasks the dressbit is used for other purposes. */
        for (uiLoopTask=0; uiLoopTask<defNumberOfTasks; uiLoopTask++)
        { /* So get the task ... */
          TtaskControlBlock * loopTCB = privTcbList(uiLoopTask);
          /* Test if may have a blocking of some kind. */
          #if (cfgUseSynchronization != cfgSyncNon) || (cfgUseFileSystem  ==  cfgTrue)
            /* If so make sure the present task is not blocked. */
            if (loopTCB->uiTaskStatus >= defBaseNoBlocksTask)
          #endif
          { /* ... and set the RunState to Runable */
            loopTCB->uiTaskStatus = (loopTCB->uiTaskStatus & defBaseDressSetMask) | defBaseDressRunable; } } }
        /* Having selected a new state to run, let us put in the status register. From
         * now on, that is the current task.  */
        uiOsStatus = (uiOsStatus & defTaskNumberSetMask) | (uiMaxTask << defOsTaskNumberShift);  } }
  /* Tell the caller the result, which may be one of: defAssignmentIdle or defAssignmentSleep or
   * (and that was the default defAssignmentTask) */
  return uiContextResult; }


#if (cfgUseLowPowerSleep == cfgTrue)

static void privEnterSleep(Tuint08 uiTickMinDelay)
{ /* If you arrive here, you obviously wanted the device to go to sleep.
   * Here we make the last preparations before we can do so.
   * The OS stack is not relevant anymore, so to allow for maximal
   * depth on in the portSleep function, we want to reset it. This
   * however cannot be done with interrupts activated, since that
   * would open the possibility of an interrupt on an incomplete
   * stack. Thus the first thing to do is to deactivate the interrupts,
   * which is only necessary if they are active at all */
  #if (cfgIntOsProtected == cfgFalse)
    /* We want timer interrupts
     * disabled during sleep. The latter is the default situation, thus we do not
     * need to change anything to the tick interrupts.
     * If we come here, global interrupts where enabled (no OS protection) thus
     * we must disable them explicitly. Timer interrupts where already disabled in
     * the privInitOs section, thus there is no need to repeat that. */
    privDisableGlobalInterrupts();
  #endif
  /* Let the system know we are about to enter the sleep mode, this information
   * is needed when we wakeup and restore the system state, since we always enter
   * the OS with privInitOS */
  uiOsStatus = ((uiOsStatus & defContextSetMask) | defContextStateSleep);
  /* report we are going to bed */
  privTrace(traceSleepStart);
  /* Now we may reset the stack, no fear for interrupts. */
  privSetStack(&xOS.StackOS[OSstackInit]);
  /* Before we go to sleep, there may be some work to do, if a hook is installed */
  #if (callAppEnterSleep == cfgTrue)
    appEnterSleep();
  #endif
  /* If we wake from sleep, we want to be sure there is no SleepRequest left. We know that we could
   * never arrive here when we where writing, but it could be that the system is in read only mode.
   * This must be preserved. */
  #if (cfgUseFileSystem == cfgTrue)
    uiFsStatus = (uiFsStatus & defFsSleepRequestSetMask) |  defFsSleepRequestClear;
  #endif
  /* This is where it happens. Call (not jump to!) portSleep. This method
   * returns when the sleep is done. Note we enter the portSleep method with
   * global interrupts disabled, and we should return that way. In between the
   * interrupts may be enabled when needed. */
  portSleep(uiTickMinDelay);
  /* After we have slept, there may be some work to do, if a hook is installed */
  #if (callAppEnterSleep == cfgTrue)
    appExitSleep();
  #endif
  /* If we return, make the jump here */
  portJump(privTickYield); }

#endif


static void privEnterIdle(void)
{ /* Arrving here, we want to put the device in idle mode.
   * Here we make the last preparations before we can do so.
   * The OS stack is not relevant anymore, so to allow for maximal
   * depth on in the portIdle function, we want to reset it. This
   * however cannot be done with interrupts activated, since that
   * would open the possibility of an interrupt on an incomplete
   * stack. Thus the first thing to do is to deactivate the interrupts,
   * which is only necessary if they are activated at all.
   * If we have OS protection, the interrupts are already off, and\
   * the timer interrupts are permanently on. If not, we must take
   * care of that. */
  #if (cfgIntOsProtected == cfgFalse)
    privDisableGlobalInterrupts();
  #endif
  /* Timer interrupts are switch on (or must be on) in the idle mode
   * because we must return from idle due to a timer interrupt.
   * however, that is the responsibility of the portIdle implementation
   * Notify the OS that we enter the idle state now. This is needed at
   * return, cause we don't need to save any context then. */
  uiOsStatus = ((uiOsStatus & defContextSetMask) | defContextStateIdle);
  /* Say that we are ready to start the idle task. */
  privTrace(traceIdleStart);
  /* Reset the stack, so we maximal space in portIdle. */
  privSetStack(&xOS.StackOS[OSstackInit]);
  /* Interrupts on of course, otherwise the timer interrupt cannot work, and
   * we are ready with the stack switch. */
  privEnableGlobalInterrupts();
  /* Before we enter the idle state the there may be some work to do */
  #if (callAppEnterIdle == cfgTrue)
    appEnterIdle();
  #endif
  /* Done, we goto (not call!) the portIdle. Thus, we do not return here. ;-) */
  portJump(portIdle); }


#if (defNumberOfTasks == 0)

/* Use an empty method test purposes if there are no tasks defined, we should never come here. */
static void privEnterTask(void) { while(true); }

#else

static void privEnterTask(void)
{ /* Arrive here when a task need to be executed.
   * Here we make the last preparations before we can do so.
   * The OS stack must be changed to the task stack. This
   * however cannot be done with interrupts activated, since that
   * would open the possibility of an interrupt on an incomplete
   * stack. Thus the first thing to do is to deactivate the interrupts,
   * which is only necessary if they are activated at all.
   * Timer interrupts are switch on (or must be on) in the task mode. This however is done
   * inside the portRestore method for its state is dependent on the previous interrupt
   * state of the task. Therefore, we keep (default situation) tick interrupts
   * disabled here. If we have OS protection, the interrupts are already off
   * If not, we must take care of that. */
  #if (cfgIntOsProtected == cfgFalse)
    privDisableGlobalInterrupts();
  #endif
  /* Get the number of the task we are going to run */
  Tuint08 uiTaskNumber = (uiOsStatus & defTaskNumberGetMask) >> defOsTaskNumberShift;
  /* Get the task control block of the current task also */
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* Notify the OS that we enter a task state now. This is needed at
   * return, cause we must save the context then. */
  uiOsStatus = ((uiOsStatus & defContextSetMask) | defContextStateTask);
  /* Say that we are ready to start the task, but we do this before we switch to the
   * task since we do not know if the privTrace action fits on the task start. */
  privTrace(traceTaskStart | uiTaskNumber);
  /* If we make use of register compression with constant registers, (remember the facility that saves
   * stack space by saving only those registers you use on the context), ... */
  #if (defRegisterUseConstant == cfgTrue)
    /* ... we can use the constant value */
    xOS.pxSave.uiRegisterUse = defRegisterUseCollect;
  #else
    /* otherwise we must extract it from flash. */
    xOS.pxSave.uiRegisterUse = portFlashReadStruc(TtaskDefinitionBlock,pxTDBlist[uiTaskNumber],Tuint16,uiRegisterUse);
  #endif
  /* If we check the use of registers we may not want to check all registers, even if they are used. Here we may
   * want to check for all tasks the same registers so ...*/
  #if (cfgCheckRegisters == cfgTrue)
    #if (defRegisterCheckConstant == cfgTrue)
      /* ... we can use the constant value */
      xOS.pxSave.uiRegisterCheck = defRegisterCheckCollect;
    #else
      /* otherwise we must extract it from flash. */
      xOS.pxSave.uiRegisterCheck = portFlashReadStruc(TtaskDefinitionBlock,pxTDBlist[uiTaskNumber],Tuint16,uiRegisterCheck);
    #endif
  #elif  (cfgCheckWatermarks == cfgTrue)
    /* If we collect watermarks and do not check the registers, we want to collect this
     * information from all registers, thus */
      xOS.pxSave.uiRegisterCheck = (registersAll);
  #endif
  /* At saveContext we need to know where the stack starts, but since getting this info
   * may overflow the stack at that time, we get it here and save it on the OS stack which
   * is used as background storage for the task. */
  xOS.pxSave.pcStackOffset = portFlashReadStruc(TtaskDefinitionBlock,pxTDBlist[uiTaskNumber],Taddress,pcStackOffset);
  /* Some methods return a boolean to the application code. This data is put in the
   * correct register by the restoreContext. At this point we prepare it */
  #if (cfgUseSynchronization != cfgSyncNon) && (defUseBoolReturns == cfgTrue)
    /* Put that information in the background variables */
    xOS.pxSave.uiReturn = curTCB->defRetField & defRetGetMask ;
  #endif
  /* If we do some checks on the use of the task stack (which are done on portSaveContext)
   * we prepare that information here, and put it in the background variables */
  #if (cfgCheckTaskStack == cfgTrue) || (cfgCheckWatermarks == cfgTrue)
    /* Stacksize is needed on two occasions */
    #if (defStackSizeConstant == cfgTrue)
      Tstack uiStackSize = defStackSizeFixed;
    #else
      Tstack uiStackSize = portFlashReadStruc(TtaskDefinitionBlock,pxTDBlist[uiTaskNumber],Tstack,uiStackSize);
    #endif
  #endif
  #if (cfgCheckTaskStack == cfgTrue)
    /* Calculate the limit for the stack size. We do that calculation also here, so that,
     * in the portSaveContext, we can do a simple compare. */
    #if (cfgSysStackGrowthUp == cfgTrue)
      /* TODO: cfgSysStackGrowthUp oops, add code */
      xOS.pxSave.pcStackLimit = 0;
    #else
      #if (defRegisterUseConstant == cfgTrue)
        /* The constant defRegisterCount holds the number of registers used.*/
        Tuint08 uiRegCount = defRegisterCount;
      #else
        /* We have to calculate the number dynamically */
        Tuint08 uiRegCount = privRegisterCount(xOS.pxSave.uiRegisterUse);
      #endif
      /* Note that the limit is calculated taking StackSavety into account. Thus, it is an error
       * to pass this limit, although the stack is not yet corrupted at that point. This is
       * intentional, so that we can stop the task rather than experiencing bizarre results,
       * when experimenting with the stack size. The +1 represents the status register.*/
      xOS.pxSave.pcStackLimit = uiRegCount+xOS.pxSave.pcStackOffset+StackSafety-uiStackSize+1;
    #endif
  #endif
  /* Here we will scan the stack for bytes that differ from the initial bye. This indicates the use of
   * the stack also on places where no interrupt is possible (at the context the stack is used heavily,
   * but it may not give the absolute top if tick interrupts are disabled during deep calls. */
  #if (cfgCheckWatermarks == cfgTrue)
    #if (cfgSysStackGrowthUp == cfgTrue)
      /* TODO: cfgSysStackGrowthUp oops, add code */
    #else
      /* The watermark measurement starts from the top of the stack, which is below the stack offset */
      Taddress uiTopOfStack = xOS.pxSave.pcStackOffset - uiStackSize;
      /* the stack is write, post decrement, thus, when we walk downwards, we must read, pre-increment.
       * We walk maximally the stack size downwards. If we encounter a value which differs from the
       * initial value, we can stop. */
      while ( (uiStackSize--) && (*(++uiTopOfStack) == defStackInitByte) );
      /* The level of used is bytes is at least the number of bytes below the boundary found. Note
       * that this calculation cannot correctly reproduce an empty stack, but therefore is no real
       * need. The minimum stack watermark thus equals one.
       * Furthermore, we assume the stack has not overflown, first it may produce erroneous results in
       * the calculation, and second, watermarking is a technique to detect how much stack space is
       * left, not to detect stack overflow.*/
      Tstack uiCurrentStackLevel = (Tstack) ((Tuint16) xOS.pxSave.pcStackOffset - (Tuint16) uiTopOfStack + 1);
      /* If we happen to find a higher level with this technique opposite to the level found
       * due to context save, there must have been a situation of heavy stack use inside a tick
       * protected region of the code. Good that we noticed it! */
      if ((curTCB->uiStackMax)<(uiCurrentStackLevel)) { curTCB->uiStackMax = (uiCurrentStackLevel); }
    #endif
  #endif
 /* Recall the address stack level, and put it in the background variables. Thus this
  * variable gets accessible from the portSave/portContext routines. */
 xOS.pxSave.pcStackLevel = (Taddress) &(curTCB->pcStackLevel);
 /* Now, switch to the task stack */
 #if (cfgSysStackGrowthUp == cfgTrue)
   privSetStack( (Taddress) ((Tuint16) xOS.pxSave.pcStackOffset + (Tuint16) curTCB->pcStackLevel) );
 #else
   privSetStack( (Taddress) ((Tuint16) xOS.pxSave.pcStackOffset - (Tuint16)  curTCB->pcStackLevel) );
 #endif
 /* We need not to activate the interrupts right here, since that is done in the
  * portRestoreContext at the end by the 'reti'. Jump to the routine that restores
  * the context for the task at hand. */
  portJump(portRestoreContext); }

#endif


#if (defUseDelay == cfgTrue)

static void privWakeupFromDelay(Tuint08 uiTaskNumber, TtaskControlBlock * taskTCB)
{ /* Call this if you want to wake up a task due to expiration of the delay timer.
   * That can be or a simple delay, or a task that has a time out on a block.
   * taskTCB must be the tcb that corresponds to the uiTaskNumber. */
   #if ((cfgUseSynchronization != cfgSyncNon) || (cfgUseFileSystem == cfgTrue)) && (cfgUseTimeout == cfgTrue)
     /* If the task was in blocked state, it may be released for its timeout has passed. Note that
      * blocked tasks without a timeout are not delayed, so they never arrive here. The same holds for terminated
      * tasks, so unblocking a terminated task cannot result in a suspended task. Sleeping tasks may still be unblocked.
      * Resuming, this are the possibilities
      * (1) Terminated task: never delayed, should not arrive here.
      * (2) Suspended task: never blocked, cannot pass the test below.
      * (3) Sleeping, may be blocked, is always syncblocked, thus may timeout to free-sleeping
      * (4) FS block: may deblock to free, we must clear the FsFields
      * (5) Sync block: may deblock to free, we must clear the blocking slots
      * (6) Simple delay, just clear the delay.
      * */
      if ((taskTCB->uiTaskStatus & defBaseBlockStateGetMask) == defBaseBlockStateBlocked)
      { /* We can savely assume we only enter this part of the code if the present task indeed
         * has a slotstack. If not, the task could never be blocked. so we dont check. Since
         * privRestoreInitialPriority and privUnblockTask assume such uiTaskSlot is present,
         * they may crash if the fields do not exist. */
        /* If we have priority lifting .. */
        #if (cfgUseSynchronization != cfgSyncNon) && (cfgUsePriorityLifting == cfgTrue)
          /* ... and if we release, its priority may be changed (we don't know for sure) in any case
           * it must be restored. For the moment however we only support priority lifting on sync blocked
           * tasks. Thus we must test if we indeed have syncblocking. We included sleeping in this task
           * for all sleeping tasks must come from syncblocks.*/
          if ((taskTCB->uiTaskStatus & (defBaseModeGetMask | defBaseDressGetMask)) == (defBaseModeSync | defBaseDressSlot))
          { privRestoreInitialPriority(uiTaskNumber); }
        #endif
        /* Release the task by unblocking and unlocking and indicate that it was release by a timeout
         * so the requested lock was not obtained (return false) There is no need to check if other
         * tasks must be released since we remove a blocking task, which cannot have consequences
         * for other blocked/waiting tasks.*/
        privUnblockTask(uiTaskNumber | defParaLockStateUnlock | defParaRetStateFalse); }
      /* The unblocking above also wakes the task so we may skip it here. */
      else
   #endif
   /* wake the task */
   { taskTCB->uiTaskStatus = (taskTCB->uiTaskStatus & defBaseDelayStateSetMask) | defBaseDelayStateWake; } }

#endif


#if (cfgUseLoadMonitor == cfgTrue)

static void privCopyLoad(void)
{ /* Arrive here is we want to monitor the load of all tasks, the Os and the Isr. Every now
   * and then we copy the collected subticks to the totals and clear the collectors. Since
   * this routine is not interrupted, we get an adequate picture of the loads. */
  uiOsLoadTotal = uiOsLoadCollect;
  uiOsLoadCollect = 0;
  uiIdleLoadTotal = uiIdleLoadCollect;
  uiIdleLoadCollect = 0;
  /* Only if non empty interrupts we need to copy the isr load. Otherwise, these variables
   * are absent. */
  #if (cfgIntUserDefined == cfgTrue)
    uiIsrLoadTotal = uiIsrLoadCollect;
    uiIsrLoadCollect = 0;
  #endif
  /* Depening on the number of tasks, we move the loads. */
  Tuint08 uiLoopTask;
  { for (uiLoopTask=0; uiLoopTask<defNumberOfTasks; uiLoopTask++)
    { TtaskControlBlock * loopTCB = privTcbList(uiLoopTask);
      loopTCB->uiLoadTotal = loopTCB->uiLoadCollect;
      loopTCB->uiLoadCollect = 0; } } }

#endif


#if (defUseDelay == cfgTrue)

static void privDelayCalc(Tuint16 uiDelayTime, Tbool bFromNow)
{ /* This is the delay core code. Several routines make use of it. Arrive here if you need to
   * to set the delay variables of the task control block. */
  #if (cfgCheckOsStack == cfgTrue) || (cfgCheckWatermarks == cfgTrue)
    /* This is one of the places in the current design of the Femto OS that is somewhat deeper.
     * Thus it makes sense to check the OS stack here. */
    privCheckOsStack();
  #endif
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* There are two different forms of delay. The time measured form the current value of the
   * tick counter or from the last value of activation. The latter is stored in the delay
   * variables. */
  if (bFromNow)
  { /* Here the new time is calculated form the current time. */
    /* Now calculate the new Delaytime. This is a 16 bit operation. Note that the uiDelaytime
     * parameter represents the requested delay time whereas the uiDelayTime field in the
     * task control block represents the wakeup time of the particular task due to delay. */
    uiDelayTime += uxTickCount.Full; }
  else
  { /* Here the new time is calculated from the last wake time. */
    #if (cfgCheckTiming == cfgTrue)
      /* We can assume we do not want to set a new wake time that is in he past, although it
       * is not an error in absolute sense. However, we no code that protects in this case
       * it being interpreted as a long delay. Therefore we report an error. */
      Tuint16 uiMinDelay = (uxTickCount.Full - curTCB->uxDelay.Full) ;
      if (uiDelayTime < uiMinDelay)
      { privShowError((errTaskDelayTooShort | errTaskStateCurrent | errOsStateReturn), errCurrentTask ); }
    #endif
    uiDelayTime += curTCB->uxDelay.Full; }
  /* We have a new wake time, so place it in the appropriate fields. */
  curTCB->uxDelay.Full =  uiDelayTime ;
  /* It may be that this task is in the near wake, in which all tasks are that are in the
   * same tick block as the current time. We must check this and set the near wake bit if
   * needed. */
  if (curTCB->uxDelay.HighByte ==  uxTickCount.HighByte )
  { uiOsStatus = ((uiOsStatus & defNearWakePresentSetMask) | defNearWakeStatePresent); }
  /* Update the task status of the task, say that the current task is delayed. If we want to
   * allow for delays below the minimum, we must add code here to check and keep the state
   * from being delayed. However we assume that this situation is normally not what you
   * want, because the task can be 'far behind'. We cannot get here for terminated tasks. */
  curTCB->uiTaskStatus = (curTCB->uiTaskStatus & defBaseDelayStateSetMask) | defBaseDelayStateDelayed; }

#endif


#if (cfgUseSynchronization == cfgSyncSingleSlot) && ((cfgUseTaskWatchdog == cfgTrue) || ( includeTaskRestart == cfgTrue ))

static void privCleanSlotStack(TtaskExtendedControlBlock * taskTCB)
{ taskTCB->uiTaskSlot = defSlotRightFree; }

#endif


#if ((cfgUseSynchronization == cfgSyncSingleBlock) || (cfgUseSynchronization == cfgSyncDoubleBlock)) && ((cfgUseTaskWatchdog == cfgTrue) || ( includeTaskRestart == cfgTrue ))

static void privCleanSlotStack(TtaskExtendedControlBlock * taskTCB)
{ Tuint08 * pTaskSlot = (Tuint08 *) &taskTCB->uiTaskSlot;
  /* We need to know the total size of the stack. Note that the number of
   * slots is the double of the depth. */
  #if (defSlotDepthConstant == cfgTrue)
    /* It may be a constant and than we use that */
    Tuint08 uiSDC = defSlotDepthCollect;
  #else
   /* Or it may be stored in flash*/
    Tuint08 uiSDC = portFlashReadStruc(TtaskDefinitionBlock,pxTDBlist[uiControlTaskNumber & defTaskNumberGetMask],Tuint08,uiSlotDepth);
  #endif
  Tuint08 uiLoopSlot;
  for (uiLoopSlot=0; uiLoopSlot<uiSDC; uiLoopSlot++) { *pTaskSlot++ = defSlotFree;  } }

#endif

#if (cfgUseSynchronization == cfgSyncSingleSlot)

static Tbool privOperateSlotStack(Tuint08 uiControlTaskNumber, Tuint08 uiSlotSlot)
{ /* Use this method to add remove one slot in the slot stack, or to search a slot.
   * This implementation can handle one slot only, and should be selected in case
   * you use only one slot and no nesting is required. The size is thus known.
   * The search is the default. The search can be decorated with search free only
   * or blocks only. Per default any match is valid. If we add a slot, it will always
   * be put at the first position. The slot that was at that position will be placed
   * elsewhere. This is to ensure that, if the task blocks, we know the blocking
   * slot is in the first position. */
  TtaskExtendedControlBlock * taskTCB = (TtaskExtendedControlBlock *) privTcbList(uiControlTaskNumber & defTaskNumberGetMask);
  /* Now we must handle all cases separately. In one special case we want only to
   * clean the stack, it is easy, done below. The clean case is special. */
  /* Load the value of the slot (left four bits are unused.) */
  Tuint08 uiVal = taskTCB->uiTaskSlot;
  /* Searching to see if the slot stack is free of Mutexes and Queues is need only in the
   * case we must restore the priority after lifting. */
  #if (cfgUsePriorityLifting == cfgTrue)
    /* We check if we want to see if the slot numbers representing Queues and Mutexes are absent */
    if ((uiControlTaskNumber & defSlotOperateSQMMask) == defSlotOperateQMAbsent)
    {  /* Test if slot represents a Queue or Mutex, if so return false, return true otherwise */
        return ((uiVal < defNumberQueuBegin) || (uiVal >= defNumberMutexEnd)); }
  #endif
  /* We need the result of the equality of the requested slot and the stored slot more
   * often, so store this in a separate variable. Also, this is the default result value */
  Tbool uiValIsSlot = (uiVal == uiSlotSlot);
  /* From this place on we will test the different situations. Please note that the default
   * result is return the value of uiValIsSlot. Thus if this is wat we want to return, we do
   * not need to do anything. */
  /* Test if we were looking for free slots only */
  if ((uiControlTaskNumber & defSlotOperateSFreeMask) == defSlotOperateSearchFree)
  { /* If we where, and the task was blocking, we must return false, since there
     * cannot be a matching slot. If the task was free we need to return the result
     * of uiValIsSlot, which is default. */
    if ((taskTCB->uiTaskStatus & defSlotLoopBlockMask) == defSlotLoopBlockBlocked) return false;  }
  /* Test if we want to add a slot */
  if ((uiControlTaskNumber & defSlotOperateIncreaseMask) == defSlotOperateIncreaseAction)
  { /* If we make use of method checking, this slot should not be used at this moment */
    #if (cfgCheckMethodUse == cfgTrue)
      /* Test if it is*/
      if (uiVal != defSlotRightFree)
      /* If so return an error. */
      { privShowError((errSlotIncreaseFail | errTaskStateInfo | errOsStateReturn), ((uiSlotSlot << errInfoNumberShift) | ((uiControlTaskNumber & defTaskNumberGetMask) << errTaskNumberShift)) ); }
    #endif
    /* Report the locks */
    #if (cfgCheckTrace == cfgTrue)
      /* First we report which task is currently running */
      privTrace(traceSlotLock | (uiControlTaskNumber & defTaskNumberGetMask) );
      /* Subsequently report the lock. */
      privTrace(uiSlotSlot);
    #endif
    /* Set the new slot*/
    taskTCB->uiTaskSlot = uiSlotSlot;
    /* and what we return is not of importance. */ }
  /* Test if we want to remove a slot */
  if ((uiControlTaskNumber & defSlotOperateDecreaseMask) == defSlotOperateDecreaseAction)
  { /* Test if the slot matches. Now this is only needed if we make use of method checking, since it
     * is considered an error to remove a non existing slot.  */
    #if (cfgCheckMethodUse == cfgTrue)
      /* If it does not match */
      if (!uiValIsSlot)
      /* report the error */
      { privShowError((errSlotDecreaseFail | errTaskStateInfo | errOsStateReturn), ((uiSlotSlot << errInfoNumberShift) | ((uiControlTaskNumber & defTaskNumberGetMask) << errTaskNumberShift)) ); }
    #endif
    /* Report the unlocks */
    #if (cfgCheckTrace == cfgTrue)
      /* First we report which task is currently running */
      privTrace(traceSlotUnlock | (uiControlTaskNumber & defTaskNumberGetMask) );
      /* Subsequently report the lock */
      privTrace(uiSlotSlot);
    #endif
    /* Remove the slot, even if it does not match, cause we may asume it does. */
    { taskTCB->uiTaskSlot = defSlotRightFree; }
    /* We return with false, since this condition may only return true if an other slot of the same number
     * is present on the stack. This cannot be the case however, since there is only one slot present. */
    return false; }
  /* In all remaining cases we where just searching, and since there is only one slot a simple test is enough. */
 return uiValIsSlot; }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && (defUseQueus == cfgTrue)

static Tuint08 privGetQueuSize(Tuint08 uiQueuNumber)
{ /* Used to read the size of a particular queue from flash or to return the
   * constant if all sizes are equal. */
   Tuint08 uiResult;
   /* So first determine if they are equal */
  #if (defQueuSizeConstant == cfgTrue)
    /* Fixed sizes, return the constant.  */
    uiResult = defQueuSizeFixed;
  #else
    /* Otherwise we must look it up from a table in flash. */
    uiResult = portFlashReadWord(Tuint08,uiQueuSize[uiQueuNumber]);
 #endif
 /* Done, return the size of the queue. */
 return uiResult; }

#endif



#if (cfgUseSynchronization == cfgSyncSingleBlock) || (cfgUseSynchronization == cfgSyncDoubleBlock)

static Tbool privOperateSlotStack(Tuint08 uiControlTaskNumber, Tuint08 uiSlotSlot)
{ /* Use this method to add remove slots in the slot stack, or to search a slot.
   * The latter is the default. The search can be decorated with search free only
   * or blocks only. Per default any match is valid. If we add a slot, it will always
   * be put at the first position. The slot that was at that position will be placed
   * elsewhere. This is to ensure that, if the task blocks, we know the blocking
   * slot is in the first position. Two slots can be added simultaneously, they always
   * take the first bytes. */
  /* The default reaction is false, but this is only needed if we have double slots, since
   * then it is sometimes handy to have a default reaction. Normally you should not enter
   * this method with the free slot. */
  TtaskExtendedControlBlock * taskTCB = (TtaskExtendedControlBlock *) privTcbList(uiControlTaskNumber & defTaskNumberGetMask);
  /* pTaskSlot holds the pointer to the first two slots in the stack. The rest
   * of the stack is outside the TaskControlBlock. */
  Tuint08 * pTaskSlot = (Tuint08 *) &taskTCB->uiTaskSlot;
  /* We need to know the total size of the stack. Note that the number of
   * slots is the double of the depth. */
  #if (defSlotDepthConstant == cfgTrue)
    /* It may be a constant and than we use that */
    Tuint08 uiSDC = defSlotDepthCollect;
  #else
   /* Or it may be stored in flash*/
    Tuint08 uiSDC = portFlashReadStruc(TtaskDefinitionBlock,pxTDBlist[uiControlTaskNumber & defTaskNumberGetMask],Tuint08,uiSlotDepth);
  #endif
  Tuint08 uiLoopSlot;
  /* This variable is needed to hold some slots later on. */
  Tuint08 uiVal;
  /* Searching to see if the slotstack is free of Mutexes and Queues is need only in the
   * case we must restore the priority after lifting. */
  #if (cfgUsePriorityLifting == cfgTrue)
    /* We check if we want to see if the slot numbers representing Queues and Mutexes are absent */
    if ((uiControlTaskNumber & defSlotOperateSQMMask) == defSlotOperateQMAbsent)
    { /* Loop through all the slots */
      for (uiLoopSlot=0; uiLoopSlot<uiSDC; uiLoopSlot++)
      { /* Get the first to slots (technically the left slot in the zeroth slot can never be
         * a wait, but checking does not harm either.*/
        uiVal = *pTaskSlot++;
        /* Extract the left slot */
        Tuint08 uiLeftSlot  = (uiVal & defSlotLeftGetMask) >> defSlotLeftShift;
        /* Test if it represents a Queue or Mutex, if so return false */
        if ((uiLeftSlot  >= defNumberQueuBegin) && (uiLeftSlot  < defNumberMutexEnd)) { return false;}
        /* Extract the right slot */
        Tuint08 uiRightSlot = (uiVal & defSlotRightGetMask) >> defSlotRightShift;
        /* Test if it represents a Queue or Mutex, if so return false */
        if ((uiRightSlot >= defNumberQueuBegin) && (uiRightSlot < defNumberMutexEnd)) { return false;} }
      /* If we did no encounter any Queues ore Mutexes, the stack is empty or contains only Waits, in that
       * case we may return true. */
      return true; }
  #endif
  /* uiLoopControl will contains two bits of information. One to indicate that is
   * the task is free or blocked. And the second (set further below) to indicate if
   * the operation is to handle all slots. The latter is the default  */
  Tuint08 uiLoopControl = (taskTCB->uiTaskStatus & defSlotLoopBlockMask);
  /* If we are still here we did not search Waits only. Now if we are not searching
   * free slots only eigther or increase the stack (i.e. we search all or we decrease the stack,  we must make
   * sure the loop below does not skip the first two slots in the search loop below. so override
   * the blocked state form the task to enforce checking the first byte.  */
  if (((uiControlTaskNumber & defSlotOperateSFreeMask) != defSlotOperateSearchFree) && ((uiControlTaskNumber & defSlotOperateIncreaseMask) != defSlotOperateIncreaseAction))
  { uiLoopControl = defSlotLoopBlockNon; }
  /* The loop below contains two parts, a test for each slot position and an action. The default
   * is to test agains the given slot, and to replace the found value with the freeSlot (0x00)
   * Since searching can only be at one slot at a time, we know uiSlotSlot has first four bits zero */
  Tuint08 uiTest = uiSlotSlot;
  Tuint08 uiFill = defSlotRightFree;
  /* Load the value of the fist two slots. */
  uiVal = *pTaskSlot;
  /* However in case we want to add a slot, we must look for a free slot and substitute the
   * value that was placed first, which in his turn must contain the new value. Thus */
  if ((uiControlTaskNumber & defSlotOperateIncreaseMask) == defSlotOperateIncreaseAction)
  {/* Place the new slot at the first position, thereby leaving a possible second position in tact */
    #if (cfgUseSynchronization == cfgSyncDoubleBlock)
      /* In this case the whole byte is reserved for the new lock, which may contain two slots. */
      *pTaskSlot = uiSlotSlot;
    #else
      /* In this case the left nibble can contain a free lock which must be preserved, and uiSlotSlot can only
       * contain one slot */
      *pTaskSlot = (uiVal & defSlotRightSetMask) | uiSlotSlot;
    #endif
    /* Report the locks */
    #if (cfgCheckTrace == cfgTrue)
      /* First we report which task is currently running */
      privTrace(traceSlotLock | (uiControlTaskNumber & defTaskNumberGetMask) );
      /* Report the slots */
      privTrace(uiSlotSlot);
    #endif
     /* and from now on we look for empty slots */
    uiTest = defSlotRightFree;
    /* Which must be filled with the value that was on the first place before. */
    #if (cfgUseSynchronization == cfgSyncDoubleBlock)
      /* For a double block both slots may need replacement */
      uiFill = uiVal;
    #else
      /* For a single block only the right nibble is interesting, the left may contain a free lock */
      uiFill = (uiVal & defSlotRightGetMask);
    #endif
    /* If that slot was empty before, it makes no sense to continue (although it cannot harm)
     * and we are done. */
    if (uiFill == defSlotFree) return false;
    /* If we do continue we may not test the first nibble/byte since that is what we just filled, and we do
     * not want to risk a double block. so we simulate a block. */
    uiLoopControl = defSlotLoopBlockBlocked; }
  /* If we do not want to add a slot, we are about to remove one, or search for one. In case
   * we do not want to remove one, we search for one, and indicate so in the uiLoopControl variable.
   * This is to make sure we leave when found, before we substitute a value. */
  else if ((uiControlTaskNumber & defSlotOperateDecreaseMask) != defSlotOperateDecreaseAction)
  { uiLoopControl |= defSlotLoopSearchActive; }
  /* If we are decreasing, maybe we must inform the outside world we are doing so */
  #if (cfgCheckTrace == cfgTrue)
    else
    { /* Here we are decreasing, which we want to report the unlocks, first we report which task is currently running */
      privTrace(traceSlotUnlock | (uiControlTaskNumber & defTaskNumberGetMask) );
      /* Report the slot(s) */
      privTrace(uiSlotSlot); }
  #endif
  /* Start looping at the beginning or skip the first byte depending on the situation of the uiLoopControl. */
  if ((uiLoopControl & defSlotLoopBlockMask) == defSlotLoopBlockNon)
  { uiLoopSlot = 0; }
  else
  { /* We must distinguish the double and single block situations */
    #if (cfgUseSynchronization == cfgSyncDoubleBlock)
      /* In this case we need to skip the first full byte, which contains the new slots */
      uiLoopSlot = 2;
      pTaskSlot++;
    #else
      /* In this case we need to skip only the first nibble, which contains the new slot */
      uiLoopSlot = 1;
    #endif
  }
  /* Now run through all slots, double the number because we check left and right nibble with the same code */
  while (uiLoopSlot<2*uiSDC)
  { uiVal = *pTaskSlot;
     /* At odd passes we check the other nibble (slot) */
    if ((uiLoopSlot & 0x01) == 0x01) { portSwapNibbles(uiVal); }
    /* Test if the right slot fulfills the test, and if so, test also if we may test this slot. The latter may
     * not be the case for the very first slot, when we are only searching for free slots and this task is blocking. */
    if ((uiVal & defSlotRightGetMask) == uiTest )
    { /* So we found a match on the right slot. If we were only searching, we may leave with the message we found a hit. */
      if ((uiLoopControl & defSlotLoopSearchMask) == defSlotLoopSearchActive) return true;
      /* If we were replacing (for increase or decrease), do so now */
      #if (cfgUseSynchronization == cfgSyncDoubleBlock)
        /* We simply slide in the new value on the right, if the values ware swapped before
         * that does not matter */
        *pTaskSlot = (uiVal & defSlotRightSetMask) | (uiFill & defSlotRightGetMask);
      #else
        /* Calculate the new value for the slot stack */
        Tuint08 uiNewVal = (uiVal & defSlotRightSetMask) | uiFill;
        /* here we must be careful since the sequence of nibbles on the bottom of the slot stack
         * does matter, so we must restore if we corrupted it */
        if ((uiLoopSlot & 0x01) == 0x01) { portSwapNibbles(uiNewVal); }
        /* Now it is safe to store. */
        *pTaskSlot = uiNewVal;
      #endif
      /* If we are decreasing, it is interesting to know if there is a second slot with the same number,
       * so we continue otherwise */
      if ((uiControlTaskNumber & defSlotOperateIncreaseMask) == defSlotOperateIncreaseAction)
      { /* If there can be a second slot */
        #if (cfgUseSynchronization == cfgSyncDoubleBlock)
          /* We move to the second slot */
          uiFill >>= 4;
          /* there may be no second slot to paste  */
          if (uiFill == defSlotFree) { return false; }
        #else
          /* If not we are done, the return value is of no importance */
          return false;
        #endif
      }
      else
      { /* we are decreasing, so turn the operation into a search */
        uiLoopControl |= defSlotLoopSearchActive; } }
     /* Only at odd passes we check go to the next byte in the stack */
    if ((uiLoopSlot & 0x01) == 0x01) { pTaskSlot++;  }
    uiLoopSlot++; }
  /* If we found nothing, return false. If we were not searching but increasing we should not arrive here,
   * for it indicates we could not add the slot. If we where decreasing arriving here means the there was no
   * slot with the given number that could be removed. If this is an error depends on the caller. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Test if we arrived here while increasing, if so we could not add the slot for the stack is full.*/
    if ((uiControlTaskNumber & defSlotOperateIncreaseMask) == defSlotOperateIncreaseAction)
    { privShowError((errSlotIncreaseFail | errTaskStateInfo | errOsStateReturn), ((uiSlotSlot << errInfoNumberShift) | ((uiControlTaskNumber & defTaskNumberGetMask) << errTaskNumberShift)) ); }
    /* Test if we arrived here while decreasing and did not yet succeed in doing so. If so the slot
     * was not present and this is considered an error. If we where decreasing and searching, we did
     * not find a second slot so we should return false, this is normal operation */
    if ( ((uiControlTaskNumber & defSlotOperateDecreaseMask) == defSlotOperateDecreaseAction) &&
         ((uiLoopControl & defSlotLoopSearchMask) != defSlotLoopSearchActive) )
    { privShowError((errSlotDecreaseFail | errTaskStateInfo | errOsStateReturn), ((uiSlotSlot << errInfoNumberShift) | ((uiControlTaskNumber & defTaskNumberGetMask) << errTaskNumberShift)) ); }
  #endif
  return false; }

#endif

#if (cfgUseSynchronization != cfgSyncNon) && ((defUseMutexes == cfgTrue) || (defUseQueus == cfgTrue))

static Tuint08 privFreeLockAbsent(Tuint08 uiSlot)
{ /* This method is used to see if a particular slot is occupied by an other task as a free lock.
   * Thus it returns true when the slot is totally unused, returns true when it is only used
   * as blocking slots or if you call it with the free Slot. Note that it returns false if the
   * slot has been used in a task as free, even if that task was later blocked. */
  if (uiSlot != defSlotFree)
  { Tuint08 uiLoopTask;
    /* Run trough all tasks that use slots */
    for (uiLoopTask=0; uiLoopTask<defNumberOfTasksWithSlot; uiLoopTask++)
    { /* Search for the use of this slots as free slots in de the slotstack. privOperateSlotStack returns
       * true if the slot is reported free in this task. If so we may stop here, if not we must continue
       * to search. */
      if ( privOperateSlotStack((uiLoopTask | defSlotOperateSearchFree ), uiSlot ) ) { return false; }  } }
  /* If we have not found any free use of the slot, the slot is not used as such. */
  return true; }

#endif


#if (cfgUseSynchronization != cfgSyncNon) || (cfgUseFileSystem == cfgTrue) || (cfgUseEvents == cfgTrue)

static void privUnblockTask(Tuint08 uiControlTaskNumber)
{ /* This method unlocks / unblocks a task for you, depending on the bits in uiControlTaskNumber.
   * It can handle fs blocks and syncblocks. Only call this on blocking tasks, since it will
   * always perform a deblock, there is no test if the task is indeed blocked. Terminated and
   * suspended tasks are ignored. */
  TtaskExtendedControlBlock * taskTCB = (TtaskExtendedControlBlock *) privTcbList(uiControlTaskNumber & defTaskNumberGetMask);
  /* Check if the task is not terminated or suspended, since they may not be unlocked, for all the
   * other types (sleeping included) there is no problem  */
  if (taskTCB->uiTaskStatus >= defBaseSleepingTask)
  { /* First we see if the caller wanted to unlock the task too. */
    if ((uiControlTaskNumber & defParaLockMask) == defParaLockStateUnlock)
    {
      #if (cfgUseSynchronization != cfgSyncNon)
        if ((taskTCB->uiTaskStatus & defBaseModeGetMask) == defBaseModeSync)
        { /* If we make use of events, the test above also passes for event unlocks, thus we
           * must explicitly test in that case that we do not have an event unlock. This could
           * lead to the erasure of free slots. */
          #if (cfgUseEvents == cfgTrue)
            if ((taskTCB->uiTaskStatus & defBaseDressGetMask) == defBaseDressSlot)
          #endif
          { /* Unlocking means setting the lowest slot number to zero, the 'free lock'. Note we assume that
             * slot is the correct one, for we came from a blocking situation. */
            #if (cfgCheckTrace == cfgTrue)
              /* First we report which task is currently running */
              privTrace(traceSlotUnlock | (uiControlTaskNumber & defTaskNumberGetMask) );
              /* Report the slots */
              privTrace(taskTCB->uiTaskSlot);
            #endif
          /* Clean the lock, depending on the type  */
            #if (cfgUseSynchronization == cfgSyncSingleSlot)
              /* We may clean all of it, since the left nibble is not used */
              taskTCB->uiTaskSlot = defSlotFree;
            #elif (cfgUseSynchronization == cfgSyncSingleBlock)
              /* We must be careful for the left nibble may contain a free lock */
              taskTCB->uiTaskSlot = (taskTCB->uiTaskSlot & defSlotRightSetMask) | defSlotRightFree;
            #elif (cfgUseSynchronization == cfgSyncDoubleBlock)
              /* We must be sure to clean all blocking slots. */
              taskTCB->uiTaskSlot = defSlotFree;
            #else
              #error you should not arrive here
            #endif
        } }
      #endif
      #if (cfgUseFileSystem == cfgTrue)
        /* Check if the task is locked on the file system. */
        if ((taskTCB->uiTaskStatus & defBaseModeGetMask) == defBaseModeFile)
        { /* If so, we clean the read/write modes. Clean the locks, if present. Note we only do so
           * when requested to unlock. This is the case in for example when a delay occurs. */
          #if (defUseFsField == cfgTrue)
            taskTCB->defFsField = (taskTCB->defFsField & (defFsReadSetMask & defFsWriteSetMask)) | (defFsReadClear | defFsWriteClear);
          #endif
        }
      #endif
      /* The last situation, namely, the Events, do not need standard unlocking operations, for we can
       * only arrive here if the uiTaskEvents has already be cleared. The only exception being in case
       * of timeouts. Also, there is no harm in cleaning the TaskEvent if we come from an unlock due
       * to a slot locking. Since if the task was blocking on a slot, it could not have been blocking on
       * an event to happen. In that case the TaskEvent variable should already be cleared. Thus */
      #if (cfgUseEvents == cfgTrue) && (cfgUseTimeout == cfgTrue)
        taskTCB->uiTaskEvents = defAllEventsReset;
      #endif
    }
    privTrace(traceTaskRelease | (uiControlTaskNumber & defTaskNumberGetMask) );
    /* ... if so, unblock the task and also remove a possible timeout. If we forget this, the task will
     * turn into a delayed state. This is needed since we also unblock tasks which are not the current ones.
     * A sleeping state may be deblocked and waked, but must stay sleeping. */
    taskTCB->uiTaskStatus = (taskTCB->uiTaskStatus & (defBaseModeSetMask & defBaseBlockStateSetMask & defBaseDelayStateSetMask)) | (defBaseModeSync | defBaseBlockStateFree | defBaseDelayStateWake);
    /* If we must return a value, this can only be the case after an unblock of course. Without an unblock, there
     * is no task to return something to. */
    #if (defUseBoolReturns == cfgTrue)
      /* The return value is stored in the defRetField, and consists of two bits, one for true/false, one to
       * indicate if there is something to return. */
      taskTCB->defRetField = (taskTCB->defRetField & defRetSetMask) | ((uiControlTaskNumber & defParaRetMask) >> defParaRetToRetShift);
    #endif
   /* If we want to keep track of accurate wake times, and we have release a task, we must copy the current
    * time to the delay fields, since this is the place where the time of last activation is kept.*/
    #if (defUseDelay == cfgTrue) && (cfgUseCorrectWakeupTimes == cfgTrue)
      taskTCB->uxDelay.Full = uxTickCount.Full;
    #endif
  } }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && (cfgUsePriorityLifting == cfgTrue)

static void privLiftLocksOnSlot(Tuint08 uiSlot)
{ /* Arrive here when you need to increase the priorities of several tasks in a slot
   * of a task about to be blocked. By increasing those priorities to the level of
   * the blocking slot, these tasks have a chance to finish, and not keep the current
   * blocking task for ever. Note that we do not level all priorities in one slot,
   * since due to the fact that tasks may be blocked on two slots simultaneously this
   * would be a very costly to sort out. This mechanism tries to keep it simple.*/
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* Get the priority of the current task. Note we do not need to shift the
   * priority since we will only compare priorities among each other, we do not
   * need the exact value. */
  Tuint08 uiCurTaskPriority = curTCB->uiTaskStatus & defBasePrioGetMask;
  Tuint08 uiLoopTask;
  TtaskControlBlock * loopTCB;
  /* Run through all task possible having a slot. */
  for (uiLoopTask=0; uiLoopTask<defNumberOfTasksWithSlot; uiLoopTask++)
  { loopTCB = privTcbList(uiLoopTask);
    /* Test if the task indeed holds the slot. We do not distinguish between blocked and
     * locked only. Solely possessing the lock is enough to rise its priority. Sometimes
     * this may imply we rise tasks without a real need, but it would be to costly to
     * analyze on which slot the blocking takes place. */
    if (privOperateSlotStack((uiLoopTask | defSlotOperateSearchDefault), uiSlot))
    { /* Get the priority of the looped task, again, don't shift */
      Tuint08 uiLoopTaskPriority = loopTCB->uiTaskStatus & defBasePrioGetMask;
      /* Compare which is larger */
      if (uiCurTaskPriority>uiLoopTaskPriority)
      { /* and if the priotity of the current task (which will be blocked and thus inactivates
         * was larger, lift the priority of the other task to that of the current task. The
         * present priority gets lost.  */
        loopTCB->uiTaskStatus = (loopTCB->uiTaskStatus & defBasePrioSetMask) | uiCurTaskPriority; } } } }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && (cfgUsePriorityLifting == cfgTrue)

static void privRestoreInitialPriority(Tuint08 uiTaskNumber)
{ /* Arrive here if we need to restore the priority of a lifted task. The priority
   * is reset to the original value that is stored in flash. First we test if the
   * task does not hold any Queue's or Mutexes any more. */
 if ( privOperateSlotStack((uiTaskNumber | defSlotOperateQMAbsent ), defSlotFree ) )
 { /* If so it now makes sense to restore the priority. */
   TtaskControlBlock * taskTCB = privTcbList(uiTaskNumber);
   /* Read the initial priority from flash, or use the constant */
   #if (defInitialStatusConstant == cfgTrue)
     Tuint08 uiInitialStatus = defInitialStatusFixed;
   #else
     Tuint08 uiInitialStatus = portFlashReadStruc(TtaskDefinitionBlock,pxTDBlist[uiTaskNumber],Tuint08,uiInitialStatus);
   #endif
   /* Restore its value. */
   taskTCB->uiTaskStatus = (taskTCB->uiTaskStatus & defBasePrioSetMask) | (uiInitialStatus & defBasePrioGetMask); } }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && (defUseQueus == cfgTrue)

/* The queue mechanism allows for several tasks writing and reading bytes. The OS
 * locks all tasks not able at that moment to write or read the required quantity of bytes.
 * Ah, and take care, the queues (and locks for that matter) are numbers from 1 and
 * onwards. Thus, slot 0 is forbidden, since it indicates a free slot. But queue's
 * are stored in arrays, thus slot 1 corresponds to queue 0 etc. Therefore you see
 * [uiSlot-1] everywhere. */

/* DISCUSSION
 * Except in the beginning, the Read (pointer) and Write (pointer) are never equal.
 * This is because otherwise you could not see the difference between a
 * full and an empty queue. This would imply that one byte is effectively lost,
 * for it could never be retrieved. Such a waste! We must solve this
 * by storing one bit of information that differentiates between a full and an
 * empty queue, when Read and Write are equal. But where?
 * It seems reasonable to limit the size of queue's to 127 bytes. Now we have
 * two extra bits available (upper bits of Read/Write storage). Second advantage
 * is that we can address the whole queue, from the queue Requests. What we don't
 * know at this moment if this will cost a lot of code.
 * Now, we decided to reserve a bit to indicate that the queue is full, is
 * it handy to use the other for indicating an empty queue? Well, it may be,
 * but, since queue's are empty at startup, we want to indicate the bit
 * empty by zero. Hmmm, that implies a lot of code. It may be shorter to
 * just ask for Read==Write and !full. Ok let us see what happens.
 * Now we keep the write pointers in uiQueuWrite[] and the read pointers in
 * uiQueuRead[]. The write operation is write-post-decrement, and the read
 * operation likewise. The pointers wrap when they hit the bottom of the queue.
 * Thus we write/read downwards, the wrapping is easier detected that way.
 */


static Tuint08 privQueuTest(Tuint08 uiSlot, Tsint08 siFreeFilling)
{ /* If we want to know the size of a queue use this function, or if we
   * want to test if a certain number of bytes will fit.  We use negative numbers
   * for writing, and positive numbers for reading. */
  /* The size information of the queue is in flash, note the shift from slot numbers
   * to queue array numbering. */
  Tuint08 uiSize = privGetQueuSize(uiSlot-1);
  /* Parameter FreeFilling zero is miss-used to request the size of the queue. */
  if (siFreeFilling==0) { return uiSize; }
  /* The information about the write and read positions are stored in two arrays, the
   * uiQueuWrite and the uiQueuRead. Obtain the write pointer. That pointer points to
   * the location that has to be written. */
  Tuint08 uiWrite = uiQueuWrite[uiSlot-1];
  /* Extract the full bit. We assume it has been set and managed correctly, although it
   * only has a meaning when uiWrite==uiRead, for this is the only situation where we
   * have ambiguity. */
  Tbool bFull = (uiWrite & defQueuFillingGetMask) == defQueuFillingStateFull;
  /* If the queue is full ... */
  if (bFull)
  { if (siFreeFilling>0)
    { /* we can read as many bytes from the queue as it is long, */
      return uiSize; }
    else
    { /* but we cannot write anything anymore to it. */
      return 0; } }
  /* In other cases we must calculate the number, extract the read pointer.  */
  Tuint08 uiRead = uiQueuRead[uiSlot-1];
  /* Now calculate how many readable characters there are. The situation that the queue
   * might be full was already resolved, so in the calculation below it is assumed the
   * queue is not full, i.e. uiWrite==uiRead implies it is empty. Make sure the calculation
   * never produced negative numbers. */
  Tuint08 uiReadable  = uiWrite<=uiRead ? uiRead - uiWrite : uiSize - (uiWrite - uiRead);
  /* Every byte that is not readable, must be writable. */
  Tuint08 uiWriteable = uiSize - uiReadable;
  /* Now decide what to return, for positive siFreeFilling we asks for the readable characters,
   * for negative for the writable. */
  if (siFreeFilling>0)
  { return uiReadable; }
  else
  { return uiWriteable; } }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && (defUseWaits == cfgTrue)

static void privReleaseWait(Tuint08 uiSlot)
{ /* This is called if we want to clean all locks in the tasks blocked by a wait. Cannot be
   * called to clean up Queues or Mutexes. */
  Tuint08 uiLoopTask;
  /* Loop trough all tasks. */
  for (uiLoopTask=0; uiLoopTask<defNumberOfTasksWithSlot; uiLoopTask++)
  { TtaskExtendedControlBlock * loopTCB = (TtaskExtendedControlBlock *) privTcbList(uiLoopTask);
     /* Check if the task holds a blocking slot with the given number. The slot can never be left,
      * since waits cannot be be blocked in pairs. Furthermore, the task must be blocking, wait slots
      * are never solely locked. */
     if ((loopTCB->uiTaskSlot & defSlotRightGetMask) == uiSlot)
     { /* If the slot matches unblock and unlock the task */
       privUnblockTask(uiLoopTask | defParaLockStateUnlock | defParaRetStateTrue); } } }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && (defUseQueus == cfgTrue)

static Tbool privSizeFitsQueu(Tuint08 uiSlot, Tsint08 siFreeFilling)
{ /* Call this if you need to test if a queue on a slot may fit the filling condition. First
   * test if we indeed have a queue, otherwise there is noting to test. */
  if ((uiSlot  >= defNumberQueuBegin) || (uiSlot  < defNumberQueuEnd ))
  { Tuint08 uiTest;
    /* Now we test the siFreeFilling. This method returns the number of bytes that can read from / written to
     * the queue, depending on the sign of siFreeFilling. If siFreeFilling==0 it returns the queue size (which
     * value we do not need here) The number returned is always positive. */
    uiTest = privQueuTest(uiSlot,siFreeFilling);
    /* If we want to read we may enter if we want to read not more bytes as are available */
    if (siFreeFilling>0) { return ( ((Tuint08) siFreeFilling) <= uiTest); }
    /* If we want to read we may enter if we want to read not more bytes as are available */
    if (siFreeFilling<0) { return ( ((Tuint08) (-siFreeFilling)) <= uiTest); } }
  /* The default is true, i.e. you get the lock and it remains in effect if the siFreeFilling == 0 or
   * we have no queue at all. */
  return true; }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && ((defUseMutexes == cfgTrue) || (defUseQueus == cfgTrue))

static void privReleaseSyncBlockingTasks(void)
{ /* Arrive here if you have just completed putting sum data on the queue, or used a Mutex
   * and are done and have released your own lock. Or call it when the task is
   * initilialized or terminated. It runs through all tasks, and unblocks (not unlock)
   * the first task that meets the conditions. This may not be the one with the highest
   * priority. It does not operate on waitblocks (since they cannot, and should not,
   * be released like this) */
  Tuint08 uiLoopTask;
  /* Run through all tasks possibly having a slot. */
  for (uiLoopTask=0; uiLoopTask<defNumberOfTasksWithSlot; uiLoopTask++)
  { TtaskExtendedControlBlock * loopTCB = (TtaskExtendedControlBlock *) privTcbList(uiLoopTask);
    /* First check it this is a blocking task (a non blocking task does not need to
     * be released) Furthermore, it must be a task that is able to run, otherwise way
     * may release a task which is for instance suspended, and that will hold the lock
     * for ever. */
     if ((loopTCB->uiTaskStatus & ( defBaseStopStateGetMask | defBaseModeGetMask | defBaseBlockStateGetMask | defBaseDressGetMask))  == (defBaseStopStateGo | defBaseModeSync | defBaseBlockStateBlocked | defBaseDressSlot) )
     { /* If yes, extract on which slots this task is blocking */
       Tuint08 uiRightSlot = (loopTCB->uiTaskSlot & defSlotRightGetMask) >> defSlotRightShift;
       /* Now we must test if this is not a wait-slot */
       #if (defUseWaits == cfgTrue)
         if (uiRightSlot < defNumberWaitBegin)
       #endif
       { /* If we arive here we are dealing with a queue of mutex. Now, depending on if we are working
          * with doublelocks, we may need the left slot too. Waits and mutex/queues can never mix, so we
          * do not need to worry */
         #if (cfgUseSynchronization == cfgSyncDoubleBlock)
           Tuint08 uiLeftSlot  = (loopTCB->uiTaskSlot & defSlotLeftGetMask) >> defSlotLeftShift;
           /* Now we must test if these slots are unlocked on some other task */
           if (privFreeLockAbsent(uiLeftSlot) && privFreeLockAbsent(uiRightSlot))
           { /* If not, we must test if the filling conditions on this task are fulfilled
              * by the queue. If it is a mutex, privSizeFitsQueu returns true per default. If
              * we do not have queues at all testing is no needed. */
            #if (defUseQueus == cfgTrue)
              if (privSizeFitsQueu(uiLeftSlot,loopTCB->siQueuLeftLock) && privSizeFitsQueu(uiRightSlot,loopTCB->siQueuRightLock))
            #endif
        #else
           /* Now we must test if these slots are unlocked on some other task */
           if (privFreeLockAbsent(uiRightSlot))
           { /* If not, we must test if the filling conditions on this task are fulfilled
              * by the queue. If it is a mutex, privSizeFitsQueu returns true per default. If
              * we do not have queues at all testing is no needed. */
            #if (defUseQueus == cfgTrue)
              if (privSizeFitsQueu(uiRightSlot,loopTCB->siQueuRightLock))
            #endif
        #endif
          { /* This task seems to fullfil all requirements to be released. Make it so. */
            privUnblockTask(uiLoopTask | defParaLockStateKeep | defParaRetStateTrue); } } } } } }


#endif


#if (cfgUseLowPowerSleep == cfgTrue) && (includeTaskSleepAll == cfgTrue)

static void privPutAllTasksToSleep(void)
{ /* Call this if you want to put all tasks to sleep. It will put all running and
   * blocked, delayed tasks to sleep. If will check if files system operations
   * are busy, or if tasks are waiting for fs operations. Those tasks may run
   * until the fs actvities are over, and are subsequently put to sleep. It will
   * leave suspended and terminated tasks alone. */
  Tuint08 uiLoopTask;
  /* Run through all tasks. */
  for (uiLoopTask=0; uiLoopTask<defNumberOfTasks; uiLoopTask++)
  { TtaskControlBlock * loopTCB = privTcbList(uiLoopTask);
    /* Putting a task to sleep, we do not want to revive a suspended task via the
     * sleep. An already sleeping task requires no attention. Thus we must check if
     * the task we put to sleep was indeed running. */
    if ((loopTCB->uiTaskStatus & defBaseStopStateGetMask) == defBaseStopStateGo )
    { /* Putting a task blocked on the fs to sleep may lead to a crash at wakeup,
       * so putting these tasks to sleep must be postponed until later. In that case
       * we will return here. The reason is we have no way to specially store the kind of
       * block in a sleepstate, so at wakeup we cannot correctly reconstruct the state.
       * It is no problem if the task is actually reading from the file system
       * for such a task is not blocked, but writing files cannot be put
       * to sleep. First of all, this may lead to data loss, but, second, a sleep
       * instruction cannot be effectuated when a burnlock is active. Thus we must
       * check if any task is writing. If there are writing tasks, all tasks that
       * want to read are readblocked, but if there are no writing tasks, readers
       * cannot be blocked, for they are released immediately after the writing
       * has finished. Thus we come to the conclusion there is no real need to store
       * a read-block-sleeping state. */
      #if (cfgUseFileSystem == cfgTrue)
        /* Thus we exclude all those tasks from sleeping if the fs writeblock is activated
         * which are in read or write mode. (We cannot test for writeblock itself, since
         * there is always one task actually writing and not blocking. */
        if ( ((uiFsStatus & defFsWriteBlockGetMask) == defFsWriteBlockClear) ||
           ((loopTCB->defFsField & (defFsReadGetMask | defFsWriteGetMask)) == (defFsReadClear | defFsWriteClear)) )
      #endif
      /* Change the status to sleeping */
      { loopTCB->uiTaskStatus = (loopTCB->uiTaskStatus & defBaseSleepingSetMask) | defBaseSleepingTask; } } } }

#endif


#if (cfgUseFileSystem  ==  cfgTrue)

static void privReleaseFileBlocks(void)
{ /* Call this method after you are done with file operations on a task and new tasks
   * may be given the opportunity to be released. Usually this is after a fileClose on
   * a file write operation, or after all reads are finished. */
  /* First check if we have more task on the file system. If not, we can only arrive
   * here if the current (and only!) task has completed read or write operations and
   * closes the access to the file system. Now, since there are no other tasks that
   * may want access we have little to do. */
  #if (defUseFsOnMultipleTasks == cfgTrue)
    /* It seems we have. We have to see in which state the file system is
     * Check if we have a write lock. When we have a write lock there must be at least
     * one task actually writing, so it makes no sense to release anything. When we don't
     * have a write lock we may have a read lock (i.e. there are tasks actually reading, but
     * we still search for tasks that may want to write. If we find these, we should not
     * accept new reads, until the tasks wanting to write has had the opportunity to do so. */
    if ((uiFsStatus & defFsWriteBlockGetMask) == defFsWriteBlockClear)
    { /* It seems we don't have a write task, so loop through all tasks and search
       * for write blocks. We only search in those tasks making use of the file system. */
      Tuint08 uiLoopTask;
      for (uiLoopTask=defTaskNumberFileSystemBegin; uiLoopTask<defTaskNumberFileSystemEnd; uiLoopTask++)
      { /* Now we must check if this is indeed a task with a file block (it could be an other kind of block) and if this
         * block is in the write mode. */
        if ((privTcbList(uiLoopTask)->uiTaskStatus & (defBaseFileBlockedGetMask | defBaseDressGetMask)) == (defBaseFileBlockedTask | defBaseDressWrite))
        { /* All right, this task is waiting for an opportunity to write to the file system.
           * There are two possibilities, or we can directly deblock to a lock, or we must issue
           * a write request. In both cases we must activate the write block in the file system
           * status (If there is already a read block present, this write block works as a local
           * flag signaling that a task is waiting for write access. Note we can never arrive here
           * in the other situation where both read and write block are activated, namely a writing
           * task waiting for file close. */
          uiFsStatus = (uiFsStatus & defFsWriteBlockSetMask) | defFsWriteBlockActive;
          /* If there is not single-write-multiple-read synchronizer active, read blocks are not
           * possible (we read under the write lock, this is always permitted. */
          #if (cfgUseFileSystemConcurrentRead == cfgTrue)
            /* Now, if the read block is set we are done, cause the write request is already set,
             * nothing more can be done. In case it is not set, we may appoint access to the current
             * task, and give it write access. */
            if (((uiFsStatus & defFsReadBlockGetMask) == defFsReadBlockClear))
          #endif
          { /* Here we must activate the task for writing, which implies that its task number is put into
             * the first nibble of the FsStatus. Furthermore, we clear the burn lock (it should already be
             * cleared btw). */
            uiFsStatus = ((uiFsStatus & (defFsBurnBlockSetMask & defFsWriteNumberSetMask)) | (defFsBurnBlockClear | (uiLoopTask << defFsTaskNumberShift) ) );
            /* The task was already blocked (we tested that, and besides, every task is blocked at
             * file-open per default). So we must unblock, but keep the lock. */
            privUnblockTask(uiLoopTask | defParaLockStateKeep | defParaRetStateTrue); }
          break; } } }
    /* If we have simultaneous read possibility we must do some more research. */
    #if (cfgUseFileSystemConcurrentRead == cfgTrue)
      /* It may seem strange, but we start again by looking at the WriteBlokc bit in the
       * FS status. If the bit is set, there are two possibilities. First, the read bit is
       * still cleared, so there is write action, and no further action with respect to reading
       * is needed. If the read bit is set also, it must be been set before we entered this method,
       * and the write bit must be interpreted as write request. In that case no new read tasks
       * may be activated. Same result, nothing to do. */
      if ((uiFsStatus & defFsWriteBlockGetMask) == defFsWriteBlockClear)
      { /* No write bit, thus there is room for a new read task. Run through all tasks. */
        Tuint08 uiLoopTask;
        for (uiLoopTask=defTaskNumberFileSystemBegin; uiLoopTask<defTaskNumberFileSystemEnd; uiLoopTask++)
        { /* Now we must check if this is indeed a task with a file block (it could be an other kind of block) and if this
           * block is in the read mode. */
          if ((privTcbList(uiLoopTask)->uiTaskStatus & (defBaseFileBlockedGetMask | defBaseDressGetMask)) == (defBaseFileBlockedTask | defBaseDressRead))
          { /* This task want to perform a read action. This is certainly possible, without further restrictions
             * since reading may be done in parallel. */
            uiFsStatus = ((uiFsStatus & defFsReadBlockSetMask) | defFsReadBlockActive);
            /* We count the number of read tasks in the least significant nibble of the FS status. If this is
             * the first read task, we can be certain the number is zero to start with. It has been cleared
             * after the last write operation, or has been cleared by down counting the last read operations. */
            uiFsStatus += defFsReadInc;
            /* Release a task for reading. It was already blocked, we switch it to lock. Since this kind
             * of call may timeout, we return true. */
            privUnblockTask(uiLoopTask | defParaLockStateKeep | defParaRetStateTrue); } } }
      else
      { /* If we indeed have write request, we must clear that flag, since it must be a local
         * flag. Outside this method, activating both read and write blocks is interpreted
         * as a closing write tag. */
        if ((uiFsStatus & defFsReadBlockGetMask) == defFsReadBlockActive)
        { /* Both flags are active, the write block must be cleared. */
          uiFsStatus = ((uiFsStatus & defFsWriteBlockSetMask) | defFsWriteBlockClear); } }
      #endif
  #endif
  /* If we make use of low power sleep, putting task to sleep who perform file operations is a
   * delicate matter. Inside the sleepAll method the sleep request bit on the FS status has been
   * activated. As soon as all tasks have completed file write access, and this is the place to test,
   * we may retry to put some or all tasks to sleep.  This situation is equal for one and more tasks
   * on the file system. */
  #if (cfgUseLowPowerSleep == cfgTrue) && (includeTaskSleepAll == cfgTrue)
    /* If we want to put a task to sleep all blocks must be resolved and no task may be writing.
     * This is however not the point to check this. If we arrive here we know for sure that there is
     * one task who finished writing or reading operations. Thus if we requested for sleep, we give it
     * an other try. */
    if ((uiFsStatus & defFsSleepRequestGetMask) == defFsSleepRequestActive) { privPutAllTasksToSleep(); }
  #endif
  }

#endif


#if (cfgUseFileSystem  ==  cfgTrue)

static Taddress privFileLocation(Tuint08 uiFileNumber, Tuint08 uiOffset)
{ /* Use this method to find the location of a particular file in eeprom. The lengths
   * of all predefined files are determined at compile time and are stored in flash.
   * Since all files are placed just one after the other, starting with the first
   * file after the 'fat', calculation of the location is easy.
   * We fill the location pointer with the location of the first file (number 0) and the
   * offset. Note that this location in itself may mean nothing. */
  #if (cfgUseFileSystemMaintainFAT == cfgTrue)
    /* If we make use of a FAT, we must correct for its size. For every file one byte containing its length. */
    Tuint16 pFileLoc = defFsNumberOfAllFiles + uiOffset;
  #else
    /* When there is no FAT, files start at the bottom of the eeprom. */
    Tuint16 pFileLoc = uiOffset;
  #endif
  /* Now test if we have a predefined file or a default file */
  if (uiFileNumber >= defFsNumberOfPreFiles)
  { /* Correct the start address for default files, which is known at compile time.  */
    pFileLoc += ((Tuint16) (uiFileNumber-defFsNumberOfPreFiles))*FileSpaceStandard + defTotalPreFileSpace; }
  else
  { /* Determine if we have equal sized files or if we have different sizes per file. */
    #if (defFilePreSpaceConstant == cfgTrue)
      /* All named files have the same size, so */
      pFileLoc += uiFileNumber * defFilePreSpaceFixed;
    #else
      /* in flash we have stored the length of all predefined files, which we must subsequently add. */
      while(uiFileNumber--)
      { /* Read the length of all predefined files from flash, and add it to the location: */
        pFileLoc += portFlashReadByte(Tuint08,uiFileSpace[uiFileNumber]); }
    #endif
  }
  /* at this moment we have the correct start address in eeprom, return it. */
  return (Taddress) pFileLoc; }

#endif


#if (cfgUseFileSystem  ==  cfgTrue) && ( ((cfgUseFileSystemMaintainFAT == cfgTrue) && (includeTaskFileAppendByte == cfgTrue)) || (cfgCheckMethodUse == cfgTrue) )

static Tuint08 privFileSpace(Tuint08 uiFileNumber)
{ Tuint08 uiResult;
  /* Use this to obtain the space available for a file. If the file is a default file ... */
  if (uiFileNumber >= defFsNumberOfPreFiles)
  { /* ... the answer is simple  */
    uiResult = FileSpaceStandard; }
  else
    { /* Determine if we have equal sized files or if we have different sizes per file. */
      #if (defFilePreSpaceConstant == cfgTrue)
        /* fixed sizes  */
        uiResult = defFilePreSpaceFixed;
      #else
        /* otherwise we must look it up from a table in flash. */
        uiResult = portFlashReadByte(Tuint08,uiFileSpace[uiFileNumber]);
     #endif
    }
  return uiResult; }

#endif


#if (cfgUseFileSystem  ==  cfgTrue)

static void privPrepareFileClose(Tuint08 uiTaskNumber)
{ /* Call this method to close the access to a file. You can also call it if the task does
   * not hold a file, if defUseFsField is activated. In that case no action is taken.
   * These are the situations that are handled
   * (1) FS-WriteBlock and current task holds write lock  => close writing operations, maybe issue a release request.
   * (2) FS-WriteBlock and current task holds write block => do nothing, there is nothing to be closed
   * (3) FS-WriteBlock and current task holds read lock => do nothing, there is nothing to be closed
   * (4) FS-ReadBlock and current task holds write lock => do nothing, there is nothing to be closed
   * (5) FS-ReadBlock and current task holds read lock => current task must be reading, close
   * (6) FS-ReadBlock & FS-WriteBlock and current task holds write lock => current task is waiting for closing, do nothing
   * (7) FS-ReadBlock & FS-WriteBlock and current task holds write block => other task is waiting closing, do nothing.
   */
  #if (defUseFsField == cfgTrue)
    TtaskControlBlock * taskTCB = privTcbList(uiTaskNumber);
  #endif
  /* First we test if the FS write block is set. */
  if (((uiFsStatus & defFsWriteBlockGetMask) == defFsWriteBlockActive))
  { /* If there is a chance we arrive here from an other place than the current task we must test if
     * the task at hand is the one the FS write block is holding. If not, this must coincide, and we
     * can leave this piece of code out. */
  #if (defUseFsField == cfgTrue)
    /* We must sort out of the requested task for closing is indeed writing at the moment. If not,
     * there is nothing to close. */
    #if (defUseFsOnMultipleTasks == cfgTrue)
      /* If we have more tasks writing, we cannot just check the fsfield, for all tasks
       * writing are in the write mode.  The number of the task writing is contained in
       * the status. If this matches we do not need to check if the task is in write mode
       * any more. This must be the case. One special case is if we come from the closeFile call,
       * this call simply supplies the current file number, which should we fine. (It is an error
       * to call closeFile on a task that does not have a file open.*/
      if ((uiTaskNumber == ((uiFsStatus & defFsWriteNumberGetMask) >> defFsTaskNumberShift)) || (uiTaskNumber == defCurrentTaskNumber))
    #else
      /* If we have one possible task writing, we can just check the fsfield, if it
       * is set this must be the one. There are no other blocking ones. */
      if  ((taskTCB->defFsField & defFsWriteGetMask) == defFsWriteActive)
    #endif
  #endif
    { /* We may come here with burn block activated if the current write task was still writing while
       * being killed. In that case we must put the fs in release request state. The task must still
       * be properly closed. Start with the latter. */
      #if (defUseFsField == cfgTrue)
        /* Apart from the FsStatus this is all we need to do. */
        taskTCB->defFsField = (taskTCB->defFsField & defFsWriteSetMask) | defFsWriteClear;
      #endif
      /* Now test the burn block. */
      if (!portFSWriteReady())
      { /* Ok we have a burn block, thus, inform the outside world */
        privTrace(traceBurnLock);
        /* Set the release request (contains burn block), leave a possible sleep request intact */
        uiFsStatus = (uiFsStatus & defFsFreeSetMask) | defFsSetReleaseRequest ; }
      else
      { /* Clear everything in the status register, except a possible request for sleep. */
        uiFsStatus = (uiFsStatus & defFsFreeSetMask) | defFsFree;
        /* Since we have no burn block we may release new tasks. */
        privReleaseFileBlocks(); } } }
  /* Only if we make use of simultaneous reads, we have read blocks. */
  #if (cfgUseFileSystemConcurrentRead == cfgTrue)
    /* This else is needed otherwise this function may treat release requests for write blocked tasks. We do not want that. */
    else
    { /* Check if we are dealing with an FS read block*/
      if ((uiFsStatus & defFsReadBlockGetMask) == defFsReadBlockActive)
      { /* OK we are reading indeed. This means that if we are dealing with the current task we must be reading, and
         * otherwise we must check this. Since this function may be called for any task from TaskInit of KillTask. */
        #if (defUseFsField == cfgTrue)
          if  ((taskTCB->defFsField & defFsReadGetMask) == defFsReadActive)
        #endif
        { /* And clear the read mode of that task when present. */
          #if (defUseFsField == cfgTrue)
            taskTCB->defFsField = (taskTCB->defFsField & defFsReadSetMask) | defFsReadClear;
          #endif
          /* If we have more tasks reading, we must keep track of the number of tasks, if not
           * the matter is simpler.*/
          #if (defUseFsOnMultipleTasks == cfgTrue)
            /* Now we are sure this task is indeed reading. Remove one reader from the read counter. */
            uiFsStatus -= defFsReadInc;
            /* Now we must check if there are any reading tasks left. This could be the last.
             * Shifting with defFsReadCount is not needed since we only need to compare with zero. */
            if ((uiFsStatus & defFsReadCountGetMask) == 0)
            { /* If there are not readers left, clear the FS read block. */
              uiFsStatus = ((uiFsStatus & defFsReadBlockSetMask) | defFsReadBlockClear);
            /* And check if there are new tasks waiting to read. */
            privReleaseFileBlocks(); }
          #else
            /* We are the only reader so clean the block. */
            uiFsStatus = ((uiFsStatus & defFsReadBlockSetMask) | defFsReadBlockClear);
            /* Although there cannot be any other tasks, there can be sleep request. */
            privReleaseFileBlocks();
          #endif
        } } }
  #endif
  /* Done, we handled writing as well as reading. */  }
#endif


void privInitOs(void)
{ /* After a context switch we always enter here. We shall make all necessary preparations
   * to correctly handle the situation. That is, check if the task has run properly,
   * First thing to do is to perform the checks. This is needed since all this data is
   * stored in background variables. These are destroyed as soon as we make use of the stack. */
  #if (cfgCheckTaskStack == cfgTrue)  || (cfgCheckRegisters == cfgTrue) || (cfgCheckWatermarks == cfgTrue) ||  (cfgUseLowPowerSleep == cfgTrue)
    Tuint08 uiLastOsStatus = uiOsStatus;
  #endif
  /* This is the moment and the place to state that we are actually running the OS.  We
   * certainly need to do it before we reactivate the interrupts, since we want to be
   * able to ask, inside the interrupt, where the system was. However, we need to
   * remember what state we came from, for some tests below, therefore we stored that
   * state in uiLastOsStatus. */
  uiOsStatus = ((uiOsStatus & defContextSetMask) | defContextStateOs);
  /* We can start checking the results from the context save. */
  #if (cfgCheckTaskStack == cfgTrue)  || (cfgCheckRegisters == cfgTrue) || (cfgCheckWatermarks == cfgTrue)
    /* These checks are only sensible if we where previously performing a task */
    if ((uiLastOsStatus & defContextGetMask) == defContextStateTask)
    { /* The watermark checks are meant to measure to what extend use is made of the stack and the
       * registers. The maximal value of the stack level is recorded here, since this place is one
       * of the placed probably consuming most of the stack. If we set the TDB uiRegisterUse to 0xFF
       * the real register use can here be collected. Note that the value of uiStackMax is only
       * correct when there was no stack error, since in that case the context
       * was not saved at all.
       * We first check the watermarks and after that the register use and
       * stack use since if the latter result in an error the background variables are corrupted
       * leading to incorrect watermarks. In the situation we have checking activated we have
       * four bumper variables in the stack which are just consumed by the the call to privInitOs
       * and to privTcbList(defCurrentTaskNumber). Note that this is a delicate matter, if the compiler decides to push
       * some more variables at the start of privInitOs() or privTcbList(defCurrentTaskNumber) this result is faulty. */
      #if (cfgCheckWatermarks == cfgTrue)
        TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
        if ((curTCB->uiStackMax)<(curTCB->pcStackLevel)) { curTCB->uiStackMax = (curTCB->pcStackLevel); }
        /* uiRegisterCheck contains the result of the check on the register use, i.e. all register blocks
         * used are marked by a bit set. */
        curTCB->uiRegisterUse |= xOS.pxSave.uiRegisterCheck;
      #endif
      /* Check if we did not exceed the stack. In fact, in most cases this check is if we
       * would have exceeded the stack, given we would have saved the entire context. In the
       * portSaveContext however, we first calculated if the context fits in the remaining
       * stack space, if not, the context is not saved, and uiStackTCheck indicates a stack error. Of course
       * we can never restart that task, but that would not have been possible in a reliable way
       * anyhow. Now, at least we may have reduced the damage to other parts of the memory. */
      #if (cfgCheckTaskStack == cfgTrue)
        if ((xOS.pxSave.uiStackTCheck & defCheckStackGetMask) == defCheckStackStateError)
        { /* Now we can have two situations. First the stack may be overflown already, which is
           * is an fatal error since most likely other tasks are damaged as well, or the context
           * may not fit the stack. The former cannot be detected with 100% certainty, but if we can
           * see it has happened, we should issue an fatal error */
          privShowError((errTaskStackWillOverflow | errTaskStateCurrent | errOsStateReturn), errCurrentTask ); }
        /* Since issuing an error damages the background variables it makes no sense to check for registers
         * is an error occurred here. Furthermore, because an detected error in the stack size will skip the
         * proper checking of register use, that information is not reliable may indicate a false error.
         * therefore we must report an error in the stack check before an error in the registers, and refrain
         * from reporting the latter. */
        else
      #endif
      /* Check if the task has used registers that it did say it would not. In the portSaveContext all
       * registers used are scanned, and one byte of information is build, uiRegisterCheck, which
       * contains set bits at the appropriate places. Now, this byte is compared with the proclaimed
       * use of registers, uiRegisterUse. Any bit outside this mask indicates an error.
       * Please note that this error does not have to be fatal. Since, it may be that some registers
       * are used only within a realm in which no task context switch is possible. That registers need
       * not to be saved. So an error here may be ignored if you know what you are doing. For the
       * moment however, we decide to stop the task anyway.*/
      #if (cfgCheckRegisters == cfgTrue)
        { /* Determine which register blocks where used without authorization, that are all register blocks
           * used (the ones not check are already set to zero) excluding any of the blocks where use
           * is allowed. */
          Tuint08 uiRegisterViolation = xOS.pxSave.uiRegisterCheck & ~xOS.pxSave.uiRegisterUse;
          /* If there is any such block we must report an error */
          if (uiRegisterViolation != 0)
          { /* We determine the highest block number in violation, that could be 7  (regs 28,29,30,31)*/
            Tuint08 uiRegBlock = 7;
            /* Check if this is indeed so */
            while ( (uiRegisterViolation & 0x80) == 0 )
            { /* If not it could be 6, so we decrease the counter one ... */
              uiRegBlock--;
             /*  and shift the 6th block to the 7th */
              uiRegisterViolation <<= 1; }
            /* By now we have found that block, report the error, the faulty block in the slot slot. */
            privShowError((errTaskIllegalRegisterUse | errTaskStateCurrent | errOsStateReturn), (uiRegBlock << errInfoNumberShift) | errCurrentTask);  }  }
      #else
        /* Empty statement to close a possible dangling else from (cfgCheckTaskStack == cfgTrue) */
        { }
      #endif
    }
  #endif
  /* Interrupts. That is an interesting case. We have different models of interrupt handling and OS
   * protection in the Femto OS. That is discussed elsewhere. In general, global interrupts are disabled when
   * we arrive here, this is done in all methods calling the saveContext (and not in the saveContext
   * itself!). Thus if we want to protect the OS against interrupts, we do not have to do anything
   * here. If this protection is not needed, we must reactivate the interrupts. This must be done
   * in two stages. We must be sure we do not get a tick interrupt while being in the OS, since
   * we would not know how to react (but, in general such an interrupt will not come since it would
   * indicate a tick rate to high), and then we may reactivate the global interrupts.
   * The default situation inside the OS is deactivated tick interrupts, and that has been taken care of
   * in the portSaveContext. Thus we only have to */
  #if (cfgIntOsProtected == cfgFalse)
    privEnableGlobalInterrupts();
  #endif
  /* If we included the use of the Low Power sleep ... */
  #if (cfgUseLowPowerSleep == cfgTrue)
    /* ... and if we come from a 'sleep task', i.e.. if we have been in a low power sleep, there is some work to
     * be done. We assume that delay tasks have already been woken and the tick counter has already
     * been corrected. */
    if ((uiLastOsStatus & defContextGetMask) == defContextStateSleep)
    { /* The tick timer could have made several rounds. We ignore these, since this is (must be)
       * corrected manually. Further, we set the next interrupt far away, we do not want any
       * hanging interrupts. */
      #if (cfgUseEquidistantTicks == cfgFalse)
        portReadAndResetTimer(defSubTicksMax);
      #endif
      /* Now the time has come to loop trough all the sleeping tasks and kiss them. */
      Tuint08 uiLoopTask;
      for (uiLoopTask=0; uiLoopTask<defNumberOfTasks; uiLoopTask++)
      { TtaskControlBlock * loopTCB = privTcbList(uiLoopTask);
        /* Check if the loopTask is indeed in the sleep state. */
        if ((loopTCB->uiTaskStatus & defBaseSleepingGetMask) == defBaseSleepingTask)
        { /* Wake the task. */
          loopTCB->uiTaskStatus = (loopTCB->uiTaskStatus & defBaseStopStateSetMask) | defBaseStopStateGo;
          /* When we keep track of the exact wake up moments, this is on of the places to store
           * the wake time. Of course, for accurate frequencies, the tick counter must already be
           * manually corrected. */
          #if (cfgUseDelay == cfgTrue) && (cfgUseCorrectWakeupTimes == cfgTrue)
            /* We must be careful with one situation. A sleeping task could be delayed also, and
             * the delay may not yet be finished. If so, we must not destroy the delay time
             * variables, for which there is no need also, since the should already contain the
             * correct wakeup time in the future. */
            if ((loopTCB->uiTaskStatus & defBaseDelayStateGetMask) == defBaseDelayStateWake)
            { loopTCB->uxDelay.Full = uxTickCount.Full; }
          #endif
        } }
      /* DISCUSSION
       * If there were sleeping tasks that were blocking or did possess some slots, is that
       * a problem? In other words, should we release some blocking tasks at this place?
       * Well, you can only call sleep on you yourself, and if you so, you cannot block, but
       * you may possess slots. Of course it is very dangerous to put a task that is possessing
       * a mutex to sleep, since it will only be released at the next wakeup. We assume that if
       * you do, you know what you are doing. The other possibly is that all tasks were put
       * sleep simultaneously. Then, since they also wakeup simultaneously again, there is no
       * danger in having dangling blocks. Thus, since there can be no dangling blocks with
       * the sleep feature, there is no need to call privReleaseSyncBlockingTasks() at this place.
       */
    }
  #endif
  /* If we have synchronization tasks and make use of the return values of some functions trying
   * to obtain locks, we must set the return values to Non. The method that will release the task
   * will fill the variable with the appropriate value. This is the correct place for that instruction
   * since we do not know if we indeed will switch the task. The task may not be switched at all. */
  #if (cfgUseSynchronization != cfgSyncNon) && (defUseBoolReturns == cfgTrue)
    { TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
      curTCB->defRetField = (curTCB->defRetField & defRetSetMask) | defRetNon; }
  #endif
}


#if ((cfgIntUserDefined == cfgTrue) && (cfgUseLoadMonitor == cfgTrue) && ((includeIsrEnter == cfgTrue) || (includeIsrStartLoad == cfgTrue)))

void isrStartLoad(void)
{ /* IsrLoad measures the total time spend in interrupts. We must fill it with the actual value
   * of the subtick timer. This is true in both cases, equidistant ticks or not. Now we want to
   * collect the number of subticks spend in the interrupt. Thus we subtract the current value of the
   * subtick counter. If the uiIsrLoadTemp underflows that is not a real problem, as long as it
   * does not underflow twice, which does not happen easily. Since we may assume that during
   * the isr there will be no tick interrupt, the portReadTimer will keep returning increasing
   * values, even if the timer reset, since the interrupt was not handled. */
  uiIsrLoadTemp -= portReadTimer(); }

#endif


#if ((cfgIntUserDefined == cfgTrue) && (cfgUseLoadMonitor == cfgTrue) && ((includeIsrExit == cfgTrue) || (includeIsrStopLoad == cfgTrue)))

void isrStopLoad(void)
{ /* After the interrupt is done, we simply add the new value of the timer to get the
   * time spend in the interrupt. If we have more interrupts during a task (this time registration
   * also works in case of absence of OS protection, they are automatically added. */
  uiIsrLoadTemp += portReadTimer(); }

#endif


#if (cfgIntUserDefined == cfgTrue) && (cfgIntOsProtected == cfgTrue) && (includeIsrExit == cfgTrue)

void privIsrExit(void)
{ /* Arrive here when you end the interrupt. This method pairs with the IsrEnter routine.
   * First switch back to the OS stack. Even if we executed the isr in OS space it is necessary
   * to reset the stack. */
  privSetStack(&xOS.StackOS[OSstackInit]);
  /* If we had a separate ISR stack, the portEnterISR method modified the uiOsStatus to indicate
   * that we executed with a separate stack space. This must be undone. */
  #if (cfgIntSwitchUsesOSstack == cfgFalse)
    uiOsStatus = ((uiOsStatus & defContextSetMask) | defContextStateOs);
  #endif
  /* Start the OS, stating that we want an task switch. */
  privEnterOS(defActionTaskStateSwitch); }

#endif


#if ((cfgUseEquidistantTicks == cfgFalse) && (cfgCheckTiming == cfgTrue))

void privSubtickOverflow(void)
{ /* This method is only called from the subtick timer overflow interrupt. It indicates that
   * the subtick timer overflowed, thus loosing full ticks, a condition which should not
   * occur. The only thing to do is to report the error, which is fatal. */
  privShowError((fatTaskSubtickOverflow | errTaskStateCurrent), errCurrentTask ); }

#endif


void privTickYield(void)
{ /* Arrive here from a tick interrupt. It is assumed that global interrupts are already switched off.
   * We know we came from a tick, so the global interrupt must have been on before, make sure, the
   * task starts with global interrupts activated when restored. */
  portResqueGlobalInterruptActive();
  /* Since we have no parameters, we may safely save the context now. */
  portSaveContext();
  /* After saving the context (all registers are preserved, and the stack is switched, we initialize
   * the OS. */
  privInitOs();
  /* Inform that we interrupted the task*/
  privTrace(traceTickInterrupt);
  /* After initializing the OS, we may Enter the OS, indicating that we want a task switch and,
   * since we come from an interrupt, may need to add a tick. */
  privEnterOS(defActionTaskStateSwitch | defActionTickStateTick);  }


void main(void)
{ /* Interrupts are switched off at entrance.
   * Note that the behavior of the stack set up has been changed from gcc4.2.x to 4.3.x. From
   * the latter version onwards the stack is only set up before the main. The former version
   * did an initialization of the stack twice. Main is now a normal function. We only have to
   * make sure the stack is defined at all, but that is done in initialization code which
   * calls main. */
  /* All tasks must be set up. */
  Tuint08 uiLoopTask;
  /* Usually, there is some hardware related stuff that must be done (setting the pll,
   * choosing the main clock etc etc */
  portInit();
  /* If we choose to show the reset, this is the place to call it. Note that we should arrive
   * here only once. Usually coming here a second time indicates a major error in your code,
   * so it is nice if you got notified. I use it to rapidly blink all leds three times. */
  #if (cfgCheckReset == cfgTrue)
    privShowReset();
  #endif
  /* If we check the OS stack, we can perform the first check here to see if the stack is not
   * under the minimum required value, just to store all background variables. Seems trivial,
   * but I got that message several times now. */
  #if (cfgCheckOsStack == cfgTrue)
    /* If the OS stack is under the minimum, there is no point in starting the system, thus this
     * is made a fatal error. */
    if (StackSizeOS < defOsStackMinimum)  { privShowError((fatOsStackUnderMinimum | errTaskStateNon), errNoInfo); }
  #endif
  /* If present call appBoot code. This is used by the user to perform general initializations. */
  #if (callAppBoot == cfgTrue)
    appBoot();
  #endif
  /* Create a task control block for every task and set up a new context, as if the task
   * already existed before. */
  for (uiLoopTask=0; uiLoopTask<defNumberOfTasks; uiLoopTask++)
  { /* Since there is no status as off yet, we must renew it. However, we
     * can be pretty sure there are no locks yet. There is no need to
     * clean any, there set it to keep. The same holds true for any watermarks.
     * In case of inlined code, the  compiler knows which peace can be cut out. */
    privTaskInit(uiLoopTask,(defInitStatusRenew | defInitLockKeep | defInitWatermarksKeep));
  /* Sometimes the user want to do some one time initialization per task,
   * this is inside the init routines. Call it if present. Note that the initializations
   * are called per task, thus initialization cannot refer to each other.*/
  #if (callAppInit == cfgTrue)
    portFlashReadWord(fpInitTask,pxInitlist[uiLoopTask])();
  #endif
  }
  /* The timer is hardware dependent. The called routine below expects the tick interrupts
   * to be setup, but not activated yet.
   * The (tick) interrupt model used depends on a lot of parameters. At this point it is needed
   * to know that the routines starting a task, idle or sleep state manage the activation of
   * the tick interrupt themselves, so we do not need to take care here. */
  portSetupTimerInterrupt();
  /* All preparations are done, we may enter the OS to start the first task. We must really switch
   * since it may be possible that no task can be started, and than we must be able to switch to
   * idle task directly. */
  privEnterOS(defActionTaskStateSwitch);
  /* we never get here */
}

/* ========================================================================= */
/* API START =============================================================== */
/* ========================================================================= */

/**
 * Below find wrappers for all switching OS function calls with parameters. GCC is not
 * able to generate code with a function call as very first instruction, thus even before
 * a register move. So, if we need to call portSaveContext first, but keep all registers
 * unchanged, we must use this wrappers. The core functionality is provided by the function
 * bodies in the Femto OS. This trick is not needed for OS functions without parameters or
 * for non switching calls, i.e. functions executed in task space.
 */

#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * It is explained elsewhere in more detail how context switching deals with manual
 * switching calls. In short the procedure is.
 * - Call the function (for example DelayFromNow())
 * - Disable the global interrupts now (only two bytes on the stack)
 * - Call the portSaveContext
 * In the latter the status register is saved, so this is always saved with disabled
 * global interrupt. That is not what we want, thus we must rescue that state before
 * the global interrupts are disabled. This must be done with as few instructions as
 * possible, since these instructions are copied into every(!) switching call. It is
 * up to you how you want to rescue that information, since you must restore it in
 * the portRestoreContext yourself later on.
 * Context switches clear the global interrupt asap to prevent nested saved contexts.
 * Thus even before we call saveContext we want to clear the interrupt. We must save
 * the current state of the interrupt.
 * In the implementation below, we decided to use a general purpose register of the
 * avr (chosen in the port file) We demand bit 7 of this register to be zero upon
 * entry, but do not clear it here to save bytes. All other code who uses this register
 * must take care of this.
 */

/* Function wrapper for taskDelayFromNow. */
#if (includeTaskDelayFromNow == cfgTrue)

void taskDelayFromNow(Tuint16 uiTicksToWait)
{ portResqueGlobalInterruptState();
  portSaveContext();
  portJump(privDelayFromNowBody); }

#endif


/* Function wrapper for taskDelayFromWake. */
#if (includeTaskDelayFromWake == cfgTrue)

void taskDelayFromWake(Tuint16 uiTicksToWait)
{ portResqueGlobalInterruptState();
  portSaveContext();
  portJump(privDelayFromWakeBody); }

#endif


/* Function wrapper for taskRestart. */
#if (includeTaskRestart == cfgTrue)

void taskRestart(Tuint08 uiTaskNumber)
{ portResqueGlobalInterruptState();
  portSaveContext();
  portJump(privRestartBody); }

#endif


/* Function wrapper for taskYield. */
#if (includeTaskYield == cfgTrue)

void taskYield(void)
{ portResqueGlobalInterruptState();
  portSaveContext();
  portJump(privYieldBody); }

#endif


/* Function wrapper for taskSuspendMe. */
#if (includeTaskSuspendMe == cfgTrue)

void taskSuspendMe(void)
{ portResqueGlobalInterruptState();
  portSaveContext();
  portJump(privSuspendMeBody); }

#endif


/* Function wrapper for taskSleep. */
#if (cfgUseLowPowerSleep == cfgTrue) && (includeTaskSleep == cfgTrue)

void taskSleep(void)
{ portResqueGlobalInterruptState();
  portSaveContext();
  portJump(privSleepBody); }

#endif

/* Function wrapper for taskKill. */
#if (includeTaskKillTask == cfgTrue)

void taskKillTask(Tuint08 uiTaskNumber)
{ portResqueGlobalInterruptState();
  portSaveContext();
  portJump(privKillTaskBody); }

#endif


/* Function wrapper for taskSleepAll. */
#if (cfgUseLowPowerSleep == cfgTrue) && (includeTaskSleepAll == cfgTrue)

void taskSleepAll(void)
{ portResqueGlobalInterruptState();
  portSaveContext();
  portJump(privSleepAllBody); }

#endif


/* Function wrapper for taskWaitForTasks. */
#if (cfgUseSynchronization != cfgSyncNon) && (includeTaskWaitForTasks == cfgTrue)

#if (cfgUseTimeout == cfgTrue)
  Tbool taskWaitForTasks(Tuint08 uiSlot, Tuint08 uiNumberOfTasks, Tuint16 uiTicksToWait)
#else
  void taskWaitForTasks(Tuint08 uiSlot, Tuint08 uiNumberOfTasks)
#endif
{ portResqueGlobalInterruptState();
  portSaveContext();
  portJump(privWaitForTasksBody); }

#endif


/* Function wrapper for taskSyncRequest. */
#if (cfgUseSynchronization != cfgSyncNon) && ( (includeTaskQueu == cfgTrue) || (includeTaskMutex == cfgTrue) )

#if (cfgUseTimeout == cfgTrue)
  #if (cfgUseSynchronization == cfgSyncDoubleBlock)
    Tbool taskSyncRequest(Tuint08 uiSlotSlot, Tsint08 siFreeLeftFilling, Tsint08 siFreeRightFilling, Tuint16 uiTicksToWait)
  #else
    Tbool taskSyncRequest(Tuint08 uiSlotSlot, Tsint08 siFreeRightFilling, Tuint16 uiTicksToWait)
  #endif
#else
  #if (cfgUseSynchronization == cfgSyncDoubleBlock)
    void taskSyncRequest(Tuint08 uiSlotSlot, Tsint08 siFreeLeftFilling, Tsint08 siFreeRightFilling)
  #else
    void taskSyncRequest(Tuint08 uiSlotSlot, Tsint08 siFreeRightFilling)
  #endif
#endif
{ portResqueGlobalInterruptState();
  portSaveContext();
  portJump(privSyncRequestBody); }

#endif


/* Function wrapper for taskSyncRelease. */
#if (cfgUseSynchronization != cfgSyncNon) && ( (includeTaskQueu == cfgTrue) || (includeTaskMutex == cfgTrue) )

void taskSyncRelease(Tuint08 uiSlotSlot)
{ portResqueGlobalInterruptState();
  portSaveContext();
  portJump(privSyncReleaseBody); }

#endif

/* Function wrapper for taskWaitForFsAccess. */
#if (cfgUseFileSystem  ==  cfgTrue)

void privWaitForFsAccess(void)
{ portResqueGlobalInterruptState();
  portSaveContext();
  portJump(privWaitForFsAccessBody); }

#endif

/* Function wrapper for taskFileOpen. */
#if (cfgUseFileSystem  ==  cfgTrue) && (includeTaskFileAccess == cfgTrue)

#if (cfgUseTimeout == cfgTrue)
  #if (cfgUseFileSystemConcurrentRead == cfgTrue)
    Tbool taskFileOpen(Tbool bReadOnly, Tuint16 uiTicksToWait)
  #else
    Tbool taskFileOpen(Tuint16 uiTicksToWait)
  #endif
#else
  #if (cfgUseFileSystemConcurrentRead == cfgTrue)
    void taskFileOpen(Tbool bReadOnly)
  #else
    void taskFileOpen(void)
  #endif
#endif
{ portResqueGlobalInterruptState();
  portSaveContext();
  portJump(privFileOpenBody); }

#endif

/* Function wrapper for taskFileClose. */
#if (cfgUseFileSystem  ==  cfgTrue) && (includeTaskFileAccess == cfgTrue)

void taskFileClose(void)
{ portResqueGlobalInterruptState();
  portSaveContext();
  portJump(privFileCloseBody); }

#endif

/* Function wrapper for taskWaitForEvent. */
#if (cfgUseEvents == cfgTrue) && (includeTaskWaitForEvents == cfgTrue)

#if (cfgUseTimeout == cfgTrue)
  Tbool taskWaitForEvent(Tuint08 uiEvent, Tuint16 uiTicksToWait)
#else
  void taskWaitForEvent(Tuint08 uiEvent)
#endif
{ portResqueGlobalInterruptState();
  portSaveContext();
  portJump(privWaitForEventBody); }

#endif


#if ( includeTaskStackCheck == cfgTrue )

Tuint16 taskStackCheck(Tuint08 uiExtraStackSafety)
{ /* Arrive here to check the amount of free stack space. We only want (and can, due
   * to the required use of background variables) to include this code in case we have
   * stack checking activated */
  #if (cfgCheckTaskStack == cfgTrue)
    /* To be sure of correct calculation we always work with a 16 bit stack size. */
    Tuint16 uiStackFree;
    Taddress pStack;
    /* Obtain the stack pointer at this very moment. Note that the call to the function
     * taskStackCheck is included, but privGetStack is a macro, so that does not count for
     * extra bytes on the stack */
    privGetStack(pStack);
    /* The calculation below equals the calculation done in portSaveContext. The call to this function
     * adds an other 2 bytes, equaling the amount the tick interrupt would add. The latter actually
     * adds four bytes, but the stack calculation itself is done after the temporarily return address
     * has been moved from the stack to the OS stack. Thus that matches precisely. The only thing we
     * must realize is that the stack must be able to accommodate that extra two bytes for that period.
     * However, that fact cannot be circumvented, otherwise no check is possible. The user must always
     * add two bytes (or as much a one call takes) to the stack. Compared to this measurement, that is
     * somewhere in the code, there will no be an problem, since usually there are registers to be saved
     * too. */
    #if (cfgSysStackGrowthUp == cfgTrue)
      /* Please do not trust any calculation with respect to cfgSysStackGrowthUp true, since this is
       * untested, and therefore most probably incorrect, at the moment */
      #if (cfgCheckTaskStack ==cfgTrue)
        if ((pStack + uiExtraStackSafety)> xOS.pxSave.pcStackLimit)
        { privShowError((fatTaskStackOverflowed | errTaskStateCurrent) , errCurrentTask); }
      #endif
      uiStackFree = xOS.pxSave.pcStackLimit - pStack - StackSafety;
    #else
      #if (cfgCheckTaskStack ==cfgTrue)
        /* Since this is a 16 bit addition/comparison, we do not have any hinder from a 256 bytes boundary. */
        /* TODO: v0.90: We issue a fatal error here because it is not possible to return to this
         * place. If it where possible to force a context switch, this problem would not exist, and
         * this call would behave like an ordinary stack overflow error.
         */
        if (pStack < (xOS.pxSave.pcStackLimit + uiExtraStackSafety))
        { privShowError((fatTaskStackOverflowed | errTaskStateCurrent) , errCurrentTask); }
      #endif
      /* We return the amount of free stack contrary to the stack level, because we do not have the
       * the total stack size at hand here. It resides inside flash, and it would cost a lot of code
       * to obtain that information. Besides that, it would still be an estimate, not the real level.*/
      uiStackFree = pStack- xOS.pxSave.pcStackLimit - StackSafety;
    #endif
    /* At this point we do not include watermark calculations The reason is twofold.
     * (1) As indicated above, it takes a lot of code to find the stack level and it would still
     * be an estimated value, whereas the watermarks should represent real measured values.
     * (2) Maybe an interrupt is not possible at the point this function is used, that would
     * compromise the result even further.  */
    return uiStackFree;
  #else
    /* Without stack checking return 0. Of course the user should remove the function for which is
     * no real need, but that may not be practical during development.  */
    return 0;
  #endif
}

#endif


#if ( includeIsrStackCheck == cfgTrue )

Tuint16 isrStackCheck(Tuint08 uiExtraStackSafety)
{ /* Arrive here to check the amount of free stack space. In this routine we do calculate the watermark
  * for isr, see below why. Furthermore it makes no sense to check the isr stack if we let the
  * isr make use of the OS stack. */
  #if ((cfgCheckIsrStack == cfgTrue) || (cfgCheckWatermarks == cfgTrue)) && (cfgIntSwitchUsesOSstack == cfgFalse)
    Taddress pStack;
    /* Since we allow for large stacks in the isr, we calculate with 16 bit here. */
    Tuint16 uiIsrStackLevel;
    /* Obtain the stack pointer at this very moment. Note that the call to the function
     * isrStackCheck is included, but privGetStack is a macro, so that does not count for
     * extra bytes on the stack */
    privGetStack(pStack);
    /* Calculate the present level. */
    #if (cfgSysStackGrowthUp == cfgTrue)
      uiIsrStackLevel = ((Tuint16) pStack - (Tuint16) &StackISR[0]);
    #else
      uiIsrStackLevel = ((Tuint16) &StackISR[StackSizeISR-1] - (Tuint16) pStack);
    #endif
    /* Contrary to the situation in the taskStackCheck routine we do a watermark calculation
     * here because
     * (1) We do have the total stack size at hand.
     * (2) There is no other place to do it. Since we do not expect a context switch, stack information
     * is hard to come by */
    #if (cfgCheckWatermarks == cfgTrue)
      /* We perform the watermark calculation before the check, since, even if the check returns an error,
       * that error may be artificial and this calculation may be valid. */
      if (uiIsrStackMax < uiIsrStackLevel) { uiIsrStackMax = uiIsrStackLevel; }
      /* For tasks we have a separate manner to calculate the watermark stack level, and that is
       * to check the stack explicitly for bytes that differ from defStackInitByte. The isr stack
       * is filled with zeros per default (.bss cleaning) so a similar check would be possible
       * here also. Up to now we did not implement it. */
    #endif
    #if (cfgCheckIsrStack ==cfgTrue)
      /* In the calculation  do not take the regular StackSavety into account, That parameter is
       * used solely for tasks. Besides, we have two bytes extra anyway. */
      if ((uiIsrStackLevel+uiExtraStackSafety)>StackSizeISR)
      { privShowError((fatIsrStackOverflowed | errTaskStateNon), errNoInfo); }
    #endif
    /* Calculate the free space and return it. */
    return ((Tuint16) StackSizeISR - uiIsrStackLevel);
  #else
    /* Without stack checking return 0. Of course the user should remove the function for which is
     * no real need, but that may not be practical during development.  */
    return 0;
  #endif
}

#endif


#if (includeTaskYield == cfgTrue)

void privYieldBody(void)
{ /* Called from user code to switch the task. This is a switching call, let us first initialize the OS.
   * (It is a 'body' function too, see the explanation of this design elsewhere.) */
  privInitOs();
  /* The Os is started all delicate information is saved or processed. Let the user know we have
   * called the task Yield.*/
  privTrace(traceAPIcallTaskYield);
  /* task Yield does not require any special functionality. so just start the OS, and ask for a
   * task switch. We never return from the call below. */
  privEnterOS(defActionTaskStateSwitch); }

#endif


#if (includeTaskKillTask == cfgTrue)

void privKillTaskBody(Tuint08 uiTaskNumber)
{ /* Called from user code to terminate a task. Note that this task can only be restarted.
   * This is a switching call, let us first initialize the OS. (It is a 'body' function too,
   * see the explanation of this design elsewhere.) */
  privInitOs();
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskKillTask);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* The usual check to see if the task even exists. Note, since uiTaskNumber is unsigned it can only be to large. */
    if ((uiTaskNumber >= defNumberOfTasks) && (uiTaskNumber != defCurrentTaskNumber))
    /* This is a fatal error, repair your code. */
    { privShowError((fatTaskIllegalTaskNumber | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* The task may kill some other task or it may kill itself. In the latter case the termination becomes
   * effective only after a context switch. */
  TtaskControlBlock * taskTCB = privTcbList(uiTaskNumber);
  /* Termination is the clearing of the two most significant bits in the StopState, thereby lowering the
   * status to a level it will never be scheduled again. Note that we cannot recover from this state since
   * there are no instructions which can set these bits again. If a new context is created, the task
   * can be restarted however. At the same time we make sure delays are removed, so this task
   * will not consume resources when looking for blocks or delays, and cannot be 'waked up' */
  taskTCB->uiTaskStatus = defBaseTerminatedTask;
  /* A task may kill itself, but the task number can be 'current task', that must be extracted since it
   * cannot be handled by internal functions. */
  #if (cfgUseSynchronization != cfgSyncNon) || (cfgUseFileSystem == cfgTrue)
    if ((uiTaskNumber & defCurrentTaskMask) == defCurrentTaskNumber) { uiTaskNumber = (uiOsStatus & defTaskNumberGetMask) >> defOsTaskNumberShift; }
  #endif
  /* When we kill a task we must be sure that the slot stack is cleaned and maybe blocking tasks
   * are released.*/
  #if (cfgUseSynchronization != cfgSyncNon)
    /* check if we are terminating a task with a slot stack */
    if ( uiTaskNumber < defNumberOfTasksWithSlot )
    { /* Now we are sure we may call the clean operation */
      privCleanSlotStack((TtaskExtendedControlBlock *) taskTCB);
      /* If we make use of mutexes of queues, maybe some of them need to be released. */
      #if ((defUseMutexes == cfgTrue) || (defUseQueus == cfgTrue))
        privReleaseSyncBlockingTasks();
      #endif
    }
  #endif
  /* The same applies for killing a task that may hold a (b)lock on the filesystem. We simply call
   * close on the task. If it does not hold a lock, it will do nothing. */
  #if (cfgUseFileSystem == cfgTrue)
    privPrepareFileClose(uiTaskNumber);
  #endif
  /* Done, there is no need to force a switch, unless we killed the current task. However, sorting that out
   * cost 12 bytes extra and since this call is not that frequent one extra switch does not really matter.
   * We save the bytes. */
  privEnterOS(defActionTaskStateSwitch); }
#endif


#if (includeTaskRestart == cfgTrue)

void privRestartBody(Tuint08 uiTaskNumber)
{ /* Called from user code to restart a task. This is a switching call, let us first initialize the OS.
   * (It is a 'body' function too, see the explanation of this design elsewhere.) */
  privInitOs();
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskRestart);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* The usual check to see if the task even exists. Note, since uiTaskNumber is unsigned it can only be to large. */
    if ((uiTaskNumber >= defNumberOfTasks) && (uiTaskNumber != defCurrentTaskNumber))
    /* This is a fatal error, repair your code. */
    { privShowError((fatTaskIllegalTaskNumber | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* A task may restart itself */
  if ((uiTaskNumber & defCurrentTaskMask) == defCurrentTaskNumber) { uiTaskNumber = (uiOsStatus & defTaskNumberGetMask) >> defOsTaskNumberShift; }
  /* Now. re-initialize the task Replace the task status register we do want to reactivate a
   * terminated/blocked/delayed task) and reset its priority. Furthermore, if this tasks holds
   * any lock, mutex, queue etc these must be properly released. */
  privTaskInit(uiTaskNumber, (defInitStatusRenew | defInitLockRelease | defInitWatermarksRenew));
  /* Done, activate a new task */
  privEnterOS(defActionTaskStateSwitch); }
#endif


#if (includeGenReboot == cfgTrue)

void genReboot(void)
{ /* Arrive here is you want a (software) reset of the system. After this is initiated we
   * do not want other processes to intervene. Therefore we disable all interrupts. */
  privDisableGlobalInterrupts();
  /* Tell what we are doing */
  privTrace(traceAPIcallGenReboot);
  /* Do your work. We do not expect to return from this call. */
  portReboot(); }

#endif



#if (includeGenAddtoTickCount == cfgTrue)

void genAddtoTickCount(Tuint16 uiSleepTime)
{ /* Call this if you want to shift the tick counter into the future. During this manipulations
   * we do not want to be disturbed. These operations take place in the realm (OS, task, isr)
   * from which we where called. This is important, it cannot be implemented as a switching call.
   * Therefore we must block all interrupts during operation. */
  privEnterSysCritical();
  /* Tell what we are doing */
  privTrace(traceAPIcallGenAddtoTickCount);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
  #endif
  /* We need the HighByte of the tick counter for later calculations. */
  Tuint08 uxTickCountOldHighByte = uxTickCount.HighByte;
  /* Create a new tick counter. The new time is the old time shifted by the given amount. This is
   * a 16 bit addition. Missed calls to appTick00() and appTick08() are lost. We assume that the given
   * uiSleepTime has a resolution on tick level. If this is not the case, the resulting new time
   * may differ from the value that is would have had when no sleeping occurred. */
  Tuint16 uiNewTime = uiSleepTime + uxTickCount.Full;
  /* Extract the new High and Low byte.*/
  uxTickCount.Full  = uiNewTime;
  /* The subbyte is of no importance. In case we come here from a low power sleep, that byte was
   * used to store the maximal sleep time in tick blocks. In any case it is beyond the scope of
   * this routine to accurately calculate a new value. The SubByte may not exist when cfgUseLowPowerSleep
   * is set to false, thus we check. */
  #if (cfgUseLowPowerSleep == cfgTrue)
    uxTickCount.SubByte  = 0;
  #endif
  /* Now we must determine the effects of this time shift. There are a couple of situations
   * (1) Was so short that we do not need to wake any delay tasks, or do we have to?
   * (2) Must we activate the near wake bit in order to signal that some tasks are about
   *     to be waked up?
   * (3) Did we cross the tick counter boundary and must we call appTick16() ?
   * In any case we need the number of tick blocks the system did actually sleep, which
   * may differ from the high byte of the uiSleepTime. */
  Tuint08 uiSleepPeriod = uxTickCount.HighByte - uxTickCountOldHighByte;
  /* First we determine if the tick counter made a full round of 0xFFxx ticks. (more
   * is not possible, or at least cannot be detected here. */
  Tbool bFullRound = ((uiSleepTime & 0x8000)!=0) && (uiSleepPeriod==0);
  /* If so we must wake all tasks, which is certainly not going to happen when
   * uiSleepPeriod equals zero, so turn it into 0xFF. In the situation where
   * uiSleepTime < ca 0x100 the uiSleepPeriod remains zero, which is OK since all
   * tasks will be woken by the near wake bit. */
  if ( bFullRound ) { uiSleepPeriod--; }
  /* See if we had passed the tick boundary. In case of a full round we must have, and
   * otherwise this is easily detected by comparing the new and old high bytes of the
   * tick counters. Of course, the appTick16() comes probably to late. */
  #if (callAppTick16 == cfgTrue)
    if ((uxTickCountOldHighByte>uxTickCount.HighByte) | bFullRound) { appTick16(); }
  #endif
  /* Releasing delay tasks that should be woken during low power sleep, or even searching for
   * such tasks costs quite some code. If you are certain you did not exceed the recommended
   * sleeping time in the portSleep() call, there is no need to do all this work. So you may
   * choose to exclude it from your code. If, by mistake, you have tasks that should have
   * been woken but are not, they remain sleeping until the next round of the tick counter. */
  #if (cfgUseDelay == cfgTrue) && (cfgUseLowPowerDelayRelease == cfgTrue)
    Tuint08  uiLoopTask;
    /* Ok, lets start searching for tasks that should have woken when the system was in low power
     * sleep. We must test each task individually. */
    for (uiLoopTask=0; uiLoopTask<defNumberOfTasks; uiLoopTask++)
    { TtaskControlBlock * loopTCB = privTcbList(uiLoopTask);
      /* Check if we have a task that is delayed. It need not to be running. */
      if ( (loopTCB->uiTaskStatus & defBaseDelayStateGetMask) == defBaseDelayStateDelayed)
      { /* Now we must calculate the delay and see if the activation time was situated
         * in the period the device did sleep. Note the uxDelay.HighByte contains
         * the moment the task should have been woken. If this subtraction should come
         * out negative, then, due to roll over the uiDelayTime gets higher as the
         * uiSleepPeriod, thus no wake up is done. */
        Tuint08 uiDelayTime = uxTickCount.HighByte - loopTCB->uxDelay.HighByte;
        /* If the delay time equals 0 we must investigate further. The first idea
         * would be, nothing should happen. This is because such tasks are to be woken
         * by the privIncrementTick() routine, since they could lie slightly
         * in the future. We activate the near wake bit so they don't escape the check.
         * The only problem with that solution is that it may be the case that the uxTickCount.LowByte
         * equals 0xFF, thus due to a tick increment the whole block would be skipped. The other
         * point of attention is as both uiDelayTime and uiSleepPeriod are zero. That will not
         * be that likely but is possible. In that case we handle according to the being
         * zero of the uiDelayTime and not according to the equality of both. The trick is
         * to lower uiDelayTime to change 0x00 into 0xFF */
        uiDelayTime --;
        /* and skip the equality test. I.e. perform a '<' test rather then '<='. The test never
         * passes for equality in case uiDelayTime did equal zero. And we can test 0xFF separately.
         * In the latter test we see if the Lowbyte of the delay is before the current time low byte. */
        if ( (uiDelayTime < uiSleepPeriod) || ((uiDelayTime == 0xFF) && (uxTickCount.LowByte >= loopTCB->uxDelay.LowByte)) )
        { /* If more time has passed as the task should delay, it is released. This may be a release due to as
           * simple delay or can be a time out on a block. Note that if uiSleepTimeBlocks = 0xFF we get
           * uiSleeptime 0xFF this still results in waking up all tasks, which is needed. Most of them
           * are waked here, a few can lie less than 0xFF in the future and will be waked by privIncrementTask() */
          privWakeupFromDelay(uiLoopTask,loopTCB); } } }
    #endif
  /* We must activate the near wake bit for possible tasks to be waked in the near future. If this
   * turns out not to be the case, it is not a problem. Let us not check this. */
  uiOsStatus = ((uiOsStatus & defNearWakePresentSetMask) | defNearWakeStatePresent);
  /* Reactivate the interrupts and return to the caller. */
  privExitSysCritical(); }

#endif


#if (cfgUseDelay == cfgTrue) && (includeTaskDelayFromNow == cfgTrue )

void privDelayFromNowBody(Tuint16 uiTicksToWait)
{ /* Called from user code to pause a task for a certain time, in this case,
   * measured from the present value of the tick counter. This is a switching call,
   * let us first initialize the OS.
   * (It is a 'body' function too, see the explanation of this design elsewhere.) */
  privInitOs();
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskDelayFromNow);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapDelay) & 0xFF) );
    /* The usual check to see if the waiting time is not to large. The point is, if we make the waiting
     * so large that the wakeup ends in the same block as the current time (due to roll over), it
     * will be woken almost instantly. This must be prohibited.  */
    if (uiTicksToWait > defMaxDelayTime)
    /* This is not a fatal error, the task is terminated. */
    { privShowError((errTaskDelayTooLong | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
  #endif
  /* If the waiting time equals zero, it is effectively a taskYield. Don't call the
   * delay for otherwise it may end up delaying one full tick round. Otherwise
   * we call the privDelayCalc with the request to measure the delay time from now */
  if (uiTicksToWait>0) { privDelayCalc(uiTicksToWait,true); }
  /* We are done and call for a task switch. */
  privEnterOS(defActionTaskStateSwitch);  }

#endif


#if (cfgUseDelay == cfgTrue) && (includeTaskDelayFromWake == cfgTrue )

void privDelayFromWakeBody(Tuint16 uiTicksToWait)
{ /* Called from user code to pause a task for a certain time. This is a switching call, let us
   * first initialize the OS. (It is a 'body' function too, see the explanation of this design
   * elsewhere.) */
  privInitOs();
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskDelayFromWake);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapDelay) & 0xFF) );
    /* The usual check to see if the waiting time is not to large. The point is, if we make the waiting
     * so large that the wakeup ends in the same block as the current time (due to roll over), it
     * will be woken almost instantly. This must be prohibited.  */
    if (uiTicksToWait > defMaxDelayTime)
    /* This is not a fatal error, the task is terminated. */
    { privShowError((errTaskDelayTooLong | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
  #endif
  /* if the waiting time equals zero, it is effectively a taskYield. Don't call the
   * delay for otherwise it may end up delaying one full tick round. Otherwise
   * we call the privDelayCalc with the request to measure the delay time from the
   * last wakeup time. */
  if (uiTicksToWait>0) { privDelayCalc(uiTicksToWait,false); }
  /* We are done and call for a task switch. */
  privEnterOS(defActionTaskStateSwitch);  }

#endif


#if (cfgUseTasknames == cfgTrue) && ((includeGenGetTaskname == cfgTrue) || (includeGenLogTask == cfgTrue))

Taddress genGetTaskname(Tuint08 uiTaskNumber)
{ /* Call this to obtain the (dynamic) task name of the specified task. This task name is
   * stored in flash. We assume that no reprogramming is going on while reading. */
  /* Standard protection */
  privEnterSysCritical();
  /* First, tell what we are doing */
  privTrace(traceAPIcallGenGetTasknameChar);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* The usual check to see if the task even exists. Note, since uiTaskNumber is unsigned it can only be to large. */
    if ((uiTaskNumber >= defNumberOfTasks) && (uiTaskNumber != defCurrentTaskNumber))
    /* This is a fatal error */
    { privShowError((fatTaskIllegalTaskNumber | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* A task may call its own name */
  if ((uiTaskNumber & defCurrentTaskMask) == defCurrentTaskNumber) { uiTaskNumber = (uiOsStatus & defTaskNumberGetMask) >> defOsTaskNumberShift; }
  /* Read the address of the first byte using the flash read macro, and return it. Note that this is
   * an address in flash, thus read the content appropriately. */
  Taddress Result = portFlashReadStruc(TtaskDefinitionBlock,pxTDBlist[uiTaskNumber],Taddress,pTaskName);
  /* We are done. */
  privExitSysCritical();
  return Result; }

#endif


#if (includeGenSuspend == cfgTrue)

void genSuspend(Tuint08 uiTaskNumber)
{ /* Call this if you want to suspend a task. Standard protection  */
  privEnterSysCritical();
  /* Tell what we are doing */
  privTrace(traceAPIcallGenSuspend);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* The usual check to see if the task even exists. Note, since uiTaskNumber is unsigned it can only be to large. */
    if ((uiTaskNumber >= defNumberOfTasks) && (uiTaskNumber != defCurrentTaskNumber))
    /* This is a fatal error */
    { privShowError((fatTaskIllegalTaskNumber | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* Get the task control block of the task at hand. */
  TtaskControlBlock * taskTCB = privTcbList(uiTaskNumber);
  /* First check if the task is running at all. We do not want to suspend a terminated or sleeping
   * task. In other words we can only suspend a running task. Also, we do not allow blocking tasks
   * (any kind of block), for we do not keep information about the block type within the suspended
   * state. And we do not have a mechanism yet which releases the blocks when resumed. */
  if (taskTCB->uiTaskStatus >= defBaseNoBlocksTask)
  /* Change the status information to mark the task suspended. Note that a task can suspend itself,
   * but that will only be effective from the moment a context switch occurs. This is rather
   * unpredictable, better call taskSuspendMe() in that case. */
  { taskTCB->uiTaskStatus = (taskTCB->uiTaskStatus & defBaseSuspendedSetMask) | defBaseSuspendedTask; }
  /* We are done. */
  privExitSysCritical(); }

#endif


#if (includeTaskSuspendMe == cfgTrue)

void privSuspendMeBody(void)
{ /* Call this if you want to suspend yourself, This is a switching call, let us first initialize the OS.
   * (It is a 'body' function too, see the explanation of this design elsewhere.) */
  privInitOs();
  /* Tell what we are doing. */
  privTrace(traceAPIcallTaskSuspendMe);
  /* Get the task control block of the task that called this routine. */
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* We do not have to check whether we are sleeping or terminated because that is not
   * possible right here, the task called it on itself remember? The same holds for the blocking
   * state. Thus just put the task in the suspended state. */
  curTCB->uiTaskStatus = (curTCB->uiTaskStatus & defBaseSuspendedSetMask) | defBaseSuspendedTask;
  /* We are done, go get a task switch */
  privEnterOS(defActionTaskStateSwitch); }

#endif


#if (includeGenResume == cfgTrue)

void genResume(Tuint08 uiTaskNumber)
{ /* Call this to resume a task previously suspended. Standard protection  */
  privEnterSysCritical();
  /* Tell what we are doing. */
  privTrace(traceAPIcallGenResume);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* The usual check to see if the task even exists. Note, since uiTaskNumber is unsigned it can only be to large. */
    if ((uiTaskNumber >= defNumberOfTasks) && (uiTaskNumber != defCurrentTaskNumber))
    /* This is a fatal error */
    { privShowError((fatTaskIllegalTaskNumber | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* Get the task control block of the task at hand. */
  TtaskControlBlock * taskTCB = privTcbList(uiTaskNumber);
  /* First check if the task is suspended at all. We do not want to resume a terminated or sleeping
   * task. In other words we can only resume a suspended task.  */
  if ( (taskTCB->uiTaskStatus & defBaseSuspendedGetMask) == defBaseSuspendedTask )
  { /* Since only a running task (may be delayed though) can be suspended we know
     * in which state we must put it at resuming. This cannot be blocked in whatever
     * way. It may only be delayed. */
    taskTCB->uiTaskStatus = (taskTCB->uiTaskStatus & defBaseNoBlocksSetMask) | defBaseNoBlocksTask;
    /* If we are keeping track of exact wakeup times, this is one of the places that
     * must be done. */
    #if (cfgUseDelay == cfgTrue) && (cfgUseCorrectWakeupTimes == cfgTrue)
      /* Of course we only need to change the wakeup time if we are really waking the task up. If the
       * task is still delayed, resuming does not change its delay status, and the task remains
       * effectively inactive.*/
      if ((taskTCB->uiTaskStatus & defBaseDelayStateGetMask) == defBaseDelayStateDelayed)
      /* The last wakeup time is kept in the uxDelay field. */
      { taskTCB->uxDelay.Full = uxTickCount.Full; }
    #endif
    /* DISCUSSION
     * Resuming a task which was blocked while being suspended may lead to the situation that
     * that task will never be resumed again. We could introduce code that tracks that situation
     * down and calls  privReleaseSyncBlockingTasks(). However, that will at the least costs a lot
     * of stack space, or requires an extra messaging system to the OS. In practice it is very
     * unwise to suspend a task which is blocking or holds queue's or mutexes. Therefore we forbid
     * suspending blocked tasks at the moment, and do not need to worry at resuming.
     */
    }
  /* We are done. */
  privExitSysCritical(); }

#endif


#if (cfgUseLowPowerSleep == cfgTrue) && (includeTaskSleep == cfgTrue)

void privSleepBody(void)
{ /* Call this if you want to put a task to sleep, This is a switching call, let us first initialize the OS.
   * (It is a 'body' function too, see the explanation of this design elsewhere.) */
  privInitOs();
  /* Tell what we are doing. */
  privTrace(traceAPIcallTaskSleep);
  /* Get the task control block of the task that called this routine. */
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* It is an error to call this method if you are writing to the file system (since you
     * cannot call this if you are blocked there is no issue with read/write blocking). You
     * may call it if you have read access, but note that this may block any write operation
     * until the next wakeup.*/
    #if (cfgUseFileSystem == cfgTrue)
      /* This if the task holds a writelock. */
      if ( (curTCB->defFsField & defFsWriteGetMask) == defFsWriteActive)
      /* Report an error and stop the task. */
      { privShowError((errFileOpenMode | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    #endif
  #endif
  /* It is not allowed to call this function when you are holding a write lock.
   * We do not have to check whether we are suspended or terminated because that is not
   * possible right here, the task called it on itself remember? Thus put the task in
   * the sleep state directly. */
  curTCB->uiTaskStatus = (curTCB->uiTaskStatus & defBaseSleepingSetMask) | defBaseSleepingTask;
  /* We are done, go get a task switch */
  privEnterOS(defActionTaskStateSwitch); }

#endif


#if (cfgUseLowPowerSleep == cfgTrue) && (includeTaskSleepAll == cfgTrue)

void privSleepAllBody(void)
{ /* Call this if you want to put all tasks to sleep. Note that all tasks are put to
   * sleep directly this may not be the optimal point in their execution.
   * This is a switching call, let us first initialize the OS.
   * (It is a 'body' function too, see the explanation of this design elsewhere.) */
  privInitOs();
  /* Tell what we are doing. */
  privTrace(traceAPIcallTaskSleepAll);
  /* If a file system is present we must request for sleep. */
  #if (cfgUseFileSystem == cfgTrue)
     /* Indicate to the fs that we want to go to sleep, so it can finish fs operations. */
    uiFsStatus = (uiFsStatus & defFsSleepRequestSetMask) |  defFsSleepRequestActive;
  #endif
  /* Put all tasks to sleep that are egiable. */
  privPutAllTasksToSleep();
 /* We are done., we may have put ourself to sleep, so switch in any case. */
  privEnterOS(defActionTaskStateSwitch); }

#endif


#if (includeTaskProtectSwitchTasks == cfgTrue) || ((includeTaskProtectSwitchCritical == cfgTrue) && (cfgUseNestedCriticals == cfgFalse) )

void taskDisableSwitchTask(void)
{ /* Call this if you want to have explicit control over the system. Tick interrupts keep coming, but the
   * switching of tasks are blocked. Note that your task must explicitly unblock this again, otherwise
   * all tasks get 'stuck' since they are never scheduled again. Standard protection  */
  privEnterSysCritical();
  /* Tell what we are doing. */
  privTrace(traceAPIcallTaskDisableSwitchTask);
  /* Change the block switch bit to enable or disabled depended on the parameter.*/
  uiOsStatus = ((uiOsStatus & defSwitchAllowSetMask) | defSwitchStateBlocked);
  /* Done, such a short routine! */
  privExitSysCritical(); }

void taskEnableSwitchTask(void)
{ /* Call this if you want to have explicit control over the system. Call this after a block of task
   * switching. Note that your task must explicitly unblock again, otherwise
   * all tasks get 'stuck' since they are never scheduled again. Standard protection  */
  privEnterSysCritical();
  /* Tell what we are doing. */
  privTrace(traceAPIcallTaskEnableSwitchTask);
  /* Change the block switch bit to enable or disabled depended on the parameter.*/
  uiOsStatus = ((uiOsStatus & defSwitchAllowSetMask) | defSwitchStateRunning);
  /* Done, such a short routine! */
  privExitSysCritical(); }

#endif


#if (includeGenSetPriority == cfgTrue)

void genSetPriority(Tuint08 uiTaskNumber, Tuint08 uiNewPriority)
{ /* Call this if you want to change the priority of a task.  Standard protection */
  privEnterSysCritical();
  /* Tell what we are doing. */
  privTrace(traceAPIcallGenSetPriority);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* The usual check to see if the task even exists. Note, since uiTaskNumber is unsigned it can only be to large. */
    if ((uiTaskNumber >= defNumberOfTasks) && (uiTaskNumber != defCurrentTaskNumber))
    /* This is a fatal error */
    { privShowError((fatTaskIllegalTaskNumber | errTaskStateCurrent), errCurrentTask ); }
    /* Check to see if the priority is not to high. */
    if (uiNewPriority >= defNumberOfPriorities)
    /* It suffices to stop this task. */
    { privShowError((errTaskPriorityToHigh | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* Get the task control block of the task at hand. */
  TtaskControlBlock * taskTCB = privTcbList(uiTaskNumber);
  /* Set the new priority, the three bits are in the status register */
  taskTCB->uiTaskStatus = (taskTCB->uiTaskStatus & defBasePrioSetMask) | ((uiNewPriority << defBasePrioShift) & defBasePrioGetMask);
  /* We are done */
  privExitSysCritical(); }

#endif


#if (includeGenGetPriority == cfgTrue)

Tuint08 genGetPriority(Tuint08 uiTaskNumber)
{ /* Call this if you want to know the priority of a task. */
  Tuint08 Result;
  /* Don't allow for changes in the background.  Standard protection */
  privEnterSysCritical();
  /* Tell what we are doing. */
  privTrace(traceAPIcallGenGetPriority);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* The usual check to see if the task even exists. Note, since uiTaskNumber is unsigned it can only be to large. */
    if ((uiTaskNumber >= defNumberOfTasks) && (uiTaskNumber != defCurrentTaskNumber))
    /* This is a fatal error */
    { privShowError((fatTaskIllegalTaskNumber | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* Extract the priority, the three bits are in the status register */
  Result = (privTcbList(uiTaskNumber)->uiTaskStatus & defBasePrioGetMask) >> defBasePrioShift ;
  /* We are done */
  privExitSysCritical();
  return Result; }

#endif


#if (includeGenGetTickCount == cfgTrue)

Tuint16 genGetTickCount(void)
{ /* Call this if you want to know the value of the tick counter. */
  Tuint16 Result;
  /* Only inside the OS the tick counter can be changed. Standard protection  */
  privEnterSysCritical();
  /* Tell what we are doing. */
  privTrace(traceAPIcallGenGetTickCount);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
  #endif
  /* Construct the tick counter. You may ask yourself what value you get if you call this from
   * an interrupt isr that came just on the moment the low byte of the tick counter was updated
   * and the high byte was still old. Well, this is not allowed. Or interrupts are allowed during
   * OS execution, but then you may not call genXXX routines, or you may call them, but then
   * you must have protection on. */
  Result = uxTickCount.Full;
  /* Done. */
  privExitSysCritical();
  return Result; }

#endif


#if (defUseDelay == cfgTrue) && (includeGenGetLastWakeTime == cfgTrue)

Tuint16 genGetLastWakeTime(Tuint08 uiTaskNumber)
{ /* Sometimes it may be useful to know what was the last time a particular task was waked.
   * use this method to obtain that information. */
  Tuint16 Result;
  /* Since this time can be influenced by the OS we want to protect it from that.
   * Standard protection */
  privEnterSysCritical();
  /* Tell what we are doing. */
  privTrace(traceAPIcallGenGetLastWakeTime);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* The usual check to see if the task even exists. Note, since uiTaskNumber is unsigned it can only be to large. */
    if ((uiTaskNumber >= defNumberOfTasks) && (uiTaskNumber != defCurrentTaskNumber))
    /* This is a fatal error */
    { privShowError((fatTaskIllegalTaskNumber | errTaskStateCurrent), errCurrentTask ); }
    /* Check the capabilities of the given task. Note that the current task may not need to possess delay capabilities. */
    privCheckCapabilities((uiTaskNumber), ((cfgCapDelay) & 0xFF) );
  #endif
  /* Get the task control block of the task at hand. */
  TtaskControlBlock * taskTCB = privTcbList(uiTaskNumber);
  /* Construct the last wake time */
  Result = taskTCB->uxDelay.Full;
  privExitSysCritical();
  /* Done, return the result, which, because we tick counters are allowed again may already be outdated. */
  return Result; }

#endif


#if (cfgUseTaskWatchdog == cfgTrue) &&  (includeTaskFeedWatchdog == cfgTrue)

void taskFeedWatchdog(void)
{ /* Call this regularly to keep the watchdog silent.  Standard protection */
  privEnterSysCritical();
  /* Tell what we are doing. */
  privTrace(traceAPIcallTaskFeedWatchdog);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the given task. */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapWatchdog) & 0xFF) );
  #endif
  /* What task where we in? */
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* reset the watchdog counter (2 bit) to sleep. */
  curTCB->defDogField = (curTCB->defDogField & defDogSetMask) | defDogSleep ;
  /* Done */
  privExitSysCritical(); }

#endif


#if (cfgUseTaskWatchdog == cfgTrue) &&  (includeTaskKillWatchdog == cfgTrue)

void taskKillWatchdog(void)
{ /* Call this to silence the watchdog definitely. Standard protection */
  privEnterSysCritical();
  /* Tell what we are doing. */
  privTrace(traceAPIcallTaskKillWatchdog);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the given task. */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapWatchdog) & 0xFF) );
  #endif
  /* What task where we in? */
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* reset the watchdog counter (2 bit) to dead (zero). */
  curTCB->defDogField = (curTCB->defDogField & defDogSetMask) | defDogDead ;
  /* Done */
  privExitSysCritical(); }

#endif


#if ((includeGenLogTask == cfgTrue) || (includeGenLogOs == cfgTrue)) && (includeGenPipeInt16 == cfgFalse)
  /* We make this method static is there is no external need, so it can be inlined. */
  static
#endif
/* take care the definition below is coupled to the static above */
#if ((includeGenLogTask == cfgTrue) || (includeGenLogOs == cfgTrue)) || (includeGenPipeInt16 == cfgTrue)
void genPipeInt16(Tuint16 uiValue, void (*pipe)(Tchar))
{ /* Easy method to push a 16 bit number through a pipe. Big endian, most significant byte first. */
  pipe((Tuint08)(uiValue>>8));
  /* The second byte is found through truncation. */
  pipe((Tuint08)uiValue); }
#endif

#if ((includeGenLogTask == cfgTrue) || (includeGenLogOs == cfgTrue)) && (includeGenPassFlashString == cfgFalse)
  /* We make this method static is there is no external need, so it can be inlined. */
  static
#endif
/* take care the definition below is coupled to the static above */
#if ((includeGenLogTask == cfgTrue) || (includeGenLogOs == cfgTrue)) || (includeGenPassFlashString == cfgTrue)
void genPassFlashString(Taddress pString, Tuint08 uiLength, Tchar cFilling, void (*pipe)(Tchar))
{ /* Easy method to push a string in flash trough a pipe. This method can be extremely stack hungry.
   * There are several option for use. If we specify uiLength==0 the whole string will be pushed until
   * (but not including) the null character. If we specify a positive length, that will be the precise number
   * of characters passed.
   * Tell what we are doing */
  privTrace(traceAPIcallGenPassFlashString);
  /* This counter counts characters pushed */
  Tuint08 uiCharCount = 0;
  /* We must remember if the number specified was zero
   * TODO This can be solved much more efficiently, */
  Tbool bFixedLength = (uiLength != 0);
  Tchar cChar = (Tchar) (Tuint08) (Tuint16) pString;
  /* Sending 0 means sending all (max 255 characters)*/
  uiLength--;
  for (uiCharCount = 0; uiCharCount<=uiLength; uiCharCount++)
  { if (cChar != 0) { cChar = portFlashReadByte(Tchar, pString[uiCharCount]); }
    if (cChar != 0) { pipe(cChar); } else if (bFixedLength) { pipe(cFilling); } else break; } }
#endif

#if (includeGenLogOs == cfgTrue)

void genLogOs(void (*pipe)(Tchar))
{ /* Push the information of the OS through the pipe. This is the structure of information to be expected:
   * [Id(1)] [StatusOs(1)] [StatusFs(1)]  [LoadOs(2)] [LoadIdle(2)] [LoadIsr(2)] [StackOsMax(1)] [StackIsrMax(2)]
   * The number of bytes is constant: 12, if some information is not available, null is send. */
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
  #endif
   /* Tell what we are doing */
  privTrace(traceAPIcallGenLogOs);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
  #endif
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* Tell we are ready to send the OS information*/
  pipe(defResponseLogOs);
  /* The Status information: [][][][] [current task(4)]*/
  pipe(uiOsStatus);
  #if (cfgUseFileSystem == cfgTrue)
    pipe(uiFsStatus);
  #else
    pipe(0);
  #endif
  /* The load of the OS, i.e. the number of subticks passed in the OS realm, if this information
   * is not available, send zero. */
  #if (cfgUseLoadMonitor == cfgTrue)
    genPipeInt16(uiOsLoadTotal,pipe);
  #else
    genPipeInt16(0,pipe);
  #endif
  /* The load of the Idle task, i.e. the number of subticks passed as idle, if this information
   * is not available, send zero. */
  #if (cfgUseLoadMonitor == cfgTrue)
    genPipeInt16(uiIdleLoadTotal,pipe);
  #else
    genPipeInt16(0,pipe);
  #endif
  /* The load of the ISR, i.e. the number of subticks passed in the isr, if this information
   * is not available, send zero. */
  #if (cfgUseLoadMonitor == cfgTrue) && (cfgIntUserDefined == cfgTrue)
    genPipeInt16(uiIsrLoadTotal,pipe);
  #else
    genPipeInt16(0,pipe);
  #endif
  /* If we are keeping watermarks, we pass the OS stack level */
  #if (cfgCheckWatermarks == cfgTrue)
    pipe(uiOsStackMax);
  #else
    pipe(0);
  #endif
  /* and possibly the amount of stack used by the isr. */
  #if (cfgCheckWatermarks == cfgTrue) && (defUseIsrStack == cfgTrue)
    genPipeInt16(uiIsrStackMax,pipe);
  #else
    genPipeInt16(0,pipe);
  #endif
  /* Done*/
  }
#endif


#if (includeGenLogTask == cfgTrue)

void genLogTask(Tuint08 uiTaskNumber, void (*pipe)(Tchar))
{ /* Push the information of one of the tasks through the pipe. This is the structure of information to be expected:
   * [Id(1)] [TaskNr(1)] [Status(1)] [LoadOs/0(2)] [Name(8)] [TaskMonitor(1)] [TaskLevels(2)] [SlotStack[4]] [StackMax(2)] [RegUse(1)]
   * The number of bytes is constant: 23, if some information is not available, null is send. */
   /* Tell what we are doing */
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
  #endif
  privTrace(traceAPIcallGenLogTask);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* The usual check to see if the task even exists. Note, since uiTaskNumber is unsigned it can only be to large. */
    if ((uiTaskNumber >= defNumberOfTasks) && (uiTaskNumber != defCurrentTaskNumber))
    { privShowError((fatTaskIllegalTaskNumber | errTaskStateCurrent), errCurrentTask ); }
  #endif
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* A task may log itself */
  if ((uiTaskNumber & defCurrentTaskMask) == defCurrentTaskNumber) { uiTaskNumber = (uiOsStatus & defTaskNumberGetMask) >> defOsTaskNumberShift; }
  /* Get the tcb */
  TtaskExtendedControlBlock * taskTCB = (TtaskExtendedControlBlock *) privTcbList(uiTaskNumber);
  Tuint08 uiTaskStatus = taskTCB->uiTaskStatus;
  /* Tell we are ready to send the task information*/
  pipe(defResponseLogTask);
  /* Start to send the task number */
  pipe(uiTaskNumber);
  /* Send the status, this contains the run status, and the priority */
  pipe(uiTaskStatus);
  /* If we collected the load information, pass it on */
  #if (cfgUseLoadMonitor == cfgTrue)
    genPipeInt16(taskTCB->uiLoadTotal,pipe);
  #else
    genPipeInt16(0,pipe);
  #endif
  /* If the dynamical task names are includes, send them. */
  #if (cfgUseTasknames == cfgTrue)
    genPassFlashString(genGetTaskname(uiTaskNumber),8,' ',pipe);
  #else
    genPassFlashString(0,8,' ',pipe);
  #endif
  /* Send this to report the critical nesting and the watchdog state */
  #if (defUseTaskMonitor == cfgTrue)
    pipe(taskTCB->uiTaskMonitor);
  #else
    pipe(0);
  #endif
  /* Send the depth of the global, switch and tick critical levels, and their format (only the values
   * for Glob and Tick width need to be transmitted, the Switch is the rest (even if it is not used) */
  #if (defUseTaskLevels == cfgTrue)
    pipe((defNumTickWidth << 4) | (defNumGlobWidth) );
    pipe(taskTCB->uiTaskLevels);
  #else
    genPipeInt16(0,pipe);
  #endif
  /* below we send which slots are used. We only send the first six slots, even if we
   * should make use of more. */
  #if (defUseTaskSlot == cfgTrue)
    Tuint08 uiSlotSize;
    /* Only tasks below the defNumberOfTasksWithSlot threshold can have slots */
    if (uiTaskNumber>=defNumberOfTasksWithSlot)
    { /* For tasks that cannot have slots we send zero. */
      uiSlotSize=0; }
    else
    { /* If the task has slots, we must deduce how many. */
      #if (defSlotOneOnly == cfgTrue)
        /* With one slot we have, well one slot */
        uiSlotSize = 1;
      #elif (defSlotDepthConstant == cfgTrue)
        /* if the number of slots is constant, we use that  */
        uiSlotSize = 2*defSlotDepthCollect;
      #else
       /* otherwise we use the value in flash */
        uiSlotSize = 2*portFlashReadStruc(TtaskDefinitionBlock,pxTDBlist[uiTaskNumber & defTaskNumberGetMask],Tuint08,uiSlotDepth);
      #endif
    }
    /* Send the value */
    pipe(uiSlotSize);
    /* Now send the actual slots. Now if uiSlotSize equals zero, we may not read the memory value of
     * taskTCB->uiTaskSlot since it does not exist, we can however calculate its address. */
    Tuint08 * pTaskSlot = (Tuint08 *) &taskTCB->uiTaskSlot;
    /* Send the existing values, send null if it does not exist. */
    if (uiSlotSize>0) { pipe(*pTaskSlot++); } else { pipe(0); }
    if (uiSlotSize>2) { pipe(*pTaskSlot++); } else { pipe(0); }
    if (uiSlotSize>4) { pipe(*pTaskSlot++); } else { pipe(0); }
  #else
    /* We must send zero's to compensate when no slots are used. */
    genPipeInt16(0,pipe);
    genPipeInt16(0,pipe);
  #endif

  /* If we collect watermarks send them trough. */
  #if (cfgCheckWatermarks == cfgTrue)
    genPipeInt16(taskTCB->uiStackMax,pipe);
    pipe(taskTCB->uiRegisterUse);
  #else
    genPipeInt16(0,pipe);
    pipe(0);
  #endif
  }

#endif



#if ((cfgUseNestedCriticals == cfgTrue) && (includeTaskProtectGlobalCritical == cfgTrue)) || \
    ((cfgIntGlobalOnly == cfgTrue) && (cfgIntTickTrack == cfgTrue) && (cfgUseNestedCriticals == cfgTrue) && (includeTaskProtectTickCritical == cfgTrue))


void taskEnterGlobalCritical(void)
{ /* If you want to make use level managed interrupt control, use this method. It keeps track
   * of the number of interrupts levels up to 4 or 16 levels per task. First we disable the
   * interrupt, if the are already disabled, this does not matter. */
  privDisableGlobalInterrupts();
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskEnterGlobalCritical);
  /* Get the taskcontrolblock of the task that called this method. */
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapCritical) & 0xFF) );
    /* The number of levels is limited, check if we did not reach the maximum. */
    if ((curTCB->defGlobField & defGlobGetMask) == defGlobMax)
    /* If so, report an error. */
    { privShowError((errTaskNestingOverflowed | errTaskStateCurrent), errCurrentTask ); }
    else
    /* if not we may safely increase the level number here */
    { curTCB->defGlobField += defGlobInc; }
  #else
    /* At this place we assume that all is save and we increase the level just like that. */
    curTCB->defGlobField += defGlobInc;
  #endif
  }

#endif


#if (cfgUseNestedCriticals == cfgTrue) && (includeTaskProtectGlobalCritical == cfgTrue) || \
    ((cfgIntGlobalOnly == cfgTrue) && (cfgIntTickTrack == cfgTrue) && (cfgUseNestedCriticals == cfgTrue) && (includeTaskProtectTickCritical == cfgTrue))

void taskExitGlobalCritical(void)
{ /* If you want to make use level managed interrupt control, use this method. It keeps track
   * of the number of interrupts levels up to 4 or 16 levels per task. First tell what we are doing */
  privTrace(traceAPIcallTaskExitGlobalCritical);
  /* Get the taskcontrolblock of the task that called this method. */
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapCritical) & 0xFF) );
    /* The number of levels is limited, check if we did not reach the minimum. */
    if ((curTCB->defGlobField & defGlobGetMask) == defGlobMin)
    /* If so, report an error. */
    { privShowError((errTaskNestingUnderflowed | errTaskStateCurrent), errCurrentTask ); }
    else
    /* if not we may safely decrease the level number here */
    { curTCB->defGlobField -= defGlobInc; }
  #else
    /* At this place we assume that all is save and we decrease the level just like that. */
     curTCB->defGlobField -= defGlobInc;
  #endif
  /* Now check if we reached the lowest level. If so, we must re-enable the interrupts. */
  if ((curTCB->defGlobField & defGlobGetMask) == defGlobMin)
  { privEnableGlobalInterrupts(); } }

#endif


#if (cfgUseNestedCriticals == cfgTrue) && (includeTaskProtectSwitchCritical == cfgTrue)

void taskEnterSwitchCritical(void)
{ /* If you want to make use level managed interrupt control, use this method. It keeps track
   * of the number of interrupts levels up to 4 or 16 levels per task. First we disable the
   * interrupt, if the are already disabled, this does not matter. Standard protection  */
  privEnterSysCritical();
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskEnterSwitchCritical);
  /* Change the block switch bit to disabled */
  uiOsStatus = ((uiOsStatus & defSwitchAllowSetMask) | defSwitchStateBlocked );
  /* Get the taskcontrolblock of the task that called this method. */
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapCritical) & 0xFF) );
    /* The number of levels is limited, check if we did not reach the maximum. */
    if ((curTCB->defSwitchField & defSwitchGetMask) == defSwitchMax)
    /* If so, report an error. */
    { privShowError((errTaskNestingOverflowed | errTaskStateCurrent), errCurrentTask ); }
    else
    /* if not we may safely increase the level number here */
    { curTCB->defSwitchField += defSwitchInc; }
  #else
    /* At this place we assume that all is save and we increase the level just like that. */
    curTCB->defSwitchField += defSwitchInc;
  #endif
  /* Done */
  privExitSysCritical(); }

#endif


#if (cfgUseNestedCriticals == cfgTrue) && (includeTaskProtectSwitchCritical == cfgTrue)

void taskExitSwitchCritical(void)
{ /* If you want to make use level managed interrupt control, use this method. It keeps track
   * of the number of interrupts levels up to 4 or 16 levels per task. Standard protection  */
  privEnterSysCritical();
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskExitSwitchCritical);
  /* Get the taskcontrolblock of the task that called this method. */
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapCritical) & 0xFF) );
    /* The number of levels is limited, check if we did not reach the minimum. */
    if ((curTCB->defSwitchField & defSwitchGetMask) == defSwitchMin)
    /* If so, report an error. */
    { privShowError((errTaskNestingUnderflowed | errTaskStateCurrent), errCurrentTask ); }
    else
    /* if not we may safely decrease the level number here */
    { curTCB->defSwitchField -= defSwitchInc; }
  #else
    /* At this place we assume that all is save and we decrease the level just like that. */
    curTCB->defSwitchField -= defSwitchInc;
  #endif
  /* Now check if we reached the lowest level. If so, we must re-enable the switches. */
  if ((curTCB->defSwitchField & defSwitchGetMask) == defSwitchMin)
  { /* Change the block switch bit to disabled */
    uiOsStatus = ((uiOsStatus & defSwitchAllowSetMask) | defSwitchStateRunning ); }
  privExitSysCritical(); }

#endif


#if (cfgIntGlobalOnly == cfgFalse) && (cfgIntTickTrack == cfgTrue) && (cfgUseNestedCriticals == cfgTrue) && (includeTaskProtectTickCritical == cfgTrue)

void taskEnterTickCritical(void)
{ /* If you want to make use level managed interrupt control, use this method. It keeps track
   * of the number of interrupts levels up to a maximum per task. First we disable the
   * interrupt, if the are already disabled, this does not matter. Note we do not need
   * further protection, since when we have disabled Tick interrupts, we can only anticipate
   * interrupts which may not invoke a context switch. They do no harm.*/
  privDisableTickInterrupts();
  /* Tell what we are doing */
  privTrace(eventAPIcallTaskEnterTickCritical);
  /* Get the taskcontrolblock of the task that called this method. */
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapCritical) & 0xFF) );
    /* The number of levels is limited, check if we did not reach the maximum. */
    if ((curTCB->defTickField & defTickGetMask) == defTickMax)
    /* If so, report an error. */
    { privShowError((errTaskNestingOverflowed | errTaskStateCurrent), errCurrentTask ); }
    else
    /* if not we may safely increase the level number here */
    { curTCB->defTickField += defTickInc; }
  #else
    /* At this place we assume that all is save and we increase the level just like that. */
    curTCB->defTickField += defTickInc;
  #endif
  }

#endif


#if (cfgIntGlobalOnly == cfgFalse) && (cfgIntTickTrack == cfgTrue)  && (cfgUseNestedCriticals == cfgTrue) && (includeTaskProtectTickCritical == cfgTrue)

void taskExitTickCritical(void)
{/* If you want to make use level managed interrupt control, use this method. It keeps track
   * of the number of interrupts levels up to a maximum per task. First we disable the
   * interrupt, if the are already disabled, this does not matter. Note we do not need
   * further protection, since when we have disabled Tick interrupts before we enter, and
   * we and can only anticipate interrupts which may not invoke a context switch. They do no harm. */
  privTrace(eventAPIcallTaskExitTickCritical);
  /* Get the taskcontrolblock of the task that called this method. */
  { TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* Check if the user uses this method properly. */
    #if (cfgCheckMethodUse == cfgTrue)
      /* Check the capabilities of the current task */
      privCheckCapabilities((defCurrentTaskNumber), ((cfgCapCritical) & 0xFF) );
      /* The number of levels is limited, check if we did not reach the minimum. */
      if ((curTCB->defTickField & defTickGetMask) == defTickMin)
      /* If so, report an error. */
      { privShowError((errTaskNestingUnderflowed | errTaskStateCurrent), errCurrentTask ); }
      else
      /* if not we may safely decrease the level number here */
      { curTCB->defTickField -= defTickInc; }
    #else
      /* At this place we assume that all is save and we decrease the level just like that. */
      curTCB->defTickField -= defTickInc;
    #endif
  /* Now check if we reached the lowest level. If so, we must re-enable the interrupts. */
    if ((curTCB->defTickField & defTickGetMask) == defTickMin)
    { privEnableTickInterrupts();  } } }

#endif


#if (cfgCheckTrace == cfgTrue) && (includeGenTrace == cfgTrue)

void genTraceInfo(Tbyte bUser)
{ /* Use this function if you as user want to send a byte trough the trace utility. Do not
   * use portTrace directly, since most codes are already occupied. First we enter the
   * critical section because we do not want that an other task gets in between and sends
   * an other byte.
   *  The user byte consists of two bytes of information. */
  privEnterSysCritical();
  /* Tell that a user byte is coming. */
  privTrace(traceUserByteMessage);
  /* Send the user byte */
  privTrace(bUser);
  /* done */
  privExitSysCritical(); }

#endif


#if (cfgCheckTrace == cfgTrue) && (includeGenTrace == cfgTrue)

void genTraceMarker(void)
{ /* Use this function to send a marker. Markers can be used to quickly find some
   * part of the trace information.
   * at all.*/
  privEnterSysCritical();
  /* Send the marker */
  privTrace(traceMarker);
  /* done */
  privExitSysCritical(); }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && (includeTaskWaitForTasks == cfgTrue)

/* Depending on the fact if we want to make use of the timeout on locking, we introduce
 * the extra parameter uiTicksToWait. */
#if (cfgUseTimeout == cfgTrue)
  void privWaitForTasksBody(Tuint08 uiSlot, Tuint08 uiNumberOfTasks, Tuint16 uiTicksToWait)
#else
  void privWaitForTasksBody(Tuint08 uiSlot, Tuint08 uiNumberOfTasks)
#endif
{ /* Use this method to synchronize more than two tasks. This is a switching call,
   * let us first initialize the OS. (It is a 'body' function too, see the explanation
   * of this design elsewhere.) */
  privInitOs();
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskWaitForTasks);
  /* We need the current task number */
  Tuint08 uiCurrentTask = (uiOsStatus & defTaskNumberGetMask) >> defOsTaskNumberShift;
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapSynchronization) & 0xFF) );
    /* Check if the current task has the right to make use of a slot. (Check remeains needed even if we check
     * capabilities, since we may have capabilities but no slotstack defined.) */
    if (uiCurrentTask >= defNumberOfTasksWithSlot)
    /* If not report the error. */
    { privShowError((errTaskHoldsNoSlots | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    /* The zero slot is reserved to indicate there is no lock, thus it cannot be actively used. */
    if (uiSlot == defSlotFree)
    /* Report an error and stop the task. */
    { privShowError((errSlotZeroUsed | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    /* Double slots are not allowed */
    if ((uiSlot & defSlotLeftGetMask) != defSlotLeftFree)
    /* Report an error and stop the task. */
    { privShowError((errSlotDoubleUsed | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    /* Check the type of the slot which is coupled to its number. */
    if ((uiSlot < defNumberWaitBegin) || (uiSlot >= defNumberWaitEnd))
    /* If so report an error. */
    { privShowError((errSlotTypeMismatch | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    /* The timeout is implemented as a delay, thus may not exceed the maximum delay time.*/
    #if (cfgUseTimeout == cfgTrue)
      /* Check the capabilities of the current task */
      privCheckCapabilities((defCurrentTaskNumber), ((cfgCapTimeout) & 0xFF) );
      /* Check if we do not violate the max delay time */
      if (uiTicksToWait > defMaxDelayTime)
      /* Report an error and stop the task. */
      { privShowError((errTaskDelayTooLong | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    #endif
  #endif
  /* First look if the requested number of tasks is already reached. Note that the system does not remember this
   * number, you have to supply it on every call, and that should be constant. */
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  Tuint08 uiLoopTask;
  /* Set the counter that will count the number of blocked task to one, representing the current task */
  Tuint08 uiBlockCount = 1;
  /* Now loop through all tasks. */
  for (uiLoopTask=0; uiLoopTask<defNumberOfTasksWithSlot; uiLoopTask++)
  { TtaskExtendedControlBlock * loopTCB = (TtaskExtendedControlBlock *) privTcbList(uiLoopTask);
    /* If the task contains the slot, it must be blocking (we do not need to check that), and
     * since this slot can never be used in a double block, we do not need to check that. But
     * if we have cfgSyncSingleBlock the left may be filled with some slot on which this task
     * has a free lock. That must be eliminated, the other cases cfgSyncSingleSlot and cfgSyncDoubleBlock
     * pose no problems.  */
    #if (cfgUseSynchronization == cfgSyncSingleBlock)
      if ((loopTCB->uiTaskSlot & defSlotRightGetMask) == uiSlot) { uiBlockCount++; }
    #else
      if (loopTCB->uiTaskSlot == uiSlot) { uiBlockCount++; }
    #endif
  }
  /* Now test if enough tasks are blocking */
  if (uiBlockCount >= uiNumberOfTasks)
  { /* We are about to release, which is equivalent to an acquired lock, report so */
    privTrace(traceLockAcquire);
    /* Yes, there are so release the other tasks */
    privReleaseWait(uiSlot);
    /* Also, we have succeeded, so we return true, if this facility has been compiled in. */
    #if (defUseBoolReturns == cfgTrue)
      curTCB->defRetField = (curTCB->defRetField & defRetSetMask ) | defRetTrue;
    #endif
    /* We are done, so enter the OS to start a new task. */
    privEnterOS(defActionTaskStateKeep); }
  else
  { /* If the lock was not occupied, or there were not enough tasks, we must add ourselves to the
     * waiting list. Say so to the outside world. */
    privTrace(traceLockReject);
    /* Make room for a new lock.*/
    privOperateSlotStack((uiCurrentTask| defSlotOperateIncreaseAction), uiSlot);
    /* First handle the timeout so the compiler may dump this variable. */
    #if (cfgUseTimeout == cfgTrue)
      /* If we make use of timeouts, set the delay variable, which is used for the timeout. */
      if (uiTicksToWait != defLockBlockInfinite) { privDelayCalc(uiTicksToWait,true); }
    #endif
    /* Lock the task */
    curTCB->uiTaskStatus = (curTCB->uiTaskStatus & (defBaseModeSetMask & defBaseBlockStateSetMask & defBaseDressSetMask)) | (defBaseModeSync | defBaseBlockStateBlocked | defBaseDressSlot);
    /* At this point it make no sense to perform priority lifting. This is because this is
     * task cannot hold an other task hostage with a low priority. */
    privEnterOS(defActionTaskStateSwitch); } }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && (includeGenWaitRelease == cfgTrue)

void genWaitRelease(Tuint08 uiSlot)
{ /* Use this method to release all blocks on a particular locks, but only those placed
   * by WaitForTasks of WaitForOtherTasks. The call is non switching (needed so we can call it from
   * an isr), so we must protect ourselves.*/
  privEnterSysCritical();
  /* Tell what we are doing */
  privTrace(traceAPIcallGenWaitRelease);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* The zero slot is reserved to indicate there is no lock, thus it cannot be actively used. */
    if (uiSlot == defSlotFree)
    /* Report an error and stop the task. */
    { privShowError((errSlotZeroUsed | errTaskStateCurrent), errCurrentTask ); }
    /* Check the type of the slot which is coupled to its number. */
    if ((uiSlot < defNumberWaitBegin) || (uiSlot >= defNumberWaitEnd))
    /* If so report an error. */
    { privShowError((errSlotTypeMismatch | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* Release everything */
  privReleaseWait(uiSlot);
  /* Done, exit the critical zone. */
  privExitSysCritical(); }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && ( (includeTaskQueu == cfgTrue) || (includeTaskMutex == cfgTrue) )

/* Depending on the fact if we want to make use of the timeout on locking, we introduce
 * the extra parameter uiTicksToWait.  */
#if (cfgUseTimeout == cfgTrue)
  #if (cfgUseSynchronization == cfgSyncDoubleBlock)
    void privSyncRequestBody(Tuint08 uiSlotSlot, Tsint08 siFreeLeftFilling, Tsint08 siFreeRightFilling, Tuint16 uiTicksToWait)
  #else
    void privSyncRequestBody(Tuint08 uiSlotSlot, Tsint08 siFreeRightFilling, Tuint16 uiTicksToWait)
  #endif
#else
  #if (cfgUseSynchronization == cfgSyncDoubleBlock)
    void privSyncRequestBody(Tuint08 uiSlotSlot, Tsint08 siFreeLeftFilling, Tsint08 siFreeRightFilling)
  #else
    void privSyncRequestBody(Tuint08 uiSlotSlot, Tsint08 siFreeRightFilling)
  #endif
#endif
{ /* Use this method to test how many bytes can be read from/ written to a queue and lock the queue
   * for that purpose,  siFreeFilling>0: readable, siFreeFilling<0: writable bytes,
   * siFreeFilling = 0 no check on size.  This is a switching call, let us first initialize
   * the OS. (It is a 'body' function too, see the explanation of this design elsewhere.) */
  privInitOs();
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskSyncRequest);
  /* Extract the left and right slots. */
  Tuint08 uiRightSlot = (uiSlotSlot & defSlotRightGetMask) >> defSlotRightShift;
  /*We only need the left slot in special situations. */
  #if (cfgUseSynchronization == cfgSyncDoubleBlock)
    Tuint08 uiLeftSlot  = (uiSlotSlot & defSlotLeftGetMask) >> defSlotLeftShift;
  #endif
  /* We need the current task number */
  Tuint08 uiCurrentTask = (uiOsStatus & defTaskNumberGetMask) >> defOsTaskNumberShift;
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapSynchronization) & 0xFF) );
    /* Check if the current task has the right to make use of a slot. (Check remeains needed even if we check
     * capabilities, since we may have capabilities but no slotstack defined.) */
    if (uiCurrentTask >= defNumberOfTasksWithSlot)
    /* If not report the error. */
    { privShowError((errTaskHoldsNoSlots | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    /* Check if the preprocessor already saw that the slot is of the wrong type. Note that we 'misused'
     * the value 0xF0 to pass this information. Formally that number could also have been passed directly
     * but that is an error two btw, but of a different kind. Here we assume it is not. The OnName macros
     * can never pass a slot number zero, so it may only collide with a manual call on taskSyncRequest with
     * 0xF0 as slot parameter. */
    if (uiSlotSlot == defCheckSlotTypeError)
    { privShowError((errSlotTypeMismatch | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    /* The zero slot is reserved to indicate there is no lock, thus it cannot be actively used. */
    if (uiRightSlot == defSlotRightFree)
    /* Report an error and stop the task. */
    { privShowError((errSlotZeroUsed | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    /* Check the type of the slot which is coupled to its number, and if mutexes do not have Fillings */
    if ( ( (uiRightSlot != defSlotRightFree) && ((uiRightSlot < defNumberQueuBegin) || (uiRightSlot >= defNumberMutexEnd ))) ||
         ( (siFreeRightFilling != 0) && ((uiRightSlot >= defNumberMutexBegin) || (uiRightSlot < defNumberMutexEnd ))) )
      /* If so report an error. */
    { privShowError((errSlotTypeMismatch | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    #if (cfgUseSynchronization == cfgSyncDoubleBlock)
      if ( ( (uiLeftSlot  != defSlotLeftFree)  && ((uiLeftSlot  < defNumberQueuBegin) || (uiLeftSlot  >= defNumberMutexEnd ))) ||
           ( (siFreeLeftFilling  != 0) && ((uiLeftSlot  >= defNumberMutexBegin) || (uiLeftSlot  < defNumberMutexEnd ))) )
        /* If so report an error. */
      { privShowError((errSlotTypeMismatch | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
      /* Check if not two identical slots are locked */
      if (uiRightSlot == uiLeftSlot)
      /* If so report an error. */
      { privShowError((errSlotTwins | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    #else
      /* Double slots are not allowed */
      if ((uiSlotSlot & defSlotLeftGetMask) != defSlotLeftFree)
      /* Report an error and stop the task. */
      { privShowError((errSlotDoubleUsed | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    #endif
    #if (defUseQueus == cfgTrue)
      /* It is an error to read more bytes from, or write to the queue than the queue contains, it would
       * block indefinitely or always return false. */
      Tuint08 uiRightSize = privGetQueuSize(uiRightSlot-1);
      /* we want read bytes, see if we do not read to many */
      if ((siFreeRightFilling>0) && (((Tuint08) siFreeRightFilling)>uiRightSize) )
      { privShowError((errQueuReadLimit | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
      /* we want write bytes, see if we do not write to many */
      if ((siFreeRightFilling<0) && (((Tuint08) -siFreeRightFilling)>uiRightSize) )
      { privShowError((errQueuWriteLimit | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
      /* The timeout is implemented as a delay, thus may not exceed the maximum delay time.*/
      #if (cfgUseSynchronization == cfgSyncDoubleBlock)
        Tuint08 uiLeftSize = privGetQueuSize(uiLeftSlot-1);
        if ((siFreeLeftFilling>0) && (((Tuint08) siFreeLeftFilling)>uiLeftSize) )
        { privShowError((errQueuReadLimit | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
        /* we want write bytes, see if we do not write to many */
        if ((siFreeLeftFilling<0) && (((Tuint08) -siFreeLeftFilling)>uiLeftSize) )
        { privShowError((errQueuWriteLimit | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
      #endif
    #endif
    #if (cfgUseTimeout == cfgTrue)
      /* Check the capabilities of the current task */
      privCheckCapabilities((defCurrentTaskNumber), ((cfgCapTimeout) & 0xFF) );
      /* Check if we do not violate the max delay time */
      if (uiTicksToWait > defMaxDelayTime)
      /* Report an error and stop the task. */
      { privShowError((errTaskDelayTooLong | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    #endif
  #endif
  /* Arriving at this point, we assume we are going to try to lock a queue. If we get access to a queue
   * depends on two questions. If the queue is not already locked, and if the number of bytes (to read
   * or write) fit. Get a pointer to the task control block for operations. */
  TtaskExtendedControlBlock * curTCB = (TtaskExtendedControlBlock *) privTcbList(defCurrentTaskNumber);
  /* When we arive here there are we may already own the slot, or own other slots. Thats fine. The only thing we
   * must be sure is that no other task owns it which is not blocking. That other task may not posses the slot
   * and block on some other slot to, since that implies free possesion of the queue slot when the other
   * block is released. This is all tested in privFreeLockAbsent*/
  Tbool bFreeLockAbsent;
  #if (cfgUseSynchronization == cfgSyncDoubleBlock)
    bFreeLockAbsent = privFreeLockAbsent(uiLeftSlot) && privFreeLockAbsent(uiRightSlot);
  #else
    bFreeLockAbsent = privFreeLockAbsent(uiRightSlot);
  #endif
  /* Now let us register the use of the slots, which is needed anyway, wheter we must block or can
   * continue. This must be done after the check for freeLock.*/
  privOperateSlotStack((uiCurrentTask| defSlotOperateIncreaseAction), uiSlotSlot);
  /* If we have a mutex, no further testing is needed (privSizeFitsQueu returns true for mutexes) and
   * if there are no queues present we can leave the test out all together. */
  #if (defUseQueus == cfgTrue)
    #if (cfgUseSynchronization == cfgSyncDoubleBlock)
      if ( bFreeLockAbsent && privSizeFitsQueu(uiLeftSlot,siFreeLeftFilling) &&  privSizeFitsQueu(uiRightSlot,siFreeRightFilling) )
    #else
      if ( bFreeLockAbsent && privSizeFitsQueu(uiRightSlot,siFreeRightFilling) )
    #endif
  #else
    if (bFreeLockAbsent)
  #endif
  { /* OK, we have finally found out that we may take possession of the queue, let the outside world know. */
    privTrace(traceLockAcquire);
    /* the slots where already registered, so there is not a lot left to do, since
     * obtaining the lock on the slot was a success. */
    #if (defUseBoolReturns == cfgTrue)
      /* return true */
      curTCB->defRetField = ( curTCB->defRetField & defRetSetMask ) | defRetTrue;
    #endif
    /* Done, ask the OS if we may continue our business. */
    privEnterOS(defActionTaskStateKeep); }
  else
  { /* OK, unfortunately we must wait until some other task has run and changed the conditions
     * so that we can run. report so to the ouside world. */
    privTrace(traceLockReject);
    /* First handle the timeout so the compiler may dump this variable. */
    #if (cfgUseTimeout == cfgTrue)
      /* If we make use of timeouts, set the delay variable, which is used for the timeout. */
      if (uiTicksToWait != defLockBlockInfinite) { privDelayCalc(uiTicksToWait,true); }
    #endif
    /* thus block the task */
    curTCB->uiTaskStatus = (curTCB->uiTaskStatus & (defBaseModeSetMask & defBaseBlockStateSetMask & defBaseDressSetMask)) | (defBaseModeSync | defBaseBlockStateBlocked | defBaseDressSlot);
    /* Of course, we must store somewhere what the release condition for this particular block is.
     * This is the  siFreeFilling parameter, which says it all. Store it in the siQueuLock field. */
    #if (defUseQueus == cfgTrue)
      curTCB->siQueuRightLock = siFreeRightFilling;
      #if (cfgUseSynchronization == cfgSyncDoubleBlock)
        curTCB->siQueuLeftLock = siFreeLeftFilling;
      #endif
    #endif
    /* At this point we must priority lift. The current task could be a very important task, but
     * is now blocked, maybe waiting for some low prio task for release. To make sure we do not
     * deadlock let us raise the priorities of all tasks on this lock to the highest available
     * level. */
    #if (cfgUsePriorityLifting == cfgTrue)
      privLiftLocksOnSlot(uiRightSlot);
      #if (cfgUseSynchronization == cfgSyncDoubleBlock)
        privLiftLocksOnSlot(uiLeftSlot);
      #endif
    #endif
    /* We are done, and, since we cannot continue, ask the OS to switch to an other task. */
    privEnterOS(defActionTaskStateSwitch); } }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && ( (includeTaskQueu == cfgTrue) || (includeTaskMutex == cfgTrue) )

void privSyncReleaseBody(Tuint08 uiSlotSlot)
{ /* Call this if you want to release a queue, It is a  'body' function too, see
   * the explanation of this design elsewhere. */
  privInitOs();
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskSyncRelease);
  /* Check if the user uses this method properly. */
  Tuint08 uiRightSlot = (uiSlotSlot & defSlotRightGetMask) >> defSlotRightShift;
  #if (cfgUseSynchronization == cfgSyncDoubleBlock)
    Tuint08 uiLeftSlot  = (uiSlotSlot & defSlotLeftGetMask) >> defSlotLeftShift;
  #endif
  /* We need the current task number */
  Tuint08 uiCurrentTask = (uiOsStatus & defTaskNumberGetMask) >> defOsTaskNumberShift;
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapSynchronization) & 0xFF) );
    /* Check if the current task has the right to make use of a slot. (Check remeains needed even if we check
     * capabilities, since we may have capabilities but no slotstack defined.) */
    if (uiCurrentTask >= defNumberOfTasksWithSlot)
    /* If not report the error. */
    { privShowError((errTaskHoldsNoSlots | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    /* Check if the preprocessor allready saw that the slot is of the wrong type. Note that we 'misused'
     * the value 0xF0 to pass this information. Formally that number could also have been passed directly
     * but that is an error two btw, but of a different kind. Here we assume it is not. The OnName macros
     * can never pass a slotnumber zero, so it may only collide with a manual call on taskSyncRequest with
     * 0xF0 as slot parameter. */
    if (uiSlotSlot == defCheckSlotTypeError)
    { privShowError((errSlotTypeMismatch | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    /* The zero slot is reserved to indicate there is no lock, thus it cannot be actively used. */
    if (uiRightSlot == defSlotRightFree)
    /* Report an error and stop the task. */
    { privShowError((errSlotZeroUsed | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    /* Check the type of the slot which is coupled to its number. */
    if ( ( (uiRightSlot != defSlotRightFree) && ((uiRightSlot < defNumberQueuBegin) || (uiRightSlot >= defNumberMutexEnd ))) )
      /* If so report an error. */
    { privShowError((errSlotTypeMismatch | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    #if (cfgUseSynchronization == cfgSyncDoubleBlock)
      if ( ( (uiLeftSlot  != defSlotLeftFree)  && ((uiLeftSlot  < defNumberQueuBegin) || (uiLeftSlot  >= defNumberMutexEnd ))) )
        /* If so report an error. */
      { privShowError((errSlotTypeMismatch | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
      /* Check if not two identical slots are locked */
      if (uiRightSlot == uiLeftSlot)
      /* If so report an error. */
      { privShowError((errSlotTwins | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    #else
      /* Double slots are not allowed */
      if ((uiSlotSlot & defSlotLeftGetMask) != defSlotLeftFree)
      /* Report an error and stop the task. */
      { privShowError((errSlotDoubleUsed | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    #endif
  #endif
  /* We must remove one lock on the requested slot. We assume we posses that slot, if not
   * we cannot remove one. Now, the function returns with true if, after removal, more
   * slots of the present slot number are present, in that case we may not release a new
   * task. If false the slot was the last one and it is save to release a new task. */
  #if (cfgUseSynchronization == cfgSyncDoubleBlock)
    /* See if the right slot is no longer used by this task, after on removal */
    Tbool bRightSlotAbsent = !privOperateSlotStack((uiCurrentTask | defSlotOperateDecreaseAction), uiRightSlot);
    /* See if the left slot is no longer used by this task, after on removal */
    Tbool bLeftSlotAbsent = !privOperateSlotStack((uiCurrentTask | defSlotOperateDecreaseAction), uiLeftSlot);
    /* If we changed something at the slotstack it may be needed to check if some tasks must be released.
     * Since it is not possible that due to this change is new task must be release if the task
     * still holds an other copy of the slot blocked or free, we need not to Release anything. If we
     * do, it does not matter, but it is extra overhead. */
    if (bRightSlotAbsent | bLeftSlotAbsent) { privReleaseSyncBlockingTasks(); }
      /* If we made use of priority lifting ... */
    #if (cfgUsePriorityLifting == cfgTrue)
      /* this task must regain its original priority, if no slots are occupied any more.
       * If the slot is still present, the slotstack cannot be empty so a call is not
       * needed. It does not harm eighter, but takes a lot of cycles to complete. */
      if (bRightSlotAbsent & bLeftSlotAbsent) { privRestoreInitialPriority(uiCurrentTask); }
    #endif
  #else
    /* See if the right slot is no longer used by this task, after on removal */
    Tbool bRightSlotAbsent = !privOperateSlotStack((uiCurrentTask | defSlotOperateDecreaseAction), uiRightSlot);
    /* If we changed something at the slotstack it may be needed to check if some tasks must be released.
     * Since it is not possible that due to this change is new task must be release if the task
     * still holds an other copy of the slot blocked or free, we need not to Release anything. If we
     * do, it does not matter, but it is extra overhead. */
    if (bRightSlotAbsent) { privReleaseSyncBlockingTasks(); }
      /* If we made use of priority lifting ... */
    #if (cfgUsePriorityLifting == cfgTrue)
      /* this task must regain its original priority, if no slots are occupied any more.
       * If the slot is still present, the slotstack cannot be empty so a call is not
       * needed. It does not harm eighter, but takes a lot of cycles to complete. */
      if (bRightSlotAbsent) { privRestoreInitialPriority(uiCurrentTask); }
    #endif
  #endif
  /* We are done, but the task may not yet be over, ask the OS to reschedule me. */
  privEnterOS(defActionTaskStateKeep); }

#endif



#if (cfgUseSynchronization != cfgSyncNon) && (defUseQueus == cfgTrue) && (includeGenQueuWrite == cfgTrue)

void genQueuWrite(Tuint08 uiSlot, Tbyte bItem)
{ /* Use this method to put a byte on a queue. Usually you lock that queuee first by taskQueuRequest, so
   * you are sure you have full control over the queue and the byte you write will indeed fit on
   * the queue. However, you do not always have this possibility, for example if you are in an isr.
   * In that case you can obtain the number of free bytes with genQueuWriteable or you can just
   * flood the queue. Any byte that does not fit is silently ignored. We could have extracted the slot
   * from the lock held by the current task, but that rendered the call from an isr impossible, so
   * you have to specify which queue you are using. Of course this call may not come
   * during the queue manipulation of some other task, but that is not possible due to the protection
   * of the privEnterSysCritical in other and this routine. */
  privEnterSysCritical();
  /* Tell what we are doing */
  privTrace(traceAPIcallGenQueuWrite);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapSynchronization) & 0xFF) );
    /* The zero slot is reserved to indicate there is no lock, thus it cannot be actively used. */
    if (uiSlot == defSlotFree)
    /* Report an error and stop the task. */
    { privShowError((errSlotZeroUsed | errTaskStateCurrent), errCurrentTask ); }
    /* Check the type of the slot which is coupled to its number. */
    if ((uiSlot < defNumberQueuBegin) || (uiSlot >= defNumberQueuEnd))
    /* If so report an error. */
    { privShowError((errSlotTypeMismatch | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* Get the write level of the queue, we put it in a register, cause gcc cannot optimize this variable for
   * it is volatile. Note that the queue number and the slot number differ by one, because the first item
   * on any array in C has the number zero. The first queue however is one, because we reserved number zero
   * to indicate a free lock. */
  Tuint08 uiWrite = uiQueuWrite[uiSlot-1];
  /* If the queue is already full we ignore the item, otherwise we may place it on the queue*/
  if ((uiWrite & defQueuFillingGetMask) == defQueuFillingStateNotFull)
  { /* Get a pointer to the queue from flash. */
    Tbyte * qCurQueu = portFlashReadWord(Tbyte *,pxQueulist[uiSlot-1]);
    /* writing is done downwards (this is because it is much easier to detect we reached the bottom of
     * an array as the top (this must be extracted from flash) so lower the writing level. */
    uiWrite--;
    /* if we went trough the bottom ... */
    if (uiWrite == 0xFF)
    { /* only then we need to access the flash to find the top of the array. */
      uiWrite = privGetQueuSize(uiSlot-1)-1; }
    /* write the item (uiWrite points to the last item written). */
    qCurQueu[uiWrite] = bItem;
    /* The queue can be full, and since this information is kept in a separate bit, we must test it
     * (of course after writing a byte the queue can never be empty thus equal read and write levels
     * must imply a full queue. ) */
    if (uiWrite == uiQueuRead[uiSlot-1])
    /* if so, write the full bit. */
    { uiWrite = (uiWrite & defQueuFillingSetMask) | defQueuFillingStateFull; }
    /* restore the write level into the right place. */
    uiQueuWrite[uiSlot-1] = uiWrite; }
  /* Sometime it may be useful to interpret writing to a full queue as an error */
  #if (cfgCheckQueuFilling == cfgTrue)
    /* arriving here means we tried to write to a full queue, thus report the error, but this can
     * only be done when we are inside a task. From an ISR this is not possible because the error
     * cannot be reported and return. Furthermore, we do not know which task to terminate. This
     * function is not called from OS. */
    else if ((uiOsStatus & defContextGetMask) == defContextStateTask)
    { privShowError((errQueuOverrun | errTaskStateCurrent), ((uiSlot << errInfoNumberShift) | errCurrentTask) ); }
  #endif
  /* We are done, leave the protected zone. */
  privExitSysCritical(); }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && (defUseQueus == cfgTrue) && (includeGenQueuRead == cfgTrue)

Tbyte genQueuRead(Tuint08 uiSlot)
{ /* Use this method to get a byte on a queue. Usually you lock that queuee first by taskQueuRequest, so
   * you are sure you have full control over the queue and the byte you read will indeed be present on
   * the queue. However, you do not always have this possibility, for example if you are in an isr.
   * In that case you can obtain the number of present bytes with genQueuReadable or you can just
   * drain the queue. If the queue is empty subsequent calls to genQueuRead will return 0x00. We could
   * have extracted the slot from the lock held by the current task, but that rendered the call
   * from an isr impossible, so you have to specify which queue you are using. Of course this call may
   * not come during the queue manipulation of some other task, but that is not possible due to the
   * protection of the privEnterSysCritical in other and this routine. */
  Tbyte Result;
  privEnterSysCritical();
  /* Tell what we are doing */
  privTrace(traceAPIcallGenQueuRead);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapSynchronization) & 0xFF) );
    /* The zero slot is reserved to indicate there is no lock, thus it cannot be actively used. */
    if (uiSlot == defSlotFree)
    /* Report an error and stop the task. */
    { privShowError((errSlotZeroUsed | errTaskStateCurrent), errCurrentTask ); }
    /* Check the type of the slot which is coupled to its number. */
    if ((uiSlot < defNumberQueuBegin) || (uiSlot >= defNumberQueuEnd))
    /* If so report an error. */
    { privShowError((errSlotTypeMismatch | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* Get the read/write level of the queue, and put it in registers, cause gcc cannot optimize this
   * variable for they are volatile. Note that the queue number and the slot number differ by one,
   * because the first item on any array in C has the number zero. The first queue however is one,
   * because we reserved number zero to indicate a free lock. */
  Tuint08 uiRead = uiQueuRead[uiSlot-1];
  Tuint08 uiWrite = uiQueuWrite[uiSlot-1];
  /* Check if the queue is not empty, if so there is no point in getting an item for the queue.
   * The queue is empty if the read and write levels are pointing to the same byte and the
   * queue is not full.  */
  if ((uiWrite != uiRead) || ((uiWrite & defQueuFillingGetMask) == defQueuFillingStateFull) )
  { /* After reading a byte, the queue cannot be full anymore. We must actively remove the full
     * byte for elsewhere we use it to test if the queue is full without comparing the read and
     * write levels. */
    uiQueuWrite[uiSlot-1] = (uiWrite & defQueuFillingSetMask) | defQueuFillingStateNotFull;
    /* Get a pointer to the queue from flash. */
    Tbyte * qCurQueu = portFlashReadWord(Tbyte *,pxQueulist[uiSlot-1]);
    /* reading is done downwards (this is because it is much easier to detect we reached the bottom of
     * an array as the top (this must be extracted from flash) so lower the reading level. */
    uiRead--;
    /* if we went trough the bottom ... */
    if (uiRead == 0xFF)
    { /* only then we need to access the flash to find the top of the array. */
      uiRead = privGetQueuSize(uiSlot-1)-1; }
    /* read the item (uiRead points to the last item written). */
    Result = qCurQueu[uiRead];
    /* restore the read level into the right place. */
    uiQueuRead[uiSlot-1] = uiRead; }
  else
  { /* Sometimes it may be useful to interpret reading from an empty queue as an error */
    #if (cfgCheckQueuFilling == cfgTrue)
      /* and arriving here means we tried to read from an empty queue, thus report the error, but this can
       * only be done when we are inside a task. From an ISR this is not possible because the error
       * cannot be reported and return. Furthermore, we do not know which task to terminate. This
       * function is not called from OS. */
      if ((uiOsStatus & defContextGetMask) == defContextStateTask)
      { privShowError((errQueuUnderrun | errTaskStateCurrent), ((uiSlot << errInfoNumberShift) | errCurrentTask) ); }
    #endif
    /* If we don't see it as an error, we must return something, so this is chosen to be 0x00. */
    Result = 0x00; }
  /* We are done, leave the protected zone. */
  privExitSysCritical();
  /* and return the result. */
  return Result; }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && (defUseQueus == cfgTrue) && (includeGenQueuClear == cfgTrue)

void genQueuClear(Tuint08 uiSlot)
{ /* Use this method to clear a queue. The method works, also if some other task is holding a lock
   * on this queue, so that is your own responsibility, you can use it inside a taskQueueRequest/taskQueuRelease
   * pair. The queue is not actually emptied, but the read/write levels are reset. Of course we act inside
   * a protected realm. */
  privEnterSysCritical();
  /* Tell what we are doing */
  privTrace(traceAPIcallGenQueuClear);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapSynchronization) & 0xFF) );
    /* The zero slot is reserved to indicate there is no lock, thus it cannot be actively used. */
    if (uiSlot == defSlotFree)
    /* Report an error and stop the task. */
    { privShowError((errSlotZeroUsed | errTaskStateCurrent), errCurrentTask ); }
    /* Check the type of the slot which is coupled to its number. */
    if ((uiSlot < defNumberQueuBegin) || (uiSlot >= defNumberQueuEnd))
    /* If so report an error. */
    { privShowError((errSlotTypeMismatch | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* Reset the read and write levels. Note that we are using constants here which can effectively only
   * be 0x00 since we also rely on clean queues to start with (there is no initializing needed before
   * a queue is used) and this cleaning is provided by the .bss cleaning section. */
  uiQueuRead[uiSlot-1] = defQueuFillingReadEmpty;
  uiQueuWrite[uiSlot-1] = defQueuFillingWriteEmpty;
  /* Thats all folks. */
  privExitSysCritical(); }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && (defUseQueus == cfgTrue) && (includeGenQueuPeek == cfgTrue)

Tbyte genQueuPeek(Tuint08 uiSlot)
{ /* Sometime is may be handy to look ahead in the queue. Use this method if you want to know which
   * byte is next. Of course, if more tasks are reading from a queue (bit odd, but still) an other
   * task could have read it before you can, so additional protection measure may be needed. */
  Tbyte Result;
  /* we protect ourselves from other tasks. */
  privEnterSysCritical();
  /* Tell what we are doing */
  privTrace(traceAPIcallGenQueuPeek);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapSynchronization) & 0xFF) );
    /* The zero slot is reserved to indicate there is no lock, thus it cannot be actively used. */
    if (uiSlot == defSlotFree)
    /* Report an error and stop the task. */
   { privShowError((errSlotZeroUsed | errTaskStateCurrent), errCurrentTask ); }
    /* Check the type of the slot which is coupled to its number. */
    if ((uiSlot < defNumberQueuBegin) || (uiSlot >= defNumberQueuEnd))
    /* If so report an error. */
    { privShowError((errSlotTypeMismatch | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* Get a pointer to the queue from flash. */
  Tbyte * qCurQueu = portFlashReadWord(Tbyte *,pxQueulist[uiSlot-1]);
  /* Get a pointer to the queue from flash. */
  Tuint08 uiRead = uiQueuRead[uiSlot-1];
  /* reading is done downwards (this is because it is much easier to detect we reached the bottom of
   * an array as the top (this must be extracted from flash) so lower the reading level. */
  uiRead--;
  /* if we went trough the bottom ... */
  if (uiRead == 0xFF)
  { /* only then we need to access the flash to find the top of the array. */
    uiRead = privGetQueuSize(uiSlot-1)-1; }
  /* read the item (uiRead points to the last item written). */
  Result = qCurQueu[uiRead];
  /* Done, leave the protected realm */
  privExitSysCritical();
  /* and return the result. The queue itself is untouched. */
  return Result; }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && (defUseQueus == cfgTrue) && (includeGenQueuReadable == cfgTrue)

Tuint08 genQueuReadable(Tuint08 uiSlot)
{ /* Use this method to see how many bytes can be read from the queue */
  Tuint08 Result;
  /* we protect ourselves from other tasks. */
  privEnterSysCritical();
  /* Tell what we are doing */
  privTrace(traceAPIcallGenQueuReadable);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapSynchronization) & 0xFF) );
    /* The zero slot is reserved to indicate there is no lock, thus it cannot be actively used. */
    if (uiSlot == defSlotFree)
    /* Report an error and stop the task. */
    { privShowError((errSlotZeroUsed | errTaskStateCurrent), errCurrentTask ); }
    /* Check the type of the slot which is coupled to its number. */
    if ((uiSlot < defNumberQueuBegin) || (uiSlot >= defNumberQueuEnd))
    /* If so report an error. */
    { privShowError((errSlotTypeMismatch | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* Get the number of bytes on the queue */
  Result = privQueuTest(uiSlot,+1);
  /* Done, leave the protected realm */
  privExitSysCritical();
  /* and return the result. The queue itself is untouched. */
  return Result; }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && (defUseQueus == cfgTrue) && (includeGenQueuWriteable == cfgTrue)

Tuint08 genQueuWriteable(Tuint08 uiSlot)
{ /* Use this method to see how many bytes can be written to the queue */
  Tuint08 Result;
  /* we protect ourselves from other tasks. */
  privEnterSysCritical();
  /* Tell what we are doing */
  privTrace(traceAPIcallGenQueuWriteable);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapSynchronization) & 0xFF) );
    /* The zero slot is reserved to indicate there is no lock, thus it cannot be actively used. */
    if (uiSlot == defSlotFree)
    /* Report an error and stop the task. */
    { privShowError((errSlotZeroUsed | errTaskStateCurrent), errCurrentTask ); }
    /* Check the type of the slot which is coupled to its number. */
    if ((uiSlot < defNumberQueuBegin) || (uiSlot >= defNumberQueuEnd))
    /* If so report an error. */
    { privShowError((errSlotTypeMismatch | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* Get the number of free bytes of the queue */
  Result = privQueuTest(uiSlot,-1);
  /* Done, leave the protected realm */
  privExitSysCritical();
  /* and return the result. The queue itself is untouched. */
  return Result; }

#endif


#if (cfgUseSynchronization != cfgSyncNon) && (defUseQueus == cfgTrue) && (includeGenQueuFull == cfgTrue)

Tbool genQueuFull(Tuint08 uiSlot)
{ /* Use this method to see if the queue is full. The queue itself may change directly after the call,
   * but if you are the only one writing, you can be sure the queue is not full after this call
   * when it returns false. We protect ourselves from other tasks. */
  privEnterSysCritical();
  /* Tell what we are doing */
  privTrace(traceAPIcallGenQueuFull);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapSynchronization) & 0xFF) );
    /* The zero slot is reserved to indicate there is no lock, thus it cannot be actively used. */
    if (uiSlot == defSlotFree)
    /* Report an error and stop the task. */
    { privShowError((errSlotZeroUsed | errTaskStateCurrent), errCurrentTask ); }
    /* Check the type of the slot which is coupled to its number. */
    if ((uiSlot < defNumberQueuBegin) || (uiSlot >= defNumberQueuEnd))
    /* If so report an error. */
    { privShowError((errSlotTypeMismatch | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* Get the write level and extract the full bit. */
  Tbool Result = (uiQueuWrite[uiSlot-1] & defQueuFillingGetMask) == defQueuFillingStateFull;
  /* Done, leave the protected realm */
  privExitSysCritical();
  /* and return the result. The queue itself is untouched. */
  return Result;  }

#endif

#if (cfgUseSynchronization != cfgSyncNon) && (defUseQueus == cfgTrue) && (includeGenQueuEmpty == cfgTrue)

Tbool genQueuEmpty(Tuint08 uiSlot)
{ /* Use this method to see if the queue is empty. Protect is needed in this case, since there is no
   * empty bit that can be read atomically. The queue itself may change directly after the call, but
   * if you are the only one reading, you can be sure the queue is not empty after this call when
   * it returns false. */
  Tuint08 Result;
  /* we protect ourselves from other tasks. */
  privEnterSysCritical();
  /* Tell what we are doing */
  privTrace(traceAPIcallGenQueuEmpty);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* If we have no protection for the OS from interrupts ... */
    #if (cfgIntOsProtected == cfgFalse)
      /* we may not arrive here from an isr */
      if ((uiOsStatus & defContextGetMask) == defContextStateIsr)
      /* and if we do this is a fatal error. */
      { privShowError((fatIllegalCallfromISR | errTaskStateNon), errNoInfo ); }
    #endif
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapSynchronization) & 0xFF) );
    /* The zero slot is reserved to indicate there is no lock, thus it cannot be actively used. */
    if (uiSlot == defSlotFree)
    /* Report an error and stop the task. */
    { privShowError((errSlotZeroUsed | errTaskStateCurrent), errCurrentTask ); }
    /* Check the type of the slot which is coupled to its number. */
    if ((uiSlot < defNumberQueuBegin) || (uiSlot >= defNumberQueuEnd))
    /* If so report an error. */
    { privShowError((errSlotTypeMismatch | errTaskStateCurrent), errCurrentTask ); }
  #endif
  /* We need both levels to determine is the queue is empty. */
  Tuint08 uiRead  = uiQueuRead[uiSlot-1];
  Tuint08 uiWrite = uiQueuWrite[uiSlot-1];
  /* The queue is empty if the read and write levels are pointing to the same byte and the
   * queue is not full.  */
  Result = ((uiWrite == uiRead) && ((uiWrite & defQueuFillingGetMask) == defQueuFillingStateNotFull) );
  /* Done, leave the protected realm */
  privExitSysCritical();
  /* and return the result. The queue itself is untouched. */
  return Result; }

#endif


#if (cfgUseFileSystem  ==  cfgTrue)

void privWaitForFsAccessBody(void)
{ /* Call this if you want to wait (block) until you may write. This method cannot be called
   * from the outside although it is a 'body' function. It is used to call from the inside
   * of api functions.
   * (See the explanation of the design with 'body functions' design elsewhere.) */
  privInitOs();
  /* Tell we can not burn again. */
  privTrace(traceBurnLock);
  /* We always want to let the current task wait for access. */
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* We block without delay (this action cannot timeout, for it is not realistic to time out) */
  curTCB->uiTaskStatus = (curTCB->uiTaskStatus & (defBaseFileBlockedSetMask & defBaseDressSetMask)) | (defBaseFileBlockedTask | defBaseDressWrite);
  /* While we are waiting, we set the burn block bit. In the main OS loop, the real
   * burn state of the device is checked, and as long as it prohibits writing,
   * this bit stays activated. Test before writing. */
  uiFsStatus = ((uiFsStatus & defFsBurnBlockSetMask) | defFsBurnBlockActive);
  /* Done, and since this method is meant to block, we must switch. */
  privEnterOS(defActionTaskStateSwitch); }

#endif


#if (cfgUseFileSystem  ==  cfgTrue) && (includeTaskFileAccess == cfgTrue)

#if (cfgUseTimeout == cfgTrue)
  #if (cfgUseFileSystemConcurrentRead == cfgTrue)
    void privFileOpenBody(Tbool bReadOnly, Tuint16 uiTicksToWait)
  #else
    void privFileOpenBody(Tuint16 uiTicksToWait)
  #endif
#else
  #if (cfgUseFileSystemConcurrentRead == cfgTrue)
    void privFileOpenBody(Tbool bReadOnly)
  #else
    void privFileOpenBody(void)
  #endif
#endif
{ /* Call this if you want to open a file. The function is present in two flavours. One
   * standard, and one if you want to make use of single writing, simultaneous reading. */
  privInitOs();
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskFileOpen);
  /* For some operations we need the current task control block */
  #if (defUseFsField == cfgTrue) || (defUseFsOnMultipleTasks == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  #endif
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapFileSystem) & 0xFF) );
    /* Opening is not allowed if we are allready open */
    if ((curTCB->defFsField & (defFsReadGetMask | defFsWriteGetMask)) != ( defFsReadClear | defFsWriteClear ) )
    /* Report an error and stop the task. */
    { privShowError((errFileOpenMode | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    /* In case we allow for timeouts there are some more checks. */
    #if (cfgUseTimeout == cfgTrue)
      /* Check the capabilities of the current task */
      privCheckCapabilities((defCurrentTaskNumber), ((cfgCapTimeout) & 0xFF) );
      /* Check if we do not violate the max delay time */
      if (uiTicksToWait > defMaxDelayTime)
      /* Report an error and stop the task. */
      { privShowError((errTaskDelayTooLong | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    #endif
  #endif
  /* If we make use of timeouts, we write it as a delay. */
  #if (cfgUseTimeout == cfgTrue)
    /* If we make use of timeouts, set the delay variable, which is used for the timeout. */
    if (uiTicksToWait != defLockBlockInfinite) { privDelayCalc(uiTicksToWait,true); }
  #endif
  /* Check if we make use of simultaneous reads.  */
  #if (cfgUseFileSystemConcurrentRead == cfgTrue)
    if (bReadOnly)
    { /* The user opens the file in the read only mode */
      #if (defUseFsField == cfgTrue)
        /* Write the read mode in the field. */
        curTCB->defFsField = (curTCB->defFsField & defFsReadSetMask) | defFsReadActive;
      #endif
      /* If we have only one task using the file system it makes no sense to block, access is
       * always possible. */
      #if (defUseFsOnMultipleTasks == cfgTrue)
        /* The way this works is that we always block, and subsequently see which task can be
         * freed. If the current task is the only one, this generates a little more overhead,
         * but save a lot of bytes from the code. */
        curTCB->uiTaskStatus = (curTCB->uiTaskStatus & (defBaseFileBlockedSetMask & defBaseDressSetMask)) | (defBaseFileBlockedTask | defBaseDressRead);
      #else
        /* But we have to set that we are reading. This is not needed in case we are blocking, since then it
         * will be done in the  privReleaseFileBlocks handling. In fact, it would be plain wrong. */
        uiFsStatus = (uiFsStatus & defFsReadBlockSetMask) | defFsReadBlockActive;
      #endif
    }
    else
  #endif
  { /* If we want to write and read, we must issue a write lock*/
    #if (defUseFsField == cfgTrue)
      /* Making use of the fields required setting the writemode. We use this typically if we allow for
       * sleeping tasks, or the possibility to terminate and restart tasks. Also, if we have the checkmodus
       * switched on, we need this information. */
      curTCB->defFsField = (curTCB->defFsField & defFsWriteSetMask) | defFsWriteActive;
    #endif
    /* If we have only one task using the file system it makes no sense to block, access is
     * always possible. */
    #if (defUseFsOnMultipleTasks == cfgTrue)
      /* The way this works is that we always block, and subsequently see which task can be
       * freed. If the current task is the only one, this generates a little more overhead,
       * but save a lot of bytes from the code. */
      curTCB->uiTaskStatus = (curTCB->uiTaskStatus & (defBaseFileBlockedSetMask & defBaseDressSetMask)) | (defBaseFileBlockedTask | defBaseDressWrite);
    #else
      /* But we have to set that we are writing. This is not needed in case we are blocking, since then it
       * will be done in the  privReleaseFileBlocks handling. In fact, it would be plain wrong. */
      uiFsStatus = (uiFsStatus & defFsWriteBlockSetMask) | defFsWriteBlockActive;
    #endif
  }
  /* If we have only one task using the file system we do not block on entry, so there is
   * nothing to release right now. */
  #if (defUseFsOnMultipleTasks == cfgTrue)
    /* Check if anything must be released, which is quite likely, since often this will be the only
     * task asking for a lock. */
    privReleaseFileBlocks();
  #endif
  /* Done, since this block may be kept, we must require a switch. */
  privEnterOS(defActionTaskStateSwitch); }

#endif


#if (cfgUseFileSystem == cfgTrue) && (includeTaskFileAccess == cfgTrue)

void privFileCloseBody(void)
{ /* Call this if you want to close a file. It should be called from the task
   * that is actually closing a file, for it is assuming the current task holds
   * the lock. If the call returns all reading or wrting actions have come to
   * an end, and no burnlocks are active. The call may block.*/
  privInitOs();
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskFileClose);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapFileSystem) & 0xFF) );
    /* Get the tack control block */
    TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
    /* Closing is not allowed if we are not open */
    if ((curTCB->defFsField & (defFsReadGetMask | defFsWriteGetMask)) == ( defFsReadClear | defFsWriteClear ) )
    /* Report an error and stop the task. */
    { privShowError((errFileOpenMode | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
  #endif
  /* Ask for closing the file. Upon return the file is closed if it was reading, but if
   * the task was writing, the call returns with a write release request, and the actual
   * closing is done in the main loop. */
  privPrepareFileClose(defCurrentTaskNumber);
  /* Done lets switch to be certain. */
  privEnterOS(defActionTaskStateSwitch); }

#endif


#if (cfgUseFileSystem  ==  cfgTrue) && (cfgCheckMethodUse == cfgTrue)

static void privCheckFileSpecsWriting(Tuint08 uiFileNumber, Tuint08 uiOffset, Tuint08 uiSize)
{ /* Get the tack control block */
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* Setting size is not allowed if we are not open in write mode */
  if ((curTCB->defFsField & defFsWriteGetMask) == defFsWriteClear)
  /* Report an error and stop the task. */
  { privShowError((errFileOpenMode | errTaskStateCurrent), errCurrentTask ); }
 /* Check if we have a valid file number */
  if (uiFileNumber >= defFsNumberOfAllFiles)
  /* Report an error and stop the task. */
  { privShowError((errFileInvalidFileNumber | errTaskStateCurrent), errCurrentTask ); }
  /* Get the maximal size of file */
  Tuint08 uiFileSizeMax = privFileSpace(uiFileNumber);
  /* If we have length information ...*/
  #if (cfgUseFileSystemMaintainFAT == cfgTrue)
    /* ... get the actual length of the file*/
    Tuint08 uiFileSizeActual = (Tuint08) portFSReadByte((Taddress) (Tuint16) uiFileNumber);
  #else
    /* ... if not, we just compare with the maximal file size. */
    Tuint08 uiFileSizeActual = uiFileSizeMax;
  #endif
  /* Now, we have the following to check
   * reading 0 <= Offset <= ActualLength  AND 0 <= Offset+Size <= ActualLength
   * writing 0 <= Offset <= ActualLength  AND 0 <= Offset+Size <= AvailableSpace
   * Note that at writing we may start after the last byte, but not at reading, except if we read zero bytes.
   * In case we do not have length information we simply set ActualLength:=AvailableSpace.
   * Of course we assume ActualLength <= AvailableSpace. If not, we should have detected that earlier.
   * Note that we must rearrange the calculations to stay within the 8 bit datasize. */
  /* Here we are writing.*/
  if ((uiOffset > uiFileSizeActual) || (uiSize > (uiFileSizeMax - uiOffset) ))
  /* Report an error and stop the task. */
  { privShowError((errFileOutOfRange | errTaskStateCurrent), errCurrentTask ); } }

#endif


#if (cfgUseFileSystem  ==  cfgTrue) && (cfgCheckMethodUse == cfgTrue)

static void privCheckFileSpecsReading(Tuint08 uiFileNumber, Tuint08 uiOffset, Tuint08 uiSize)
{ /* Get the tack control block */
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* Getting the size is not allowed if we are not open */
  if ((curTCB->defFsField & (defFsReadGetMask | defFsWriteGetMask)) == ( defFsReadClear | defFsWriteClear ) )
  /* Report an error and stop the task. */
  { privShowError((errFileOpenMode | errTaskStateCurrent), errCurrentTask ); }
  /* Check if we have a valid file number */
  if (uiFileNumber >= defFsNumberOfAllFiles)
  /* Report an error and stop the task. */
  { privShowError((errFileInvalidFileNumber | errTaskStateCurrent), errCurrentTask ); }
  /* If we have length information ...*/
  #if (cfgUseFileSystemMaintainFAT == cfgTrue)
    /* ... get the actual length of the file*/
    Tuint08 uiFileSizeActual = (Tuint08) portFSReadByte((Taddress) (Tuint16) uiFileNumber);
  #else
    /* ... if not, we just compare with the maximal file size. */
    Tuint08 uiFileSizeActual = privFileSpace(uiFileNumber);
  #endif
  /* Now, we have the following to check
   * reading 0 <= Offset <= ActualLength  AND 0 <= Offset+Size <= ActualLength
   * writing 0 <= Offset <= ActualLength  AND 0 <= Offset+Size <= AvailableSpace
   * Note that at writing we may start after the last byte, but not at reading, except if we read zero bytes.
   * In case we do not have length information we simply set ActualLength:=AvailableSpace.
   * Of course we assume ActualLength <= AvailableSpace. If not, we should have detected that earlier.
   * Note that we must rearrange the calculations to stay within the 8 bit datasize. */
  /* Here we are reading.*/
  if ((uiOffset > uiFileSizeActual) || (uiSize > (uiFileSizeActual - uiOffset) ))
  /* Report an error and stop the task. */
  { privShowError((errFileOutOfRange | errTaskStateCurrent), errCurrentTask ); } }



#endif

#if (cfgUseFileSystem  ==  cfgTrue) && (cfgUseFileSystemMaintainFAT == cfgTrue) && (includeTaskFileSetSize == cfgTrue)

void taskFileSetSize(Tuint08 uiFileNumber, Tuint08 uiSize)
{ /* Call this if you want adjust the size of a file or to clear a file. Only call it inside
   * fileOpen/fileClose construct. Note that this method is interruptable by a tick interrupt.
   * The file is not actually cleared or changed but the length is simply set to a new value.
   * If you need to really clear a file, fill it with zero's manually. */
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
  #endif
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskFileSetSize);
  /* Check the use of this call.*/
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapFileSystem) & 0xFF) );
    /* Check if the parameters are such that operations stay within the filespace and are sensible. */
    privCheckFileSpecsWriting(uiFileNumber,0,uiSize);
  #endif
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* Set the length of the file. Since the length info is at the start of the filespace,
   * we can just cast the filenumber to an address. Writing the new length is all to do. */
  portFSWriteByte(((Taddress) (Tuint16) uiFileNumber),uiSize);
  /* Wait until the write is over. The privWaitForFsAccess() blocks until
   * the burnlock has been cleared. */
  privWaitForFsAccess(); }

#endif


#if (cfgUseFileSystem  ==  cfgTrue) && (cfgUseFileSystemMaintainFAT == cfgTrue) && (includeTaskFileGetSize == cfgTrue)

Tuint08 taskFileGetSize(Tuint08 uiFileNumber)
{ /* Call this if you know the size of a file. Only call it inside fileOpen/fileClose construct.
   * Note that this method is interruptable by a tick interrupt.*/
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
  #endif
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskFileGetSize);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapFileSystem) & 0xFF) );
    /* Check if the parameters are such that operations stay within the filespace and are sensible. */
    privCheckFileSpecsReading(uiFileNumber,0,0);
  #endif
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* Return the length of the file. Since the length info is at the start of the filespace,
   * we can just cast the filenumber to an address. */
  return (Tuint08) portFSReadByte((Taddress) (Tuint16) uiFileNumber); }

#endif


#if (cfgUseFileSystem  ==  cfgTrue) && (includeTaskFileFormat == cfgTrue)

void taskFileFormat(void)
{ /* Call this if you want to set all bytes from the filesystem to zero. Only call it inside
   * fileOpen/fileClose construct. Note that this method is interruptable by a tick interrupt.*/
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
  #endif
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskFileFormat);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapFileSystem) & 0xFF) );
    /* Get the tack control block */
    TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
    /* Formatting the file system is not allowed if we are not open in the write mode */
    if ((curTCB->defFsField & defFsWriteGetMask) == defFsWriteClear)
    /* Report an error and stop the task. */
    { privShowError((errFileOpenMode | errTaskStateCurrent), errCurrentTask ); }
  #endif
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* Loop through all bytes and clean them */
  Tuint16 uiLoopBytes;
  for(uiLoopBytes=0; uiLoopBytes<defTotalAllFileSpace; uiLoopBytes++)
  { /* Wait until we may write. We do not ask if the burnlock is present, for it most certainly
     * is, directly after a write operation. */
    privWaitForFsAccess();
    /* Get the bytes from the pipe and write them to the file system. */
    portFSWriteByte((Taddress) uiLoopBytes,0); }
  /* Wait until we are finished writing. */
  privWaitForFsAccess(); }

#endif


#if (cfgUseFileSystem  ==  cfgTrue) && (includeTaskFileReadByte == cfgTrue)

Tbyte taskFileReadByte(Tuint08 uiFileNumber, Tuint08 uiOffset)
{ /* Call this if you to read a byte from the file. You have 'raw access'
   * and can read beyond the end of the file. Only call it inside fileOpen/fileClose
   * construct. Note that this method is interruptable by a tick interrupt.*/
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
  #endif
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskFileReadByte);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapFileSystem) & 0xFF) );
    /* Check if the parameters are such that operations stay within the filespace and are sensible. */
    privCheckFileSpecsReading(uiFileNumber,uiOffset,1);
  #endif
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* Get the location of the byte that must be read. */
  Taddress pFileLoc = privFileLocation(uiFileNumber,uiOffset);
  /* Read the byte and return the value */
  return (Tuint08) portFSReadByte(pFileLoc); }

#endif


#if (cfgUseFileSystem  ==  cfgTrue) && (includeTaskFileWriteByte == cfgTrue)

void taskFileWriteByte(Tuint08 uiFileNumber, Tuint08 uiOffset, Tbyte bItem)
{ /* Call this if you to write a byte to the file. You have 'raw access'
   * and can write beyond the end of the file. Length info is not maintained.
   * Only call it inside fileOpen/fileClose
   * construct. Note that this method is interruptable by a tick interrupt.*/
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
  #endif
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskFileWriteByte);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapFileSystem) & 0xFF) );
    /* Check if the parameters are such that operations stay within the filespace and are sensible. */
    privCheckFileSpecsWriting(uiFileNumber,uiOffset,1);
  #endif
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* Get the location of the byte that must be written. */
  Taddress pFileLoc = privFileLocation(uiFileNumber,uiOffset);
  /* write the value, it is assumed any burnlocks are gone. */
  portFSWriteByte(pFileLoc,bItem);
  /* Wait until the write is over. The privWaitForFsAccess() blocks until
   * the burnlock has been cleared. */
  privWaitForFsAccess(); }

#endif


#if (cfgUseFileSystem  ==  cfgTrue) && (cfgUseFileSystemMaintainFAT == cfgTrue) && (includeTaskFileAppendByte == cfgTrue)

void taskFileAppendByte(Tuint08 uiFileNumber, Tbyte bItem)
{ /* Call this if you to write one byte to the end of the file. If you
   * pass the end the byte is written at the start. Use as ringbuffer.
   * Only call it inside fileOpen/fileClose */
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
  #endif
  /* Tell what we are doing */
  privTrace(traceAPIcallTaskFileAppendByte);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapFileSystem) & 0xFF) );
    /* Since appending works like a ring buffer, the byte will always fit. So we can
     * specify zero for both the offset and the size (whereas we usually have to specify 1. */
    privCheckFileSpecsWriting(uiFileNumber,0,0);
  #endif
  #if (cfgCheckTrace == cfgTrue) || (cfgCheckMethodUse == cfgTrue)
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* We must know the size of the present file. */
  Tuint08 uiFileSize =  portFSReadByte((Taddress) (Tuint16) uiFileNumber);
  /* We must know the maximal space this file may occupy. */
  Tuint08 uiFileSizeMax = privFileSpace(uiFileNumber);
  /* If the file is full, we wrap around */
  if (uiFileSize >= uiFileSizeMax)
  { /* The next byte is written at the beginning. */
    uiFileSize = 0;
    /* To protect the file from for example power down we first write the new length. This is not needed
     * if we don't wrap since in that case we do not overwrite any old information, so if the writing
     * is corrupted after writing the new byte, the old length is still an accurate value. */
    portFSWriteByte((Taddress) (Tuint16) uiFileNumber,uiFileSize);
    /* Wait until we may write again */
    privWaitForFsAccess(); }
  /* Get the location of the byte that must be written. */
  Taddress pFileLoc = privFileLocation(uiFileNumber,uiFileSize);
  /* write the value, it is assumed any burnlocks are gone. */
  portFSWriteByte(pFileLoc,bItem);
  /* Wait until we may write. */
  privWaitForFsAccess();
  /* Write the new length, thereby effectively creating the file. */
  portFSWriteByte((Taddress) (Tuint16) uiFileNumber,uiFileSize+1);
  /* Wait until the write is over. The privWaitForFsAccess() blocks until
   * the burnlock has been cleared. */
  privWaitForFsAccess(); }

#endif



#if (cfgUseFileSystem  ==  cfgTrue) && (includeTaskFileReadPipe == cfgTrue)

void taskFileReadPipe(Tuint08 uiFileNumber, Tuint08 uiOffset, Tuint08 uiSize, void (*pipe)(Tchar))
{ /* Call this if you want to read a byte stream from the file.  Only call it inside fileOpen/fileClose
   * construct. Note that this method is interruptable by a tick interrupt.*/
  #if (cfgCheckTrace == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
    /* Tell what we are doing */
    privTrace(traceAPIcallTaskFileReadPipe);
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* Get the location where to start. */
  Taddress pFileLoc = privFileLocation(uiFileNumber,uiOffset);
  /* If we do not have a FAT length and size calculations are not possible. */
  #if (cfgUseFileSystemMaintainFAT == cfgTrue)
    /* see if we must maximize the size */
    if (uiSize == defUntilFileEnd) { uiSize = (Tuint08) portFSReadByte((Taddress) (Tuint16) uiFileNumber); }
  #endif
  /* Check if the user uses this method properly. We cannot do this at the start since the
   * parameters may have changed. We can posphone it because we have no writing here. Incorrect
   * reads are just wrapped. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapFileSystem) & 0xFF) );
    /* Check if the parameters are such that operations stay within the filespace and are sensible. */
    privCheckFileSpecsReading(uiFileNumber,uiOffset,uiSize);
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* Loop through all bytes and read them */
  while(uiSize--)
  { pipe((Tchar) portFSReadByte(pFileLoc));
    /* After each read increase the pointer to get the next byte. */
    pFileLoc++; } }

#endif


#if (cfgUseFileSystem  ==  cfgTrue) && (includeTaskFileReadBuffer == cfgTrue)

void taskFileReadBuffer(Tuint08 uiFileNumber, Tuint08 uiOffset, Tuint08 uiSize, Taddress pBuffer)
{ /* Call this if you want to read a byte stream from the file. You have managed access so
   * you cannot read beyond the end of the file. Only call it inside fileOpen/fileClose
   * construct. Note that this method is interruptable by a tick interrupt.*/
  #if (cfgCheckTrace == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
    /* Tell what we are doing */
    privTrace(traceAPIcallTaskFileReadBuffer);
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* Get the location where to start. */
  Taddress pFileLoc = privFileLocation(uiFileNumber,uiOffset);
  /* If we do not have a FAT length and size calculations are not possible. */
  #if (cfgUseFileSystemMaintainFAT == cfgTrue)
    /* see if we must maximize the size */
    if (uiSize == defUntilFileEnd) { uiSize = (Tuint08) portFSReadByte((Taddress) (Tuint16) uiFileNumber); }
  #endif
  /* Check if the user uses this method properly. We cannot do this at the start since the
   * parameters may have changed. We can posphone it because we have no writing here. Incorrect
   * reads are just wrapped. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapFileSystem) & 0xFF) );
    /* Check if the parameters are such that operations stay within the filespace and are sensible. */
    privCheckFileSpecsReading(uiFileNumber,uiOffset,uiSize);
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* Loop through all bytes and read them */
  while(uiSize--)
  { *(pBuffer++) = portFSReadByte(pFileLoc);
    /* After each read increase the pointer to get the next byte. */
    pFileLoc++;  } }

#endif



#if (cfgUseFileSystem  ==  cfgTrue) && (includeTaskFileWritePipe == cfgTrue)

void taskFileWritePipe(Tuint08 uiFileNumber, Tuint08 uiOffset, Tuint08 uiSize, Tchar (*pipe)(void))
{ /* Call this if you want to write a byte stream from the file. You have managed access. You
   * can start at the beginning of a file or append at the end. Only call it inside fileOpen/fileClose
   * construct. Note that this method is interruptable by a tick interrupt.*/
  #if (cfgCheckTrace == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
    /* Tell what we are doing */
    privTrace(traceAPIcallTaskFileWritePipe);
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* If we do not have a FAT length and size calculations are not possible. */
  #if (cfgUseFileSystemMaintainFAT == cfgTrue)
    /* If we want to append to the file, we set the offset to point at the first new byte. */
    if (uiOffset == defFromFileEnd)  { uiOffset = (Tuint08) portFSReadByte((Taddress) (Tuint16) uiFileNumber); }
    /* Calculate the new size of the file. */
    Tuint08 uiNewFileLength = uiOffset + uiSize;
  #endif
  /* Check if the user uses this method properly. We cannot do this at the start since the
   * parameters may have changed. But we must do it before any writing takes place. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapFileSystem) & 0xFF) );
    /* Check if the parameters are such that operations stay within the filespace and are sensible. */
    privCheckFileSpecsWriting(uiFileNumber,uiOffset,uiSize);
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* If we do not have a FAT length and size calculations are not possible. */
  #if (cfgUseFileSystemMaintainFAT == cfgTrue)
    /* Clear the length of the file. Effectively the file is now gone. If the power goes down while
     * writing, the file will be deleted, but at least the file system will be in a consistent state.
     * This way we protect the file integrity. */
    portFSWriteByte(((Taddress) (Tuint16) uiFileNumber),0);
  #endif
  /* Get the location where to start. */
  Taddress pFileLoc = privFileLocation(uiFileNumber,uiOffset);
  /* Loop through all bytes and write them */
  while(uiSize--)
  { /* Wait until we may write. We do not ask if the burnlock is present, for it most certainly
     * is, directly after a write operation. */
    privWaitForFsAccess();
    /* Get the bytes from the pipe and write them to the file system. */
    portFSWriteByte(pFileLoc,pipe());
    /* Adjust the pointer. (Do not issue this command like this: portFSWriteByte(pFileLoc++,pipe()), it will
     * generate a frame pointer and costs a lot of bytes. ) */
    pFileLoc++; }
  /* If we do not have a FAT no size needs to be saved */
  #if (cfgUseFileSystemMaintainFAT == cfgTrue)
    /* Wait until we may write. */
    privWaitForFsAccess();
    /* Write the new length, thereby effectively creating the file. */
    portFSWriteByte((Taddress) (Tuint16) uiFileNumber,uiNewFileLength);
  #endif
  /* Wait until we may write */
  privWaitForFsAccess(); }

#endif


#if (cfgUseFileSystem  ==  cfgTrue) && (includeTaskFileWriteBuffer == cfgTrue)

void taskFileWriteBuffer(Tuint08 uiFileNumber, Tuint08 uiOffset, Tuint08 uiSize, Taddress pBuffer)
{ /* Call this if you want to write a byte stream from the file. You have managed access. You
   * can start at the beginning of a file or append at the end. Only call it inside fileOpen/fileClose
   * construct. Note that this method is interruptible by a tick interrupt.*/
  #if (cfgCheckTrace == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
    /* Tell what we are doing */
    privTrace(traceAPIcallTaskFileWriteBuffer);
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* If we do not have a FAT length and size calculations are not possible. */
  #if (cfgUseFileSystemMaintainFAT == cfgTrue)
    /* If we want to append to the file, we set the offset to point at the first new byte. */
    if (uiOffset == defFromFileEnd)  { uiOffset = (Tuint08) portFSReadByte((Taddress) (Tuint16) uiFileNumber); }
    /* Calculate the new size of the file. */
    Tuint08 uiNewFileLength = uiOffset + uiSize;
  #endif
  /* Check if the user uses this method properly. We cannot do this at the start since the
   * parameters may have changed. But we must do it before any writing takes place. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* We need protection for the maintenance functions. */
    privEnterSysCritical();
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapFileSystem) & 0xFF) );
    /* Check if the parameters are such that operations stay within the file space and are sensible. */
    privCheckFileSpecsWriting(uiFileNumber,uiOffset,uiSize);
    /* Here we can switch of that protection */
    privExitSysCritical();
  #endif
  /* If we do not have a FAT length size modifications are not possible. */
  #if (cfgUseFileSystemMaintainFAT == cfgTrue)
    /* Clear the length of the file. Effectively the file is now gone. If the power goes down while
     * writing, the file will be deleted, but at least the file system will be in a consistent state.
     * This way we protect the file integrity. */
    portFSWriteByte(((Taddress) (Tuint16) uiFileNumber),0);
  #endif
  /* Get the location where to start. */
  Taddress pFileLoc = privFileLocation(uiFileNumber,uiOffset);
  /* Loop through all bytes and write them */
  while(uiSize--)
  { /* Wait until we may write. We do not ask if the burn lock is present, for it most certainly
     * is, directly after a write operation. */
    privWaitForFsAccess();
    /* Get the bytes from the pipe and write them to the file system. */
    portFSWriteByte(pFileLoc,*pBuffer);
    /* Adjust the pointers. (Do not issue this command like this: portFSWriteByte(pFileLoc++,*(pBuffer++)), it will
     * generate a frame pointer and costs a lot of bytes. ) */
    pBuffer++;
    pFileLoc++; }
  /* If we do not have a FAT no size needs to be saved */
  #if (cfgUseFileSystemMaintainFAT == cfgTrue)
    /* Wait until we may write. */
    privWaitForFsAccess();
    /* Write the new length, thereby effectively creating the file. */
    portFSWriteByte((Taddress) (Tuint16) uiFileNumber,uiNewFileLength);
  #endif
  /* Wait until we may write */
  privWaitForFsAccess(); }

#endif


#if (cfgUseEvents == cfgTrue) && (includeTaskWaitForEvents == cfgTrue)

#if (cfgUseTimeout == cfgTrue)
  void privWaitForEventBody(Tuint08 uiEventSet, Tuint16 uiTicksToWait)
#else
  void privWaitForEventBody(Tuint08 uiEventSet)
#endif
{ /* Call this if you want to wait for one or more events.
   * This is a switching call, let us first initialize the OS.
   * (It is a 'body' function too, see the explanation of this design elsewhere.) */
  privInitOs();
  /* Tell what we are doing. */
  privTrace(traceAPIcallTaskWaitForEvent);
  /* Check if the user uses this method properly. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* Check the capabilities of the current task */
    privCheckCapabilities((defCurrentTaskNumber), ((cfgCapEvent) & 0xFF) );
    #if (cfgUseTimeout == cfgTrue)
      /* Check the capabilities of the current task */
      privCheckCapabilities((defCurrentTaskNumber), ((cfgCapTimeout) & 0xFF) );
      /* Check if we do not violate the max delay time */
      if (uiTicksToWait > defMaxDelayTime)
      /* Report an error and stop the task. */
      { privShowError((errTaskDelayTooLong | errTaskStateCurrent | errOsStateSwitch), errCurrentTask ); }
    #endif
  #endif
  /* If we make use of timeouts, we write it as a delay. */
  #if (cfgUseTimeout == cfgTrue)
    /* If we make use of timeouts, set the delay variable, which is used for the timeout. */
    if (uiTicksToWait != defLockBlockInfinite) { privDelayCalc(uiTicksToWait,true); }
  #endif
  /* Get the task control block of the task that called this routine. */
  TtaskControlBlock * curTCB = privTcbList(defCurrentTaskNumber);
  /* Set the event block. This may contain one or more events. */
  curTCB->uiTaskEvents = uiEventSet;
  /* block the task */
  curTCB->uiTaskStatus = (curTCB->uiTaskStatus & (defBaseModeSetMask & defBaseBlockStateSetMask & defBaseDressSetMask)) | (defBaseModeSync | defBaseBlockStateBlocked | defBaseDressEvent);
  /* We are done, since we blocked we switch in any case. */
  privEnterOS(defActionTaskStateSwitch); }

#endif


#if (cfgUseEvents == cfgTrue) && (includeGenFireEvent == cfgTrue) && ((cfgUseEventsOnVariables == cfgTrue) || (defEventRegisterAtomicOperation == cfgFalse) || (cfgCheckTrace == cfgTrue))

void genFireEvent(Tuint08 uiEvent)
{ /* Use this definition of getFireEvent if it is not possible to make use of privFireEvent directly.
   * This is the case if we need tracing, if the register is not located in a region reachable by
   * atomic bitsetting or if you cannot supply a compile time constant to the routine.
   * We need global protection in any situation. */
  privEnterGlobalCritical();
  /* Tell what we are doing. */
  privTrace(traceFireEvent | uiEvent);
  /* Check if the user uses this method properly. Note we do not need to activate capabilities for
   * the task using genFireEvent, so don't check for it. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* It makes a difference if the Femto OS needs one bit of the register for its own purpose.  */
    #if (cfgSysSqueezeState == cfgTrue)
      /* If not we only need to check if the Event number is a valid one. */
      if ((uiEvent < defNumberEventBegin) || (uiEvent >= defNumberEventEnd))
    #else
      /* Otherwise we must check is the same register is used for events and the AuxRegBit */
      if (defEqualAuxiliaryRegisters)
        /* and if the event number coincides with this number*/
        if ((uiEvent==devAuxSysRegBit) || (uiEvent < defNumberEventBegin) || (uiEvent >= defNumberEventEnd))
    #endif
    /* If any of these conditions apply we signal an error. */
    { privShowError((fatIllegalEvent | errTaskStateCurrent) , errCurrentTask); }
  #endif
  /* Fire the event now we know we may do so. */
  privFireEvent(uiEvent);
  /* Remove the protection. */
  privExitGlobalCritical(); }

#endif


#if (cfgUseEvents == cfgTrue) && (includeGenCountEventBlocks == cfgTrue)

Tuint08 genCountEventBlocks(Tuint08 uiEventSet)
{ /* Call this if you want to know how many tasks are blocking on a particular combination of events. */
  /* Don't allow for changes in the background.  Handling events require global protection.
   * Please note that, even in that case, this may not be enough since the number may
   * be unreliable directly after is is returned. */
  privEnterGlobalCritical();
  /* Tell what we are doing. */
  privTrace(traceAPIcallGenCountEventBlocks);
  /* Check if the user uses this method properly. Note we do not need to activate capabilities
   * for the task using genCountEventBlocks, so don't check for it. */
  #if (cfgCheckMethodUse == cfgTrue)
    /* It makes a difference if the Femto OS needs one bit of the register for its own purpose.  */
    #if (cfgSysSqueezeState == cfgFalse)
      /* We must check that no event is fired which is the same as the register used the AuxRegBit, thus
       * first check if the AuxRegBit and the event registers are shared */
      if (defEqualAuxiliaryRegisters)
        /* and then check if this bit is not set. */
        if ((uiEventSet & (1 << devAuxSysRegBit)) != 0)
    /* If any of these conditions apply we signal an error. */
    { privShowError((fatIllegalEvent | errTaskStateCurrent) , errCurrentTask); }
    #endif
  #endif
  /* Reset the counter for blocked tasks. */
  Tuint08 Result = 0;
  /* Loop through all tasks possibly waiting on an event. */
  Tuint08 uiLoopTask;
  for (uiLoopTask=defTaskNumberEventBegin; uiLoopTask<defTaskNumberEventEnd; uiLoopTask++)
  { TtaskControlBlock * loopTCB = privTcbList(uiLoopTask);
    /* Test if the task is really waiting on all the events, and increase the counter if so. */
    if ((loopTCB->uiTaskEvents & uiEventSet) == uiEventSet) { Result++; } }
  /* We are done */
  privExitGlobalCritical();
  /* Return the number of tasks that are waiting on this particular EventSet */
  return Result; }

#endif


#if (cfgUseEvents == cfgTrue) && (includeGenFireEventSet == cfgTrue)

void genFireEventSet(Tuint08 uiEventSet)
{ /* Call this if you want to release all tasks waiting for events. It has its
   * own interrupt protection. We must acquire global protection, for tick protection
   * may not be sufficient. */
  privEnterGlobalCritical();
  /* Tell what we are doing. */
  privTrace(traceAPIcallGenFireEventSet);
  /* Set all events. */
  #if (cfgSysSqueezeState == cfgFalse)
    /* In case the Femto makes use of the auxiliary bit, we must check if the register for the
     * events does not collide with the registers that holds that bit. If so, that is not a valid
     * event flag. */
    if (defEqualAuxiliaryRegisters)
    { /* The space for the events is shared with the auxiliary bit needed by the OS, make sure we do not
       * set that bit. */
      portEventRegister = preBitClr1(uiEventSet,devAuxSysRegBit); }
    else
  #endif
  { portEventRegister = uiEventSet; }
  /* leave the protected zone, and we are done. */
  privExitGlobalCritical(); }

#endif
