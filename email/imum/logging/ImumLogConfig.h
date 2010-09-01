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
* Description: ImumLogConfig.h
*
*/


#ifndef IMUMLOGCONFIG_H
#define IMUMLOGCONFIG_H


#ifdef _DEBUG

_LIT8(KImumLogBanner,           _IMUM_LOG_BANNER );
_LIT8(KImumLogEnterFn,          "-> %S");
_LIT8(KImumLogLeaveFn,          "<- %S");
_LIT(KImumLogTimeFormatString,  "%H:%T:%S:%*C2");

/**
* @file
* trace_utils.h contains definitions needed for advanced tracing features.
* Tracing can be customized using the following compile time flags:
* - <b>_DEBUG</b>
*   - With this flag undefined, all traces are disabled
* - <b>__KERNEL_MODE__</b>
*   - if kernel mode flag is defined, kernel macro variants are used (no unicode or shared heap related stuff, faster)
* - <b>DISABLE_SYNTAX_CHECK</b>
*   - If this flag is defined, runtime syntax checking features are disabled from traces
* - <b>DISABLE_GROUP_CHECKS</b>
*   - If this flag is defined, runtime group checking features are disabled from traces (API vs local traces, Ctx groups)
*/


#ifndef DISABLE_SYNTAX_CHECK
    #define _MARK_ENTRY() _dc.inOk=ETrue
    #define _DOINCHK() _dc.DoInChk()
    #define _CHK_MULTIIN() _dc.ChkMultiIn()
    #define _CHK_MULTIOUT() _dc.ChkMultiOut()
    #define _MARK_EXIT() _dc.outOk=ETrue
#else
    #define _MARK_ENTRY()
    #define _DOINCHK()
    #define _CHK_MULTIIN()
    #define _CHK_MULTIOUT()
    #define _MARK_EXIT()
#endif // DISABLE_SYNTAX_CHECK

//    #ifndef DISABLE_GROUP_CHECKS
//        #define _CHK_GRP() if ( (!TraceHeap::IsMaskOn(TRACENFO[_dc.iId].iGrpId)) || ((!_dc.iApi) && TraceHeap::IsApiOnly()) ) { break; }
//        #define _CREATE_MASK() TraceHeap::CreateL(CtxDefaultGroups)
//        #define _CHK_LEVEL(level) if ( TraceHeap::IsBelowLevel(level) ) { break; }
//    #else
        #define _CHK_GRP()
        #define _CREATE_MASK()
        #define _CHK_LEVEL(level)
//    #endif // DISABLE_GROUP_CHECKS

/** For tracing */
#ifdef _IMUM_RDEBUG

	/* Includes */
	#include <e32debug.h>

    #define _IT(a) (TPtrC((const TText *)(a)))
    #define _IMUMPRINTER RDebug::Print

/** For filedebug */
#else   // _IMUM_RDEBUG

    /** Includes */
    #include <e32base.h>
    #include <e32std.h>
    #include <e32def.h>
    #include <e32svr.h>
    #include <flogger.h>
    _LIT( KImumLogPath, "Email" );

    inline TBuf<30> LogFile( const TUint aThread, const TText* aText )
        {
        TBuf<30> name;

        name.AppendNum( aThread, EHex );
        name.Append( '-' );
        name.Append( TPtrC( aText ) );

        return name;
        }

    #define _IT(a) KImumLogPath, LogFile( _dc.iThdId, _dc.iFile ), EFileLoggingModeAppend, (TPtrC((const TText *)(a)))
    #define _IMUMPRINTER RFileLogger::WriteFormat
#endif // _IMUM_RDEBUG

#endif // _DEBUG

#endif // IMUMLOGCONFIG_H


