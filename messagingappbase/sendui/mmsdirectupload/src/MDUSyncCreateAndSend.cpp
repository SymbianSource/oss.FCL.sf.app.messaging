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
* Description:   Wrapper to make message creation and sending synchronous.
*
*/




// INCLUDE FILES
#include <e32def.h>     // for basic types
#include <E32BASE.H>    // for CActiveScheduler
#include <eikenv.h>     // for CBase

#include "MDUSyncCreateAndSend.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMDUSyncCreateAndSend::CMDUSyncCreateAndSend
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMDUSyncCreateAndSend::CMDUSyncCreateAndSend()
    {
    }

// ---------------------------------------------------------
// CMDUSyncCreateAndSend::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMDUSyncCreateAndSend::ConstructL( CCoeEnv& aCoeEnv, CSendUiSingleton& aSingleton )
    {
    iCreateAndSend = CMDUCreateAndSend::NewL( aCoeEnv, aSingleton, this );
    }

// ---------------------------------------------------------
// CMDUSyncCreateAndSend::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMDUSyncCreateAndSend* CMDUSyncCreateAndSend::NewLC( CCoeEnv& aCoeEnv, CSendUiSingleton& aSingleton )
    {
    CMDUSyncCreateAndSend* self = new(ELeave) CMDUSyncCreateAndSend();

    CleanupStack::PushL( self );
    self->ConstructL( aCoeEnv, aSingleton );
    
    return self;
    }

// Destructor
CMDUSyncCreateAndSend::~CMDUSyncCreateAndSend()
    {
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }

    delete iCreateAndSend;
    }


// ---------------------------------------------------------
// CMDUSyncCreateAndSend::CreateAndSendL
// Creates and sends as many MMS messages as there are files
// to be send.Synchronous operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMDUSyncCreateAndSend::CreateAndSendL(
    CArrayPtrFlat<CSendUiAttachment>* aAttachments,
    const TDesC& aToAddress,
    const TDesC& aToAlias,
    TInt aMaxMsgSize )
    {
    iCreateAndSend->CreateAndSendL(
        aAttachments,
        aToAddress,
        aToAlias,
        aMaxMsgSize );

    BeginActiveWait();
    }

// ---------------------------------------------------------
// CMDUSyncCreateAndSend::MessagesReady
// Callback function called when message creation is completed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMDUSyncCreateAndSend::MessagesReady()
    {
    if( !iWait.IsStarted() )
        {
        return;
        }
    iWait.AsyncStop();
    }

// ---------------------------------------------------------
// CMDUSyncCreateAndSend::BeginActiveWait
// Starts active scheduler waiter.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMDUSyncCreateAndSend::BeginActiveWait()
    {
    if( iWait.IsStarted() )
        {
        return;
        }
    iWait.Start();
    }

// End of file
