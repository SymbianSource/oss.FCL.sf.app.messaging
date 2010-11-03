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
* Description: Class for more comprehensive logging features
*
*/


#ifndef __ALWAYSONLINEEMAILPLUGINLOGGING_H__
#define __ALWAYSONLINEEMAILPLUGINLOGGING_H__

#include "AlwaysOnlineEmailLoggingTools.h"

#ifdef _DEBUG
#define AOEMAILLOGGING_ON
#endif

//#ifndef __WINS__
// comment this if file logger is wanted
// #define AOEMAILLOGGING_RDEBUG

//#endif

#ifdef AOEMAILLOGGING_ON

#include <e32debug.h>
#include <flogger.h>

const TInt KAOEmailMajorVersion=0;
const TInt KAOEmailMinorVersion=0;
const TInt KAOEmailBuildVersion=1;

const TInt KAOEmailLoggingTextBuffer = 255;
_LIT(KAOEmailLogTimeFormatString,    " %H:%T:%S:%*C2 ");
_LIT(KAOEmailLogDateTimeFormatString,    "%D%M%Y%/0%1%/1%2%/2%3%/3 %H:%T:%S:%*C2 ");
  

#ifdef AOEMAILLOGGING_RDEBUG
_LIT(KAOEmailLogTag,                 "AOEMAIL: ");

_LIT(KAOEmailLogMessageAppBanner,"AlwaysOnlineEmailPlugin %d.%d.%d   (AOEMAIL)");
_LIT(KAOEmailLogEnterFn,         "AOEMAIL: -> %S");
_LIT(KAOEmailLogLeaveFn,         "AOEMAIL: <- %S");
_LIT(KAOEmailLogWrite,           "AOEMAIL: %S");
_LIT(KAOEmailLogExitBanner,      "AlwaysOnlineEmailPlugin exit");
#define KAOEMAIL_LOGGER_WRITE_FORMAT(a,b)   {_LIT(temp, a); TBuf<KAOEmailLoggingTextBuffer> buf; buf.Format(temp, b); buf.Insert(0, KAOEmailLogTag); RDebug::Print(buf);} // CSI: 46 # 
#define KAOEMAIL_LOGGER_WRITE_FORMAT2(a,b,c) {_LIT(temp, a); TBuf<KAOEmailLoggingTextBuffer> buf; buf.Format(temp, b, c); buf.Insert(0, KAOEmailLogTag); RDebug::Print(buf);} // CSI: 46 # 
#define KAOEMAIL_LOGGER_CREATE              {RDebug::Print(KAOEmailLogMessageAppBanner,KAOEmailMajorVersion, KAOEmailMinorVersion, KAOEmailBuildVersion);} // CSI: 46 # 
#define KAOEMAIL_LOGGER_DELETE              {RDebug::Print(KAOEmailLogExitBanner);}
#define KAOEMAIL_LOGGER_WRITE(a)            {_LIT(temp, a); RDebug::Print(KAOEmailLogWrite, &temp);} // CSI: 46 # 
#define KAOEMAIL_LOGGER_WRITE_TEXT(a)       {RDebug::Print(KAOEmailLogWrite, &a);}
#define KAOEMAIL_LOGGER_FN1(a)          {_LIT(temp, a); RDebug::Print(KAOEmailLogEnterFn, &temp);}
#define KAOEMAIL_LOGGER_FN2(a)          {_LIT(temp, a); RDebug::Print(KAOEmailLogLeaveFn, &temp);}
#define KAOEMAIL_LOGGER_WRITE_NUMBER(a)     {TBuf<KAOEmailLoggingTextBuffer> num; num.Num(a); RDebug::Print(KAOEmailLogWrite, &num);} // CSI: 46 # 
#define KAOEMAIL_LOGGER_WRITE_BUFFER(a)     {TBuf<KAOEmailLoggingTextBuffer> temp; temp.Copy(a); RDebug::Print(KAOEmailLogWrite, &temp);} // CSI: 46 # 
#define KAOEMAIL_LOGGER_WRITE_DATETIME(a, time)

#else
_LIT(KAOEmailLogDir,                 "AOEmail");
_LIT(KAOEmailLogFile,                "AOEmail.txt");
_LIT8(KAOEmailLogMessageAppBanner,      "AlwaysOnlineEmailPlugin %d.%d.%d   (AOEMAIL)");
_LIT8(KAOEmailLogEnterFn,               "-> %S");
_LIT8(KAOEmailLogLeaveFn,               "<- %S");
_LIT8(KAOEmailLogExit,                  "AlwaysOnlineEmailPlugin exit");

#define KAOEMAIL_LOGGER_CREATE              {RFileLogger::WriteFormat(KAOEmailLogDir, KAOEmailLogFile, EFileLoggingModeOverwrite, KAOEmailLogMessageAppBanner, KAOEmailMajorVersion, KAOEmailMinorVersion, KAOEmailBuildVersion);} // CSI: 46 # 
#define KAOEMAIL_LOGGER_DELETE              {RFileLogger::WriteFormat(KAOEmailLogDir, KAOEmailLogFile, EFileLoggingModeOverwrite, KAOEmailLogMessageAppBanner, KAOEmailMajorVersion, KAOEmailMinorVersion, KAOEmailBuildVersion);} // CSI: 46 # 
#define KAOEMAIL_LOGGER_FN1(a)          {_LIT8(temp, a); RFileLogger::WriteFormat(KAOEmailLogDir, KAOEmailLogFile, EFileLoggingModeAppend, KAOEmailLogEnterFn, &temp);} // CSI: 46 # 
#define KAOEMAIL_LOGGER_FN2(a)          {_LIT8(temp, a); RFileLogger::WriteFormat(KAOEmailLogDir, KAOEmailLogFile, EFileLoggingModeAppend, KAOEmailLogLeaveFn, &temp);} // CSI: 46 # 
#define KAOEMAIL_LOGGER_WRITE(a)            {_LIT(temp, a); RFileLogger::Write(KAOEmailLogDir, KAOEmailLogFile, EFileLoggingModeAppend, temp);} // CSI: 46 # 
#define KAOEMAIL_LOGGER_WRITE_FORMAT(a,b)   {_LIT(temp, a); RFileLogger::WriteFormat(KAOEmailLogDir, KAOEmailLogFile, EFileLoggingModeAppend, temp, b);} // CSI: 46 # 
#define KAOEMAIL_LOGGER_WRITE_FORMAT2(a,b,c)   {_LIT(temp, a); RFileLogger::WriteFormat(KAOEmailLogDir, KAOEmailLogFile, EFileLoggingModeAppend, temp, b, c);} // CSI: 46 # 
#define KAOEMAIL_LOGGER_WRITE_TIMESTAMP(a)  {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KAOEmailLogTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KAOEmailLogDir, KAOEmailLogFile, EFileLoggingModeAppend, buffer); } // CSI: 46 # 
#define KAOEMAIL_LOGGER_WRITE_TEXT(a)       {RFileLogger::Write(KAOEmailLogDir, KAOEmailLogFile, EFileLoggingModeAppend, a);} // CSI: 46 # 
#define KAOEMAIL_LOGGER_WRITE_NUMBER(a)     {TBuf<KAOEmailLoggingTextBuffer> num; num.Num(a); RFileLogger::Write(KAOEmailLogDir, KAOEmailLogFile, EFileLoggingModeAppend, num);} // CSI: 46 # 
#define KAOEMAIL_LOGGER_WRITE_BUFFER(a)     {TBuf8<KAOEmailLoggingTextBuffer> temp; temp.Copy(a); RFileLogger::Write(KAOEmailLogDir, KAOEmailLogFile, EFileLoggingModeAppend, temp);} // CSI: 46 # 

#define KAOEMAIL_LOGGER_WRITE_DATETIME(a, time) {_LIT(temp, a); TBuf<256> buffer; time.FormatL( buffer, KAOEmailLogDateTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KAOEmailLogDir, KAOEmailLogFile, EFileLoggingModeAppend, buffer); } // CSI: 46 # 


#endif

#else

#define KAOEMAIL_LOGGER_DEFINE             
#define KAOEMAIL_LOGGER_CREATE         
#define KAOEMAIL_LOGGER_DELETE         
#define KAOEMAIL_LOGGER_FN1(a)     
#define KAOEMAIL_LOGGER_FN2(a)     
#define KAOEMAIL_LOGGER_WRITE(a)           
#define KAOEMAIL_LOGGER_WRITE_FORMAT(a, b) 
#define KAOEMAIL_LOGGER_WRITE_FORMAT2(a,b,c)
#define KAOEMAIL_LOGGER_WRITE_TEXT(a)
#define KAOEMAIL_LOGGER_WRITE_NUMBER(a)
#define KAOEMAIL_LOGGER_WRITE_BUFFER(a)
#define KAOEMAIL_LOGGER_WRITE_DATETIME(a, time)
#endif // _MCE_LOGGING_

#endif // __LOGTEXT_H__

    // End of File
