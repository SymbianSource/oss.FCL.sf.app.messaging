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
*     Class offers static utility functions for CWmlBMSubItem.
*
*/



// INCLUDE FILES

#include <e32svr.h>
#include "VCalLog.h"


#ifdef VCALBC_ENABLE_LOGGING
#include <flogger.h>

/// Folder where the VCALBC log resides
_LIT( KLogFolder, "vcalbc" );

/// The name of the log file
_LIT( KLogFileName, "vcalbc.txt" );

/// The format in which the time is formatted in log
_LIT( KLogTimeFormat, "%02d.%02d:%02d:%06d ");

/// The length of the string produced by KLogTimeFormat
const TInt KLogTimeFormatLength = 16;

/// How many characters a log line can contain
const TInt KLogLineLength = 256;

#endif // VCALBC_ENABLE_LOGGING       

// ================= MEMBER FUNCTIONS =======================

#ifdef VCALBC_ENABLE_LOGGING
// ---------------------------------------------------------
// Log() 
//
// ---------------------------------------------------------
void VCalLog::Log( TRefByValue<const TDesC> aText, ... )
    {
    VA_LIST args;
    VA_START( args, aText );

    TBuf<256> buf;
    buf.FormatList( aText, args );

#ifdef _DEBUG
#ifdef VCALBC_LOG_TO_FILE
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
            dateTime.Hour(), dateTime.Minute(),
            dateTime.Second(), dateTime.MicroSecond() );
		buf.Insert( 0, timeStamp );

		logger.Write(buf);
		}

	logger.Close();

#else
    RDebug::Print( buf );
#endif // VCALBC_LOG_TO_FILE
#endif // _DEBUG
    VA_END( args );
    }

#endif // VCALBC_ENABLE_LOGGING

// End of file
