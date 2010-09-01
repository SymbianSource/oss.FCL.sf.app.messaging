/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Logging macros for MsgErrorWatcher.dll
*
*/



// ========== LOGGING MACROS ===============================

#ifndef MSGERRORWATCHERLOGGING_H
#define MSGERRORWATCHERLOGGING_H

#ifdef _DEBUG

#ifndef USE_LOGGER
#define USE_LOGGER
#endif 


#include <e32std.h>
#include <e32def.h>
#include <flogger.h>


#ifdef USE_RDEBUG

_LIT(KMEWLogBanner,             "MEW - MsgErrorWatcer");
_LIT(KMEWLogEnterFn,            "MEW: >> %S");
_LIT(KMEWLogLeaveFn,            "MEW: << %S");
_LIT(KMEWLogWrite,              "MEW: %S");
_LIT(KMEWLogTag,                "MEW: ");
_LIT(KMEWLogExit,               "MEW: Application exit");
_LIT(KMEWLogTimeFormatString,   "%H:%T:%S:%*C2");

#define MEWLOGGER_CREATE(a)          {RDebug::Print(KMEWLogBanner);}
#define MEWLOGGER_DELETE             {RDebug::Print(KMEWLogExit);}
#define MEWLOGGER_ENTERFN(a)         {_LIT(temp, a); RDebug::Print(KMEWLogEnterFn, &temp);}
#define MEWLOGGER_LEAVEFN(a)         {_LIT(temp, a); RDebug::Print(KMEWLogLeaveFn, &temp);}
#define MEWLOGGER_WRITE(a)           {_LIT(temp, a); RDebug::Print(KMEWLogWrite, &temp);}
#define MEWLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMEWLogTimeFormatString ); buffer.Insert(0, temp); buffer.Insert(0, KMEWLogTag); RDebug::Print(buffer); }
#define MEWLOGGER_WRITEF             RDebug::Print

#else

_LIT( KMEWLogDir,               "MEW");
_LIT( KMEWLogFile,              "MEW.txt");
_LIT8(KMEWLogBanner,            "MEW - MsgErrorWatcer");
_LIT8(KMEWLogEnterFn,           "MEW: >> %S");
_LIT8(KMEWLogLeaveFn,           "MEW: << %S");
_LIT8(KMEWLogExit,              "MEW: Application exit");
_LIT( KMEWLogTimeFormatString,  "%H:%T:%S:%*C2");

#define MEWLOGGER_CREATE(a)          {FCreate(a);}
#define MEWLOGGER_DELETE             {RFileLogger::Write(KMEWLogDir, KMEWLogFile, EFileLoggingModeAppend, KMEWLogExit);}
#define MEWLOGGER_ENTERFN(a)         {_LIT8(temp, a); RFileLogger::WriteFormat(KMEWLogDir, KMEWLogFile, EFileLoggingModeAppend, KMEWLogEnterFn, &temp);}
#define MEWLOGGER_LEAVEFN(a)         {_LIT8(temp, a); RFileLogger::WriteFormat(KMEWLogDir, KMEWLogFile, EFileLoggingModeAppend, KMEWLogLeaveFn, &temp);}
#define MEWLOGGER_WRITE(a)           {_LIT(temp, a); RFileLogger::Write(KMEWLogDir, KMEWLogFile, EFileLoggingModeAppend, temp);}
#define MEWLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMEWLogTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KMEWLogDir, KMEWLogFile, EFileLoggingModeAppend, buffer); }
#define MEWLOGGER_WRITEF             FPrint

inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
{
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KMEWLogDir, KMEWLogFile, EFileLoggingModeAppend, aFmt, list);
}

inline void FPrint(const TDesC& aDes)
{
    RFileLogger::WriteFormat(KMEWLogDir, KMEWLogFile, EFileLoggingModeAppend, aDes);
}

inline void FHex(const TUint8* aPtr, TInt aLen)
{
    RFileLogger::HexDump(KMEWLogDir, KMEWLogFile, EFileLoggingModeAppend, 0, 0, aPtr, aLen);
}

inline void FHex(const TDesC8& aDes)
{
    FHex(aDes.Ptr(), aDes.Length());
}

inline void FCreate(RFs& aFs)
{
    TFileName path(_L("c:\\logs\\"));
    path.Append(KMEWLogDir);
    path.Append(_L("\\"));
    aFs.MkDirAll(path);
    RFileLogger::WriteFormat(KMEWLogDir, KMEWLogFile, EFileLoggingModeOverwrite, KMEWLogBanner);
}

#endif // USE_RDEBUG

#else // not _DEBUG

// dummy inline FPrint for MEWLOGGER_WRITEF macro.
inline TInt FPrint(const TRefByValue<const TDesC> /*aFmt*/, ...) { return 0; }

#define MEWLOGGER_CREATE(a)
#define MEWLOGGER_DELETE
#define MEWLOGGER_ENTERFN(a)
#define MEWLOGGER_LEAVEFN(a)
#define MEWLOGGER_WRITE(a)
// if USE_LOGGER not defined this call to FPrint gets optimized away.
#define MEWLOGGER_WRITEF 1 ? 0 : FPrint
#define MEWLOGGER_WRITE_TIMESTAMP(a)

#endif // _DEBUG

#endif // MSGERRORWATCHERLOGGING_H

// =========================================================
