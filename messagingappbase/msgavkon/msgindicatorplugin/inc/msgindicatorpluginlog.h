/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*     Logging defines for Msgindicatorplugin application
*
*/



#if !defined(__MGSPLUGINLOG_H__)
#define __MGSPLUGINLOG_H__

#if defined ( _DEBUG )

//OFFLINE_TESTING 
//#define OFFLINE_TESTING

// Offline id's
// 1 = Envelope
// 2 = Email
// 4 = Fax  
// 5 = Other message
// 7 = Voicemail general, 
// 8 = Voicemail Line1 
// 9 = Voicemail Line2 
// 10 = Voicemail Both Lines 
// 14 = Outbox
// 30 = Remotemailbox

const TInt KOffLineId = 7;
//OFFLINE_TESTING 

#include <flogger.h>

const TInt KMsgPluginMajorVersion=4;
const TInt KMsgPluginMinorVersion=0;
const TInt KMsgPluginBuildVersion=0;


_LIT(KMsgIndicatorPluginLogDir,                "Mce");
_LIT(KMsgIndicatorPluginLogFile,               "MSGPLUGIN.txt");
_LIT8(KMsgIndicatorPluginLogMessageAppBanner,  "MSG Indicator Plugin %d.%d.%d");
_LIT8(KMsgIndicatorPluginLogExit,              "MSG Indicator Plugin exit");

#define MSGPLUGINLOGGER_CREATE            {RFileLogger::WriteFormat(KMsgIndicatorPluginLogDir, KMsgIndicatorPluginLogFile, EFileLoggingModeOverwrite, KMsgIndicatorPluginLogMessageAppBanner, KMsgPluginMajorVersion, KMsgPluginMinorVersion, KMsgPluginBuildVersion);}
#define MSGPLUGINLOGGER_DELETE            {RFileLogger::Write(KMsgIndicatorPluginLogDir, KMsgIndicatorPluginLogFile, EFileLoggingModeAppend, KMsgIndicatorPluginLogExit);}
#define MSGPLUGINLOGGER_WRITE(a)          {_LIT(temp, a); RFileLogger::Write(KMsgIndicatorPluginLogDir, KMsgIndicatorPluginLogFile, EFileLoggingModeAppend, temp);}
#define MSGPLUGINLOGGER_WRITE_FORMAT(a,b) {_LIT(temp, a); RFileLogger::WriteFormat(KMsgIndicatorPluginLogDir, KMsgIndicatorPluginLogFile, EFileLoggingModeAppend, temp, b);}


#else // _DEBUG

#define MSGPLUGINLOGGER_CREATE
#define MSGPLUGINLOGGER_DELETE
#define MSGPLUGINLOGGER_WRITE(a)
#define MSGPLUGINLOGGER_WRITE_FORMAT(a, b)

#endif // _DEBUG

#endif // __MGSPLUGINLOG_H__

    // End of File
