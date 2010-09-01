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
* Description:  ImumLogContext.h
*
*/


#ifndef IMUMLOGCONTEXT_H
#define IMUMLOGCONTEXT_H

#include <e32base.h>
#include "ImumLogConfig.h"

#ifdef _DEBUG

// These macros are real macros, that should be used. For temporary purposes, these
// are left out and the logging is done by simple entry logging
    #define _IMUM_UNCONTROLLED_INPUT_MSG  L"%s%d[%x:%x:%x]>%s UNCONTROLLED ENTRY!"
    #define _IMUM_MULTIPLE_ENTRY_MSG      L"%s%d[%x:%x:%x]%s  ADDITIONAL ENTRY!"
    #define _IMUM_UNCONTROLLER_EXIT_MSG   L"%s%d[%x:%x:%x]<%s UNCONTROLLED EXIT!"
    #define _IMUM_MULTIPLE_EXIT_MSG       L"%s%d[%x:%x:%x]%s  ADDITIONAL EXIT!"

    #define _IMUM_API_PREFIX              L"_A"
    #define _IMUM_LOCAL_PREFIX            L"_L"

    class _TImumContext
        {
        public:
            _TImumContext(
                const TText* _fn,
                const TUint _id,
                const TUint _vis,
                const TUint _addr,
                const TUint _thdId,
                const TText* _file,
                const TUint _category=0 )
                :
                iFn(_fn),
                iId(_id),
                iApi((TBool)_vis),
                iAddr(_addr),
                iThdId(_thdId),
                iVis((_vis == 0 ? (TText*)_IMUM_API_PREFIX : (TText*)_IMUM_LOCAL_PREFIX)),
                iFile(_file),
                iCategory(_category),
                _dc( *this )

            #ifndef DISABLE_SYNTAX_CHECK
                ,outOk(EFalse), inOk(EFalse)
            #endif
                {
                }
            ~_TImumContext()
                {
                #ifndef DISABLE_SYNTAX_CHECK
                    do
                        {
//                        #ifndef DISABLE_GROUP_CHECKS
//                                if ( (!TraceHeap::IsMaskOn(TRACENFO[iId].iGrpId)) || ((!iApi) && TraceHeap::IsApiOnly()) )
//                                    {
//                                    break;
//                                    }
//                        #endif

                        DoInChk();
                        if (!outOk)
                            {
                            _IMUMPRINTER(_IT(_IMUM_UNCONTROLLER_EXIT_MSG), iVis, iCategory, iId, iThdId, iAddr, iFn);
                            }
                        } while (0);
                #endif // DISABLE_SYNTAX_CHECK
                }

            const TText* iFn;
            const TUint iId;
            const TText* iVis;
            const TUint iAddr;
            const TInt iThdId;
            const TBool iApi;
            const TText* iFile;
            const TUint iCategory;
            _TImumContext& _dc;

            #ifndef DISABLE_SYNTAX_CHECK
                inline void DoInChk()
                    {
                    if (!inOk)
                        {
                        _IMUMPRINTER(_IT(_IMUM_UNCONTROLLED_INPUT_MSG), iVis, iCategory, iId, iThdId, iAddr, iFn);
                        inOk = ETrue;
                        }
                    }

                inline void ChkMultiIn()
                    {
                    if (inOk)
                        {
                        _IMUMPRINTER(_IT(_IMUM_MULTIPLE_ENTRY_MSG), iVis, iCategory, iId, iThdId, iAddr, iFn);
                        }
                    }

                inline void ChkMultiOut()
                    {
                    if (outOk)
                        {
                        _IMUMPRINTER(_IT(_IMUM_MULTIPLE_EXIT_MSG), iVis, iCategory, iId, iThdId, iAddr, iFn);
                        }
                    }

                TBool inOk;
                TBool outOk;
            #endif // DISABLE_SYNTAX_CHECK
        };

#endif // _DEBUG

#endif      // IMUMLOGCONTEXT_H

// End of File
