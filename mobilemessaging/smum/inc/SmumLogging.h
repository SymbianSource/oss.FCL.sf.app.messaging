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



#if !defined(__SMUMLOGGING_H__)
#define __SMUMLOGGING_H__

#include <flogger.h>

//
#ifdef USE_LOGGER
//
#ifdef USE_RDEBUG
// SMUM RDEbug Logging
    _LIT(KSmumLogEnterFn,          	"SMUM: -> %S");
    _LIT(KSmumLogLeaveFn,          	"SMUM: <- %S");
    _LIT(KSmumLogWrite,            	"SMUM: %S");
    _LIT(KSmumLogTimeFormatString,	"%H:%T:%S:%*C2");
#define SMUMLOGGER_ENTERFN(a)        {_LIT(temp, a); RDebug::Print(KSmumLogEnterFn, &temp);}
#define SMUMLOGGER_LEAVEFN(a)        {_LIT(temp, a); RDebug::Print(KSmumLogLeaveFn, &temp);} 
#define SMUMLOGGER_WRITE(a)          {_LIT(temp, a); RDebug::Print(KSmumLogWrite, &temp);}
#define SMUMLOGGER_WRITE_FORMAT(a,b) {_LIT(temp, a); TBuf<256> buf; buf.Format(temp, b); RDebug::Print(buf);}
#define SMUMLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KSmumLogTimeFormatString ); buffer.Insert(0, temp); RDebug::Print(buffer); }

#else
//Use RFileLogger
    _LIT(KSmumLogDir,               "Sms");
    _LIT(KSmumLogFile,              "Smum.txt");
    _LIT8(KSmumLogEnterFn,          "-> %S");
    _LIT8(KSmumLogLeaveFn,          "<- %S");
    _LIT( KSmumLogTimeFormatString, "%H:%T:%S:%*C2");
#define SMUMLOGGER_ENTERFN(a)        {_LIT8(temp, a); RFileLogger::WriteFormat(KSmumLogDir, KSmumLogFile, EFileLoggingModeAppend, KSmumLogEnterFn, &temp);}
#define SMUMLOGGER_LEAVEFN(a)        {_LIT8(temp, a); RFileLogger::WriteFormat(KSmumLogDir, KSmumLogFile, EFileLoggingModeAppend, KSmumLogLeaveFn, &temp);}
#define SMUMLOGGER_WRITE(a)          {_LIT(temp, a); RFileLogger::Write(KSmumLogDir, KSmumLogFile, EFileLoggingModeAppend, temp);}
#define SMUMLOGGER_WRITE_FORMAT(a,b) {_LIT(temp, a); RFileLogger::WriteFormat(KSmumLogDir, KSmumLogFile, EFileLoggingModeAppend, temp, b);}
#define SMUMLOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMceLogTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KSmumLogDir, KSmumLogFile, EFileLoggingModeAppend, buffer); }

#endif //USE_RDEBUG
#else // no logger

#define SMUMLOGGER_ENTERFN(a)
#define SMUMLOGGER_LEAVEFN(a)
#define SMUMLOGGER_WRITE(a)
#define SMUMLOGGER_WRITE_FORMAT(a, b)
#define SMUMLOGGER_WRITE_TIMESTAMP(a)

#endif // USE_LOGGER
#endif // __SMUMLOGGING_H__

    // End of File
