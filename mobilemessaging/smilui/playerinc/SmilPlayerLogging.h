/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     Contains logging macros for SmilUi. Logging can be enabled from mmp file.
*
*/



// ========== LOGGING MACROS ===============================

#ifndef SMILPLAYERLOGGING_H
#define SMILPLAYERLOGGING_H

#ifdef USE_LOGGER

#include <e32std.h>
#include <e32def.h>
#include <flogger.h>

/*  logging macros usage:

// replace SMILPLAYER with your own TLA (Three Letter Acronyme).

// set logging on in mmp file...
MACRO USE_LOGGER
#ifdef WINS
MACRO USE_RDEBUG
#endif


void CTestAppUi::ConstructL()
{
    // creates log directory and log file on app startup.
    // prints starting banner...
    SMILPLAYERLOGGER_CREATE(iFs);

    // entering to function...
    SMILPLAYERLOGGER_ENTERFN("ConstructL()");

    TInt number = 42;
    TFileName file;
    file = "something" ....

    // print variable number of parameters...
    // Note: use _L macro for format string!
    SMILPLAYERLOGGER_WRITEF(_L("SMILPLAYER: number = %d, file = %S"), number, &file);

    // print just a string (no _L macro here!)
    SMILPLAYERLOGGER_WRITE("Hello world!");

    // leave from function...
    SMILPLAYERLOGGER_LEAVEFN("ConstructL()");
}

CTestAppUi::~CTestAppUi()
{
    // print app exit banner when application exits.
    SMILPLAYERLOGGER_DELETE;
}

*/

#ifdef USE_RDEBUG

_LIT(KSMILPLAYERLogBanner,             "SMILPLAYER");
_LIT(KSMILPLAYERLogEnterFn,            "SMILPLAYER: >> %S");
_LIT(KSMILPLAYERLogLeaveFn,            "SMILPLAYER: << %S");
_LIT(KSMILPLAYERLogWrite,              "SMILPLAYER: %S");
_LIT(KSMILPLAYERLogTag,                "SMILPLAYER: ");
_LIT(KSMILPLAYERLogExit,               "SMILPLAYER: Application exit");
_LIT(KSMILPLAYERLogTimeFormatString,   "%H:%T:%S:%*C2      ");

#define SMILPLAYERLOGGER_CREATE(a)          {RDebug::Print(KSMILPLAYERLogBanner);}
#define SMILPLAYERLOGGER_DELETE             {RDebug::Print(KSMILPLAYERLogExit);}
#define SMILPLAYERLOGGER_ENTERFN(a)         {_LIT(temp, a); RDebug::Print(KSMILPLAYERLogEnterFn, &temp);}
#define SMILPLAYERLOGGER_LEAVEFN(a)         {_LIT(temp, a); RDebug::Print(KSMILPLAYERLogLeaveFn, &temp);}
#define SMILPLAYERLOGGER_WRITE(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KSMILPLAYERLogTimeFormatString );buffer.Append(temp); buffer.Insert(0, KSMILPLAYERLogTag); RDebug::Print(buffer); }
//#define SMILPLAYERLOGGER_WRITE(a)           {_LIT(temp, a); RDebug::Print(KSMILPLAYERLogWrite, &temp);}
#define SMILPLAYERLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KSMILPLAYERLogTimeFormatString ); buffer.Insert(0, temp); buffer.Insert(0, KSMILPLAYERLogTag); RDebug::Print(buffer); }
#define SMILPLAYERLOGGER_WRITEF             RDebug::Print

#else

_LIT( KSMILPLAYERLogDir,               "smilui");
_LIT( KSMILPLAYERLogFile,              "smilui.txt");
_LIT8(KSMILPLAYERLogBanner,            "SMILPLAYER");
_LIT8(KSMILPLAYERLogEnterFn,           "SMILPLAYER: >> %S");
_LIT8(KSMILPLAYERLogLeaveFn,           "SMILPLAYER: << %S");
_LIT8(KSMILPLAYERLogExit,              "SMILPLAYER: Application exit");
_LIT( KSMILPLAYERLogTimeFormatString,  "%H:%T:%S:%*C2      ");

#define SMILPLAYERLOGGER_CREATE(a)          {FCreate(a);}
#define SMILPLAYERLOGGER_DELETE             {RFileLogger::Write(KSMILPLAYERLogDir, KSMILPLAYERLogFile, EFileLoggingModeAppend, KSMILPLAYERLogExit);}
#define SMILPLAYERLOGGER_ENTERFN(a)         {_LIT8(temp, a); RFileLogger::WriteFormat(KSMILPLAYERLogDir, KSMILPLAYERLogFile, EFileLoggingModeAppend, KSMILPLAYERLogEnterFn, &temp);}
#define SMILPLAYERLOGGER_LEAVEFN(a)         {_LIT8(temp, a); RFileLogger::WriteFormat(KSMILPLAYERLogDir, KSMILPLAYERLogFile, EFileLoggingModeAppend, KSMILPLAYERLogLeaveFn, &temp);}
#define SMILPLAYERLOGGER_WRITE(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KSMILPLAYERLogTimeFormatString ); buffer.Append(temp); RFileLogger::Write(KSMILPLAYERLogDir, KSMILPLAYERLogFile, EFileLoggingModeAppend, buffer); }
//#define SMILPLAYERLOGGER_WRITE(a)           {_LIT(temp, a); RFileLogger::Write(KSMILPLAYERLogDir, KSMILPLAYERLogFile, EFileLoggingModeAppend, temp);}
#define SMILPLAYERLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KSMILPLAYERLogTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KSMILPLAYERLogDir, KSMILPLAYERLogFile, EFileLoggingModeAppend, buffer); }
#define SMILPLAYERLOGGER_WRITEF             FPrint

inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
{
    VA_LIST list;
    VA_START(list,aFmt);
    RFileLogger::WriteFormat(KSMILPLAYERLogDir, KSMILPLAYERLogFile, EFileLoggingModeAppend, aFmt, list);
}

inline void FPrint(const TDesC& aDes)
{
    RFileLogger::WriteFormat(KSMILPLAYERLogDir, KSMILPLAYERLogFile, EFileLoggingModeAppend, aDes);
}

inline void FHex(const TUint8* aPtr, TInt aLen)
{
    RFileLogger::HexDump(KSMILPLAYERLogDir, KSMILPLAYERLogFile, EFileLoggingModeAppend, 0, 0, aPtr, aLen);
}

inline void FHex(const TDesC8& aDes)
{
    FHex(aDes.Ptr(), aDes.Length());
}

inline void FCreate(RFs& aFs)
{
    TFileName path(_L("c:\\logs\\"));
    path.Append(KSMILPLAYERLogDir);
    path.Append(_L("\\"));
    aFs.MkDirAll(path);
    RFileLogger::WriteFormat(KSMILPLAYERLogDir, KSMILPLAYERLogFile, EFileLoggingModeOverwrite, KSMILPLAYERLogBanner);
}

#endif // USE_RDEBUG

#else // USE_LOGGER

// dummy inline FPrint for SMILPLAYERLOGGER_WRITEF macro.
inline TInt FPrint(const TRefByValue<const TDesC> /*aFmt*/, ...) { return 0; }

#define SMILPLAYERLOGGER_CREATE(a)
#define SMILPLAYERLOGGER_DELETE
#define SMILPLAYERLOGGER_ENTERFN(a)
#define SMILPLAYERLOGGER_LEAVEFN(a)
#define SMILPLAYERLOGGER_WRITE(a)
// if USE_LOGGER not defined this call to FPrint gets optimized away.
#define SMILPLAYERLOGGER_WRITEF 1 ? 0 : FPrint
#define SMILPLAYERLOGGER_WRITE_TIMESTAMP(a)

#endif // USE_LOGGER

#endif // SMILPLAYERLOGGING_H

// =========================================================
