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
#include    "MNcnCRRepositoryHandlerCallback.h"

// ================= LOCAL CONSTANTS =======================
namespace
    {
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------
//  MNcnCRRepositoryHandlerCallback::HandleRepositoryNotifyInt
// ---------------------------------------------------------------
//
void MNcnCRRepositoryHandlerCallback::HandleRepositoryNotifyInt(
            CRepository& /*aRepository*/,
            TUint32 /*aId*/,
            TInt /*aNewValue*/ )
    {
    // empty default implementation
    }

// ---------------------------------------------------------------
//  MNcnCRRepositoryHandlerCallback::HandleRepositoryNotifyReal
// ---------------------------------------------------------------
//
void MNcnCRRepositoryHandlerCallback::HandleRepositoryNotifyReal(
            CRepository& /*aRepository*/,
            TUint32 /*aId*/,
            TReal /*aNewValue*/ )
    {
    // empty default implementation
    }

// ---------------------------------------------------------------
//  MNcnCRRepositoryHandlerCallback::HandleRepositoryNotifyString
// ---------------------------------------------------------------
//
void MNcnCRRepositoryHandlerCallback::HandleRepositoryNotifyString(
            CRepository& /*aRepository*/,
            TUint32 /*aId*/,
            const TDesC16& /*aNewValue*/ )
    {
    // empty default implementation
    }

// ---------------------------------------------------------------
//  MNcnCRRepositoryHandlerCallback::HandleRepositoryNotifyBinary
// ---------------------------------------------------------------
//  
void MNcnCRRepositoryHandlerCallback::HandleRepositoryNotifyBinary(
            CRepository& /*aRepository*/,
            TUint32 /*aId*/,
            const TDesC8& /*aNewValue*/ )
    {
    // empty default implementation
    }

// ---------------------------------------------------------------
//  MNcnCRRepositoryHandlerCallback::HandleRepositoryNotifyGeneric
// ---------------------------------------------------------------
//    
void MNcnCRRepositoryHandlerCallback::HandleRepositoryNotifyGeneric(
            CRepository& /*aRepository*/,
            TUint32 /*aId*/ )
    {
    // empty default implementation
    }

// ---------------------------------------------------------------
//  MNcnCRRepositoryHandlerCallback::HandleRepositoryNotifyError
// ---------------------------------------------------------------
//    
void MNcnCRRepositoryHandlerCallback::HandleRepositoryNotifyError(
            CRepository& /*aRepository*/,
            TUint32 /*aId*/,
            TInt /*aError*/,
            CCenRepNotifyHandler* /*aHandler*/ )
    {
    // empty default implementation
    }
        

//  End of File
