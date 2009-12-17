/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Unified Message Editor logging macros.
*
*/



#ifndef UNIEDITORLOGGING_H
#define UNIEDITORLOGGING_H

// ========== LOGGING MACROS ===============================

#if defined( _DEBUG ) || defined( USE_LOGGER )

#ifndef USE_LOGGER
#define USE_LOGGER
#endif 

#include <e32std.h>
#include <e32def.h>
#include <eikenv.h>
#include <flogger.h>

#ifdef USE_RDEBUG

_LIT( KUNILogBanner,            "UNI " );
_LIT( KUNILogEnterFn,           "UNI: enter --> %S" );
_LIT( KUNILogLeaveFn,           "UNI: leave <-- %S" );
_LIT( KMEBLogWrite,             "UNI: %S" );
_LIT( KMEBLogTag,               "UNI: " );
_LIT( KUNILogExit,              "UNI: Application exit" );
_LIT( KUNILogTimeFormatString,  "%H:%T:%S:%*C2" );

#define UNILOGGER_CREATE             {RDebug::Print(KUNILogBanner);}
#define UNILOGGER_DELETE             {RDebug::Print(KUNILogExit);}
#define UNILOGGER_ENTERFN(a)         {_LIT(unilogger, a); RDebug::Print(KUNILogEnterFn, &unilogger);}
#define UNILOGGER_LEAVEFN(a)         {_LIT(unilogger, a); RDebug::Print(KUNILogLeaveFn, &unilogger);}
#define UNILOGGER_WRITE(a)           {_LIT(unilogger, a); RDebug::Print(KUNILogWrite, &unilogger);}
#define UNILOGGER_WRITE_TIMESTAMP(a) {_LIT(unilogger, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KUNILogTimeFormatString ); buffer.Insert(0, unilogger); buffer.Insert(0, KUNILogTag); RDebug::Print(buffer); }
#define UNILOGGER_WRITEF             RDebug::Print
#define UNILOGGER_WRITEF8            RDebug::Print

#else // USE_RDEBUG

_LIT( KUNILogDir,               "UNI");
_LIT( KUNILogFile,              "UNI.txt");
_LIT8(KUNILogBanner,            "UNI ");
_LIT8(KUNILogEnterFn,           "UNI: -> %S");
_LIT8(KUNILogLeaveFn,           "UNI: <- %S");
_LIT8(KUNILogExit,              "UNI: Application exit");
_LIT( KUNILogTimeFormatString,  "%H:%T:%S:%*C2");

#define UNILOGGER_CREATE             {FCreate();}
#define UNILOGGER_DELETE             {RFileLogger::Write(KUNILogDir, KUNILogFile, EFileLoggingModeAppend, KUNILogExit);}
#define UNILOGGER_ENTERFN(a)         {_LIT8(unilogger, a); RFileLogger::WriteFormat(KUNILogDir, KUNILogFile, EFileLoggingModeAppend, KUNILogEnterFn, &unilogger);}
#define UNILOGGER_LEAVEFN(a)         {_LIT8(unilogger, a); RFileLogger::WriteFormat(KUNILogDir, KUNILogFile, EFileLoggingModeAppend, KUNILogLeaveFn, &unilogger);}
#define UNILOGGER_WRITE(a)           {_LIT(unilogger, a); RFileLogger::Write(KUNILogDir, KUNILogFile, EFileLoggingModeAppend, unilogger);}
#define UNILOGGER_WRITE_TIMESTAMP(a) {_LIT(unilogger, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KUNILogTimeFormatString ); buffer.Insert(0, unilogger); RFileLogger::Write(KUNILogDir, KUNILogFile, EFileLoggingModeAppend, buffer); }
#define UNILOGGER_WRITEF             FPrint
#define UNILOGGER_WRITEF8            FPrint8

inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KUNILogDir, KUNILogFile, EFileLoggingModeAppend, aFmt, list);
    }


inline void FPrint8(const TRefByValue<const TDesC8> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KUNILogDir, KUNILogFile, EFileLoggingModeAppend, aFmt, list);
    }


inline void FPrint(const TDesC& aDes)
    {
    RFileLogger::WriteFormat(KUNILogDir, KUNILogFile, EFileLoggingModeAppend, aDes);
    }

inline void FHex(const TUint8* aPtr, TInt aLen)
    {
    RFileLogger::HexDump(KUNILogDir, KUNILogFile, EFileLoggingModeAppend, 0, 0, aPtr, aLen);
    }

inline void FHex(const TDesC8& aDes)
    {
    FHex(aDes.Ptr(), aDes.Length());
    }

inline void FCreate()
    {
    TFileName path(_L("c:\\logs\\"));
    path.Append(KUNILogDir);
    path.Append(_L("\\"));
    RFs& fs = CEikonEnv::Static()->FsSession();
    fs.MkDirAll(path);
    RFileLogger::WriteFormat(KUNILogDir, KUNILogFile, EFileLoggingModeOverwrite, KUNILogBanner);
    }

#endif // USE_RDEBUG

#else // not logging

inline void FPrint(const TRefByValue<const TDesC> /*aFmt*/, ...) {};
inline void FPrint8(const TRefByValue<const TDesC8> /*aFmt*/, ...) {};

#define UNILOGGER_CREATE
#define UNILOGGER_DELETE
#define UNILOGGER_ENTERFN(a)
#define UNILOGGER_LEAVEFN(a)
#define UNILOGGER_WRITE(a)
#define UNILOGGER_WRITEF 1 ? ((void)0) : FPrint
#define UNILOGGER_WRITEF8 1 ? ((void)0) : FPrint8
#define UNILOGGER_WRITE_TIMESTAMP(a)

#endif // logging

#endif // UNIEDITORLOGGING_H

// End of File
