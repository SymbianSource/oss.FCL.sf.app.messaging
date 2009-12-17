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
*     Mixed class to offer different account functions.
*     Implemented by CMceUi.
*
*/




// INCLUDE FILES
#include <msvids.h>
#include <msvapi.h>
#include <AknUtils.h>     // AknTextUtils
#include "MceUtils.h"
#include "MceIds.h"
#include "MceListItem.h"
#include <featmgr.h>

#include <messagingvariant.hrh>
#include <centralrepository.h>
#include <MessagingDomainCRKeys.h>
#include <centralrepository.h>
#include <messaginginternalcrkeys.h>

#include <NumberGroupingCRKeys.h>

#include <mmsvattachmentmanager.h>
#include <cmsvattachment.h>

#include <SenduiMtmUids.h>  // mtm uids
#include <mmsconst.h>
#include <mmsgenutils.h>
#include <mmsclient.h>
#include <mtclreg.h>
#include <mtudreg.h>        // CMtmUiDataRegistry
#include <miutset.h>
#include <commonphoneparser.h>
#include <smuthdr.h>

#include <ImumInternalApi.h>
#include <ImumInHealthServices.h>

// CONSTANTS

const TInt KMceReplaceCharacterCount = 3;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// MceUtils::IsEntryFixed
// This is static function
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool MceUtils::IsEntryFixed(TMsvId aId)
    {
    return (aId == KMsvGlobalInBoxIndexEntryId
        || aId == KMsvGlobalOutBoxIndexEntryId
        || aId == KMsvDraftEntryId
        || aId == KMsvSentEntryId
        || aId == KMsvLocalServiceIndexEntryId
        || aId == KMceDocumentsEntryId
        || aId == KMceTemplatesEntryId
        );
    }

// ---------------------------------------------------------
// MceUtils::ReplaceCharacters
// This is static function
// (other items were commented in a header).
// ---------------------------------------------------------
//
void MceUtils::ReplaceCharacters( TDes& aText )
    {
    TBuf<KMceReplaceCharacterCount> replaceChars;
    replaceChars.Zero();
    replaceChars.Append( CEditableText::EParagraphDelimiter );
    replaceChars.Append( CEditableText::ETabCharacter );
    replaceChars.Append( CEditableText::EByteOrderMark );
    AknTextUtils::ReplaceCharacters(
        aText,
        replaceChars,
        CEditableText::ESpace );
    }

// ---------------------------------------------------------
// MceUtils::GetIrFilePathL
// This is static function
// (other items were commented in a header).
// ---------------------------------------------------------
//
void MceUtils::GetIrFilePathL( CMsvSession& aSession, TMsvId aEntryId, TFileName& aFileName, RFile& aFileHandle, TInt& aSize )
    {
    aFileName.Zero();

	// Get attachment manager from the entry's store

    CMsvEntry* message = aSession.GetEntryL( aEntryId );
    CleanupStack::PushL( message ); // message
    
     // In case of incomplete message which has no attachment
    if ( message->Count() )
        {
        CMsvEntry* entry = aSession.GetEntryL( ((*message)[0]).Id() );
        CleanupStack::PushL( entry ); // entry, message
        CMsvStore* store = entry->ReadStoreL();
        CleanupStack::PushL( store ); // store, entry, message
        
        MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    	
    	if ( attachMan.AttachmentCount() != 1 )
    		{
    		User::Leave( KErrNotFound );
    		}
    	
    	// Get the path
    	CMsvAttachment *attachment = attachMan.GetAttachmentInfoL(0);
    	CleanupStack::PushL( attachment ); // attachment, store, entry, message
    	const TDesC& filePath = attachment->FilePath();	
    	// Save
    	aFileName.Copy( filePath );
    	
        aFileHandle = attachMan.GetAttachmentFileL( 0 );
    	
    	// Get the size
    	aSize = attachment->Size();
        
        CleanupStack::PopAndDestroy( 4 ); // attachment, store, entry, message
        }
    else
        {
        CleanupStack::PopAndDestroy(); // message 
        User::Leave( KErrNotFound );
        }    
    }

// ---------------------------------------------------------
// MceUtils::MemoryInUseOptionL
// This is static function
// Returns ETrue if 'Memory in use' option is enabled in
// mce/settings
// ---------------------------------------------------------
//
TBool MceUtils::MemoryInUseOptionL( )
    {
    TBool memoryInUse = ETrue;
    if ( FeatureManager::FeatureSupported( KFeatureIdMmc ) )
        {        
        if ( FeatureManager::FeatureSupported( KFeatureIdMmcHotswap) )
            {
            // if not hotswap phone, then Memory in use is enabled
            // if hotswap phone, check from shared data, if
            // Memory in use is enabled            

            CRepository* repository = NULL;
            TRAPD( ret, repository = CRepository::NewL(KCRUidMuiuVariation) );
            CleanupStack::PushL( repository );

            TInt featureBitmask = 0;

            if ( ret == KErrNone )
                {
                if ( repository->Get(KMuiuMceFeatures,featureBitmask) != KErrNone )
                    {
                    memoryInUse = ETrue;
                    }
                else
                    {
                    if ( !(featureBitmask & KMceFeatureIdHotswap) )
                        {
                        memoryInUse = EFalse;
                        }
                    }
                }

            CleanupStack::Pop( repository );
            delete repository;

            }
        
        }
    else
        {
        //no mmc support
        memoryInUse = EFalse;
        }
    return memoryInUse;     
    }
   
// ---------------------------------------------------------
// MceUtils::GetHealthyMailboxList
// This is static function
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt MceUtils::GetHealthyMailboxListL(
    const MImumInHealthServices& aHealthServices,
    MImumInHealthServices::RMailboxIdArray& aMailboxIdArray,
    TBool aGetImap4,
    TBool aGetPop3,
    TBool aGetSyncML,
    TBool aGetOther )
    {
    TInt64 mailboxFlags = MImumInHealthServices::EFlagGetHealthy;

    TInt mtmPluginID = 0;
    if ( FeatureManager::FeatureSupported( KFeatureIdEmailMceIntegration ) )
        {
        // Check if there is other than platform email application registered
        // to handle S60 mailboxes	
        CRepository* repository = NULL;
         TRAPD( ret, repository = CRepository::NewL(
                                 KCRUidSelectableDefaultEmailSettings ) );

        if ( ret == KErrNone )
            {
            TInt err = repository->Get( KIntegratedEmailAppMtmPluginId,
                                        mtmPluginID );
            if ( err != KErrNone )
                {
                mtmPluginID = 0;
                }
            }
        delete repository;
        }
    if ( mtmPluginID == 0 )
        {
    if ( aGetImap4 )
        {
        mailboxFlags |= MImumInHealthServices::EFlagIncludeImap4;
        }
    if ( aGetPop3 )
        {
        mailboxFlags |= MImumInHealthServices::EFlagIncludePop3;
        }
        }
    if ( aGetSyncML )
        {
        mailboxFlags |= MImumInHealthServices::EFlagIncludeSyncMl;
        }
    if ( aGetOther )
        {
        mailboxFlags |= MImumInHealthServices::EFlagIncludeOther;
        }
        
    TInt error = aHealthServices.GetMailboxList( 
        aMailboxIdArray,
        mailboxFlags );
    
    return error;
    }

// ---------------------------------------------------------
// MceUtils::ValidPhoneNumberL
// This is static function
// Returns ETrue if phonenumber is valid
// ---------------------------------------------------------
//
TBool MceUtils::ValidPhoneNumberL( 
    TMsvId aEntryId,
    CMsvSession& aSession,
    TPhCltTelephoneNumber& aPhoneNumber,
    TDes* aEntryDetails )
    {
    TBool validNumber = EFalse;
    TMsvEntry entry;
    TMsvId service = KMsvLocalServiceIndexEntryId;    
    User::LeaveIfError( aSession.GetEntry( aEntryId, service, entry ) );
    if ( aEntryDetails )
        {
        aEntryDetails->Copy( entry.iDetails.Left( KMceVisibleTextLength ) );
        }
    if ( entry.iMtm == KSenduiMtmSmsUid || entry.iMtm == KSenduiMtmBioUid )
        {
        MceUtils::GetSmsNumberL( aEntryId, aSession, aPhoneNumber );
        validNumber = CommonPhoneParser::IsValidPhoneNumber( 
            aPhoneNumber, CommonPhoneParser::ESMSNumber );                               
        }
    else if ( entry.iMtm == KSenduiMtmMmsUid || entry.iMtm == KSenduiMMSNotificationUid )
        { 
        MceUtils::GetMmsNumberL( entry, aSession, aPhoneNumber );
        validNumber = CommonPhoneParser::IsValidPhoneNumber( 
            aPhoneNumber, CommonPhoneParser::ESMSNumber );
        }
    return validNumber;
    }
    
// ---------------------------------------------------------
// MceUtils::ValidPhoneNumberL
// This is static function
// Returns ETrue if phonenumber is valid
// ---------------------------------------------------------
//
TBool MceUtils::ValidPhoneNumberL( 
    TMsvId aEntryId,
    CMsvSession& aSession )
    {
    TPhCltTelephoneNumber number;
    
    
    return MceUtils::ValidPhoneNumberL( 
        aEntryId,
        aSession,
        number,
        NULL );
    }
    
// ----------------------------------------------------
// MceUtils::GetSmsNumberL
// Gets phone number from SMS entry
// ----------------------------------------------------
void MceUtils::GetSmsNumberL( 
    TMsvId aEntryId, 
    CMsvSession& aSession,
    TPhCltTelephoneNumber& aNumber )
    {
    CMsvEntry* parentEntry = aSession.GetEntryL( aEntryId );
    CleanupStack::PushL( parentEntry );
    CPlainText* nullString = CPlainText::NewL();
    CleanupStack::PushL( nullString );
    CSmsHeader* header = CSmsHeader::NewL( CSmsPDU::ESmsDeliver, *nullString );
    CleanupStack::PushL( header );
    CMsvStore* store = parentEntry->ReadStoreL();
    CleanupStack::PushL( store );
    header->RestoreL( *store );
    aNumber = header->FromAddress();
    CleanupStack::PopAndDestroy( 4 ); // store, header, parentEntry, nullString
    }

// ----------------------------------------------------
// MceUtils::GetMmsNumberL
// Gets phone number from MMS entry
// ----------------------------------------------------
void MceUtils::GetMmsNumberL( 
    TMsvEntry& aEntry, 
    CMsvSession& aSession,
    TPhCltTelephoneNumber& aNumber )
    {
    TMsvId id = aEntry.Id();
    CClientMtmRegistry* mtmRegistry = CClientMtmRegistry::NewL( aSession );
    CleanupStack::PushL( mtmRegistry );

    CMmsClientMtm* mmsMtm = (CMmsClientMtm *) mtmRegistry->NewMtmL( aEntry.iMtm );
    CleanupStack::PushL( mmsMtm );
    mmsMtm->SwitchCurrentEntryL( aEntry.Id() );
    mmsMtm->LoadMessageL();
    TPtrC sender = mmsMtm->Sender();
    aNumber = TMmsGenUtils::PureAddress( sender );
    CleanupStack::PopAndDestroy( 2 ); // mtmRegistry, mmsMtm
    }



// ---------------------------------------------------------
// MceUtils::StripAndAppendStringL
// This is static function
// ---------------------------------------------------------
//
void MceUtils::StripAndAppendStringL(
    TDes& aTargetString, 
    const TDesC& aSourceString, 
    TInt aDescriptionLength )
    {
    TBool addThreeDots = EFalse;
    
    if ( aDescriptionLength <= 0 )
        {
        aDescriptionLength = MceUtils::DescriptionLength();
        }

    TInt length = aSourceString.Left( aDescriptionLength ).Length();
    if ( length == 0 )
        {
        // append space, needed by list box
        aTargetString.Append( CEditableText::ESpace );
        return;
        }

    if ( aSourceString.Length() > length )
        {
        length--;
        addThreeDots = ETrue;
        }
        
    HBufC* tempString = aSourceString.Left( length ).AllocLC();
    TPtr tempStringPtr = tempString->Des();
    MceUtils::ReplaceCharacters( tempStringPtr );
    aTargetString.Append( tempStringPtr );
    CleanupStack::PopAndDestroy( tempString );
    if ( addThreeDots )
        {
        aTargetString.Append( CEditableText::EEllipsis );
        }
    }
    
    
// ---------------------------------------------------------
// MceUtils::DescriptionLength
// This is static function
// ---------------------------------------------------------
//
TInt MceUtils::DescriptionLength()
    {
    TInt descriptionLength = KMceVisibleTextLength;
    TRAPD( error, descriptionLength = MceUtils::DescriptionLengthL() );
    if ( error != KErrNone )
        {
        descriptionLength = KMceVisibleTextLength;
        }
    return descriptionLength;
    }

// ---------------------------------------------------------
// MceUtils::DescriptionLengthL
// This is static function
// ---------------------------------------------------------
//
TInt MceUtils::DescriptionLengthL()
    {
    TInt descriptionLength = KMceVisibleTextLength;
    CRepository* repository = CRepository::NewLC(KCRUidMuiuSettings);
        if ( repository->Get(KMuiuDescriptionLength, descriptionLength) != KErrNone )
            {
            descriptionLength = KMceVisibleTextLength;
            }
        if ( descriptionLength < KMceVisibleTextLength )
            {
            descriptionLength = KMceVisibleTextLength;
            }
    // How to make sure it is sensible?
    else if ( descriptionLength > 3*KMceVisibleTextLength )
            {
            descriptionLength = 3*KMceVisibleTextLength;
            }
        CleanupStack::PopAndDestroy( repository );
    return descriptionLength;
    }


// ---------------------------------------------------------
// MceUtils::IsMtmBuiltIn
// This is static function
// ---------------------------------------------------------
//
TBool MceUtils::IsMtmBuiltIn( TUid aMtmUid )
    {
    TBool mtmIsBuiltIn = EFalse;
    switch ( aMtmUid.iUid )
        {
        case KSenduiMtmSmtpUidValue:
        case KSenduiMtmImap4UidValue:
        case KSenduiMtmPop3UidValue:
        case KSenduiMtmSmsUidValue:
        case KSenduiMtmMmsUidValue:
        case KSenduiMtmIrUidValue:
        case KSenduiMtmBtUidValue:
        case KSenduiMtmPushMtmUidValue:
        case KSenduiMtmSyncMLEmailUidValue:
        case KSenduiMtmBioUidValue:
        case KSenduiMtmRoUidValue:
        case KSenduiMMSNotificationUidValue:
        case KSenduiMtmPostcardUidValue:
        case KSenduiMtmAudioMessageUidValue:
        case KSenduiMtmUniMessageUidValue:
            mtmIsBuiltIn = ETrue;
            break;

        default:
            break;
        }
    return mtmIsBuiltIn;        
    }

// ---------------------------------------------------------
// MceUtils::NumberGroupingEnabled
// This is static function
// ---------------------------------------------------------
//
TBool MceUtils::NumberGroupingEnabled()
    {
    TBool numberGroupingEnabled = EFalse;
    TRAPD( error, numberGroupingEnabled = MceUtils::NumberGroupingEnabledL() );
    if ( error != KErrNone )
        {
        numberGroupingEnabled = EFalse;
        }
    return numberGroupingEnabled;
    }

// ---------------------------------------------------------
// MceUtils::NumberGroupingEnabledL
// This is static function
// ---------------------------------------------------------
//
TBool MceUtils::NumberGroupingEnabledL()
    {
    TInt numberGrouping = 0;
    CRepository* repository = CRepository::NewL(KCRUidNumberGrouping);
    // CleanupStack is not needed since no leavable functions below
    if ( repository->Get(KNumberGrouping, numberGrouping) != KErrNone )
        {
        numberGrouping = 0;
        }
    delete repository;
    
    // CenRep key KCRUidNumberGrouping / KNumberGrouping is used to check if
    // number grouping is supported
    return ( numberGrouping == 1 );
//    return FeatureManager::FeatureSupported( KFeatureIdPhoneNumberGrouping );
    }



// ---------------------------------------------------------
// TMceFlags::TMceFlags
// This is static function
// (other items were commented in a header).
// ---------------------------------------------------------
//
TMceFlags::TMceFlags()
    :    
    iFlags( 0 )
    {
    }

//  End of File
