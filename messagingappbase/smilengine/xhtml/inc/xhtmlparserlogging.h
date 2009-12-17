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
* Description:   XHTML Parserlogging macros.
*
*/



#ifndef XHTMLPARSERLOGGING_H
#define XHTMLPARSERLOGGING_H

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

_LIT( KXHTMLLogBanner,            "XHTML " );
_LIT( KXHTMLLogEnterFn,           "XHTML: enter --> %S" );
_LIT( KXHTMLLogLeaveFn,           "XHTML: leave <-- %S" );
_LIT( KMEBLogWrite,             "XHTML: %S" );
_LIT( KMEBLogTag,               "XHTML: " );
_LIT( KXHTMLLogExit,              "XHTML: Application exit" );
_LIT( KXHTMLLogTimeFormatString,  "%H:%T:%S:%*C2" );

#define XHTMLLOG_CREATE             {RDebug::Print(KXHTMLLogBanner);}
#define XHTMLLOG_DELETE             {RDebug::Print(KXHTMLLogExit);}
#define XHTMLLOG_ENTERFN(a)         {_LIT(xhtmllog, a); RDebug::Print(KXHTMLLogEnterFn, &xhtmllog);}
#define XHTMLLOG_LEAVEFN(a)         {_LIT(xhtmllog, a); RDebug::Print(KXHTMLLogLeaveFn, &xhtmllog);}
#define XHTMLLOG_WRITE(a)           {_LIT(xhtmllog, a); RDebug::Print(KXHTMLLogWrite, &xhtmllog);}
#define XHTMLLOG_WRITE_TIMESTAMP(a) {_LIT(xhtmllog, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KXHTMLLogTimeFormatString ); buffer.Insert(0, xhtmllog); buffer.Insert(0, KXHTMLLogTag); RDebug::Print(buffer); }
#define XHTMLLOG_WRITEF             RDebug::Print
#define XHTMLLOG_WRITEF8            RDebug::Print

#else // USE_RDEBUG

_LIT( KXHTMLLogDir,               "XHTML");
_LIT( KXHTMLLogFile,              "XHTML.txt");
_LIT8(KXHTMLLogBanner,            "XHTML ");
_LIT8(KXHTMLLogEnterFn,           "XHTML: -> %S");
_LIT8(KXHTMLLogLeaveFn,           "XHTML: <- %S");
_LIT8(KXHTMLLogExit,              "XHTML: Application exit");
_LIT( KXHTMLLogTimeFormatString,  "%H:%T:%S:%*C2");

#define XHTMLLOG_CREATE             {FCreate();}
#define XHTMLLOG_DELETE             {RFileLogger::Write(KXHTMLLogDir, KXHTMLLogFile, EFileLoggingModeAppend, KXHTMLLogExit);}
#define XHTMLLOG_ENTERFN(a)         {_LIT8(xhtmllog, a); RFileLogger::WriteFormat(KXHTMLLogDir, KXHTMLLogFile, EFileLoggingModeAppend, KXHTMLLogEnterFn, &xhtmllog);}
#define XHTMLLOG_LEAVEFN(a)         {_LIT8(xhtmllog, a); RFileLogger::WriteFormat(KXHTMLLogDir, KXHTMLLogFile, EFileLoggingModeAppend, KXHTMLLogLeaveFn, &xhtmllog);}
#define XHTMLLOG_WRITE(a)           {_LIT(xhtmllog, a); RFileLogger::Write(KXHTMLLogDir, KXHTMLLogFile, EFileLoggingModeAppend, xhtmllog);}
#define XHTMLLOG_WRITE_TIMESTAMP(a) {_LIT(xhtmllog, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KXHTMLLogTimeFormatString ); buffer.Insert(0, xhtmllog); RFileLogger::Write(KXHTMLLogDir, KXHTMLLogFile, EFileLoggingModeAppend, buffer); }
#define XHTMLLOG_WRITEF             FPrint
#define XHTMLLOG_WRITEF8            FPrint8

inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KXHTMLLogDir, KXHTMLLogFile, EFileLoggingModeAppend, aFmt, list);
    }


inline void FPrint8(const TRefByValue<const TDesC8> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KXHTMLLogDir, KXHTMLLogFile, EFileLoggingModeAppend, aFmt, list);
    }


inline void FPrint(const TDesC& aDes)
    {
    RFileLogger::WriteFormat(KXHTMLLogDir, KXHTMLLogFile, EFileLoggingModeAppend, aDes);
    }

inline void FHex(const TUint8* aPtr, TInt aLen)
    {
    RFileLogger::HexDump(KXHTMLLogDir, KXHTMLLogFile, EFileLoggingModeAppend, 0, 0, aPtr, aLen);
    }

inline void FHex(const TDesC8& aDes)
    {
    FHex(aDes.Ptr(), aDes.Length());
    }

inline void FCreate()
    {
    TFileName path(_L("c:\\logs\\"));
    path.Append(KXHTMLLogDir);
    path.Append(_L("\\"));
    RFs& fs = CEikonEnv::Static()->FsSession();
    fs.MkDirAll(path);
    RFileLogger::WriteFormat(KXHTMLLogDir, KXHTMLLogFile, EFileLoggingModeOverwrite, KXHTMLLogBanner);
    }

#endif // USE_RDEBUG

#else // not logging

inline void FPrint(const TRefByValue<const TDesC> /*aFmt*/, ...) {};
inline void FPrint8(const TRefByValue<const TDesC8> /*aFmt*/, ...) {};

#define XHTMLLOG_CREATE
#define XHTMLLOG_DELETE
#define XHTMLLOG_ENTERFN(a)
#define XHTMLLOG_LEAVEFN(a)
#define XHTMLLOG_WRITE(a)
#define XHTMLLOG_WRITEF 1 ? ((void)0) : FPrint
#define XHTMLLOG_WRITEF8 1 ? ((void)0) : FPrint8
#define XHTMLLOG_WRITE_TIMESTAMP(a)

#endif // logging

#endif // XHTMLPARSERLOGGING_H

// End of File
