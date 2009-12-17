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
* Description: 
*       Static functions for imum
*
*/


// INCLUDE FILES
#include <bldvariant.hrh>
#include <miutset.h>
#include <s32file.h>
#include <eikenv.h>
#include <eikrutil.h>
#include <eikbtgpc.h>
#include <iapprefs.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <AknGlobalNote.h>
#include <sendui.h>
#include <ErrorUI.h>                    // CErrorUI
#include <AknQueryDialog.h>
#include <aknnotedialog.h>
#include <StringLoader.h>
#include <MuiuOperationWait.h>          // CMuiuOperationWait
#include <MuiuMsvUiServiceUtilities.h>  // MsvUiServiceUtilities
#include <imapset.h>
#include <pop3set.h>
#include <smtpset.h>
#include <MTMStore.h>                   // CMtmStore
#include <mtmuibas.h>                   // BaseMtmUi
#include <messaginginternalcrkeys.h>    // Messaging keys
#include <centralrepository.h>          // CRepository
#include <CoreApplicationUIsSDKCRKeys.h>
#include <cemailaccounts.h>
#include <bldvariant.hrh>
#include <messagingvariant.hrh>
#include <MNcnInternalNotification.h>
#include <data_caging_path_literals.hrh>    // KDC_MTM_RESOURCE_DIR
#include <ImumUtils.rsg>
#include <bautils.h>
#include <muiuemailtools.h>
#include "ImumInMailboxUtilitiesImpl.h"
#include "ImumInMailboxServicesImpl.h"

#include "SenduiMtmUids.h"
#include "ImumMboxManager.h"
#include "ImumMboxData.h"
#include "EmailFeatureUtils.h"
#include "EmailUtils.H"
#include "ImumUtilsLogging.h"
#include "ImumPanic.h"
#include <ImumUtils.rsg>
#include "ImumMboxSettingsCtrl.h"
#include "ImumUtilsLogging.h"
#include "IMSSettingsNoteUi.h"
#include "ImumInternalApiImpl.h"
#include "ImumDaSettingsKeys.h"
#include "ImumInSettingsData.h"             // CImumInSettingsData

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
const TInt KBytesInKiloByte = 1024;
const TInt KImumSizeBufferSize = 32;
const TInt KImumReplaceCharacterCount = 2;
const TInt KIMSMaxAoMailboxes = 2;
const TInt KImumExtraStringLength = 4;
_LIT( KImumDefaultNameFormat, "%S(%02d)" );
_LIT(KEmailUtilsComma, ",");
_LIT(KEmailUtilsSpace, " ");
_LIT( KIMSDirAndResFileName,"z:ImumUtils.rsc" );

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::CheckAvailableDiskSpaceForDownloadL
// This is static function
// ----------------------------------------------------------------------------
//
EXPORT_C TBool MsvEmailMtmUiUtils::CheckAvailableDiskSpaceForDownloadL(
    TInt aMsgSize,
    CEikonEnv& aEikonEnv,
    CMsvSession& aSession )
//
// Check there is enough disk space on the specified drive to fetch a message
// of aMsgSize bytes. If not, display a message and return EFalse.
// static
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::CheckAvailableDiskSpaceForDownloadL, 0, utils, KLogUi );
    
    if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(
        aSession, aMsgSize ) )
        {
        CErrorUI* errorUi = CErrorUI::NewLC( aEikonEnv );
        errorUi->ShowGlobalErrorNoteL( KErrDiskFull );
        CleanupStack::PopAndDestroy( errorUi );
        return EFalse;
        }
    return ETrue;
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::CheckActiveAOLimitsL()
//
// This function goes through all the mailboxes and checks, if there are any
// Always Online enabled boxes around. The included mailbox id is ignored
// during the search. If less than 2 ( <2 ) always online mailboxes are found,
// ETrue is returned, otherwise EFalse is returned.
// ----------------------------------------------------------------------------
//
TBool MsvEmailMtmUiUtils::CheckActiveAOLimitsL(
    CImumInternalApi& aEmailApi,    
    const TMsvId aMailboxId )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::CheckActiveAOLimitsL, 0, utils, KLogUi );
    
    // Get the list of healthy mailboxes from the API
    TInt count = 0;    
    const MImumInHealthServices& healthApi = aEmailApi.HealthServicesL();
    MImumInHealthServices::RMailboxIdArray mailboxes;
    
    // Get the list of mailboxes in array. Gather the list based on the smtpId, 
    // since the aMailboxId is actually also the smtpId
    if ( healthApi.GetMailboxList( mailboxes, 
    		MImumInHealthServices::EFlagIncludeSmtp | 
    		MImumInHealthServices::EFlagGetHealthy ) == KErrNone )
    	{
    	for ( TInt mbox = mailboxes.Count(); --mbox >= 0; )
    		{           
            if ( aMailboxId != mailboxes[mbox] )
                {
                // Create object for the extended mail settings
                CImumInSettingsData* settings = 
                    aEmailApi.MailboxServicesL().LoadMailboxSettingsL( 
                        mailboxes[mbox] );
                CleanupStack::PushL( settings );
                
                TInt aoState = 0;
                if ( settings )
                	{
	                settings->GetAttr(
	                    TImumDaSettings::EKeyAutoRetrieval, aoState );

					// If always online is on, increase the count
					if ( aoState != TImumDaSettings::EValueAutoOff )
						{
						count++;
						}
                	}
                CleanupStack::PopAndDestroy( settings );
                settings = NULL;                         
                }                
            }
        }
    
    mailboxes.Reset();

    return count < KIMSMaxAoMailboxes;
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::DoCEntryCopyMoveL
// This is static function
// ----------------------------------------------------------------------------
//
EXPORT_C CMsvOperation* MsvEmailMtmUiUtils::DoCEntryCopyMoveL(
    CMsvEntry& aEntry, const CMsvEntrySelection& aSel, TMsvId aTargetId, 
    TRequestStatus& aCompletionStatus, TBool aCopy)
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::DoCEntryCopyMoveL, 0, utils, KLogUi );
    
    CMsvOperation* op = NULL;
    if(aCopy)
        {
        op = aEntry.CopyL(aSel, aTargetId, aCompletionStatus);
        }
    else
        {
        op = aEntry.MoveL(aSel, aTargetId, aCompletionStatus);
        }
    return op;
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::StripCompleteEntriesLC
// This is static function
// ----------------------------------------------------------------------------
//
EXPORT_C CMsvEntrySelection* MsvEmailMtmUiUtils::StripCompleteEntriesLC(
    const CMsvEntry& aParent, const CMsvEntrySelection& aSelection)
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::StripCompleteEntriesLC, 0, utils, KLogUi );
    
    CMsvEntrySelection* entries=aSelection.CopyLC();
    TInt cc = entries->Count();
    TMsvEntry tEntry;
    while(cc--)
        {
        tEntry = aParent.ChildDataL((*entries)[cc]);
        TMsvEmailEntry eEntry( tEntry );

        TBool complete = tEntry.Complete();
        TBool partial = eEntry.PartialDownloaded();

        if( complete && !partial )
            {
            entries->Delete(cc);
            }
        }
    return entries;
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::StripDeletedEntriesLC
// This is static function
// ----------------------------------------------------------------------------
//
EXPORT_C CMsvEntrySelection* MsvEmailMtmUiUtils::StripDeletedEntriesLC(
    const CMsvEntry& aParent, const CMsvEntrySelection& aSelection)
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::StripDeletedEntriesLC, 0, utils, KLogUi );
     // static
    CMsvEntrySelection* entries=aSelection.CopyLC();
    TInt cc = entries->Count();
    while(cc--)
        {
        const TMsvEmailEntry mailEntry(aParent.ChildDataL((*entries)[cc]));
        if(EDisconnectedDeleteOperation == mailEntry.DisconnectedOperation())
            {
            entries->Delete(cc);
            }
        }
    return entries;
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::SetMessageInfoDataLCCCCC
// This is static function
// sets fields in the message info data array using TMsvEntry and CImHeader
// information; the TDes parameters are needed so that the strings have
// persisted memory to live in since the info data elements are all TPtrs
// ----------------------------------------------------------------------------
//
EXPORT_C void MsvEmailMtmUiUtils::SetMessageInfoDataLCCCCC(
    TMsgInfoMessageInfoData& aInfoData,
    CBaseMtm& aMtm,
    CCoeEnv& aEnv,
    TDes& /* aDateBuf */,
    TDes& /* aTimeBuf */,
    TDes& aSizeBuf,
    TDes& aPriorityBuf )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::SetMessageInfoDataLCCCCC, 0, utils, KLogUi );
    
    //retrieve header
    CMsvStore* store = aMtm.Entry().ReadStoreL();
    CleanupStack::PushL( store );
    CImHeader* header = CImHeader::NewLC();
    header->RestoreL( *store );

    //subject
    HBufC* tmpSubj = header->Subject().AllocLC();
    aInfoData.iSubject.Set( *tmpSubj );

    //date and time
    TMsvEntry tentry = aMtm.Entry().Entry();
    aInfoData.iDateTime = tentry.iDate;

    //size
    TInt size = tentry.iSize / KBytesInKiloByte; //in bytes
    if ( tentry.iSize % KBytesInKiloByte )
        {
        size++;
        }
    TBuf<KImumSizeBufferSize> kB;
    StringLoader::Load( kB, R_IMAS_MESSAGE_INFO_KB, &aEnv );
    StringLoader::Format( aSizeBuf, kB, -1, size );
    aInfoData.iSize.Set( aSizeBuf );

    //Priority
    MsvEmailMtmUiUtils::GetPriorityString(tentry,aEnv,aPriorityBuf );             
    aInfoData.iPriority.Set( aPriorityBuf );  

    //to field
    HBufC* toList = TurnArrayIntoCommaSeparatedStringLC(
        header->ToRecipients() );
    aInfoData.iTo.Set( *toList );

    //cc field
    HBufC* ccList = TurnArrayIntoCommaSeparatedStringLC(
        header->CcRecipients() );
    aInfoData.iCC.Set( *ccList );

    //bcc field
    HBufC* bccList = TurnArrayIntoCommaSeparatedStringLC(
        header->BccRecipients() );
    aInfoData.iBCC.Set( *bccList );

    //from field
    HBufC* tmpFrom = header->From().AllocLC();
    aInfoData.iFrom.Set( *tmpFrom );

    //take HBufCs out
    CleanupStack::Pop( 5 ); // CSI: 47 # tmpFrom, bccList, ccList, toList, tmpSubj.
    //destroy store and header. if not, message info in outbox prevents
    //mail sending!
    CleanupStack::PopAndDestroy( 2 ); // CSI: 47 # header, store.
    //put HBufCs back, so no need to change function name.
    CleanupStack::PushL( tmpSubj );
    CleanupStack::PushL( toList );
    CleanupStack::PushL( ccList );
    CleanupStack::PushL( bccList );
    CleanupStack::PushL( tmpFrom );
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::SetMessageInfoDataLCC
// This is static function
// sets fields in the message info data array using TMsvEntry and CImHeader
// information; the TDes parameters are needed so that the strings have
// persisted memory to live in since the info data elements are all TPtrs
// ----------------------------------------------------------------------------
//
EXPORT_C TBool MsvEmailMtmUiUtils::SetMessageInfoDataLCC(
    TMsgInfoMessageInfoData& aInfoData,
    CBaseMtm& aMtm,
    CCoeEnv& aEnv,
    TDes& /* aDateBuf */,
    TDes& /* aTimeBuf */,
    TDes& aSizeBuf,
    TDes& aPriorityBuf )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::SetMessageInfoDataLCC, 0, utils, KLogUi );
            
    //retrieve header
    CMsvStore* store = NULL;
    CImHeader* header = NULL;
    TRAPD( storeError, store = aMtm.Entry().ReadStoreL() );
    if ( storeError == KErrNone )
        {
        CleanupStack::PushL( store );
        header = CImHeader::NewLC();
        TRAP( storeError, header->RestoreL( *store ) );
        // if restore leaves then we can find out lots of info from
        // TMsvEntry's fields.
        if ( storeError != KErrNone )
            {
            CleanupStack::PopAndDestroy( 2, store ); // CSI: 47 # header, store.
            header = NULL;
            store = NULL;
            }
        // if no errors, then store and header should be left to cleanup stack.
        }

    TMsvEntry tentry = aMtm.Entry().Entry();

    //subject
    if ( header )
        {
        aInfoData.iSubject.Set( header->Subject() );
        }
    else
        {
        aInfoData.iSubject.Set( tentry.iDescription );
        }

    //date and time
    aInfoData.iDateTime = tentry.iDate;

    //size
    TInt size = tentry.iSize / KBytesInKiloByte; //in bytes
    if ( tentry.iSize % KBytesInKiloByte )
        {
        size++;
        }
    TBuf<KImumSizeBufferSize> kB;
    StringLoader::Load( kB, R_IMAS_MESSAGE_INFO_KB, &aEnv );
    StringLoader::Format( aSizeBuf, kB, -1, size );
    aInfoData.iSize.Set( aSizeBuf );

     //Priority
     MsvEmailMtmUiUtils::GetPriorityString(tentry,aEnv,aPriorityBuf );             
    aInfoData.iPriority.Set( aPriorityBuf );  

    if ( header )
        {
        //to field
        HBufC* toList = TurnArrayIntoCommaSeparatedStringLC(
            header->ToRecipients() );
        aInfoData.iTo.Set( *toList );

        //cc field
        HBufC* ccList = TurnArrayIntoCommaSeparatedStringLC(
            header->CcRecipients() );
        aInfoData.iCC.Set( *ccList );

        //from field
        aInfoData.iFrom.Set( header->From() );
        }
    else
        {
        // to field
        HBufC* toList = HBufC::NewLC( 0 );
        aInfoData.iTo.Set( *toList );

        // cc field
        HBufC* ccList = HBufC::NewLC( 0 );
        aInfoData.iCC.Set( *ccList );

        // from field
        aInfoData.iFrom.Set( tentry.iDetails );
        }

    return ( header != NULL );
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::TurnArrayIntoCommaSeparatedStringLC
// This is static function
// returns an HBufC* which contains all of the lines in aTextArray concaternated
// together with commas separating them
// ----------------------------------------------------------------------------
//
HBufC* MsvEmailMtmUiUtils::TurnArrayIntoCommaSeparatedStringLC(CDesCArray& aTextArray)
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::TurnArrayIntoCommaSeparatedStringLC, 0, utils, KLogUi );
    
    TInt totalLength=0;
    //first calculate the length of string required
    TInt index = 0;
    for(index=0;index<aTextArray.Count();index++)
        {
        totalLength += aTextArray[index].Length();

        //don't count comma and space for last item
        if(index != ( aTextArray.Count() - 1 ) )
            {
            totalLength+=2; // CSI: 47 # comma and space.
            }
        }

    HBufC* list = HBufC::NewLC(totalLength);
    //now fill out the string
    for(index=0;index<aTextArray.Count();index++)
        {
        list->Des().Append(aTextArray[index]);

        //don't add for last item
        if(index != ( aTextArray.Count() - 1 ) )
            {
            list->Des().Append(KEmailUtilsComma);
            list->Des().Append(KEmailUtilsSpace);
            }
        }

    return list;
    }


// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::ReplaceCharacters
// This is static function
// ----------------------------------------------------------------------------
//
void MsvEmailMtmUiUtils::ReplaceCharacters( TDes& aText )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::ReplaceCharacters, 0, utils, KLogUi );
    
    TBuf<KImumReplaceCharacterCount> replaceChars;
    replaceChars.Zero();
    replaceChars.Append( CEditableText::EParagraphDelimiter );
    replaceChars.Append( CEditableText::ETabCharacter );
    AknTextUtils::ReplaceCharacters(
        aText,
        replaceChars,
        CEditableText::ESpace );
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::StripCharacters
// This is static function
// ----------------------------------------------------------------------------
//
void MsvEmailMtmUiUtils::StripCharacters( TDes& aText )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::StripCharacters, 0, utils, KLogUi );
    
    TBuf<KImumReplaceCharacterCount> stripChars;
    stripChars.Zero();
    stripChars.Append( CEditableText::EParagraphDelimiter );
    stripChars.Append( CEditableText::ETabCharacter );
    AknTextUtils::StripCharacters(
        aText,
        stripChars );
    }


// ----------------------------------------------------------------------------
// DisconnectMailboxL
// ----------------------------------------------------------------------------
//
EXPORT_C TBool MsvEmailMtmUiUtils::DisconnectMailboxL(
    const TMsvEntry& aService, CMsvSession& aSession )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::DisconnectMailboxL, 0, utils, KLogUi );
    
    __ASSERT_DEBUG( aService.iMtm != KSenduiMtmSmtpUid,
        User::Panic( KImumMtmUiPanic, EPanicRcvServiceMustBePOP3OrIMAP4) );

    if ( CIMSSettingsNoteUi::ShowQueryL(
        R_IMAS_SETTINGS_DIALOG_MAILSET_DISCON, R_EMAIL_CONFIRMATION_QUERY ) )
        {
        CMtmStore* mtmStore = CMtmStore::NewL( aSession );
        CleanupStack::PushL(mtmStore);
        CBaseMtmUi& relatedMtmUi = mtmStore->GetMtmUiAndSetContextLC(aService);
        CMuiuOperationWait* wait =
            CMuiuOperationWait::NewLC( EActivePriorityWsEvents + 10 ); // CSI: 47 # <Insert comment here>.
        CMsvOperation* op = relatedMtmUi.CloseL(wait->iStatus);
        wait->Start();
        CleanupStack::PopAndDestroy( wait );
        delete op;
        CleanupStack::PopAndDestroy( 2, mtmStore ); // CSI: 47 # release mtmui.

        return ETrue; // did disconnect succeed?
        }
    else
        {
        return EFalse;
        }
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::DoOfflineChecksL()
// Function to check offline mode
// ----------------------------------------------------------------------------
//
EXPORT_C TBool MsvEmailMtmUiUtils::DoOfflineChecksL( TMsvId /*aService*/ )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::DoOfflineChecksL, 0, utils, KLogUi );
    
    if ( !MsvEmailMtmUiFeatureUtils::LocalFeatureL(
        KCRUidCoreApplicationUIs, KCoreAppUIsNetworkConnectionAllowed, 1 ) )
        {
        // offline mode has been set
        HBufC* text = StringLoader::LoadLC(
            R_IMUM_OFFLINE_NOT_POSSIBLE, CCoeEnv::Static()  );
        CAknInformationNote* note = new (ELeave)
        CAknInformationNote();
        note->ExecuteLD( *text );
        CleanupStack::PopAndDestroy( text ); 

        return ETrue;
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::RemoveOfflineDeleteL()
// ----------------------------------------------------------------------------
//
EXPORT_C TInt MsvEmailMtmUiUtils::RemoveOfflineDeleteL(
    CBaseMtm& aBaseMtm,
    TInt aFunctionId,
    TMsvId aEntryId )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::RemoveOfflineDeleteL, 0, utils, KLogUi );
    
    TBuf8<1> params;
    params.Zero();

    // Create the selection and add the id into it
    CMsvEntrySelection* selection = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    selection->AppendL( aEntryId );

    // Create waiting operation and the main operation
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    CMsvOperation* op = aBaseMtm.InvokeAsyncFunctionL(
        aFunctionId,
        *selection,
        params,
        wait->iStatus );

    // Start the waiter operation
    wait->Start();

    // Get the error from waiter
    TInt error = wait->iStatus.Int();

    // Cleanup the objects
    delete op;
    op = NULL;    
    CleanupStack::PopAndDestroy( 2, selection ); // CSI: 47 # wait, selection.
    wait = NULL;
    selection = NULL;

    return error;
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::AcquireDiskSpace()
// ----------------------------------------------------------------------------
//
EXPORT_C TInt MsvEmailMtmUiUtils::AcquireDiskSpace(
    RFs& aFileSession,
    const TInt aDriveNo,
    const TInt aSize )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::AcquireDiskSpace, 0, utils, KLogUi );
    
    // Try to reserve diskspace
    TInt error = aFileSession.ReserveDriveSpace(
        aDriveNo, aSize );

    // If reserving ok, get access
    if ( error == KErrNone )
        {
        error = aFileSession.GetReserveAccess( aDriveNo );
        }

    return error;
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::SendAOCommandL()
// ----------------------------------------------------------------------------
//
EXPORT_C void MsvEmailMtmUiUtils::SendAOCommandL(
    const TAlwaysOnlineServerAPICommands aCommand,
    TMsvId aMailboxId )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::SendAOCommandL, 0, utils, KLogUi );
    
    // Create connection to Always Online
	RAlwaysOnlineClientSession aoclient;
	CleanupClosePushL(aoclient);
	User::LeaveIfError(aoclient.Connect());

	// Prepare the parameters to be forwarded to AO-server
	TPckg<TMsvId> param = aMailboxId;

	// Send message to server and close it
	aoclient.RelayCommandL(aCommand, param);
	CleanupStack::PopAndDestroy();
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::GetAccountId()
// ----------------------------------------------------------------------------
//
TUint32 MsvEmailMtmUiUtils::GetAccountId(
    const CImumMboxData& aAccountSettings )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::GetAccountId, 0, utils, KLogUi );
    
    if ( aAccountSettings.iIsImap4 )
        {
        return aAccountSettings.iImap4AccountId.iImapAccountId;
        }
    else
        {
        return aAccountSettings.iPop3AccountId.iPopAccountId;
        }
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::CreateCenRepConnectionL()
// ----------------------------------------------------------------------------
//
void MsvEmailMtmUiUtils::CreateCenRepConnectionL(
    CRepository*& aRepository,
    TUid aRepositoryUid )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::CreateCenRepConnectionL, 0, utils, KLogUi );
    
    // If session doesn't exist, create one
    if ( !aRepository )
        {
        aRepository = CRepository::NewL( aRepositoryUid );
        }
    }
// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::StoreSecuritySettings()
// ----------------------------------------------------------------------------
//
void MsvEmailMtmUiUtils::StoreSecuritySettings(
    CImBaseEmailSettings& aSettings,
    const TIMASMailSecurity& aSecurity )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::StoreSecuritySettings, 0, utils, KLogUi );
    
    switch( aSecurity )
        {        
        case EImumSecurityOn:
            aSettings.SetSecureSockets( ETrue );
            aSettings.SetSSLWrapper( EFalse );
            break;
            
        case EImumSecurityMs:
            aSettings.SetSecureSockets( EFalse );
            aSettings.SetSSLWrapper( ETrue );
            break;
                
        case EImumSecurityOff:
            aSettings.SetSecureSockets( EFalse );
            aSettings.SetSSLWrapper( EFalse );
            break;

        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::StoreSecuritySettings()
// ----------------------------------------------------------------------------
//
void MsvEmailMtmUiUtils::StoreSecuritySettings(
    CImBaseEmailSettings& aSettings,
    const TImumDaSettings::TSecurityValues& aSecurity )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::StoreSecuritySettings, 0, utils, KLogUi );
    
    switch( aSecurity )
        {
        case TImumDaSettings::EValueSecurityTls:        
            aSettings.SetSecureSockets( ETrue );
            aSettings.SetSSLWrapper( EFalse );
            break;
    
        case TImumDaSettings::EValueSecuritySsl:        
            aSettings.SetSecureSockets( EFalse );
            aSettings.SetSSLWrapper( ETrue );
            break;
        
        case TImumDaSettings::EValueSecurityOff:        
            aSettings.SetSecureSockets( EFalse );
            aSettings.SetSSLWrapper( EFalse );
            break;

        default:
            break;
        }
    }


// ---------------------------------------------------------------------------
// MsvEmailMtmUiUtils::RestoreSecuritySettings()
// ---------------------------------------------------------------------------
//
TInt MsvEmailMtmUiUtils::RestoreSecuritySettings(
    const CImBaseEmailSettings& aSettings )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::RestoreSecuritySettings, 0, utils, KLogUi );
    
    TInt security = 0;
    
    // Security: TLS
    if( aSettings.SecureSockets() )
        {
        security = TImumDaSettings::EValueSecurityTls;
        }
    // Security: MS
    else if( aSettings.SSLWrapper() )
        {
        security = TImumDaSettings::EValueSecuritySsl;
        }
    // Security Off
    else
        {
        security = TImumDaSettings::EValueSecurityOff;
        }    
        
    return security;                
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::GetDefaultSecurityPort()
// ----------------------------------------------------------------------------
//
TUint32 MsvEmailMtmUiUtils::GetDefaultSecurityPort(
    const TIMASMailSecurity aSecurity,
    const TBool aIncoming,
    const TBool aIsImap4 )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::GetDefaultSecurityPort, 0, utils, KLogUi );
    
    TUint32 port( 0 ); // this should never be returned

    switch( aSecurity )
        {
        case EImumSecurityOn:
        case EImumSecurityOff:
            {
            if ( aIncoming )
                {
                port = aIsImap4 ? KIMASDefaultPortImap4 :
                    KIMASDefaultPortPop3;
                }
            else
                {
                port = KIMASDefaultPortSmtp;
                }
            }
            break;
        case EImumSecurityMs:
            {
            if ( aIncoming )
                {
                port = aIsImap4 ? KIMASDefaultSecurityPortImap4 :
                    KIMASDefaultSecurityPortPop3;
                }
            else
                {
                port = KIMASDefaultSecurityPortSmtp;
                }
            }
            break;
        default:
            break;
        }
    return port;
    }
    
// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::CallNewMessagesL()
// ----------------------------------------------------------------------------
//
EXPORT_C void MsvEmailMtmUiUtils::CallNewMessagesL( const TMsvId aMailboxId )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::CallNewMessagesL, 0, utils, KLogUi );
    
    MNcnInternalNotification* notification = NULL;

    // Create a dummy array. It is not currently used to anything.
    CDesCArrayFlat* dummyArray = new (ELeave) CDesCArrayFlat( 1 );
    CleanupStack::PushL( dummyArray );

    // No need to tell user, that we could not connect to NCN
    TRAP_IGNORE( notification =
        MNcnInternalNotification::CreateMNcnInternalNotificationL() );

    if ( notification )
        {
        // Ignore result, user is not interested about it, hopefully.
        TInt result = notification->NewMessages(
            EMailMessage, aMailboxId, *dummyArray );
        }
    else
        {
        }

    CleanupStack::PopAndDestroy( dummyArray );
    dummyArray = NULL;
    delete notification;
    notification = NULL;

    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::LoadResourceFileL()
// ----------------------------------------------------------------------------
//
EXPORT_C void MsvEmailMtmUiUtils::LoadResourceFileL(
    RConeResourceLoader& aResourceLoader )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::LoadResourceFileL, 0, utils, KLogUi );
    
    RFs fs;
    CleanupClosePushL(fs);
    User::LeaveIfError(fs.Connect());
    TFileName resourceFileName(KIMSDirAndResFileName);
    BaflUtils::NearestLanguageFile(fs,resourceFileName);
    CleanupStack::PopAndDestroy( 1 );
    
    TFileName fileName;
    TParse parse;
    parse.Set( resourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    fileName.Copy( parse.FullName() );

    // Attempt to load the resource
    aResourceLoader.OpenL( fileName );
    
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::GetMailboxName()
// ----------------------------------------------------------------------------
//
void MsvEmailMtmUiUtils::GetMailboxName(
    TDes& aDest,
    CMsvSession& aMsvSession,
    TMsvId aMailboxId )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::GetMailboxName, 0, utils, KLogUi );
    
    aDest.Zero();

    // Get mailbox
    TMsvEntry mailbox;
    TMsvId serviceId;
    TInt error = aMsvSession.GetEntry( aMailboxId, serviceId, mailbox );

    if ( error == KErrNone )
        {
        aDest.Copy( mailbox.iDetails.Left( KIMASMaxMailboxNameLength ) );
        }
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::CreateConnectingToText()
// ----------------------------------------------------------------------------
//
EXPORT_C void MsvEmailMtmUiUtils::CreateConnectingToText(
    TDes& aDest,
    CMsvSession& aMsvSession,
    TMsvId aMailboxId )
    {
    IMUM_STATIC_CONTEXT( CBaseMtmUi::EProgressStringMaxLen, 0, utils, KLogUi );

    // Get mailbox name
    TBuf<KIMASMaxMailboxNameLength> name;
    MsvEmailMtmUiUtils::GetMailboxName( name,
        aMsvSession, aMailboxId );
    
    // Combine mailbox name with a resource string
    TRAP_IGNORE( CIMSSettingsNoteUi::MakeStringL( aDest, R_EMAIL_CONNECTING_SERVER,name ) );
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::ContinueRenamingL()
// ----------------------------------------------------------------------------
//
TBool MsvEmailMtmUiUtils::ContinueRenamingL( 
    CImumInternalApi& aMailboxApi,
    TInt& aResult,
    TDes& aAccountName, 
    const TMsvId aOwningMailbox,
    const TInt aNumber,
    const CMsvEntry& aAccount,
    const TBool aGenerate )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::ContinueRenamingL, 0, utils, KLogUi );
    
    // Max number of mailboxes is checked
    // Allow name generating. This is for Email Wizard, which attempts
    // to generate the mailbox name. 
    TBool doContinue = ( aNumber <= KImumMaxMailboxes );
    
    // Check if the name is already used
    if ( doContinue )
        {
        doContinue = IsAccountNameUsedL( 
            aMailboxApi, aResult, aAccountName, aOwningMailbox, aAccount );
        }         
    
    return doContinue && aGenerate;                   
    }
    
// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::CheckAccountNameL()
// ----------------------------------------------------------------------------
//
void MsvEmailMtmUiUtils::CheckAccountNameL( 
    CImumInternalApi& aMailboxApi,      
    TDes& aAccountName, 
    const TMsvId aOwningMailbox,   
    const TBool& aGenerate )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::CheckAccountNameL, 0, utils, KLogUi );
    
    // Remove illegal characters from the name, such as tabulators and enters
    ReplaceCharacters( aAccountName );
    
    // Get entrypoint of the root
    int result = KErrNone;
    CMsvSession& session = aMailboxApi.MsvSession();
    CMsvEntry* root = session.GetEntryL( 
        KMsvRootIndexEntryIdValue );
    CleanupStack::PushL( root );

    // Sort the root
    root->SetSortTypeL( TMsvSelectionOrdering(
        KMsvGroupByType | KMsvGroupByStandardFolders,
        EMsvSortByDetailsReverse, ETrue ) );

    // This is the first suggested name of the mailbox
    TMuiuSettingsText originalName;
    originalName.Copy( aAccountName );

    // Find an account name
    TInt mailbox = 0;
    for ( TInt number = 1; ContinueRenamingL( 
            aMailboxApi, result, aAccountName, aOwningMailbox, 
            number, *root, aGenerate );
         ++number )
        {
        // Rename and set the result
        result = ChangeAccountNameL(
            originalName, aAccountName, ++mailbox );
        }

    // User must be notified, when invalid name is offered
    if ( result == KErrAlreadyExists && aGenerate )
        {
        result = KErrNone;
        }

    CleanupStack::PopAndDestroy( root );
    root = NULL;

    User::LeaveIfError( result ); 
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::IsAccountNameUsedL()
// ----------------------------------------------------------------------------
//
TBool MsvEmailMtmUiUtils::IsAccountNameUsedL(
    CImumInternalApi& aMailboxApi,
    TInt& aResult,
    const TDesC& aAccountName,  
    const TMsvId aOwningMailbox,  
    const CMsvEntry& aAccount )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::IsAccountNameUsedL, 0, utils, KLogUi );

    FeatureManager::InitializeLibL();

    if(FeatureManager::FeatureSupported(KFeatureIdSelectableEmail))
    	{
    	// Because the unhealthy mailbox's name is not used.
    	// So try to filter the unhealthy mailbox's name.
    	aResult = KErrNone;
    	}

    const MImumInHealthServices& healthServices = aMailboxApi.HealthServicesL();

    // Search through all the mail account entries until duplicate name
    // is found, or all entries are searched through    
    TInt item = aAccount.Count();
    while ( --item >= 0 && !aResult )
        {
        const TMsvEntry& entry = aAccount[item];

    	if(FeatureManager::FeatureSupported(KFeatureIdSelectableEmail))
    		{
    		// If the mailbox is unhealthy, ignore it.
    		if ( ( !healthServices.IsMailboxHealthy( entry.Id() ) ) &&
    				( !( ( entry.iType == KUidMsvServiceEntry) && 
    						( entry.Id() != KMsvLocalServiceIndexEntryId ) && 
    						( !MuiuEmailTools::IsMailMtm( entry.iMtm, ETrue ) ) ) ) )
    			{
    			continue;
    			}
    		}

        // Interrupt the search if mailbox entry with same name is found
        if ( !aAccountName.CompareC( entry.iDetails ) &&
             aMailboxApi.MailboxUtilitiesL().IsMailbox( entry ) &&
             entry.Id() != aOwningMailbox && 
             entry.iRelatedId != aOwningMailbox )
            {
            aResult = KErrAlreadyExists;
            }
        else
            {
            aResult = KErrNone;
            }                 
        }

    FeatureManager::UnInitializeLib();
    
    // If mailbox account with the same name is found, return ETrue
    return ( aResult == KErrAlreadyExists );
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::ChangeAccountNameL()
// ----------------------------------------------------------------------------
//
TInt MsvEmailMtmUiUtils::ChangeAccountNameL(
    const TDesC& aOriginalAccountName,
    TDes& aAccountName,    
    const TInt aNumber )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::ChangeAccountNameL, 0, utils, KLogUi );
    
    // Make sure the is enough room in the mailbox name for the string "(%d)"    
    const TInt maxLength = 
        KImasImailServiceNameLength - KImumExtraStringLength;

    // Remove KISIAExtraStringLength amount of letters from the string in
    // case the max length has been exceeded.
    if ( aOriginalAccountName.Length() > maxLength )
        {
        aAccountName.Copy( aAccountName.Left( maxLength ) );
        }

    // Add the brackets with number inside to the end of the string
    TMuiuSettingsText tempText;
    tempText.Copy( aAccountName );
    tempText.Format( KImumDefaultNameFormat, &aOriginalAccountName, aNumber );
    aAccountName.Copy( tempText );

    return KErrNone;
    }    

// ----------------------------------------------------------------------------
// MsvEmailMtmUiUtils::GetPriorityString()
// ----------------------------------------------------------------------------
//    
void MsvEmailMtmUiUtils::GetPriorityString(
    const TMsvEntry& aEntry,
    CCoeEnv& aEnv,
    TDes& aPriorityBuf ) 
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiUtils::GetPriorityString, 0, utils, KLogUi );
    
    TInt emailPriority = EMsvMediumPriority;
    emailPriority = aEntry.Priority();
        
    if ( emailPriority == EMsvHighPriority )
        {
        StringLoader::Load( aPriorityBuf, R_IMAS_MESSAGE_INFO_PRIORITY_HIGH, &aEnv );
        }
    else if ( emailPriority == EMsvLowPriority )
        {                                          
        StringLoader::Load( aPriorityBuf, R_IMAS_MESSAGE_INFO_PRIORITY_LOW, &aEnv );
        }
    else
        { 
        StringLoader::Load( aPriorityBuf, R_IMAS_MESSAGE_INFO_PRIORITY_NORMAL, &aEnv );
        }    
    }

// End of File


