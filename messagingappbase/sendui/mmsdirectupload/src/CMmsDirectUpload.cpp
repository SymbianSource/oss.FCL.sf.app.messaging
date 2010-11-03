/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides MMS direct upload service.
*
*/



// INCLUDE FILES
#include    <CSendingServiceInfo.h>
#include    <SendUiConsts.h>
#include    <CMessageData.h>
#include    <TSendingCapabilities.h>
#include    <bautils.h>                        // BaflUtils
#include    <AknQueryDialog.h>                 // Query dialog
#include    <FeatMgr.h>
#include    <StringLoader.h>
#include    <MmsDirectUploadUi.rsg>
#include    <Mmserrors.h>
#include    <data_caging_path_literals.hrh>
#include    <Sendnorm.rsg>
#include    <aknnotewrappers.h>                // Error note
#include    <senduisingleton.h>

#include    "CMmsDirectUpload.h"
#include    "MDUSyncCreateAndSend.h"
#include    "SendUiFileRightsEngine.h"
#include    "CSendUiAttachment.h"

// CONSTANTS

const TInt KMmsUploadServiceCount       = 4;
const TInt KMmsDirectUploadService      = 1;
const TInt KMaxServiceAddressLength     = 512;
const TInt KMaxServiceNameLength        = 128;
const TInt KMaxServiceMenuNameLength    = 128;

_LIT( KMmsDirectUploadUiResource, "MmsDirectUploadUi.rsc");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsDirectUpload::CMmsDirectUpload
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsDirectUpload::CMmsDirectUpload( CCoeEnv& aCoeEnv, CSendUiSingleton& aSingleton ) :
	CSendingService( aCoeEnv, aSingleton ),
    iResourceLoader( iCoeEnv )
    {
    }

// -----------------------------------------------------------------------------
// CMmsDirectUpload::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmsDirectUpload::ConstructL()
    {
    
    ReadServiceDataL();

    // Load UI resource file.
    TFileName resourceFile;
    TParse* fp = new(ELeave) TParse(); 
    fp->Set( KMmsDirectUploadUiResource, &KDC_RESOURCE_FILES_DIR, NULL );
    resourceFile = fp->FullName();
    delete fp;
    fp = NULL;
    
    iResourceLoader.OpenL( resourceFile );
    }

// -----------------------------------------------------------------------------
// CMmsDirectUpload::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmsDirectUpload* CMmsDirectUpload::NewL( TSendingServiceParams* aParams )
    {
    CMmsDirectUpload* self = new(ELeave) CMmsDirectUpload( aParams->iCoeEnv, aParams->iSingleton ); 

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// Destructor
CMmsDirectUpload::~CMmsDirectUpload()
    {
    // Unload UI resource file.
    iResourceLoader.Close();
    iServiceArray.ResetAndDestroy();
    iServiceArray.Close();
    }


// ---------------------------------------------------------
// CMmsDirectUpload::ServiceProviderId
// Returns the id of the service provider.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TUid CMmsDirectUpload::ServiceProviderId() const
    {
    return KMmsDirectUpload;
    }

// -----------------------------------------------------------------------------
// CMmsDirectUpload::PopulateServicesListL
// Populates given list with the services provided by this plugin.
// The ownership of the pointed objects remains.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmsDirectUpload::PopulateServicesListL(
    RPointerArray<CSendingServiceInfo>& aList )
    {
    CleanupClosePushL( aList );
    for ( TInt cc = 0; cc < iServiceArray.Count(); cc++ )
        {
        User::LeaveIfError( aList.Append( iServiceArray[cc] ) );         
        }
    CleanupStack::Pop( &aList );
    }

// -----------------------------------------------------------------------------
// CMmsDirectUpload::CreateAndSendMessageL
// Creates to outbox as many MMS messages as there are files to be send.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmsDirectUpload::CreateAndSendMessageL(
    TUid                    aServiceUid,
    const CMessageData*     aMessageData,
    TBool /*aLaunchEditorEmbedded */)
    {
    TInt i = 0;
    
    if ( !aMessageData || ( iServiceArray.Count() == 0 ) )
        {
        return;
        }
    
    CArrayPtrFlat<CSendUiAttachment>* attachments;
	TInt cleanupItems = 0;
    // Get attachments
    attachments = CSendUiAttachment::InitAttachmentArrayLCC( 
        aMessageData->AttachmentArray(), 
        aMessageData->AttachmentHandleArray(),
        iCoeEnv.FsSession() );
    cleanupItems += 2;
		
    if ( !ValidateAttachmentsL( attachments ) )
        { // User canceled sending.
        
        // Cleanup: attachments, *attachments
        CleanupStack::PopAndDestroy( cleanupItems );
        return;
        }

    const CRichText* bodyText = aMessageData->BodyText();
    if ( bodyText->DocumentLength() > 0 )
        {
        CSendUiAttachment* atta = CSendUiAttachment::NewLC( *bodyText, iCoeEnv.FsSession() );
        attachments->AppendL( atta );
        CleanupStack::Pop( atta );
        }
    TInt attachmentCount = attachments->Count();

    // No valid attachments
    if ( attachmentCount <= 0 )
        {
        CleanupStack::PopAndDestroy( cleanupItems  ); // *attachmemts, attachments
        return;
        }

    // Ask users opinion about sending files in separate messages or canceling
    // the actual sending operation.
    if ( attachmentCount > 1 )
        {
        if ( !ShowConfirmationQueryL( R_MMS_DIRECT_UPLOAD_SEND_MESSAGES_QUERY,
            R_MMS_DIRECT_UPLOAD_SEND_MESSAGES, attachmentCount ) )
            {
            CleanupStack::PopAndDestroy( cleanupItems );
            return;
            }
        }

   // Get service name
    HBufC* serviceName = NULL;
    for ( i = 0; i < iServiceArray.Count(); i++)
        {
        if ( (*iServiceArray[i]).ServiceId() == aServiceUid)
            {
            serviceName = ((*iServiceArray[i]).ServiceName()).AllocLC();
            cleanupItems++;
            if ( serviceName->Length() == 0 )
                {
                User::Leave( KErrNotFound );
                }
            }
        }

    // Get service address
    HBufC*  serviceAddress = NULL;
    for ( i = 0; i < iServiceArray.Count(); i++)
        {
        if ( (*iServiceArray[i]).ServiceId() == aServiceUid)
            {
            serviceAddress = (*iServiceArray[i]).ServiceAddress().AllocLC();
            cleanupItems++;
            if ( serviceAddress->Length() == 0 )
                {
                User::Leave( KErrNotFound );
                }
            }
        }

    // Create and send messages        
    CMDUSyncCreateAndSend* messageSender = CMDUSyncCreateAndSend::NewLC( iCoeEnv, iSingleton );
    cleanupItems++;
    
    TRAPD( error, messageSender->CreateAndSendL(
        attachments,
        *serviceAddress,
        *serviceName,
        iMaxMsgSize ) );
    // KErrCompletion is received, if for some attachments information note was shown
    if ( !( error == KErrNone || error == KErrCompletion ))
        {
        ErrorHandlerL( error, attachmentCount );
        }

    if( messageSender->Error() )
        {
        ErrorHandlerL( messageSender->Error(), attachmentCount );
        }
    else
        {
        // Get compression results
        TInt compressionOk = 0;
        TInt compressionFail = 0;
        
        messageSender->CompressionResults( compressionOk, compressionFail );
        
        if ( compressionFail > 0 )
            {
            ResolveCompressionQueryL(
                attachmentCount, compressionFail );
            }
        }
    // Cleanup: serviceName, serviceAddress, messageSender, attachments, *attachments
    CleanupStack::PopAndDestroy( cleanupItems );
    }

// -----------------------------------------------------------------------------
// CMmsDirectUpload::ValidateAttachmentsL
// Validates attachments for sending. DRM protection and MMS validity are
// checked. DRM query and error note is shown if needed. Return value of
// attachment arrays contains valid attachments. EFalse is returned if user
// cancels sending, otherwise ETrue is returned.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMmsDirectUpload::ValidateAttachmentsL(
    CArrayPtrFlat<CSendUiAttachment>* aAttachments )
    {
    TBool continueSending = ETrue;
   
    // Validate attachments and show query and error note if needed.
    if ( aAttachments->Count() > 0 )
        {
        CSendUiFileRightsEngine* fileRightsEngine =
            CSendUiFileRightsEngine::NewLC( iCoeEnv.FsSession() );

        fileRightsEngine->ConfirmDrmFileRightsL( aAttachments );
        fileRightsEngine->ConfirmMmsValidityL( aAttachments, KMmsDirectUpload );

        continueSending =
            fileRightsEngine->ShowDrmAndMmsInfoL( KMmsDirectUpload );

        CleanupStack::PopAndDestroy( fileRightsEngine );
        }

    return continueSending;
    }

// -----------------------------------------------------------------------------
// CMmsDirectUpload::ReadServiceDataL
// Reads service names from resource file and addresses from shared data or 
// from central repository.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmsDirectUpload::ReadServiceDataL()
    {
    // Read the maximum MMS message size (bytes).
    CRepository* mmsRepository = CRepository::NewL( KCRUidMmsEngine );
    mmsRepository->Get( KMmsEngineMaximumSendSize, iMaxMsgSize );
    delete mmsRepository;

    CRepository* senduiRepository = CRepository::NewLC( KCRUidSendUi );
    TInt i = 0;
    // Go through services and get settings for direct upload services.
    // There can be 0-4 direct upload services configured.
    for ( i = 0; i < KMmsUploadServiceCount; i++)
        {
        TInt serviceType = 0;
        TInt err = senduiRepository->Get( KSUMmsUploadType1 + i , serviceType );
        
        // Get settings and store those to service array.
        if ( !err && serviceType == KMmsDirectUploadService )
            {
            CSendingServiceInfo* serviceInfo = CSendingServiceInfo::NewL();
            CleanupStack::PushL( serviceInfo );

            HBufC* serviceAddress = HBufC::NewLC( KMaxServiceAddressLength );
            TPtr serviceAddressPtr = serviceAddress->Des();
            User::LeaveIfError( senduiRepository->Get( KSUMmsUploadAddress1 + i , serviceAddressPtr ) );
            serviceInfo->SetServiceAddressL( serviceAddressPtr );
            CleanupStack::PopAndDestroy( serviceAddress );

            HBufC* serviceName = HBufC::NewLC( KMaxServiceNameLength );
            TPtr serviceNamePtr = serviceName->Des();
            User::LeaveIfError( senduiRepository->Get( KSUMmsUploadServiceName1 + i , serviceNamePtr ) );
            serviceInfo->SetServiceNameL( serviceNamePtr );
            CleanupStack::PopAndDestroy( serviceName );
            
            HBufC* serviceMenuName = HBufC::NewLC( KMaxServiceMenuNameLength );
            TPtr serviceMenuNamePtr = serviceMenuName->Des();
            User::LeaveIfError( senduiRepository->Get( KSUMmsUploadServiceMenuName1 + i , serviceMenuNamePtr ) );
            serviceInfo->SetServiceMenuNameL( serviceMenuNamePtr );
            CleanupStack::PopAndDestroy( serviceMenuName );

            // Set service type.
            serviceInfo->SetServiceProviderId( KMmsDirectUpload );
            serviceInfo->SetServiceFeatures( CSendingServiceInfo::EServiceCanSendDirectly );

            // Set sending capabilities.
            TSendingCapabilities capabilities = 
                TSendingCapabilities( 0, KMaxTInt,
                    TSendingCapabilities::ESupportsAttachments );

            serviceInfo->SetServiceCapabilities( capabilities );

            TUid serviceId = KNullUid;
            switch ( i )
                {
                case 0:
                    {
                    serviceId = KMmsUploadService1Id;
                    break;
                    }
                case 1:
                    {
                    serviceId = KMmsUploadService2Id;
                    break;
                    }
                case 2:
                    {
                    serviceId = KMmsUploadService3Id;
                    break;
                    }
                case 3:
                    {
                    serviceId = KMmsUploadService4Id;
                    break;
                    }
                default:
                    break;
                }
            serviceInfo->SetServiceId( serviceId );

            iServiceArray.Append( serviceInfo );
            CleanupStack::Pop( serviceInfo );
            }
        }

    CleanupStack::PopAndDestroy( senduiRepository );
    }

// ---------------------------------------------------------
// CMmsDirectUpload::ShowConfirmationQueryL
// Shows confirmation query.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CMmsDirectUpload::ShowConfirmationQueryL(
    const TUint&    aQueryId,
    const TUint&    aQueryTextId,
    TInt            aQueryValue) const
    {
    HBufC* prompt = NULL;

    // Get text to be shown
    if ( aQueryValue )
        {
        prompt = StringLoader::LoadLC( aQueryTextId, aQueryValue );
        }
    else
        {
        prompt = StringLoader::LoadLC( aQueryTextId );
        }

    // Create dialog
    CAknQueryDialog* dlg = CAknQueryDialog::NewL( CAknQueryDialog::ENoTone );

    TInt result = dlg->ExecuteLD( aQueryId, prompt->Des() );

    CleanupStack::PopAndDestroy( prompt ); // prompt

    return ( result == EAknSoftkeyOk );
    }

// ---------------------------------------------------------
// CMmsDirectUpload::ErrorHandlerL
// Show confirmation query to the user if images have been compressed.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMmsDirectUpload::ErrorHandlerL( TInt aErr, TInt aFileCount ) const
    {
    if ( aErr )
        {
        switch ( aErr )
            {
            case KMmsErrorInvalidSettings:
            case KMmsErrorNoWAPAccessPoint:
            case KMmsErrorNoWapAp1:
                {
                ShowErrorNoteL( aFileCount > 1 ?
                    R_SENDUI_SETTINGS_NOT_OK_MANY : R_SENDUI_SETTINGS_NOT_OK );
                break;
                }
            default:
                {
                ShowConfirmationQueryL( R_MMS_DIRECT_UPLOAD_CONFIRMABLE_INFO,
                    aFileCount > 1 ? 
                    R_MMS_DIRECT_UPLOAD_CREATION_FAILED_ALL:
                    R_MMS_DIRECT_UPLOAD_CREATION_FAILED_ONE);         
                break;
                }
            }
        }
    }

// ---------------------------------------------------------
// CMmsDirectUpload::ResolveCompressionQueryL
// Show confirmation query to the user if images have been compressed.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMmsDirectUpload::ResolveCompressionQueryL(
    TInt aFileCount,
    TInt aCompressionFail ) const
    {
    // Show confirmation query to the user if images have been compressed.
    if ( aCompressionFail > 0 )
        {
        // Compression has failed for all files
        if ( aFileCount == aCompressionFail )
            {
            ShowConfirmationQueryL( R_MMS_DIRECT_UPLOAD_CONFIRMABLE_INFO,
                aCompressionFail > 1 ? 
                    R_MMS_DIRECT_UPLOAD_CREATION_FAILED_ALL:
                    R_MMS_DIRECT_UPLOAD_CREATION_FAILED_ONE);
            }
        else
            {
            ShowConfirmationQueryL( R_MMS_DIRECT_UPLOAD_CONFIRMABLE_INFO,
                R_MMS_DIRECT_UPLOAD_CREATION_FAILED_SOME );
            }
        }
    }

// ---------------------------------------------------------
// CMmsDirectUpload::ShowErrorNoteL
// Show error note to the user.
// (other items were commented in a header).
// ---------------------------------------------------------
//   
void CMmsDirectUpload::ShowErrorNoteL( TInt aResourceId ) const
    {
    HBufC* text = static_cast<CEikonEnv&>( iCoeEnv ).AllocReadResourceLC( aResourceId );

    CAknErrorNote* note = new (ELeave) CAknErrorNote;
    note->ExecuteLD(*text);

    CleanupStack::PopAndDestroy( text ); // text
    }
    
TUid CMmsDirectUpload::TechnologyTypeId( ) const
{
    return TUid::Uid( KSenduiTechnologyMmsUidValue );
}

//  End of File
