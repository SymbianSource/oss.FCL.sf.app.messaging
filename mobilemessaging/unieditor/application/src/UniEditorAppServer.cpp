/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides CUniEditorAppServer class methods.
*
*/

 

// INCLUDES
#include "UniEditorAppServer.h"
#include "MuiuMsgEditorServiceSession.h"
#include "MuiuMsgEditorServiceConst.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CUniEditorAppServer::CreateServiceL
// ---------------------------------------------------------
//
CApaAppServiceBase* CUniEditorAppServer::CreateServiceL( TUid aServiceType ) const
    {
    if ( aServiceType == TUid::Uid( KMuiuMsgEditorServiceUid ) )
        {
        return new( ELeave ) CMuiuMsgEditorServiceSession;
        }
    else
        {
        return CAknAppServer::CreateServiceL( aServiceType );
        }
    }

// ---------------------------------------------------------
// CUniEditorAppServer::~CUniEditorAppServer
// ---------------------------------------------------------
//
CUniEditorAppServer::~CUniEditorAppServer()
    {    
    }

// ---------------------------------------------------------
// CUniEditorAppServer::CustomFailureActionL
// ---------------------------------------------------------
//
CPolicyServer::TCustomResult CUniEditorAppServer::CustomFailureActionL( const RMessage2& /*aMsg*/, 
                                                                        TInt /*aAction*/, 
                                                                        const TSecurityInfo& /*aMissing*/ )
    {
    return CPolicyServer::EPass;
    }

//  End of File  
