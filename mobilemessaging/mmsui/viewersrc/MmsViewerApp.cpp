/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides CMmsViewerApplication class methods.
*
*/



// INCLUDES
#include <eikstart.h>

#include "UniMmsViewerDocument.h"  // Documentpart of the Symbian OS - app
#include "MmsViewerApp.h"       // Applicationpart of the Symbian OS - app
#include "MmsViewer.hrh"        // Exception codes for MmsViewer
#include "MmsViewerAppServer.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMmsViewerApplication::AppDllUid
// Returns the application's uid to the EIKON framework
// ---------------------------------------------------------
// 
TUid CMmsViewerApplication::AppDllUid() const
    {
    return KUidMmsViewer;
    }

// ---------------------------------------------------------
// CMmsViewerApplication::CreateDocumentL
// Part of the EIKON framework's application construction. Creates Doc-part.
// ---------------------------------------------------------
// 
CApaDocument* CMmsViewerApplication::CreateDocumentL()
    {
    return CMmsViewerDocument::NewL( *this );
    }

// ---------------------------------------------------------
// CMmsViewerApplication::NewAppServerL
// Called by framework. Creates an instance of AppServer.
// ---------------------------------------------------------
// 
void CMmsViewerApplication::NewAppServerL( CApaAppServer*& aAppServer )
	{
	aAppServer = new (ELeave) CMmsViewerAppServer;
	}

// ================= OTHER EXPORTED FUNCTIONS ==============

LOCAL_C CApaApplication* NewApplication()
    {
    return new CMmsViewerApplication;
    }

GLDEF_C TInt E32Main( )
    {
    return EikStart::RunApplication( NewApplication );
    }

// ---------------------------------------------------------
// Panic implements
// panic handling for Mms Viewer
// Returns: nothing
// ---------------------------------------------------------
//
GLDEF_C void Panic( TMmsViewerExceptions aPanic )
    {
    _LIT( KPanicText,"MmsViewer.app" );
    User::Panic( KPanicText, aPanic );
    }   

//  End of File  
