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
*     Logging defines for Mce application
*
*/



#include <e32debug.h>

#if !defined(__MCELOGTEXT_H__)
#define __MCELOGTEXT_H__

#if defined ( _DEBUG )

#include <flogger.h>

const TInt KMceMajorVersion=4;
const TInt KMceMinorVersion=0;
const TInt KMceBuildVersion=0;


#if !defined (__WINS__)
    _LIT(KMceLogMessageAppBanner,   "Message Application %d.%d.%d   (MCE)");
    _LIT(KMceLogEnterFn,            "MCE: -> %S");
    _LIT(KMceLogLeaveFn,            "MCE: <- %S");
    _LIT(KMceLogWrite,              "MCE: %S");
    _LIT(KMceLogTag,                "MCE: ");
    _LIT(KMceLogExit,               "MCE: Application exit");
    _LIT(KMceLogTimeFormatString,   "%H:%T:%S:%*C2");
#define MCELOGGER_WRITE_FORMAT(a,b) {_LIT(temp, a); TBuf<256> buf; buf.Format(temp, b); buf.Insert(0, KMceLogTag); RDebug::Print(buf);}
#define MCELOGGER_WRITE_FORMAT2(a,b, c) {_LIT(temp, a); TBuf<256> buf; buf.Format(temp, b, c); buf.Insert(0, KMceLogTag); RDebug::Print(buf);}
#define MCELOGGER_CREATE            {RDebug::Print(KMceLogMessageAppBanner,KMceMajorVersion, KMceMinorVersion, KMceBuildVersion);}
#define MCELOGGER_DELETE            {RDebug::Print(KMceLogExit);}
#define MCELOGGER_WRITE(a)          {_LIT(temp, a); RDebug::Print(KMceLogWrite, &temp);}
#define MCELOGGER_ENTERFN(a)        {_LIT(temp, a); RDebug::Print(KMceLogEnterFn, &temp);}
#define MCELOGGER_LEAVEFN(a)        {_LIT(temp, a); RDebug::Print(KMceLogLeaveFn, &temp);}
#define MCELOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMceLogTimeFormatString ); buffer.Insert(0, temp); buffer.Insert(0, KMceLogTag); RDebug::Print(buffer); }

#else

    _LIT(KMceLogDir,                "Mce");
    _LIT(KMceLogFile,               "MCE.txt");
    _LIT8(KMceLogMessageAppBanner,  "Message Application %d.%d.%d");
    _LIT8(KMceLogEnterFn,           "-> %S");
    _LIT8(KMceLogLeaveFn,           "<- %S");
    _LIT8(KMceLogExit,              "Message Application exit");
    _LIT( KMceLogTimeFormatString,  "%H:%T:%S:%*C2");
#define MCELOGGER_CREATE            {RFileLogger::WriteFormat(KMceLogDir, KMceLogFile, EFileLoggingModeOverwrite, KMceLogMessageAppBanner, KMceMajorVersion, KMceMinorVersion, KMceBuildVersion);}
#define MCELOGGER_DELETE            {RFileLogger::Write(KMceLogDir, KMceLogFile, EFileLoggingModeAppend, KMceLogExit);}
#define MCELOGGER_ENTERFN(a)        {_LIT8(temp, a); RFileLogger::WriteFormat(KMceLogDir, KMceLogFile, EFileLoggingModeAppend, KMceLogEnterFn, &temp);}
#define MCELOGGER_LEAVEFN(a)        {_LIT8(temp, a); RFileLogger::WriteFormat(KMceLogDir, KMceLogFile, EFileLoggingModeAppend, KMceLogLeaveFn, &temp);}
#define MCELOGGER_WRITE(a)          {_LIT(temp, a); RFileLogger::Write(KMceLogDir, KMceLogFile, EFileLoggingModeAppend, temp);}
#define MCELOGGER_WRITE_FORMAT(a,b) {_LIT(temp, a); RFileLogger::WriteFormat(KMceLogDir, KMceLogFile, EFileLoggingModeAppend, temp, b);}
#define MCELOGGER_WRITE_FORMAT2(a,b, c) {_LIT(temp, a); RFileLogger::WriteFormat(KMceLogDir, KMceLogFile, EFileLoggingModeAppend, temp, b, c);}
#define MCELOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMceLogTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KMceLogDir, KMceLogFile, EFileLoggingModeAppend, buffer); }

#endif // _WINS

#else // _DEBUG

#define MCELOGGER_CREATE
#define MCELOGGER_DELETE
#define MCELOGGER_ENTERFN(a)
#define MCELOGGER_LEAVEFN(a)
#define MCELOGGER_WRITE(a)
#define MCELOGGER_WRITE_FORMAT(a, b)
#define MCELOGGER_WRITE_FORMAT2(a,b, c)
#define MCELOGGER_WRITE_TIMESTAMP(a)

#endif // _DEBUG

#endif // __MCELOGTEXT_H__

    // End of File
