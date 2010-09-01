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
*     Logging class
*
*/




#ifndef WMLLOG_H
#define WMLLOG_H

/////////////////////////////////////////////////////////////////////////////
// #Defines that control printing of additional information to debug stream.
/////////////////////////////////////////////////////////////////////////////

// INCLUDES

#include "WmlLogDef.h"
#include <e32std.h>

// DEFINES

#ifdef WMLBC_ENABLE_LOGGING
#define LOG(s) WmlLog::Log(_L(s))
#define LOG1(s, v) WmlLog::Log( _L(s), v )
#define LOG2(s, v1, v2) WmlLog::Log( _L(s), v1, v2 )
#else
#define LOG(s)
#define LOG1(s, v)
#define LOG2(s, v1, v2)
#endif // WMLBC_ENABLE_LOGGING

// DATA TYPES


// CLASS DECLARATION

/**
* Class offers logging functionality.
*/
NONSHARABLE_CLASS( WmlLog )
    {

    public:

        /**
        * Log the message.
		* @param aText text to be written to log.
        */
        static void Log( TRefByValue<const TDesC> aText, ... );

    private: // constructor
        
		/// Prohibited default constructor
		WmlLog();

		/// Prohibited copy constructor
		WmlLog( const WmlLog& );
    
		/// Prohibited assignment operator
		WmlLog& operator=( const WmlLog& );
    };

#endif // WMLLOG_H

//  End of File  
