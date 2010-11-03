/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     Contains logging macros for SmilMediaRenderer.dll. Logging can be enabled from mmp file.
*
*/



// ========== LOGGING MACROS ===============================

#ifndef SMILMEDIALOGGING_H
#define SMILMEDIALOGGING_H

#ifdef USE_LOGGER

#include <e32std.h>
#include <e32def.h>
#include <flogger.h>

#ifdef USE_RDEBUG

_LIT(KSMILUILogBanner,             "SMILUI");
_LIT(KSMILUILogEnterFn,            "SMILUI: >> %S");
_LIT(KSMILUILogLeaveFn,            "SMILUI: << %S");
_LIT(KSMILUILogWrite,              "SMILUI: %S");
_LIT(KSMILUILogTag,                "SMILUI: ");
_LIT(KSMILUILogExit,               "SMILUI: Application exit");
_LIT(KSMILUILogTimeFormatString,   "%H:%T:%S:%*C2      ");

#define SMILUILOGGER_CREATE(a)          {RDebug::Print(KSMILUILogBanner);}
#define SMILUILOGGER_DELETE             {RDebug::Print(KSMILUILogExit);}
#define SMILUILOGGER_ENTERFN(a)         {_LIT(temp, a); RDebug::Print(KSMILUILogEnterFn, &temp);}
#define SMILUILOGGER_LEAVEFN(a)         {_LIT(temp, a); RDebug::Print(KSMILUILogLeaveFn, &temp);}
#define SMILUILOGGER_WRITE(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KSMILUILogTimeFormatString );buffer.Append(temp); buffer.Insert(0, KSMILUILogTag); RDebug::Print(buffer); }
//#define SMILUILOGGER_WRITE(a)           {_LIT(temp, a); RDebug::Print(KSMILUILogWrite, &temp);}
#define SMILUILOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KSMILUILogTimeFormatString ); buffer.Insert(0, temp); buffer.Insert(0, KSMILUILogTag); RDebug::Print(buffer); }
#define SMILUILOGGER_WRITEF             RDebug::Print

#else

_LIT( KSMILUILogDir,               "smilui");
_LIT( KSMILUILogFile,              "smilui.txt");
_LIT8(KSMILUILogBanner,            "SMILUI");
_LIT8(KSMILUILogEnterFn,           "SMILUI: >> %S");
_LIT8(KSMILUILogLeaveFn,           "SMILUI: << %S");
_LIT8(KSMILUILogExit,              "SMILUI: Application exit");
_LIT( KSMILUILogTimeFormatString,  "%H:%T:%S:%*C2      ");

#define SMILUILOGGER_CREATE(a)          {FCreate(a);}
#define SMILUILOGGER_DELETE             {RFileLogger::Write(KSMILUILogDir, KSMILUILogFile, EFileLoggingModeAppend, KSMILUILogExit);}
#define SMILUILOGGER_ENTERFN(a)         {_LIT8(temp, a); RFileLogger::WriteFormat(KSMILUILogDir, KSMILUILogFile, EFileLoggingModeAppend, KSMILUILogEnterFn, &temp);}
#define SMILUILOGGER_LEAVEFN(a)         {_LIT8(temp, a); RFileLogger::WriteFormat(KSMILUILogDir, KSMILUILogFile, EFileLoggingModeAppend, KSMILUILogLeaveFn, &temp);}
#define SMILUILOGGER_WRITE(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KSMILUILogTimeFormatString ); buffer.Append(temp); RFileLogger::Write(KSMILUILogDir, KSMILUILogFile, EFileLoggingModeAppend, buffer); }
//#define SMILUILOGGER_WRITE(a)           {_LIT(temp, a); RFileLogger::Write(KSMILUILogDir, KSMILUILogFile, EFileLoggingModeAppend, temp);}
#define SMILUILOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KSMILUILogTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KSMILUILogDir, KSMILUILogFile, EFileLoggingModeAppend, buffer); }
#define SMILUILOGGER_WRITEF             FPrint

inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
{
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KSMILUILogDir, KSMILUILogFile, EFileLoggingModeAppend, aFmt, list);
}

inline void FPrint(const TDesC& aDes)
{
    RFileLogger::WriteFormat(KSMILUILogDir, KSMILUILogFile, EFileLoggingModeAppend, aDes);
}

inline void FHex(const TUint8* aPtr, TInt aLen)
{
    RFileLogger::HexDump(KSMILUILogDir, KSMILUILogFile, EFileLoggingModeAppend, 0, 0, aPtr, aLen);
}

inline void FHex(const TDesC8& aDes)
{
    FHex(aDes.Ptr(), aDes.Length());
}

inline void FCreate(RFs& aFs)
{
    TFileName path(_L("c:\\logs\\"));
    path.Append(KSMILUILogDir);
    path.Append(_L("\\"));
    aFs.MkDirAll(path);
    RFileLogger::WriteFormat(KSMILUILogDir, KSMILUILogFile, EFileLoggingModeOverwrite, KSMILUILogBanner);
}

#endif // USE_RDEBUG

#else // USE_LOGGER

// dummy inline FPrint for SMILUILOGGER_WRITEF macro.
inline TInt FPrint(const TRefByValue<const TDesC> /*aFmt*/, ...) { return 0; }

#define SMILUILOGGER_CREATE(a)
#define SMILUILOGGER_DELETE
#define SMILUILOGGER_ENTERFN(a)
#define SMILUILOGGER_LEAVEFN(a)
#define SMILUILOGGER_WRITE(a)
// if USE_LOGGER not defined this call to FPrint gets optimized away.
#define SMILUILOGGER_WRITEF 1 ? 0 : FPrint
#define SMILUILOGGER_WRITE_TIMESTAMP(a)

#endif // USE_LOGGER

#endif // SMILMEDIALOGGING_H

// =========================================================
