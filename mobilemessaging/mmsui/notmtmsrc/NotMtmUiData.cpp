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
* Description:  
*       Provides Notification MTM UI data methods.
*
*/



// INCLUDE FILES
#include "NotMtmUiData.h"

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

#include <NotUi.rsg>
#include <muiu.mbg>                         // for notif icon bitmaps
#include <messagingvariant.hrh>
#include <avkon.rsg>
#include "NotMtmUiPanic.h"
#include "MmsMtmConst.h"
#include <mmserrors.h>
#include <mmsconst.h>                       // for KUidMsgTypeMultimedia

#include <featmgr.h>  
#include <StringLoader.h>           // StringLoader link against CommonEngine.lib

#include <centralrepository.h>    // link against centralrepository.lib
#include <messaginginternalcrkeys.h> // for Central Repository keys

// CONSTANTS
const TInt KMmsReadIconIndex = 0;
const TInt KMmsUnreadIconIndex = 1;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------
// NewNotMtmUiDataL
// Factory function
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUiData* NewNotMtmUiDataL( CRegisteredMtmDll& aRegisteredDll )
    {
    return CNotMtmUiData::NewL( aRegisteredDll );
    }


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNotMtmUiData::CMmsMtmUiData
// C++ constructor
// ---------------------------------------------------------
//
CNotMtmUiData::CNotMtmUiData( CRegisteredMtmDll& aRegisteredDll )
    :   CBaseMtmUiData( aRegisteredDll )
    {
    }

// ---------------------------------------------------------
// CNotMtmUiData::ConstructL
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CNotMtmUiData::ConstructL()
    {
    // KFeatureIdMMSNotificationsView is needed in CNotMtmUiData::PopulateArraysL,
    // which is called from CBaseMtmUiData::ConstructL();
    
    // Get supported features from feature manager.
    FeatureManager::InitializeLibL();
    
    // KFeatureIdMmsNonDestructiveForward has been deprecated
    // Check of TMsvEntry::iMtmData2 & KMmsStoredInMMBox replaces it. 
    // MMSEngine sets KMmsStoredInMMBox for received notifications
    
    if ( FeatureManager::FeatureSupported( KFeatureIdMMSNotificationsView ) )
        {
        iNotUiDataFlags |= ENotificationsViewInMce;
        }
    FeatureManager::UnInitializeLib();
    CBaseMtmUiData::ConstructL();

    //  Get mms local variation values
    TInt featureBitmask = 0;
    
    CRepository* repository = CRepository::NewL( KCRUidMuiuVariation );
    repository->Get( KMuiuMmsFeatures, featureBitmask );
    delete repository;
    repository = NULL;
    if ( featureBitmask & KMmsFeatureIdNotificationForward )
        {
        iNotUiDataFlags |= EForwardSupported;
        }
    featureBitmask = 0;
    
    }


// ---------------------------------------------------------
// CNotMtmUiData::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CNotMtmUiData* CNotMtmUiData::NewL( CRegisteredMtmDll& aRegisteredDll )
    {
    LOGTEXT( _L8( "MmsMtmUiData::NewL" ) );
    CNotMtmUiData* self = new ( ELeave ) CNotMtmUiData( aRegisteredDll );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //self

    return self;
    }

    
// ---------------------------------------------------------
// CNotMtmUiData::~CMmsMtmUiData
// Destructor.
// ---------------------------------------------------------
//
CNotMtmUiData::~CNotMtmUiData()
    {
    }

// ---------------------------------------------------------
// CNotMtmUiData::OperationSupportedL
// ---------------------------------------------------------
//
TInt CNotMtmUiData::OperationSupportedL(
    TInt aFunctionId,
    const TMsvEntry& aContext ) const
    {
    LOGTEXT2( _L16( "NotMtmUiData - OperationSupported: %d" ), aContext.Id() ); 
    if( ( aContext.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageMNotificationInd ) // Notification
        {
        TNotUiMsgStatus msgstatus = MsgStatus( aContext );

        // Only Message Info for messages is a supported operation
        if ( ( aFunctionId == KMtmUiFunctionMessageInfo ) &&
            ( aContext.iType == KUidMsvMessageEntry ) )
            {   // Info is always supported for messages
            LOGTEXT( _L8( "NotMtmUiData - Operation Message Info supported" ) );
            return 0;
            }

        if ( ( aFunctionId == KMtmUiFunctionFetchMMS ) &&
            ( aContext.iType == KUidMsvMessageEntry ) )
            {

            if( aContext.iMtmData2 & KMmsNewOperationForbidden ) // New Forbidden
                {
                LOGTEXT( _L8( "NotMtmUiData - Operation Retrieve not supported" ) );
                return R_EIK_TBUF_NOT_AVAILABLE;
                }
            // If notification is succesfully routed to app 
            // aContext.iMtmData2 & KMmsMessageRoutedToApplication is ETrue and
            // if notification is NOT succesfully routed to app 
            // aContext.iError is KMmsErrorUnregisteredApplication.
            if(    ( aContext.iError == KMmsErrorUnregisteredApplication ) // To unregistered application
                || ( aContext.iMtmData2 & KMmsMessageRoutedToApplication ) )
                {
                LOGTEXT( _L8( "NotMtmUiData - Operation Retrieve not supported" ) );
                return R_EIK_TBUF_NOT_AVAILABLE;
                }

            if( msgstatus == EMsgStatusDeleted )
                { // Msg already been deleted from server
                LOGTEXT( _L8( "NotMtmUiData - Operation Retrieve not supported" ) );
                return R_EIK_TBUF_NOT_AVAILABLE;
                }

            if( msgstatus == EMsgStatusReadyForFetching 
                || msgstatus == EMsgStatusFailed )
                {   // Fetch is supported if the msg is waiting or something has been failed
                LOGTEXT( _L8( "NotMtmUiData - Operation Retrieve message supported" ) );
                return 0;
                }

            if (    msgstatus == EMsgStatusForwarded 
                &&  aContext.iMtmData2 & KMmsStoredInMMBox )
                { // Fetch is supported if it's forwarded and multiple forward is supported
                LOGTEXT( _L8( "NotMtmUiData - Operation Retrieve supported" ) );
                return 0;
                }

            // Otherwise it's not supported
            LOGTEXT( _L8( "NotMtmUiData - Operation Retrieve not supported" ) );
            return R_EIK_TBUF_NOT_AVAILABLE;
            }


        if (    ( aFunctionId == KMtmUiFunctionDeleteMessage )
            &&  ( aContext.iType == KUidMsvMessageEntry )
            &&  ( aContext.iMtmData2 & KMmsNewOperationForbidden ) ) // New Forbidden
                {
                LOGTEXT( _L8( "NotMtmUiData - Operation Delete not supported" ) );
                return R_EIK_TBUF_NOT_AVAILABLE;
                }

        if ( ( aFunctionId == KMtmUiFunctionDeleteMessage ) &&
            ( aContext.iType == KUidMsvMessageEntry ) &&
            ( msgstatus != EMsgStatusRetrieving ) &&
            ( msgstatus != EMsgStatusForwarding ) )
            {   // Delete is supported if msg is not retrieving or forwarding
            LOGTEXT( _L8( "NotMtmUiData - Operation Delete message supported" ) );
            return 0;
            }

        if ( ( aFunctionId == KMtmUiFunctionOpenMessage ) &&
            ( aContext.iType == KUidMsvMessageEntry ) &&
            ( aContext.iMtmData2 & KMmsNewOperationForbidden ) &&
            ( aContext.iMtmData2 & KMmsOperationForward  ) &&
            ( !( aContext.iMtmData2 & KMmsOperationFetch ) ) )
            {   // Open is supported only if forward is in progress
            LOGTEXT( _L8( "NotMtmUiData - Operation Open message supported" ) );
            return 0;
            }


        if ( ( aFunctionId == KMtmUiFunctionOpenMessage ) &&
            ( aContext.iType == KUidMsvMessageEntry ) &&
            ( msgstatus != EMsgStatusRetrieving ) &&
            ( msgstatus != EMsgStatusWaiting ) )
            {   // Open is supported if msg is not retrieving or forwarding
            LOGTEXT( _L8( "NotMtmUiData - Operation Open message supported" ) );
            return 0;
            }

        if ( ( aFunctionId == KMtmUiFunctionMoveTo ) &&
            ( aContext.iType == KUidMsvMessageEntry ) )
            {   // Move is never supported
            LOGTEXT( _L8( "NotMtmUiData - Operation MoveTo not supported" ) );
            return R_EIK_TBUF_NOT_AVAILABLE;
            }
            
        if ( ( aFunctionId == KMtmUiFunctionForward ) &&
            ( aContext.iType == KUidMsvMessageEntry ) )
            {

            if( aContext.iMtmData2 & KMmsNewOperationForbidden ) // New Forbidden
                {
                LOGTEXT( _L8( "NotMtmUiData - Operation Forward not supported" ) );
                return R_EIK_TBUF_NOT_AVAILABLE;
                }

            if( iNotUiDataFlags & EForwardSupported )
                {

                if( msgstatus == EMsgStatusDeleted )
                    {   // Forward not supported if msg has been deleted
                    LOGTEXT( _L8( "NotMtmUiData - Operation Retrieve not supported" ) );
                    return R_EIK_TBUF_NOT_AVAILABLE;
                    }

                // If notification is succesfully routed to app 
                // aContext.iMtmData2 & KMmsMessageRoutedToApplication is ETrue and
                // if notification is NOT succesfully routed to app 
                // aContext.iError is KMmsErrorUnregisteredApplication.
                if(    ( aContext.iError == KMmsErrorUnregisteredApplication ) // To unregistered application
                    || ( aContext.iMtmData2 & KMmsMessageRoutedToApplication ) )
                    {
                    LOGTEXT( _L8( "NotMtmUiData - Operation Forward not supported" ) );
                    return R_EIK_TBUF_NOT_AVAILABLE;
                    }

                if ( msgstatus == EMsgStatusReadyForFetching
                    || msgstatus == EMsgStatusFailed )
                    { // Forward supported if it's ready or failed
                    LOGTEXT( _L8( "NotMtmUiData - Operation Forward supported" ) );
                    return 0;
                    }

                if (    msgstatus == EMsgStatusForwarded
                    &&  aContext.iMtmData2 & KMmsStoredInMMBox )
                    { // Forward supported if it's forwarded and multiple forward supported
                    LOGTEXT( _L8( "NotMtmUiData - Operation Forward supported" ) );
                    return 0;
                    }
                // Otherwise forward is not supported
                LOGTEXT( _L8( "NotMtmUiData - Operation Forward not supported" ) );
                return R_EIK_TBUF_NOT_AVAILABLE;
                }
            else
                {   // If forward is not supported at all -> it's not supported
                LOGTEXT( _L8( "NotMtmUiData - Operation Forward not supported" ) );
                return R_EIK_TBUF_NOT_AVAILABLE;
                }
            }

        }
    else if( ( aContext.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageForwardReq ) // Forward request
        {
        // Only Message Info for messages is a supported operation
        if ( ( aFunctionId == KMtmUiFunctionMessageInfo ) &&
            ( aContext.iType == KUidMsvMessageEntry ) )
            { // Info always supported
            LOGTEXT( _L8( "NotMtmUiData - Operation Message Info supported" ) );
            return 0;
            }

        if ( ( aFunctionId == KMtmUiFunctionDeferSending ) &&
            ( aContext.iType == KUidMsvMessageEntry ) )
            { // Defer never supported
            LOGTEXT( _L8( "NotMtmUiData - Operation Defer sending not supported" ) );
            return R_EIK_TBUF_NOT_AVAILABLE;
            }

        if ( ( aFunctionId == KMtmUiFunctionMoveTo ) &&
            ( aContext.iType == KUidMsvMessageEntry ) )
            { // MoveTo never supported
            LOGTEXT( _L8( "NotMtmUiData - Operation MoveTo not supported" ) );
            return R_EIK_TBUF_NOT_AVAILABLE;
            }

        if ( ( aFunctionId == KMtmUiFunctionDeleteMessage ) &&
            ( aContext.iType == KUidMsvMessageEntry ) )
            { // Delete supported
            LOGTEXT( _L8( "NotMtmUiData - Operation Delete supported" ) );
            return 0;
            }

        if ( ( aFunctionId == KMtmUiFunctionOpenMessage ) &&
            ( aContext.iType == KUidMsvMessageEntry ) &&
            ( aContext.Parent( ) == KMsvSentEntryId ) )
            { // Open supported in Sent items only
            LOGTEXT( _L8( "NotMtmUiData - Operation Open supported in Sent" ) );
            return 0;
            }

        if ( ( aFunctionId == KMtmUiFunctionSend ) &&
            ( aContext.iType == KUidMsvMessageEntry ) )
            { // Send is supported
            LOGTEXT( _L8( "NotMtmUiData - Operation Send supported" ) );
            return 0;
            } 
        }
    
    if ( ( aFunctionId == KMtmUiFunctionDeleteMessage ) &&
        ( aContext.iType == KUidMsvMessageEntry ) &&
        ! ( aContext.iMtmData2 & KMmsNewOperationForbidden ) ) 
        {
        LOGTEXT( _L8( "NotMtmUiData - Operation Delete is supported as message type is treated as Unknown" ) );
        return 0;
        }
        
    LOGTEXT( _L8( "NotMtmUiData - Operation not supported" ) );
    return R_EIK_TBUF_NOT_AVAILABLE;
    }


// ---------------------------------------------------------
// CNotMtmUiData::QueryCapability
// The default UI capability checker (UI MTM may have also minimal
// UI specific checks also)
// ---------------------------------------------------------
//
TInt CNotMtmUiData::QueryCapability(
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
            aResponse = KNotViewer;
            break;
        case KUidMsvMtmQueryViewerUidValue:
            aResponse = KNotViewer;
            break;
        case KUidMsvMtmQuerySupportLinks: // flow through
        case KUidMsvMtmUiQueryMessagingInitialisation: // flow through. This is also in UI MTM!
        case KUidMsvMtmQueryFactorySettings: // This is also in UI MTM!
            aResponse = ETrue;
            break;
        case KUidMtmQuerySupportAttachmentsValue:
        case KUidMtmQueryCanReceiveMsgValue:
        case KUidMsvMtmQueryMessageInfo:
            break;

        default:
            // All others - Not Supported. At least these:
            //KUidMtmQueryCanCreateNewMsgValue
            //KUidMtmQueryCanSendMsgValue
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
    LOGTEXT2( _L16( "NotMtmUiData - QueryCapability: %d" ), error );
    return error;   
    }

// ---------------------------------------------------------
// CNotMtmUiData::ContextIcon
// Returns the array of icons relevant to given entry
// ---------------------------------------------------------
//
const CNotMtmUiData::CBitmapArray& CNotMtmUiData::ContextIcon(
    const TMsvEntry& aContext,
    TInt /*aStateFlags*/ ) const
    {
    //    Return the set of icons (i.e. different sizes) appropriate to the current context
    __ASSERT_DEBUG( aContext.iMtm == KUidMsgMMSNotification, Panic( EMmsWrongMtm ) );
    __ASSERT_DEBUG( aContext.iType.iUid != KUidMsvFolderEntryValue, Panic( EMmsFoldersNotSupported ) );
    __ASSERT_DEBUG( aContext.iType.iUid != KUidMsvAttachmentEntryValue, Panic( EMmsNoIconForAttachment ) );

    const TInt bmpIndex = aContext.Unread() ? KMmsUnreadIconIndex : KMmsReadIconIndex;
    return *iIconArrays->At( bmpIndex );
    }

// ---------------------------------------------------------
// CNotMtmUiData::CanCreateEntryL
// ---------------------------------------------------------
//
TBool CNotMtmUiData::CanCreateEntryL(
    const TMsvEntry& /*aParent*/,
    TMsvEntry& /*aNewEntry*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CNotMtmUiData::CanOpenEntryL
// ---------------------------------------------------------
//
TBool CNotMtmUiData::CanOpenEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CNotMtmUiData::CanCloseEntryL
// ---------------------------------------------------------
//
TBool CNotMtmUiData::CanCloseEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CNotMtmUiData::CanViewEntryL
// ---------------------------------------------------------
//
TBool CNotMtmUiData::CanViewEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CNotMtmUiData::CanEditEntryL
// ---------------------------------------------------------
//
TBool CNotMtmUiData::CanEditEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CNotMtmUiData::CanDeleteFromEntryL
// ---------------------------------------------------------
//
TBool CNotMtmUiData::CanDeleteFromEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CNotMtmUiData::CanDeleteServiceL
// ---------------------------------------------------------
//
TBool CNotMtmUiData::CanDeleteServiceL(
    const TMsvEntry& /*aService*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CNotMtmUiData::CanCopyMoveToEntryL
// ---------------------------------------------------------
//
TBool CNotMtmUiData::CanCopyMoveToEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CNotMtmUiData::CanCopyMoveFromEntryL
// ---------------------------------------------------------
//
TBool CNotMtmUiData::CanCopyMoveFromEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CNotMtmUiData::CanReplyToEntryL
// ---------------------------------------------------------
//
TBool CNotMtmUiData::CanReplyToEntryL(
    const TMsvEntry& aContext,
    TInt& aReasonResourceId ) const
    {
    __ASSERT_DEBUG(aContext.iMtm == KUidMsgMMSNotification, Panic(EMmsWrongMtm));
    if ( aContext.iMtm == KUidMsgMMSNotification
        && aContext.iType == KUidMsvMessageEntry
        && aContext.ReadOnly()
        && aContext.iDetails.Length() )
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
// CNotMtmUiData::CanForwardEntryL
// ---------------------------------------------------------
//
TBool CNotMtmUiData::CanForwardEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CNotMtmUiData::CanCancelL
// ---------------------------------------------------------
//
TBool CNotMtmUiData::CanCancelL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CNotMtmUiData::StatusTextL
// Shows the text in outbox
// ---------------------------------------------------------
//
HBufC* CNotMtmUiData::StatusTextL( const TMsvEntry& aContext ) const
    {
    LOGTEXT2( _L16( "NotMtmUiData - StatusTextL: %d" ), aContext.Id() );
    TInt res = 0;
    TBool needForSubject( EFalse );

    if( ( aContext.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageMNotificationInd )
        {
        //notification
        TNotUiMsgStatus inboxstatus;
        inboxstatus = MsgStatus( aContext );
        switch ( inboxstatus )
            {
            case EMsgStatusRetrieving:      // retrieving in progress
                res = R_NOTMTM_INBOX_STATUS_RETRIEVING;
                break;

            case EMsgStatusWaiting:         // scheduled for automatic fetching
                res = R_NOTMTM_INBOX_STATUS_WAITING;
                break;

            case EMsgStatusForwarding:      // forwarding in progress
                res = R_NOTMTM_INBOX_STATUS_FORWARDING;
                break;

            case EMsgStatusForwarded:       // forwarded
                res = R_NOTMTM_INBOX_STATUS_FORWARDED;
                break;

            case EMsgStatusFailed:          // failed
                res = R_NOTMTM_INBOX_STATUS_FAILED;
                break;

            case EMsgStatusDeleted:
                {
                if( aContext.Parent( ) == KMsvGlobalInBoxIndexEntryId )
                    {
                    res = R_NOTMTM_INBOX_STATUS_DELETED;
                    break;
                    }
                //Let non-inbox entries to flow to next one
                //because entry in mmbox view can not be "deleted from server"
                }
            case EMsgStatusReadyForFetching: //get subject as status text
                res = R_NOTMTM_INBOX_STATUS_NORMAL;
                needForSubject = ETrue; 
                break;

            default:
                // This state is set at start - ignore
                break;

            }
        }
    else 
        {
        if( (aContext.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageForwardReq )
            {
            TUint sendingState(0);
            sendingState = aContext.SendingState();
            //outbox states
            switch ( sendingState )
                {
                case KMsvSendStateSending:          // sending in progress.
                    res = R_NOTMTM_OUTBOX_STATUS_SENDING;
                    break;

                case KMsvSendStateSuspended:        // set when in offline by mmsengine
                case KMsvSendStateResend:           // set when offline->online by mmsengine
                case KMsvSendStateWaiting:
                case KMsvSendStateScheduled:        // not set in mmsengine
                    res = R_NOTMTM_OUTBOX_STATUS_WAITING;
                    break;

                case KMsvSendStateFailed:
                    res = R_NOTMTM_OUTBOX_STATUS_FAILED;
                    break;

                case KMsvSendStateSent:             // sent -> moving to sent items
                case KMsvSendStateNotApplicable:    // not set in mmsengine
                case KMsvSendStateUponRequest:      // not set in mmsengine
                case KMsvSendStateUnknown:
                default:
                    // This state is set at start - ignore
                    break;
                }
            }
        }

    if( needForSubject )
        { 
        if( aContext.iDescription.Length( ) > 0 )
            { // Subject needed and it exists
            return aContext.iDescription.AllocL( );
            }
        }
    
    if ( !res )
        {   // Just in case returns an empty string
        return HBufC::NewL( 0 );
        }

    // Returns the status of the message
    HBufC* statusText = StringLoader::LoadL( res );

    return statusText;
    }

// ---------------------------------------------------------
// CNotMtmUiData::PopulateArraysL
// Populate MTM specific arrays: functions and bitmaps
// ---------------------------------------------------------
//
void CNotMtmUiData::PopulateArraysL()
    {
    ReadFunctionsFromResourceFileL( R_NOT_MTM_FUNCTION_ARRAY );

    if (    ! ( iNotUiDataFlags & ENotificationsViewInMce ) 
        &&  iMtmSpecificFunctions )
        {
        TInt count = iMtmSpecificFunctions->Count();
        for ( TInt i = 0; i < count ; i ++ )
            {
            if ( iMtmSpecificFunctions->At( i ).iFunctionId == KMtmUiFunctionMMBox )
                {
                iMtmSpecificFunctions->Delete( i );
                iMtmSpecificFunctions->Compress( );
                break;
                }
            }
        }

    // Populate bitmap array
    TParse fileParse;
    fileParse.Set( KMmsMtmUiBitmapFile, &KDC_APP_BITMAP_DIR, NULL );
    CreateSkinnedBitmapsL(
        fileParse.FullName(), 
        EMbmMuiuQgn_prop_mce_notif_read, 
        EMbmMuiuQgn_prop_mce_notif_unread_mask );
    }

// ---------------------------------------------------------
// CMmsMtmUiData::CreateSkinnedBitmapsL
// Creates bitmaps and skinned masks with correct TAknsItemID
// ---------------------------------------------------------
//
void CNotMtmUiData::CreateSkinnedBitmapsL( const TDesC& aBitmapFile,
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
            case EMbmMuiuQgn_prop_mce_notif_read:
                {
                skinId.Set( KAknsIIDQgnPropMceNotifRead );
                break;
                }
            case EMbmMuiuQgn_prop_mce_notif_unread:
                {
                skinId.Set( KAknsIIDQgnPropMceNotifUnread );
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
// CNotMtmUiData::GetResourceFileName
// ---------------------------------------------------------
//
void CNotMtmUiData::GetResourceFileName( TFileName& aFileName ) const
    {
    aFileName = KNotMtmResourceFile; 
    }

// ---------------------------------------------------------
// CNotMtmUiData::MsgStatus
// ---------------------------------------------------------
//
TNotUiMsgStatus CNotMtmUiData::MsgStatus( const TMsvEntry& aEntry ) const
    {
    LOGTEXT2( _L16( "CNotMtmUiData::MsgStatus: aEntry.iMtmData2 0x%x" ), aEntry.iMtmData2 ); 
    TNotUiMsgStatus status = EMsgStatusNull;
    
    // operationMask includes operation type. It is not bitmap but ordinal number. 
    // It does not include operation status and result
    TInt operationMask = (aEntry.iMtmData2 & KMmsOperationIdentifier) ;
    
    // Note! Ongoing operation resets every bit of operation type, operation status
    // and result. E.g. If message has been forwarded and then fetching starts, 
    // information about forwarding is lost
    
    if( ( aEntry.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageMNotificationInd ) // Notification
        {
        if(     operationMask == KMmsOperationFetch 
            &&  OperationOngoing( aEntry ) )
            { 
            // It's in retrieving state
            status = EMsgStatusRetrieving;
            }
        else if(    operationMask == KMmsOperationForward
                &&  OperationOngoing( aEntry ) )
            { 
            // It's in forwarding state
            status = EMsgStatusForwarding;
            }
        else if(    operationMask == KMmsOperationForward
                &&  OperationFinished( aEntry )
                &&  !( aEntry.iMtmData2 & KMmsOperationResult ) )
            { 
            // It's been forwarded succesfully
            status = EMsgStatusForwarded;
            }
        else if(    operationMask == KMmsOperationFetch 
                &&  OperationFinished( aEntry )
                &&   (  aEntry.iMtmData2 & KMmsOperationResult 
                    ||  aEntry.iError ) )
            { 
            // Fetch has been failed
            status = EMsgStatusFailed;
            }
        else if(    operationMask == KMmsOperationDelete
                &&  OperationFinished( aEntry )
                &&  !( aEntry.iMtmData2 & KMmsOperationResult ) )
            { 
            // It's been deleted succesfully
            status = EMsgStatusDeleted;
            }
        else 
            {   // Normal waiting state
            status = EMsgStatusReadyForFetching;
            }
        }
    LOGTEXT2( _L16( "CNotMtmUiData::MsgStatus: returns %d" ), status ); 
    return status;
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

// ---------------------------------------------------------
// OperationOngoing
// ---------------------------------------------------------
//
TBool CNotMtmUiData::OperationOngoing( const TMsvEntry& aEntry ) const
    {
    return (    aEntry.iMtmData2 & KMmsOperationOngoing 
            &&  !( aEntry.iMtmData2 & KMmsOperationFinished ) );
    }

// ---------------------------------------------------------
// OperationFinished
// ---------------------------------------------------------
//
TBool CNotMtmUiData::OperationFinished( const TMsvEntry& aEntry ) const
    {
    return (    aEntry.iMtmData2 & KMmsOperationFinished
            &&  !( aEntry.iMtmData2 & KMmsOperationOngoing ) );
    }


//  End of File
