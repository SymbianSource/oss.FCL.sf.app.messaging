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
*
*/



#ifndef MAILLOG_H
#define MAILLOG_H

/////////////////////////////////////////////////////////////////////////////
// #Defines that control printing of additional information to debug stream.
/////////////////////////////////////////////////////////////////////////////

// INCLUDES

#include "MailLogDef.h"
#include <e32std.h>

// DEFINES

#ifdef MAIL_ENABLE_LOGGING
#define LOGHANDLESIZE(v) MailLog::LogHandleSize(v);
#define LOGHEAP() MailLog::LogHeap();
#define LOG(s) MailLog::Log(_L(s))						// CSI: 78 # Have to use a _L macro
#define LOG1(s, v) MailLog::Log( _L(s), v )				// CSI: 78 # Have to use a _L macro
#define LOG2(s, v1, v2) MailLog::Log( _L(s), v1, v2 )	// CSI: 78 # Have to use a _L macro
#define LOGPARAM_ONLY( x ) x
#else
#define LOGHANDLESIZE(v)
#define LOGHEAP()
#define LOG(s)
#define LOG1(s, v)
#define LOG2(s, v1, v2)
#define LOGPARAM_ONLY( x )
#endif // MAIL_ENABLE_LOGGING

#ifdef MAIL_ENABLE_UNIT_TEST
#define UNIT_TEST(ClassName) friend class ClassName;
#else 
#define UNIT_TEST(ClassName)
#endif

/**
 * Asserts and checks that statement doesn't allocate any memory. If
 * allocation happens evaluates failed.
 * 2147483647 KMaxTInt
 */
#ifdef MAIL_ENABLE_ALLOC_TEST 
#define MAIL_ALLOC_TEST(statement) \
    { TInt err(KErrNoMemory); TInt i(1); \
    while( err != KErrNone && i <= KMaxTInt ) \
        {__UHEAP_FAILNEXT(i); \
        LOG2("MAIL_ALLOC_TEST_FAILNEXT: %d err:%d", i, err); \
        TRAP(err, statement); \
        if(err==KLeaveExit) \
            User::Leave(err); \
        __UHEAP_RESET; \
        i += 1; \
        } \
    }
#else

/**
 * Release version of macro. Just executes statement.
 */
#define MAIL_ALLOC_TEST(statement) statement

#endif // MAIL_ENABLE_ALLOC_TEST


// DATA TYPES


// CLASS DECLARATION

/**
* Class offers logging functionality.
*/
#ifdef MAIL_ENABLE_LOGGING
class RFile;
class MailLog
    {

    public:
    	static void LogHandleSize( RFile aFile );
        /**
        * Log the heap.
        */
        static void LogHeap();
        
        /**
        * Log the message.
		* @param aText text to be written to log.
        */
        static void Log( TRefByValue<const TDesC> aText, ... );
        

    private: // constructor
        
		/// Prohibited default constructor
		MailLog();

		/// Prohibited copy constructor
		MailLog( const MailLog& );
    
		/// Prohibited assignment operator
		MailLog& operator=( const MailLog& );
    };

#endif // MAIL_ENABLE_LOGGING

#endif // MAILLOG_H

//  End of File  
