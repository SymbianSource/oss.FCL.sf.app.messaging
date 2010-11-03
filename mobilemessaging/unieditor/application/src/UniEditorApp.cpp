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
* Description:   Provides Uni Editor App methods.
*
*/



// ========== INCLUDE FILES ================================

#include <eikstart.h>

#include "UniEditorApp.h"           // UniEditor application class 
#include "UniEditorAppServer.h"        // ServerApplication
#include "UniEditorDocument.h"      // UniEditor document class

#include "UniEditorUids.hrh"

// ========== CONSTANTS ====================================
// Note that the UID values are recorded in two places:
// - this source file (below, as a constant for AppDllUid() method)
// - MMP file (i.e. also in the makefile)

// ---------------------------------------------------------
//  MEMBER FUNCTIONS 
// ---------------------------------------------------------

// ----------------------------------------------------
// CUniEditorApplication::CreateDocumentL
// ----------------------------------------------------
//
CApaDocument* CUniEditorApplication::CreateDocumentL()
    {
    return CUniEditorDocument::NewL( *this, *iCoeEnv );
    }

// ----------------------------------------------------
// CUniEditorApplication::AppDllUid
// ----------------------------------------------------
//
TUid CUniEditorApplication::AppDllUid() const
    {
    return TUid::Uid( KUidUniEditorApplication );
    }

// ---------------------------------------------------------
// CUniEditorApplication::NewAppServerL
// Called by framework. Creates an instance of AppServer.
// ---------------------------------------------------------
// 
void CUniEditorApplication::NewAppServerL( CApaAppServer*& aAppServer )
    {
    aAppServer = new( ELeave ) CUniEditorAppServer;
    }

// ========== OTHER EXPORTED FUNCTIONS =====================

// ---------------------------------------------------------
// NewApplication
// ---------------------------------------------------------
// 
LOCAL_C CApaApplication* NewApplication()
    {
    return new CUniEditorApplication;
    }

// ---------------------------------------------------------
// E32Main
// ---------------------------------------------------------
// 
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

//  End of File

