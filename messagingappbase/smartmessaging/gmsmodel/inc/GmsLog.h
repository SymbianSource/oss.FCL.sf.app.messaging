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




#ifndef GmsLog_H
#define GmsLog_H

/////////////////////////////////////////////////////////////////////////////
// #Defines that control printing of additional information to debug stream.
/////////////////////////////////////////////////////////////////////////////

// INCLUDES

#include "GmsLogDef.h"
#include <e32std.h>

// DEFINES

#ifdef GMSMODEL_ENABLE_LOGGING
#define LOG(s) GmsLog::Log(_L(s))
#define LOG1(s, v) GmsLog::Log( _L(s), v )
#define LOG2(s, v1, v2) GmsLog::Log( _L(s), v1, v2 )
#endif // GMSMODEL_ENABLE_LOGGING

// DATA TYPES


// CLASS DECLARATION

/**
* Class offers logging functionality.
*/
NONSHARABLE_CLASS( GmsLog )
    {
    public:

        /**
        * Log the message.
		* @param aText text to be written to log.
        */
        static void Log( TRefByValue<const TDesC> aText, ... );

    private: // constructor
        
		/// Prohibited default constructor
		GmsLog();

		/// Prohibited copy constructor
		GmsLog( const GmsLog& );
    
		/// Prohibited assignment operator
		GmsLog& operator=( const GmsLog& );
    };

#endif // GmsLog_H

//  End of File  
