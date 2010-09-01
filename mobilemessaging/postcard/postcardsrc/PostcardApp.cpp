/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides Postcard App methods.
*
*/



// ========== INCLUDE FILES ================================

#include <eikstart.h>

#include "PostcardApp.h"        // Postcard application class 
#include "PostcardAppServer.h"  // ServerApplication
#include "PostcardDocument.h"   // Postcard document class
#include "PostcardLaf.h"        // Panic enums

// ========== CONSTANTS ====================================
// Note that the UID values are recorded in two places:
// - this source file (below, as a constant for AppDllUid() method)
// - MMP file (i.e. also in the makefile)

const TUid KUidPostcardApplication = { 0x10207247 };

// ---------------------------------------------------------
//  MEMBER FUNCTIONS 
// ---------------------------------------------------------

// ----------------------------------------------------
// CPostcardApplication::CreateDocumentL
// ----------------------------------------------------
CApaDocument* CPostcardApplication::CreateDocumentL()
    {
    return CPostcardDocument::NewL( *this );
    }

// ----------------------------------------------------
// CPostcardApplication::AppDllUid
// ----------------------------------------------------
TUid CPostcardApplication::AppDllUid() const
    {
    return KUidPostcardApplication;
    }

// ---------------------------------------------------------
// CPostcardApplication::NewAppServerL
// Called by framework. Creates an instance of AppServer.
// ---------------------------------------------------------
void CPostcardApplication::NewAppServerL( CApaAppServer*& aAppServer )
	{
	aAppServer = new (ELeave) CPostcardAppServer;
	}

// ========== OTHER EXPORTED FUNCTIONS =====================

LOCAL_C CApaApplication* NewApplication()
    {
    return new CPostcardApplication;
    }

GLDEF_C TInt E32Main( )
    {
    return EikStart::RunApplication( NewApplication );
    }

//  End of File

