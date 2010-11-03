/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MsgIndicatorPlugin plugin implementation
*
*/



#include <bautils.h>  // BaflUtils
#include <eikdll.h>
#include <apgcli.h>             // for RApaLsSession

// INCLUDE FILES
#include <coemain.h> // CCoeEnv
#include <coeaui.h> // CCoeAppUi
#include <vwsdef.h> // TVwsViewId
#include <e32property.h> // RPropert
#include <msgindicator.rsg> // MsgIndicator resources
#include <StringLoader.h> // Stringloader
#include <AknUtils.h>
#include <msvids.h> // KMsvGlobalInBoxIndexEntryIdValue
#include "msgindicatorpluginimplementation.h"
#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR
#include <ConeResLoader.h> // RConeResourceLoader
#include <NcnListDomainCRKeys.h> // CenRepKeys
#include <centralrepository.h> //CRepository
#include <muiumsvuiserviceutilitiesinternal.h> // MsvUiServiceUtilitiesInternal
#include <messaginginternalpskeys.h>
#include "msgindicatorpluginlog.h" //Logging
#include <messaginginternalcrkeys.h>

#include <AknDlgShut.h>         // for AknDialogShutter
// CONSTANTS
_LIT( KIndicatorResourceFile,"msgindicator.rsc" );
const TUid KMceShowMail  = { 0x01 };
const TInt KMaxMessageNumberLength = 4; // 9999 = 4 numbers
const TUid KMCEAppUid = { 0x100058C5 };
const TUid KMCEMessageListViewUid = { 0x02 };
const TUid KMCEMainViewUid = { 0x01 }; 
const TInt KListgranularity = 22;
const TInt KTextLength = 150;
    	/* Maximum number of voice mails */
const TInt EVMMaximumNumber = 255;
const TUid KVmBxUid = { 0x100058F5 };
const TInt KRestartTimer = 100000 ;
#define KSpeeddialVmbxDialUidView TVwsViewId( TUid::Uid( 0x1000590A ),TUid::Uid( 0x02 ) )
_LIT8( KExternalLaunch, "outside" );

const TUid KConversationAppUid = { 0x2002A540 };
const TUid KConversationListViewUid = { 0x01 };
const TMsvId KConversationListViewid = 0x01;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::CMsgIndicatorPluginImplementation
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMsgIndicatorPluginImplementation::CMsgIndicatorPluginImplementation()
    {
    }

// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMsgIndicatorPluginImplementation::ConstructL()
    {
    MSGPLUGINLOGGER_CREATE
    
    //CRepository
    iStorage = CRepository::NewL( KCRUidNcnList );
    
	//Get the ALS status
	iIsALSSupported = UpdateALSStatus();
    
    //RVmbxNumber
    iSVmbxOpen = EFalse;
    
    // Read resource strings
    // Resource loader
    CEikonEnv* env = CEikonEnv::Static();
    RConeResourceLoader resourceLoader( *env );

    iObserver = CWaitingObserver::NewL( iMobilePhone, *this );

    TParse fp;
    fp.Set( KIndicatorResourceFile, &KDC_RESOURCE_FILES_DIR , NULL );
    TFileName filename = fp.FullName();

    resourceLoader.OpenL( filename ); 
    
    BaflUtils::NearestLanguageFile(env->FsSession(), filename);

    env->AddResourceFileL(filename); 
    
    iMessagesStrings = new(ELeave)CDesCArrayFlat( KListgranularity );
    iMessagesStrings->Reset();
    TBuf<KTextLength> tmpBuf;

    StringLoader::Load( tmpBuf, R_QTN_GEN_NEW_ITEM_IN_INBOX, env );
    iMessagesStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_QTN_GEN_NEW_ITEMS_IN_INBOX, env );
    iMessagesStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_QTN_GEN_NEW_ITEM_IN_OUTBOX, env );
    iMessagesStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_QTN_GEN_NEW_ITEMS_IN_OUTBOX, env );
    iMessagesStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_QTN_GEN_NEW_OTHER_MESSAGES, env );
    iMessagesStrings->AppendL( tmpBuf );

    StringLoader::Load( tmpBuf, R_QTN_GEN_NEW_EMAIL, env );
    iMessagesStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_QTN_GEN_VOICE_MAIL_INDICATOR, env );
    iMessagesStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_QTN_GEN_VOICE_MAILS_INDICATOR, env );
    iMessagesStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_QTN_GEN_UNK_AMOUNT_VOICE_MAILS_INDICATOR, env );
    iMessagesStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_QTN_GEN_VOICE_MAIL_ON_LINE1, env );
    iMessagesStrings->AppendL( tmpBuf );

    StringLoader::Load( tmpBuf, R_QTN_GEN_VOICE_MAIL_INDICATOR_ON_LINE1, env );
    iMessagesStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_QTN_GEN_VOICE_MAILS_INDICATOR_ON_LINE1, env ); 
    iMessagesStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_QTN_GEN_VOICE_MAIL_ON_LINE2, env );
    iMessagesStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_QTN_GEN_VOICE_MAIL_INDICATOR_ON_LINE2, env );
    iMessagesStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_QTN_GEN_VOICE_MAILS_INDICATOR_ON_LINE2, env ); 
    iMessagesStrings->AppendL( tmpBuf );


    StringLoader::Load( tmpBuf, R_QTN_GEN_NEW_FAX, env );
    iMessagesStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_QTN_GEN_NEW_FAXES, env );
    iMessagesStrings->AppendL( tmpBuf );
    StringLoader::Load( tmpBuf, R_QTN_GEN_UKN_AMOUNT_NEW_FAXES, env );
    iMessagesStrings->AppendL( tmpBuf );

    StringLoader::Load( tmpBuf, R_QTN_GEN_SIM_MSG_FULL, env );
    iMessagesStrings->AppendL( tmpBuf );

    iRepository = CRepository::NewL(KCRUidMuiuSettings);       
    resourceLoader.Close();
    }

// -----------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgIndicatorPluginImplementation* CMsgIndicatorPluginImplementation::NewL()
    {
    CMsgIndicatorPluginImplementation* self = 
        new( ELeave ) CMsgIndicatorPluginImplementation;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CMsgIndicatorPluginImplementation::~CMsgIndicatorPluginImplementation()
    {
    delete iMessagesStrings;
    delete iStorage;
    iVmbx.Close();
//    iObserver->Cancel();
	delete iObserver;			
	if(iRepository)		
		{
		delete iRepository;
		iRepository = NULL;
		}
    MSGPLUGINLOGGER_DELETE
    }


// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::HandleIndicatorTapL
// ---------------------------------------------------------------------------
//
void CMsgIndicatorPluginImplementation::HandleIndicatorTapL( const TInt aUid )
    {

    MSGPLUGINLOGGER_WRITE_FORMAT("HandleIndicatorTapL: Indicator: %d", aUid);
    

    const TVwsViewId KMessagingCentreView( KMCEAppUid, KMCEMessageListViewUid  );
    const TVwsViewId KMessagingCentreMainView( KMCEAppUid, KMCEMainViewUid  );
    const TVwsViewId KConversationView( KConversationAppUid, KConversationListViewUid  );

#ifdef OFFLINE_TESTING // See msgindicatorpluginlog.h
    TInt testaUid;
    if ( aUid == EAknIndicatorOutbox ) // Change outbox id to ....
        {
        testaUid = KOffLineId;
        }
    else
        {
        testaUid = aUid; // Other indicators should be same
        }
    switch ( testaUid )
#else    
    switch ( aUid )
#endif // OFFLINE_TESTING 
      {
            // SMS/MMS Envelope
        case EAknIndicatorEnvelope:
            {
            // Launch "Inbox view"
           TInt conversationview = 0;    
           if (  iRepository )
           	{   
           	iRepository->Get(KMuiuMceDefaultView,conversationview);        
         	}
           if (conversationview == 1 )
               {
               CCoeEnv::Static()->AppUi()->CreateActivateViewEventL( 
                                           KConversationView, TUid::Uid( KConversationListViewid ), 
                                            KNullDesC8 );                  
               }                                       
            else
                {
                CCoeEnv::Static()->AppUi()->CreateActivateViewEventL( 
                													KMessagingCentreView, TUid::Uid( KMsvGlobalInBoxIndexEntryIdValue ), 
                													KNullDesC8 );
                }
            }
        break;
            // Email
        case EAknIndicatorEmailMessage:
            // Launch "Email Inbox view" or mce mainview
            CCoeEnv::Static()->AppUi()->CreateActivateViewEventL( 
                KMessagingCentreMainView, KMceShowMail, 
                KNullDesC8 );
        break;
            // Outbox
        case EAknIndicatorOutbox:
            {
		  	TInt closedEntryId = 0;
			TInt r = RProperty::Get( KPSUidMuiu, KMceTemplatesDialog, closedEntryId );
			if ( r != KErrNone )
	            {
	            closedEntryId = 0;
	            }
	        
	        if ( closedEntryId )
	            {
		        TFindProcess findProc(_L("*[100058C5]*")); // by MCE UID3
			    TFullName result;

			    RProcess processHandle;
				while ( findProc.Next(result) == KErrNone) 
				    {
				    TInt err;
				    TRAP (err,processHandle.Open( findProc, EOwnerThread));
                    //Kernel opened the process successfully
					//Call Terminate on that process,to check the Cancel API.
					//Calling the processHandle.Terminate.
				    if ( err == KErrNone )
					    {	
				        processHandle.Terminate(KErrCancel);
		   			    processHandle.Close();
					    }
				    }
				// After termination of the process KMceTemplatesDialog key should be set to '0'   
			    r = RProperty::Set( KPSUidMuiu, KMceTemplatesDialog, 0 ); 
			    User::After(KRestartTimer);
	            }
	        // Launch "Outbox view"
            CCoeEnv::Static()->AppUi()->CreateActivateViewEventL( 
                 KMessagingCentreView, TUid::Uid( KMsvGlobalOutBoxIndexEntryIdValue ), 
                 KNullDesC8 );
            }
        break;
            // Other message
        case EAknIndicatorOtherMessage:
            // Hide popup. Closes the stylus activated popup in this case                
        break;
            // Voicemail General
        case EAknIndicatorVoiceMailWaiting:
            LaunchNewVoiceMailAppL ( EMsgIndicatorLine1 );
        break;
            // Voicemail Line1
        case EAknIndicatorVoiceMailWaitingOnLine1:
            LaunchNewVoiceMailAppL ( EMsgIndicatorLine1 );
        break;
            // Voicemail Line2
        case EAknIndicatorVoiceMailWaitingOnLine2:
            LaunchNewVoiceMailAppL( EMsgIndicatorLine2 );
        break;
            // Remote mail
        case EAknIndicatorRemoteMailboxEmailMessage:
            //None
        break;
            // Fax
        case EAknIndicatorFaxMessage:
        break;
            // Default        
        default:
        break;
        }
    }


void CMsgIndicatorPluginImplementation::LaunchNewVoiceMailAppL(TInt aType)
    {
    TUid uid = {1}; // als line 1 or no als (TVmbxNumberEntry::EAlsLine1Entry)
    if ( aType == EMsgIndicatorLine2 )
        {
        uid.iUid = 2; // (TVmbxNumberEntry::EAlsLine2Entry)
        }

    LaunchViewL( KSpeeddialVmbxDialUidView, uid, KExternalLaunch );
    }
    
void CMsgIndicatorPluginImplementation::LaunchViewL(const TVwsViewId& aViewId, TUid aCustomMessageId, 
     const TDesC8& aCustomMessage)
     {
     CCoeEnv::Static()->AppUi()->CreateActivateViewEventL(aViewId, aCustomMessageId, aCustomMessage);
     }


// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::HandlePreviewPopUpEventL
// Virtual function from MAknPreviewPopUpObserver, not used
// ---------------------------------------------------------------------------
//
void CMsgIndicatorPluginImplementation::HandlePreviewPopUpEventL(
    CAknPreviewPopUpController* /*aController*/,
    TPreviewPopUpEvent /*aEvent*/ )
    {
    }


// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::ProcessCommandL
// Virtual function from MEikCommandObserver, not used
// ---------------------------------------------------------------------------
//

void CMsgIndicatorPluginImplementation::ProcessCommandL( TInt /*aCommandId*/ )
    {
    }


// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::CreatePopupContentEnvelopeMessageL()
// ---------------------------------------------------------------------------
//
HBufC* CMsgIndicatorPluginImplementation::CreatePopupContentEnvelopeMessageL(TBool& aMsgCountZero)
    {
    
    // Get unread message count 
    // Create storage KNcnNewMessageCountInInbox
    // Construct proper popup message (singular or plural)            
    HBufC* dynStringBuf = NULL;        
    TInt messageCount = 0;

    if ( iStorage->Get( KNcnNewMessageCountInInbox, messageCount ) == KErrNone )
        {
        if ( messageCount > 1 ) 
            {
            dynStringBuf = HBufC::NewL( ( *iMessagesStrings )[ENewItemsInInbox].Length() + 
                                          KMaxMessageNumberLength ); 
            TPtr dynString = dynStringBuf->Des();
            StringLoader::Format( dynString,
                                  ( *iMessagesStrings )[ENewItemsInInbox], //"%N New messages"
                                  -1,             //No index code in source string
                                  messageCount );
            }
        else if ( messageCount == 1 )
            {
            dynStringBuf = ( *iMessagesStrings )[ENewItemInInbox].AllocL() ; //"1 New message"
            }
        else if ( messageCount == 0 )
            {
            dynStringBuf = ( *iMessagesStrings )[ESimMemoryFull].AllocL() ; //"Sim Messages Full"
            // Msg count is zero and indicator is blinking.
            aMsgCountZero = ETrue; 
            }
        }
    return dynStringBuf;        
    }

// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::CreatePopupContentFaxMessageL()
// ---------------------------------------------------------------------------
//
HBufC* CMsgIndicatorPluginImplementation::CreatePopupContentFaxMessageL()
    {
    // Get EIndicatorMessageTypeFax count 
    // Construct proper popup message (singular or plural)            
    HBufC* dynStringBuf = NULL;        
    TInt messageCount = iObserver->GetMsgCount(EIndicatorMessageTypeFax);

    if ( messageCount > 1 && messageCount < EVMMaximumNumber) 
        {
        dynStringBuf = HBufC::NewL( ( *iMessagesStrings )[ENewItemsInInbox].Length() + 
                                      KMaxMessageNumberLength ); 
        TPtr dynString = dynStringBuf->Des();
        StringLoader::Format( dynString,
                              ( *iMessagesStrings )[ENewFaxes], //"%N New Faxes"
                              -1,             //No index code in source string
                              messageCount );
        }
    else if ( messageCount == 1 )
        {
        dynStringBuf = ( *iMessagesStrings )[ENewFax].AllocL() ; //"1 New Fax"
        }
    else
        {
        dynStringBuf = ( *iMessagesStrings )[ENewUknFaxes].AllocL(); //"New Faxes"
        }      
    return dynStringBuf;            
    }

// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::CreatePopupContentOutboxMessageL()
// ---------------------------------------------------------------------------
//    
HBufC* CMsgIndicatorPluginImplementation::CreatePopupContentOutboxMessageL()
    {
    // Get message count 
    // Create storage KNcnMessageCountInOutbox
    // Construct proper popup message (singular or plural)            
    HBufC* dynStringBuf = NULL;        
    TInt messageCount = 0;

    if ( iStorage->Get( KNcnMessageCountInOutbox, messageCount ) == KErrNone )
        {
        if ( messageCount > 1 ) 
            {
            dynStringBuf = HBufC::NewL( ( *iMessagesStrings )[ENewItemsInOutbox].Length() + 
                                          KMaxMessageNumberLength ); 
            TPtr dynString = dynStringBuf->Des();
            StringLoader::Format( dynString,
                                  ( *iMessagesStrings )[ENewItemsInOutbox], //"%N msgs in outbox"
                                  -1,             //No index code in source string
                                  messageCount );
                              
            }
        else if ( messageCount == 1 )
            {
            dynStringBuf = ( *iMessagesStrings )[ENewItemInOutbox].AllocL(); //"1 message in outbox"
            }
        }
    return dynStringBuf;    
    }

// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::CreatePopupContentEmailMessageL()
// ---------------------------------------------------------------------------
//    
HBufC* CMsgIndicatorPluginImplementation::CreatePopupContentEmailMessageL()
    {
    // Construct proper popup message (singular)            
    HBufC* dynStringBuf = NULL;        
    dynStringBuf = ( *iMessagesStrings )[ENewEmail].AllocL(); //"New e-mail"
    return dynStringBuf;
    }

// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::CreatePopupContentOtherMessageL()
// ---------------------------------------------------------------------------
//    
HBufC* CMsgIndicatorPluginImplementation::CreatePopupContentOtherMessageL()
    {
    // Construct proper popup message (singular)            
    HBufC* dynStringBuf = NULL;        
    //"New messages available."
    dynStringBuf = ( *iMessagesStrings )[ENewOtherMessages].AllocL(); //"New msgs available"
    return dynStringBuf;
    }    
    
// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::CreatePopupContentVoiceMailGeneralL()
// ---------------------------------------------------------------------------
//    
HBufC* CMsgIndicatorPluginImplementation::CreatePopupContentVoiceMailGeneralL()
    {
    // Get message count EIndicatorMessageTypeVMLine1
    // Construct proper popup message (singular or plural)            
    HBufC* dynStringBuf = NULL;        
    TInt messageCount = iObserver->GetMsgCount(EIndicatorMessageTypeVMLine1);

    if ( messageCount > 1 && messageCount < EVMMaximumNumber ) 
        {
        dynStringBuf = HBufC::NewL( ( *iMessagesStrings )[ENewVoiceMails].Length() + 
                                      KMaxMessageNumberLength ); 
        TPtr dynString = dynStringBuf->Des();
        StringLoader::Format( dynString,
                              ( *iMessagesStrings )[ENewVoiceMails], //"%N new voice msgs"
                              -1,             //No index code in source string
                              messageCount );
        }
    else if ( messageCount == 1 )
        {
        dynStringBuf = ( *iMessagesStrings )[ENewVoiceMail].AllocL(); //"New Voice msg"
        }
    else
        {
        dynStringBuf = ( *iMessagesStrings )[ENewUnkVoiceMails].AllocL(); //"New voice msgs"
        }      
    return dynStringBuf;
    }

// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::CreatePopupContentVoiceMailLineOneL()
// ---------------------------------------------------------------------------
//    
HBufC* CMsgIndicatorPluginImplementation::CreatePopupContentVoiceMailLineOneL()
    {
    // Get message count EIndicatorMessageTypeVMLine1
    // Construct proper popup message          
    HBufC* dynStringBuf = NULL;        
    TInt messageCount = iObserver->GetMsgCount(EIndicatorMessageTypeVMLine1);

    if ( messageCount > 1 && messageCount < EVMMaximumNumber ) 
        {
        dynStringBuf = HBufC::NewL( ( *iMessagesStrings )[ENewVoiceMailsOnLine1].Length() + 
                                      KMaxMessageNumberLength ); 
        TPtr dynString = dynStringBuf->Des();
        StringLoader::Format( dynString,
                              ( *iMessagesStrings )[ENewVoiceMailsOnLine1], //"%N new msgs, line 1"
                              -1,             //No index code in source string
                              messageCount );
        }
    else if ( messageCount == 1 )
        {
        dynStringBuf = ( *iMessagesStrings )[ENewVoiceMailOnLine1].AllocL(); //"New msg, line 1"
        }
    else
        {
        dynStringBuf = ( *iMessagesStrings )[ENewUnkVoiceMailsOnLine1].AllocL(); //"New msgs, line 1"
        }      
    return dynStringBuf;
    }
    
// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::CreatePopupContentVoiceMailLineTwoL()
// ---------------------------------------------------------------------------
//    
HBufC* CMsgIndicatorPluginImplementation::CreatePopupContentVoiceMailLineTwoL()
    {
    // Get message count EIndicatorMessageTypeVMLine2
    // Construct proper popup message
    HBufC* dynStringBuf = NULL;        
    TInt messageCount = iObserver->GetMsgCount(EIndicatorMessageTypeVMLine2);

    if ( messageCount > 1 && messageCount < EVMMaximumNumber ) 
        {
        dynStringBuf = HBufC::NewL( ( *iMessagesStrings )[ENewVoiceMailsOnLine2].Length() + 
                                      KMaxMessageNumberLength ); 
        TPtr dynString = dynStringBuf->Des();
        StringLoader::Format( dynString,
                              ( *iMessagesStrings )[ENewVoiceMailsOnLine2], //"%N new msgs, line 2"
                              -1,             //No index code in source string
                              messageCount );
        }
    else if ( messageCount == 1 )
        {
        dynStringBuf = ( *iMessagesStrings )[ENewVoiceMailOnLine2].AllocL(); //"New msg, line 2"
        }
    else
        {
        dynStringBuf = ( *iMessagesStrings )[ENewUnkVoiceMailsOnLine2].AllocL(); //"New msgs, line 2"
        }      
    return dynStringBuf;
    }

// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::CheckVMBNumberAndCallL( TMsgLine aLine )
// ---------------------------------------------------------------------------
// 
void CMsgIndicatorPluginImplementation::CheckVMBNumberAndCallL( TMsgLine aLine )
    {
	
    TPhCltTelephoneNumber vmbxNumber;
    vmbxNumber.Zero();
	
    if ( !iSVmbxOpen )
        {
        if ( iVmbx.Open() == KErrNone )
            {
            iSVmbxOpen = ETrue;    
            }
        else
            {
            User::Leave( KLeaveWithoutAlert ); //No vmbx available, so nothing we can do
            }
        }
    if ( aLine == EMsgIndicatorGeneral )        
        {
        if ( ( iVmbx.GetVmbxNumber( vmbxNumber, EAlsActiveLineEntry ) == KErrNotFound ) )
            {
            // Launching the VmBx application - User need to call manually after entring VmBx number
            LaunchVmBxAppL() ; 
		    }
		else
            {
            CallL(vmbxNumber); // If VmBx number is already defined
            }
        }
    else if ( aLine == EMsgIndicatorLine1 )
        {
        if ( ( iVmbx.GetVmbxNumber( vmbxNumber, EAlsLine1Entry ) == KErrNotFound ) )
            {
            // Launching the VmBx application - User need to call manually after entring VmBx number
            LaunchVmBxAppL() ;
            }
        else
            {
            CallL(vmbxNumber); // If VmBx number is already defined
            }
        }
    else if ( aLine == EMsgIndicatorLine2 )
        {
        if ( ( iVmbx.GetVmbxNumber( vmbxNumber, EAlsLine2Entry ) == KErrNotFound ) )
            {
            // Launching the VmBx application - User need to call manually after entring VmBx number
            LaunchVmBxAppL() ;
            }
        else
            {
            CallL(vmbxNumber); // If VmBx number is already defined
            }
            }
        }
    
// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::LaunchVmBxApp () 
// Launches voice mailbox application
// ---------------------------------------------------------------------------
//    
void CMsgIndicatorPluginImplementation::LaunchVmBxAppL()
	{
	TInt err = KErrNone ;
	RApaLsSession appArcSession;
	User::LeaveIfError( appArcSession.Connect() );
	CleanupClosePushL( appArcSession );

	CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
	TApaAppInfo info;
	User::LeaveIfError( appArcSession.GetAppInfo( info, KVmBxUid ) );
#ifdef RD_APPS_TO_EXES
	cmdLine->SetExecutableNameL( info.iFullName );
#else
	cmdLine->SetLibraryNameL( info.iFullName );
#endif
	cmdLine->SetCommandL( EApaCommandOpen );
	err = appArcSession.StartApp( *cmdLine );
    User::LeaveIfError( err );
	CleanupStack::PopAndDestroy( cmdLine );
	CleanupStack::PopAndDestroy(); // appArcSession
    }

// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::CallL( TPhCltTelephoneNumber aTelNumber )
// ---------------------------------------------------------------------------
//    
void CMsgIndicatorPluginImplementation::CallL( TPhCltTelephoneNumber aTelNumber )
    {
    TPhCltNameBuffer recipientstring;
    recipientstring = KNullDesC; // No need to show voicemailbox name
    // ETrue = No query
    MsvUiServiceUtilitiesInternal::CallToSenderQueryL( aTelNumber, recipientstring, ETrue );
    }

// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::TextL( const TInt aUid, TInt& aTextType )
// ---------------------------------------------------------------------------
//    
HBufC* CMsgIndicatorPluginImplementation::TextL( const TInt aUid,
                                                 TInt& aTextType )
    {

    HBufC* textBuf = NULL;
    
#ifdef OFFLINE_TESTING // See msgindicatorpluginlog.h
    TInt testaUid;
    if ( aUid == EAknIndicatorOutbox ) // Change outbox id to ....
        {
        testaUid = KOffLineId;
        }
    else
        {
        testaUid = aUid; // Other indicators should be same
        }
    switch ( testaUid ) // OFFLINE TESTING
#else
    switch ( aUid )
#endif //OFFLINE_TESTING
        {
            // SMS/MMS Envelope
        case EAknIndicatorEnvelope:
            {
            TBool isMsgCountZero = EFalse; 
            textBuf = CreatePopupContentEnvelopeMessageL( isMsgCountZero );
            if ( isMsgCountZero ) // Sim Memory case
                {
                // If indicator is visible and msg count is zero 
                // we can't show link.
                aTextType = EAknIndicatorPluginInformationText;    
                }
            else
                {
                aTextType = EAknIndicatorPluginLinkText;
                }
            }
        break;
            // Email
        case EAknIndicatorEmailMessage:
            textBuf = CreatePopupContentEmailMessageL();
            aTextType = EAknIndicatorPluginLinkText;
        break;
            // Outbox
        case EAknIndicatorOutbox:
            textBuf = CreatePopupContentOutboxMessageL();
            aTextType = EAknIndicatorPluginLinkText;
        break;
            // Other message
        case EAknIndicatorOtherMessage:
            textBuf = CreatePopupContentOtherMessageL();
            aTextType = EAknIndicatorPluginInformationText;
        break;
            // Voicemail General
        case EAknIndicatorVoiceMailWaiting: 
            textBuf = CreatePopupContentVoiceMailGeneralL();
            aTextType = EAknIndicatorPluginLinkText;
        break;
            // Voicemail Line1
        case EAknIndicatorVoiceMailWaitingOnLine1:
            textBuf = CreatePopupContentVoiceMailLineOneL();
            aTextType = EAknIndicatorPluginLinkText;
        break;
            // Voicemail Line2
        case EAknIndicatorVoiceMailWaitingOnLine2:
            textBuf = CreatePopupContentVoiceMailLineTwoL();
            aTextType = EAknIndicatorPluginLinkText;
        break;
            // Remotemail
        case EAknIndicatorRemoteMailboxEmailMessage:
            aTextType = EAknIndicatorPluginInformationText;
        break;
            // Fax
        case EAknIndicatorFaxMessage:
            textBuf = CreatePopupContentFaxMessageL();
            aTextType = EAknIndicatorPluginInformationText;
        break;
            // Default        
        default:
        break;

        }
    return textBuf;
    }        

// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::IsALSSupported()
// ---------------------------------------------------------------------------
//    
TBool CMsgIndicatorPluginImplementation::IsALSSupported()
    {
    return iIsALSSupported;
    }

// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::UpdateALSStatus()
// Find out if alternative line subscription is active in the terminal
// ---------------------------------------------------------------------------
//    

TBool CMsgIndicatorPluginImplementation::UpdateALSStatus()
	{
	//Open the information settings
	RSSSettings settings;
	TInt status = settings.Open();

    if ( status == KErrNone )
        {
    	// Get the ALS information, 
    	// ALS is supported, if:
    	// 1. supported by the device (ALS PP enabled) 
    	// 2. CSP enabled and SIM supports alternative service line (line2)
    	TInt alsValue = KErrUnknown;
    	status = settings.Get( ESSSettingsAls, alsValue );

    	//Close the settings, they are no longer needed
    	settings.Close();

    	// Meaning of the ALS values
    	// ESSSettingsAlsNotSupported - ALS not supported, always primary line.
    	// ESSSettingsAlsPrimary - ALS supported, primary line selected.
    	// ESSSettingsAlsAlternate - ALS supported, alternate line selected.
    	
    	//Value read OK and ALS not supported
    	if( status == KErrNone && alsValue == ESSSettingsAlsNotSupported )
    		{
    		return EFalse;
    		}
    	//Value read OK and ALS is supported
    	else if( status == KErrNone && 
    			( alsValue == ESSSettingsAlsPrimary || alsValue == ESSSettingsAlsAlternate ) )
    		{
    		return ETrue;
    		}
    	//Default value. ALS not supported. Returned if ALS status can't be read properly!
    	else
    		{
    		return EFalse;		
    		}
        }   
    else 
        {
        return EFalse;
        }        		
	}


//  End of File
