/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: sim-dialog plugin implementation
 *
 */

// INCLUDE FILES
#include <ecom/ecom.h>

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSimDlgPlugin::NewL
// Two phase construction
// ----------------------------------------------------------------------------
inline CSimDlgPlugin* CSimDlgPlugin::NewL( 
        TUid aImpUid,
        CSmsMtmUi&  aSmsMtmUi, 
        CMsvSession& aSession )
    {
    CSimDlgPlugin* self = CSimDlgPlugin::NewLC( aImpUid, aSmsMtmUi, aSession);	
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------------------------------
// CSimDlgPlugin::NewLC
// Two phase construction
// ----------------------------------------------------------------------------
inline CSimDlgPlugin* CSimDlgPlugin::NewLC( 
        TUid aImpUid,
        CSmsMtmUi&  aSmsMtmUi, 
        CMsvSession& aSession)
    {
    // Get the instantiation
	TSimDlgParams params( aSmsMtmUi, aSession );
  
    TAny* implementation = REComSession::CreateImplementationL ( aImpUid,
            _FOFF ( CSimDlgPlugin, iDtor_ID_Key ),
            &params);
    CSimDlgPlugin* self = REINTERPRET_CAST( CSimDlgPlugin*, implementation );
    
    CleanupStack::PushL( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CSimDlgPlugin::~CSimDlgPlugin
// Destructor
// ----------------------------------------------------------------------------
inline CSimDlgPlugin::~CSimDlgPlugin()
    {
    REComSession::DestroyedImplementation ( iDtor_ID_Key );
    }

// End of file

