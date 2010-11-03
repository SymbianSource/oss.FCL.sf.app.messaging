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
* Description: EMN Logging.
*
*/


#ifndef __EMAILNOTIFICATIONLOGGING_H__
#define __EMAILNOTIFICATIONLOGGING_H__


#ifdef _DEBUG
#define LOGGING_ON
#endif

//#ifndef __WINS__
// comment this if file logger is wanted
//#define EMAILNOTLOGGING_RDEBUG

//#endif

#ifdef LOGGING_ON

#include <flogger.h>

const TInt KEMNMajorVersion=0;
const TInt KEMNMinorVersion=0;
const TInt KEMNBuildVersion=1;

const TInt KEMNLoggingTextBuffer = 255;
_LIT(KEMNLogTimeFormatString,    " %H:%T:%S:%*C2 ");
_LIT(KEMNLogDateTimeFormatString,    "%D%M%Y%/0%1%/1%2%/2%3%/3 %H:%T:%S:%*C2 ");
  

#ifdef EMAILNOTLOGGING_RDEBUG
_LIT(KEMNLogTag,                 "EMAILNOT: ");

_LIT(KEMNLogMessageAppBanner,"EMNHandler %d.%d.%d   (EMAILNOT)");
_LIT(KEMNLogEnterFn,         "EMAILNOT: -> %S");
_LIT(KEMNLogLeaveFn,         "EMAILNOT: <- %S");
_LIT(KEMNLogWrite,           "EMAILNOT: %S");
_LIT(KEMNLogExitBanner,      "EMNHandler exit");
#define KEMNLOGGER_WRITE_FORMAT(a,b)   {_LIT(temp, a); TBuf<KEMNLoggingTextBuffer> buf; buf.Format(temp, b); buf.Insert(0, KEMNLogTag); RDebug::Print(buf);}        // CSI: 46 # more readable
#define KEMNLOGGER_WRITE_FORMAT8(a,b)   {_LIT8(temp, a); TBuf<KEMNLoggingTextBuffer> buf; buf.Format(temp, b); buf.Insert(0, KEMNLogTag); RDebug::Print(buf);}      // CSI: 46 # more readable
#define KEMNLOGGER_WRITE_FORMAT2(a,b,c) {_LIT(temp, a); TBuf<KEMNLoggingTextBuffer> buf; buf.Format(temp, b, c); buf.Insert(0, KEMNLogTag); RDebug::Print(buf);}    // CSI: 46 # more readable
#define KEMNLOGGER_WRITE_FORMAT28(a,b,c) {_LIT8(temp, a); TBuf<KEMNLoggingTextBuffer> buf; buf.Format(temp, b, c); buf.Insert(0, KEMNLogTag); RDebug::Print(buf);}  // CSI: 46 # more readable
#define KEMNLOGGER_CREATE              {RDebug::Print(KEMNLogMessageAppBanner,KEMNMajorVersion, KEMNMinorVersion, KEMNBuildVersion);}   // CSI: 46 # more readable
#define KEMNLOGGER_DELETE              {RDebug::Print(KEMNLogExitBanner);}
#define KEMNLOGGER_WRITE(a)            {_LIT(temp, a); RDebug::Print(KEMNLogWrite, &temp);}
#define KEMNLOGGER_WRITE_TEXT(a)       {RDebug::Print(KEMNLogWrite, &a);}
#define KEMNLOGGER_FN1(a)          {_LIT(temp, a); RDebug::Print(KEMNLogEnterFn, &temp);}
#define KEMNLOGGER_FN2(a)          {_LIT(temp, a); RDebug::Print(KEMNLogLeaveFn, &temp);}
#define KEMNLOGGER_WRITE_NUMBER(a)     {TBuf<KEMNLoggingTextBuffer> num; num.Num(a); RDebug::Print(KEMNLogWrite, &num);}        // CSI: 46 # more readable
#define KEMNLOGGER_WRITE_BUFFER(a)     {TBuf<KEMNLoggingTextBuffer> temp; temp.Copy(a); RDebug::Print(KEMNLogWrite, &temp);}    // CSI: 46 # more readable

#else
_LIT(KEMNLogDir,                 "emailnot");
_LIT(KEMNLogFile,                "emailnot.txt");
_LIT8(KEMNLogMessageAppBanner,      "EMNHandler %d.%d.%d   (EMAILNOT)");
_LIT8(KEMNLogEnterFn,               "-> %S");
_LIT8(KEMNLogLeaveFn,               "<- %S");
_LIT8(KEMNLogExit,                  "EMNHandler exit");

#define KEMNLOGGER_CREATE              {RFileLogger::WriteFormat(KEMNLogDir, KEMNLogFile, EFileLoggingModeOverwrite, KEMNLogMessageAppBanner, KEMNMajorVersion, KEMNMinorVersion, KEMNBuildVersion);}   // CSI: 46 # more readable
#define KEMNLOGGER_DELETE              {RFileLogger::WriteFormat(KEMNLogDir, KEMNLogFile, EFileLoggingModeOverwrite, KEMNLogMessageAppBanner, KEMNMajorVersion, KEMNMinorVersion, KEMNBuildVersion);}   // CSI: 46 # more readable
#define KEMNLOGGER_FN1(a)          {_LIT8(temp, a); RFileLogger::WriteFormat(KEMNLogDir, KEMNLogFile, EFileLoggingModeAppend, KEMNLogEnterFn, &temp);}  // CSI: 46 # more readable
#define KEMNLOGGER_FN2(a)          {_LIT8(temp, a); RFileLogger::WriteFormat(KEMNLogDir, KEMNLogFile, EFileLoggingModeAppend, KEMNLogLeaveFn, &temp);}  // CSI: 46 # more readable
#define KEMNLOGGER_WRITE(a)            {_LIT(temp, a); RFileLogger::Write(KEMNLogDir, KEMNLogFile, EFileLoggingModeAppend, temp);}                      // CSI: 46 # more readable
#define KEMNLOGGER_WRITE_FORMAT(a,b)   {_LIT(temp, a); RFileLogger::WriteFormat(KEMNLogDir, KEMNLogFile, EFileLoggingModeAppend, temp, b);}             // CSI: 46 # more readable
#define KEMNLOGGER_WRITE_FORMAT8(a,b)   {_LIT8(temp, a); RFileLogger::WriteFormat(KEMNLogDir, KEMNLogFile, EFileLoggingModeAppend, temp, b);}           // CSI: 46 # more readable
#define KEMNLOGGER_WRITE_FORMAT2(a,b,c)   {_LIT(temp, a); RFileLogger::WriteFormat(KEMNLogDir, KEMNLogFile, EFileLoggingModeAppend, temp, b, c);}       // CSI: 46 # more readable
#define KEMNLOGGER_WRITE_FORMAT28(a,b,c)   {_LIT8(temp, a); RFileLogger::WriteFormat(KEMNLogDir, KEMNLogFile, EFileLoggingModeAppend, temp, b, c);}     // CSI: 46 # more readable
#define KEMNLOGGER_WRITE_TIMESTAMP(a)  {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KEMNLogTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KEMNLogDir, KEMNLogFile, EFileLoggingModeAppend, buffer); }   // CSI: 46 # more readable
#define KEMNLOGGER_WRITE_TEXT(a)       {RFileLogger::Write(KEMNLogDir, KEMNLogFile, EFileLoggingModeAppend, a);}    // CSI: 46 # more readable
#define KEMNLOGGER_WRITE_NUMBER(a)     {TBuf<KEMNLoggingTextBuffer> num; num.Num(a); RFileLogger::Write(KEMNLogDir, KEMNLogFile, EFileLoggingModeAppend, num);}         // CSI: 46 # more readable
#define KEMNLOGGER_WRITE_BUFFER(a)     {TBuf8<KEMNLoggingTextBuffer> temp; temp.Copy(a); RFileLogger::Write(KEMNLogDir, KEMNLogFile, EFileLoggingModeAppend, temp);}    // CSI: 46 # more readable        

#define KEMNLOGGER_WRITE_DATETIME(a, time) {_LIT(temp, a); TBuf<256> buffer; time.FormatL( buffer, KEMNLogDateTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KEMNLogDir, KEMNLogFile, EFileLoggingModeAppend, buffer); }    // CSI: 46 # more readable


#endif

#else

#define KEMNLOGGER_CREATE         
#define KEMNLOGGER_DELETE         
#define KEMNLOGGER_FN1(a)     
#define KEMNLOGGER_FN2(a)     
#define KEMNLOGGER_WRITE(a)           
#define KEMNLOGGER_WRITE_FORMAT(a,b) 
#define KEMNLOGGER_WRITE_FORMAT8(a,b)
#define KEMNLOGGER_WRITE_FORMAT2(a,b,c)
#define KEMNLOGGER_WRITE_FORMAT28(a,b,c)
#define KEMNLOGGER_WRITE_TIMESTAMP(a)
#define KEMNLOGGER_WRITE_TEXT(a)
#define KEMNLOGGER_WRITE_NUMBER(a)
#define KEMNLOGGER_WRITE_BUFFER(a)
#define KEMNLOGGER_WRITE_DATETIME(a, time)
#endif

#endif

    // End of File
