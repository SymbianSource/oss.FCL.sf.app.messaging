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
*     Sms Ui Mtm.
*
*/



// INCLUDE FILES
#include <bldvariant.hrh>

// Specific includes
#include <msvids.h>
#include <smutset.h>
#include <smuthdr.h>
#include <tia637.h>
#include <smsMessageSettings.h>
#include <smsmessagewrappers.h>
#include <csmsaccount.h>
#include <MsvOffPeakTime.h>
using namespace tia637;

#include <smum.rsg>                       // resource ids
#include <smutcmds.hrh>
// Standard includes
#include <txtrich.h>                     // CRichText
// Comms includes
#include <AknInputBlock.h>				 // CAknInputBlock
#include <muiumsginfo.h>                 // CMsgInfoMessageInfoDialog
// other includes
#include <StringLoader.h>                // StringLoader
#include <AknPhoneNumberGrouping.h>      //CAknPhoneNumberGrouping
#include <msgbiouids.h>                  // KMsgBioUidPictureMsg
#include <MsvScheduleSettings.h>         //
// locals
#include "SmumSettingsDialogCDMA.h"      // CSmumMainSettingsDialogCDMA
#include "smsu.hrh"                      // for KMtmUiFunctionSimDialog
#include "smsucdma.h"                    // CSmsMtmUiCDMA
#include "smsui.pan"                     // panics
#include <MsvSysAgentAction.h>           // for CMsvSysAgentActions

#include <E32Property.h>                 // RProperty
#include <PsVariables.h>                 // RProperty
#include <SaCls.h>

#include <CentralRepository.h>
#include <MessagingInternalCRKeys.h>

//  LOCAL CONSTANTS AND MACROS
const TInt KMaxSubjectLength = 30;
const TInt KSmumAdditionalCharsStringLength = 6;
const TUint KSmumLRMarker = 0x200E;
const TInt KDefMaxRetries = 1;
const TInt KMsgQError = -31003;
const TInt KPendingTimeout = 4320;
_LIT( KSmumCharLeftAddressIterator, "<" );
_LIT( KSmumCharRightAddressIterator, ">" );
_LIT( KSmumCharCommaAndSpace, ", " );
_LIT( KSmumSmsService, "Sms Service" );

// Callback Number
_LIT(KDefCallbackNumber, "");

// Default values, if central repository data reading fails
const TInt KDefDeliveryReport = 1;

// Use for reading the callback number 
const TInt KMaxPhoneNumberLength = 64;


//  MEMBER FUNCTIONS

// Factory constructor function
CSmsMtmUiCDMA* CSmsMtmUiCDMA::NewL( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll )
    {
    CSmsMtmUiCDMA* smsmtmui=new ( ELeave ) CSmsMtmUiCDMA( aBaseMtm, aRegisteredMtmDll );
    CleanupStack::PushL( smsmtmui );
    smsmtmui->ConstructL();
    CleanupStack::Pop();
    return smsmtmui;
    }

// C++ constructor can NOT contain any code, that
// might leave.
CSmsMtmUiCDMA::CSmsMtmUiCDMA( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll ):
    CSmsMtmUi( aBaseMtm, aRegisteredMtmDll ),
    iMsgQueueTimeout( KPendingTimeout )
    {
    }

// Symbian OS default constructor can leave.
void CSmsMtmUiCDMA::ConstructL()
    {
    CSmsMtmUi::ConstructL();
    
    iNoCallbackNumber = StringLoader::LoadL( R_QTN_SMS_DETAIL_CALLBACK_NONE, iCoeEnv );
    }

// Destructor
CSmsMtmUiCDMA::~CSmsMtmUiCDMA()
    {
    delete iNoCallbackNumber;
    // iCenRepSession, iEditorFilename, iViewerFilename, iErrorResolver will be deallocated by its parent
    }

// ---------------------------------------------------------
// CSmsMtmUiCDMA::OpenL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUiCDMA::OpenL(TRequestStatus& aStatus)
    {

    __ASSERT_DEBUG( 
        BaseMtm().Entry().Entry().iMtm == Type(), Panic( ESmsuWrongMtmType ));
    __ASSERT_DEBUG( BaseMtm().Entry().Entry().iType != 
        KUidMsvFolderEntry, Panic( ESmsuFoldersNotSupported ));
    __ASSERT_DEBUG( BaseMtm().Entry().Entry().iType != KUidMsvAttachmentEntry, 
        Panic( ESmsuAttachmentsNotSupported ));

    const TMsvEntry myEntry = BaseMtm().Entry().Entry();
    if( myEntry.iType == KUidMsvFolderEntry || myEntry.iType == KUidMsvAttachmentEntry )
        {
        User::Leave(KErrNotSupported);
        }
    else if ( myEntry.iType == KUidMsvMessageEntry )
        {
        CheckAndEnsureSmsServiceOkL( ETrue, ETrue, ETrue );
        }
    // read-only messages and messages in the sent folder will be shown using the
    // viewer.
    if (myEntry.ReadOnly() || myEntry.Parent() == KMsvSentEntryId)
        {
        return ViewL(aStatus);
        }

    return EditL(aStatus);
    }

// ---------------------------------------------------------
// CSmsMtmUiCDMA::InvokeSyncFunctionL
//
// ---------------------------------------------------------
void CSmsMtmUiCDMA::InvokeSyncFunctionL(
    TInt aFunctionId, 
    const CMsvEntrySelection& aSelection, 
    TDes8& aParameter )
    {
    switch ( aFunctionId )
        {

        case KMtmUiFunctionSimDialog:
            break;

        case KMtmUiMessagingInitialisation:
        case KMtmUiFunctionRestoreFactorySettings: //deliberate fall through
            {
            TMsvId serviceEntryId = KMsvNullIndexEntryId;
            serviceEntryId = CreateSmsServiceL( EFalse );
            // Creates service settings and sets up default
            //
            CreateDefaultSettingsL( serviceEntryId );            
            break;
            }
        case KMtmUiFunctionSmumRemoveSMSC:
            break;
            
        default:
            CSmsMtmUi::InvokeSyncFunctionL( aFunctionId, aSelection, aParameter );
            return;
        }
    }

// ---------------------------------------------------------
// CSmsMtmUiCDMA::LaunchSettingsDialogL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUiCDMA::LaunchSettingsDialogL( TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG(
        BaseMtm().Entry().Entry().iMtm == Type(), Panic( ESmsuWrongMtmType ));
    __ASSERT_DEBUG(
        BaseMtm().Entry().Entry().iType.iUid == 
        KUidMsvServiceEntryValue, Panic( ESmsuNotAServiceEntry ));
	
	TInt err = KErrNone;
	 
    // Load settings
    //
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CSmsSettings* smsSettings = CSmsSettings::NewL();
    CleanupStack::PushL( smsSettings );
    smsAccount->LoadSettingsL( *smsSettings );
    	
    // Launch the service settings dialog on the current entry
    //
    TInt exitCode = ESmumSmsSettingsInitialvalue;
    TInt notUsed( 0 );
    CSmumMainSettingsDialog* mainSettingsDialog = CSmumMainSettingsDialog::NewL(
                                                  *smsSettings, ESmumMainSmsSettings, exitCode, notUsed );                                 
    mainSettingsDialog->ExecuteLD( R_MAIN_SETTINGS_DIALOG );

    // Save Settings
    // @todo what to do for conditional scheduling???
    //
    smsAccount->SaveSettingsL( *smsSettings );	
    CleanupStack::PopAndDestroy( 2, smsAccount ); // smsSettings, smsAccount
	
    // Check if user wants to exit to idle = ESmumSmsSettingsMenuExit
    //
    if ( exitCode == ESmumSmsSettingsMenuExit )
    	{ // Exiting...
        err = EEikCmdExit;
        }

    return CMsvCompletedOperation::NewL(
        Session(), 
        Type(), 
        KNullDesC8, 
        KMsvLocalServiceIndexEntryId, 
        aStatus, 
        err);
    }

// ---------------------------------------------------------
// CSmsMtmUi::CreateSmsServiceL
// Function should be deprecated now that we have Messaging_V2
// ---------------------------------------------------------
TMsvId CSmsMtmUiCDMA::CreateSmsServiceL( TBool aNoServiceNoCheckNeeded )
    {
    // Check for service if needed
    TMsvId serviceEntryId = KMsvNullIndexEntryId;
    TInt err = KErrNone;
    if ( !aNoServiceNoCheckNeeded )
        {
        TRAP( err, serviceEntryId = ServiceIdL());
        }

    // If no service, create one
    if ( err == KErrNotFound || aNoServiceNoCheckNeeded )
        {
        TMsvEntry entry;
        entry.iMtm = KUidMsgTypeSMS;
        entry.iType = KUidMsvServiceEntry;
        entry.SetReadOnly( EFalse );
        entry.SetVisible( EFalse );
        entry.iDate.HomeTime();
        entry.iDetails.Set( KSmumSmsService );
        CMsvEntry* root = Session().GetEntryL( KMsvRootIndexEntryId );
        CleanupStack::PushL( root );
        // In case no root store, create one...
        if ( !root->HasStoreL() )
            {
            // --- The entry does not have a store. EditStoreL() will create one ---
            CMsvStore* store = root->EditStoreL();
            CleanupStack::PushL( store );
            store->CommitL();
            CleanupStack::PopAndDestroy(); // store
            store = NULL; // destroyed
            }
        root->CreateL( entry );
        CleanupStack::PopAndDestroy(); // root
        BaseMtm().SwitchCurrentEntryL( entry.Id());
        serviceEntryId = entry.Id();
        iNewSmsSettings = ETrue;
        }
    else
        {
        User::LeaveIfError( err );
        }

    return serviceEntryId;
    }

// ---------------------------------------------------------
// CSmsMtmUiCDMA::ReadDefaultSettingsFromSharedDataL
//
// ---------------------------------------------------------
void CSmsMtmUiCDMA::ReadDefaultSettingsFromSharedDataL( CSmsSettings* aServiceSettings )
    {
    if ( iCenRepSession )
    	{
        TInt readSetting;
        TBuf<KMaxPhoneNumberLength> readSettingStr;

        // Priority
        if ( iCenRepSession->Get( KSmumMsgPriority, readSetting ) != KErrNone )
            {
            readSetting = tia637::KBdNormal;
            }
        aServiceSettings->MessageSettings().Cdma().SetPriority(readSetting);//FYI:KBdUrgent=0x02;
            
        // Callback Number
        if ( iCenRepSession->Get( KSmumCallbackNumber, readSettingStr ) != KErrNone )
            {
            readSettingStr.Copy(KDefCallbackNumber);
            }
        aServiceSettings->MessageSettings().Cdma().SetCallBackNumberL(readSettingStr);

        // Default delivery report setting
        TBool isDeliveryReportSupported = EFalse;
        TBool isDeliveryReportEnabled = EFalse;
        // Transport ACK enabled
        // delivery/user/read ACK disabled
        TSmsReplyOptions reply(ETrue, EFalse, EFalse, EFalse);

        if ( iCenRepSession->Get( KSmumDefDelReport, isDeliveryReportSupported ) != KErrNone )
            {
            isDeliveryReportSupported = EFalse;
            }
        
        if ( isDeliveryReportSupported ) 
            {
            // @note Currently we are not supporting the Delivery Report (note) functionality
            if ( iCenRepSession->Get( KSmumDeliveryReport, isDeliveryReportEnabled ) != KErrNone )
                {
                isDeliveryReportEnabled = EFalse;
                }
            reply.SetDeliveryAck(isDeliveryReportEnabled);
            aServiceSettings->SetDeliveryReport(isDeliveryReportEnabled);
            aServiceSettings->SetStatusReportHandling(CSmsSettings::EMoveReportToInboxInvisibleAndMatch);
            }
        else
            {
            aServiceSettings->SetStatusReportHandling(CSmsSettings::EDiscardReport);
            }        
        aServiceSettings->MessageSettings().SetReplyOptions(reply);

        // Message Queue On/Off
        if ( iCenRepSession->Get( KSmumMsgQueue, readSetting ) != KErrNone )
            {
            readSetting = EFalse;
            }
        iMsgQueueEnabled = readSetting;

        // Message Queue Timeout
        if ( iCenRepSession->Get( KSmumMsgQueueTimeout, readSetting ) != KErrNone )
            {
            readSetting = 0;
            }
        iMsgQueueTimeout = readSetting;
        }
    }

// ---------------------------------------------------------
// CSmsMtmUiCDMA::CreateDefaultSettingsL
//
// ---------------------------------------------------------
void CSmsMtmUiCDMA::CreateDefaultSettingsL( TMsvId aServiceId )
    {
    // P&S account
    //
    CSmsAccount* smsAccount = CSmsAccount::NewLC();

    CSmsSettings* serviceSettings = CSmsSettings::NewL();
    CleanupStack::PushL(serviceSettings);
    	
    // Init Default Settings
    smsAccount->InitialiseDefaultSettingsL( *serviceSettings );  // Creates service entry

    // Enable specific bearer subparameters
    //
    serviceSettings->MessageSettings().Cdma().ActivateMessageSetting(tia637::KTUserData,ETrue);
    serviceSettings->MessageSettings().Cdma().ActivateMessageSetting(tia637::KTPriorityIndicator, ETrue);
    serviceSettings->MessageSettings().Cdma().ActivateMessageSetting(tia637::KTCallBackNumber, ETrue);
    serviceSettings->MessageSettings().Cdma().ActivateMessageSetting(tia637::KTReplyOption, ETrue);
    serviceSettings->MessageSettings().Cdma().ActivateMessageSetting(tia637::KTLanguageIndicator, ETrue);

    // Read the RFS related settings from Central Repository data/P&S
    ReadDefaultSettingsFromSharedDataL( serviceSettings );

    // Voice mail notification handling
    // Now handled in the Bio Watchers, keep sake only for now.
    //serviceSettings->SetVMNReportHandling(CSmsSettings::EMoveReportToInboxInvisible);

    serviceSettings->MessageSettings().SetEncoding( TSmsDataCodingScheme::ESmsAlphabet7Bit );
    serviceSettings->MessageSettings().Cdma().SetLanguage( tia637::KLanguageEnglish );
    //aServiceSettings->SetSmsAckRequest(ETrue) Transport ACK on by default... no need to set?

    // Setup Message Queuing Parameters
    //
    CMsvOffPeakTimes* peakTimes = new (ELeave) CMsvOffPeakTimes();
    CleanupStack::PushL(peakTimes);
    CMsvSendErrorActions* sendErrActions = CMsvSendErrorActions::NewLC();
    CMsvSysAgentActions* actions = new (ELeave) CMsvSysAgentActions();
    CleanupStack::PushL(actions);
    CMsvScheduleSettings* schSettings = CMsvScheduleSettings::NewL();
    CleanupStack::PushL(schSettings);

    // Get default settings from SMS Account
    //
    smsAccount->LoadSettingsL( *schSettings, *peakTimes, *sendErrActions, *actions );
    actions->Reset();

    // Message queuing condition to meet in order to send the message
    // Must have network coverage, if we do not have network coverage, we fail with -31003
    // Max retry of 3 days (3430minutes) and maximum of 1 retry
    //
    if ( iMsgQueueEnabled ) 
        {
        TMsvSysAgentConditionAction action;
        action.iUseDefaultSysAgentAction = EFalse;
        action.iErrorAction.iAction = ESendActionRetryConditionMet;
        action.iErrorAction.iRetries = ESendRetriesFixed;
        action.iErrorAction.iRetrySpacing = ESendRetrySpacingStatic;
        action.iErrorAction.iError  = KMsgQError;
        action.iErrorAction.SetMaxRetries(KDefMaxRetries);

        action.iCondition.iVariable = KUidNetworkStatus; //KUidNetworkStatus;
        action.iCondition.iState = ESANetworkAvailable;    //ESANetworkAvailable
        action.iCondition.iType = TMsvCondition::EMsvSchSendEquals;
        actions->AppendL(action);
        }

    // Set schedule time outs
    //
    schSettings->SetPendingConditionsTimeout( iMsgQueueTimeout );

    // Save Settings
    //
    smsAccount->SaveSettingsL( *serviceSettings );

    // Create the message store
    //
    CMsvEntry* service = Session().GetEntryL( aServiceId );
    CleanupStack::PushL( service );
    CMsvStore* msvstore = service->EditStoreL();
    CleanupStack::PushL( msvstore );
    msvstore->CommitL();    
    CleanupStack::PopAndDestroy( 2 ); // msvstore, service

    // Cleanup Pushed items
    //
    CleanupStack::PopAndDestroy( 5, serviceSettings ); //  serviceSettings,schSettings,
                                                       //  peaktimes,actions,sendErrActions
    CleanupStack::PopAndDestroy( smsAccount );
    }

// ---------------------------------------------------------
// CSmsMtmUi::ConstructAndShowInfoDialogL
//
// ---------------------------------------------------------
void CSmsMtmUiCDMA::ConstructAndShowInfoDialogL() const
    {
    // Load the message
    //
    CSmsClientMtm& smsClientMtm = SmsClientMtm();
    smsClientMtm.LoadMessageL();
    
    // Message Header and SMS Settings
    //
    CSmsHeader& smsHeader = smsClientMtm.SmsHeader();
    TMsvMessageCdma& msg = smsHeader.CdmaMessage();
    
    // Structure to hold data for Message Info
    //
    TMsgInfoMessageInfoData infoData;

    const TMsvEntry tentry = smsClientMtm.Entry().Entry();

    // Temporary HBufs
    //
    HBufC* callback = NULL;
    HBufC* priority = NULL;
    HBufC* subject = NULL;
    HBufC* status = NULL;

    // Status
    //
    status = StatusTextL(tentry);
    if (status)
        {
        CleanupStack::PushL(status);
        }

    infoData.iStatus.Set(*status);

    // Subject
    // If this was a Picture Bio Message, show subject
    // Else, if it was a SMS message show subject field
    // For others, no subject field
    //
    if ( tentry.iBioType == KMsgBioUidPictureMsg.iUid )
        {           
        subject = ExtractTitleFromGMSBodyL( smsClientMtm.Body(), KMaxSubjectLength );
        CleanupStack::PushL( subject );          
        infoData.iSubject.Set( *subject );           
        }
    else if ( !tentry.iBioType )
        { //normal sms          
        //sms subject field (dialog truncates text to fit screen width and adds '...' so we don't need to do it here)
        TBuf<KMaxSubjectLength> subjText;
        subjText = smsClientMtm.Body().Read( 0, KMaxSubjectLength );
        subjText.TrimAll();
        subject = subjText.AllocLC();
        infoData.iSubject.Set( *subject );
        }

    // Callback Number
    //
    callback = msg.GetCallbackNumberL();
    if( callback != NULL )
        {
        CleanupStack::PushL(callback);
    	if ( callback->Length() >0 )
            {
            infoData.iCallback.Set(*callback); 
            }
        else 
            {
            infoData.iCallback.Set(*iNoCallbackNumber);
            }
        }
    else 
        {
        infoData.iCallback.Set(*iNoCallbackNumber);
        }
        
    // Priority
    //
    TPriorityIndicator priIndicator;
    msg.GetPriorityIndicatorL( priIndicator );
    switch( priIndicator )
    	{
        case tia637::KBdNormal :
            priority  = StringLoader::LoadLC( R_QTN_SMS_DET_PRIORITY_NOM, iCoeEnv );
            break;
        case tia637::KBdUrgent:
            priority  = StringLoader::LoadLC( R_QTN_SMS_DET_PRIORITY_URG, iCoeEnv );
            break;
        default:
            priority  = StringLoader::LoadLC( R_QTN_SMS_DET_PRIORITY_NOM, iCoeEnv );
            break;
        }
    infoData.iPriority.Set( *priority);


    // Sms type
    //
    HBufC* messageType = NULL; // Holds the message info text for sms
    if( tentry.iBioType )
        { 

        TInt resourceId( KErrNotFound );

        if ( tentry.iBioType == KMsgBioUidVCard.iUid )
            {
            resourceId = R_QTN_SM_INFO_BUSINESS_CARD;
            }
        else if ( tentry.iBioType == KMsgBioUidVCalendar.iUid )
            {
            resourceId = R_QTN_SM_INFO_CALENDAR;
            }
        else if ( tentry.iBioType == KMsgBioUidWmlBrSettings.iUid )
            {
            resourceId = R_QTN_SM_TITLE_CONFIGURATION;
            }
        else if ( tentry.iBioType == KMsgBioUidPictureMsg.iUid )
            {
            resourceId = R_QTN_SM_INFO_GMS;
            }
        else
            {
            // Don't change the resourceId
            }

        if ( resourceId != KErrNotFound )
            {
            messageType = iEikonEnv->AllocReadResourceLC( resourceId );
            infoData.iType.Set( *messageType );
            }
        else
            {
            infoData.iType.Set(tentry.iDescription);
            }
        }
    else
        {
        //sms type
        messageType = StringLoader::LoadLC( R_QTN_SMS_MINFO_TYPE_SMS, iCoeEnv );
        infoData.iType.Set( *messageType );
        }

    // Sms date and time
    //
    TMsvEntry entry = smsClientMtm.Entry().Entry();
    infoData.iDateTime = entry.iDate;   
    
    // Recipient info check if we are MO or MT
    // 
    if ( smsHeader.MessageType() == KSmsTypeDeliver )
        { 
        // Want to show 'viewer' type fields
        // Sms 'from' field
        //
        HBufC* senderNumber = smsHeader.FromAddress().AllocLC();
        HBufC* fromText = HBufC::NewLC( entry.iDetails.Length() + senderNumber->Length() + KSmumAdditionalCharsStringLength ); // for the chars: '<' '>' 2 x KSmumLRMarker
        
        // Do we have alias?
        if ( senderNumber->Compare( entry.iDetails ))
            { // yes
            TPtr fromTextPtr = fromText->Des();
            fromTextPtr.Append( entry.iDetails );
            fromTextPtr.Append( KSmumLRMarker );
            fromTextPtr.Append( KSmumCharLeftAddressIterator );

            // Before we append a number, we have to number group it
            HBufC* groupedNumber = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( *senderNumber );
            CleanupStack::PushL(groupedNumber);
            fromTextPtr.Append( groupedNumber->Des() );
            CleanupStack::PopAndDestroy(groupedNumber);

            fromTextPtr.Append( KSmumCharRightAddressIterator );
            fromTextPtr.Append( KSmumLRMarker );
            infoData.iFrom.Set( *fromText );
            }
        else
            { // no
            infoData.iFrom.Set( *senderNumber );
            }
        
        CMsgInfoMessageInfoDialog* infoDialog = CMsgInfoMessageInfoDialog::NewL();
        infoDialog->ExecuteLD( infoData, CMsgInfoMessageInfoDialog::ESmsViewer );

        if ( fromText )
            {
            CleanupStack::PopAndDestroy( fromText );
            }
        if ( senderNumber )
            {
            CleanupStack::PopAndDestroy( senderNumber );
            }
        }
    else
        { 
        // EditType is ETrue, want to show 'editor' type fields
        // Sms 'to:' list
        //
        HBufC* toList = TurnRecipientsArrayIntoStringLC( smsHeader.Recipients());
        infoData.iTo.Set( *toList );

        CMsgInfoMessageInfoDialog* infoDialog = CMsgInfoMessageInfoDialog::NewL();
        if( tentry.iBioType )
            {
            infoDialog->ExecuteLD( infoData, CMsgInfoMessageInfoDialog::EBiosEditor );
            }
        else
            {
            infoDialog->ExecuteLD( infoData, CMsgInfoMessageInfoDialog::ESmsEditor );
            }

        CleanupStack::PopAndDestroy(toList);
        }

	
    // Cleanup 
    //
    if ( messageType )
        {
        CleanupStack::PopAndDestroy( messageType );
        }

    if ( priority )
        {
        CleanupStack::PopAndDestroy( priority );
        }

    if ( callback )
        {
        CleanupStack::PopAndDestroy( callback );
        }

    if ( subject )
        {
        CleanupStack::PopAndDestroy( subject );
        }

    if ( status)
        {
        CleanupStack::PopAndDestroy( status);
        }
        
    }

// ---------------------------------------------------------
// CSmsMtmUiCDMA::CheckAndEnsureSmsServiceOkL
//
// ---------------------------------------------------------
TInt CSmsMtmUiCDMA::CheckAndEnsureSmsServiceOkL( TBool aCreateService, TBool aCreateStore, TBool /*aFetchSMSCs*/ )
    {
    TInt recreation = ESmsReCreatedNone;
    TMsvId currContext = iBaseMtm.Entry().Entry().Id(); // take current context to safe
    TMsvId serviceEntryId = KMsvNullIndexEntryId;
    
    // Check first do we have service entry...
    TRAPD( err, serviceEntryId = ServiceIdL());
    if ( err == KErrNotFound && aCreateService )
        { // ...no, let's create one
        iNewSmsSettings = EFalse;
        CreateDefaultSettingsL( serviceEntryId );
        recreation = ESmsReCreatedService;
        iBaseMtm.SwitchCurrentEntryL( currContext );
        }
    else if ( err == KErrNone ) 
        {
        // If service ok, then check do we have msg store for it...
        iBaseMtm.SwitchCurrentEntryL( serviceEntryId );
        if ( !iBaseMtm.Entry().HasStoreL() && aCreateStore )
            { // ...no, let's create one
            iNewSmsSettings = EFalse;
            CreateDefaultSettingsL( serviceEntryId );
            recreation = recreation ? ESmsReCreatedBoth : ESmsReCreatedServicesStore;
            }
        iBaseMtm.SwitchCurrentEntryL( currContext );
        }

    return recreation;
    }

// ---------------------------------------------------------
// CSmsMtmUi::TurnRecipientsArrayIntoStringLC
//
// ---------------------------------------------------------
HBufC* CSmsMtmUiCDMA::TurnRecipientsArrayIntoStringLC( 
    const CArrayPtrFlat<CSmsNumber>& aRecip, 
    const TBool /*aEmailMessage*/ ) const
    {
    //calc length of string needed
    TInt stringLength = 0;
    TInt index;
    TInt count = aRecip.Count();

    // Do north american number grouping first, the length is no longer the same!
    for( index = 0; index < count; index++ )
        {
        stringLength += aRecip[index]->Name().Length();
        HBufC* groupedNumber = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( aRecip[index]->Address() );
        stringLength += groupedNumber->Length();
        delete groupedNumber;
        stringLength += KSmumAdditionalCharsStringLength; // for the 6 chars: '<' '>' ', ' 2 x KSmumLRMarker
        }
    //construct string
    HBufC* toList = HBufC::NewLC( stringLength );
    for( index=0; index < count; index++ )
        {
        TPtrC name = aRecip[index]->Name();
        TPtrC number = aRecip[index]->Address();
        TPtr toListPtr = toList->Des();
        if( name.Length())
            {
            toListPtr.Append( name );
            toListPtr.Append( KSmumLRMarker );
            toListPtr.Append( KSmumCharLeftAddressIterator );

            // Before we append a number, we have to number group it
            HBufC* groupedNumber = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( number );
            CleanupStack::PushL(groupedNumber);
            toListPtr.Append( groupedNumber->Des() );
            CleanupStack::PopAndDestroy(groupedNumber);

            toListPtr.Append( KSmumCharRightAddressIterator );
            toListPtr.Append( KSmumLRMarker );
            }
        else
            {
            HBufC* groupedNumber = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( number );
            CleanupStack::PushL(groupedNumber);
            toListPtr.Append( groupedNumber->Des() );
            CleanupStack::PopAndDestroy(groupedNumber);
            }
        
        if ( index < ( count - 1 )) // comma needed?
            {
            toListPtr.Append( KSmumCharCommaAndSpace );
            }
        }

    return toList;
    }       


// ---------------------------------------------------------
// CSmsMtmUiCDMA::StatusTextL
//
// ---------------------------------------------------------
HBufC* CSmsMtmUiCDMA::StatusTextL( const TMsvEntry& aContext ) const
    {
    TInt res = 0;
    TUint sendingState = aContext.SendingState();
    switch( sendingState )
        {
        case KMsvSendStateFailed:
            res = R_QTN_MCE_OUTBOX_STATUS_FAILED;
            break;

        case KMsvSendStateUponRequest: // Mapped to be the same
        case KMsvSendStateSuspended:
            res = R_QTN_MCE_OUTBOX_STATUS_SUSPENDED;
            break;
        
        case KMsvSendStateScheduled: // Mapped to be the same
        case KMsvSendStateWaiting:
            res = R_QTN_MCE_OUTBOX_STATUS_WAITING;
            break;
            
        case KMsvSendStateResend:
            res = R_QTN_MCE_OUTBOX_STATUS_RETRY_AT;
            break;
            
        case KMsvSendStateSending:
            res = R_QTN_MCE_OUTBOX_STATUS_SENDING;
            break;
        
        case KMsvSendStateSent:
            res = R_QTN_MCE_OUTBOX_STATUS_SENT;
            break;
        case KMsvSendStateNotApplicable:
            res = 0;
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }
    
        if (!res)
            {
            return HBufC::NewL(0);
            }
      
        if ( sendingState == KMsvSendStateResend )
            {
            TBuf<KTBuf80Length> bufTimeFormat;
            TBuf<KTBuf80Length> bufTimeFormatted;
            StringLoader::Load( 
                bufTimeFormat, R_QTN_TIME_USUAL_WITH_ZERO, iCoeEnv );
            aContext.iDate.FormatL( bufTimeFormatted, bufTimeFormat );
            return StringLoader::LoadL( 
                R_QTN_MCE_OUTBOX_STATUS_RETRY_AT, bufTimeFormatted, iCoeEnv );
            }
        else
            {
            return StringLoader::LoadL( res, iCoeEnv );
            }
    }

//  End of File