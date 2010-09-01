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
*     Class offers static utility functions for EMail.
*     
*
*/


// INCLUDE FILES

#include "MailLog.h"

#ifdef MAIL_ENABLE_LOGGING
#include <flogger.h>
#include <e32svr.h>
#include <f32file.h>

/// Folder where the log resides
_LIT( KLogFolder, "EMail" );

/// The name of the log file
_LIT( KLogFileName, "EMail.txt" );

/// The format in which the time is formatted in log
_LIT( KLogTimeFormat, "%02d,%06d ;");

/// The length of the string produced by KLogTimeFormat
const TInt KLogTimeFormatLength = 16;

/// How many characters a log line can contain
const TInt KLogLineLength = 256;

#endif // MAIL_ENABLE_LOGGING       

// ================= MEMBER FUNCTIONS =======================

#ifdef MAIL_ENABLE_LOGGING
// ---------------------------------------------------------
// LogHeap() 
//
// ---------------------------------------------------------
void MailLog::LogHandleSize( RFile aFile )
    {       
    _LIT(KLOGHEAP, "MsgMail handle size: %d");
    TInt size(0);
    aFile.Size( size );
    Log(KLOGHEAP, size);    
    }
    

// ---------------------------------------------------------
// LogHeap() 
//
// ---------------------------------------------------------
void MailLog::LogHeap()
    {
    _LIT(KLOGHEAP, "MsgMail heap used: %d @ %d");
    TInt maxHeapSize(0);
    TInt usedHeap = User::Heap().AllocSize(maxHeapSize);
    Log(KLOGHEAP, usedHeap, maxHeapSize);    
    }

// ---------------------------------------------------------
// Log() 
//
// ---------------------------------------------------------
void MailLog::Log( TRefByValue<const TDesC> aText, ... )
    {
    VA_LIST args;
    VA_START( args, aText );

    TBuf<KLogLineLength> buf;
    buf.FormatList( aText, args );

#ifdef MAIL_LOG_TO_FILE
	RFileLogger logger;
	TInt ret = logger.Connect();
	if (ret==KErrNone)
		{
		logger.SetDateAndTime( EFalse,EFalse );
		logger.CreateLog( KLogFolder, KLogFileName, EFileLoggingModeAppend );		
		TBuf<KLogTimeFormatLength> timeStamp;
		TTime now;
		now.HomeTime();
		TDateTime dateTime;
		dateTime = now.DateTime();
		timeStamp.Format( KLogTimeFormat, 
            dateTime.Second(), dateTime.MicroSecond() );
		buf.Insert( 0, timeStamp );

		logger.Write(buf);
		}

	logger.Close();

#else
    RDebug::Print( buf );
#endif // MAIL_LOG_TO_FILE

    VA_END( args );
    }

#endif // MAIL_ENABLE_LOGGING

// End of file
