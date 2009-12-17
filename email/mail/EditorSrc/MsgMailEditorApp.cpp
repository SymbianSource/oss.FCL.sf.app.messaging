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
* Description:  Implements main application class
*
*/


// INCLUDE FILES
#include    "MsgMailEditorApp.h"
#include    "MsgMailEditorDocument.h"
#include    <MsgMailUIDs.h>             // UID for application
#include 	<eikstart.h>
#include    "MsgMailEditorAppServer.h"


// ================= MEMBER FUNCTIONS =======================

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::AppDllUid()
// ------------------------------------------------------------------------------
//
TUid CMsgMailEditorApp::AppDllUid() const
    {
    return TUid::Uid(KUidMsgMailEditor);
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::CreateDocumentL()
// ------------------------------------------------------------------------------
//
CApaDocument* CMsgMailEditorApp::CreateDocumentL()
    {
    return CMsgMailEditorDocument::NewL( *this );
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::NewApplication()
// ------------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CMsgMailEditorApp;
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::E32Main()
// ------------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::NewAppServerL()
// ------------------------------------------------------------------------------
//    
void CMsgMailEditorApp::NewAppServerL( CApaAppServer*& aAppServer )
	{
	aAppServer = new (ELeave) CMsgMailEditorAppServer;
	}

// End of File

