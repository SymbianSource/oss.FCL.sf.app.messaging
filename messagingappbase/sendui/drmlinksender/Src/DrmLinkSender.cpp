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
* Description:   Provides DRM link sender service.
*
*/



// INCLUDE FILES
#include    <eikenv.h>
#include    <CSendingServiceInfo.h>
#include    <SendUiConsts.h>
#include    <CMessageData.h>
#include    <senduisingleton.h>
#include    <smsclnt.h>
#include    <mtclreg.h>                         // CClientMtmRegistry
#include    <msvids.h>
#include    <StringLoader.h>
#include    <coemain.h>
#include    <mtmuidef.hrh>
#include    <mtuireg.h>                 // MTM UI registery
#include    <DrmLinkSenderUi.rsg>
#include    <muiumsvuiserviceutilitiesinternal.h>
#include    <s32mem.h>
#include    <f32file.h>
#include    <data_caging_path_literals.hrh>
#include    <caf/caf.h>
#include    <unieditorconsts.h>
#include    <unimsventry.h>

#include    "DrmLinkSender.h"
#include    <TSendingCapabilities.h>
#include    "SendUiOperationWait.h"

// CONSTANTS
_LIT( KDrmLinkSenderUiResource, "DrmLinkSenderUi.rsc");
//const TInt KDLSRichTextStoreGranularity = 512; // Removal of Warning:  #177-D
// The string is defined in another .cpp too -> should be moved to a .h file
const TInt KSendUiProgressStringSize = 256;
const TInt KSenduiUrlMaxLen = 1024;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDrmLinkSender::CDrmLinkSender
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

CDrmLinkSender::CDrmLinkSender( CCoeEnv& aCoeEnv, CSendUiSingleton& aSingleton ) : 
    CSendingService( aCoeEnv, aSingleton )
    {
    }

// -----------------------------------------------------------------------------
// CDrmLinkSender::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDrmLinkSender::ConstructL( )
    {
    iResourceLoader = new(ELeave)RConeResourceLoader( iCoeEnv );
    
    InitializeServiceL();
    }

// -----------------------------------------------------------------------------
// CDrmLinkSender::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDrmLinkSender* CDrmLinkSender::NewL( TSendingServiceParams* aParams )
    {
    CDrmLinkSender* self = new(ELeave) CDrmLinkSender( aParams->iCoeEnv, aParams->iSingleton ); 

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// Destructor
CDrmLinkSender::~CDrmLinkSender()
    {
    // Unload UI resource file.
    if ( iResourceLoader )
        {
        iResourceLoader->Close();
        delete iResourceLoader;
        }
    iServiceArray.ResetAndDestroy();
    iServiceArray.Close();
    delete iClientMtm;
    delete iMtmUi;
    }


// ---------------------------------------------------------
// CDrmLinkSender::ServiceProviderId
// Returns the id of the service provider.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TUid CDrmLinkSender::ServiceProviderId() const
    {
    return KDrmLinkSender;
    }

// -----------------------------------------------------------------------------
// CDrmLinkSender::PopulateServicesListL
// Populates given list with the services provided by this plugin.
// The ownership of the pointed objects remains.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDrmLinkSender::PopulateServicesListL(
    RPointerArray<CSendingServiceInfo>& aList )
    {
    for ( TInt cc = 0; cc < iServiceArray.Count(); cc++ )
        {
        User::LeaveIfError( aList.Append( iServiceArray[cc] ) );
        }
    }


// -----------------------------------------------------------------------------
// CDrmLinkSender::CreateAndSendMessageL
// Creates to outbox as many MMS messages as there are files to be send.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDrmLinkSender::CreateAndSendMessageL(
    TUid                    aServiceUid,
    const CMessageData*     aMessageData,
    TBool /*aLaunchEditorEmbedded */)
    {
    HBufC8* contentUrl = NULL;

    if ( !aMessageData ||
         aServiceUid != KDrmLinkSender ||
         iServiceArray.Count() == 0 )
        {
        return;
        }

    const CDesCArray& attachmentPaths = aMessageData->AttachmentArray();
    const RArray<RFile>& attachments = aMessageData->AttachmentHandleArray();

    if ( attachmentPaths.MdcaCount() == 1 )
        {
        contentUrl = DrmContentUrlLC( attachmentPaths.MdcaPoint(0), iCoeEnv.FsSession() );
        }
    else if ( attachments.Count() == 1 )
        {
        contentUrl = DrmContentUrlLC( attachments[0] );
        }
    else
        {
        User::Leave( KErrArgument );
        return;
        }

    CreateMessageAndLaunchEditorL( *contentUrl );
    CleanupStack::PopAndDestroy( contentUrl );
    }


// -----------------------------------------------------------------------------
// CDrmLinkSender::DrmContentUrlLC
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CDrmLinkSender::DrmContentUrlLC( const TDesC& aFilePath, RFs& aFs )
    {
    RFile file;
    CleanupClosePushL( file );

    TInt err = file.Open( aFs, aFilePath, EFileShareReadersOnly );
    if ( err )
        {
        User::LeaveIfError( file.Open( aFs, aFilePath, EFileShareAny ) );
        }

    HBufC8* contentUrl = DrmContentUrlLC( file );
    CleanupStack::Pop( contentUrl );
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PushL( contentUrl );
    return contentUrl;
    }

// -----------------------------------------------------------------------------
// CDrmLinkSender::DrmContentUrlLC
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CDrmLinkSender::DrmContentUrlLC( const RFile& aFile )
    {
    HBufC8* contentUrl = NULL;
    TBuf<KSenduiUrlMaxLen> temp;

    ContentAccess::CData* content = ContentAccess::CData::NewLC(
        const_cast<RFile&> (aFile),
        ContentAccess::KDefaultContentObject,
        ContentAccess::EPeek );
    User::LeaveIfError( content->GetStringAttribute( ContentAccess::EContentURI, temp ) );
    CleanupStack::PopAndDestroy( content );

    if ( temp.Length() > 0 )
        {
        contentUrl = HBufC8::NewLC( temp.Length() );
        contentUrl->Des().Copy( temp );
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    return contentUrl;
    }


// -----------------------------------------------------------------------------
// CDrmLinkSender::InitializeServiceL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDrmLinkSender::InitializeServiceL()
    {
    // Load UI resource file.
    TFileName resourceFile;
    TParse* fp = new(ELeave) TParse(); 
    fp->Set( KDrmLinkSenderUiResource, &KDC_RESOURCE_FILES_DIR, NULL );
    resourceFile = fp->FullName();
    delete fp;
    fp = NULL;

    iResourceLoader->OpenL( resourceFile );

    CSendingServiceInfo* serviceInfo = CSendingServiceInfo::NewLC();

    // Set service type.
    serviceInfo->SetServiceProviderId( KDrmLinkSender );
    serviceInfo->SetServiceId( KDrmLinkSender );    

    // Read service name from resource
    HBufC* serviceName = StringLoader::LoadLC( R_DRM_LINK_SENDER_MENU_NAME );
    serviceInfo->SetServiceMenuNameL( *serviceName );

    CleanupStack::PopAndDestroy( serviceName );

    // Sending capabilities doesn't need to set.
    // Content URL is used for filtering instead of TSendingCapabilities.

    iServiceArray.Append( serviceInfo );
    CleanupStack::Pop( serviceInfo );        
    }

// -----------------------------------------------------------------------------
// CDrmLinkSender::CreateMessageAndLaunchEditorL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDrmLinkSender::CreateMessageAndLaunchEditorL( HBufC8& aContentUrl )
    {
    if (!iClientMtm )
        {        
        // Uni client side MTM
        iClientMtm = iSingleton.ClientMtmRegistryL().NewMtmL( KSenduiMtmUniMessageUid );
        }
        
    // Get Uni MTM UI and set its context
    iMtmUi = iSingleton.MtmUiRegistryL().NewMtmUiL( *iClientMtm );

    // Create draft entry for the message
    CMsvEntry* draftEntry = iSingleton.MsvSessionL().GetEntryL( KMsvDraftEntryId );
    CleanupStack::PushL( draftEntry );
    iClientMtm->SetCurrentEntryL( draftEntry ); //iClientMtm takes ownership
    CleanupStack::Pop( draftEntry );
    
    // Create the new message
    TMsvId serviceId = MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
                            iSingleton.MsvSessionL(), KSenduiMtmUniMessageUid, ETrue);
    iClientMtm->CreateMessageL( serviceId );

    // Set message entry data      
    TMsvEntry entry = iClientMtm->Entry().Entry();
    
    TUniMsvEntry::SetCurrentMessageType(
            entry, EUniMessageCurrentTypeSms );
    TUniMsvEntry::SetMessageTypeLocked( entry, ETrue );
    iClientMtm->Entry().ChangeL( entry );

    entry.iDate.UniversalTime();
    entry.SetVisible( EFalse );
    entry.SetAttachment( EFalse );
    entry.SetInPreparation( ETrue );
    iClientMtm->Entry().ChangeL( entry );

    // Add DRM content link to body text
    AddAttachmentsL(aContentUrl);

    // Save message.
    iClientMtm->SaveMessageL();
    entry = iClientMtm->Entry().Entry();
    iClientMtm->SwitchCurrentEntryL( entry.Id() );

    // Launch editor embedded
    TUint preferences = iMtmUi->Preferences();
    preferences |= EMtmUiFlagEditorPreferEmbedded;
    iMtmUi->SetPreferences( preferences );

    // Edit the message.
    CSendUiOperationWait* waiter = CSendUiOperationWait::NewLC();
    CMsvOperation* op = iMtmUi->EditL( waiter->iStatus );
    CleanupStack::PushL( op );

    waiter->Start( op );

    TBuf8<KSendUiProgressStringSize> progressBuf = op->ProgressL();
    iMtmUi->DisplayProgressSummary( progressBuf );

    CleanupStack::PopAndDestroy( 2 ); // waiter, op
    }

// -----------------------------------------------------------------------------
// CDrmLinkSender::AddAttachmentsL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDrmLinkSender::AddAttachmentsL( HBufC8& aContentUrl )
    {
    TInt cleanupItems(0);
    CMessageData* msgData = CMessageData::NewLC();
    cleanupItems++;
    
    TPtr8 string(aContentUrl.Des());
    CRichText* text = CRichText::NewL(
        static_cast<CEikonEnv&>( iCoeEnv ).SystemParaFormatLayerL(),        
        static_cast<CEikonEnv&>( iCoeEnv ).SystemCharFormatLayerL() );
    CleanupStack::PushL( text );
    cleanupItems++;
    text->Reset();

    HBufC16* bufConv = HBufC16::NewLC(string.Length());
    cleanupItems++;
    TPtr16 string16(bufConv->Des());
    string16.Copy(string);
    text->InsertL( 0, string16 );

    // Set body text
    msgData->SetBodyTextL( text );
    
    CArrayPtrFlat<CSendUiAttachment>* attachments = NULL;    
    // Get attachments
    if ( msgData )
        {
        attachments = CSendUiAttachment::InitAttachmentArrayLCC( 
                msgData->AttachmentArray(), 
                msgData->AttachmentHandleArray(),
                iCoeEnv.FsSession() );
        cleanupItems += 2;
        }
    CSendUiAttachment* atta = CSendUiAttachment::NewLC( *text, iCoeEnv.FsSession() );
    attachments->AppendL( atta );
    CleanupStack::Pop( atta );	

    TInt attachmentCount = attachments->Count();
    TInt i;
    for ( i = 0; i < attachmentCount ; i++ )
        {
        CSendUiAttachment& attachment = *(attachments->At(i ));
        AddAttachmentL( &attachment );
        }
    CleanupStack::PopAndDestroy( cleanupItems ); //  msgData, text, bufConv, attachments
    }

// -----------------------------------------------------------------------------
// CDrmLinkSender::AddAttachmentL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDrmLinkSender::AddAttachmentL( CSendUiAttachment* aAttachment)
    {
    CSendUiOperationWait* waiter = CSendUiOperationWait::NewLC();

    iClientMtm->AddLinkedAttachmentL(
            *(aAttachment->Path() ), 
            aAttachment->MimeType(), 
            0, 
            waiter->iStatus );
    TBool success = waiter->Start( iClientMtm );
    CleanupStack::PopAndDestroy( waiter );
}

// -----------------------------------------------------------------------------
// TechnologyType
// -----------------------------------------------------------------------------
//
TUid CDrmLinkSender::TechnologyTypeId( ) const
    {
    return KNullUid;
    }
    
//  End of File
