/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides UniEditor Plugin methods.
*
*/




// INCLUDE FILES
#include <e32base.h>
#include <ecom.h>
#include "UniPluginApi.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUniEditorPlugin::NewL
// Factory function.
// -----------------------------------------------------------------------------
//
EXPORT_C CUniEditorPlugin* CUniEditorPlugin::NewL(
    const TUid& aImplementationUid,
    CMsvSession& aSession,
    CUniClientMtm& aUniMtm )
    {
    TUniPluginParams params( aSession, aUniMtm );
    // Get the instantiation
    CUniEditorPlugin* plugin = 
        REINTERPRET_CAST( CUniEditorPlugin*,
            REComSession::CreateImplementationL(
                aImplementationUid,
                _FOFF( CUniEditorPlugin, iDtor_ID_Key ),
                &params ) );
    return plugin; 
    }

// Destructor
EXPORT_C CUniEditorPlugin::~CUniEditorPlugin()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

//  End of File
