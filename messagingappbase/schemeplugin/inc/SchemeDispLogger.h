/*
* Copyright (c) 2002 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*      Logging macros for Scheme Dispatcher.   
*
*/



#ifndef SCHEME_DISP_LOGGER_H

// INCLUDES

#include <e32std.h>
#include <e32def.h>
#ifdef __SCHEMEDISP_LOG__
#include <flogger.h>

/// Scheme logging directory.
_LIT( KSchemeLogDir,                   "Scheme" );
/// Scheme log file name.
_LIT( KSchemeLogFile,                  "SchemePlugin.txt" );
/// Format string: enter function.
_LIT( KSchemeLogEnterFn,               "-> %S" );
/// Format string: leave function.
_LIT( KSchemeLogLeaveFn,               "<- %S" );
/// Format string: time.
_LIT( KSchemeLogTimeFormatString,      "%H:%T:%S:%*C2" );
/// Format string: timestamp.
_LIT( KSchemeLogTimeStampFormatString, "%S %S" );

/// Write log: enter function.
#define CLOG_ENTERFN( a )           \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::WriteFormat        \
        (                           \
        KSchemeLogDir,                 \
        KSchemeLogFile,                \
        EFileLoggingModeAppend,     \
        KSchemeLogEnterFn,             \
        &temp                       \
        );                          \
    }

/// Write log: leave function.
#define CLOG_LEAVEFN( a )           \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::WriteFormat        \
        (                           \
        KSchemeLogDir,                 \
        KSchemeLogFile,                \
        EFileLoggingModeAppend,     \
        KSchemeLogLeaveFn,             \
        &temp                       \
        );                          \
    }

/// Write log: string 'a'.
#define CLOG_WRITE( a )             \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::Write              \
        (                           \
        KSchemeLogDir,                 \
        KSchemeLogFile,                \
        EFileLoggingModeAppend,     \
        temp                        \
        );                          \
    }

/// Write log: formatted.
#define CLOG_WRITE_FORMAT( a, b )   \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::WriteFormat        \
        (                           \
        KSchemeLogDir,                 \
        KSchemeLogFile,                \
        EFileLoggingModeAppend,     \
        temp,                       \
        b                           \
        );                          \
    }

/// Write log: timestamp.
#define CLOG_WRITE_TIMESTAMP( a )                                   \
    {                                                               \
    _LIT( temp, a );                                                \
    TTime time;                                                     \
    time.HomeTime();                                                \
    TBuf<32> timeBuf;                                               \
    TRAPD( err, time.FormatL( timeBuf, KSchemeLogTimeFormatString ) ); \
    if ( !err )                                                     \
        {                                                           \
        RFileLogger::WriteFormat                                    \
            (                                                       \
            KSchemeLogDir,                                             \
            KSchemeLogFile,                                            \
            EFileLoggingModeAppend,                                 \
            KSchemeLogTimeStampFormatString,                           \
            &temp,                                                  \
            &timeBuf                                                \
            );                                                      \
        }                                                           \
    }

#else /* not defined __TEST_SCHEME_DISP_DEBUG */

/// Empty definition (disable log).
#define CLOG_ENTERFN( a )

/// Empty definition (disable log).
#define CLOG_LEAVEFN( a )

/// Empty definition (disable log).
#define CLOG_WRITE( a )

/// Empty definition (disable log).
#define CLOG_WRITE_FORMAT( a, b )

/// Empty definition (disable log).
#define CLOG_WRITE_TIMESTAMP( a )

#endif /* def __SCHEMEDISP_LOG__ */

#endif /* def SCHEME_DISP_LOGGER_H */
