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
*     Logging macros for MmsSettings.dll
*
*/



// ========== LOGGING MACROS ===============================

#ifndef __MMSSETTINGSLOGGING_H
#define __MMSSETTINGSLOGGING_H

#ifdef USE_LOGGER

#include <e32std.h>
#include <e32def.h>
#include <flogger.h>

#ifdef USE_RDEBUG

_LIT(KMMSSETTINGSLogBanner,             "MMSSETTINGS");
_LIT(KMMSSETTINGSLogEnterFn,            "MMSSETTINGS: >> %S");
_LIT(KMMSSETTINGSLogLeaveFn,            "MMSSETTINGS: << %S");
_LIT(KMMSSETTINGSLogWrite,              "MMSSETTINGS: %S");
_LIT(KMMSSETTINGSLogTag,                "MMSSETTINGS: ");
_LIT(KMMSSETTINGSLogExit,               "MMSSETTINGS: Application exit");
_LIT(KMMSSETTINGSLogTimeFormatString,   "%H:%T:%S:%*C2");

#define MMSSETTINGSLOGGER_CREATE(a)          {RDebug::Print(KMMSSETTINGSLogBanner);}
#define MMSSETTINGSLOGGER_DELETE             {RDebug::Print(KMMSSETTINGSLogExit);}
#define MMSSETTINGSLOGGER_ENTERFN(a)         {_LIT(temp, a); RDebug::Print(KMMSSETTINGSLogEnterFn, &temp);}
#define MMSSETTINGSLOGGER_LEAVEFN(a)         {_LIT(temp, a); RDebug::Print(KMMSSETTINGSLogLeaveFn, &temp);}
#define MMSSETTINGSLOGGER_WRITE(a)           {_LIT(temp, a); RDebug::Print(KMMSSETTINGSLogWrite, &temp);}
#define MMSSETTINGSLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMMSSETTINGSLogTimeFormatString ); buffer.Insert(0, temp); buffer.Insert(0, KMMSSETTINGSLogTag); RDebug::Print(buffer); }
#define MMSSETTINGSLOGGER_WRITEF             RDebug::Print

#else

_LIT( KMMSSETTINGSLogDir,               "MMSUI");
_LIT( KMMSSETTINGSLogFile,              "MMSSETTINGS.txt");
_LIT8(KMMSSETTINGSLogBanner,            "MMSSETTINGS");
_LIT8(KMMSSETTINGSLogEnterFn,           "MMSSETTINGS: >> %S");
_LIT8(KMMSSETTINGSLogLeaveFn,           "MMSSETTINGS: << %S");
_LIT8(KMMSSETTINGSLogExit,              "MMSSETTINGS: Application exit");
_LIT( KMMSSETTINGSLogTimeFormatString,  "%H:%T:%S:%*C2");

#define MMSSETTINGSLOGGER_CREATE(a)          {FCreate(a);}
#define MMSSETTINGSLOGGER_DELETE             {RFileLogger::Write(KMMSSETTINGSLogDir, KMMSSETTINGSLogFile, EFileLoggingModeAppend, KMMSSETTINGSLogExit);}
#define MMSSETTINGSLOGGER_ENTERFN(a)         {_LIT8(temp, a); RFileLogger::WriteFormat(KMMSSETTINGSLogDir, KMMSSETTINGSLogFile, EFileLoggingModeAppend, KMMSSETTINGSLogEnterFn, &temp);}
#define MMSSETTINGSLOGGER_LEAVEFN(a)         {_LIT8(temp, a); RFileLogger::WriteFormat(KMMSSETTINGSLogDir, KMMSSETTINGSLogFile, EFileLoggingModeAppend, KMMSSETTINGSLogLeaveFn, &temp);}
#define MMSSETTINGSLOGGER_WRITE(a)           {_LIT(temp, a); RFileLogger::Write(KMMSSETTINGSLogDir, KMMSSETTINGSLogFile, EFileLoggingModeAppend, temp);}
#define MMSSETTINGSLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMMSSETTINGSLogTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KMMSSETTINGSLogDir, KMMSSETTINGSLogFile, EFileLoggingModeAppend, buffer); }
#define MMSSETTINGSLOGGER_WRITEF             FPrint

inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KMMSSETTINGSLogDir, KMMSSETTINGSLogFile, EFileLoggingModeAppend, aFmt, list);
    }

inline void FPrint(const TDesC& aDes)
    {
    RFileLogger::WriteFormat(KMMSSETTINGSLogDir, KMMSSETTINGSLogFile, EFileLoggingModeAppend, aDes);
    }

inline void FHex(const TUint8* aPtr, TInt aLen)
    {
    RFileLogger::HexDump(KMMSSETTINGSLogDir, KMMSSETTINGSLogFile, EFileLoggingModeAppend, 0, 0, aPtr, aLen);
    }

inline void FHex(const TDesC8& aDes)
    {
    FHex(aDes.Ptr(), aDes.Length());
    }

inline void FCreate(RFs& aFs)
    {
    TFileName path(_L("c:\\logs\\"));
    path.Append(KMMSSETTINGSLogDir);
    path.Append(_L("\\"));
    aFs.MkDirAll(path);
    RFileLogger::WriteFormat(KMMSSETTINGSLogDir, KMMSSETTINGSLogFile, EFileLoggingModeOverwrite, KMMSSETTINGSLogBanner);
    }

#endif // USE_RDEBUG

#else // USE_LOGGER

// dummy inline FPrint for MMSSETTINGSLOGGER_WRITEF macro.
inline TInt FPrint(const TRefByValue<const TDesC> /*aFmt*/, ...) { return 0; }

#define MMSSETTINGSLOGGER_CREATE(a)
#define MMSSETTINGSLOGGER_DELETE
#define MMSSETTINGSLOGGER_ENTERFN(a)
#define MMSSETTINGSLOGGER_LEAVEFN(a)
#define MMSSETTINGSLOGGER_WRITE(a)
// if USE_LOGGER not defined this call to FPrint gets optimized away.
#define MMSSETTINGSLOGGER_WRITEF 1 ? 0 : FPrint
#define MMSSETTINGSLOGGER_WRITE_TIMESTAMP(a)

#endif // USE_LOGGER

#endif // __MMSSETTINGSLOGGING_H

// =========================================================
