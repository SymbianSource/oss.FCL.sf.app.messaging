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
*       Provides MMS MTM UI data methods.
*
*/



// INCLUDE FILES
#include "MmsMtmUiData.h"

#include <mtclbase.h>
#include <eikon.rsg>
#include <data_caging_path_literals.hrh> 

#include <coemain.h>
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <AknUtils.h>

#include <mtmuids.h>
#include <msvids.h>
#include <msvuids.h>
#include <mtmuidef.hrh>

#include <MmsUi.rsg>
#include <muiu.mbg>                         // for MMS icon bitmaps
#include <avkon.rsg>
#include "MmsMtmUiPanic.h"
#include "MmsMtmConst.h"
#include <mmsconst.h>                       // for KUidMsgTypeMultimedia
#include <mmsmsventry.h>
#include "MmsMtmUi.h"           //  IsUpload()

#include <StringLoader.h>           // StringLoader

// Features
#include <featmgr.h>    
#include <bldvariant.hrh>

#include <messagingvariant.hrh> // for messaging features
 #include <centralrepository.h>    // link against centralrepository.lib
 #include <MmsEngineInternalCRKeys.h>
 #include <messaginginternalcrkeys.h> // for Central Repository keys

// CONSTANTS
const TInt KDateSize = 30;
const TInt KMmsReadIconIndex = 0;
const TInt KMmsUnreadIconIndex = 1;
const TInt KMmsPostcardIconIndex = 2;
const TInt KAmsListenedIconIndex = 3;
const TInt KAmsNotListenedIconIndex = 4;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------
// NewMmsMtmUiDataL
// Factory function
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUiData* NewMmsMtmUiDataL( CRegisteredMtmDll& aRegisteredDll )
    {
    return CMmsMtmUiData::NewL( aRegisteredDll );
    }


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMmsMtmUiData::CMmsMtmUiData
// C++ constructor
// ---------------------------------------------------------
//
CMmsMtmUiData::CMmsMtmUiData( CRegisteredMtmDll& aRegisteredDll )
    :   CBaseMtmUiData( aRegisteredDll )
    {
    }

// ---------------------------------------------------------
// CMmsMtmUiData::ConstructL
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMmsMtmUiData::ConstructL()
    {
    CBaseMtmUiData::ConstructL();
    TInt features(0);
    CRepository* repository = CRepository::NewL( KCRUidMuiuVariation );
    repository->Get( KMuiuMmsFeatures, features );
    delete repository;

    if ( features & KMmsFeatureIdDeliveryStatusDialog ) 
        {
        iDeliveryStatusSupported = ETrue;
        }
    }


// ---------------------------------------------------------
// CMmsMtmUiData::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CMmsMtmUiData* CMmsMtmUiData::NewL( CRegisteredMtmDll& aRegisteredDll )
    {
    LOGTEXT( _L8( "MmsMtmUiData::NewL" ) );
    CMmsMtmUiData* self = new ( ELeave ) CMmsMtmUiData( aRegisteredDll );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //self

    return self;
    }

    
// ---------------------------------------------------------
// CMmsMtmUiData::~CMmsMtmUiData
// Destructor.
// ---------------------------------------------------------
//
CMmsMtmUiData::~CMmsMtmUiData()
    {
    }


// ---------------------------------------------------------
// CMmsMtmUiData::OperationSupportedL
// ---------------------------------------------------------
//
TInt CMmsMtmUiData::OperationSupportedL(
    TInt aFunctionId,
    const TMsvEntry& aContext ) const
    {
    if ( aFunctionId == KMtmUiFunctionDeliveryStatus )
        {
        if (    iDeliveryStatusSupported
            &&  ( aContext.iMtmData2 & KMmsDeliveryStatusMask) != KMmsDeliveryStatusNotRequested 
            &&  aContext.iType == KUidMsvMessageEntry )
            {
            LOGTEXT( _L8( "MmsMtmUiData - Delivery Status supported" ) );
            return 0;   
            }
        LOGTEXT( _L8( "MmsMtmUiData - Delivery Status not supported" ) );
        return R_EIK_TBUF_NOT_AVAILABLE; 
        }
    // Only Message Info for messages is a supported operation
    else if ( aFunctionId == KMtmUiFunctionMessageInfo )
        {
        if ( aContext.iType == KUidMsvMessageEntry )
            {
            LOGTEXT( _L8( "MmsMtmUiData - Operation Message Info supported" ) );
            return 0;
            }
        LOGTEXT( _L8( "MmsMtmUiData - Operation Message Info not supported" ) );
        return R_EIK_TBUF_NOT_AVAILABLE; 
        }

    LOGTEXT( _L8( "MmsMtmUiData - Unknown operation not supported" ) );
    return R_EIK_TBUF_NOT_AVAILABLE;
    }


// ---------------------------------------------------------
// CMmsMtmUiData::QueryCapability
// The default UI capability checker (UI MTM may have also minimal
// UI specific checks also)
// ---------------------------------------------------------
//
TInt CMmsMtmUiData::QueryCapability(
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
        case KUidMsvMtmQueryViewerUidValue:
            aResponse = KMmsViewer;
            break;
        case KUidMsvMtmQuerySupportLinks: // flow through
        case KUidMsvMtmUiQueryMessagingInitialisation: // flow through. This is also in UI MTM!
        case KUidMsvMtmQueryFactorySettings: // This is also in UI MTM!
        case KUidMtmQuerySupportAttachmentsValue:
        case KUidMtmQueryCanSendMsgValue:
        case KUidMtmQueryCanReceiveMsgValue:
        case KUidMsvMtmQueryMessageInfo:
            break;

        default:
            // All others - Not Supported. At least these:
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
    LOGTEXT2( _L16( "MmsMtmUiData - QueryCapability: %d" ), error );
    return error;   
    }



// ---------------------------------------------------------
// CMmsMtmUiData::ContextIcon
// Returns the array of icons relevant to given entry
// ---------------------------------------------------------
//
const CMmsMtmUiData::CBitmapArray& CMmsMtmUiData::ContextIcon(
    const TMsvEntry& aContext,
    TInt /*aStateFlags*/ ) const
    {
    //    Return the set of icons (i.e. different sizes) appropriate to the current context
    __ASSERT_DEBUG( aContext.iMtm == KUidMsgTypeMultimedia, Panic( EMmsWrongMtm ) );
    __ASSERT_DEBUG( aContext.iType.iUid != KUidMsvFolderEntryValue, Panic( EMmsFoldersNotSupported ) );
    __ASSERT_DEBUG( aContext.iType.iUid != KUidMsvAttachmentEntryValue, Panic( EMmsNoIconForAttachment ) );

    TInt bmpIndex(0);   
    // Changes for the cr # 401-1806
    TRAP_IGNORE( FeatureManager::InitializeLibL() );
    if( FeatureManager::FeatureSupported( KFeatureIdMmsPostcard ) && CMmsMtmUi::IsPostcard( aContext  ) )
        // Postcard does not request delivery report
        {
        bmpIndex = KMmsPostcardIconIndex;
        LOGTEXT( _L8( "MmsMtmUiData::ContextIcon() - bmpIndex = KMmsPostcardIconIndex;" ) );
        }
	// Context icon is called for read/unread audio message icons
   	else if( FeatureManager::FeatureSupported( KFeatureIdAudioMessaging ) && CMmsMtmUi::IsAudioMessage( aContext ) )
		{
		bmpIndex = aContext.Unread() ? KAmsNotListenedIconIndex : KAmsListenedIconIndex;
		}
   		
    // Context icon is called for read/unread icons    
    else
        {
        bmpIndex = aContext.Unread() ? KMmsUnreadIconIndex : KMmsReadIconIndex;
        LOGTEXT( _L8( "MmsMtmUiData::ContextIcon() - bmpIndex = aContext.Unread() ? KMmsUnreadIconIndex : KMmsReadIconIndex;" ) );
        }
    FeatureManager::UnInitializeLib();
    return *iIconArrays->At( bmpIndex );
    }

// ---------------------------------------------------------
// CMmsMtmUiData::CanCreateEntryL
// ---------------------------------------------------------
//
TBool CMmsMtmUiData::CanCreateEntryL(
    const TMsvEntry& /*aParent*/,
    TMsvEntry& /*aNewEntry*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CMmsMtmUiData::CanOpenEntryL
// ---------------------------------------------------------
//
TBool CMmsMtmUiData::CanOpenEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CMmsMtmUiData::CanCloseEntryL
// ---------------------------------------------------------
//
TBool CMmsMtmUiData::CanCloseEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CMmsMtmUiData::CanViewEntryL
// ---------------------------------------------------------
//
TBool CMmsMtmUiData::CanViewEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CMmsMtmUiData::CanEditEntryL
// ---------------------------------------------------------
//
TBool CMmsMtmUiData::CanEditEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CMmsMtmUiData::CanDeleteFromEntryL
// ---------------------------------------------------------
//
TBool CMmsMtmUiData::CanDeleteFromEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CMmsMtmUiData::CanDeleteServiceL
// ---------------------------------------------------------
//
TBool CMmsMtmUiData::CanDeleteServiceL(
    const TMsvEntry& /*aService*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CMmsMtmUiData::CanCopyMoveToEntryL
// ---------------------------------------------------------
//
TBool CMmsMtmUiData::CanCopyMoveToEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CMmsMtmUiData::CanCopyMoveFromEntryL
// ---------------------------------------------------------
//
TBool CMmsMtmUiData::CanCopyMoveFromEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CMmsMtmUiData::CanReplyToEntryL
// ---------------------------------------------------------
//
TBool CMmsMtmUiData::CanReplyToEntryL(
    const TMsvEntry& aContext,
    TInt& aReasonResourceId ) const
    {
    __ASSERT_DEBUG( aContext.iMtm == KUidMsgTypeMultimedia, Panic( EMmsWrongMtm ) );
    if ( aContext.iMtm == KUidMsgTypeMultimedia
        && aContext.iType == KUidMsvMessageEntry
        && aContext.ReadOnly()
        && aContext.iDetails.Length()
        && !CMmsMtmUi::IsUpload( aContext )
        && static_cast<const TMmsMsvEntry&>( aContext ).IsMobileTerminated() )
        {
        return ETrue;
        }
    else
        {
        aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
        return EFalse;
        }
    }

// ---------------------------------------------------------
// CMmsMtmUiData::CanForwardEntryL
// ---------------------------------------------------------
//
TBool CMmsMtmUiData::CanForwardEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CMmsMtmUiData::CanCancelL
// ---------------------------------------------------------
//
TBool CMmsMtmUiData::CanCancelL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CMmsMtmUiData::StatusTextL
// Shows the text in outbox
// ---------------------------------------------------------
//
HBufC* CMmsMtmUiData::StatusTextL( const TMsvEntry& aContext ) const
    {
    TInt res = 0;
    TUint sendingState = aContext.SendingState();

    switch ( sendingState )
        {
        case KMsvSendStateFailed:
            res = R_MMS_OUTBOX_STATUS_FAILED;
            break;

        case KMsvSendStateSent:             // sent -> moving to sent items
        case KMsvSendStateNotApplicable:    // not set in mmsengine
            res = 0;
            break;

        case KMsvSendStateWaiting:
        case KMsvSendStateScheduled:        // not set in mmsengine
            res = R_MMS_OUTBOX_STATUS_WAITING;
            break;

        case KMsvSendStateSending:          // sending in progress.
            res = R_MMS_OUTBOX_STATUS_SENDING;
            break;

        case KMsvSendStateUponRequest:      // not set in mmsengine
        case KMsvSendStateSuspended:        // not set in mmsengine
            res = R_MMS_OUTBOX_STATUS_SUSPENDED;
            break;

        case KMsvSendStateResend:           // not set in mmsengine
            res = R_MMS_OUTBOX_STATUS_RESEND;
            break;

        case KMsvSendStateUnknown:
        default:
            // This state is set at start - ignore
            break;
        }
    
    if ( !res )
        {
        return HBufC::NewL( 0 );
        }

    if ( sendingState == KMsvSendStateResend )
        {
        // Time is UTC time
        TTime resendTime = aContext.iDate;
        CMmsMtmUi::ConvertUtcToLocalTime( resendTime );
        
        HBufC* format = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO );
        TBuf<KDateSize> bufTimeFormatted;
        resendTime.FormatL( bufTimeFormatted, format->Des() );

        CleanupStack::PopAndDestroy(); //format

        AknTextUtils::LanguageSpecificNumberConversion( bufTimeFormatted );
        return StringLoader::LoadL( res, bufTimeFormatted );
        }
    else
        {
        return StringLoader::LoadL( res );
        }
    }

// ---------------------------------------------------------
// CMmsMtmUiData::PopulateArraysL
// Populate MTM specific arrays: functions and bitmaps
// ---------------------------------------------------------
//
void CMmsMtmUiData::PopulateArraysL()
    {
    ReadFunctionsFromResourceFileL( R_MMS_MTM_FUNCTION_ARRAY );
    // Populate bitmap array
    TParse fileParse;
    fileParse.Set( KMmsMtmUiBitmapFile, &KDC_APP_BITMAP_DIR, NULL );
    CreateSkinnedBitmapsL(
        fileParse.FullName(), 
        EMbmMuiuQgn_prop_mce_mms_read, 
        EMbmMuiuQgn_prop_mce_mms_unread_mask );
    CreateSkinnedBitmapsL(
        fileParse.FullName(), 
        EMbmMuiuQgn_prop_mce_postcard, 
        EMbmMuiuQgn_prop_mce_postcard_mask );
        
    CreateSkinnedBitmapsL(
        fileParse.FullName(), 
        EMbmMuiuQgn_prop_mce_audio,
        EMbmMuiuQgn_prop_mce_audio_unread_mask );   
    }

// ---------------------------------------------------------
// CMmsMtmUiData::CreateSkinnedBitmapsL
// Populate MTM specific bitmaps
// ---------------------------------------------------------
//
void CMmsMtmUiData::CreateSkinnedBitmapsL( const TDesC& aBitmapFile,
                                          TInt aStartBitmap,
                                          TInt aEndBitmap )
    {
    CBitmapArray* array = NULL;
    for ( TInt i = aStartBitmap; i <= aEndBitmap; i += KMmsNumberOfZoomStates )
        {
        array = new( ELeave ) CArrayPtrFlat<CFbsBitmap>( KMmsNumberOfZoomStates );
        CleanupStack::PushL( array );

        TAknsItemID skinId;
        switch ( i )
            {
            case EMbmMuiuQgn_prop_mce_mms_read:
                {
                skinId.Set( KAknsIIDQgnPropMceMmsRead );
                break;
                }
            case EMbmMuiuQgn_prop_mce_mms_unread:
                {
                skinId.Set( KAknsIIDQgnPropMceMmsUnread );
                break;
                }
            case EMbmMuiuQgn_prop_mce_postcard:
                {
                skinId.Set( KAknsIIDQgnPropMcePostcard );
                break;
                }            
            case EMbmMuiuQgn_prop_mce_audio:
				{
            	skinId.Set( KAknsIIDQgnPropMceAudio );
                break;
				}
           	case EMbmMuiuQgn_prop_mce_audio_unread:
				{
            	skinId.Set( KAknsIIDQgnPropMceAudioUnread );
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

        CleanupStack::PushL( bitmap );
        CleanupStack::PushL( mask );        
        array->AppendL( bitmap );
        CleanupStack::Pop();
        array->AppendL( mask );
        CleanupStack::Pop();

        iIconArrays->AppendL( array );
        CleanupStack::Pop();// array
        }
    }

// ---------------------------------------------------------
// CMmsMtmUiData::GetResourceFileName
// Self explanatory.
// ---------------------------------------------------------
//
void CMmsMtmUiData::GetResourceFileName( TFileName& aFileName ) const
    {
    aFileName = KMmsMtmResourceFile; 
    }


// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File
