/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides UNI MTM UI data methods.
*
*/



// INCLUDE FILES
#include "UniMtmUiData.h"

#include <mtclbase.h>
#include <eikon.rsg>
#include <data_caging_path_literals.hrh> 

#include <coemain.h>
#include <AknsUtils.h>
#include <AknsConstants.h>

#include <mtmuids.h>
#include <msvids.h>
#include <msvuids.h>
#include <mtmuidef.hrh>

#include <MmsUi.rsg>
#include <muiu.mbg>                         // for MMS icon bitmaps
#include <avkon.rsg>

#include <StringLoader.h>           // StringLoader

#include <UniMtms.rsg>

#include "UniMtmPanic.h"
#include "UniMtmConst.h"
#include "UniMtmUi.h"

#include "UniMsvEntry.h"
#include "UniEditorUids.hrh"

// CONSTANTS
const TInt KMceMmsPriority = 1;

const TInt KUniSmsIconIndex = 0;
const TInt KUniMmsIconIndex = 1;
const TInt KUniPriorityHighIconIndex = 2;
const TInt KUniPriorityLowIconIndex = 3;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------
// NewUniMtmUiDataL
// Factory function
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUiData* NewUniMtmUiDataL( CRegisteredMtmDll& aRegisteredDll )
    {
    return CUniMtmUiData::NewL( aRegisteredDll );
    }


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CUniMtmUiData::CUniMtmUiData
// C++ constructor
// ---------------------------------------------------------
//
CUniMtmUiData::CUniMtmUiData( CRegisteredMtmDll& aRegisteredDll )
    :   CBaseMtmUiData( aRegisteredDll )
    {
    }

// ---------------------------------------------------------
// CUniMtmUiData::ConstructL
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CUniMtmUiData::ConstructL()
    {
    CBaseMtmUiData::ConstructL();
    }


// ---------------------------------------------------------
// CUniMtmUiData::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CUniMtmUiData* CUniMtmUiData::NewL( CRegisteredMtmDll& aRegisteredDll )
    {
    CUniMtmUiData* self = new ( ELeave ) CUniMtmUiData( aRegisteredDll );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); //self
    return self;
    }

    
// ---------------------------------------------------------
// CUniMtmUiData::~CUniMtmUiData
// Destructor.
// ---------------------------------------------------------
//
CUniMtmUiData::~CUniMtmUiData()
    {
    }


// ---------------------------------------------------------
// CUniMtmUiData::OperationSupportedL
// ---------------------------------------------------------
//
TInt CUniMtmUiData::OperationSupportedL(
    TInt aFunctionId,
    const TMsvEntry& aContext ) const
    {
    // Only Message Info for messages is a supported operation
    if ( ( aFunctionId == KMtmUiFunctionMessageInfo ) &&
        ( aContext.iType == KUidMsvMessageEntry ) )
        {
        return KErrNone;
        }
    return R_EIK_TBUF_NOT_AVAILABLE;
    }


// ---------------------------------------------------------
// CUniMtmUiData::QueryCapability
// The default UI capability checker (UI MTM may have also minimal
// UI specific checks also)
// ---------------------------------------------------------
//
TInt CUniMtmUiData::QueryCapability(
    TUid aFunctionId,
    TInt& aResponse ) const
    {
    TInt error = KErrNone;

    switch ( aFunctionId.iUid )
        {
        // Supported:
        case KUidMtmQueryMaxTotalMsgSizeValue:
            aResponse = KMaxTInt;
            break;
        case KUidMsvMtmQueryEditorUidValue:
            {
            aResponse = KUidUniEditorApplication;
            break;
            }
        case KUidMsvMtmQuerySupportLinks: // flow through
        case KUidMsvMtmQuerySupportEditor:
            aResponse = ETrue;
            break;
        case KUidMtmQuerySupportAttachmentsValue:
        case KUidMtmQueryCanSendMsgValue:
        case KUidMtmQueryCanReceiveMsgValue:
        case KUidMtmQueryCanCreateNewMsgValue:
        case KUidMsvMtmQueryMessageInfo:
            break;

        default:
            // All others - Not Supported. At least these:
            //KUidMsvMtmQueryViewerUidValue
            //KUidMtmQuerySupportedBodyValue: text part(s) are attachments!
            //KUidMtmQueryMaxBodySizeValue: texts part(s) are attachments!
            //KUidMtmQuerySupportSubjectValue: subject is supported in Viewer, but not in Editor
            //KUidMsvMtmUiQueryCanPrintPreviewMsgValue: no printing in Series 60
            //KUidMsvMtmUiQueryCanPrintMsgValue: no printing in Series 60
            //KUidMtmQueryMaxRecipientCountValue
            //KUidMtmQuerySupportsBioMsgValue
            //KUidMtmQuerySendAsRequiresRenderedImageValue
            //KUidMtmQuerySendAsRenderingUidValue
            //KUidMsvMtmUiQueryExtendedOpenProgress
            //KUidMsvMtmUiQueryExtendedGetMailProgress
            //KUidMsvMtmUiQueryConnectionOrientedServices
            //KUidMsvMtmUiQueryServiceAttributes
            //KUidMsvMtmUiQueryCanGetNew
            //KUidMsvMtmUiQueryCanOpenServiceOnAction
            //KUidMtmQueryOffLineAllowedValue
            //KUidMtmQuerySupportsSchedulingValue
            //KUidMtmQuerySupportsFolderValue

            error = KErrNotSupported;
        }
    return error;   
    }



// ---------------------------------------------------------
// CUniMtmUiData::ContextIcon
// Returns the array of icons relevant to given entry
// ---------------------------------------------------------
//
const CUniMtmUiData::CBitmapArray& CUniMtmUiData::ContextIcon(
    const TMsvEntry& aContext,
    TInt aStateFlags ) const
    {
    //    Return the set of icons (i.e. different sizes) appropriate to the current context
    __ASSERT_DEBUG( aContext.iMtm == Type(), Panic( EUniWrongMtm ) );
    __ASSERT_DEBUG( aContext.iType.iUid != KUidMsvFolderEntryValue, Panic( EUniFoldersNotSupported ) );
    __ASSERT_DEBUG( aContext.iType.iUid != KUidMsvAttachmentEntryValue, Panic( EUniNoIconForAttachment ) );

	TInt bmpIndex = 0;

	if( aStateFlags == KMceMmsPriority )
		{
        if ( aContext.Priority() == EMsvHighPriority )
            {
            bmpIndex = KUniPriorityHighIconIndex;
            }
        else if ( aContext.Priority() == EMsvLowPriority )
            {
            bmpIndex = KUniPriorityLowIconIndex;
            }
		}
	else
		{
	    TUniMessageCurrentType currType = TUniMsvEntry::CurrentMessageType( aContext );	
	    bmpIndex = ( currType == EUniMessageCurrentTypeSms )?KUniSmsIconIndex : KUniMmsIconIndex;
		}
    
    return *iIconArrays->At( bmpIndex );
    
    }

// ---------------------------------------------------------
// CUniMtmUiData::CanCreateEntryL
// ---------------------------------------------------------
//
TBool CUniMtmUiData::CanCreateEntryL(
    const TMsvEntry& /*aParent*/,
    TMsvEntry& /*aNewEntry*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CUniMtmUiData::CanOpenEntryL
// ---------------------------------------------------------
//
TBool CUniMtmUiData::CanOpenEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CUniMtmUiData::CanCloseEntryL
// ---------------------------------------------------------
//
TBool CUniMtmUiData::CanCloseEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CUniMtmUiData::CanViewEntryL
// ---------------------------------------------------------
//
TBool CUniMtmUiData::CanViewEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CUniMtmUiData::CanEditEntryL
// ---------------------------------------------------------
//
TBool CUniMtmUiData::CanEditEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CUniMtmUiData::CanDeleteFromEntryL
// ---------------------------------------------------------
//
TBool CUniMtmUiData::CanDeleteFromEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CUniMtmUiData::CanDeleteServiceL
// ---------------------------------------------------------
//
TBool CUniMtmUiData::CanDeleteServiceL(
    const TMsvEntry& /*aService*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CUniMtmUiData::CanCopyMoveToEntryL
// ---------------------------------------------------------
//
TBool CUniMtmUiData::CanCopyMoveToEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CUniMtmUiData::CanCopyMoveFromEntryL
// ---------------------------------------------------------
//
TBool CUniMtmUiData::CanCopyMoveFromEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CUniMtmUiData::CanReplyToEntryL
// ---------------------------------------------------------
//
TBool CUniMtmUiData::CanReplyToEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CUniMtmUiData::CanForwardEntryL
// ---------------------------------------------------------
//
TBool CUniMtmUiData::CanForwardEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CUniMtmUiData::CanCancelL
// ---------------------------------------------------------
//
TBool CUniMtmUiData::CanCancelL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CUniMtmUiData::StatusTextL
// Shows the text in outbox
// ---------------------------------------------------------
//
HBufC* CUniMtmUiData::StatusTextL( const TMsvEntry& /*aContext*/ ) const
    {
    return HBufC::NewL( 0 );
    }

// ---------------------------------------------------------
// CUniMtmUiData::PopulateArraysL
// Populate MTM specific arrays: functions and bitmaps
// ---------------------------------------------------------
//
void CUniMtmUiData::PopulateArraysL()
    {
    ReadFunctionsFromResourceFileL( R_UNI_MTM_FUNCTION_ARRAY );

    // Populate bitmap array
    TParse fileParse;
    fileParse.Set( KUniMtmUiBitmapFile, &KDC_APP_BITMAP_DIR, NULL );
    CreateSkinnedBitmapsL(
        fileParse.FullName(), 
        EMbmMuiuQgn_prop_mce_sms_read, 
        EMbmMuiuQgn_prop_mce_sms_read_mask );
    CreateSkinnedBitmapsL(
        fileParse.FullName(), 
        EMbmMuiuQgn_prop_mce_mms_read, 
        EMbmMuiuQgn_prop_mce_mms_read_mask );   
    CreateSkinnedBitmapsL( 
    	fileParse.FullName(),
		EMbmMuiuQgn_indi_mce_priority_high,
		EMbmMuiuQgn_indi_mce_priority_low_mask );	
    }

// ---------------------------------------------------------
// CUniMtmUiData::CreateSkinnedBitmapsL
// Populate MTM specific bitmaps
// ---------------------------------------------------------
//
void CUniMtmUiData::CreateSkinnedBitmapsL( const TDesC& aBitmapFile,
                                          TInt aStartBitmap,
                                          TInt aEndBitmap )
    {
    CBitmapArray* array = NULL;
    for ( TInt i = aStartBitmap; i <= aEndBitmap; i += KUniNumberOfZoomStates )
        {
        array = new( ELeave ) CArrayPtrFlat<CFbsBitmap>( KUniNumberOfZoomStates );
        CleanupStack::PushL( array );

        TAknsItemID skinId;
        switch ( i )
            {
            case EMbmMuiuQgn_prop_mce_mms_read:
                {
                skinId.Set( KAknsIIDQgnPropMceMmsRead );
                break;
                }
            case EMbmMuiuQgn_prop_mce_sms_read:
                {
                skinId.Set( KAknsIIDQgnPropMceSmsRead );
                break;
                }
            case EMbmMuiuQgn_indi_mce_priority_high:
                {
                skinId.Set( KAknsIIDQgnIndiTodoHighAdd );
                break;
                }
            case EMbmMuiuQgn_indi_mce_priority_low:
                {
                skinId.Set( KAknsIIDQgnIndiTodoLowAdd );
                break;
                }
            default:
                {
                User::Leave( KErrNotFound );
                break;
                }
            }

        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;
        AknsUtils::CreateIconL(
            AknsUtils::SkinInstance(),
            skinId,
            bitmap,
            mask,
            aBitmapFile,
            i,
            i + 1 );

        CleanupStack::PushL( mask );
        CleanupStack::PushL( bitmap );       
        array->AppendL( bitmap );
        CleanupStack::Pop( bitmap  );
        array->AppendL( mask );
        CleanupStack::Pop( mask );

        iIconArrays->AppendL( array );
        CleanupStack::Pop( array ); 
        }
    }

// ---------------------------------------------------------
// CUniMtmUiData::GetResourceFileName
// Self explanatory.
// ---------------------------------------------------------
//
void CUniMtmUiData::GetResourceFileName( TFileName& aFileName ) const
    {
    aFileName = KUniMtmResourceFile; 
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File
