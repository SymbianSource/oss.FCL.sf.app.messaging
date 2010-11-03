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
* Description:  Implements main application class.
*
*/


// INCLUDE FILES
#include    "MsgMailViewerApp.h"
#include    "MsgMailViewerDocument.h"
#include    "MsgMailViewerAppServer.h"
#include    <MsgMailUIDs.h>             // UID for application
#include 	<eikstart.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgMailViewerApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CMsgMailViewerApp::AppDllUid() const
    {
    return TUid::Uid(KUidMsgMailViewer);
    }
// ----------------------------------------------------------------------------
// CMsgMailViewerApp::NewAppServerL()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerApp::NewAppServerL( CApaAppServer*& aAppServer )
	{
	aAppServer = new (ELeave) CMsgMailViewerAppServer;
	}   

// ---------------------------------------------------------
// CMsgMailViewerApp::CreateDocumentL()
// Creates CMsgMailViewerDocument object
// ---------------------------------------------------------
//
CApaDocument* CMsgMailViewerApp::CreateDocumentL()
    {
    return CMsgMailViewerDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============
//
// ---------------------------------------------------------
// NewApplication() 
// Constructs CMsgMailViewerApp
// Returns: CApaDocument*: created application object
// ---------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CMsgMailViewerApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

// End of File  

