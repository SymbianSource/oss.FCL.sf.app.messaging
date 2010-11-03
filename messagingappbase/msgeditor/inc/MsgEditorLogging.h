/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgEditorLogging  declaration
*
*/



#ifndef MSGEDITORLOGGING_H
#define MSGEDITORLOGGING_H

// ========== LOGGING MACROS ===============================

#if defined( _DEBUG ) || defined( USE_LOGGER )

#include <e32std.h>
#include <e32def.h>
#include <eikenv.h>
#include <flogger.h>

#ifdef USE_RDEBUG

_LIT( KMEBLogBanner,             "MEB" );
_LIT( KMEBLogEnterFn,            "MEB: enter --> %S" );
_LIT( KMEBLogLeaveFn,            "MEB: leave <-- %S" );
_LIT( KMEBLogWrite,              "MEB: %S" );
_LIT( KMEBLogTag,                "MEB: " );
_LIT( KMEBLogExit,               "MEB: Application exit" );
_LIT( KMEBLogTimeFormatString,   "%H:%T:%S:%*C2" );


#define MEBLOGGER_CREATE             {RDebug::Print(KMEBLogBanner);}
#define MEBLOGGER_DELETE             {RDebug::Print(KMEBLogExit);}
#define MEBLOGGER_ENTERFN(a)         {_LIT(temp, a); RDebug::Print(KMEBLogEnterFn, &temp);}
#define MEBLOGGER_LEAVEFN(a)         {_LIT(temp, a); RDebug::Print(KMEBLogLeaveFn, &temp);}
#define MEBLOGGER_WRITE(a)           {_LIT(temp, a); RDebug::Print(KMEBLogWrite, &temp);}
#define MEBLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMEBLogTimeFormatString ); buffer.Insert(0, temp); buffer.Insert(0, KMEBLogTag); RDebug::Print(buffer); }
#define MEBLOGGER_WRITEF             RDebug::Print

#else // USE_RDEBUG

_LIT( KMEBLogDir,               "MEB" );
_LIT( KMEBLogFile,              "MEB.txt" );
_LIT8( KMEBLogBanner,           "MEB" );
_LIT8( KMEBLogEnterFn,          "MEB: -> %S" );
_LIT8( KMEBLogLeaveFn,          "MEB: <- %S" );
_LIT8( KMEBLogExit,             "MEB: Application exit" );
_LIT( KMEBLogTimeFormatString,  "%H:%T:%S:%*C2" );

#define MEBLOGGER_CREATE             {FCreate();}
#define MEBLOGGER_DELETE             {RFileLogger::Write(KMEBLogDir, KMEBLogFile, EFileLoggingModeAppend, KMEBLogExit);}
#define MEBLOGGER_ENTERFN(a)         {_LIT8(temp, a); RFileLogger::WriteFormat(KMEBLogDir, KMEBLogFile, EFileLoggingModeAppend, KMEBLogEnterFn, &temp);}
#define MEBLOGGER_LEAVEFN(a)         {_LIT8(temp, a); RFileLogger::WriteFormat(KMEBLogDir, KMEBLogFile, EFileLoggingModeAppend, KMEBLogLeaveFn, &temp);}
#define MEBLOGGER_WRITE(a)           {_LIT(temp, a); RFileLogger::Write(KMEBLogDir, KMEBLogFile, EFileLoggingModeAppend, temp);}
#define MEBLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMEBLogTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KMEBLogDir, KMEBLogFile, EFileLoggingModeAppend, buffer); }
#define MEBLOGGER_WRITEF             FPrint

inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KMEBLogDir, KMEBLogFile, EFileLoggingModeAppend, aFmt, list);
    }

inline void FPrint(const TDesC& aDes)
    {
    RFileLogger::WriteFormat(KMEBLogDir, KMEBLogFile, EFileLoggingModeAppend, aDes);
    }

inline void FHex(const TUint8* aPtr, TInt aLen)
    {
    RFileLogger::HexDump(KMEBLogDir, KMEBLogFile, EFileLoggingModeAppend, 0, 0, aPtr, aLen);
    }

inline void FHex(const TDesC8& aDes)
    {
    FHex(aDes.Ptr(), aDes.Length());
    }

inline void FCreate()
    {
    TFileName path(_L("c:\\logs\\"));
    path.Append(KMEBLogDir);
    path.Append(_L("\\"));
    RFs& fs = CEikonEnv::Static()->FsSession();
    fs.MkDirAll(path);
    RFileLogger::WriteFormat(KMEBLogDir, KMEBLogFile, EFileLoggingModeOverwrite, KMEBLogBanner);
    }

#endif // USE_RDEBUG

#else // not _DEBUG

inline void FPrint(const TRefByValue<const TDesC> /*aFmt*/, ...) {};

#define MEBLOGGER_CREATE
#define MEBLOGGER_DELETE
#define MEBLOGGER_ENTERFN(a)
#define MEBLOGGER_LEAVEFN(a)
#define MEBLOGGER_WRITE(a)
#define MEBLOGGER_WRITEF 1 ? ((void)0) : FPrint
#define MEBLOGGER_WRITE_TIMESTAMP(a)

#endif // _DEBUG

#endif // MSGEDITORLOGGING_H

// End of File
