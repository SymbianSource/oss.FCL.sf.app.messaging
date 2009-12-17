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
* Description:   Provides AudioMessageui data methods. 
*
*/



#include <mtclbase.h>
#include <eikon.rsg>
#include <data_caging_path_literals.hrh> 
#include <coemain.h>
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <fbs.h>
#include <mtmuids.h>
#include <msvids.h>
#include <msvuids.h>
#include <mtmuidef.hrh>
#include <avkon.rsg>
#include <featmgr.h>

#include <audiomessagemtm.rsg>
#include "audiomessageuidata.h"

_LIT(KAMSUiResourceFile, "AudioMessageMtm");

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Factory function
// ---------------------------------------------------------------------------
//
EXPORT_C CBaseMtmUiData* NewAudiomessageUiDataL( CRegisteredMtmDll& aRegisteredDll )
    {
    FeatureManager::InitializeLibL();
    if ( !FeatureManager::FeatureSupported( KFeatureIdAudioMessaging ) )
        {
        FeatureManager::UnInitializeLib();
        User::Leave( KErrNotSupported );
        }
    FeatureManager::UnInitializeLib();
    
    return CAudioMessageUiData::NewL( aRegisteredDll );
    }


// ======== MEMBER FUNCTIONS ========

CAudioMessageUiData::CAudioMessageUiData( CRegisteredMtmDll& aRegisteredDll )
    :   CBaseMtmUiData( aRegisteredDll )
    {
    }

void CAudioMessageUiData::ConstructL()
    {
    CBaseMtmUiData::ConstructL();
    }

CAudioMessageUiData* CAudioMessageUiData::NewL( CRegisteredMtmDll& aRegisteredDll )
    {
    CAudioMessageUiData* self = new ( ELeave ) CAudioMessageUiData( aRegisteredDll );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //self

    return self;
    }

CAudioMessageUiData::~CAudioMessageUiData()
    {
    }

TInt CAudioMessageUiData::OperationSupportedL(
    TInt aFunctionId,
    const TMsvEntry& aContext ) const
    {
    // Only Message Info for messages is a supported operation
    if ( ( aFunctionId == KMtmUiFunctionMessageInfo ) &&
        ( aContext.iType == KUidMsvMessageEntry ) )
        {
        return 0;
        }

    return R_EIK_TBUF_NOT_AVAILABLE;
    }

// ---------------------------------------------------------------------------
// The default UI capability checker (UI MTM may have also minimal
// UI specific checks also)
// ---------------------------------------------------------------------------
//
TInt CAudioMessageUiData::QueryCapability(
    TUid aFunctionId,
    TInt& aResponse ) const
    {
    TInt error = KErrNone;

    switch ( aFunctionId.iUid )
        {
        // Supported:
        case KUidMtmQueryMaxTotalMsgSizeValue:
            aResponse = KMaxTInt;
        case KUidMtmQueryCanSendMsgValue:
        case KUidMtmQueryCanCreateNewMsgValue:
        case KUidMsvMtmQueryMessageInfo:
            break;

        case KUidMsvMtmQuerySupportEditor:
        case KUidMtmQuerySupportAttachmentsValue: // For SendUi
            aResponse = ETrue;
            break;
        default:
            error = KErrNotSupported;
        }
    return error;   
    }

// ---------------------------------------------------------------------------
// Returns the array of icons relevant to given entry
// ---------------------------------------------------------------------------
//
const CBaseMtmUiData::CBitmapArray& CAudioMessageUiData::ContextIcon(
    const TMsvEntry& /*aContext*/,
    TInt /*aStateFlags*/ ) const
    {
    CArrayPtrFlat<CFbsBitmap>* array (NULL); 
    TRAP_IGNORE( array = new (ELeave) CArrayPtrFlat<CFbsBitmap>( 0 ) );
    return *array;
    }

TBool CAudioMessageUiData::CanCreateEntryL(
    const TMsvEntry& /*aParent*/,
    TMsvEntry& /*aNewEntry*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

TBool CAudioMessageUiData::CanOpenEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

TBool CAudioMessageUiData::CanCloseEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

TBool CAudioMessageUiData::CanViewEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

TBool CAudioMessageUiData::CanEditEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

TBool CAudioMessageUiData::CanDeleteFromEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

TBool CAudioMessageUiData::CanDeleteServiceL(
    const TMsvEntry& /*aService*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

TBool CAudioMessageUiData::CanCopyMoveToEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

TBool CAudioMessageUiData::CanCopyMoveFromEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

TBool CAudioMessageUiData::CanReplyToEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

TBool CAudioMessageUiData::CanForwardEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

TBool CAudioMessageUiData::CanCancelL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Shows the text in outbox
// ---------------------------------------------------------------------------
//
HBufC* CAudioMessageUiData::StatusTextL( const TMsvEntry& /*aContext*/ ) const
    {
    return HBufC::NewL( 0 );
    }

void CAudioMessageUiData::PopulateArraysL( )
    {
    ReadFunctionsFromResourceFileL( R_AMS_MTM_FUNCTION_ARRAY );    
    }

void CAudioMessageUiData::GetResourceFileName( TFileName& aFileName ) const
    {
    aFileName = KAMSUiResourceFile; 
    }

