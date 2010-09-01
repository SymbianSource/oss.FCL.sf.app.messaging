/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides AudioMessage App methods. 
*
*/



#include <eikstart.h>

#include "audiomessageapp.h" 
#include "audiomessageappserver.h"
#include "audiomessagedocument.h" 

const TUid KUidAudioMessageApplication = { 0x1020745A };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAudioMessageApplication::CreateDocumentL
// ---------------------------------------------------------------------------
//
CApaDocument* CAudioMessageApplication::CreateDocumentL()
    {
    return CAudioMessageDocument::NewL( *this );
    }

// ---------------------------------------------------------------------------
// CAudioMessageApplication::AppDllUid
// ---------------------------------------------------------------------------
//
TUid CAudioMessageApplication::AppDllUid() const
    {
    return KUidAudioMessageApplication;
    }

// ---------------------------------------------------------------------------
// CAudioMessageApplication::NewAppServerL
// Called by framework. Creates an instance of AppServer.
// ---------------------------------------------------------------------------
//
void CAudioMessageApplication::NewAppServerL( CApaAppServer*& aAppServer )
	{
	aAppServer = new ( ELeave ) CAudioMessageAppServer;
	}

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewApplication
// Constructs and returns an application object.
// ---------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CAudioMessageApplication;
    }

// ---------------------------------------------------------------------------
// E32Main
// Main function of the application executable.
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main( )
    {
    return EikStart::RunApplication( NewApplication );
    }

// ---------------------------------------------------------------------------
// Panic
// Panic handling for AudioMessage
// Returns: nothing
// ---------------------------------------------------------------------------
//
GLDEF_C void Panic( TAmsEditorPanic aPanic )
    {
    _LIT( KPanicText,"AudioMessage.app" );
    User::Panic( KPanicText, aPanic );
    }   


