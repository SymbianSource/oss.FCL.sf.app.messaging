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
* Description:   Logging functionality for BVA
*
*/



#ifndef BVALOG_H
#define BVALOG_H

/////////////////////////////////////////////////////////////////////////////
// #Defines that control printing of additional information to debug stream.
/////////////////////////////////////////////////////////////////////////////

// INCLUDES

#include "bvalogdef.h"
#include <e32std.h>

// DEFINES

#ifdef BVA_ENABLE_LOGGING
#define LOG(s) BvaLog::Log(_L(s))
#define LOG1(s, v) BvaLog::Log( _L(s), v )
#define LOG2(s, v1, v2) BvaLog::Log( _L(s), v1, v2 )
#else
#define LOG(s)
#define LOG1(s, v)
#define LOG2(s, v1, v2)
#endif // BVA_ENABLE_LOGGING

// DATA TYPES


// CLASS DECLARATION

/**
* Class offers logging functionality.
*/
NONSHARABLE_CLASS( BvaLog )
    {

    public:

        /**
        * Log the message.
		* @param aText text to be written to log.
        */
        static void Log( TRefByValue<const TDesC> aText, ... );

    private: // constructor

		/// Prohibited default constructor
		BvaLog();

		/// Prohibited copy constructor
		BvaLog( const BvaLog& );

		/// Prohibited assignment operator
		BvaLog& operator=( const BvaLog& );
    };

#endif // BVALOG_H

//  End of File
