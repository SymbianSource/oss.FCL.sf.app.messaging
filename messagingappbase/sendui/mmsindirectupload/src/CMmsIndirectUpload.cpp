/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides MMS indirect upload service.
*
*/




// INCLUDE FILES

#include    <CSendingServiceInfo.h>
#include    <SendUiConsts.h>
#include    <CMessageData.h>
#include    <senduisingleton.h>
#include    <MmsConst.h>
#include    <bautils.h>                     // BaflUtils
#include    <barsc.h>                       // RResourceFile
#include    <f32file.h>                     // TParse
#include    <TSendingCapabilities.h>
#include    <SenduiMtmUids.h>

#include    "CMmsIndirectUpload.h"
#include    "SendUiOperationWait.h"
#include    "SendUiFileRightsEngine.h"
#include    "CSendUiAttachment.h"

#include    <data_caging_path_literals.hrh>

// CONSTANTS
const TInt KMaxMmsUploadServiceCount    = 4;
const TInt KMmsIndirectUploadService    = 2;
const TInt KMaxServiceAddressLength     = 512;
const TInt KMaxServiceNameLength        = 128;
const TInt KMaxServiceMenuNameLength    = 128;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsIndirectUpload::CMmsIndirectUpload
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsIndirectUpload::CMmsIndirectUpload( CCoeEnv& aCoeEnv, CSendUiSingleton& aSingleton )
    : CSendingService( aCoeEnv, aSingleton )
    {
    }

// -----------------------------------------------------------------------------
// CMmsIndirectUpload::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmsIndirectUpload::ConstructL()
    {
    ReadServiceDataL();
    }

// -----------------------------------------------------------------------------
// CMmsIndirectUpload::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmsIndirectUpload* CMmsIndirectUpload::NewL( TSendingServiceParams* aParams )
    {
    CMmsIndirectUpload* self = new(ELeave) CMmsIndirectUpload( aParams->iCoeEnv, aParams->iSingleton ); 

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// Destructor
CMmsIndirectUpload::~CMmsIndirectUpload()
    {
    iServiceArray.ResetAndDestroy();
    iServiceArray.Close();
    }


// ---------------------------------------------------------
// CMmsIndirectUpload::ServiceProviderId
// Returns the id of the service provider.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TUid CMmsIndirectUpload::ServiceProviderId() const
    {
    return KMmsIndirectUpload;
    }

TUid CMmsIndirectUpload::TechnologyTypeId( ) const
{
    return TUid::Uid( KSenduiTechnologyMmsUidValue );
}

// -----------------------------------------------------------------------------
// CMmsIndirectUpload::CreateAndSendMessageL
// Creates message to be sent and opens MMS editor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmsIndirectUpload::CreateAndSendMessageL(
    TUid                    aServiceUid,
    const CMessageData*     aMessageData,
    TBool /*aLaunchEditorEmbedded */)
    {
    TInt i = 0;

    RSendAs sendAsSession;
    User::LeaveIfError( sendAsSession.Connect() );
    TInt cleanupItems(0);
    CleanupClosePushL( sendAsSession );
    cleanupItems++;
    if ( !CheckMmsServiceAvailabilityL( sendAsSession ) )
        {
        CleanupStack::PopAndDestroy( &sendAsSession ); // sendAsSession
        return;
        }

    RSendAsMessage sendAsMessage;
    CleanupClosePushL( sendAsMessage );
    cleanupItems++;
    sendAsMessage.CreateL( sendAsSession, KSenduiMtmUniMessageUid );
    // BIO type identifies MMS message as upload message.
    sendAsMessage.SetBioTypeL( KUidMsgSubTypeMmsUpload );

    // Set service address
    for ( i=0; i < iServiceArray.Count(); i++)
        {
        if ( (*iServiceArray[i]).ServiceId() == aServiceUid)
            {
            TPtrC address = (*iServiceArray[i]).ServiceAddress();

            if ( address != KNullDesC )
                {
                sendAsMessage.AddRecipientL(
                    address,
                    (*iServiceArray[i]).ServiceName(),
                    RSendAsMessage::ESendAsRecipientTo );
                }
            else
                {
                User::Leave( KErrNotFound );
                }
            break;
            }
        }

    CArrayPtrFlat<CSendUiAttachment>* attachments(NULL);

    // Get attachments
    if ( aMessageData )
        {
        attachments = CSendUiAttachment::InitAttachmentArrayLCC( 
            aMessageData->AttachmentArray(), 
            aMessageData->AttachmentHandleArray(),
            iCoeEnv.FsSession() );
        cleanupItems += 2;
        }
    if ( aMessageData )
        {
        if ( !ValidateAttachmentsL( attachments ) )
            {
            // attachments, sendAsMessage, sendAsSession
            CleanupStack::PopAndDestroy( cleanupItems, &sendAsSession ); 
            return;
            }

        const CRichText* bodyText = aMessageData->BodyText();
        if ( bodyText->DocumentLength() > 0 )
            {
            CSendUiAttachment* atta = CSendUiAttachment::NewLC( *bodyText, iCoeEnv.FsSession() );
            attachments->AppendL( atta );
            CleanupStack::Pop( atta );
            }
        
        for ( i = 0; i < attachments->Count(); i++ )
            {
            CSendUiOperationWait* waiter = CSendUiOperationWait::NewLC();
            CSendUiAttachment& attachmentPacket = *(attachments->At(i));

            if ( attachmentPacket.Type() == CSendUiAttachment::EAttachmentPath )
            	{
                sendAsMessage.AddAttachment(
                    *(attachmentPacket.Path()), 
                    attachmentPacket.MimeType(),
                    waiter->iStatus );
				}
			else
				{
				RFile attachment;
	            attachment.Duplicate( *(attachmentPacket.Handle()) );
	            sendAsMessage.AddAttachment( 
	                attachment, 
	                attachmentPacket.MimeType(),
	                waiter->iStatus );
				}	
            
            waiter->Start( (CActive*)NULL );
            CleanupStack::PopAndDestroy( waiter );
            }

        }
    TInt attachmentCount( 0 );
    if ( attachments )
        {
        attachmentCount = attachments->Count();    
        CleanupStack::PopAndDestroy( 2, attachments );
        cleanupItems -= 2;
        }
        
    if (  attachmentCount > 0 )
        {
        sendAsMessage.LaunchEditorAndCloseL(); // sendAsMessage is closed
        CleanupStack::Pop( &sendAsMessage );
        }
    else
        {
        CleanupStack::PopAndDestroy( &sendAsMessage); // sendAsMessage
        }
    cleanupItems--;
    CleanupStack::PopAndDestroy( cleanupItems, &sendAsSession );
    }

// -----------------------------------------------------------------------------
// CMmsIndirectUpload::ValidateAttachmentsL
// Validates attachments for sending. DRM protection and MMS validity are
// checked. DRM query and error note is shown if needed. Return value of
// attachment arrays contains valid attachments.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMmsIndirectUpload::ValidateAttachmentsL( CArrayPtrFlat<CSendUiAttachment>* aAttachments )
    {
    TBool continueSending( ETrue );
  
  
    // Validate attachments and show query and error note if needed.
    if ( aAttachments->Count() > 0 )
        {
        CSendUiFileRightsEngine* fileRightsEngine =
            CSendUiFileRightsEngine::NewLC( iCoeEnv.FsSession() );

        fileRightsEngine->ConfirmDrmFileRightsL( aAttachments );
        fileRightsEngine->ConfirmMmsValidityL( aAttachments );
        
        continueSending = fileRightsEngine->ShowDrmAndMmsInfoL( KMmsIndirectUpload );

        CleanupStack::PopAndDestroy( fileRightsEngine );
        }
    return continueSending;
    }

// -----------------------------------------------------------------------------
// CMmsIndirectUpload::PopulateServicesListL
// Populates given list with the services provided by this plugin.
// The ownership of the pointed objects remains.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmsIndirectUpload::PopulateServicesListL(
    RPointerArray<CSendingServiceInfo>& aList )
    {
    for ( TInt cc = 0; cc < iServiceArray.Count(); cc++ )
        {
        User::LeaveIfError( aList.Append( iServiceArray[cc] ) );         
        }
    }

// -----------------------------------------------------------------------------
// CMmsIndirectUpload::ReadServiceDataL
// Reads service names from resource file and addresses from shared data or
// central repository.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmsIndirectUpload::ReadServiceDataL()
    {
    // Read the maximum MMS message size (bytes).
    TInt maxMsgSize = 0;
    CRepository* mmsRepository = CRepository::NewL( KCRUidMmsEngine );
    mmsRepository->Get( KMmsEngineMaximumSendSize, maxMsgSize );
    delete mmsRepository;

    CRepository* senduiRepository = CRepository::NewLC( KCRUidSendUi );
    TInt i = 0;
    // Go through services and get settings for indirect upload services.
    // There can be 0-4 indirect upload services configured.
    for ( i = 0; i < KMaxMmsUploadServiceCount; i++)
        {
        TInt serviceType = 0;
        TInt err = senduiRepository->Get( KSUMmsUploadType1 + i , serviceType );
        
        // Get settings and store those to service array.
        if ( !err && serviceType == KMmsIndirectUploadService )
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
            serviceInfo->SetServiceProviderId( KMmsIndirectUpload );

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

// -----------------------------------------------------------------------------
// CMmsIndirectUpload::CheckMmsServiceAvailabilityL
// Checks MMS service availability.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMmsIndirectUpload::CheckMmsServiceAvailabilityL(
    RSendAs& aSendAsSession )
    {
    TInt i = 0;
    
    CSendAsMessageTypes* msgTypes = CSendAsMessageTypes::NewL();
    CleanupStack::PushL( msgTypes );

    aSendAsSession.FilteredMessageTypesL( *msgTypes );

    for ( i = 0; i < msgTypes->Count(); i++ )
        {
        if (  msgTypes->MessageTypeUid( i ) == KSenduiMtmMmsUid )
            {
            CleanupStack::PopAndDestroy( msgTypes );
            return ETrue;
            }
        }

    return EFalse;
    }

//  End of File
