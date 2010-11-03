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
*     Defines methods for CMceSessionHolder
*
*/



// INCLUDE FILES
#include <msvapi.h>

#include "MceSessionHolder.h"
#include "McePanic.h"

#include <bldvariant.hrh>

// CONSTANTS
const TInt KMceDefaultNumberOfSessionObservers = 1;
const TInt KMceMessageSessionPriority = CActive::EPriorityStandard - 10;

// ================= MEMBER FUNCTIONS =======================

CMceSessionHolder::CMceSessionHolder() :
    iClientCount( KMceDefaultNumberOfSessionObservers )
    {
    }

CMceSessionHolder* CMceSessionHolder::NewL( MMsvSessionObserver& aSessionObserver )
    {
    CMceSessionHolder* self = new ( ELeave ) CMceSessionHolder();
    CleanupStack::PushL( self );
    self->ConstructL( aSessionObserver );
    CleanupStack::Pop( self );
    return self;
    }


void CMceSessionHolder::ConstructL( MMsvSessionObserver& aSessionObserver )
    {
    iSession = CMsvSession::OpenAsyncL( aSessionObserver, KMceMessageSessionPriority );
    }


CMceSessionHolder::~CMceSessionHolder()
    {
    __ASSERT_DEBUG( !iClientCount, Panic( EMceSessionHolderDeletedWhenClients ) );
    delete iSession;
    }

// ----------------------------------------------------
// CMceSessionHolder::AddClient
// ----------------------------------------------------
void CMceSessionHolder::AddClient()
    {
    iClientCount++;
    }

// ----------------------------------------------------
// CMceSessionHolder::RemoveClient
// ----------------------------------------------------
void CMceSessionHolder::RemoveClient()
    {
    iClientCount--;
    if ( iClientCount == 0 )
        {
        delete iSession;
        iSession = NULL;
        delete this;
        }
    }

// ----------------------------------------------------
// CMceSessionHolder::ClientCount
// ----------------------------------------------------
TInt CMceSessionHolder::ClientCount() const
    {
    return iClientCount;
    }

// ----------------------------------------------------
// CMceSessionHolder::Session
// ----------------------------------------------------
CMsvSession* CMceSessionHolder::Session() const
    {
    return iSession;
    }

// ----------------------------------------------------
// CMceSessionHolder::SetSession
// Sets session pointer
// ----------------------------------------------------
void CMceSessionHolder::SetSession( CMsvSession* aSession )
    {
    iSession = aSession;
    }

//  End of File
