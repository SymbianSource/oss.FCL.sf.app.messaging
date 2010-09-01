/*
* Copyright (c) 1998 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  bioi implementation
*
*/



#include "bioi.h"               // Class declaration
#include "bium.pan"             // Panic codes

#include <bium.rsg>             // Resources
#include <mtmuidef.hrh>
#include <mtmdef.hrh>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mtmuidsdef.hrh>
#endif
#include <MtmExtendedCapabilities.hrh>  // for KMtmUiNewMessageAction...
#include <msvuids.h>
#include <muiu.mbg>             // Resource Bitmap Ids
#include <bioscmds.h>           // TBiosCmds & TBioProgress
#include <biouids.h>            // KUidBIOMessageTypeMtm...
#include <MsgBioUids.h> // KMsgBioUidPictureMsg
#include <AknsUtils.h>
#include <data_caging_path_literals.hrh>

// Defines/constants
_LIT(KBIOMtmUdResourceFile,"bium");
_LIT(KMuiuIconsFile,"muiu.mbm");

const TInt KBiumNumberOfZoomStates = 2; 
const TInt KBiumFirstBitmapIndex = EMbmMuiuQgn_prop_mce_smart_read;
const TInt KBiumLastBitmapIndex = EMbmMuiuQgn_prop_mce_smart_unread_mask;

// -----------------------------------------------------------------------------
// CBioMtmUiData::NewL
// Panic function
// -----------------------------------------------------------------------------
//
GLDEF_C void Panic(TBioUdPanic aPanic)
    {
    _LIT(KPanicName,"BioUiData");
    User::Panic(KPanicName, aPanic);
    }
    
// -----------------------------------------------------------------------------
// CBioMtmUiData::NewL
// -----------------------------------------------------------------------------
//
CBioMtmUiData* CBioMtmUiData::NewL(CRegisteredMtmDll& aRegisteredDll)
    {
    CBioMtmUiData* self=new(ELeave) CBioMtmUiData(aRegisteredDll);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::CBioMtmUiData
// -----------------------------------------------------------------------------
//
CBioMtmUiData::CBioMtmUiData(CRegisteredMtmDll& aRegisteredDll)
    :   CBaseMtmUiData(aRegisteredDll)
    {
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::ConstructL
// -----------------------------------------------------------------------------
//
void CBioMtmUiData::ConstructL()
    {
    //
    // Construct base
    CBaseMtmUiData::ConstructL();
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::~CBioMtmUiData
// -----------------------------------------------------------------------------
//
CBioMtmUiData::~CBioMtmUiData()
    {
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::OperationSupportedL
// --- Function querys ---
//@todo What about the strings?
// -----------------------------------------------------------------------------
//
TInt CBioMtmUiData::OperationSupportedL(
    TInt aOperationId, const TMsvEntry& aContext) const
    {
    //
    // Not supported if context is not of BIO message type or folder or service.
    if(!CheckEntry(aContext))
        return KErrNotSupported; //R_BIOUD_NOT_SUPPORTED;

    //
    // Handle query, return 0 if operation *is* supported
    switch(aOperationId)
        {
        case KMtmUiFunctionMessageInfo:
            if (aContext.iType==KUidMsvMessageEntry)
                return NULL;
            break;
        case KMtmUiFunctionPrint:               // Drop thru
        case KMtmUiFunctionPrintPreview:
            return KErrNotSupported; //R_BIOUD_PRINT_NOT_SUPPORTED;

        case KMtmUiNewMessageAction:            // Drop thru
        case KBiosMtmParse:                     // Drop thru
        case KBiosMtmParseThenProcess:          // Drop thru
        case KBiosMtmProcess:                   // Drop thru
            return 0;
        // Called once when MTM is being registered
        case KMtmUiMessagingInitialisation:
            break;

#if defined(DEBUG)
        default:
            Panic(EBioMtmUdUnknownOperationId);
#endif
        };

    return KErrNotSupported; //R_BIOUD_NOT_SUPPORTED;
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::QueryCapability
// --- Capability checks ---
// -----------------------------------------------------------------------------
//
TInt CBioMtmUiData::QueryCapability(TUid aCapability, TInt& aResponse) const
    {
	TInt error(KErrNone);

    switch (aCapability.iUid)
        {
        case KMtmUiFunctionMessageInfo:
			{
            break;
			}
        // --- Supported valued capabilities ---
        case KUidMtmQueryMaxBodySizeValue:
        case KUidMtmQueryMaxTotalMsgSizeValue:
			{
            aResponse = KMaxTInt;                 // ie. No max value
            break;
			}
        case KUidMtmQuerySupportedBodyValue:
			{
            aResponse = ETrue;
            break;
			}
        // --- Supported non-valued capabilities ---
        case KUidMsvMtmCanActOnNewMessagesUidValue:
			{
            break;
			}
        // --- Non-Supported capabilities ---
        default:
			{
            error = KErrNotSupported;
			}
        };

    return error;
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::ContextIcon
// --- Context icons ---
// -----------------------------------------------------------------------------
//
const CBaseMtmUiData::CBitmapArray& CBioMtmUiData::ContextIcon(
    const TMsvEntry& aContext,TInt /*aStateFlags*/) const
    {
    __ASSERT_DEBUG(CheckEntry(aContext), Panic(EBioMtmUdNoIconAvailable));
    const TInt KUnreadArrayIndex = 1;
    const TInt KReadArrayIndex = 0;
    const TInt arrayIndex = aContext.Unread() ? 
        KUnreadArrayIndex : 
        KReadArrayIndex;
        
    return *iIconArrays->At(arrayIndex);
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::PopulateArraysL
// CBioMtmUiData - (private methods)
// -----------------------------------------------------------------------------
//
void CBioMtmUiData::PopulateArraysL()
    {
    ReadFunctionsFromResourceFileL(R_BIOUD_FUNCTION_ARRAY);

    // Populate bitmap array
	CreateSkinnedBitmapsL( KBiumNumberOfZoomStates );
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::GetResourceFileName
// -----------------------------------------------------------------------------
//
void CBioMtmUiData::GetResourceFileName(TFileName& aFileName) const
    {
    aFileName=KBIOMtmUdResourceFile;
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::CanCreateEntryL
// --- Function query ---
// -----------------------------------------------------------------------------
//
TBool CBioMtmUiData::CanCreateEntryL(
    const TMsvEntry& /*aParent*/, 
    TMsvEntry& /*aNewEntry*/, 
    TInt& /*aReasonResourceId*/) const
    {
    // BIO Messages are received only, never created or sent.
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::CanOpenEntryL
// --- Function query ---
// -----------------------------------------------------------------------------
//
TBool CBioMtmUiData::CanOpenEntryL(
    const TMsvEntry& aContext, TInt& /*aReasonResourceId*/) const
    {
    //
    // If context is a BIO Message Entry then we can open it.
    if(CheckEntry(aContext))
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::CanCloseEntryL
// --- Function query ---
// -----------------------------------------------------------------------------
//
TBool CBioMtmUiData::CanCloseEntryL(
    const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
    {
    //
    // This only used for services, and BIO has no service entry.
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::CanViewEntryL
// --- Function query ---
// -----------------------------------------------------------------------------
//
TBool CBioMtmUiData::CanViewEntryL(
    const TMsvEntry& aContext, TInt& /*aReasonResourceId*/) const
    {
    //
    // If context is a BIO Message Entry then we can view it.
    if(CheckEntry(aContext))
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::CanEditEntryL
// --- Function query ---
// -----------------------------------------------------------------------------
//
TBool CBioMtmUiData::CanEditEntryL(
    const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
    {
    //
    // BIO Messages are received only, never created or sent.
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::CanDeleteFromEntryL
// --- Function query ---
// -----------------------------------------------------------------------------
//
TBool CBioMtmUiData::CanDeleteFromEntryL(
    const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
    {
    //
    // As BIO messaging does not support folder or service entries,
    // you can never delete the child of an entry.
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::CanDeleteServiceL
// --- Function query ---
// -----------------------------------------------------------------------------
//
TBool CBioMtmUiData::CanDeleteServiceL(
    const TMsvEntry& /*aService*/, TInt& /*aReasonResourceId*/) const
    {
    //
    // BIO messaging does not have folders or services.
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::CanCopyMoveFromEntryL
// --- Function query---
// -----------------------------------------------------------------------------
//
TBool CBioMtmUiData::CanCopyMoveFromEntryL(
    const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
    {
    //
    // As BIO messaging does not support folders or service entries,
    // you can never copy or move the child of an entry.
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::CanCopyMoveToEntryL
// --- Function query---
// -----------------------------------------------------------------------------
//
TBool CBioMtmUiData::CanCopyMoveToEntryL(
    const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
    {
    //
    // As BIO messaging does not support folders or service entries,
    // you can never copy or move the child of an entry.
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::CanReplyToEntryL
// --- Function query ---
// -----------------------------------------------------------------------------
//
TBool CBioMtmUiData::CanReplyToEntryL(
    const TMsvEntry& aContext, TInt& /*aReasonResourceId*/) const
    {
    //
	// Reply is possible only for Picture messages
	return (aContext.iBioType == KMsgBioUidPictureMsg.iUid);
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::CreateSkinnedBitmapsL
// -----------------------------------------------------------------------------
//
// --- Function query ---
TBool CBioMtmUiData::CanForwardEntryL(
    const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
    {
    //
    // BIO Messages are received only, never created or sent.
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::CanCancelL
// --- Function query ---
// -----------------------------------------------------------------------------
//
TBool CBioMtmUiData::CanCancelL(
    const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
    {
    // BIO messages can never be sent, consequently can never be cancelled.
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::StatusTextL
// --- Function query ---
// -----------------------------------------------------------------------------
//
HBufC* CBioMtmUiData::StatusTextL(const TMsvEntry& /*aContext*/) const
    {
    // BIO messages never have a status in the outbox.
    return HBufC::NewL(1);
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::CheckEntry
// --- Specific functions to BIOI ---
// -----------------------------------------------------------------------------
//
TBool CBioMtmUiData::CheckEntry(const TMsvEntry& aContext) const
    {
    // Only MESSAGES of BIO MsgType are supported, 
    // not Bio Msg FOLDERS or SERVICES.
    return ((aContext.iMtm==KUidBIOMessageTypeMtm) &&
            (aContext.iType.iUid==KUidMsvMessageEntryValue));
    }

// -----------------------------------------------------------------------------
// CBioMtmUiData::CreateSkinnedBitmapsL
// -----------------------------------------------------------------------------
//
void CBioMtmUiData::CreateSkinnedBitmapsL( TInt aNumZoomStates )
    {
    MAknsSkinInstance* skins = AknsUtils::SkinInstance();
    TAknsItemID id;
    CFbsBitmap* bitmap;
    CFbsBitmap* bitmapMask;
    TBool found = ETrue;
    
    CBitmapArray* array = NULL;
    
    for( TInt i = KBiumFirstBitmapIndex; i < KBiumLastBitmapIndex+1; i++ )
        {
        found = ETrue;
        switch( i )
            {
			case EMbmMuiuQgn_prop_mce_smart_read:
				id.Set( KAknsIIDQgnPropMceSmartRead );
                break;
			case EMbmMuiuQgn_prop_mce_smart_unread:
				id.Set( KAknsIIDQgnPropMceSmartUnread );
                break;
            default:
                found = EFalse;
                break;
            }

        if( found )
            {
            array = new(ELeave) CArrayPtrFlat< CFbsBitmap >( aNumZoomStates );
            CleanupStack::PushL( array );

            TParse tp;
            tp.Set( KMuiuIconsFile, &KDC_APP_BITMAP_DIR, NULL );
            
            AknsUtils::CreateIconL( skins, id, bitmap,
                bitmapMask, tp.FullName(), i, i + 1 );

            array->AppendL( bitmap );
            array->AppendL( bitmapMask );
            iIconArrays->AppendL( array );

            CleanupStack::Pop(); //array
            }       
        }
	}

// end of file
