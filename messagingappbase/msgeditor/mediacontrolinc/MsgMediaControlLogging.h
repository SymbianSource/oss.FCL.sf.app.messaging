/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*     Logging macros for MsgEditorMediaControl.dll
*
*/



// ========== LOGGING MACROS ===============================

#ifndef MSGMEDIACONTROLLOGGING_H
#define MSGMEDIACONTROLLOGGING_H

#ifdef USE_LOGGER

#include <e32std.h>
#include <e32def.h>
#include <flogger.h>
#include <msgasynccontrolobserver.h>

#ifdef USE_RDEBUG

_LIT(KMsgMediaControlLogDir,                "MsgEditor");
_LIT(KMsgMediaControlLogFile,               "MsgMediaControl.txt");
_LIT(KMsgMediaControlLogBanner,             "MsgMediaControl");
_LIT(KMsgMediaControlLogEnterFn,            "MsgMediaControl: >> %S");
_LIT(KMsgMediaControlLogLeaveFn,            "MsgMediaControl: << %S");
_LIT(KMsgMediaControlLogWrite,              "MsgMediaControl: %S");
_LIT(KMsgMediaControlLogTag,                "MsgMediaControl: ");
_LIT(KMsgMediaControlLogExit,               "MsgMediaControl: Application exit");
_LIT(KMsgMediaControlLogTimeFormatString,   "%H:%T:%S:%*C2");

#define MSGMEDIACONTROLLOGGER_CREATE(a)          {RDebug::Print(KMsgMediaControlLogBanner);}
#define MSGMEDIACONTROLLOGGER_DELETE             {RDebug::Print(KMsgMediaControlLogExit);}
#define MSGMEDIACONTROLLOGGER_ENTERFN(a)         {_LIT(temp, a); RDebug::Print(KMsgMediaControlLogEnterFn, &temp);}
#define MSGMEDIACONTROLLOGGER_LEAVEFN(a)         {_LIT(temp, a); RDebug::Print(KMsgMediaControlLogLeaveFn, &temp);}
#define MSGMEDIACONTROLLOGGER_WRITE(a)           {_LIT(temp, a); RDebug::Print(KMsgMediaControlLogWrite, &temp);}
#define MSGMEDIACONTROLLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMsgMediaControlLogTimeFormatString ); buffer.Insert(0, temp); buffer.Insert(0, KMsgMediaControlLogTag); RDebug::Print(buffer); }
#define MSGMEDIACONTROLLOGGER_WRITEF             RDebug::Print

#else

_LIT(KMsgMediaControlLogDir,                "MsgEditor");
_LIT(KMsgMediaControlLogFile,               "MsgMediaControl.txt");
_LIT(KMsgMediaControlLogBanner,             "MsgMediaControl");
_LIT(KMsgMediaControlLogEnterFn,            "MsgMediaControl: >> %S");
_LIT(KMsgMediaControlLogLeaveFn,            "MsgMediaControl: << %S");
_LIT(KMsgMediaControlLogWrite,              "MsgMediaControl: %S");
_LIT(KMsgMediaControlLogTag,                "MsgMediaControl: ");
_LIT(KMsgMediaControlLogExit,               "MsgMediaControl: Application exit");
_LIT(KMsgMediaControlLogTimeFormatString,   "%H:%T:%S:%*C2");

#define MSGMEDIACONTROLLOGGER_CREATE(a)          {FCreate(a);}
#define MSGMEDIACONTROLLOGGER_DELETE             {RFileLogger::Write(KMsgMediaControlLogDir, KMsgMediaControlLogFile, EFileLoggingModeAppend, KMsgMediaControlLogExit);}
#define MSGMEDIACONTROLLOGGER_ENTERFN(a)         {_LIT(temp, a); RFileLogger::WriteFormat(KMsgMediaControlLogDir, KMsgMediaControlLogFile, EFileLoggingModeAppend, KMsgMediaControlLogEnterFn, &temp);}
#define MSGMEDIACONTROLLOGGER_LEAVEFN(a)         {_LIT(temp, a); RFileLogger::WriteFormat(KMsgMediaControlLogDir, KMsgMediaControlLogFile, EFileLoggingModeAppend, KMsgMediaControlLogLeaveFn, &temp);}
#define MSGMEDIACONTROLLOGGER_WRITE(a)           {_LIT(temp, a); RFileLogger::Write(KMsgMediaControlLogDir, KMsgMediaControlLogFile, EFileLoggingModeAppend, temp);}
#define MSGMEDIACONTROLLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMsgMediaControlLogTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KMsgMediaControlLogDir, KMsgMediaControlLogFile, EFileLoggingModeAppend, buffer); }
#define MSGMEDIACONTROLLOGGER_WRITEF             FwPrint
// a=aFmt MUST contain %S, %d and %S in this order
#define MSGMEDIACONTROLLOGGER_WRITEF_CONTROL_ERROR_STATE(a,b,c)  FPrintControlErrorState(a, iMediaControlType, b, c)
// a=aFmt MUST contain %d and %S in this order
#define MSGMEDIACONTROLLOGGER_WRITEF_ERROR_STATE(a,b,c)  FPrintErrorState(a,b,c)
// a=aFmt MUST contain %d, %S in this order
#define MSGMEDIACONTROLLOGGER_WRITEF_CONTROL_STATE(a,b)  FPrintControlState(a,iMediaControlType,b)
#define MSGMEDIACONTROLLOGGER_WRITEF_CONTROL_STATE2(a,b,c)  FPrintControlState(a,b,c)

inline void FwPrint(const TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KMsgMediaControlLogDir, KMsgMediaControlLogFile, EFileLoggingModeAppend, aFmt, list);
    }

inline void FwPrint(const TDesC& aDes)
    {
    RFileLogger::WriteFormat(KMsgMediaControlLogDir, KMsgMediaControlLogFile, EFileLoggingModeAppend, aDes);
    }

inline void FwHex(const TUint8* aPtr, TInt aLen)
    {
    RFileLogger::HexDump(KMsgMediaControlLogDir, KMsgMediaControlLogFile, EFileLoggingModeAppend, 0, 0, aPtr, aLen);
    }

inline void FwHex(const TDesC8& aDes)
    {
    FwHex(aDes.Ptr(), aDes.Length());
    }

inline void FCreate(RFs& aFs)
    {
    TFileName path(_L("c:\\logs\\"));
    path.Append(KMsgMediaControlLogDir);
    path.Append(_L("\\"));
    aFs.MkDirAll(path);
    RFileLogger::WriteFormat(KMsgMediaControlLogDir, KMsgMediaControlLogFile, EFileLoggingModeOverwrite, KMsgMediaControlLogBanner);
    }

inline void StateToDes(TInt aState, TDes& aDes)
    {
    switch ( aState )
        {
        case EMsgAsyncControlStateIdle:
            aDes.Copy(_L("EMsgMediaControlStateIdle"));
            break;
        case EMsgAsyncControlStateOpening:
            aDes.Copy(_L("EMsgMediaControlStateOpening"));
            break;
        case EMsgAsyncControlStateBuffering:
            aDes.Copy(_L("EMsgMediaControlStateBuffering"));
            break;
        case EMsgAsyncControlStateReady:
            aDes.Copy(_L("EMsgMediaControlStateReady"));
            break;
        case EMsgAsyncControlStateAboutToPlay:
            aDes.Copy(_L("EMsgMediaControlStateAboutToPlay"));
            break;
        case EMsgAsyncControlStatePlaying:
            aDes.Copy(_L("EMsgMediaControlStatePlaying"));
            break;
        case EMsgAsyncControlStatePaused:
            aDes.Copy(_L("EMsgMediaControlStatePaused"));
            break;
        case EMsgAsyncControlStateStopped:
            aDes.Copy(_L("EMsgMediaControlStateStopped"));
            break;
        case EMsgAsyncControlStateNoRights:
            aDes.Copy(_L("EMsgMediaControlStateNoRights"));
            break;
        case EMsgAsyncControlStateCorrupt:
            aDes.Copy(_L("EMsgMediaControlStateCorrupt"));
            break;
        default:
            aDes.Copy(_L("???Unknown state???"));
            break;
        }
    }

inline void ControlToDes(TInt aControl, TDes& aDes)
    {
    switch ( aControl )
        {
        case EMsgComponentIdImage:
            aDes.Copy(_L("CMsgImageControl"));
            break;
        case EMsgComponentIdVideo:
            aDes.Copy(_L("CMsgVideoControl"));
            break;
        case EMsgComponentIdAudio:
            aDes.Copy(_L("CMsgAudioControl"));
            break;
        case EMsgComponentIdSvg:
            aDes.Copy(_L("CMsgSvgControl"));
            break;
        default:
            aDes.Copy(_L("???Unknown control???"));
            break;
        }
    }


inline void FPrintControlErrorState(const TRefByValue<const TDesC> aFmt, TInt aControl, TInt aError, TInt aState)
    {
    TBuf<32> control;
    ControlToDes(aControl,control);
    TBuf<64> state;
    StateToDes(aState,state);
    FwPrint(aFmt,&control,aError,&state);
    }

inline void FPrintControlState(const TRefByValue<const TDesC> aFmt, TInt aControl, TInt aState)
    {
    TBuf<32> control;
    ControlToDes(aControl,control);
    TBuf<64> state;
    StateToDes(aState,state);
    FwPrint(aFmt,&control,&state);
    }

inline void FPrintErrorState(const TRefByValue<const TDesC> aFmt, TInt aError, TInt aState)
    {
    TBuf<64> state;
    StateToDes(aState,state);
    FwPrint(aFmt,aError,&state);
    }


#endif // USE_RDEBUG

#else // USE_LOGGER

// dummy inline FPrint for MMSUIUTILSLOGGER_WRITEF macro.
inline TInt FwPrint(const TRefByValue<const TDesC> /*aFmt*/, ...) { return 0; }

#define MSGMEDIACONTROLLOGGER_CREATE(a)
#define MSGMEDIACONTROLLOGGER_DELETE
#define MSGMEDIACONTROLLOGGER_ENTERFN(a)
#define MSGMEDIACONTROLLOGGER_LEAVEFN(a)
#define MSGMEDIACONTROLLOGGER_WRITE(a)
// if USE_LOGGER not defined this call to FPrint gets optimized away.
#define MSGMEDIACONTROLLOGGER_WRITEF 1 ? 0 : FwPrint
#define MSGMEDIACONTROLLOGGER_WRITE_TIMESTAMP(a)
#define MSGMEDIACONTROLLOGGER_WRITEF_CONTROL_ERROR_STATE(a,b,c)
#define MSGMEDIACONTROLLOGGER_WRITEF_ERROR_STATE(a,b,c) 
#define MSGMEDIACONTROLLOGGER_WRITEF_CONTROL_STATE(a,b)

#endif // USE_LOGGER

#endif // MSGMEDIACONTROLLOGGING_H

// =========================================================
