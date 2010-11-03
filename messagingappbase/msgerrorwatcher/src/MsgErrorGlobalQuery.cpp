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
*    CMsgErrorGlobalQuery implementation file
*
*/



#include <e32base.h>
#include <avkon.hrh>
#include <AknGlobalConfirmationQuery.h>

#include "MsgErrorWatcher.h"
#include "MsgErrorGlobalQuery.h"

// LOCAL CONSTANTS AND MACROS

// ---------------------------------------------------------
// CMsgErrorGlobalQuery::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorGlobalQuery* CMsgErrorGlobalQuery::NewL( CMsgErrorWatcher* aWatcher )
    {
    CMsgErrorGlobalQuery* self = new ( ELeave ) CMsgErrorGlobalQuery( aWatcher );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMsgErrorGlobalQuery::ConstructL
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgErrorGlobalQuery::ConstructL()
    {
    iGlobalQuery = CAknGlobalConfirmationQuery::NewL();
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMsgErrorGlobalQuery::CMsgErrorGlobalQuery
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorGlobalQuery::CMsgErrorGlobalQuery( CMsgErrorWatcher* aWatcher ) 
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher )
    {
    }

// ---------------------------------------------------------
// CMsgErrorGlobalQuery::~CMsgErrorGlobalQuery
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorGlobalQuery::~CMsgErrorGlobalQuery()
    {
    Cancel();
    delete iPrompt;
    }

// ---------------------------------------------------------
// CMsgErrorGlobalQuery::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorGlobalQuery::RunL() 
    {
    TRAP_IGNORE( iWatcher->HandleGlobalQueryEventL( iQueryId, iStatus.Int() ) );
    }

// ---------------------------------------------------------
// CMsgErrorGlobalQuery::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorGlobalQuery::DoCancel()
    {
    if ( iGlobalQuery )
        {
        iGlobalQuery->CancelConfirmationQuery();
        }
    }

// ---------------------------------------------------------
// CMsgErrorGlobalQuery::Start
//
// Starts the active object
// ---------------------------------------------------------
//
void CMsgErrorGlobalQuery::ShowQueryL( const TDesC& aPrompt,
                                      TInt aSoftkeys,
                                      TInt aQueryId )
    {
    if ( IsActive() )
        {
        Cancel();
        }
    delete iPrompt;
    iPrompt = NULL;
    iPrompt = aPrompt.AllocL();
    iQueryId = aQueryId;

    SetActive();
    iGlobalQuery->ShowConfirmationQueryL(
        iStatus,
        *iPrompt,
        aSoftkeys,
        R_QGN_NOTE_ERROR_ANIM );
    }


// End of File
