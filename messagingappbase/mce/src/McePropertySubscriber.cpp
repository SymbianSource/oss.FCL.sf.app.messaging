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
*     Active object for handling RProperty events
*
*/



// INCLUDE FILES
#include "McePropertySubscriber.h"

// LOCAL CONSTANTS AND MACROS


// ================= MEMBER FUNCTIONS =======================

CMcePropertySubscriber::CMcePropertySubscriber( TCallBack aCallBack, RProperty& aProperty ):
    CActive( EPriorityStandard ), iCallBack( aCallBack ), iProperty( aProperty )
    {
    }

void CMcePropertySubscriber::ConstructL()
    {
    CActiveScheduler::Add( this );    
    }

CMcePropertySubscriber* CMcePropertySubscriber::NewL( TCallBack aCallBack, RProperty& aProperty )
    {   
    CMcePropertySubscriber* self =
        new (ELeave) CMcePropertySubscriber(
        aCallBack, aProperty );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

CMcePropertySubscriber::~CMcePropertySubscriber()
    {
    Cancel();
    }
    
//-------------------------------------------------------------------------------
//CMcePropertySubscriber::SubscribeL()
//-------------------------------------------------------------------------------
void CMcePropertySubscriber::SubscribeL()
    {
    if (!IsActive())
        {
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    }

//-------------------------------------------------------------------------------
//CMcePropertySubscriber::StopSubscribeL()
//-------------------------------------------------------------------------------
void CMcePropertySubscriber::StopSubscribe()
    {
    Cancel();
    }

//-------------------------------------------------------------------------------
//CMcePropertySubscriber::RunL()
//-------------------------------------------------------------------------------
void CMcePropertySubscriber::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        iCallBack.CallBack();
        }
    SubscribeL();
    }

//-------------------------------------------------------------------------------
//CMcePropertySubscriber::DoCancel()
//-------------------------------------------------------------------------------
void CMcePropertySubscriber::DoCancel()
    {
    iProperty.Cancel();
    }
    
    
