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
*             Provides CMsgSmsViewerAppServer class methods.
*
*/



// INCLUDES
#include "MsgSmsViewerAppServer.h"
#include <MuiuMsgEditorServiceSession.h>
#include <MuiuMsgEditorServiceConst.h>

// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CSmsViewerAppServer::CreateServiceL
//
// -----------------------------------------------------------------------------
//
CApaAppServiceBase* CSmsViewerAppServer::CreateServiceL( TUid aServiceType ) const
    {
   	if (aServiceType == TUid::Uid(KMuiuMsgEditorServiceUid))
        {
		return new(ELeave) CMuiuMsgEditorServiceSession;
        }
    return CAknAppServer::CreateServiceL( aServiceType );
    }
// -----------------------------------------------------------------------------
// CSmsViewerAppServer::CreateServiceL
//
// -----------------------------------------------------------------------------
//
CPolicyServer::TCustomResult CSmsViewerAppServer::CustomFailureActionL(
    const RMessage2& /*aMsg*/, 
    TInt /*aAction*/, 
    const TSecurityInfo& /*aMissing*/ )
    {
    CPolicyServer::TCustomResult res = CPolicyServer::EPass;
    return res;
    }

//  End of File  
