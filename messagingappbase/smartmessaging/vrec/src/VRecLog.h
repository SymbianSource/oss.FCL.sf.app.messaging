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




#ifndef LOG_H
#define LOG_H

/////////////////////////////////////////////////////////////////////////////
// #Defines that control printing of additional information to debug stream.
/////////////////////////////////////////////////////////////////////////////

// INCLUDES

#include "VRecLogDef.h"
#include <e32std.h>

// DEFINES

#ifdef ENABLE_LOGGING
#define LOG(s) Log::LogIt(_L(s))
#define LOG1(s, v) Log::LogIt( _L(s), v )
#define LOG2(s, v1, v2) Log::LogIt( _L(s), v1, v2 )
#else
#define LOG(s)
#define LOG1(s, v)
#define LOG2(s, v1, v2)
#endif // ENABLE_LOGGING

// DATA TYPES


// CLASS DECLARATION

/**
* Class offers logging functionality.
*/
NONSHARABLE_CLASS( Log )
    {

    public:

        /**
        * Log the message.
		* @param aText text to be written to log.
        */
        static void LogIt( TRefByValue<const TDesC> aText, ... );

    private: // constructor
        
		/// Prohibited default constructor
		Log();

		/// Prohibited copy constructor
		Log( const Log& );
    
		/// Prohibited assignment operator
		Log& operator=( const Log& );
    };

#endif // Log_H

//  End of File  
