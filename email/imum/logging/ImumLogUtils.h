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
* Description:  ImumLogUtils.h
*
*/


#ifndef IMUMLOGUTILS_H
#define IMUMLOGUTILS_H

#include <e32def.h>
#include <e32std.h>

#include "ImumLogContext.h"
#include "ImumLogTrapHandler.h"

#define IMUM_ASSERT(_assertion)  __IMUM_ASSERT_DBG(_assertion)
//#define IMUM_ASSERT(_assertion) __ASSERT_DEBUG((_assertion), User::Invariant() )

#ifdef _DEBUG

/*****************************************************************************
 LOGGING MACROS - DEBUG ON
*****************************************************************************/

    #define IMUM_CONTEXT(_fn, _vis ,_logfile) _TImumContext _dc((TText*)L ## #_fn, _IMUM_LOG_COMPONENT_ID, _vis, (TUint)this, RProcess().SecureId().iId, _logfile().Ptr() )
    #define IMUM_STATIC_CONTEXT(_fn, _vis, _thdId, _logfile) _TImumContext _dc((TText*)L ## #_fn, _IMUM_LOG_COMPONENT_ID, _vis, 0, RProcess().SecureId().iId, _logfile().Ptr())
//
    #define IMUM_IN() do { _CHK_GRP(); _CHK_MULTIIN(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s "), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn); _MARK_ENTRY(); } while(0)
    #define IMUM0_IN(string) do { _CHK_GRP(); _CHK_MULTIIN(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn); _MARK_ENTRY(); } while(0)
    #define IMUM1_IN(string, p1) do { _CHK_GRP(); _CHK_MULTIIN(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1); _MARK_ENTRY(); } while(0)
    #define IMUM2_IN(string, p1, p2) do { _CHK_GRP(); _CHK_MULTIIN(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2); _MARK_ENTRY(); } while(0)
    #define IMUM3_IN(string, p1, p2, p3) do { _CHK_GRP(); _CHK_MULTIIN(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3); _MARK_ENTRY(); } while(0)
    #define IMUM4_IN(string, p1, p2, p3, p4) do { _CHK_GRP(); _CHK_MULTIIN(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4); _MARK_ENTRY(); } while(0)
    #define IMUM5_IN(string, p1, p2, p3, p4, p5) do { _CHK_GRP(); _CHK_MULTIIN(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4, p5); _MARK_ENTRY(); } while(0)

    #define IMUM_OUT() do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s "), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn); _MARK_EXIT(); } while(0)
//    #define IMUM0_OUT(string) do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn); _MARK_EXIT(); } while(0)
//    #define IMUM1_OUT(string, p1) do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1); _MARK_EXIT(); } while(0)
//    #define IMUM2_OUT(string, p1, p2) do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2); _MARK_EXIT(); } while(0)
//    #define IMUM3_OUT(string, p1, p2, p3) do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3); _MARK_EXIT(); } while(0)
//    #define IMUM4_OUT(string, p1, p2, p3, p4) do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4); _MARK_EXIT(); } while(0)
//    #define IMUM5_OUT(string, p1, p2, p3, p4, p5) do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4, p5); _MARK_EXIT(); } while(0)
//
//    #define IMUM0_RET(val, fmtstr) do { do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val); _MARK_EXIT(); } while(0); return val;} while(0)
//    #define IMUM1_RET(val, fmtstr, p1) do { do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val, p1); _MARK_EXIT(); } while(0); return val;} while(0)
//    #define IMUM2_RET(val, fmtstr, p1, p2) do { do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val, p1, p2); _MARK_EXIT(); } while(0); return val;} while(0)
//    #define IMUM3_RET(val, fmtstr, p1, p2, p3) do { do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val, p1, p2, p3); _MARK_EXIT(); } while(0); return val;} while(0)
//    #define IMUM4_RET(val, fmtstr, p1, p2, p3, p4) do { do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val, p1, p2, p3, p4); _MARK_EXIT(); } while(0); return val;} while(0)
//    #define IMUM5_RET(val, fmtstr, p1, p2, p3, p4, p5) do { do { _CHK_GRP(); _DOINCHK(); _CHK_MULTIOUT(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val, p1, p2, p3, p4, p5); _MARK_EXIT(); } while(0); return val;} while(0)
//
    #define IMUM0(level, string) do { _CHK_GRP(); _CHK_LEVEL(level); _DOINCHK(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn); } while(0)
    #define IMUM1(level, string, p1) do { _CHK_GRP(); _CHK_LEVEL(level); _DOINCHK(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1); } while(0)
    #define IMUM2(level, string, p1, p2) do { _CHK_GRP(); _CHK_LEVEL(level); _DOINCHK(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2); } while(0)
    #define IMUM3(level, string, p1, p2, p3) do { _CHK_GRP(); _CHK_LEVEL(level); _DOINCHK(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3); } while(0)
    #define IMUM4(level, string, p1, p2, p3, p4) do { _CHK_GRP(); _CHK_LEVEL(level); _DOINCHK(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4); } while(0)
    #define IMUM5(level, string, p1, p2, p3, p4, p5) do { _CHK_GRP(); _CHK_LEVEL(level); _DOINCHK(); _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4, p5); } while(0)
//    #define __IMUM_ASSERT_DBG( _assertion ) do { if( _assertion ) { break; } TFileName file; file.Copy( _L8( __FILE__ ) );   _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s Assert:%S:%d:" L ## #_assertion) , _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, &file, __LINE__ ); User::Invariant(); } while( 0 )
//    #define __TRACE_ASSERT_DBG(_assertion, _textToPrint, _panicCode)  do { if (_assertion) { break; } _IMUMPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s ASSERTION FAILED!!! %s file: %s, line: %s"), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, _textToPrint, __FILE__, __LINE__); User::Panic(_L("AssertionFailed"), _panicCode} while(0)
//    #define BIND_TRACE_TRAPHANDLER() _TImumTrapHandler _traceTrapHandler; _traceTrapHandler.oldHandler = User::SetTrapHandler(&_traceTrapHandler)
//    #define TRACE_DECL() TInt _iTraceThreadId
//    #define TRACE_CREATE() _CREATE_MASK(); RThread _traceThreadId; _iTraceThreadId = (TInt)_traceThreadId.Id()
//    #define TRACE_FAST_CREATE(_thdId) _CREATE_MASK(); _iTraceThreadId = _thdId

/*****************************************************************************
 LOGGING MACROS - NO DEBUGGING
*****************************************************************************/
#else // _DEBUG

    /**
    * Creates a trace context for traceable function. This variant is used with instance functions (methods).
    * Parameter @a _fn defines the name of the function (method) to trace. @_id defines the group where this trace entry belongs.
    * For full list of available trace contexts, see file trace_ctxt.h. Parameter @a _vis defines the visibility this context is having.
    * Methods with IMPORT_C / EXPORT_C definitions are marked as IMUMAPI, others have signature IMUMLOCAL. \n\n
    * Usage example:
    * @code
    * IMUM_CONTEXT(CFoo::Bar, CtxTest, IMUMLOCAL);
    * @endcode
    */
    #define IMUM_CONTEXT(_fn, _vis ,_logfile)

    /**
    * Creates a trace context for traceable function. This variant is used with plain functions and static methods.
    * Parameter @a _fn defines the name of the function (method) to trace. @_id defines the group where this trace entry belongs.
    * For full list of available trace contexts, see file trace_ctxt.h. Parameter @a _vis defines the visibility this context is having.
    * Methods / functions with IMPORT_C / EXPORT_C definitions are marked as IMUMAPI, others have signature IMUMLOCAL.
    * Parameter @a _thdId is a name for a variable to create when thread id is queried from kernel. Later in the code you can use this
    * variable e.g. to instantiate other trace enabled classes.\n\n
    * Usage example:
    * @code
    * IMUM_CONTEXT(FooBar, CtxTest, IMUMAPI, thdId);
    * ...
    * TRACE_FAST_CREATE(thdId);
    * @endcode
    */
    #define IMUM_STATIC_CONTEXT(_fn, _vis, _thdId, _logfile) TInt _thdId = 0; _thdId++;

    /**
    * Using the defined trace context, outputs method entry information into traces.
    * <b> NOTE: Trace context must be defined before this method can be used.</b>
    * Trace context can be defined using either IMUM_CONTEXT or IMUM_STATIC_CONTEXT
    */
    #define IMUM_IN()
    #define IMUM0_IN(string)
    #define IMUM1_IN(string, p1)
    #define IMUM2_IN(string, p1, p2)
    #define IMUM3_IN(string, p1, p2, p3)
    #define IMUM4_IN(string, p1, p2, p3, p4)
    #define IMUM5_IN(string, p1, p2, p3, p4, p5)

    #define IMUM_OUT()
    #define IMUM0_OUT(string)
    #define IMUM1_OUT(string, p1)
    #define IMUM2_OUT(string, p1, p2)
    #define IMUM3_OUT(string, p1, p2, p3)
    #define IMUM4_OUT(string, p1, p2, p3, p4)
    #define IMUM5_OUT(string, p1, p2, p3, p4, p5)

    #define IMUM0_RET(val, fmtstr) return val
    #define IMUM1_RET(val, fmtstr, p1) return val
    #define IMUM2_RET(val, fmtstr, p1, p2) return val
    #define IMUM3_RET(val, fmtstr, p1, p2, p3) return val
    #define IMUM4_RET(val, fmtstr, p1, p2, p3, p4) return val
    #define IMUM5_RET(val, fmtstr, p1, p2, p3, p4, p5) return val

    #define IMUM0(level, string)
    #define IMUM1(level, string, p1)
    #define IMUM2(level, string, p1, p2)
    #define IMUM3(level, string, p1, p2, p3)
    #define IMUM4(level, string, p1, p2, p3, p4)
    #define IMUM5(level, string, p1, p2, p3, p4, p5)

    #define BIND_TRACE_TRAPHANDLER()
    #define TRACE_DECL() TInt _iTraceThreadId
    #define TRACE_FAST_CREATE(_thdId) _thdId++;
    #define TRACE_CREATE()

    #define __IMUM_ASSERT_DBG(_assertion)
    #define __TRACE_ASSERT_DBG(_assertion, _message, _panicCode )

#endif // _DEBUG

#endif      // IMUMLOGUTILS_H

// End of File
