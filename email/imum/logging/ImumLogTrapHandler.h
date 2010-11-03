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
* Description:  ImumLogTrapHandler.h
*
*/


#ifndef IMUMLOGTRAPHANDLER_H
#define IMUMLOGTRAPHANDLER_H

#ifdef _DEBUG

#include <e32std.h>
#include "ImumLogConfig.h"
#include "ImumLogContext.h"

#define _IMUM_TRAP_HARNESS_ENTRY      L"_D%d[%x:%x:%x]TraceFramework: Entering trap harness"
#define _IMUM_TRAP_HARNESS_EXIT       L"_D%d[%x:%x:%x]TraceFramework: Exiting trap harness"
#define _IMUM_TRAP_HARNESS_LEAVE      L"_D%d[%x:%x:%x]TraceFramework: ---------- LEAVE OCCURRED !!! ---------- "
static const TUint IMUMERR   = 2;  /**< Used to create an error message */

class _TImumTrapHandler: public TTrapHandler
    {
    public:

        _TImumTrapHandler(_TImumContext& _context) : _dc( _context )
            {
            RThread me;
            iThdId = (TInt)me.Id();
            }
    	void Trap()
    	    {
    	    _IMUMPRINTER(_IT(_IMUM_TRAP_HARNESS_ENTRY), 0, IMUMERR, iThdId, this);
    	    oldHandler->Trap();
    	    }
    	void UnTrap()
    	    {
    	    _IMUMPRINTER(_IT(_IMUM_TRAP_HARNESS_EXIT), 0, IMUMERR, iThdId, this);
    	    oldHandler->UnTrap();
    	    }
    	void Leave(TInt aValue)
    	    {
    	    _IMUMPRINTER(_IT(_IMUM_TRAP_HARNESS_LEAVE), 0, IMUMERR, iThdId, this);
    	    oldHandler->Leave(aValue);
    	    }
    	TTrapHandler* oldHandler;
    private:
        _TImumContext& _dc;
        TInt iThdId;
    };

#endif // _DEBUG

#endif // IMUMLOGTRAPHANDLER_H