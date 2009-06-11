//
// Femto OS v 0.88 - Copyright (C) 2008  Ruud Vlaming
//
// This file is part of the Femto OS distribution.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Please note that, due to the GPLv3 license, for application of this
// work and/or combined work in embedded systems special obligations apply.
// If these are not to you liking, please know the Femto OS is dual
// licensed. A commercial license and support are available.
// See http://www.femtoos.org/ for details.
//

unit Constants;

interface

const defBaseStopStateGetMask        : integer = $80;
const defBaseModeGetMask             : integer = $40;
const defBaseBlockStateGetMask       : integer = $20;
const defBaseDelayStateGetMask       : integer = $10;
const defBasePrioGetMask             : integer = $0E;
const defBaseDressGetMask            : integer = $01;
const defBaseSuspendedGetMask        : integer = $E0;
const defBaseTerminatedGetMask       : integer = $F0;
const defBaseSleepingGetMask         : integer = $C0;
const defBaseRunningGetMask          : integer = $F0;
const defBaseNoBlocksGetMask         : integer = $E0;
const defBaseSyncBlockedGetMask      : integer = $E0;
const defBaseFileBlockedGetMask      : integer = $E0;
const defBaseUnusedBlockedGetMask    : integer = $E0;
const defBaseStopStateGo             : integer = $80;
const defBaseStopStateHalt           : integer = $00;
const defBaseModeSuspend             : integer = $00;
const defBaseModeSleep               : integer = $40;
const defBaseModeFile                : integer = $00;
const defBaseModeSync                : integer = $40;
const defBaseBlockStateFree          : integer = $20;
const defBaseBlockStateBlocked       : integer = $00;
const defBaseDelayStateWake          : integer = $10;
const defBaseDelayStateDelayed       : integer = $00;
const defBaseDressRunable            : integer = $01;
const defBaseDressDone               : integer = $00;
const defBaseDressSlot               : integer = $01;
const defBaseDressEvent              : integer = $00;
const defBaseDressRead               : integer = $01;
const defBaseDressWrite              : integer = $00;
const defBaseRunningTask             : integer = $F0;
const defBaseNoBlocksTask            : integer = $E0;
const defBaseSyncBlockedTask         : integer = $C0;
const defBaseFileBlockedTask         : integer = $80;
const defBaseSleepingTask            : integer = $40;
const defBaseSuspendedTask           : integer = $20;
const defBaseTerminatedTask          : integer = $10;
const defBasePrioShift               : Integer = 1;

const defFsReadGetMask          : integer = $80;
const defFsWriteGetMask         : integer = $40;
const defFsReadActive           : integer = $80;
const defFsReadClear            : integer = $00;
const defFsWriteActive          : integer = $40;
const defFsWriteClear           : integer = $00;

const defFsReadBlockGetMask         : integer = $80;
const defFsWriteBlockGetMask        : integer = $40;
const defFsSleepRequestGetMask      : integer = $20;
const defFsBurnBlockGetMask         : integer = $10;
const defFsWriteNumberGetMask       : integer = $0F;
const defFsReadCountGetMask         : integer = $0F;
const defFsReadBlockActive          : integer = $80;
const defFsReadBlockReleaseRequest  : integer = $80;
const defFsWriteBlockActive         : integer = $40;
const defFsSleepRequestActive       : integer = $20;
const defFsBurnBlockActive          : integer = $10;
const defFsReadBlockClear           : integer = $00;
const defFsWriteBlockClear          : integer = $00;
const defFsSleepRequestClear        : integer = $00;
const defFsBurnBlockClear           : integer = $00;

const defDogGetMask           : integer = $0C;
const defRetGetMask           : integer = $03;
const defDogDead              : integer = $00;
const defDogBark              : integer = $04;
const defDogWake              : integer = $08;
const defDogSleep             : integer = $0C;
const defSlotRightGetMask     : integer = $0F;
const defSlotLeftGetMask      : integer = $F0;
const defSlotRightShift       : integer = 0;
const defSlotLeftShift        : integer = 4;

const defNearWakePresentGetMask  : integer = $20;
const defSwitchAllowGetMask      : integer = $10;
const defNearWakeStatePresent    : integer = $20;
const defNearWakeStateAbsent     : integer = $00;
const defSwitchStateBlocked      : integer = $10;
const defSwitchStateRunning      : integer = $00;
const defTaskNumberGetMask       : integer = $0F;
const defTaskNumberShift         : integer = 0;

const defResponseOK                  : Integer = $01;  // datalength: 0
const defResponseUnknownCommand      : Integer = $02;  // datalength: 0
const defResponseError               : Integer = $03;  // datalength: 0
const defResponseInvalidTask         : Integer = $04;  // datalength: 0
const defResponseInvalidPriority     : Integer = $05;  // datalength: 0

const defResponseFemtoOs             : Integer = $10;  // datalength: 12
const defResponseLogTask             : Integer = $11;
const defResponseLogOs               : Integer = $12;
const defResponseTickCount           : Integer = $13;  // datalength: 2
const defResponseDump                : Integer = $14;  // datalength: 2
const defResponseUptime              : Integer = $15;  // datalength: 4
const defResponseFile                : Integer = $16;  // datalength: x
const defResponseInvalidFileNumber   : Integer = $17;  // datalength: 0

const defDumpRAM                     : integer = $01;
const defDumpEEPROM                  : integer = $02;
const defDumpFLASH                   : integer = $03;


const defLengthOK                  : Integer = 1;
const defLengthUnknownCommand      : Integer = 1;
const defLengthError               : Integer = 1;      // datalength: 0
const defLengthInvalidTask         : Integer = 1;      // datalength: 0
const defLengthInvalidPriority     : Integer = 1;      // datalength: 0
const defLengthInvalidFileNumber   : Integer = 1;      // datalength: 0
const defLengthFemtoOs             : Integer = 13;     // datalength: 12
const defLengthLogTask             : Integer = 23;
const defLengthLogOs               : Integer = 12;
const defLengthTickCount           : Integer = 3;      // datalength: 2
const defLengthUptime              : Integer = 6;      // datalength: 5
const defLengthDump                : Integer = 19;     // datalength: 2
const defLengthFile                : Integer = 2;      // datalength: 2

const defRequestFemtoOs              : Integer = $90;  // datalength: 0
const defRequestProcessList          : Integer = $91;  // datalength: 0
const defRequestTickCount            : Integer = $93;  // datalength: 0
const defRequestReboot               : Integer = $94;  // datalength: 0
const defRequestSuspend              : Integer = $95;  // datalength: 1
const defRequestResume               : Integer = $96;  // datalength: 1
const defRequestKill                 : Integer = $97;  // datalength: 1
const defRequestPriority             : Integer = $98;  // datalength: 2
const defRequestTaskRestart          : Integer = $99;  // datalength: 1
const defRequestDump                 : Integer = $9A;  // datalength: 3
const defRequestStore                : Integer = $9B;  // datalength: 3
const defRequestRecall               : Integer = $9C;  // datalength: 3
const defRequestUptime               : Integer = $9D;  // datalength: 0
const defRequestTest                 : Integer = $9E;  // datalength: 1  , test overflows the queue


const defDemoRequestLedState         : Integer = $20;
const defDemoRequestLedCycle         : Integer = $21;
const defDemoRequestLedSave          : Integer = $22;
const defDemoRequestLedLoad          : Integer = $23;
const defDemoRequestLedStateOff      : Integer = $00;
const defDemoRequestLedStateOn       : Integer = $01;
const defDemoRequestLedStateFlash    : Integer = $02;
const defDemoRequestLedStateFreeze   : Integer = $04;
const defDemoRequestLedStateFollow   : Integer = $05;
const defDemoRequestLedStateUndef    : Integer = $FF;
const defDemoResponseLedOK           : Integer = $A0;
const defDemoResponseLedError        : Integer = $A1;

const eventArray: array[0..255] of String = (
'Null', // 0x00
'MARKER', // 0x01
'Undefined 0x02', // 0x02
'Undefined 0x03', // 0x03
'Undefined 0x04', // 0x04
'Undefined 0x05', // 0x05
'Undefined 0x06', // 0x06
'Undefined 0x07', // 0x07
'Undefined 0x08', // 0x08
'Undefined 0x09', // 0x09
'Undefined 0x0A', // 0x0A
'Undefined 0x0B', // 0x0B
'Undefined 0x0C', // 0x0C
'Undefined 0x0D', // 0x0D
'Undefined 0x0E', // 0x0E
'LOOP_MARKER 0x0F', // 0x0F, used internally in trace program as marker for multibyte messages
'TaskStart 0', // 0x10
'TaskStart 1', // 0x11
'TaskStart 2', // 0x12
'TaskStart 3', // 0x13
'TaskStart 4', // 0x14
'TaskStart 5', // 0x15
'TaskStart 6', // 0x16
'TaskStart 7', // 0x17
'TaskStart 8', // 0x18
'TaskStart 9', // 0x19
'TaskStart 10', // 0x1A
'TaskStart 11', // 0x1B
'TaskStart 12', // 0x1C
'TaskStart 13', // 0x1D
'TaskStart 14', // 0x1E
'TaskStart 15', // 0x1F
'OsStart', // 0x20
'IdleStart', // 0x21
'SleepStart', // 0x22
'TickInterrupt', // 0x23
'Watchdog barks', // 0x24
'LockAquired', // 0x25
'LockRejected', // 0x26
'Reset', // 0x27
'ISR 0', // 0x28
'ISR 1', // 0x29
'ISR 2', // 0x2A
'ISR 3', // 0x2B
'ISR 4', // 0x2C
'ISR 5', // 0x2D
'ISR 6', // 0x2E
'ISR 7', // 0x2F
'SlotLock task 0 slots ', // 0x30
'SlotLock task 1 slots ', // 0x31
'SlotLock task 2 slots ', // 0x32
'SlotLock task 3 slots ', // 0x33
'SlotLock task 4 slots ', // 0x34
'SlotLock task 5 slots ', // 0x35
'SlotLock task 6 slots ', // 0x36
'SlotLock task 7 slots ', // 0x37
'SlotLock task 8 slots ', // 0x38
'SlotLock task 9 slots ', // 0x39
'SlotLock task 10 slots ', // 0x3A
'SlotLock task 11 slots ', // 0x3B
'SlotLock task 12 slots ', // 0x3C
'SlotLock task 13 slots ', // 0x3D
'SlotLock task 14 slots ', // 0x3E
'SlotLock task 15 slots ', // 0x3F
'SlotUnLock task 0 slots  ', // 0x40
'SlotUnLock task 1 slots  ', // 0x41
'SlotUnLock task 2 slots  ', // 0x42
'SlotUnLock task 3 slots  ', // 0x43
'SlotUnLock task 4 slots  ', // 0x44
'SlotUnLock task 5 slots  ', // 0x45
'SlotUnLock task 6 slots  ', // 0x46
'SlotUnLock task 7 slots  ', // 0x47
'SlotUnLock task 8 slots  ', // 0x48
'SlotUnLock task 9 slots  ', // 0x49
'SlotUnLock task 10 slots  ', // 0x4A
'SlotUnLock task 11 slots  ', // 0x4B
'SlotUnLock task 12 slots  ', // 0x4C
'SlotUnLock task 13 slots  ', // 0x4D
'SlotUnLock task 14 slots  ', // 0x4E
'SlotUnLock task 15 slots  ', // 0x4F
'FireEvent 0', // 0x50
'FireEvent 1', // 0x51
'FireEvent 2', // 0x52
'FireEvent 3', // 0x53
'FireEvent 4', // 0x54
'FireEvent 5', // 0x55
'FireEvent 6', // 0x56
'FireEvent 7', // 0x57
'Reserved 0x58', // 0x58
'Reserved 0x59', // 0x59
'REGISTERLEAK_5A', // 0x5A
'BurnLock', // 0x5B
'BurnFree', // 0x5C
'Reserved 0x5D', // 0x5D
'Reserved 0x5E', // 0x5E
'Reserved 0x5F', // 0x5F
'TaskInit 0', // 0x60
'TaskInit 1', // 0x61
'TaskInit 2', // 0x62
'TaskInit 3', // 0x63
'TaskInit 4', // 0x64
'TaskInit 5', // 0x65
'TaskInit 6', // 0x66
'TaskInit 7', // 0x67
'TaskInit 8', // 0x68
'TaskInit 9', // 0x69
'TaskInit 10', // 0x6A
'TaskInit 11', // 0x6B
'TaskInit 12', // 0x6C
'TaskInit 13', // 0x6D
'TaskInit 14', // 0x6E
'TaskInit 15', // 0x6F
'TaskRelease 0', // 0x70
'TaskRelease 1', // 0x71
'TaskRelease 2', // 0x72
'TaskRelease 3', // 0x73
'TaskRelease 4', // 0x74
'TaskRelease 5', // 0x75
'TaskRelease 6', // 0x76
'TaskRelease 7', // 0x77
'TaskRelease 8', // 0x78
'TaskRelease 9', // 0x79
'TaskRelease 10', // 0x7A
'TaskRelease 11', // 0x7B
'TaskRelease 12', // 0x7C
'TaskRelease 13', // 0x7D
'TaskRelease 14', // 0x7E
'TaskRelease 15', // 0x7F
'APIcallTaskYield', // 0x80
'APIcallTaskDelayFromNow', // 0x81
'APIcallTaskDelayFromWake', // 0x82
'APIcallTaskSuspendMe', // 0x83
'APIcallTaskSleep', // 0x84
'APIcallTaskSleepAll', // 0x85
'Undefined 0x86', // 0x86
'APIcallTaskFeedWatchdog', // 0x87
'APIcallTaskKillWatchdog', // 0x88
'APIcallTaskRestart', // 0x89
'APIcallTaskKillTask', // 0x8A
'APIcallTaskWaitForTasks', // 0x8B
'APIcallTaskSyncRequest', // 0x8C
'APIcallTaskSyncRelease', // 0x8D
'APIcallTaskEnableGlobalInterrupts', // 0x8E
'APIcallTaskDisableGlobalInterrupts', // 0x8F
'APIcallTaskEnableSwitchTask', // 0x90
'APIcallTaskDisableSwitchTask', // 0x91
'APIcallTaskEnterGlobalCritical', // 0x92
'APIcallTaskExitGlobalCritical', // 0x93
'APIcallTaskEnterSwitchCritical', // 0x94
'APIcallTaskExitSwitchCritical', // 0x95
'APIcallTaskWaitForEvent', // 0x96
'APIcallTaskFileOpen', // 0x97
'APIcallTaskFileClose', // 0x98
'APIcallTaskFileFormat', // 0x99
'APIcallTaskFileGetSize', // 0x9A
'APIcallTaskFileSetSize', // 0x9B
'APIcallTaskFileReadByte', // 0x9C
'APIcallTaskFileWriteByte', // 0x9D
'APIcallTaskFileAppendByte', // 0x9E
'APIcallTaskFileReadPipe', // 0x9F
'APIcallTaskFileWritePipe', // 0xA0
'APIcallTaskFileReadBuffer', // 0xA1
'APIcallTaskFileWriteBuffer', // 0xA2
'APIcallUndefined 0xA3', // 0xA3
'APIcallUndefined 0xA4', // 0xA4
'REGISTERLEAK_A5', // 0xA5
'APIcallUndefined 0xA6', // 0xA6
'APIcallUndefined 0xA7', // 0xA7
'APIcallGenSuspend', // 0xA8
'APIcallGenResume', // 0xA9
'APIcallGenSetPriority', // 0xAA
'APIcallGenGetPriority', // 0xAB
'APIcallGenGetTickCount', // 0xAC
'APIcallGenGetLastWakeTime', // 0xAD
'APIcallGenGetTasknameChar', // 0xAE
'APIcallGenFireEventSet', // 0xAF
'APIcallGenReboot', // 0xB0
'APIcallGenLogTask', // 0xB1
'APIcallGenLogOs', // 0xB2
'APIcallGenSyncWaitRelease', // 0xB3
'APIcallGenQueuWrite', // 0xB4
'APIcallGenQueuRead', // 0xB5
'APIcallGenQueuPeek', // 0xB6
'APIcallGenQueuReadable', // 0xB7
'APIcallGenQueuWriteable', // 0xB8
'APIcallGenQueuFull', // 0xB9
'APIcallGenQueuEmpty', // 0xBA
'APIcallGenQueuClear', // 0xBB
'APIcallGenAddtoTickCount', // 0xBC
'APIcallAuxPassFlashString', // 0xBD
'APIcallGenEventCountBlocks', // 0xBE
'APIcallUndefined 0xBF', // 0xBF
'ERRUndefined 0xC0', // 0xC0
'ERRTaskStackUnderMinimum', // 0xC1
'ERRTaskStackSetupTooSmall', // 0xC2
'ERRTaskStackWillOverflow', // 0xC3
'ERRTaskIllegalRegisterUse', // 0xC4
'ERRTaskTakesTooLong', // 0xC5
'ERRTaskNestingOverflowed', // 0xC6
'ERRTaskNestingUnderflowed', // 0xC7
'ERRUndefined 0xC8', // 0xC8
'ERRUndefined 0xC9', // 0xC9
'ERRTaskHoldsNoSlots', // 0xCA
'ERRSlotDoubleUsed', // 0xCB
'ERRQueuOverrun', // 0xCC
'ERRQueuUnderrun', // 0xCD
'ERRTaskPriorityToHigh', // 0xCE
'ERRTaskDelayTooLong', // 0xCF
'ERRTaskDelayTooShort', // 0xD0
'ERRSlotZeroUsed', // 0xD1
'ERRSlotTypeMismatch', // 0xD2
'ERRSlotTwins', // 0xD3
'ERRUndefined 0xD4', // 0xD4
'ERRQueuWriteLimit', // 0xD5
'ERRQueuReadLimit', // 0xD6
'ERRSlotDecreaseFail', // 0xD7
'ERRSlotIncreaseFail', // 0xD8
'ERRFileOpenMode', // 0xD9
'ERRFileInvalidFileNumber', // 0xDA
'ERRFileOutOfRange', // 0xDB
'ERRInsufficientCapabilites', // 0xDC
'ERRUndefined 0xDD', // 0xDD
'ERRUndefined 0xDE', // 0xDE
'ERRUndefined 0xDF', // 0xDF
'FATUndefined 0xE0', // 0xE0
'FATOsStackUnderMinimum', // 0xE1
'FATOsStackOverflowed', // 0xE2
'FATIllegalEvent', // 0xE3
'FATTaskStackOverflowed', // 0xE4
'FATIsrStackOverflowed', // 0xE5
'FATOsTickRateTooHigh', // 0xE6
'FATTaskSubtickOverflow', // 0xE7
'FATTaskIllegalTaskNumber', // 0xE8
'FATSlotStackOverflow', // 0xE9
'FATIllegalCallfromISR', // 0xEA
'FATUndefined 0xEB ', // 0xEB
'FATUndefined 0xEC', // 0xEC
'FATUndefined 0xED', // 0xED
'FATUndefined 0xEE', // 0xEE
'FATUndefined 0xEF', // 0xEF
'UserCatch', // 0xF0
'UserByteMessage', // 0xF1
'UserMessage 0xF2', // 0xF2
'UserMessage 0xF3', // 0xF3
'UserMessage 0xF4', // 0xF4
'UserMessage 0xF5', // 0xF5
'UserMessage 0xF6', // 0xF6
'UserMessage 0xF7', // 0xF7
'UserMessage 0xF8', // 0xF8
'UserMessage 0xF9', // 0xF9
'UserMessage 0xFA', // 0xFA
'UserMessage 0xFB', // 0xFB
'UserMessage 0xFC', // 0xFC
'UserMessage 0xFD', // 0xFD
'UserMessage 0xFE', // 0xFE
'UserMessage 0xFF'); // 0xFF




implementation

end.

