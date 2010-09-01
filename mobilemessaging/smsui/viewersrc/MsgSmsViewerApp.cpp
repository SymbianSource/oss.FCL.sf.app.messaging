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
*     Sms viewer application
*
*/



// INCLUDE FILES
#include "MsgSmsViewerDoc.h"        // Documentpart of the Symbian OS app
#include "MsgSmsViewerApp.h"        // Applicationpart of the Symbian OS app
#include "MsgSmsViewerPan.h"        // Panic-codes for SmsViewer
#include "MsgSmsViewerAppServer.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgSmsViewerApplication::AppDllUid
// Returns the application's uid to the UIKON framework
// 
// ---------------------------------------------------------
TUid CMsgSmsViewerApplication::AppDllUid() const
    {
    return KUidSmsViewer;
    }

// ---------------------------------------------------------
// CMsgSmsViewerApplication::CreateDocumentL
// Part of the UIKON framework's application construction. Creates Doc-part.
// 
// ---------------------------------------------------------
CApaDocument* CMsgSmsViewerApplication::CreateDocumentL()
    {
    return CMsgSmsViewerDocument::NewL(*this);
    }

// ---------------------------------------------------------
// CMsgSmsViewerApplication::NewAppServerL
// 
// ---------------------------------------------------------
void CMsgSmsViewerApplication::NewAppServerL( CApaAppServer*& aAppServer )
    {
    aAppServer = new (ELeave) CSmsViewerAppServer;
    }
// ================= OTHER EXPORTED FUNCTIONS ==============

#include <eikstart.h>
LOCAL_C CApaApplication* NewApplication()
    {
	return new CMsgSmsViewerApplication;
    }
    
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
    }

// ---------------------------------------------------------
// Panic implements
// panic handling for Sms Viewer
// Returns: nothing
GLDEF_C void Panic(
    TMsgSmsPanic aPanic) // enum for panic codes
    {
    _LIT(KPanicText,"SmsViewer.app");
    User::Panic(KPanicText, aPanic);
    }   

//  End of File  
