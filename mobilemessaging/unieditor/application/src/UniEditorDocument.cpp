/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides CUniEditorDocument class methods.
*
*/

 

// ========== INCLUDE FILES ================================

#include <eikenv.h>
#include <eikdoc.h>

#include <msvstd.h>                     // TMsvEntry
#include <msvapi.h>
#include <msvuids.h>                    // Msgs Uids
#include <mtclbase.h>                   // CBaseMtm
#include <mmsvattachmentmanager.h>
#include <cmsvattachment.h>
#include <cmsvmimeheaders.h>

#include <bautils.h>

#include <charconv.h>                   // Character Converter

#include <mmsclient.h>                  // Client Mtm API
#include <MuiuMsvUiServiceUtilities.h>  // Disk space check
#include <MuiuOperationWait.h>          // CMuiuOperationWait
#include <MsgAttachmentInfo.h>
#include <msgmediacontrol.h>            // TMsgMediaControlId

// Features
#include <featmgr.h>    
#include <bldvariant.hrh>
#include <messagingvariant.hrh>

#include <centralrepository.h>          // link against centralrepository.lib
#include <MmsEngineInternalCRKeys.h>
#include <messaginginternalcrkeys.h>    // for Central Repository keys

#include <SendUiConsts.h>               // for KSenduiMtmSms/MmsUid

#include <RCustomerServiceProfileCache.h>   // CSP bits
#include <fileprotectionresolver.h>

// Msg Media
#include <MmsConformance.h>
#include <MsgMediaInfo.h>

// UniModel stuff
#include <unidatamodel.h>   
#include <uniobjectlist.h>
#include <unidatautils.h>

// UniUtils stuff
#include <uniobjectsmodel.h>

#include <mmssettingsdefs.h>

// MTM's
#include "UniMsvEntry.h"
#include "UniClientMtm.h"               // Client Mtm API

#include "UniPluginApi.h"
#include "UniEditorUids.hrh"

#include "UniEditorDocument.h"
#include "UniEditorAppUi.h"
#include "UniEditorObjectsModelObserver.h"
#include "UniEditorLogging.h"
#include "UniEditorEnum.h"

// ========== CONSTANTS ====================================

// Length of one 7bit normal sms
const TInt KFirstNormalSmsLength = 160;

// Length of one 8bit sms
// const TInt KFirst8bitSmsLength = 133;

// Length of one unicode sms
const TInt KFirstUnicodeSmsLength = 70;

// Submsg length for normal 7bit sms
const TInt KNormalConcatenationInterval = 153;

// Submsg length for 8bit sms
// const TInt K8bitConcatenationInterval = 128;

// Submsg length for unicode sms
const TInt KUnicodeConcatenationInterval = 67;


const TInt KDefaultMaxSize = 300 * 1024;
const TInt KDefaultMaxSmsSize = 10;
const TInt KMaxSmsSizeByStandard = 30;
const TInt KDefaultSmsRecipients = 20;
const TInt KDefaultMmsRecipients = 100;

// Char conv plugin ID (private Sms editor plugin)
const TUint KSmsEdSmsStrictPluginID = 0x101F85CD;

const TInt KUniMmsUploadImageWidth = 1600;
const TInt KUniMmsUploadImageHeight = 1200;

// ---------------------------------------------------------
// CUniEditorDocument::Constructor
// ---------------------------------------------------------
//
CUniEditorDocument::CUniEditorDocument( CEikApplication& aApp, CCoeEnv& aCoeEnv ) :
    CMsgEditorDocument( aApp ),
    iEnvironment( static_cast<CEikonEnv*>( &aCoeEnv ) ),
    iFs( aCoeEnv.FsSession() ),
    iMessageType( EUniReadOnly ),
    iDataModel( NULL ),
    iCharConverter( NULL ),
    iCurrentSlide( 0 ),
    iMaxMessageSize( KDefaultMaxSize ),
    iAddressSize( -1 ),
    iSubjectSize( -1 ),
    iMaxMmsRecipients( 100 ),
    iMaxSmsRecipients( 20 ), // Read from CR
    iMaxConcatenatedSms( KDefaultMaxSmsSize ),   // Read from CR
    iAbsMaxConcatenatedSms( KDefaultMaxSmsSize ), // Read from CR
    iCreationMode( EMmsCreationModeWarning ),
    iNonConfCount( 0 ),
    iMaxImageWidth( KMmsUniImageSmallWidth ),
    iMaxImageHeight( KMmsUniImageSmallHeight ),
    iSupportedFeatures( 0 ),
    iPrevSaveType( EClosingSave ),
    iBodyState( EUniSms ),
    iHeaderState( EUniSms ),
    iFlags( 0 ),
    iCSPBits( 255 ),
    iSmsPlugin( NULL ),
    iMmsPlugin( NULL ),
    iAbsMaxSmsCharacters( 0 )
    {
    }

// ---------------------------------------------------------
// CUniEditorDocument::ConstructL
// ---------------------------------------------------------
//
void CUniEditorDocument::ConstructL()
    {
    CMsgEditorDocument::ConstructL();
    
    TInt maxSize = KDefaultMaxSize;
    TInt creationMode = EMmsCreationModeWarning;
    TInt features = 0;

    // MmsEngine / MaxSendSize + CreationMode
    CRepository* repository = CRepository::NewL( KCRUidMmsEngine );
    CleanupStack::PushL( repository );
    
    if ( repository->Get( KMmsEngineMaximumSendSize, maxSize ) == KErrNone )
        {
        iMaxMessageSize = maxSize;
        }
            
    if ( repository->Get( KMmsEngineCreationMode, creationMode ) == KErrNone )
        {
        iCreationMode = creationMode;
        }
        
    CleanupStack::PopAndDestroy( repository );

    // MUIU / MmsFeatures / SmsFeatures
    TBool emailOverSmsVariationOn = EFalse;
    
    repository = CRepository::NewL( KCRUidMuiuVariation );
    CleanupStack::PushL( repository );
    
    if ( repository->Get( KMuiuMmsFeatures, features ) == KErrNone )
        {
        if ( features & KMmsFeatureIdEditorSubjectField )
            {
            iSupportedFeatures |= EUniFeatureSubject;
            }
            
        if ( features & KMmsFeatureIdUserCreationMode )
            {
            iFlags |= EUniDocumentCreationModeUserChangeable;
            }
        }
        
    if ( repository->Get( KMuiuSmsFeatures, features ) == KErrNone )
        {
        if ( features & KSmsFeatureIdEmailOverSms )
            {
            emailOverSmsVariationOn = ETrue;
            }
        }
        
    if ( repository->Get( KMuiuUniEditorFeatures, features ) == KErrNone )
        {
        if ( features & KUniEditorFeatureIdRestrictedReplySms )
            {
            iFlags |= EUniDocumentRestrictedReplySms;
            }
        if ( features & KUniEditorFeatureIdRestrictedReplyMms )
            {
            iFlags |= EUniDocumentRestrictedReplyMms;
            }
        }
        
    if ( repository->Get( KMuiuMceFeatures, features ) == KErrNone )
        {
        if ( features & KMceFeatureIdCSPSupport )
            {
            iFlags |= EUniDocumentCSPBitsSupported;
            }
        }
        
    CleanupStack::PopAndDestroy( repository );

    // MMS Engine CR
    repository = CRepository::NewL( KUidMmsServerMtm );
    CleanupStack::PushL( repository );
    
    TInt temp = 0;
    if ( repository->Get( KMmsEngineImageWidth, temp ) == KErrNone )
        {
        iMaxImageWidth = temp;
        }
        
    if ( repository->Get( KMmsEngineImageHeight, temp ) == KErrNone )
        {
        iMaxImageHeight = temp;
        }
        
    CleanupStack::PopAndDestroy( repository );
    
    // Just to make sure the size is at least "small"
    iMaxImageWidth = Max( KMmsUniImageSmallWidth, iMaxImageWidth );
    iMaxImageHeight = Max( KMmsUniImageSmallHeight, iMaxImageHeight );
    
    // SMUM CR
    repository = CRepository::NewL( KCRUidSmum );
    CleanupStack::PushL( repository );
    
    if ( repository->Get( KSmumMaxSubMsgCount, iAbsMaxConcatenatedSms ) ||
         iAbsMaxConcatenatedSms < 1 || 
         iAbsMaxConcatenatedSms > KMaxSmsSizeByStandard )
        {
        // Unreasonable count, change it back to 30 ( S60 default )
        iAbsMaxConcatenatedSms = KMaxSmsSizeByStandard;
        }
        
    CleanupStack::PopAndDestroy( repository );

    // UniEditor CR
    repository = CRepository::NewL( KCRUidUniEditor );
    CleanupStack::PushL( repository );
    
    if ( repository->Get( KUniEditorSoftLimitSubMsgCount, iMaxConcatenatedSms ) ||
         iMaxConcatenatedSms < 1 || 
         iMaxConcatenatedSms > iAbsMaxConcatenatedSms )
        {
        // Unreasonable count, change it back to abs max
        iMaxConcatenatedSms = iAbsMaxConcatenatedSms;
        }
        
    if ( repository->Get( KUniEditorSoftLimitRecipientCount, iMaxSmsRecipients ) != KErrNone || 
         iMaxSmsRecipients < 0)
        {
        // Unreasonable count, change it back to default value
        iMaxSmsRecipients = KDefaultSmsRecipients;
        }
    if(iMaxSmsRecipients == 0 )    
        {
        iMaxSmsRecipients = KMaxTInt;
        }
        
    if ( repository->Get( KUniEditorMaxRecipientCount, iMaxMmsRecipients ) != KErrNone || 
         iMaxMmsRecipients < 0 )
        {
        // Unreasonable count, change it back to default value
        iMaxMmsRecipients = KDefaultMmsRecipients;
        }
    if ( iMaxMmsRecipients == 0 )
        {
        iMaxMmsRecipients = KMaxTInt;
        }
     TBool softlimitenable;     
     if ( repository->Get( KUniEditorSoftLimitEnable, softlimitenable ) != KErrNone ||  !softlimitenable )
        {
        // Softlimit disabled so its value is same as hardlimit 
        iMaxConcatenatedSms = iAbsMaxConcatenatedSms;
        }
    repository->Get( KUniEditorMaxSmsCharacterCount, iAbsMaxSmsCharacters );
    
    TInt maxSlideCount( 0 );
    repository->Get( KUniEditorMaxSlideCount, maxSlideCount );    
    
    iSmsSizeWarningBytes = 0;
    repository->Get(KUniEditorSMSSizeWarning, iSmsSizeWarningBytes);
    
    CleanupStack::PopAndDestroy( repository );

    if ( !iMaxMessageSize )
        {
        //iMaxMessageSize == 0 means "anything goes" so set the size as large value as possible.
        iMaxMessageSize = KMaxTUint32;
        }
    
    // Read CSP bits from SIM
    RCustomerServiceProfileCache* cspProfile = new (ELeave) RCustomerServiceProfileCache;    
    TInt error = cspProfile->Open();
    
    UNILOGGER_WRITEF( _L("UniEditor: Customer service profile opened = %d" ), error );
    if ( error == KErrNone )
        {
        if ( emailOverSmsVariationOn )
            { 
            UNILOGGER_WRITE( "UniEditor: Email over SMS enabled" );
            
            // EmailOverSms bit was variated ON so let's check the bit from SIM
            // Get tele services flags from CSP
            RMobilePhone::TCspValueAdded params;
            // Read the CPHS bit so we know if EmailOverSms is supported
            error = cspProfile->CspCPHSValueAddedServices( params );
            
            UNILOGGER_WRITEF( _L("UniEditor: CspCPHSValueAddedServices read = %d" ), error );
            
            if ( error == KErrNone && 
                 params >= 0 &&
                 params & RMobilePhone::KCspSMMOEmail ) 
                {
                UNILOGGER_WRITE( "UniEditor: Email over SMS supported by SIM" );
                
                // It's supported
                iFlags |= EUniDocumentEMailOverSms;
                }
            }
        
        if ( iFlags & EUniDocumentCSPBitsSupported )
            {
            // Get tele services flags from CSP
            RMobilePhone::TCspTeleservices params2;
            // Read the TeleServices byte so we know which settings are supported
            error = cspProfile->CspTeleServices( params2 );
        
            UNILOGGER_WRITEF( _L("UniEditor: CSP read = %d" ), error );
        
            if ( KErrNone == error && params2 >= 0 ) 
                {
                UNILOGGER_WRITEF( _L("UniEditor: CSP bits = %b" ), params2 );
                
                iCSPBits = params2;
                }
            }
            
        cspProfile->Close();
        }
        
    delete cspProfile;    

    PrepareMtmL( TUid::Uid( KUidUniMtm ) );
    
    iDataModel = CUniDataModel::NewL( iFs, Mtm() );
    iDataModel->SmilModel().SetMaxSlideCount( maxSlideCount );
    }

// ----------------------------------------------------
// CUniEditorDocument::NewL
// ----------------------------------------------------
//
CUniEditorDocument* CUniEditorDocument::NewL( CEikApplication& aApp,
                                              CCoeEnv& aCoeEnv )
    {
    CUniEditorDocument* self = new ( ELeave ) CUniEditorDocument( aApp, aCoeEnv );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CUniEditorDocument::~CUniEditorDocument
//
// Destructor.
// ---------------------------------------------------------
//
CUniEditorDocument::~CUniEditorDocument()
    {
    delete iDataModel;
    delete iCharConverter;
    delete iSmsPlugin;
    delete iMmsPlugin;
    delete iObjectObserver;
    }

// ----------------------------------------------------
// CUniEditorDocument::CreateAppUiL
// ----------------------------------------------------
//
CEikAppUi* CUniEditorDocument::CreateAppUiL() 
    {
    CUniEditorAppUi* appui = new ( ELeave ) CUniEditorAppUi;
    
    iObjectObserver = new ( ELeave ) CUniEditorObjectsModelObserver( *this, *appui );
    AttachmentModel().SetObserver( iObjectObserver );
    
    return appui;
    }

// ---------------------------------------------------------
// CUniEditorDocument::DefaultMsgFolder
// ---------------------------------------------------------
//
TMsvId CUniEditorDocument::DefaultMsgFolder() const
    {
    return KMsvGlobalOutBoxIndexEntryIdValue;
    }

// ---------------------------------------------------------
// CUniEditorDocument::DefaultMsgService
// ---------------------------------------------------------
//
TMsvId CUniEditorDocument::DefaultMsgService() const
    {
    return KMsvLocalServiceIndexEntryIdValue;
    }

// ---------------------------------------------------------
// CUniEditorDocument::CreateNewL
// ---------------------------------------------------------
//
TMsvId CUniEditorDocument::CreateNewL(TMsvId /* aServiceId */, TMsvId aDestFolder )
    {
    // Can only create to draft
    if ( aDestFolder != KMsvDraftEntryIdValue )
        {
        User::Leave( KErrNotSupported );
        }
    return Mtm().CreateNewEntryL( aDestFolder );
    }

// ---------------------------------------------------------
// CUniEditorDocument::CreateNewAttachmentModelL
//
// Creates new objects model object (virtual from CMsgEditorDocument).
// ---------------------------------------------------------
//
CMsgAttachmentModel* CUniEditorDocument::CreateNewAttachmentModelL( TBool aReadOnly )
    {
    return CUniObjectsModel::NewL( aReadOnly );
    }

// ---------------------------------------------------------
// CUniEditorDocument::Mtm
// ---------------------------------------------------------
//
CUniClientMtm& CUniEditorDocument::Mtm()
    {
    return static_cast <CUniClientMtm&> ( CMsgEditorDocument::Mtm() );
    }

// ---------------------------------------------------------
// CUniEditorDocument::MtmUi
// ---------------------------------------------------------
//
CUniMtmUi& CUniEditorDocument::MtmUiL()
    {
    return static_cast <CUniMtmUi&> ( CMsgEditorDocument::MtmUiL() );
    }


// ---------------------------------------------------------
// CUniEditorDocument::EntryChangedL
// Intentionally empty implementation.
// ---------------------------------------------------------
//
void CUniEditorDocument::EntryChangedL()
    {
    }

// ---------------------------------------------------------
// CUniEditorDocument::PrepareContextL
// ---------------------------------------------------------
//
TMsvId CUniEditorDocument::PrepareContextL( TMsvId aContext )
    {
    TMsvId service;
    TMsvEntry entry;
    User::LeaveIfError( Session().GetEntry( aContext, service, entry ) );
    
    if ( entry.iMtm.iUid == KUidUniMtm )
        {
        return aContext;
        }

    TMsvId newId = KMsvNullIndexEntryIdValue;

    if ( entry.iMtm == KSenduiMtmSmsUid )
        {
        __ASSERT_DEBUG( !iSmsPlugin, Panic( EUniAlreadyExists ) );
        
        delete iSmsPlugin;
        iSmsPlugin = NULL;
        
        iSmsPlugin = CUniEditorPlugin::NewL( TUid::Uid( KUidUniEditorSmsPlugin ),
                                             Session(),
                                             Mtm() );
                                             
        newId = iSmsPlugin->ConvertFromL( aContext );
        }
    else if ( entry.iMtm == KSenduiMtmMmsUid )
        {
        __ASSERT_DEBUG( !iMmsPlugin, Panic( EUniAlreadyExists ) );
        
        delete iMmsPlugin;
        iMmsPlugin = NULL;
        
        iMmsPlugin = CUniEditorPlugin::NewL( TUid::Uid( KUidUniEditorMmsPlugin ),
                                             Session(),
                                             Mtm() );
        newId = iMmsPlugin->ConvertFromL( aContext );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }

    return newId;
    }
    
// ---------------------------------------------------------
// CUniEditorDocument::ChangeContextL
// ---------------------------------------------------------
//
TMsvId CUniEditorDocument::ChangeContextL( const TEditorParameters& aParameters )
    {
    TMsvId id( KMsvNullIndexEntryIdValue );

    const TMsvId src = aParameters.iId;
    const TMsvId dest = aParameters.iDestinationFolderId;
    const TMsvPartList parts = aParameters.iPartList;

    if ( aParameters.iFlags & EMsgReplyToMessageSender ||
         aParameters.iFlags & EMsgReplyToMessageAll )
        {
        id = DoReplyForwardL( ETrue, src, dest, parts );
        }
    else if (aParameters.iFlags & EMsgForwardMessage)
        {
        id = DoReplyForwardL( EFalse, src, dest, parts );
        }
    else
        {
        User::Leave( KErrArgument );
        }
        
    return id;
    }

// ---------------------------------------------------------
// CUniEditorDocument::DoReplyForwardL
// ---------------------------------------------------------
//
TMsvId CUniEditorDocument::DoReplyForwardL(
        TBool aReply,
        TMsvId aSrc,
        TMsvId aDest,
        TMsvPartList aParts )
    {
    TMsvId newId;
    TMsvId service;
    TMsvEntry entry;
    User::LeaveIfError( Session().GetEntry( aSrc, service, entry ) );
    CUniEditorPlugin* plugin = NULL;
    
    if ( entry.iMtm == KSenduiMtmSmsUid )
        {
        __ASSERT_DEBUG( !iSmsPlugin, Panic( EUniAlreadyExists ) );
        
        delete iSmsPlugin;
        iSmsPlugin = NULL;
        
        iSmsPlugin = CUniEditorPlugin::NewL( TUid::Uid( KUidUniEditorSmsPlugin ),
                                             Session(),
                                             Mtm() );
        plugin = iSmsPlugin;
        if( iFlags & EUniDocumentRestrictedReplySms )
            {
            if( aReply )
                {
                Mtm().SetMessageTypeSetting( EUniMessageTypeSettingSms );
                Mtm().SetMessageTypeLocking( EUniMessageTypeLocked );
                }
            }
        } 
    else if ( entry.iMtm == KSenduiMtmMmsUid ||
              entry.iMtm == KSenduiMMSNotificationUid )
        {
        __ASSERT_DEBUG( !iMmsPlugin, Panic( EUniAlreadyExists ) );
        
        delete iMmsPlugin;
        iMmsPlugin = NULL;
        
        iMmsPlugin = CUniEditorPlugin::NewL( TUid::Uid( KUidUniEditorMmsPlugin ),
                                             Session(),
                                             Mtm() );
        plugin = iMmsPlugin;
        if ( iFlags & EUniDocumentRestrictedReplyMms )
            {
            if ( aReply )
                {
                Mtm().SetMessageTypeSetting( EUniMessageTypeSettingMms );
                Mtm().SetMessageTypeLocking( EUniMessageTypeLocked );
                }
            }
        }
    if ( !plugin )
        {
        User::Leave( KErrNotSupported );
        }
        
    if ( aReply )
        {
        newId = plugin->CreateReplyL( aSrc, aDest, aParts );
        }
    else
        {
        newId = plugin->CreateForwardL( aSrc, aDest, aParts );
        }
    return newId;
    }

// ---------------------------------------------------------
// CUniEditorDocument::SetMessageType
// ---------------------------------------------------------
//
void CUniEditorDocument::SetMessageType()
    {
    TMsvEntry tEntry = Entry();
    
    const TInt flags = LaunchFlags();

    if ( flags & EMsgCreateNewMessage )
        {
        iMessageType = EUniNewMessage;
        }
    else if ( flags & ( EMsgReplyToMessageSender | EMsgReplyToMessageAll ) )
        {
        iMessageType = EUniReply;
        }
    else if ( flags & EMsgForwardMessage ||
              TUniMsvEntry::IsForwardedMessage( tEntry ) )
        {
        iMessageType = EUniForward;
        }
    else if ( TUniMsvEntry::IsEditorOriented( tEntry ) )
        {
        iMessageType = EUniOpenFromDraft;
        }
    else
        {
        iMessageType = EUniSendUi;
        }   
    }

// ---------------------------------------------------------
// MessageSize
//
// ---------------------------------------------------------
//
TInt CUniEditorDocument::MessageSize( TBool aSmilEstimate /*= EFalse*/ )
    {
    TInt msgSizeBytes( 0 );

    msgSizeBytes = iDataModel->ObjectList().ObjectByteSize();

    TBool addSmilSize = ETrue;
    
    if ( iDataModel->SmilType() == EMmsSmil )
        {
        if ( !msgSizeBytes )
            {
            // MMS SMIL & no objects
            // -> Consider message (body) empty
            addSmilSize = EFalse;
            }
        }
    else
        {
        // Not MMS SMIL
        // -> make sure SmilModel estimate size is not used.
        aSmilEstimate = EFalse;
        }

    if ( addSmilSize )
        {
        if ( aSmilEstimate )
            {
            msgSizeBytes += iDataModel->SmilModel().SmilComposeSize();
            }
        else
            {
            msgSizeBytes += iDataModel->SmilList().SmilByteSize();
            }
        }

    msgSizeBytes += iDataModel->AttachmentList().ObjectByteSize();

    // if "iSubjectSize" is not initialized get the size from MTM
    if ( iSubjectSize < 0 )
        {
        // CMmsClientMtm::SubjectL() is non-leaving!
        TPtrC subject;
        TRAP_IGNORE( subject.Set( Mtm().SubjectL() ) );
        iSubjectSize = CUniDataUtils::UTF8Size( subject );
        }
        
    msgSizeBytes += iSubjectSize;

    return msgSizeBytes;
    }

// ----------------------------------------------------
// DeleteAttachmentL
// ----------------------------------------------------
//
void CUniEditorDocument::DeleteAttachmentL( TInt aSlideNum, 
                                            CUniObject* aObject, 
                                            TBool aAttachmentObject )
    {
    // This should be called only from Objects view

    // If SMIL is not MMS SMIL remove is only possible
    // for attachments in Objects view.

    if ( !aAttachmentObject )
        {
        //SmilModel takes care of the whole delete procedure
        iDataModel->SmilModel().RemoveObjectL( aSlideNum, aObject );
        }
    else
        {
        iDataModel->AttachmentList().RemoveObjectL( aObject );
        delete aObject;
        }
    }

// ----------------------------------------------------
// GetAttachmentFileL
// ----------------------------------------------------
//
RFile CUniEditorDocument::GetAttachmentFileL( TMsvAttachmentId aId )
    {
    RFile file;
    
    CMsvStore* store = Mtm().Entry().ReadStoreL();
    CleanupStack::PushL( store );
    
    MMsvAttachmentManager& manager = store->AttachmentManagerL();
    
    file = manager.GetAttachmentFileL( aId );
    
    CleanupStack::PopAndDestroy( store );
    
    return file;
    }

// ----------------------------------------------------
// UpdatedNonConformantCount
// ----------------------------------------------------
//
TInt CUniEditorDocument::UpdatedNonConformantCount() const    
    {
    TInt countNonConformant( 0 );
    TInt countSuppObjects = iDataModel->ObjectList().Count();
    TInt i( 0 );
    for (i = 0; i < countSuppObjects ; i++ )
        {
        CUniObject* obj = iDataModel->ObjectList().GetByIndex( i );
        if ( obj )
            {
            if( iDataModel->MmsConformance().MediaConformance( *obj->MediaInfo() ).iConfClass == 
                    EMmsClassUnclassified )
                {
                countNonConformant++;
                }
            }
        }
    TInt countAttObjects = iDataModel->AttachmentList().Count();
    for (i = 0; i < countAttObjects ; i++ )
        {
        CUniObject* obj = iDataModel->AttachmentList().GetByIndex( i );
        if ( obj )
            {
            if( iDataModel->MmsConformance().MediaConformance( *obj->MediaInfo() ).iConfClass == 
                    EMmsClassUnclassified )
                {
                countNonConformant++;
                }
            }
        }
    return countNonConformant;
    }

// ----------------------------------------------------
// CUniEditorDocument::CreateCharConverterL
// ----------------------------------------------------
//    
void CUniEditorDocument::CreateCharConverterL( TUniMessageCharSetSupport aMode )
    {
    delete iCharConverter;
    iCharConverter = NULL;
        
    iCharConverter = CCnvCharacterSetConverter::NewL();    
    
    // TODO: Error situation needs to be handled somehow
    CCnvCharacterSetConverter::TAvailability available = 
                        CCnvCharacterSetConverter::ENotAvailable;
                        
    if ( aMode == EUniMessageCharSetFull || 
         aMode == EUniMessageCharSetFullLocked )
        {
        // For Full CharacterSet support we use 
        // S60 SmsStrict converter plug-in
        available = iCharConverter->PrepareToConvertToOrFromL( KSmsEdSmsStrictPluginID,
                                                               iFs );
        }
    else
        {
        // For Reduced support we use converter plug-in
        // provided by Symbian
        available = iCharConverter->PrepareToConvertToOrFromL( KCharacterSetIdentifierSms7Bit,
                                                               iFs );
        }
    
    if ( available == CCnvCharacterSetConverter::ENotAvailable )
        {
        __ASSERT_DEBUG( EFalse, Panic( EUniCharConvNotFound ) );
        User::Leave( KErrNotFound );
        }
    
    iCharConverter->SetDowngradeForExoticLineTerminatingCharacters(
        CCnvCharacterSetConverter::EDowngradeExoticLineTerminatingCharactersToJustLineFeed );
        
    iCharSetSupport = aMode;
    }

// ----------------------------------------------------
// CUniEditorDocument::LaunchPlugings
//
// TRAP's needed as some plugins might not be available
// at all in some variants.
// ----------------------------------------------------
//
void CUniEditorDocument::LaunchPlugings()
    {
    // This might have been set in PrepareContextL or DoReplyForwardL
    // if not -> construct them here
    if( !iSmsPlugin )
        {
        TRAP_IGNORE( iSmsPlugin = CUniEditorPlugin::NewL( TUid::Uid( KUidUniEditorSmsPlugin ),
                                                          Session(),
                                                          Mtm() ) );
        }

    if( !iMmsPlugin )
        {
        TRAP_IGNORE( iMmsPlugin = CUniEditorPlugin::NewL( TUid::Uid( KUidUniEditorMmsPlugin ),
                                                          Session(),
                                                          Mtm() ) );
        }
    }

// ----------------------------------------------------
// CUniEditorDocument::MaxImageSize
// ----------------------------------------------------
//
TSize CUniEditorDocument::MaxImageSize() const
    {
    TMsvEntry tEntry = Entry();
    
    if ( TUniMsvEntry::IsMmsUpload( tEntry ) )
        {
        return TSize( KUniMmsUploadImageWidth, KUniMmsUploadImageHeight );
        }
    else
        {
        return TSize( iMaxImageWidth, iMaxImageHeight );
        }
    }

// ----------------------------------------------------
// CUniEditorDocument::SuperDistributableObjectCount
// ----------------------------------------------------
//
TInt CUniEditorDocument::SuperDistributableObjectCount() const    
    {
    TInt resultCount( 0 );
    TInt countSuppObjects = iDataModel->ObjectList().Count();
    TInt i( 0 );
    for ( i = 0; i < countSuppObjects ; i++ )
        {
        CUniObject* obj = iDataModel->ObjectList().GetByIndex( i );
        if ( obj->MediaInfo() &&  
             obj->MediaInfo()->Protection() & EFileProtSuperDistributable )
            {
            resultCount++;
            }
        }
    TInt countAttObjects = iDataModel->AttachmentList().Count();
    for ( i = 0; i < countAttObjects ; i++ )
        {
        CUniObject* obj = iDataModel->AttachmentList().GetByIndex( i );
        if ( obj->MediaInfo() &&  
             obj->MediaInfo()->Protection() & EFileProtSuperDistributable )
            {
            resultCount++;
            }
        }
    return resultCount;
    }

// ----------------------------------------------------
// CUniEditorDocument::UniState
// ----------------------------------------------------
//
TUniState CUniEditorDocument::UniState() const
    {
    CUniClientMtm& mtm = static_cast <CUniClientMtm&> ( CMsgEditorDocument::Mtm() );
    
    if ( mtm.MessageTypeLocking() == EUniMessageTypeLocked )
        { 
        // Ok, message type is locked
        if ( mtm.MessageTypeSetting() == EUniMessageTypeSettingSms )
            { 
            // Locked to SMS - it must be SMS
            return EUniSms;
            }
        else if ( iBodyState == EUniSms && 
                  iHeaderState ==  EUniSms && 
                  mtm.MessageTypeSetting() == EUniMessageTypeSettingAutomatic )
            { 
            // locked to auto and both body and headers are SMS -> SMS
            return EUniSms;
            }
        else
            { 
            // otherwise it's MMS
            return EUniMms;
            }
        }
    else
        { 
        // type is not locked
        if ( iBodyState ==   EUniSms && 
             iHeaderState == EUniSms && 
             mtm.MessageTypeSetting() != EUniMessageTypeSettingMms )
            { 
            // type is not mms and both body and headers are sms
            return EUniSms;
            }
        else
            {
            // otherwise it's MMS
            return EUniMms;       
            }
        }
    }
    
// ----------------------------------------------------
// CUniEditorDocument::SmsCharacterLimits
// ----------------------------------------------------
//
void CUniEditorDocument::SmsCharacterLimits( TInt& aSinglePartLength, 
                                             TInt& aConcatenatedPartLength ) const
    {
    if ( UnicodeCharacterMode() )
        {
        aSinglePartLength = KFirstUnicodeSmsLength;
        aConcatenatedPartLength = KUnicodeConcatenationInterval;
        }
   else
        {
        aSinglePartLength = KFirstNormalSmsLength;
        aConcatenatedPartLength = KNormalConcatenationInterval;
        }
    }

// ----------------------------------------------------
// CUniEditorDocument::GetLanguageSpecificAltEncodingType
// @return the alternate encoding type based on input language id.
// Turkish SMS-PREQ2265 specific
//
// NOTE: In future based on the new requirements, new language(supporting NLT) and the corresponding 
//       alternate encoding type can be added to this function and used appropriately
// ----------------------------------------------------

TSmsEncoding CUniEditorDocument::GetLanguageSpecificAltEncodingType(TInt aLangId)
    {
    TSmsEncoding alternateEncodingType = ESmsEncodingNone;
    switch(aLangId)
        {
        case ELangTurkish:
            {
            alternateEncodingType = ESmsEncodingTurkishSingleShift;
            break;
            }
        default:
            {
            break;
            }
        }
    return alternateEncodingType;
    }

//  End of File
