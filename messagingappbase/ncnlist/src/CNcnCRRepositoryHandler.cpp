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
* Description:   Methods for CNcnCRRepositoryHandler class.
*
*/



// INCLUDE FILES
#include    "NcnDebug.h"
#include    "CNcnCRRepositoryHandler.h"
#include    "MNcnCRRepositoryHandlerCallback.h"

// ================= LOCAL CONSTANTS =======================
namespace
    {
    }

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
//  CNcnCRRepositoryHandler::CNcnCRRepositoryHandler
// ----------------------------------------------------
//
CNcnCRRepositoryHandler::CNcnCRRepositoryHandler(
            MNcnCRRepositoryHandlerCallback& aCallback,
            CRepository& aRepository,
            CCenRepNotifyHandler::TCenRepKeyType aKeyType,
            TUint32 aId ) :
            iCallback( aCallback ),
            iRepository( aRepository ),
            iKeyType( aKeyType ),
            iId( aId )
    {
    // empty
    }

// ----------------------------------------------------
//  CNcnCRRepositoryHandler::NewL
// ----------------------------------------------------
//
CNcnCRRepositoryHandler* CNcnCRRepositoryHandler::NewL(
    MNcnCRRepositoryHandlerCallback& aCallback,
    CRepository& aRepository,
    CCenRepNotifyHandler::TCenRepKeyType aKeyType,
    TUint32 aId )
    {    
    CNcnCRRepositoryHandler* self =
        new (ELeave) CNcnCRRepositoryHandler(
            aCallback,
            aRepository,
            aKeyType,
            aId );
            
    return self;
    }

// ----------------------------------------------------
//  CNcnCRRepositoryHandler::~CNcnCRRepositoryHandler
// ----------------------------------------------------
//
CNcnCRRepositoryHandler::~CNcnCRRepositoryHandler()
    {
    // just stop listening (releases allocated resources)
    StopListening();
    }

// ----------------------------------------------------
//  CNcnCRRepositoryHandler::HandleNotifyInt
// ----------------------------------------------------
//    
void CNcnCRRepositoryHandler::HandleNotifyInt(TUint32 aId, TInt aNewValue)
    {
    // delegate to callback
    iCallback.HandleRepositoryNotifyInt( iRepository, aId, aNewValue );
    }

// ----------------------------------------------------
//  CNcnCRRepositoryHandler::HandleNotifyReal
// ----------------------------------------------------
//    
void CNcnCRRepositoryHandler::HandleNotifyReal(TUint32 aId, TReal aNewValue)
    {
    // delegate to callback
    iCallback.HandleRepositoryNotifyReal( iRepository, aId, aNewValue );
    }

// ----------------------------------------------------
//  CNcnCRRepositoryHandler::HandleNotifyString
// ----------------------------------------------------
//    
void CNcnCRRepositoryHandler::HandleNotifyString(TUint32 aId, const TDesC16& aNewValue)
    {
    // delegate to callback
    iCallback.HandleRepositoryNotifyString( iRepository, aId, aNewValue );
    }

// ----------------------------------------------------
//  CNcnCRRepositoryHandler::HandleNotifyBinary
// ----------------------------------------------------
//    
void CNcnCRRepositoryHandler::HandleNotifyBinary(TUint32 aId, const TDesC8& aNewValue)
    {
    // delegate to callback
    iCallback.HandleRepositoryNotifyBinary( iRepository, aId, aNewValue );
    }

// ----------------------------------------------------
//  CNcnCRRepositoryHandler::HandleNotifyGeneric
// ----------------------------------------------------
//    
void CNcnCRRepositoryHandler::HandleNotifyGeneric(TUint32 aId)
    {
    // delegate to callback
    iCallback.HandleRepositoryNotifyGeneric( iRepository, aId );
    }

// ----------------------------------------------------
//  CNcnCRRepositoryHandler::HandleNotifyError
// ----------------------------------------------------
//    
void CNcnCRRepositoryHandler::HandleNotifyError(TUint32 aId,
    TInt aError,
    CCenRepNotifyHandler* aHandler)
    {
    // delegate to callback
    iCallback.HandleRepositoryNotifyError( iRepository, aId, aError, aHandler );
    }

// ----------------------------------------------------
//  CNcnCRRepositoryHandler::StartListeningL
// ----------------------------------------------------
//
void CNcnCRRepositoryHandler::StartListeningL()
    {
    // if notify handler is not yet instantiated
    if( !iNotifyHandler )
        {
        // instantiate notify handler
        iNotifyHandler =
            CCenRepNotifyHandler::NewL(
                *this,
                iRepository,
                iKeyType,
                iId );
                
        // and start listening
        iNotifyHandler->StartListeningL();
        }
    }

// ----------------------------------------------------
//  CNcnCRRepositoryHandler::StopListening
// ----------------------------------------------------
//
void CNcnCRRepositoryHandler::StopListening()
    {
    // if notify handler is instantiated
    if( iNotifyHandler )
        {
        // stop listening and delete the instance
        iNotifyHandler->StopListening();
        delete iNotifyHandler;
        iNotifyHandler = 0;
        }
    }
        

//  End of File
