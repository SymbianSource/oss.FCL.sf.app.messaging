/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   SendUI sending service provider that manages all MTM based
*                sending services.
*
*/



#include <smsclnt.h>
#include <mtclreg.h>                         // CClientMtmRegistry
#include <msvids.h>
#include <StringLoader.h>
#include <coemain.h>
#include <mtmuibas.h>                        // CBaseMtmUi
#include <mtmuidef.hrh>
#include <mtuireg.h>                 // MTM UI registery
#include <MuiuMsvUiServiceUtilities.h> 
#include <muiumsvuiserviceutilitiesinternal.h> 
#include <s32mem.h>
#include <f32file.h>
#include <MtmExtendedCapabilities.hrh>
#include <mtmuids.h>
#include <eiksrv.h>
#include <PhCltTypes.h>   
#include <btcmtm.h>
#include <AiwCommon.hrh>
#include <AiwServiceHandler.h>
#include <AknWaitDialog.h>          // CAknWaitDialog
#include <AknGlobalNote.h>
#include <AknQueryDialog.h>
#include <AknsUtils.h>
#include <e32property.h>
#include <telremotepartyinformationpskeys.h>
#include <mtudcbas.h>
#include <Sendnorm.rsg>
#include <MsgBioUids.h>             // KMsgBioUidVCard, KMsgBioUidVCalendar
#include <mtudreg.h>
#include <mtud.hrh>                // EMtudCommandSendAs
#include <featmgr.h>               // Feature Manager
#include <unieditorconsts.h>
#include <unimsventry.h>

#include <mcesettingsemailsel.h>
#include <CSendingServiceInfo.h>
#include <SendUiConsts.h>
#include <CMessageData.h>
#include <TSendingCapabilities.h>
#include <senduisingleton.h>
#include <messaginginternalpskeys.h>
#include "SendUiOperationWait.h"
#include "SendUiLog.h"              // Logging
#include "genericmtmplugin.h"
#include "SendUiFileRightsEngine.h"
#include "SendUiDummyMtm.h"
#include "CSendUiAttachment.h"
#include "GenericMtmPluginUids.hrh"

const TInt KSendUiProgressStringSize = 256;
const TInt KSendUiRichTextStoreGranularity = 512;
// For address information separation (start)
const TUint KSendUiMsgAddressStartChar         ('<');
// For address information separation (end)
const TUint KSendUiMsgAddressEndChar           ('>');

const TUint KSendUiFeatureSFI           = 0x2;
const TUint KSendUiFeatureMMS           = 0x4;
const TUint KSendUiFeatureSelectableEmail	= 0x8;

const TUint KSeconds = 1000000;
const TUid KMailTechnologyTypeUid = { 0x10001671 };

// ======== LOCAL FUNCTIONS ========

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGenericMtmPlugin* CGenericMtmPlugin::NewL( TSendingServiceParams* aParams )
    {
    CGenericMtmPlugin* self = new(ELeave) CGenericMtmPlugin( aParams->iCoeEnv, aParams->iSingleton ); 

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGenericMtmPlugin::~CGenericMtmPlugin()
    {
    iServiceArray.ResetAndDestroy();
    iServiceArray.Close();
    
    delete iMtmUi;
    delete iClientMtm;      
    delete iStore;      
    }


// ---------------------------------------------------------
// CGenericMtmPlugin::InitializeSendingL
// ---------------------------------------------------------
//
TBool CGenericMtmPlugin::InitializeSendingL( )
    {
    LOGTEXT(_L8("CGenericMtmPlugin::InitializeSendingL >>"));
    
    if ( iIsSending )
        {
        LOGTEXT(_L8("CGenericMtmPlugin::Sending canceled, previous sending in progress."));
        return EFalse;
        }

    // Check if MMC mounted or switched to another MMC
    TDriveUnit currentDrive = iSingleton.MsvSessionL().CurrentDriveL();

    if ( currentDrive != EDriveC )
        {
        TBool storeOnMMC( EFalse );
        TRAPD ( error, storeOnMMC = iSingleton.MsvSessionL().MessageStoreDrivePresentL( ) );
        if ( !iSingleton.MsvSessionL().DriveContainsStoreL( currentDrive ) || !storeOnMMC || error )
            {
            //Change message store to phones internal memory(EDriveC)
            MsvUiServiceUtilitiesInternal::ChangeMessageStoreToPhoneL( iSingleton.MsvSessionL() );
            }
        }

    LOGTEXT(_L8("CGenericMtmPlugin::InitializeSendingL <<"));
    
    return ETrue;
    }

// -----------------------------------------------------------------------------
// Populates given list with the services provided by this plugin.
// The ownership of the pointed objects remains.
// -----------------------------------------------------------------------------
//
void CGenericMtmPlugin::PopulateServicesListL(
    RPointerArray<CSendingServiceInfo>& aList )
    {
    ScanMtmsL( iServiceArray );
    
    TInt i = iServiceArray.Count();
    while( i-- )
        {
        aList.Append( iServiceArray[i]);
        }
    }

// ---------------------------------------------------------
// CGenericMtmPlugin::CreateAndSendMessageL
// ---------------------------------------------------------
//
void CGenericMtmPlugin::CreateAndSendMessageL(
    TUid                            aMtmUid,
    const CMessageData*             aMessageData,
    TBool                           aLaunchEditorEmbedded )
    {
    LOGTEXT(_L8("CGenericMtmPlugin::CreateAndSendMessageL >>"));

    if ( aMtmUid != KSenduiMtmBtUid &&
     MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(
        iSingleton.MsvSessionL(), 0 ) )
	    {
	    User::Leave(KErrDiskFull);
	    }
    
    TUid tempUid( KNullUid );
    if ( aMtmUid == KSenduiMtmSmsUid ||
         aMtmUid == KSenduiMtmMmsUid )
        {
        tempUid = aMtmUid;
        aMtmUid = KSenduiMtmUniMessageUid;
        // TODO: Use original MTM for checking attachments?
        // TODO: Use original MTM for "InitializeSending" (does some
        //       SmartMessaging checks)?
        }
    TMsvId service;
    TBool onlineSharing = EFalse;
    TBool doContinue = EFalse;

    iClientMtm = iSingleton.ClientMtmRegistryL().NewMtmL( aMtmUid );
    iMtmUi = iSingleton.MtmUiRegistryL().NewMtmUiL( *iClientMtm );
    
    if ( IsSupported( KSendUiFeatureSelectableEmail ) )
    	{
    	// from OnlineMtm.rss, mtm_type_uid
    	onlineSharing = ( aMtmUid == KSenduiMtmOnlineAlbumUid) ? 
    					ETrue : EFalse;
    	service = GetAndValidateServiceL( aMtmUid, aLaunchEditorEmbedded,
    									  doContinue );
    	if( service == KMsvUnkownServiceIndexEntryIdValue &&
    		!onlineSharing )
    		{
    		// cancel pressed on "selecta email account" query
    		return;
    		}
    	}
    TInt cleanupItems(0);

    if ( !InitializeSendingL( ) )
        {
        return;
        }

    CArrayPtrFlat<CSendUiAttachment>* attachments = NULL;
    // Get attachments
    if ( aMessageData )
    	{
   	    attachments = CSendUiAttachment::InitAttachmentArrayLCC( 
	        aMessageData->AttachmentArray(), 
	        aMessageData->AttachmentHandleArray(),
	        iCoeEnv.FsSession() );
    	cleanupItems += 2;
    	}
    	
    // Ensures proper sending state if leave happens.
    iIsSending = ETrue;
    CleanupStack::PushL(TCleanupItem(ResetIsSending, (TAny*)&iIsSending));
    cleanupItems++;
    
    //iClientMtm = iSingleton.ClientMtmRegistryL().NewMtmL( aMtmUid );
    //iMtmUi = iSingleton.MtmUiRegistryL().NewMtmUiL( *iClientMtm );

    CBaseMtmUiData* mtmUiData = iSingleton.MtmUiDataRegistryL().NewMtmUiDataLayerL( aMtmUid );
    CleanupStack::PushL( mtmUiData );
    cleanupItems++;
        
    if ( aMessageData &&
         !ValidateAttachmentsL(
                aMtmUid,
                *mtmUiData,
                attachments ))
        {
        // User canceled sending.
        CleanupStack::PopAndDestroy( cleanupItems ); // TCleanupItem
        return;
        }

    // Show wait note if needed
    TBool showWaitNote = EFalse;
    if ( aMessageData )
        {
        showWaitNote = 
        IsWaitNoteNeededL( aMtmUid, *aMessageData, attachments->Count() );
    
        if ( showWaitNote )
            {
            ShowWaitNoteLC();
            cleanupItems++;
            }
        }
        TBool success = EFalse;
    if ( IsSupported( KSendUiFeatureSelectableEmail ) )
    	{
    	if ( !onlineSharing )
    		{
    		if ( doContinue )
    			{
    			CreateMessageL( aMtmUid, service );
    			//If doContinue, service already validated to be launched 
    			//embedded
    			success = ETrue; 
    			}
    		} 
    	else	
    		{
    	    success = CreateMessageL( aMtmUid, aLaunchEditorEmbedded );
    	    }       	
    	}
    else
        {
        success = CreateMessageL( aMtmUid, aLaunchEditorEmbedded );
        }
    
    if ( success && tempUid != KNullUid )
        {
        TMsvEntry entry = iClientMtm->Entry().Entry();
        TUniMsvEntry::SetCurrentMessageType(
            entry,
            tempUid == KSenduiMtmSmsUid
                ? EUniMessageCurrentTypeSms
                : EUniMessageCurrentTypeMms );
        TUniMsvEntry::SetMessageTypeLocked( entry, ETrue );
        iClientMtm->Entry().ChangeL( entry );
        }

    // Set message data
    if ( aMessageData && success)
        {
        SetRecipientsL( aMtmUid, *aMessageData );
        SetBodyL( *mtmUiData, *aMessageData, attachments );
 
        if ( attachments->Count() > 0 )
            {
            if ( aMtmUid == KSenduiMtmBtUid )
                {
                // Bluetooth sending over AIW
                SendBtMessageL( attachments );
                CleanupStack::PopAndDestroy( cleanupItems ); // TCleanupItem
                return;
                }
            else
                {
                success = AddAttachmentsL( *mtmUiData, attachments );
                }
            }

        if ( success && ( aMtmUid == KSenduiMtmSmtpUid ||
             aMtmUid == KSenduiMtmMmsUid ||
             aMtmUid == KSenduiMtmUniMessageUid ))
            {
            HBufC* subject = aMessageData->Subject().AllocLC();
            cleanupItems++;
            iClientMtm->SetSubjectL( *subject );
            }
        }
     if ( aMtmUid == KSenduiMtmBtUid )
        {
        success = EFalse; // bluetooth can launch editor only from SendBtMessageL                
        }
    if ( success )
        {
        TInt attachmentsCount = attachments ? attachments->Count(): 0;
        LaunchEditorL( attachmentsCount, aLaunchEditorEmbedded );
        }
    
    CleanupStack::PopAndDestroy( cleanupItems ); // TCleanupItem, attachments
    LOGTEXT(_L8("CGenericMtmPlugin::CreateAndSendMessageL <<"));
    }

// ---------------------------------------------------------
// CGenericMtmPlugin::ServiceProviderId
// Returns the id of the service provider.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TUid CGenericMtmPlugin::ServiceProviderId() const
    {
    return KGenericMtmPlugin;
    }

// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGenericMtmPlugin::CGenericMtmPlugin( CCoeEnv& aCoeEnv, CSendUiSingleton& aSingleton ) : 
    CSendingService( aCoeEnv, aSingleton ),
    iSendUiFeatureFlags(0)
    {
    }

// -----------------------------------------------------------------------------
// ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGenericMtmPlugin::ConstructL()
    {
    iStore = CBufStore::NewL(KSendUiRichTextStoreGranularity);
    
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdSendFileInCall ) )
        {
        iSendUiFeatureFlags |= KSendUiFeatureSFI;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdMMS ) )
        {
        iSendUiFeatureFlags |= KSendUiFeatureMMS;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdSelectableEmail ) )
    	{
    	iSendUiFeatureFlags |= KSendUiFeatureSelectableEmail;
    	}
    FeatureManager::UnInitializeLib();
 
    }

// ---------------------------------------------------------
// Start asyncronous MTM scanning
// ---------------------------------------------------------
//
void CGenericMtmPlugin::ScanMtmsL( RPointerArray<CSendingServiceInfo>& aList )
    {
    TInt regMtmIndex = 0;
    TInt err;
    TInt count( iSingleton.MtmUiDataRegistryL().NumRegisteredMtmDlls() );
    while ( regMtmIndex < count )
        {
        TUid mtmUid = iSingleton.MtmUiDataRegistryL().MtmTypeUid(regMtmIndex);
        LOGTEXT3(_L("Loading MTM index %d, id %d"), regMtmIndex, mtmUid );
        TRAP( err, LoadMtmL( iSingleton.MtmUiDataRegistryL().MtmTypeUid(regMtmIndex), aList ) );
        LOGTEXT2( _L("Loading MTM done, code %d"), err );
        regMtmIndex++;
        }
    }


// ---------------------------------------------------------
// ValidateAttachmentsL
// ---------------------------------------------------------
//
TBool CGenericMtmPlugin::ValidateAttachmentsL(
    TUid                    aMtmUid,
    CBaseMtmUiData&         aMtmUiData,    
    CArrayPtrFlat<CSendUiAttachment>* aAttachments )
    {
    LOGTEXT(_L8("CGenericMtmPlugin::ValidateAttachmentsL >>"));
    TBool continueSending = ETrue;
	
	// Validate attachments and show query and error note if needed.
    if ( aAttachments->Count() > 0 )
        { 
        CSendUiFileRightsEngine* fileRightsEngine =
            CSendUiFileRightsEngine::NewLC( iCoeEnv.FsSession() );
        if ( aMtmUid == KSenduiMtmPostcardUid &&
            IsPostcardContactLink( aAttachments ) )
            {
            // Postcard special case. ConfirmDrmFileRightsL() fails for contact
            // link file sent from Phonebook to Postcard. Therefore the check
            // is bypassed. Postcard treats the special file as a contact link.
            // Therefore bypassing drm check in this case is not a
            // vulnerability.
            }
        else
            {
            fileRightsEngine->ConfirmDrmFileRightsL( aAttachments );
            }
        // Confirm and filter away non-MMS files, if sending via MMS.
        if ( aMtmUid == KSenduiMtmMmsUid ||
             aMtmUid == KSenduiMtmAudioMessageUid ||
             ( aMtmUid == KSenduiMtmUniMessageUid &&
               IsSupported( KSendUiFeatureMMS ) ) )
            {
            fileRightsEngine->ConfirmMmsValidityL( aAttachments );
            }

        continueSending = fileRightsEngine->ShowDrmAndMmsInfoL();

        if ( aAttachments->Count() <= 0 )
            {
            continueSending = EFalse; // No files to send after filtering.
            }

        if ( continueSending )
            {
            TInt messageSize = 0;

            TUid uidQuery = { KUidMsvMtmQuerySupportLinks };
            TInt supportsLinks = 0;
            aMtmUiData.QueryCapability( uidQuery, supportsLinks );
            
            messageSize = fileRightsEngine->CalculateTotalSizeOfFiles(
                aAttachments,
                supportsLinks );
    
            if ( aMtmUid != KSenduiMtmBtUid &&
                 MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(
                    iSingleton.MsvSessionL(), messageSize ) )
                {
                User::Leave(KErrDiskFull);
                }
            }
        CleanupStack::PopAndDestroy( fileRightsEngine );
        }
    LOGTEXT(_L8("CGenericMtmPlugin::ValidateAttachmentsL <<"));
    return continueSending;
    }

// ---------------------------------------------------------
// IsWaitNoteNeededL
// ---------------------------------------------------------
//
TBool CGenericMtmPlugin::IsWaitNoteNeededL(
    TUid                    aMtmUid,
    const CMessageData&     aMessageData,
    TInt                    aAttachmentCount )
    {
    LOGTEXT(_L8("CGenericMtmPlugin::IsWaitNoteNeededL >>"));
    if ( aMtmUid != KSenduiMtmIrUid && aMtmUid != KSenduiMtmBtUid )
        {
        return ETrue;       
        }

    const CRichText* bodyText = aMessageData.BodyText();
    const CMessageAddressArray& toAddresses = aMessageData.ToAddressArray();

    if ( aAttachmentCount > 0 ||
       ( bodyText && bodyText->DocumentLength() ) ||
       ( toAddresses.Count() > 2 ) )
        {
        LOGTEXT(_L8("CGenericMtmPlugin::IsWaitNoteNeededL <<"));
        return ETrue;
        }

    LOGTEXT(_L8("CGenericMtmPlugin::IsWaitNoteNeededL <<"));

    return EFalse;
    }

// ---------------------------------------------------------
// CGenericMtmPlugin::ShowWaitNoteLC
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CGenericMtmPlugin::ShowWaitNoteLC()
    {
    LOGTEXT(_L8("CGenericMtmPlugin::ShowWaitNoteLC >>"));
    
    delete iWaitDialog;
    iWaitDialog = NULL;

    iWaitDialog = new (ELeave) CAknWaitDialog(
        reinterpret_cast<CEikDialog**>(&iWaitDialog), ETrue );

    CleanupStack::PushL(TCleanupItem(CleanupWaitDialog, (TAny**)&iWaitDialog));
    
    iWaitDialog->ExecuteLD( R_SENDUI_WAIT_NOTE );
    LOGTEXT(_L8("CGenericMtmPlugin::ShowWaitNoteLC <<"));
    }

// ---------------------------------------------------------
// ResetIsSending
// ---------------------------------------------------------
//
void CGenericMtmPlugin::ResetIsSending(TAny* aAny)
    {
    *((TBool*)aAny) = EFalse;
    }

// ---------------------------------------------------------
// CGenericMtmPlugin::CleanupWaitDialog
// ---------------------------------------------------------
//
void CGenericMtmPlugin::CleanupWaitDialog(TAny* aAny)
    {
    CAknWaitDialog** dialog = (CAknWaitDialog**) aAny;
    if (dialog && *dialog)
        {
        delete *dialog;
        *dialog = NULL;
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CGenericMtmPlugin::CreateMessageL(
		TUid&    aMtmUid, 
		TMsvId  aService
	)
	{

	LOGTEXT(_L8("CGenericMtmPlugin::CreateMessageL >>"));

	CMsvEntry* draftEntry = iSingleton.MsvSessionL().GetEntryL( KMsvDraftEntryId );
	CleanupStack::PushL( draftEntry );
	iClientMtm->SetCurrentEntryL( draftEntry ); // mtm takes ownership
	CleanupStack::Pop( draftEntry );

	iClientMtm->CreateMessageL( aService );

	if ( aMtmUid == KSenduiMtmSmtpUid )
		{
		// For mail message: this loads signature.
		iClientMtm->LoadMessageL();
		}

	LOGTEXT(_L8("CGenericMtmPlugin::CreateMessageL <<"));
 
	}    
TBool CGenericMtmPlugin::CreateMessageL(
    TUid&    aMtmUid,
    TBool&   aLaunchEmbedded )
    {
    LOGTEXT(_L8("CGenericMtmPlugin::CreateMessageL >>"));
    TBool doContinue;
    TMsvId service = GetAndValidateServiceL( aMtmUid, aLaunchEmbedded, doContinue );
    
    if ( doContinue )
        {
        CMsvEntry* draftEntry = iSingleton.MsvSessionL().GetEntryL( KMsvDraftEntryId );
        CleanupStack::PushL( draftEntry );
        iClientMtm->SetCurrentEntryL( draftEntry ); // mtm takes ownership
        CleanupStack::Pop( draftEntry );
        iClientMtm->CreateMessageL( service );
        
        if ( aMtmUid == KSenduiMtmSmtpUid )
            {
            // For emails, this loads signature.
            iClientMtm->LoadMessageL();
            }
        }
    LOGTEXT(_L8("CGenericMtmPlugin::CreateMessageL <<"));
    return doContinue;
    }

// -----------------------------------------------------------------------------
// SetRecipientsL
// -----------------------------------------------------------------------------
//
void CGenericMtmPlugin::SetRecipientsL(
    TUid                    aMtmUid,
    const CMessageData&     aMessageData )
    {
    LOGTEXT(_L8("CGenericMtmPlugin::SetRecipientsL >>"));

    // Send file in call
    TPhCltTelephoneNumber callerNumber;
    TPhCltNameBuffer callerName;
    // Add "To"-recipients addressees
    if ( aMessageData.ToAddressArray().Count() > 0 ) 
        {
        AddAddresseesL( EMsvRecipientTo, aMessageData.ToAddressArray() );
        }
    else if( IsCallOngoing( callerNumber, callerName ) && // Incall situation
            ( aMtmUid == KSenduiMtmMmsUid || aMtmUid == KSenduiMtmUniMessageUid ) )
        {
        if ( callerName != KNullDesC16)
            {
            iClientMtm->AddAddresseeL( callerNumber, callerName );
            }
        else
            {
            iClientMtm->AddAddresseeL( callerNumber );
            }
        }

    // Add "CC"-recipients addressees
    if ( aMessageData.CcAddressArray().Count() > 0 ) 
        {
        AddAddresseesL( EMsvRecipientCc, aMessageData.CcAddressArray() );
        }

    // Add "BCC"-recipients addressees
    if ( aMessageData.BccAddressArray().Count() > 0 ) 
        {
        AddAddresseesL( EMsvRecipientBcc, aMessageData.BccAddressArray() );
        }

    LOGTEXT(_L8("CGenericMtmPlugin::SetRecipientsL <<"));
    }

// -----------------------------------------------------------------------------
// Add addressees.
// -----------------------------------------------------------------------------
//    
void CGenericMtmPlugin::AddAddresseesL(
    TMsvRecipientType               aRecipientType,
    const CMessageAddressArray&     aAddressees )
    {
    LOGTEXT(_L8("CGenericMtmPlugin::AddAddresseesL >>"));

    for( TInt i(0) ; i < aAddressees.Count(); i++ )
        {
        if( aAddressees[i]->Address().Length() > 0 )
            {
            // If address has an alias, process it            
            if( aAddressees[i]->Alias().Length() > 0 )
                {
                TPtrC alias = aAddressees[i]->Alias();

                // Check if alias contains illegal characters. If it does
                // remove them. 
                if( ( alias.Locate( KSendUiMsgAddressStartChar ) != KErrNotFound  ) ||
                    ( alias.Locate( KSendUiMsgAddressEndChar ) != KErrNotFound ) )
                    {
                    HBufC* aliasNew = alias.AllocLC();
                    TPtr aliasPtr = aliasNew->Des();
                    RemoveIllegalChars( aliasPtr );
                    iClientMtm->AddAddresseeL(
                        aRecipientType, aAddressees[i]->Address(), *aliasNew );

                    CleanupStack::PopAndDestroy( aliasNew );
                    }
                else
                    {
                    iClientMtm->AddAddresseeL(
                        aRecipientType, aAddressees[i]->Address(), alias );
                    }
                }
            else 
                {
                // Address entry didn't have alias
                iClientMtm->AddAddresseeL(
                    aRecipientType, aAddressees[i]->Address() );
                }
            }
        // Done setting one addressee
        }

    LOGTEXT(_L8("CGenericMtmPlugin::AddAddresseesL <<"));
    }

// -----------------------------------------------------------------------------
// Add attachments to message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CGenericMtmPlugin::AddAttachmentsL( CBaseMtmUiData& aMtmUiData,
        CArrayPtrFlat<CSendUiAttachment>* aAttachments )
    {
    LOGTEXT(_L8("CGenericMtmPlugin::AddAttachmentsL >>"));

    TInt supportsLinks = 0;
    aMtmUiData.QueryCapability( TUid::Uid(KUidMsvMtmQuerySupportLinks), supportsLinks );
    
    TBool success(ETrue);
    // Set linked attachments. (Attachments given as file paths)
    TInt attachmentCount = aAttachments->Count();
	TInt i;
    for ( i = 0; i < attachmentCount && success ; i++ )
        {
        CSendUiAttachment& attachment = *(aAttachments->At(i ));
        success = AddAttachmentL( &attachment, supportsLinks );
        }

    LOGTEXT(_L8("CGenericMtmPlugin::AddAttachmentsL <<"));
    return success;
    }

// -----------------------------------------------------------------------------
// CGenericMtmPlugin::AddAttachmentL
// Add attachment to message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CGenericMtmPlugin::AddAttachmentL( CSendUiAttachment* aAttachment, TInt aSupportsLinks )
    {
    CSendUiOperationWait* waiter = CSendUiOperationWait::NewLC();
	// File paths
    if ( aSupportsLinks && aAttachment->Type() == CSendUiAttachment::EAttachmentPath )
   		{
    	iClientMtm->AddLinkedAttachmentL(
	        *(aAttachment->Path() ), 
	        aAttachment->MimeType(), 
	        0, 
	        waiter->iStatus );
		}
	else
	    {
	    RFile attachmentFile;
	    User::LeaveIfError( attachmentFile.Duplicate( *(aAttachment->Handle() ) ));
	    CleanupClosePushL( attachmentFile );

	    // File handle ownership transferred.
	    iClientMtm->AddAttachmentL(
	        attachmentFile, 
	        aAttachment->MimeType(), 
	        0, 
	        waiter->iStatus );
	    CleanupStack::Pop( &attachmentFile );
	    }
        
    TBool success = waiter->Start( iClientMtm );

    CleanupStack::PopAndDestroy( waiter );
    return success;
    }
    
// ---------------------------------------------------------
// SetBodyL
// ---------------------------------------------------------
//
void CGenericMtmPlugin::SetBodyL(
    const CBaseMtmUiData& aMtmUiData,
    const CMessageData& aMessageData,
    CArrayPtrFlat<CSendUiAttachment>* aAttachments )
    {
    LOGTEXT(_L8("CGenericMtmPlugin::SetBodyL >>"));
    __ASSERT_DEBUG( iStore, User::LeaveIfError(KErrNoMemory) );

    const CRichText* bodyText = aMessageData.BodyText();
    LOGTEXT2( _L("CMessageData bodytext length %d" ), bodyText->DocumentLength() );
    
    if ( !bodyText || !bodyText->DocumentLength() )
        { // No body text
        return;
        }

    TInt response;
    if ( aMtmUiData.QueryCapability( KUidMtmQuerySupportedBody, response ) == KErrNotSupported )
        {
        CSendUiAttachment* atta = CSendUiAttachment::NewLC( *bodyText, iCoeEnv.FsSession() );
        aAttachments->AppendL( atta );
        CleanupStack::Pop( atta );
        return;
        }

    CRichText& mtmBody = iClientMtm->Body();
    LOGTEXT2( _L("mtm body length %d" ), mtmBody.DocumentLength() );
    // Set the picture factory
    if ( bodyText->PictureFactory() )
        {
        mtmBody.SetPictureFactory( bodyText->PictureFactory(), this );
        }
    
    // read aMtmBody and add to aSentMessageBody so that possible signature is added.
    if ( aMtmUiData.Type() == KSenduiMtmSmtpUid && mtmBody.DocumentLength() )
        {
        CRichText* text = CRichText::NewL(
            static_cast<CEikonEnv&>( iCoeEnv ).SystemParaFormatLayerL(), 
            static_cast<CEikonEnv&>( iCoeEnv ).SystemCharFormatLayerL() );
        CleanupStack::PushL( text );
        
        TInt len1 = bodyText->DocumentLength();
        TInt len2 = mtmBody.DocumentLength();
        
        HBufC* buf = HBufC::NewLC( len1 );
        TPtr ptr = buf->Des();
        bodyText->Extract( ptr, 0 );
        text->InsertL( 0, *buf );
        CleanupStack::PopAndDestroy( buf );
        
        len1 = text->DocumentLength();
        text->InsertL (len1, CEditableText::EParagraphDelimiter );
        len1 = text->DocumentLength();
        text->InsertL( len1, mtmBody.Read( 0, len2 ) );
        
        // Save and restore the rich text
        TStreamId id = text->StoreL( *iStore );
        mtmBody.RestoreL( *iStore, id );
        
        CleanupStack::PopAndDestroy( text );
        }
    else
        {
        // Save and restore the rich text
        TStreamId id = bodyText->StoreL( *iStore );
        mtmBody.RestoreL( *iStore, id );
        
        LOGTEXT(_L8("CGenericMtmPlugin::mtm body restored"));
        }
    
    LOGTEXT2( _L("Final Mtm body length %d" ), mtmBody.DocumentLength() );
    // Get access to protected data
    CSendUiDummyMtm* mtm = ( CSendUiDummyMtm* )iClientMtm;
    
    mtm->SetCharFormatL( *bodyText->GlobalCharFormatLayer() );
    mtm->SetParaFormatL( *bodyText->GlobalParaFormatLayer() );
    LOGTEXT(_L8("CGenericMtmPlugin::SetBodyL <<"));
    }

// -----------------------------------------------------------------------------
// CGenericMtmPlugin::LaunchEditorL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CGenericMtmPlugin::LaunchEditorL(
        TInt aAttachmentCount,
        TBool aLaunchEditorEmbedded )
    {
    LOGTEXT(_L8("CGenericMtmPlugin::LaunchEditorL >>"));
    // save message.
    iClientMtm->SaveMessageL();
    // set the time and make invisible.
    iIndexEntry = iClientMtm->Entry().Entry();
    iIndexEntry.iDate.UniversalTime();
    iIndexEntry.SetVisible( EFalse );
    iIndexEntry.SetInPreparation( ETrue );
    
    if ( aAttachmentCount > 0 )
        {
        iIndexEntry.SetAttachment( ETrue );
        }
    else
        {
        iIndexEntry.SetAttachment( EFalse );        
        }
    
    iClientMtm->Entry().ChangeL( iIndexEntry );
    
    CSendUiOperationWait* waiter = 
        CSendUiOperationWait::NewLC( EActivePriorityWsEvents+10 );
    
    iClientMtm->SwitchCurrentEntryL( iIndexEntry.Id() );
   
    // Set preferences.
    
    TUint preferences = iMtmUi->Preferences();
    if ( aLaunchEditorEmbedded )
        {
        preferences |= EMtmUiFlagEditorPreferEmbedded;
        }
    else
        {
        preferences &= ~EMtmUiFlagEditorPreferEmbedded;
        preferences |= EMtmUiFlagEditorNoWaitForExit;
        }
    
    iMtmUi->SetPreferences( preferences );
    
    // Remove wait dialog before calling EditL.
    // Otherwise possible (e.g. settings) dialogs opened by UI MTM may get dimmed.
    delete iWaitDialog;
    iWaitDialog = NULL;

    RProperty integerProperty;
        
    TInt err = integerProperty.Attach(KPSUidMsgEditor,KMuiuStandAloneOpenMsgEditors);
    TInt openeditorcount = 0;
    err = integerProperty.Get(KPSUidMsgEditor, KMuiuStandAloneOpenMsgEditors, openeditorcount);
	switch ( err )
        {
        case KErrPermissionDenied:
        case KErrArgument:
            {
            User::Leave( err );
            }
            break;
        case KErrNotFound: // shoudl be executed only once for every boot cycle
            {
            User::LeaveIfError ( 
                RProperty::Define( 
                    KPSUidMsgEditor,
                    KMuiuStandAloneOpenMsgEditors,
                    RProperty::EInt,
                    ECapabilityReadDeviceData,
                    ECapabilityWriteDeviceData,
                    0 ) );
            }
            break;
        case KErrNone:
        default:
            break;
        }

	if (!aLaunchEditorEmbedded && openeditorcount )
        {                                       
         RProperty property;

         User::LeaveIfError( property.Attach( KPSUidMuiu,KMuiuSysOpenMsgEditors ) );
         TInt openEditors;
         TInt err = property.Get( openEditors );            
         CleanupClosePushL( property );
         
         openeditorcount++;
 		 integerProperty.Set(KPSUidMsgEditor, KMuiuStandAloneOpenMsgEditors, openeditorcount);
         property.Set( KPSUidMuiu, KMuiuSysOpenMsgEditors, openEditors );
 		
            // wait a second
         User::After( KSeconds );
         CleanupStack::PopAndDestroy( &property );      
        }
    else
        {
        openeditorcount = 1; 
        integerProperty.Set(KPSUidMsgEditor, KMuiuStandAloneOpenMsgEditors, openeditorcount);
        }    
        integerProperty.Close();
    // Edit the message.        
    CMsvOperation* op = iMtmUi->EditL( waiter->iStatus );
    CleanupStack::PushL( op );    
    TBool success = waiter->Start( op );
    if ( success )
        {
        TBuf8<KSendUiProgressStringSize> progressBuf = op->ProgressL();
        iMtmUi->DisplayProgressSummary( progressBuf );
        }
    CleanupStack::PopAndDestroy( op ); //op
    CleanupStack::PopAndDestroy(waiter); // waiter

    LOGTEXT(_L8("CGenericMtmPlugin::LaunchEditorL <<"));
    }

// ---------------------------------------------------------
// StreamStoreL
// ---------------------------------------------------------
//
const CStreamStore& CGenericMtmPlugin::StreamStoreL(TInt) const
    {
    return *iStore;
    }

// ---------------------------------------------------------
// CGenericMtmPlugin::EmailDefaultServiceL
// ---------------------------------------------------------
//
TMsvId CGenericMtmPlugin::EmailDefaultServiceL()
    {
    TMsvId service = MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
        iSingleton.MsvSessionL(), KSenduiMtmSmtpUid, ETrue);

    if (service == KMsvUnknownServiceIndexEntryId)
        {
        // Hide wait-note. Email-settings dialog (wizard) status
        // pane is dimmed if wait-note is left visible behind settings.
        if ( iWaitDialog )
            {
            iWaitDialog->MakeVisible( EFalse );
            }

        CAknQueryDialog* dlg = CAknQueryDialog::NewL(CAknQueryDialog::ENoTone);
        
        if (dlg->ExecuteLD(R_SENDUI_CREATE_MAILBOX_CONFIRMATION_QUERY))
            {
            CEikStatusPane* sp = static_cast<CEikonEnv&>( iCoeEnv ).AppUiFactory()->StatusPane();
            CAknContextPane* cp = (CAknContextPane *)
                sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
            
            // switch layout (e.g. from phone idle layout is different).
            TInt current = sp->CurrentLayoutResId();
            if (current != R_AVKON_STATUS_PANE_LAYOUT_USUAL)
                {
                sp->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
                sp->MakeVisible(ETrue);
                }
            
            iIndexEntry = TMsvEntry();
            iIndexEntry.iMtm = KSenduiMtmSmtpUid;
            iIndexEntry.iType.iUid = KUidMsvServiceEntryValue;
            iIndexEntry.iDate.UniversalTime();
            
            CMsvEntry* centry = iSingleton.MsvSessionL().GetEntryL(KMsvRootIndexEntryId);
            CleanupStack::PushL(centry);
            
            CSendUiOperationWait* waiter = CSendUiOperationWait::NewLC();
            
            CMsvOperation* op = 
                iMtmUi->CreateL( iIndexEntry, *centry, waiter->iStatus );
            CleanupStack::PushL(op);

            waiter->Start( op );

            CleanupStack::PopAndDestroy( 3, centry ) ; // op, waiter, centry           
            service = MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
                iSingleton.MsvSessionL(), KSenduiMtmSmtpUid, ETrue);
            
            if (current != R_AVKON_STATUS_PANE_LAYOUT_USUAL)
                {
                sp->SwitchLayoutL(current);
                sp->MakeVisible(ETrue);
                }

            // Display wait note while editor starts
            if ( iWaitDialog )
                {
                iWaitDialog->MakeVisible( ETrue );
                }
            }
        }
    
    if (service == KMsvUnknownServiceIndexEntryId)
        {
        // User canceled creating mailbox.
        // Leaving now pops cleanupstack filled earlier.
        User::Leave(KLeaveWithoutAlert);
        }
    
    return service;
    }

// ---------------------------------------------------------
// RemoveIllegalChars < and > characters
// ---------------------------------------------------------
//
void CGenericMtmPlugin::RemoveIllegalChars( TPtr& aCheckedString)
    {
    TInt stringLength = aCheckedString.Length();

    while(stringLength--)
        {
        if ( aCheckedString[stringLength] == KSendUiMsgAddressStartChar ||
             aCheckedString[stringLength] == KSendUiMsgAddressEndChar)
            {
            aCheckedString[stringLength] = CEditableText::ESpace;
            }
        }
    aCheckedString.TrimAll();
    }

// ---------------------------------------------------------
// Checks if the given feature is supported
// ---------------------------------------------------------
//
TBool CGenericMtmPlugin::IsSupported( const TUint& aFeature )
    {
    return iSendUiFeatureFlags & aFeature;
    }

// ---------------------------------------------------------
// IsCallOngoing
// ---------------------------------------------------------
//
TBool CGenericMtmPlugin::IsCallOngoing(
    TDes& aPhoneNumberString,
    TDes& aName )
    {
    LOGTEXT(_L8("CGenericMtmPlugin::IsCallOngoing >>"));

    TBool phoneNumberStringFound( EFalse );
    TInt error(KErrNone);
    if( IsSupported( KSendUiFeatureSFI ) )
        {
         error = RProperty::Get( 
            KPSUidTelRemotePartyInformation,
            KTelCLINumber,
            aPhoneNumberString );

        if( error == KErrNone && aPhoneNumberString != KNullDesC16 )
            {
            phoneNumberStringFound = ETrue;
            error = RProperty::Get( 
                KPSUidTelRemotePartyInformation,
                KTelCLIName,
                aName );
            }
        }
    LOGTEXT(_L8("CGenericMtmPlugin::IsCallOngoing <<"));
    return phoneNumberStringFound;
    }

// ---------------------------------------------------------
// Sends Bluetooth message using AIW
// ---------------------------------------------------------
//
void CGenericMtmPlugin::SendBtMessageL( CArrayPtrFlat<CSendUiAttachment>* aAttachments )
    {
    LOGTEXT(_L8("CGenericMtmPlugin::SendBtMessageL >>"));

    TInt i = 0;

    CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();
    
    // Go through attachments
    TInt attaCount = aAttachments->Count();
    
    while ( i < attaCount )
	    {
	    CSendUiAttachment& attachment = *(aAttachments->At(i));

	    if ( attachment.Type() == CSendUiAttachment::EAttachmentPath )
	    	{
	    	TAiwGenericParam filePath( EGenericParamFile, *attachment.Path() );
	    	paramList->AppendL( filePath );
	        }
	    else
	    	{
	    	TAiwGenericParam fileHandle( EGenericParamFile, *attachment.Handle() );
	        paramList->AppendL( fileHandle );
	    	}
	    i++;
	    }
    CAiwServiceHandler* serviceHandler = CAiwServiceHandler::NewLC();
    serviceHandler->AttachL( R_SENDUI_BASEINTEREST );
    iWaitDialog->MakeVisible(EFalse);
    // Give file list to Service Handler
    serviceHandler->ExecuteServiceCmdL(
        KAiwCmdSend, 
        *paramList ,
        serviceHandler->OutParamListL());

    CleanupStack::PopAndDestroy( 2, paramList ); // serviceHandler, paramList

    LOGTEXT(_L8("CGenericMtmPlugin::SendBtMessageL <<"));
    }

// ---------------------------------------------------------
// Calls MTM UI validate service function 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CGenericMtmPlugin::IsValidServiceL()
    {
    TPckgBuf<TBool> paramPack( ETrue );
        
    // This is not actually used.
    CMsvEntrySelection* dummySelection = new ( ELeave ) CMsvEntrySelection; 
    CleanupStack::PushL( dummySelection );

    iMtmUi->InvokeSyncFunctionL( KMtmUiFunctionValidateService, 
                                *dummySelection, 
                                paramPack );

    CleanupStack::PopAndDestroy( dummySelection );

    return ( KErrNone == paramPack() );
    }


// ---------------------------------------------------------
// ValidateMtmL
// 
// Returns
//
// ETrue, if service supports validate service and service is valid
// ETrue, if service does not support validate service
// EFalse, if service supports validate service, but service is not valid
// ---------------------------------------------------------
//
TBool CGenericMtmPlugin::ValidateMtmL()
    {
    TBool serviceOk = ETrue;
    TInt supportsValidateService = 0;

    const TUid uidQuery = { KUidMsvMtmQuerySupportValidateService };
    
    TInt supported = ( KErrNone == iMtmUi->QueryCapability(
        uidQuery, supportsValidateService ) );

    if ( supported && supportsValidateService )
        {
        serviceOk = IsValidServiceL();
        }
    return serviceOk;
    }

// ---------------------------------------------------------
// GetAndValidateServiceL
// ---------------------------------------------------------
//
TMsvId CGenericMtmPlugin::GetAndValidateServiceL(
        TUid& aMtmUid, 
        TBool aLaunchEmbedded,
        TBool& aContinue )
    {
    LOGTEXT(_L8("CGenericMtmPlugin::GetAndvalidateServiceL >>"));
    aContinue = ETrue;
    TMsvId service = KMsvUnknownServiceIndexEntryId;
    TUid previousMtmUid( aMtmUid ); 
     
    if (aLaunchEmbedded)
        {
        if (aMtmUid == KSenduiMtmSmtpUid)
            {
			if( IsSupported( KSendUiFeatureSelectableEmail ) && 
				!CMceSettingsEmailSel::ShowSelectEmailDlgL( service, 
						aMtmUid, iRequiredCapabilities ) )
				{
				return KMsvUnknownServiceIndexEntryId;
				}
			
			if ( previousMtmUid != aMtmUid )
				{
				delete iMtmUi; iMtmUi = NULL;
				delete iClientMtm; iClientMtm = NULL;

				iClientMtm = iSingleton.ClientMtmRegistryL().NewMtmL( aMtmUid );
				iMtmUi = iSingleton.MtmUiRegistryL().NewMtmUiL( *iClientMtm );
				}
			
			if( service == KMsvUnknownServiceIndexEntryId )
				{
				service = EmailDefaultServiceL();		        
				}
            }
        else
            {
            service = MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
                iSingleton.MsvSessionL(), aMtmUid, ETrue);
            }
        }
    else
        {
		if( IsSupported( KSendUiFeatureSelectableEmail ) && 
				aMtmUid == KSenduiMtmSmtpUid )
			{
			if ( !CMceSettingsEmailSel::ShowSelectEmailDlgL( service, 
					aMtmUid, iRequiredCapabilities ) )
				{
				return KMsvUnknownServiceIndexEntryId;
				}
			
			if ( previousMtmUid != aMtmUid )
				{
				delete iMtmUi; iMtmUi = NULL;
				delete iClientMtm; iClientMtm = NULL;

				iClientMtm = iSingleton.ClientMtmRegistryL().NewMtmL( aMtmUid );
				iMtmUi = iSingleton.MtmUiRegistryL().NewMtmUiL( *iClientMtm );
				}

			}
		else 
			{
        service = MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
            iSingleton.MsvSessionL(), aMtmUid, ETrue);
			}
        // If stand-alone launch requested and service is not OK, show error note.
        // This is mainly for phone idle "New message" function which does not
        // like possible settings dialogs hanging around...
        if ( !ValidateMtmL() )
            {
            CAknGlobalNote* note = CAknGlobalNote::NewL();
            CleanupStack::PushL( note );
            
            HBufC* text = NULL;
            if( iSingleton.ClientMtmRegistryL().TechnologyTypeUid( aMtmUid ) == KMailTechnologyTypeUid )
                text = StringLoader::LoadLC( R_SENDUI_SETTINGS_EMAIL_NOT_OK, &iCoeEnv );
            else
                text = StringLoader::LoadLC( R_SENDUI_SETTINGS_NOT_OK, &iCoeEnv );
            note->ShowNoteL( EAknGlobalErrorNote, *text );
            CleanupStack::PopAndDestroy( 2, note ); // note, stringLoader
            aContinue = EFalse;
            }
        }
    LOGTEXT(_L8("CGenericMtmPlugin::GetAndvalidateServiceL <<"));
    return service;
    }

// ---------------------------------------------------------
// GetMtmSendingCapabilitiesL
// ---------------------------------------------------------
//
void CGenericMtmPlugin::GetMtmSendingCapabilitiesL( 
    CBaseMtmUiData& mtmUiData, 
    TSendingCapabilities& cap )
    {
    cap.iFlags = 0; // reset flags.body and message size resets themselves.
    TInt response;
    // Query capabilities
    mtmUiData.QueryCapability( KUidMtmQueryMaxBodySize, cap.iBodySize  );
    mtmUiData.QueryCapability( KUidMtmQueryMaxTotalMsgSize, cap.iMessageSize  );
    
    if ( mtmUiData.QueryCapability( KUidMtmQuerySupportAttachments, response ) == KErrNone )
        {
        cap.iFlags |= TSendingCapabilities::ESupportsAttachments;   
        }
    if ( mtmUiData.QueryCapability( KUidMtmQuerySupportedBody, response ) == KErrNone )
        {
        cap.iFlags |= TSendingCapabilities::ESupportsBodyText;  
        }
    if ( mtmUiData.QueryCapability( KUidMsvQuerySupportsBioMsg, response ) == KErrNone )
        {
        cap.iFlags |= TSendingCapabilities::ESupportsBioSending;    
        }
    if ( mtmUiData.QueryCapability( TUid::Uid( KUidMsvMtmQuerySupportEditor ), response ) == KErrNone )
        {
        cap.iFlags |= TSendingCapabilities::ESupportsEditor;    
        }
    }

// ---------------------------------------------------------
// IsPostcardContactLink
// ---------------------------------------------------------
//
TBool CGenericMtmPlugin::IsPostcardContactLink( const CArrayPtrFlat<CSendUiAttachment>* aAttachments )
    {
    _LIT(KFileName, "X-Nokia-PhonebookId_temp");

    // Check if attachment is a file containing serialized contact
    // link. Such attachment is sent by Phonebook to Postcard. The
    // special file is recognized by name.
    if ( aAttachments->Count() == 0 )
        {
        return EFalse;
        }
    CSendUiAttachment& attachment = *(aAttachments->At(0));
    return attachment.Type() == CSendUiAttachment::EAttachmentPath 
        && ( attachment.Path()->Right( KFileName().Length() ).Compare( KFileName ) == 0 );
    
    }

// ---------------------------------------------------------
// ValidateAccountL
// ---------------------------------------------------------
//
TBool CGenericMtmPlugin::ValidateAccountL( CBaseMtmUiData& aMtmUiData, TUid aMtmUid )
    {
    LOGTEXT2(_L("ValidateAccountL: %d"), aMtmUid.iUid );
    TInt response = 0;
    
    // Check to see that accounts (if required) have been set up
    TMsvId id;
    User::LeaveIfError(
        iSingleton.MsvSessionL().GetEntry( KMsvRootIndexEntryId, id, iIndexEntry ) );

    TInt validAccounts = ETrue;
    TMsvEntry serviceEntry;
    serviceEntry.iType.iUid = KUidMsvServiceEntryValue;
    serviceEntry.iMtm = aMtmUid;

    if ( aMtmUiData.CanCreateEntryL( iIndexEntry, serviceEntry, response ) )
        {
        CMsvEntrySelection* accounts =
            MsvUiServiceUtilities::GetListOfAccountsWithMTML(
                iSingleton.MsvSessionL(), aMtmUid, ETrue );
        
        if ( accounts->Count() == 0 )
            {
            validAccounts = EFalse;
            }
        
        delete accounts;
        accounts = NULL;
        }
    LOGTEXT2(_L("ValidateAccountL result: %d"), validAccounts );
    
    return validAccounts;
    }

// ---------------------------------------------------------
// CGenericMtmPlugin::LoadMtmL
// ---------------------------------------------------------
//
void CGenericMtmPlugin::LoadMtmL(
    TUid aMtmType, 
    RPointerArray<CSendingServiceInfo>& aList )
    {
    // these mtm's are not needed, because they don't support sending
    if (aMtmType == KSenduiMtmImap4Uid ||
        aMtmType == KSenduiMtmPop3Uid ||
        aMtmType == KSenduiMtmPushMtm1Uid ||
        aMtmType == KSenduiMtmPushMtm2Uid ||
        aMtmType == KSenduiMtmPushMtm3Uid ||
        aMtmType == KSenduiMtmPushMtmUid ||
        aMtmType == KSenduiMtmBioUid ||
        aMtmType == KSenduiMMSNotificationUid ||
        aMtmType == KSenduiMtmRoUid 
        )
        {
        return ;
        }
        
    TInt err(KErrNone);

    CBaseMtmUiData* uiData = iSingleton.MtmUiDataRegistryL().NewMtmUiDataLayerL( aMtmType );
    CleanupStack::PushL( uiData );
    TInt response = 0;

    err = uiData->QueryCapability(KUidMtmQueryCanSendMsg, response);
    
    if ( err )
        {
        CleanupStack::PopAndDestroy( uiData );
        if ( err == KErrNotSupported )
            {
            return;
            }
        User::Leave( err );
        }

    CSendingServiceInfo* serviceInfo =  CSendingServiceInfo::NewLC();
    serviceInfo->SetServiceId( aMtmType );
    
    // the MTM can send messages
    const CArrayFix<CBaseMtmUiData::TMtmUiFunction>& funcs = 
                                        uiData->MtmSpecificFunctions();
    
    TInt i = funcs.Count();
    TBool usingPriorityName( EFalse );
    while (i--)
        {
        CBaseMtmUiData::TMtmUiFunction func(funcs.At(i));
        if ( func.iFunctionId == KMtmUiFunctionSendAs /*func.iFlags & EMtudCommandSendAs */)
            {
            serviceInfo->SetServiceMenuNameL( func.iCaption );
            }
        else if ( func.iFunctionId == KMtmUiMceWriteMessagePopup )
            {
            serviceInfo->SetServiceNameL( func.iCaption );
            usingPriorityName = ETrue;
            }
           else if ( !usingPriorityName && func.iFlags & EMtudCommandTransferSend )
               {
               serviceInfo->SetServiceNameL( func.iCaption );
               }
        }
     
    if ( serviceInfo->ServiceMenuName() == KNullDesC )
        {
        // no info available for caption - use the human readable name
        serviceInfo->SetServiceMenuNameL( iSingleton.MtmUiDataRegistryL().RegisteredMtmDllInfo( aMtmType ).HumanReadableName() );
        }

    if ( serviceInfo->ServiceName() == KNullDesC )
        {
        // no info available for caption - use the human readable name
        serviceInfo->SetServiceNameL( 
            iSingleton.MtmUiDataRegistryL().RegisteredMtmDllInfo( aMtmType ).HumanReadableName() );
        }

    TSendingCapabilities cap;
    TInt featureFlags;
    
    serviceInfo->SetServiceProviderId( KGenericMtmPlugin );
    serviceInfo->SetTechnologyTypeId( iSingleton.ClientMtmRegistryL().TechnologyTypeUid( aMtmType ) );
    
    GetMtmSendingCapabilitiesL( *uiData, cap );

    TBool validAccount = ValidateAccountL( *uiData, aMtmType );
    
    serviceInfo->SetServiceCapabilities( cap );
    
    featureFlags = 0;
    if ( !validAccount )
        {
        featureFlags |= CSendingServiceInfo::EServiceInValid;
        }
        
    if ( aMtmType == KSenduiMtmSmsUid || aMtmType == KSenduiMtmMmsUid )
        {
        featureFlags |= CSendingServiceInfo::EServiceHidden;
        }
        
        
    serviceInfo->SetServiceFeatures( featureFlags );       
        
    aList.Append( serviceInfo ); 
    CleanupStack::Pop( serviceInfo);
    CleanupStack::PopAndDestroy( uiData );
    LOGTEXT2(_L("Service flags: %d"), featureFlags );

    }
// end of file
