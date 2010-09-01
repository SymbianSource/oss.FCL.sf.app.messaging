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
*     Logging macros for Audio Messaging Application
*
*/



// ========== LOGGING MACROS ===============================

#ifndef AUDIOMESSAGELOGGING_H
#define AUDIOMESSAGELOGGING_H

#ifdef _DEBUG

#ifndef USE_LOGGER
#define USE_LOGGER
#endif 


#include <e32std.h>
#include <e32def.h>
#include <flogger.h>


#ifdef USE_RDEBUG

_LIT(KAMSLogBanner,             "AMS - AudioMessage");
_LIT(KAMSLogEnterFn,            "AMS: >> %S");
_LIT(KAMSLogLeaveFn,            "AMS: << %S");
_LIT(KAMSLogWrite,              "AMS: %S");
_LIT(KAMSLogTag,                "AMS: ");
_LIT(KAMSLogExit,               "AMS: Application exit");
_LIT(KAMSLogTimeFormatString,   "%H:%T:%S:%*C2");

#define AMSLOGGER_CREATE(a)          {RDebug::Print(KAMSLogBanner);}
#define AMSLOGGER_DELETE             {RDebug::Print(KAMSLogExit);}
#define AMSLOGGER_ENTERFN(a)         {_LIT(temp, a); RDebug::Print(KAMSLogEnterFn, &temp);}
#define AMSLOGGER_LEAVEFN(a)         {_LIT(temp, a); RDebug::Print(KAMSLogLeaveFn, &temp);}
#define AMSLOGGER_WRITE(a)           {_LIT(temp, a); RDebug::Print(KAMSLogWrite, &temp);}
#define AMSLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KAMSLogTimeFormatString ); buffer.Insert(0, temp); buffer.Insert(0, KAMSLogTag); RDebug::Print(buffer); }
#define AMSLOGGER_WRITEF             RDebug::Print

#else

_LIT( KAMSLogDir,               "AMS");
_LIT( KAMSLogFile,              "AMS.txt");
_LIT8(KAMSLogBanner,            "AMS - MsgErrorWatcer");
_LIT8(KAMSLogEnterFn,           "AMS: >> %S");
_LIT8(KAMSLogLeaveFn,           "AMS: << %S");
_LIT8(KAMSLogExit,              "AMS: Application exit");
_LIT( KAMSLogTimeFormatString,  "%H:%T:%S:%*C2");

#define AMSLOGGER_CREATE(a)          {FCreate(a);}
#define AMSLOGGER_DELETE             {RFileLogger::Write(KAMSLogDir, KAMSLogFile, EFileLoggingModeAppend, KAMSLogExit);}
#define AMSLOGGER_ENTERFN(a)         {_LIT8(temp, a); RFileLogger::WriteFormat(KAMSLogDir, KAMSLogFile, EFileLoggingModeAppend, KAMSLogEnterFn, &temp);}
#define AMSLOGGER_LEAVEFN(a)         {_LIT8(temp, a); RFileLogger::WriteFormat(KAMSLogDir, KAMSLogFile, EFileLoggingModeAppend, KAMSLogLeaveFn, &temp);}
#define AMSLOGGER_WRITE(a)           {_LIT(temp, a); RFileLogger::Write(KAMSLogDir, KAMSLogFile, EFileLoggingModeAppend, temp);}
#define AMSLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KAMSLogTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KAMSLogDir, KAMSLogFile, EFileLoggingModeAppend, buffer); }
#define AMSLOGGER_WRITEF             FPrint

inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
{
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KAMSLogDir, KAMSLogFile, EFileLoggingModeAppend, aFmt, list);
}

inline void FPrint(const TDesC& aDes)
{
    RFileLogger::WriteFormat(KAMSLogDir, KAMSLogFile, EFileLoggingModeAppend, aDes);
}

inline void FHex(const TUint8* aPtr, TInt aLen)
{
    RFileLogger::HexDump(KAMSLogDir, KAMSLogFile, EFileLoggingModeAppend, 0, 0, aPtr, aLen);
}

inline void FHex(const TDesC8& aDes)
{
    FHex(aDes.Ptr(), aDes.Length());
}

inline void FCreate(RFs& aFs)
{
    TFileName path(_L("c:\\logs\\"));
    path.Append(KAMSLogDir);
    path.Append(_L("\\"));
    aFs.MkDirAll(path);
    RFileLogger::WriteFormat(KAMSLogDir, KAMSLogFile, EFileLoggingModeOverwrite, KAMSLogBanner);
}

#endif // USE_RDEBUG

#else // not _DEBUG

// dummy inline FPrint for AMSLOGGER_WRITEF macro.
inline TInt FPrint(const TRefByValue<const TDesC> /*aFmt*/, ...) { return 0; }

#define AMSLOGGER_CREATE(a)
#define AMSLOGGER_DELETE
#define AMSLOGGER_ENTERFN(a)
#define AMSLOGGER_LEAVEFN(a)
#define AMSLOGGER_WRITE(a)
// if USE_LOGGER not defined this call to FPrint gets optimized away.
#define AMSLOGGER_WRITEF 1 ? 0 : FPrint
#define AMSLOGGER_WRITE_TIMESTAMP(a)

#endif // _DEBUG

#endif // AUDIOMESSAGELOGGING_H

// =========================================================
