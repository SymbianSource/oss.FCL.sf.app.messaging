/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcnSubscriber implementation.
 *
*/



// INCLUDES
#include <e32svr.h>
#include "NcnSubscriber.h"
#include "NcnDebug.h"
#include "NcnSubscribeHandler.h"

// CONSTANTS

// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CSysApSubscriber::NewL()
// ----------------------------------------------------------------------------
CNcnSubscriber* CNcnSubscriber::NewL( MNcnSubscribeHandler& aPropertyResponder, const TUid& aCategory, TUint aKey )
    {
    CNcnSubscriber* self = new (ELeave) CNcnSubscriber( aPropertyResponder, aCategory, aKey );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //self
    return self;
    }

// ----------------------------------------------------------------------------
// CNcnSubscriber::ConstructL()
// ----------------------------------------------------------------------------
void CNcnSubscriber::ConstructL()
    {
    CActiveScheduler::Add( this );
    iProperty.Attach( iCategory, iKey );
    }

// ----------------------------------------------------------------------------
// CNcnSubscriber::Subscribe()
// ----------------------------------------------------------------------------
void CNcnSubscriber::Subscribe()
    {
	iProperty.Cancel();
	iProperty.Subscribe( iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CNcnSubscriber::CNcnSubscriber()
// ----------------------------------------------------------------------------
CNcnSubscriber::CNcnSubscriber( MNcnSubscribeHandler& aPropertyResponder, const TUid& aCategory, TUint aKey ) :
    CActive( EPriorityStandard ),
    iPropertyResponder( aPropertyResponder ),
    iCategory( aCategory),
    iKey( aKey )
    {
    }

// ----------------------------------------------------------------------------
// CNcnSubscriber::RunL()
// ----------------------------------------------------------------------------
void CNcnSubscriber::RunL()
    {
    Subscribe();
    iPropertyResponder.HandlePropertyChangedL( iCategory, iKey );
    }

// ----------------------------------------------------------------------------
// CNcnSubscriber::DoCancel()
// ----------------------------------------------------------------------------
void CNcnSubscriber::DoCancel()
    {
    iProperty.Cancel();
    }

// ----------------------------------------------------------------------------
// CNcnSubscriber::RunError()
// ----------------------------------------------------------------------------
TInt CNcnSubscriber::RunError( TInt aError )
    {
    NCN_RDEBUG_INT(_L("CNcnSubscriber::RunError: error: %d"), aError );
    return aError;
    }

// ----------------------------------------------------------------------------
// CNcnSubscriber::~CNcnSubscriber()
// ----------------------------------------------------------------------------
CNcnSubscriber::~CNcnSubscriber()
    {
    Cancel();
    iProperty.Close();
    }

// End of File



