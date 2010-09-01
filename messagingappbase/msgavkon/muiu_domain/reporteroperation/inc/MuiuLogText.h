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
*     Logging definitions
*
*/



#ifndef __MUIULOGTEXT_H__
#define __MUIULOGTEXT_H__

#ifdef _DEBUG

//  INCLUDES
#include <e32std.h>
#include <e32def.h>
#include <flogger.h>

// CONSTANTS
const TInt KMuiuMajorVersion = 4;
const TInt KMuiuMinorVersion = 0;
const TInt KMuiuBuildVersion = 0;


#ifndef __WINS__
    _LIT(KMuiuLogMessageAppBanner,   "Muiu library %d.%d.%d   (MUIU)");
    _LIT(KMuiuLogEnterFn,            "MUIU: -> %S");
    _LIT(KMuiuLogLeaveFn,            "MUIU: <- %S");
    _LIT(KMuiuLogWrite,              "MUIU: %S");
    _LIT(KMuiuLogTag,                "MUIU: ");
    _LIT(KMuiuLogTimeFormatString,   "%H:%T:%S:%*C2");
#define MUIULOGGER_ENTERFN(a)        {_LIT(temp, a); RDebug::Print(KMuiuLogEnterFn, &temp);}
#define MUIULOGGER_LEAVEFN(a)        {_LIT(temp, a); RDebug::Print(KMuiuLogLeaveFn, &temp);}
#define MUIULOGGER_WRITE(a)          {_LIT(temp, a); RDebug::Print(KMuiuLogWrite, &temp);}
#define MUIULOGGER_WRITE_FORMAT(a,b) {_LIT(temp, a); TBuf<256> buf; buf.Format(temp, b); buf.Insert(0, KMuiuLogTag); RDebug::Print(buf);/*RDebug::Print(a, b);*/}
#define MUIULOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMuiuLogTimeFormatString ); buffer.Insert(0, temp); buffer.Insert(0, KMuiuLogTag); RDebug::Print(buffer); }

#else // __WINS__

    _LIT(KMuiuLogDir,                "Mce");
    _LIT(KMuiuLogFile,               "MUIU.txt");
    _LIT8(KMuiuLogMessageAppBanner,  "Muiu library %d.%d.%d");
    _LIT8(KMuiuLogEnterFn,           "-> %S");
    _LIT8(KMuiuLogLeaveFn,           "<- %S");
    _LIT( KMuiuLogTimeFormatString,  "%H:%T:%S:%*C2");
#define MUIULOGGER_ENTERFN(a)        {_LIT8(temp, a); RFileLogger::WriteFormat(KMuiuLogDir, KMuiuLogFile, EFileLoggingModeAppend, KMuiuLogEnterFn, &temp);}
#define MUIULOGGER_LEAVEFN(a)        {_LIT8(temp, a); RFileLogger::WriteFormat(KMuiuLogDir, KMuiuLogFile, EFileLoggingModeAppend, KMuiuLogLeaveFn, &temp);}
#define MUIULOGGER_WRITE(a)          {_LIT(temp, a); RFileLogger::Write(KMuiuLogDir, KMuiuLogFile, EFileLoggingModeAppend, temp);}
#define MUIULOGGER_WRITE_FORMAT(a,b) {_LIT(temp, a); RFileLogger::WriteFormat(KMuiuLogDir, KMuiuLogFile, EFileLoggingModeAppend, temp, b);}
#define MUIULOGGER_WRITE_TIMESTAMP(a) {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMuiuLogTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KMuiuLogDir, KMuiuLogFile, EFileLoggingModeAppend, buffer); }

#endif // __WINS__

#else // _DEBUG

#define MUIULOGGER_ENTERFN(a)
#define MUIULOGGER_LEAVEFN(a)
#define MUIULOGGER_WRITE(a)
#define MUIULOGGER_WRITE_FORMAT(a, b)
#define MUIULOGGER_WRITE_TIMESTAMP(a)

#endif // _DEBUG

#endif // __MUIULOGTEXT_H__

// End of File
