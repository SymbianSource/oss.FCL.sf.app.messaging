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
*       Provides CNotViewerApplication class methods.
*
*/



// INCLUDES
#include "NotViewerDocument.h"  // Documentpart of the Symbian OS - app

#include "NotViewerAppServer.h"	// ServerApplication

#include "NotViewerApp.h"       // Applicationpart of the Symbian OS - app
#include "NotViewer.hrh"        // Exception codes for MmsViewer

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNotViewerApplication::AppDllUid
// Returns the application's uid to the EIKON framework
// ---------------------------------------------------------
// 
TUid CNotViewerApplication::AppDllUid() const
    {
    return KUidNotViewer;
    }

// ---------------------------------------------------------
// CNotViewerApplication::CreateDocumentL
// Part of the EIKON framework's application construction. Creates Doc-part.
// ---------------------------------------------------------
// 
CApaDocument* CNotViewerApplication::CreateDocumentL()
    {
    return CNotViewerDocument::NewL( *this );
    }

// ---------------------------------------------------------
// CNotViewerApplication::NewAppServerL
// Called by framework. Creates an instance of AppServer.
// ---------------------------------------------------------
// 
void CNotViewerApplication::NewAppServerL( CApaAppServer*& aAppServer )
	{
	aAppServer = new (ELeave) CNotViewerAppServer;
	}

// ================= OTHER EXPORTED FUNCTIONS ==============

#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
    {
    return new CNotViewerApplication;
    }

GLDEF_C TInt E32Main( )
    {
    return EikStart::RunApplication( NewApplication );
    }

// ---------------------------------------------------------
// Panic implements
// panic handling for Notification Viewer
// Returns: nothing
// ---------------------------------------------------------
//
GLDEF_C void Panic( TNotViewerExceptions aPanic )
    {
    _LIT( KPanicText,"NotViewer.app" );
    User::Panic( KPanicText, aPanic );
    }   

//  End of File  
