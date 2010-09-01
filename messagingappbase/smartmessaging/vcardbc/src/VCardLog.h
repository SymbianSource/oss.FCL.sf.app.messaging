/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     Logging class
*
*/




#ifndef VCARDLOG_H
#define VCARDLOG_H

/////////////////////////////////////////////////////////////////////////////
// #Defines that control printing of additional information to debug stream.
/////////////////////////////////////////////////////////////////////////////

// INCLUDES

#include "VCardLogDef.h"
#include <e32std.h>

// DEFINES

#ifdef VCARDBC_ENABLE_LOGGING
#define LOG(s) VCardLog::Log(_L(s))
#define LOG1(s, v) VCardLog::Log( _L(s), v )
#define LOG2(s, v1, v2) VCardLog::Log( _L(s), v1, v2 )
#else
#define LOG(s)
#define LOG1(s, v)
#define LOG2(s, v1, v2)
#endif // VCARDBC_ENABLE_LOGGING

// DATA TYPES


// CLASS DECLARATION

/**
* Class offers logging functionality.
*/
NONSHARABLE_CLASS( VCardLog )
    {

    public:

        /**
        * Log the message.
		* @param aText text to be written to log.
        */
        static void Log( TRefByValue<const TDesC> aText, ... );

    private: // constructor

		/// Prohibited default constructor
		VCardLog();

		/// Prohibited copy constructor
		VCardLog( const VCardLog& );

		/// Prohibited assignment operator
		VCardLog& operator=( const VCardLog& );
    };

#endif // vCardLog_H

//  End of File
