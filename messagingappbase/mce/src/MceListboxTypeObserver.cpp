/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Active object to listen list type settings from CenRep 
*     (KCRUidMuiuSettings / KMuiuMailMessageListType )
*
*/



// INCLUDE FILES

#include <centralrepository.h>
#include <messaginginternalcrkeys.h>
#include <messagingvariant.hrh>

#include "MceListboxTypeObserver.h"
#include "MceLogText.h"

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================


CMceListboxTypeObserver* CMceListboxTypeObserver::NewL(
    MMceListboxTypeObserver& aObserver, TUint32 aSetting )
    {
    CMceListboxTypeObserver* self = new (ELeave) CMceListboxTypeObserver( aObserver, aSetting );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// C++ default constructor can NOT contain any code that
// might leave.
//
CMceListboxTypeObserver::CMceListboxTypeObserver(
    MMceListboxTypeObserver& aObserver,    TUint32 aSetting )
:   CActive( CActive::EPriorityIdle ),
    iObserver( aObserver ),
    iSetting( aSetting )
    {
    CActiveScheduler::Add(this);
    }
    
    
// ---------------------------------------------------------
// CMceListboxTypeObserver::ConstructL
// ---------------------------------------------------------
//
void CMceListboxTypeObserver::ConstructL()    
    {
    iRepository = CRepository::NewL( KCRUidMuiuSettings );
    
    TInt error = iRepository->Get( iSetting, iListType );
    if ( error == KErrNotFound )
        {
        error = iRepository->Set( iSetting, 0 );
        }
    
    if ( error != KErrNone )
        {
        iListType = EMceListTypeTwoRow;
        }
        
    User::LeaveIfError ( iRepository->NotifyRequest( iSetting, iStatus ) );
    SetActive();
    }

// destructor
CMceListboxTypeObserver::~CMceListboxTypeObserver()
    {
    Cancel();
    delete iRepository;
    }

// ---------------------------------------------------------
// CMceListboxTypeObserver::DoCancel
// ---------------------------------------------------------
//
void CMceListboxTypeObserver::DoCancel()
    {
    iRepository->NotifyCancel( iSetting );
    }
    
// ---------------------------------------------------------
// CMceListboxTypeObserver::ListboxType
// ---------------------------------------------------------
//
TInt CMceListboxTypeObserver::ListboxType() const
    {
    return iListType;
    }


// ---------------------------------------------------------
// CMceListboxTypeObserver::RunL
// ---------------------------------------------------------
//
void CMceListboxTypeObserver::RunL()
    {
    MCELOGGER_ENTERFN("CMceListboxTypeObserver::RunL()");
    if ( iRepository->Get( iSetting, iListType ) != KErrNone )
        {
        iListType = EMceListTypeTwoRow;
        }
    MCELOGGER_WRITE_FORMAT("Calling observer with value %d", iListType );
    iObserver.ListboxTypeChangedL();
    User::LeaveIfError ( iRepository->NotifyRequest( iSetting, iStatus ) );
    SetActive();
    MCELOGGER_LEAVEFN("CMceListboxTypeObserver::RunL()");
    }


// ---------------------------------------------------------
// CMceListboxTypeObserver::RunError
// Try to restart observer, worst case if that fails is that listbox
// is not updated when message list view is active, but we have to 
// live with that.
// ---------------------------------------------------------
//
TInt CMceListboxTypeObserver::RunError( TInt /*aError*/ )
    {
    if ( iRepository->NotifyRequest( iSetting, iStatus ) == KErrNone )
        {
        SetActive();
        }
    return KErrNone;
    }

//  End of File
