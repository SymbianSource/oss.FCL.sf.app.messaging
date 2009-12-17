/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides UniEditor SMS Plugin methods.
*
*/




// INCLUDE FILES

// Symbian: 
#include <e32base.h>
#include <implementationproxy.h>
#include <ecom.h>
#include <txtetext.h>
#include <txtrich.h>

#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <gsmerror.h>                       // KErrGsmOfflineOpNotAllowed
#include <featmgr.h> 

#include <mtclreg.h>
#include <smsclnt.h>
#include <smscmds.h>
#include <smuthdr.h>
#include <csmsemailfields.h>
#include <csmsaccount.h>
#include <charconv.h>
#include <smut.h>
#include <smumutil.h>
#include <cmsvmimeheaders.h>
#include <badesca.h>
#include <e32cmn.h>
#include <muiumsvuiserviceutilitiesinternal.h> 
#include <gsmuset.h>

// S60
#include <mmsgenutils.h>
#include <MuiuOperationWait.h>
#include <vcard.h>
#include <SenduiMtmUids.h>
#include <MsgBioUids.h>
#include <MsgMimeTypes.h>
#include "UniSendingSettings.h"
#include "UniClientMtm.h"
#include "UniEditorUids.hrh"
#include "messagingvariant.hrh"
#include "UniSmsPlugin.h"
#include "UniSmsUtils.h"
#include "MsgAttachmentUtils.h" 
#include "UniMsvEntry.h"

// resources
#include <UniSmsPluginD.rsg>                   
#include <avkon.rsg>              

//LOGGING
#include "UniEditorLogging.h"

// CONSTANTS
const TImplementationProxy KImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY( KUidUniEditorSmsPlugin, CUniSmsPlugin::NewL)
	};

// Used to set msg in unparsed state
const TInt KSmsPluginBioMsgUnparsed = 0;

// Description length for sms messages
const TInt KSmsMessageEntryDescriptionAmountOfChars = 60;

//used for descriptor array containing recipients
const TInt KRecipientsArrayGranularity = 8;

// For address information separation (start)
const TUint KMsgSmsAddressStartChar         ('<'); 

// For address information separation (end)
const TUint KMsgSmsAddressEndChar           ('>');

const TUint KUniSmsStartParenthesis ('(');
const TUint KUniSmsEndParenthesis (')');

// String length for Service centre name
const TInt KUniSmsSCStringLength = 50;

const TUid KUidMsvSMSHeaderStream_COPY_FROM_SMUTHDR_CPP  = {0x10001834};

// Amount of to be converted chars during extracting description
const TInt KSmsEdExtrDescReplaceCharacterCount = 3;

// Unicode char for linefeed regocnised by basic phones
const TUint KSmsEdUnicodeLFSupportedByBasicPhones = 0x000A;
// ========== LOCAL FUNCTIONS ==================================================
GLDEF_C void Panic( TInt aCategory );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUniSmsPlugin* CUniSmsPlugin::NewL( TAny* aConstructionParameters )
    {
	TUniPluginParams* params = reinterpret_cast<TUniPluginParams*>( aConstructionParameters );
    CUniSmsPlugin* self = new ( ELeave ) CUniSmsPlugin( params->iSession, params->iUniMtm ); 
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CUniSmsPlugin::~CUniSmsPlugin()
    {
    delete iRichText;
    delete iParaFormatLayer;
    delete iCharFormatLayer;
    delete iSmsHeader;
    delete iSmsMtm;
    delete iMtmRegistry;
    delete iEmailOverSmsC;
    delete iRecipients;	
    }

// -----------------------------------------------------------------------------
// C++ default constructor
// -----------------------------------------------------------------------------
//
CUniSmsPlugin::CUniSmsPlugin( CMsvSession& aSession, CUniClientMtm& aUniMtm )
    : iSession( aSession ),
    iUniMtm( aUniMtm ),
    iBioMsg( EFalse ),
    iOfflineSupported( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CUniSmsPlugin::ConstructL()
    {
    UNILOGGER_ENTERFN("CUniSmsPlugin::ConstructL");
    iParaFormatLayer = CParaFormatLayer::NewL();
    iCharFormatLayer = CCharFormatLayer::NewL();
    iRichText = CRichText::NewL( iParaFormatLayer, iCharFormatLayer );
    iEmailOverSmsC = CSmsNumber::NewL();
	
    CMsvEntry& entry = SmsMtmL()->Entry();
    entry.SetEntryL( KMsvRootIndexEntryId );
    
    TSmsUtilities::ServiceIdL( entry, iSmsServiceId );
    
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdOfflineMode ) )
        {
        iOfflineSupported = ETrue;   
        }
    else
        {
        iOfflineSupported = EFalse;  
        }
    
    //Turkish SMS-PREQ2265 Specific
    if ( FeatureManager::FeatureSupported( KFeatureIdNltSupport ) )
        {
        iNLTFeatureSupport = ETrue;
        }
    else
        {
        iNLTFeatureSupport = EFalse;
        }
    
    FeatureManager::UnInitializeLib();
    
    UNILOGGER_LEAVEFN("CUniSmsPlugin::ConstructL");
    }

// -----------------------------------------------------------------------------
// LoadHeadersL
// -----------------------------------------------------------------------------
//
void CUniSmsPlugin::LoadHeadersL( CMsvStore* aStore )
    {
    UNILOGGER_ENTERFN("CUniSmsPlugin::LoadHeadersL");
    delete iSmsHeader;
    iSmsHeader = NULL;

    if ( aStore && aStore->HasBodyTextL() )
        {
    	aStore->RestoreBodyTextL( *iRichText );
        }

	iSmsHeader = CSmsHeader::NewL( CSmsPDU::ESmsSubmit, *iRichText );
	
	if ( aStore && aStore->IsPresentL( KUidMsvSMSHeaderStream_COPY_FROM_SMUTHDR_CPP ) )
	    {
	    iSmsHeader->RestoreL( *aStore );
	    }
	else
	    {
        CSmsSettings* settings = CSmsSettings::NewLC();
    	CSmsAccount* account = CSmsAccount::NewLC();
    	account->LoadSettingsL( *settings );
    	CleanupStack::PopAndDestroy( account );
        iSmsHeader->SetSmsSettingsL( *settings );
        CleanupStack::PopAndDestroy( settings );
	    }
		TSmsUserDataSettings smsSettings;		
		smsSettings.SetTextCompressed( EFalse );
		TRAP_IGNORE(iSmsHeader->Message().SetUserDataSettingsL( smsSettings ));
    UNILOGGER_LEAVEFN("CUniSmsPlugin::LoadHeadersL");
    }

// -----------------------------------------------------------------------------
// SaveHeadersL 
// -----------------------------------------------------------------------------
//
void CUniSmsPlugin::SaveHeadersL( CMsvStore& aStore )
    {
    if ( iSmsHeader )
        {
        iSmsHeader->StoreL( aStore );
        }
    }

// -----------------------------------------------------------------------------
// ConvertFromL
// -----------------------------------------------------------------------------
//
TMsvId CUniSmsPlugin::ConvertFromL( TMsvId aId )
    {
    return DoConvertFromL( aId, EFalse );
    }

// -----------------------------------------------------------------------------
// DoConvertFromL
// -----------------------------------------------------------------------------
//
TMsvId CUniSmsPlugin::DoConvertFromL( TMsvId aId, TBool aIsForward )
	{
    UNILOGGER_ENTERFN("CUniSmsPlugin::ConvertFromL");
    UNILOGGER_WRITE_TIMESTAMP("CUniSmsPlugin::ConvertFromL start");
    SmsMtmL()->SwitchCurrentEntryL( aId );
    SmsMtmL()->LoadMessageL();
    iUniMtm.SwitchCurrentEntryL( aId );
    iUniMtm.LoadMessageL();

    TPtrC name;
    TPtrC address;

	const CSmsEmailFields& emailFields = SmsMtmL( )->SmsHeader( ).EmailFields();

	if( emailFields.HasAddress( ) && !aIsForward )
		{ // If email address set -> copy them here
	    const MDesCArray& emailRecipients = emailFields.Addresses();
		for( TInt id = 0; id < emailRecipients.MdcaCount( ); id++ )
			{
	        NameAndAddress( emailRecipients.MdcaPoint( id ), name, address );
	        iUniMtm.AddAddresseeL( 
	            EMsvRecipientTo,
	            address,
	            name );
			}
		}

    // Copy non-email over sms addresses if needed
    const CMsvRecipientList& smsRecipients = SmsMtmL()->AddresseeList();

    while ( smsRecipients.Count() )
        { // Go thru all the recipients
		if( !emailFields.HasAddress( ) && !aIsForward )
			{ // and copy them only if email addresses did not exist
	        NameAndAddress( smsRecipients[ 0 ], name, address );
	        iUniMtm.AddAddresseeL( 
	            EMsvRecipientTo,
	            address,
	            name );
			}
        SmsMtmL()->RemoveAddressee( 0 );
        }
    
    if( emailFields.Subject( ).Length( ) )
    	{ // If email subject exists -> copy it
    	iUniMtm.SetSubjectL( emailFields.Subject( ) );
    	}
    
    //Get sms entry 
    TMsvEntry smsTEntry = SmsMtmL()->Entry().Entry();
    
    if (  smsTEntry.iBioType == KMsgBioUidVCard.iUid
        || smsTEntry.iBioType == KMsgBioUidVCalendar.iUid )
        { 
        //We must check here if there is two attachments. This happens in case
        //if sending fails and message opened from drafts
        CMsvStore* store = iUniMtm.Entry().EditStoreL();
        CleanupStack::PushL( store );
        MMsvAttachmentManagerSync& managerSync = store->AttachmentManagerExtensionsL();        
        MMsvAttachmentManager& manager = store->AttachmentManagerL();
        if ( manager.AttachmentCount() == 2)
        	{
            managerSync.RemoveAttachmentL( 1 ); 	
        	}
        store->CommitL();  	
        CleanupStack::PopAndDestroy( store );
        }     
    else   //plain text
        {
        TInt totalLength( SmsMtmL()->Body().DocumentLength() );
        if ( totalLength > 0 )
            {
            HBufC* bodyText = HBufC::NewLC ( totalLength ); 
            TPtr bodyTextPtr ( bodyText->Des() ); 
            
            SmsMtmL()->Body().Extract( bodyTextPtr, 0, totalLength );
            
            iUniMtm.Body().InsertL( 0, bodyTextPtr );
                        
            CleanupStack::PopAndDestroy( bodyText );            
            }
        }
    
    SmsMtmL()->Body().Reset();
    
    SmsMtmL()->SaveMessageL();
    TMsvEntry uniTEntry = iUniMtm.Entry().Entry();
   
    iUniMtm.SaveMessageL();
    
    // Lets convert the bits to Uni mode
	TUniMsvEntry::SetForwardedMessage( uniTEntry, aIsForward );
	
    uniTEntry.iMtm.iUid = KUidUniMtm;
    iUniMtm.Entry().ChangeL( uniTEntry );
    UNILOGGER_LEAVEFN("CUniSmsPlugin::ConvertFromL");    
    UNILOGGER_WRITE_TIMESTAMP("CUniSmsPlugin::ConvertFromL end");
    return aId;
    }

// -----------------------------------------------------------------------------
// ConvertToL
// -----------------------------------------------------------------------------
//
TMsvId CUniSmsPlugin::ConvertToL( TMsvId aId )
    {    
    UNILOGGER_ENTERFN("CUniSmsPlugin::ConvertToL");
    SmsMtmL()->SwitchCurrentEntryL( aId );
    SmsMtmL()->LoadMessageL();
    iUniMtm.SwitchCurrentEntryL( aId );
    iUniMtm.LoadMessageL();
 
    const CMsvRecipientList& uniRecipients = iUniMtm.AddresseeList();
    
    //Save for email over sms  
    if ( iRecipients )
    	{
    	delete iRecipients;
    	iRecipients = NULL;
    	}
    iRecipients = new ( ELeave ) CDesCArrayFlat( KRecipientsArrayGranularity );   
         
    CSmsEmailFields *emailFields = CSmsEmailFields::NewL();
    CleanupStack::PushL( emailFields );
   
    while ( uniRecipients.Count() )
        {
        if ( IsEmailAddress( TMmsGenUtils::PureAddress( uniRecipients[ 0 ] )))
            {
            emailFields->AddAddressL( TMmsGenUtils::PureAddress( uniRecipients[ 0 ] ) );
            }
        else
            {
            SmsMtmL()->AddAddresseeL( 
                TMmsGenUtils::PureAddress( uniRecipients[ 0 ] ),
                TMmsGenUtils::Alias( uniRecipients[ 0 ] ) );    
            }

        iRecipients->AppendL( uniRecipients[ 0 ]  );
        iUniMtm.RemoveAddressee( 0 );
        }   
    if ( iUniMtm.SubjectL().Length() )
        {
        emailFields->SetSubjectL( iUniMtm.SubjectL() );
        }
    
    SmsMtmL()->SmsHeader().SetEmailFieldsL( *emailFields );
    CleanupStack::PopAndDestroy( emailFields );        
    
    CMsvStore* store = iUniMtm.Entry().EditStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& manager = store->AttachmentManagerL();
    MMsvAttachmentManagerSync& managerSync = store->AttachmentManagerExtensionsL();
    TUid bioType =
        {
        0 
        };
   
    CMsvAttachment* atta ( NULL );
    switch ( manager.AttachmentCount() )
        {
        case 0:
            {
            //In case of empty message.
            //There is not even smil.
            SmsMtmL()->Body().Reset();
            break;
            }
        case 1:  
        	{
        	atta = manager.GetAttachmentInfoL( 0 );
        	 if ( atta->MimeType() == KMsgMimeSmil )
                {                        
                managerSync.RemoveAttachmentL( 0 ); 
                delete atta;
                atta= NULL;
                }
        	 else
        	     CleanupStack::PushL(atta);
        	break;
        	}
        case 2:  //There is usually a SMIL also.
            {
            //In these case branch we assume that there is only one text atta
            //or one text atta and a SMIL
            //Everything else is a programming error in the caller

            CMsvAttachment* tempAtta1 = manager.GetAttachmentInfoL( 0 );
            CleanupStack::PushL(tempAtta1);
            CMsvAttachment* tempAtta2 = manager.GetAttachmentInfoL( 1 );
            CleanupStack::PushL(tempAtta2);
                   
            if ( tempAtta1->MimeType() == KMsgMimeSmil )
                {
                CleanupStack::Pop( tempAtta2 );
                CleanupStack::PopAndDestroy( tempAtta1 );
                CleanupStack::PushL( tempAtta2 );
                     
                managerSync.RemoveAttachmentL( 0 ); 
                atta=tempAtta2;
                }
            else if ( tempAtta2->MimeType() == KMsgMimeSmil )    
                {
                CleanupStack::PopAndDestroy( tempAtta2 );
                managerSync.RemoveAttachmentL( 1 ); 
                atta=tempAtta1;
                }
            else
                { 
                //Programming error in caller code
#ifdef _DEBUG 
                UNILOGGER_WRITEF(_L("PANIC: Two attas but no smil"));
                Panic ( EIllegalArguments );     
#endif 
                User::Leave( KErrArgument );
                
                }
            break;
            }
        default:
            {
            //Programming error in caller code
#ifdef _DEBUG 
            UNILOGGER_WRITEF(_L("PANIC: Two many attas"));
            Panic ( EIllegalArguments );
#endif            
            User::Leave( KErrArgument );
            break;
            }
        }

    if ( atta )
        {  	
        UNILOGGER_WRITEF8(_L8("Mime Type: %S"),&(atta->MimeType()));
        UNILOGGER_WRITEF(_L("Atta Name: %S"),&(atta->AttachmentName()));

        RFile file = manager.GetAttachmentFileL( 0 );
        CleanupClosePushL( file );
                   
        //Note that the attachments is not removed in case of smart messaging
        if ( atta->MimeType().CompareF( KMsgMimeTextPlain ) == 0 )
            {
            iBioMsg=EFalse;
            CreatePlainTextSMSL( file );
            managerSync.RemoveAttachmentL( 0 ); 
            }
        else if ( atta->MimeType().CompareF(KMsgMimeVCard) == 0 )
            {
            iBioMsg=ETrue;
            CreateVCardSMSL( file );
            bioType = KMsgBioUidVCard;
            SmsMtmL()->BioTypeChangedL( bioType );
            //Do not remove vCard atta here. 
            }
        else if ( atta->MimeType().CompareF(KMsgMimeVCal ) == 0 ||
                  atta->MimeType().CompareF(KMsgMimeICal ) == 0 )
            {
            iBioMsg=ETrue;
            CreateVCalSMSL( file );
            bioType = KMsgBioUidVCalendar;
            SmsMtmL()->BioTypeChangedL( bioType );
            //Do not remove vCal atta here. 
            }
        else
            {
            User::Leave( KErrArgument );
            } 
        CleanupStack::PopAndDestroy( &file );
        CleanupStack::PopAndDestroy( atta );
        }
        	
    TMsvEntry tEntry = SmsMtmL()->Entry().Entry();
    tEntry.SetAttachment( EFalse ); 
    tEntry.iMtm = KSenduiMtmSmsUid;
    tEntry.iType = KUidMsvMessageEntry;
    tEntry.iRelatedId = iSmsServiceId;
    tEntry.iServiceId = KMsvLocalServiceIndexEntryId;
    tEntry.iDate.UniversalTime();
    tEntry.SetInPreparation( ETrue );
    tEntry.SetVisible( EFalse );

    CSmsSettings* sendOptions = CSmsSettings::NewL();
    CleanupStack::PushL( sendOptions );
    
    // "ConvertToL" might be called right after constructor.
    // In this case iSmsHeader is still NULL. Need to initialise.
    if ( !iSmsHeader )
        {
        LoadHeadersL( store );
        }
  	iSmsHeader->GetSmsSettingsL( *sendOptions );
	
    sendOptions->CopyL( *sendOptions );
    
	if ( !iBioMsg )
	    {
	    if( iUnicodeMode )
		    {
		    sendOptions->SetCharacterSet( TSmsDataCodingScheme::ESmsAlphabetUCS2 );
		    }
	    else
		    {
		    sendOptions->SetCharacterSet( TSmsDataCodingScheme::ESmsAlphabet7Bit );
		    }
	    }
    else
        {
        // make sure bio messages have no conversion
        tEntry.iBioType = bioType.iUid;
        sendOptions->SetMessageConversion( ESmsConvPIDNone );
        sendOptions->SetCharacterSet( TSmsDataCodingScheme::ESmsAlphabet8Bit );
        }
    
    // Update some global SMS settings affecting all messages.
    // These might have changed from the ones retrieved when
    // the message was created and so needs to be updated.
    CSmsSettings* defaultSettings = CSmsSettings::NewLC();
	
	CSmsAccount* account = CSmsAccount::NewLC();
	account->LoadSettingsL( *defaultSettings );
	CleanupStack::PopAndDestroy( account );
    
    sendOptions->SetDeliveryReport( defaultSettings->DeliveryReport() );
    sendOptions->SetSmsBearer( defaultSettings->SmsBearer() );
    sendOptions->SetValidityPeriod( defaultSettings->ValidityPeriod() );
    sendOptions->SetReplyPath( defaultSettings->ReplyPath() );
    
    CleanupStack::PopAndDestroy( defaultSettings );
    
    iSmsHeader->SetSmsSettingsL( *sendOptions );

	// Move all the stuff from iSmsHeader::SmsSettings to SmsMtm::SmsHeader::SmsSettings
	SmsMtmL()->SmsHeader( ).SetSmsSettingsL( *sendOptions );
	SmsMtmL()->SmsHeader( ).Message( ).
		SetServiceCenterAddressL( iSmsHeader->Message( ).ServiceCenterAddress( ) );

    if(iNLTFeatureSupport)
        {
        //Turkish SMS-PREQ2265 Specific
        TSmsEncoding currAlternateEncoding = iSmsHeader->Message().Alternative7bitEncoding();
        SmsMtmL()->SmsHeader().Message().SetAlternative7bitEncoding(currAlternateEncoding);       
        }
	
    CleanupStack::PopAndDestroy( sendOptions ); 

    SmsMtmL()->Entry().ChangeL( tEntry );

    store->CommitL();        
    CleanupStack::PopAndDestroy( store );
    
    iUniMtm.SaveMessageL();
    SmsMtmL()->SaveMessageL();
    UNILOGGER_LEAVEFN("CUniSmsPlugin::ConvertToL");
    return aId;
    }

// -----------------------------------------------------------------------------
// CreateReplyL
// -----------------------------------------------------------------------------
//
TMsvId CUniSmsPlugin::CreateReplyL( TMsvId aSrc, TMsvId aDest, TMsvPartList aParts )
    {
    return DoCreateReplyOrForwardL( ETrue, aSrc, aDest, aParts );
    }

// -----------------------------------------------------------------------------
// CreateForwardL
// -----------------------------------------------------------------------------
//
TMsvId CUniSmsPlugin::CreateForwardL( TMsvId aSrc, TMsvId aDest, TMsvPartList aParts )
    {
    return DoCreateReplyOrForwardL( EFalse, aSrc, aDest, aParts );
    }
        
// -----------------------------------------------------------------------------
// SendL
// -----------------------------------------------------------------------------
//
void CUniSmsPlugin::SendL( TMsvId aId )
    {
    UNILOGGER_ENTERFN("CUniSmsPlugin::SendL");
    SmsMtmL()->SwitchCurrentEntryL( aId );
    SmsMtmL()->LoadMessageL();
    MoveMessagesToOutboxL();
    UNILOGGER_LEAVEFN("CUniSmsPlugin::SendL");
    }

// -----------------------------------------------------------------------------
// ValidateServiceL
// -----------------------------------------------------------------------------
//
TBool CUniSmsPlugin::ValidateServiceL( TBool aEmailOverSms )
    {
    TBool valid = ValidateSCNumberL();
    
    if ( aEmailOverSms )
        { 
        valid = ValidateSCNumberForEmailOverSmsL();
        }
    
    return valid;
    }
        
// -----------------------------------------------------------------------------
// GetSendingSettingsL
// -----------------------------------------------------------------------------
//
void CUniSmsPlugin::GetSendingSettingsL( TUniSendingSettings& aSettings )
    {
    // Modify only the settings this mtm plugin supports
    CSmsSettings* smsSettings = CSmsSettings::NewLC();
	iSmsHeader->GetSmsSettingsL( *smsSettings );

    switch ( smsSettings->ValidityPeriod().Int() )
        {
        case ESmsVPHour:
            aSettings.iValidityPeriod = TUniSendingSettings::EUniValidityPeriod1h;
            break;
        case ESmsVPSixHours:
            aSettings.iValidityPeriod = TUniSendingSettings::EUniValidityPeriod6h;
            break;
        case (3 * (TInt) ESmsVP24Hours):
            aSettings.iValidityPeriod = TUniSendingSettings::EUniValidityPeriod3Days;
            break;
        case ESmsVPWeek:
            aSettings.iValidityPeriod = TUniSendingSettings::EUniValidityPeriodWeek;
            break;
        case ESmsVPMaximum:
            aSettings.iValidityPeriod = TUniSendingSettings::EUniValidityPeriodMax;
            break;
        default: // default to 24h
        case ESmsVP24Hours:
            aSettings.iValidityPeriod = TUniSendingSettings::EUniValidityPeriod24h;
            break;
        }

	switch( smsSettings->MessageConversion( ) )
		{
		case ESmsConvFax:
			aSettings.iMessageType = TUniSendingSettings::EUniMessageTypeFax;
			break;
		case ESmsConvPaging:
			aSettings.iMessageType = TUniSendingSettings::EUniMessageTypePaging;
			break;
		default: // any other is text
			aSettings.iMessageType = TUniSendingSettings::EUniMessageTypeText;
			break;
		}

    aSettings.iDeliveryReport = smsSettings->DeliveryReport();
    aSettings.iReplyViaSameCentre = smsSettings->ReplyPath();
    CleanupStack::PopAndDestroy( smsSettings );
    }
    



// -----------------------------------------------------------------------------
// SetSendingSettingsL
// -----------------------------------------------------------------------------
//
void CUniSmsPlugin::SetSendingSettingsL( TUniSendingSettings& aSettings )
    {
    CSmsSettings* smsSettings = CSmsSettings::NewLC();
	iSmsHeader->GetSmsSettingsL( *smsSettings );

    TTimeIntervalMinutes validityPeriod = smsSettings->ValidityPeriod().Int();
    switch ( aSettings.iValidityPeriod )
        {
        case TUniSendingSettings::EUniValidityPeriod1h:
            validityPeriod = ( TInt ) ESmsVPHour;
            break;
        case TUniSendingSettings::EUniValidityPeriod6h:
            validityPeriod = ( TInt ) ESmsVPSixHours;
            break;
        case TUniSendingSettings::EUniValidityPeriod3Days:   
            validityPeriod = ( 3 * ( TInt ) ESmsVP24Hours );// Instead of modifying smutset.h
            break;
        case TUniSendingSettings::EUniValidityPeriodWeek:
            validityPeriod = ( TInt ) ESmsVPWeek;
            break;
        case TUniSendingSettings::EUniValidityPeriodMax:
            validityPeriod = ( TInt ) ESmsVPMaximum;
            break;
        default: // default to 24h
        case TUniSendingSettings::EUniValidityPeriod24h:
            validityPeriod = ( TInt ) ESmsVP24Hours;
            break;
        }
	switch( aSettings.iMessageType )
		{
		case TUniSendingSettings::EUniMessageTypeFax:
			smsSettings->SetMessageConversion( ESmsConvFax );
			break;
		case TUniSendingSettings::EUniMessageTypePaging:
			smsSettings->SetMessageConversion( ESmsConvPaging );
			break;
		default: // any other is text
			smsSettings->SetMessageConversion( ESmsConvPIDNone );
			break;
		}

    smsSettings->SetValidityPeriod( validityPeriod );
    smsSettings->SetDeliveryReport( aSettings.iDeliveryReport );
    smsSettings->SetReplyPath( aSettings.iReplyViaSameCentre );
    
	iSmsHeader->SetSmsSettingsL( *smsSettings );
    CleanupStack::PopAndDestroy( smsSettings );
    }

// -----------------------------------------------------------------------------
// IsServiceValidL
// -----------------------------------------------------------------------------
//
TBool CUniSmsPlugin::IsServiceValidL()
    {
    // Not implemented.
    return ETrue;
    }


// -----------------------------------------------------------------------------
// SmsMtmL
// -----------------------------------------------------------------------------
//
CSmsClientMtm* CUniSmsPlugin::SmsMtmL()
    {
    if ( !iSmsMtm )
        {        
        if ( !iMtmRegistry )
            {            
            iMtmRegistry = CClientMtmRegistry::NewL( iSession );
            }
        iSmsMtm = static_cast<CSmsClientMtm*>( iMtmRegistry->NewMtmL( KSenduiMtmSmsUid ) );
        }
    return iSmsMtm; 
    }
    
// -----------------------------------------------------------------------------
// DoCreateReplyOrForwardL
// -----------------------------------------------------------------------------
//
TMsvId CUniSmsPlugin::DoCreateReplyOrForwardL( 
    TBool aReply, 
    TMsvId aSrc, 
    TMsvId aDest, 
    TMsvPartList aParts )
    {
    SmsMtmL()->SwitchCurrentEntryL( aSrc );
    
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    
    CMsvOperation* oper = aReply
        ? SmsMtmL()->ReplyL( aDest, aParts, wait->iStatus )
        : SmsMtmL()->ForwardL( aDest, aParts, wait->iStatus );
        
    CleanupStack::PushL( oper );
    wait->Start();

    TMsvId newId;
    TPckgC<TMsvId> paramPack( newId );
    const TDesC8& progress = oper->FinalProgress();
    paramPack.Set( progress );
    newId = paramPack();
    CleanupStack::PopAndDestroy( oper );
    CleanupStack::PopAndDestroy( wait );
    
    return DoConvertFromL( newId, !aReply );   
    }

// ----------------------------------------------------------------------------
// MoveMessagesToOutboxL
// ----------------------------------------------------------------------------
void CUniSmsPlugin::MoveMessagesToOutboxL()
    {
    if ( !iRecipients || !iRecipients->Count() )
        {
        User::Leave( KErrGeneral );
        }

    //we must create an entry selection for message copies
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );

    CMsvEntry& entry = SmsMtmL()->Entry();
    TMsvEntry msvEntry( entry.Entry() );
    
    if ( iOfflineSupported && MsvUiServiceUtilitiesInternal::IsPhoneOfflineL() )  
    	{
    	msvEntry.SetSendingState( KMsvSendStateSuspended );
        msvEntry.iError = KErrGsmOfflineOpNotAllowed;
    	}	
    else
    	{
    	msvEntry.SetSendingState( KMsvSendStateWaiting );
    	}	
     
    CSmsHeader& header = SmsMtmL()->SmsHeader();  
    TBuf<KSmsMessageEntryDescriptionAmountOfChars> buf;
    
    if (!iBioMsg  )
        {
        ExtractDescriptionFromMessageL( 
            header.Message(), 
            buf, 
            KSmsMessageEntryDescriptionAmountOfChars );
        msvEntry.iDescription.Set( buf );
        } 
    
    CSmsNumber* rcpt = CSmsNumber::NewL();
    CleanupStack::PushL( rcpt );

    TPtrC name;
    TPtrC address;
    TBool cantExit = ETrue; 
    while ( cantExit )
        {
        HBufC* addressBuf = NULL;
        TPtr addressPtr( 0, 0);
        
        NameAndAddress( iRecipients->MdcaPoint(0) , name, address );
        UNILOGGER_WRITEF( _L("iRecipients: %S"),   &iRecipients->MdcaPoint(0) );       
 
        // set To-field stuff into the Details of the entry
        if ( name.Length() )
            {
            msvEntry.iDetails.Set( name );
            }
        else
            {
            // Internal data structures always holds the address data in western format.
            // UI is responsible of doing language specific conversions.    
            addressBuf = HBufC::NewLC( address.Length() );
            addressPtr.Set( addressBuf->Des() );
            addressPtr.Copy( address );

            AknTextUtils::ConvertDigitsTo( addressPtr, EDigitTypeWestern );      
            msvEntry.iDetails.Set( addressPtr );
            }
        UNILOGGER_WRITEF( _L("TMsvEntry::iDetails: %S"),   &msvEntry.iDetails );
        UNILOGGER_WRITEF( _L("TMsvEntry::iDescription: %S"),  &msvEntry.iDescription ); 
        TMsvId copyId;
       
        if ( iRecipients->Count() == 1 )
            {
            //Note that we came here also in case of many recipients. ...eventually.
            
            if ( IsEmailAddress( address ) )
                {
                FillEmailInformationDataL( header, address );              
                //Let's remove the recipient and replace it with Email over SMS gateway address
                //But let's first cehck if it exists
                if( SmsMtmL( )->AddresseeList().Count() )
                	{
                    SmsMtmL( )->RemoveAddressee( 0 );	
                	}
                SmsMtmL()->AddAddresseeL( 
	            iEmailOverSmsC->Address( ) ,
		        KNullDesC( ) );
                }
            else
                {                           
                InsertSubjectL( header, SmsMtmL()->Body() );                                 
                }

            entry.ChangeL( msvEntry );
            SmsMtmL()->SaveMessageL();
            // Move it
            copyId = MoveMessageEntryL( KMsvGlobalOutBoxIndexEntryId );
            cantExit = EFalse;
            }
        else 
            {// Many recipients in array
            
            // Own copy function for Emails
            // This is because EmailOverSms messages can 
            // contain adresses longer than 21 digits
            if ( IsEmailAddress( address ) )
                {
                copyId = CreateMessageInOutboxL(
                msvEntry, address );

                }
            else // For MSISDN's
                {
                rcpt->SetAddressL( address );
                if ( name.Length() )
                    { // add name only if we have alias
                    rcpt->SetNameL( name );
                    }
                
                copyId = CreateMessageInOutboxL(
                    msvEntry, *rcpt, SmsMtmL()->Body());
 
                SmsMtmL()->RemoveAddressee( 0 );
                }
            //If hundreds of recipient, make sure viewserver
            //timers are reseted
            if ( iRecipients->Count() > 100 && ( iRecipients->Count() ) % 30 == 0 )
                {
                User::ResetInactivityTime();
                }
                
            iRecipients->Delete(0);   
            }
        if ( addressBuf )
            {
            CleanupStack::PopAndDestroy( addressBuf );
            }
        
        // let's add the entry id into the cmsventryselection
        selection->AppendL( copyId );
        }
    CleanupStack::PopAndDestroy( rcpt );    

#ifdef USE_LOGGER 
    for ( TInt i=0; i<selection->Count(); i++ ) 
        {
        SmsMtmL()->SwitchCurrentEntryL( selection->At(i) );
        SmsMtmL()->LoadMessageL();
        const CMsvRecipientList& testRecipients = SmsMtmL()->AddresseeList(); 
        UNILOGGER_WRITEF( _L("Recipient: %S"), &(testRecipients[ 0 ]) ); 
        UNILOGGER_WRITEF( _L("Body: %S"), &(SmsMtmL()->Body().Read( 0,80 )  ));   
        UNILOGGER_WRITEF( _L("EmailOverSMS address: %S"), &iEmailOverSmsC->Address( ) );
    	UNILOGGER_WRITEF( _L("EmailOverSMS name : %S"),   &iEmailOverSmsC->Name( ) );
     
        CSmsHeader& testHeader = SmsMtmL()->SmsHeader();
        const CSmsEmailFields& testEmailFields = testHeader.EmailFields();
        
        if ( testEmailFields.Addresses().MdcaCount() )
            {
        	UNILOGGER_WRITEF( _L("CSmsEmailFields::iAddresses: %S"), &(testEmailFields.Addresses().MdcaPoint(0) ));
            }
        else
            {
            UNILOGGER_WRITEF(_L("CSmsEmailFields::iAddresses is empty"));	
            }    
        
        
        UNILOGGER_WRITEF( _L("CSmsEmailFields::iSubject : %S"),   &testEmailFields.Subject() );
        
        }  
#endif    
    
    //Let's free some memory
    if ( iRecipients )
        { 
        delete iRecipients;	
        iRecipients = NULL; 
        }
    
    SetScheduledSendingStateL( selection );
    CleanupStack::PopAndDestroy( selection ); 
    }


// ----------------------------------------------------------------------------
// MoveMessageEntryL
// ----------------------------------------------------------------------------
TMsvId CUniSmsPlugin::MoveMessageEntryL( TMsvId aTarget )
    {
    TMsvEntry msvEntry( SmsMtmL()->Entry().Entry() );
    TMsvId id = msvEntry.Id();
    
    if ( msvEntry.Parent() != aTarget )
        {
        TMsvSelectionOrdering sort;
        sort.SetShowInvisibleEntries( ETrue );
        CMsvEntry* parentEntry= CMsvEntry::NewL( iSession, msvEntry.Parent(), sort );
        CleanupStack::PushL( parentEntry );
        
        // Copy original from the parent to the new location
        CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();

        CMsvOperation* op = parentEntry->MoveL(
            msvEntry.Id(), 
            aTarget, 
            wait->iStatus );
        
        CleanupStack::PushL( op );
        wait->Start();
        TMsvLocalOperationProgress prog = McliUtils::GetLocalProgressL( *op );
        User::LeaveIfError( prog.iError );

        id = prog.iId;
        
        CleanupStack::PopAndDestroy( op );
        CleanupStack::PopAndDestroy( wait );
        CleanupStack::PopAndDestroy( parentEntry );
        }
    
    return id;
    }

// ----------------------------------------------------------------------------
// CreateMessageInOutboxL
// Use this function for non-email messages
// ----------------------------------------------------------------------------
TMsvId CUniSmsPlugin::CreateMessageInOutboxL(
    const TMsvEntry& aEntry, 
    const CSmsNumber& aRecipient, 
    const CRichText& aBody )
    {
    // Initialize the richtext object
    CRichText* richText = CRichText::NewL( iParaFormatLayer, iCharFormatLayer );
    CleanupStack::PushL( richText );
    
    // Initialise header and store
    CSmsHeader* header = CSmsHeader::NewL( CSmsPDU::ESmsSubmit, *richText );
    CleanupStack::PushL( header );
    CMsvStore* sourceStore = SmsMtmL()->Entry().ReadStoreL();
    CleanupStack::PushL( sourceStore );
    
    // Read store
    header->RestoreL( *sourceStore );

    // Initialise number with parameters
    CSmsNumber* rcpt = CSmsNumber::NewL( aRecipient );
    CleanupStack::PushL( rcpt );
    header->Recipients().ResetAndDestroy();
    header->Recipients().AppendL( rcpt );
    CleanupStack::Pop( rcpt );  
    
    // Create entry to Outbox
    TMsvEntry entry( aEntry );
    entry.iMtmData3 = KSmsPluginBioMsgUnparsed;
    CMsvEntry* outbox = iSession.GetEntryL( KMsvGlobalOutBoxIndexEntryId );
    CleanupStack::PushL( outbox );
    outbox->CreateL( entry );
    iSession.CleanupEntryPushL( entry.Id() );
    outbox->SetEntryL( entry.Id());
    
    //Initialize target store
    CMsvStore* targetStore;
    targetStore = outbox->EditStoreL();
    CleanupStack::PushL( targetStore );
    
    //Add attachment  
    MMsvAttachmentManager& attaManager = sourceStore->AttachmentManagerL(); 
    RFile tmpFile;
     
    //Check if attachment exists and add it  
    if( sourceStore->AttachmentManagerL().AttachmentCount() )
    	{       	    		    		
        tmpFile = attaManager.GetAttachmentFileL( 0 );      
    	CleanupClosePushL( tmpFile );  
    		
    	MMsvAttachmentManager& targetAttaMan = targetStore->AttachmentManagerL();
		CMsvAttachment* targetAtta = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
		CleanupStack::PushL( targetAtta );

        CMuiuOperationWait* waiter = CMuiuOperationWait::NewLC(); 
		targetAttaMan.AddAttachmentL( tmpFile, targetAtta, waiter->iStatus );			
		waiter->Start();

     	CleanupStack::PopAndDestroy( waiter ); //waiter 
		CleanupStack::Pop(targetAtta );// targetAtta
		CleanupStack::Pop( &tmpFile );//tmpFile
    	}
    
    TInt totalLength( aBody.DocumentLength() );
    HBufC* bodyText = HBufC::NewLC ( totalLength ); 
    TPtr bodyTextPtr ( bodyText->Des() ); 
    
    aBody.Extract( bodyTextPtr, 0, totalLength );
    richText->InsertL( 0, bodyTextPtr );
    CleanupStack::PopAndDestroy( bodyText );

    InsertSubjectL( *header, *richText );
                
    targetStore->StoreBodyTextL( *richText );
    
    header->StoreL( *targetStore );
    targetStore->CommitL();
       
    // Usually SMCM takes care of updating iSize, but now when msg is
    // created to Outbox for several recipients this has to be done manually.
    entry.iSize = targetStore->SizeL();
    entry.iRelatedId = iSmsServiceId;
    entry.iServiceId = KMsvLocalServiceIndexEntryId;
    outbox->ChangeL( entry );
    CleanupStack::PopAndDestroy( targetStore );
    
    iSession.CleanupEntryPop();
    CleanupStack::PopAndDestroy( outbox ); 
    CleanupStack::PopAndDestroy( sourceStore );
    CleanupStack::PopAndDestroy( header );
    CleanupStack::PopAndDestroy( richText );
    return entry.Id();
    }

// ---------------------------------------------------------
// CMsgSmsEditorAppUi::CreateMessageInOutboxL
// Creates message in outbox in case of multiple recipients 
// with some e-mail over SMS addresses
// ---------------------------------------------------------
TMsvId CUniSmsPlugin::CreateMessageInOutboxL(
    const TMsvEntry& aEntry, 
    const TDesC& aAddress )
    {
    CRichText* richText = CRichText::NewL( iParaFormatLayer, iCharFormatLayer );
    CleanupStack::PushL( richText );
    // Initialise header and store
    CSmsHeader* header = CSmsHeader::NewL( CSmsPDU::ESmsSubmit, *richText );
    CleanupStack::PushL( header );
    CMsvStore* store = SmsMtmL()->Entry().ReadStoreL();
     
    CleanupStack::PushL( store );
    // Read store
    header->RestoreL( *store );
    CleanupStack::PopAndDestroy( store );
    // Initialise number
    CSmsNumber* rcpt = CSmsNumber::NewL();
    CleanupStack::PushL( rcpt );
    header->Recipients().ResetAndDestroy();
    // Save Email specific information in header
    FillEmailInformationDataL( *header, aAddress );
    // Fill the recipient data for Email
    // Address = Email gateway
    // Alias = The real address
    rcpt->SetAddressL( iEmailOverSmsC->Address()  );
    rcpt->SetNameL( aAddress ); // This takes only 21 chars

    header->Recipients().AppendL( rcpt );
    CleanupStack::Pop( rcpt );
    // Create entry to Outbox
    TMsvEntry entry( aEntry );
    entry.iMtmData3 = KSmsPluginBioMsgUnparsed;
     
    CMsvEntry* outbox = iSession.GetEntryL( KMsvGlobalOutBoxIndexEntryId );
    CleanupStack::PushL( outbox );
    outbox->CreateL( entry );
    iSession.CleanupEntryPushL( entry.Id());
    outbox->SetEntryL( entry.Id());
    // Save 
    store = outbox->EditStoreL();
    CleanupStack::PushL( store );
    header->StoreL( *store );
    
    richText->Reset();
    richText->InsertL( 0 , SmsMtmL()->Body().Read( 0 ) );
    
    store->StoreBodyTextL( *richText );
    store->CommitL();
    // Usually SMCM takes care of updating iSize, but now when msg is
    // created to Outbox for several recipients this has to be done manually.
    entry.iSize = store->SizeL();
    entry.iRelatedId = iSmsServiceId;
    entry.iServiceId = KMsvLocalServiceIndexEntryId;
    outbox->ChangeL( entry );
    
    CleanupStack::PopAndDestroy( store );
    iSession.CleanupEntryPop();
    CleanupStack::PopAndDestroy( outbox );
    CleanupStack::PopAndDestroy( header );
    CleanupStack::PopAndDestroy( richText );
    return entry.Id();
    }

// ----------------------------------------------------------------------------
// SetScheduledSendingStateL
// ----------------------------------------------------------------------------
void CUniSmsPlugin::SetScheduledSendingStateL( CMsvEntrySelection* aSelection )
    {
    const TMsvEntry msvEntry = SmsMtmL()->Entry().Entry();
    if ( msvEntry.SendingState() == KMsvSendStateWaiting )
        {
        // Add entry to task scheduler
        TBuf8<1> dummyParams;

        CMuiuOperationWait* waiter = CMuiuOperationWait::NewLC();
        waiter->iStatus = KRequestPending;

        CMsvOperation* op= SmsMtmL()->InvokeAsyncFunctionL(
            ESmsMtmCommandScheduleCopy,
            *aSelection,
            dummyParams,
            waiter->iStatus );
        CleanupStack::PushL( op );
        waiter->Start();
        
        CleanupStack::PopAndDestroy( op );  
        CleanupStack::PopAndDestroy( waiter );
        } 
    }

// ----------------------------------------------------------------------------
// NameAndAddress
// ----------------------------------------------------------------------------
void CUniSmsPlugin::NameAndAddress( const TDesC& aMsvAddress, TPtrC& aName, TPtrC& aAddress )
    {
    TInt addressStart = aMsvAddress.LocateReverse( KMsgSmsAddressStartChar );
    TInt addressEnd = aMsvAddress.LocateReverse( KMsgSmsAddressEndChar );

    if ( addressStart != KErrNotFound && addressEnd != KErrNotFound 
        && addressEnd > addressStart )
        {
        // verified address, will be used as selected from contacts manager
        aName.Set( aMsvAddress.Ptr(), addressStart );
        aAddress.Set( 
            aMsvAddress.Mid( addressStart + 1 ).Ptr(), 
            ( addressEnd - addressStart ) -1 );
        if ( !aAddress.Length())
            {
            aAddress.Set( aName );
            aName.Set( KNullDesC ); // empty string
            }
        }
    else
        {
        // unverified string, will be used as entered in the header field
        aName.Set( KNullDesC ); // empty string
        aAddress.Set( aMsvAddress.Ptr(), aMsvAddress.Length() ); // a whole string to address
        }
    
    if ( aName.CompareF( aAddress ) == 0 )
        {
        aName.Set( KNullDesC ); // empty string
        }
    }

// ----------------------------------------------------------------------------
// CUniSmsPlugin::ExtractDescriptionFromMessageL
// ----------------------------------------------------------------------------
void CUniSmsPlugin::ExtractDescriptionFromMessageL(
    const CSmsMessage& aMessage, 
    TDes& aDescription, 
    TInt aMaxLength)
    {  
    if ( iUniMtm.SubjectL().Length() )
        {
        aDescription.Copy( iUniMtm.SubjectL() );
        }
    else
        {// Extract from message body
        aMessage.Buffer().Extract(
            aDescription, 
            0, 
            Min( 
                aMaxLength, 
                aMessage.Buffer().Length()));
        }

    //replace paragraphs with spaces.
    TBuf<KSmsEdExtrDescReplaceCharacterCount> replaceChars;
    replaceChars.Zero();
    replaceChars.Append( CEditableText::EParagraphDelimiter );
    replaceChars.Append( KSmsEdUnicodeLFSupportedByBasicPhones );
    replaceChars.Append( CEditableText::ELineBreak );
    AknTextUtils::ReplaceCharacters(
        aDescription, 
        replaceChars, 
        CEditableText::ESpace );

    aDescription.Trim();
    }

// ----------------------------------------------------------------------------
// CreatePlainTextSMSL 
// ----------------------------------------------------------------------------
void CUniSmsPlugin::CreatePlainTextSMSL( RFile& aFile)
    {       
    RFileReadStream stream( aFile );
    CleanupClosePushL( stream );
          
    CPlainText::TImportExportParam param;
    param.iForeignEncoding = KCharacterSetIdentifierUtf8;
    param.iOrganisation = CPlainText::EOrganiseByParagraph; 
            
    CPlainText::TImportExportResult result;
    
    SmsMtmL()->Body().ImportTextL( 0, stream, param, result );
                    
    CleanupStack::PopAndDestroy( &stream );        
    }
    
// ----------------------------------------------------------------------------
// InsertSubjectL
// Insert subject for non email addresses into the body
// ----------------------------------------------------------------------------
void CUniSmsPlugin::InsertSubjectL( CSmsHeader& aHeader, CRichText& aText  ) 
    {
    if ( iUniMtm.SubjectL().Length() && !iBioMsg )
        {
        TPtrC subject = iUniMtm.SubjectL();
        TInt writePosition = subject.Length()+2;//+2 for the parentesis
    
        if ( subject.Locate( KUniSmsStartParenthesis )!= KErrNotFound ||
            subject.Locate( KUniSmsEndParenthesis ) != KErrNotFound)
            {
            HBufC* modifiableSubject = subject.Alloc();
            CleanupStack::PushL(modifiableSubject);
            TPtr ptr = modifiableSubject->Des();
            
            TBuf<1> replaceChars;
            replaceChars.Zero();
            replaceChars.Append( KUniSmsStartParenthesis );
            // Replace '(' chars with '<'
            AknTextUtils::ReplaceCharacters(
                ptr, 
                replaceChars, 
                TChar('<') );
            
            replaceChars.Zero();
            replaceChars.Append( KUniSmsEndParenthesis );    
        
            // Replace ')' chars with '>'
            AknTextUtils::ReplaceCharacters(
                ptr, 
                replaceChars, 
                TChar('>') );  
            
            aText.InsertL( 0, KUniSmsStartParenthesis );
            aText.InsertL( 1, ptr );
            aText.InsertL( writePosition-1, KUniSmsEndParenthesis );
            CleanupStack::PopAndDestroy( modifiableSubject );
            }
 
        else
            {
            aText.InsertL( 0, KUniSmsStartParenthesis );
            aText.InsertL( 1, subject );
            aText.InsertL( writePosition-1, KUniSmsEndParenthesis );
            }
        }
    
    // Clears the CSmsHeaders EmailFields for non Email addresses
    CSmsEmailFields* emailFields = CSmsEmailFields::NewL();
    CleanupStack::PushL( emailFields );
    aHeader.SetEmailFieldsL( *emailFields );
    CleanupStack::PopAndDestroy( emailFields );  
    }
           
// ----------------------------------------------------------------------------
// CreateVCardSMS
// ----------------------------------------------------------------------------
void CUniSmsPlugin::CreateVCardSMSL( RFile& aFile )
    {   
    TInt fileSize;
    TInt err ( aFile.Size( fileSize ) );
    User::LeaveIfError(err);
    
    // Create two buffers: 8-bit for reading from file and 16-bit for
    // converting to 16-bit format
    HBufC8* buf8 = HBufC8::NewLC( fileSize );
    TPtr8 ptr8 = buf8->Des();
    HBufC16* buf16 = HBufC16::NewLC( fileSize );
    TPtr16 ptr16 = buf16->Des();

    for (TInt err = aFile.Read(ptr8); 
        ptr8.Length() > 0; 
        err = aFile.Read(ptr8))
        {
        User::LeaveIfError(err);
        ptr16.Copy(ptr8);
        SmsMtmL()->Body().InsertL(SmsMtmL()->Body().DocumentLength(), ptr16);
        }

    // Cleanup and return
    CleanupStack::PopAndDestroy( buf16 );   
    CleanupStack::PopAndDestroy( buf8 );   
    }
        
// ----------------------------------------------------------------------------
// CreateVCalSMS
// ----------------------------------------------------------------------------
void CUniSmsPlugin::CreateVCalSMSL( RFile& aFile )
    {
    TInt err (KErrNone);
    TInt fileSize;
    err = aFile.Size( fileSize );
    User::LeaveIfError(err);
     
    HBufC8* buf8 = HBufC8::NewLC( fileSize );
    TPtr8 ptr8 = buf8->Des();
    
    err = aFile.Read( ptr8 );
    User::LeaveIfError(err);
    
    HBufC16* buf16 = HBufC16::NewLC( fileSize );
    TPtr16 ptr16 = buf16->Des();
   
    ptr16.Copy(ptr8);
    SmsMtmL()->Body().InsertL(0, ptr16);
    
    CleanupStack::PopAndDestroy( buf16 );
    CleanupStack::PopAndDestroy( buf8 );
    }

// ----------------------------------------------------------------------------
// CUniSmsPlugin::ValidateSCNumberL
// ----------------------------------------------------------------------------
TBool CUniSmsPlugin::ValidateSCNumberL()
    {
    TBool valid( ETrue );
    if ( iSmsHeader->Message().ServiceCenterAddress().Length() == 0 ||
         !iSmsHeader->ReplyPathProvided() )
        {
        if ( !SmsMtmL()->ServiceSettings().ServiceCenterCount() )
            {
            if ( !SmsScDialogL())
                {
                valid = EFalse;
                }
            else
                {
                valid = ETrue;    
                CSmsServiceCenter& sc = SmsMtmL()->ServiceSettings().GetServiceCenter( 0 );

			    // Update service settings' sc address list and set it to default.
	            SmsMtmL()->ServiceSettings().SetDefaultServiceCenter( 0 ); 
	                
	            CSmsAccount* smsAccount = CSmsAccount::NewLC();
	            smsAccount->SaveSettingsL( SmsMtmL()->ServiceSettings() );
	            CleanupStack::PopAndDestroy( smsAccount );  
	
			    // Set the SC addres for this message
                iSmsHeader->Message().SetServiceCenterAddressL( 
                	sc.Address() );
                }
            }
        else
            {
            //Else use the default - or then the first one on the sc address list.
            TInt index = Max( 0, SmsMtmL()->ServiceSettings().DefaultServiceCenter() );
            CSmsServiceCenter& sc = SmsMtmL()->ServiceSettings().GetServiceCenter( index );
            iSmsHeader->Message().SetServiceCenterAddressL( sc.Address() );
            }
        }
    return valid;
    }

// ---------------------------------------------------------
// CUniSmsPlugin::ValidateSCNumberForEmailOverSmsL
// ---------------------------------------------------------
TBool CUniSmsPlugin::ValidateSCNumberForEmailOverSmsL()
    {
    TBool confNeeded( EFalse );
    // Read the email settings
    TBuf<KUniSmsSCStringLength> emailSmscNumber;
    TBuf<KUniSmsSCStringLength> emailGateWayNumber;
    TBool notUsed( EFalse );
    // The file may not exist
    TInt readResult = SmumUtil::ReadEmailOverSmsSettingsL( 
                        emailSmscNumber,
                        emailGateWayNumber,
                        notUsed );
    if ( KErrNone == readResult )
        {
        // Check that both have valid values
        // In any otther case we need to show the conf pop-up window
        if ( emailSmscNumber == KNullDesC )
            {
            // Use the sms smsc as default
            CSmsSettings* serviceSettings = &( SmsMtmL()->ServiceSettings() );
            
            if ( SmsMtmL()->ServiceSettings().DefaultServiceCenter() > 0 )
                {
                emailSmscNumber = 
                	serviceSettings->GetServiceCenter( 
                	    SmsMtmL()->ServiceSettings().DefaultServiceCenter() ).Address();
                }
            else
                {
                emailSmscNumber = 
                	serviceSettings->GetServiceCenter( 0 ).Address();
                }
            confNeeded = ETrue;
            }
        if ( emailGateWayNumber == KNullDesC )
            {
            confNeeded = ETrue;
            }
        }
    else
        {
        confNeeded = ETrue;
        }
    if ( confNeeded ) // Show the query
        {
        CUniSmsEMultilineQueryDialog* dlg = 
            CUniSmsEMultilineQueryDialog::NewL( 
                emailGateWayNumber, 
                emailSmscNumber, 
                ETrue );
        if ( dlg->ExecuteLD( R_ADDEMAILSC_QUERY ) )
            {
            // Save settings to Smum
            SmumUtil::WriteEmailOverSmsSettingsL( 
                        emailSmscNumber,        
                        emailGateWayNumber,
                        ETrue );
            }
        else
            {
            return EFalse;
            }
        }
    // Use them
    iEmailOverSmsC->SetNameL( emailSmscNumber );
	iEmailOverSmsC->SetAddressL( emailGateWayNumber );
	return ETrue;
    }

// ----------------------------------------------------------------------------
// CUniSmsPlugin::SmsScDialogL
// ----------------------------------------------------------------------------
TBool CUniSmsPlugin::SmsScDialogL()
    {
    TBuf<KUniSmsSCStringLength> name;
    TBuf<KUniSmsSCStringLength> number;
    TBool retVal = EFalse;
     
    // Read the default name
    SmumUtil::FindDefaultNameForSCL( name, EFalse );
    CUniSmsEMultilineQueryDialog* dlg = 
        CUniSmsEMultilineQueryDialog::NewL( name, number );
    if ( dlg->ExecuteLD( R_ADDSC_QUERY ) )
        {	
        retVal = ETrue;
        if ( !name.Length())
            {
            // read default name again 
            SmumUtil::FindDefaultNameForSCL( name, EFalse );
            }        
        SmsMtmL()->ServiceSettings().AddServiceCenterL( name, number );
        }
    return retVal;
    }

// ---------------------------------------------------------
// CUniSmsPlugin::FillEmailInformationDataL
// ---------------------------------------------------------
void CUniSmsPlugin::FillEmailInformationDataL( 
    CSmsHeader& aHeader, 
    const TPtrC& aAddress ) 
    {
    UNILOGGER_ENTERFN( "CUniSmsPlugin::FillEmailInformationDataL()" )
 
    CSmsEmailFields* emailFields = CSmsEmailFields::NewL();
    CleanupStack::PushL( emailFields );

    // The Email SMSC may differ from sms SMSC
    UNILOGGER_WRITEF( _L("Set EMailOverSMS SC: %S"), &iEmailOverSmsC->Name() );
    aHeader.Message().SetServiceCenterAddressL( iEmailOverSmsC->Name() );
        
    // Check if there is need to save as EmailFieds with header
    if ( aAddress.Length() ) 
        {
        // Set the address
        UNILOGGER_WRITEF( _L("Recipient: %S"), &aAddress );
        emailFields->AddAddressL( aAddress );
        }
           
    if ( iUniMtm.SubjectL().Length() )
        { // Handle the subject
        HBufC* buf = iUniMtm.SubjectL().AllocL();
        CleanupStack::PushL( buf );
        TPtr text = buf->Des();    
           
        TBuf<1> replaceChars;
        replaceChars.Zero();
        replaceChars.Append( KUniSmsStartParenthesis );
        // Replace '(' chars with '<'
        AknTextUtils::ReplaceCharacters(
            text, 
            replaceChars, 
            TChar('<') );
            
        replaceChars.Zero();
        replaceChars.Append( KUniSmsEndParenthesis );
        // Replace ')' chars with '>'
        AknTextUtils::ReplaceCharacters(
            text, 
            replaceChars, 
            TChar('>') );
            
        // For Emails save it to CSmsEmailFields
        emailFields->SetSubjectL( text );
        CleanupStack::PopAndDestroy( buf ); 
        }  
     
        
    aHeader.SetEmailFieldsL( *emailFields );
    CleanupStack::PopAndDestroy( emailFields ); 
    UNILOGGER_LEAVEFN( "CUniSmsPlugin::FillEmailInformationDataL()" )
    } 



// ----------------------------------------------------
//  CUniSmsPlugin::IsEmailAddress()
// ----------------------------------------------------
TBool CUniSmsPlugin::IsEmailAddress( const TPtrC& aAddress ) const
    {
    UNILOGGER_ENTERFN( "CMsgSmsEmailOverSmsFunc::IsEmailAddress()" )
    TBool isEmailAddress( EFalse );
    if (  aAddress.Locate('@')  != KErrNotFound)
        {
        UNILOGGER_WRITEF(
            _L("IsEmailAddress - Address: %s"), aAddress.Ptr() );
        isEmailAddress = ETrue;
        }
    UNILOGGER_LEAVEFN( "CMsgSmsEmailOverSmsFunc::IsEmailAddress()" )
    return isEmailAddress;
    }
    
// ----------------------------------------------------------------------------
// Panic
// ----------------------------------------------------------------------------
GLDEF_C void Panic( TInt aCategory )
    {
    _LIT( KUniSmsPluginPanic, "CUniSmsPlugin-Panic" );
    User::Panic( KUniSmsPluginPanic, aCategory );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// ------------------------------------------------------------------------------------------------
// ImplementationProxy
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof( KImplementationTable ) / sizeof( TImplementationProxy );
	return KImplementationTable;
	}

//------------------------------------------------------------------------------
// CUniSmsPlugin::SetEncodingSetings
// Turkish SMS-PREQ2265 Specific
// To Set encoding settings like encoding type, character support
// and alternative encoding if any
// 
// IMPORTANT NOTE: 
// This function is usually called from UniEditorAppUI to reset/set alternative encoding or char support
// when corresponding feilds change. Hence aUnicodeMode is always set to false
//------------------------------------------------------------------------------
void CUniSmsPlugin::SetEncodingSettings( TBool aUnicodeMode, TSmsEncoding aAlternativeEncodingType, TInt aCharSupportType)
    {
    UNILOGGER_WRITE_TIMESTAMP("CUniSmsPlugin::SetEncodingSettings Start <<<---- ");
    TSmsUserDataSettings smsSettings;
    CSmsMessage& smsMsg = iSmsHeader->Message();
    TBool settingsChanged = EFalse;
    
    iCharSupportType = aCharSupportType;
    iAlternativeEncodingType = aAlternativeEncodingType;
    if( iUnicodeMode != aUnicodeMode )
        {
        iUnicodeMode = aUnicodeMode;
    if(iUnicodeMode)
        {
        smsSettings.SetAlphabet( TSmsDataCodingScheme::ESmsAlphabetUCS2 );
        }
    else
        {
        smsSettings.SetAlphabet( TSmsDataCodingScheme::ESmsAlphabet7Bit );
        }
        settingsChanged = ETrue;
        }
    if( smsSettings.TextCompressed())
        {
    smsSettings.SetTextCompressed( EFalse );
        settingsChanged = ETrue;
        }
    if( settingsChanged )
        {
    TRAP_IGNORE(smsMsg.SetUserDataSettingsL( smsSettings ));
#ifdef USE_LOGGER
        UNILOGGER_WRITE("SetEncodingSettings: SetUserDataSettingsL");
#endif /* USE_LOGGER */
        }
    //First try without any alternate encoding
    if( aAlternativeEncodingType != smsMsg.Alternative7bitEncoding() )
        {
        smsMsg.SetAlternative7bitEncoding( aAlternativeEncodingType );// Optimal
#ifdef USE_LOGGER
        UNILOGGER_WRITEF(_L("SetEncodingSettings: aAlternativeEncodingType-> %d"), aAlternativeEncodingType);
#endif /* USE_LOGGER */
    }
    UNILOGGER_WRITE_TIMESTAMP("CUniSmsPlugin::SetEncodingSettings End ---->>> ");
    }
//------------------------------------------------------------------------------
// CUniSmsPlugin::GetNumPDUsL
// Turkish SMS-PREQ2265 Specific
// To get PDU Info: extracts details of number of PDUs, number of remaining chars in last PDU
// and encoding types used.
//------------------------------------------------------------------------------
void CUniSmsPlugin::GetNumPDUsL( 
        TDesC& aBuf,
        TInt& aNumOfRemainingChars,
        TInt& aNumOfPDUs,
        TBool& aUnicodeMode, 
        TSmsEncoding & aAlternativeEncodingType )
    {
    UNILOGGER_WRITE_TIMESTAMP("<<<<<<<<---------------------CUniSmsPlugin::GetNumPDUsL Start: ");
    TInt numOfUnconvChars, numOfDowngradedChars, isAltEncSupported;
    TSmsEncoding currentAlternativeEncodingType;

    CSmsMessage& smsMsg = iSmsHeader->Message();
    
#ifdef USE_LOGGER
    HBufC* tempBuff = HBufC::NewL( aBuf.Length() + 2 );
    tempBuff->Des().Copy( aBuf );
    UNILOGGER_WRITEF(_L("GetNumPDUsL:->> Buf Length: %d"), aBuf.Length() );
    UNILOGGER_WRITEF(_L("GetNumPDUsL:->> buffer    : %s"), tempBuff->Des().PtrZ() );
    delete tempBuff;
#endif /* USE_LOGGER */
    // need to set the input buffer to SMS buffer through iRichText(which is reference to SMS Buffer object)
    iRichText->Reset();
    iRichText->InsertL(0, aBuf);
    
    //call SMS stack API to get PDU info
    smsMsg.GetEncodingInfoL( aNumOfPDUs, numOfUnconvChars, numOfDowngradedChars, aNumOfRemainingChars );
    
#ifdef USE_LOGGER
    UNILOGGER_WRITE(" ---Get Encoding Info details:--- ");
    UNILOGGER_WRITEF(_L("      aNumPdus     : %d"), aNumOfPDUs);
    UNILOGGER_WRITEF(_L("      numOfUnconvChars : %d"), numOfUnconvChars);
    UNILOGGER_WRITEF(_L("      numOfDowngradedChars: %d"), numOfDowngradedChars);
    UNILOGGER_WRITEF(_L("      aNumOfRemainingChars: %d"), aNumOfRemainingChars);
#endif /* USE_LOGGER */
    //Algo to switch to Unicode if required
    while( (numOfUnconvChars || numOfDowngradedChars) && !iUnicodeMode )
        {
        currentAlternativeEncodingType = smsMsg.Alternative7bitEncoding();
#ifdef USE_LOGGER
        UNILOGGER_WRITEF(_L("GetNumPDUsL: currAltEnc -> %d"), currentAlternativeEncodingType);
#endif /* USE_LOGGER */
        if( currentAlternativeEncodingType != iAlternativeEncodingType )
            {
            //try with this new alternative encoding type
            isAltEncSupported = smsMsg.SetAlternative7bitEncoding( iAlternativeEncodingType );
            if( isAltEncSupported == KErrNotSupported )
                {
                // if required alternative encoding plugin is not supported, retain the existing encoding mechanism.
                iAlternativeEncodingType = currentAlternativeEncodingType;
                continue;
                }
            }
        else if( numOfUnconvChars || (TUniSendingSettings::TUniCharSupport)iCharSupportType == TUniSendingSettings::EUniCharSupportFull)
            {
            //switch to Unicode
            //iUnicodeMode = ETrue;
            SetEncodingSettings( ETrue, /*ESmsEncodingNone*/ iAlternativeEncodingType, iCharSupportType);
            }
        else
            {
            //Get out of while loop and return the results
            break;
            }
        //get the PDU info with new settings
        iRichText->Reset();
        iRichText->InsertL(0, aBuf);
        smsMsg.GetEncodingInfoL( aNumOfPDUs, numOfUnconvChars, numOfDowngradedChars, aNumOfRemainingChars );
#ifdef USE_LOGGER
        UNILOGGER_WRITE("******* Get Encoding Info details (Again):***** ");
        UNILOGGER_WRITEF(_L("     aNumPdus     : %d"), aNumOfPDUs);
        UNILOGGER_WRITEF(_L("     numOfUnconvChars : %d"), numOfUnconvChars);
        UNILOGGER_WRITEF(_L("     numOfDowngradedChars: %d"), numOfDowngradedChars);
        UNILOGGER_WRITEF(_L("     aNumOfRemainingChars: %d"), aNumOfRemainingChars);
#endif /* USE_LOGGER */
        }

    /*
     * Enable the below code to debug if something wrong with characters sent even in unicode mode
     */
    /*
    If((numOfUnconvChars || numOfDowngradedChars) && iUnicodeMode)
        UNILOGGER_WRITEF("GOTCHA... some chars are not convertable in unicode mode as well...????");
    */
    aUnicodeMode = iUnicodeMode;
    aAlternativeEncodingType = iAlternativeEncodingType;
    if(iUnicodeMode)
        {
        //In case of Unicode mode, SMS Stack returns number of available free User Data units.
        //Need to convert them w.r.t characters(each char takse 2 UD units).
        aNumOfRemainingChars = aNumOfRemainingChars/2;
        }
    UNILOGGER_WRITE_TIMESTAMP("CUniSmsPlugin::GetNumPDUsL End -------------------->>>>>>>>>>> ");
    }

//  End of File
