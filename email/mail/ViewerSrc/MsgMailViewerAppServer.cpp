/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  App server
*
*/



// INCLUDE FILES
#include "MsgMailViewerAppServer.h"
#include <MuiuMsgEditorServiceSession.h>
#include <MuiuMsgEditorServiceConst.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgMailViewerAppServer::CreateServiceL
// -----------------------------------------------------------------------------
//
CApaAppServiceBase* CMsgMailViewerAppServer::CreateServiceL(
	TUid aServiceType ) const
    {    
   	if (aServiceType == TUid::Uid(KMuiuMsgEditorServiceUid))
   		{
   		return new(ELeave) CMuiuMsgEditorServiceSession;
   		}
	else
		{
		return CAknAppServer::CreateServiceL( aServiceType );	
		}	
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAppServer::CustomFailureActionL
// -----------------------------------------------------------------------------
//
CPolicyServer::TCustomResult CMsgMailViewerAppServer::CustomFailureActionL(
	const RMessage2& /*aMsg*/, 
	TInt /*aAction*/, 
	const TSecurityInfo& /*aMissing*/)
    {
    CPolicyServer::TCustomResult res = EPass;
    return res;
    }
    
// Destructor
CMsgMailViewerAppServer::~CMsgMailViewerAppServer()
    {   
    }



//  End of File  
