/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  State machine -based operation for sending mail message.
*
*/



// INCLUDE FILES
#include "cmsgmailsendop.h"
#include "mmsgmailappuiopdelegate.h"
#include "MailLog.h"
#include "MailUtils.h"
#include "MsgMailPreferences.h"
#include "MsgMailEditorDocument.h"
#include <StringLoader.h>
#include <mtmuibas.h>
#include <cemailaccounts.h>
#include <Muiumsginfo.hrh>
#include <aknnotewrappers.h>
#include <MtmExtendedCapabilities.hrh>
#include <MsgEditorAppUi.rsg>       // resource identifiers
#include <MsgMailEditor.rsg>
#include <MuiuMsvUiServiceUtilities.h>  // MUIU MTM utils
#include <ImumInHealthServices.h>
#include <ImumInMailboxServices.h>
#include "EmailFeatureUtils.h"


const TInt KMaximumAddressLength(100);

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgMailSendOp::CMsgMailSendOp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgMailSendOp::CMsgMailSendOp(
    CMsgMailEditorDocument& aDocument,
    MMsgMailAppUiOpDelegate& aOpDelegate )
    : CMsgMailBaseOp( aDocument ),
      iOpDelegate( aOpDelegate )
    {
    }

// -----------------------------------------------------------------------------
// CMsgMailSendOp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMsgMailSendOp::ConstructL()
    {
    iFlags = MsvEmailMtmUiFeatureUtils::EmailFeaturesL( ETrue, ETrue );
    }

// -----------------------------------------------------------------------------
// CMsgMailSendOp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgMailSendOp* CMsgMailSendOp::NewL(
    CMsgMailEditorDocument& aDocument,
    MMsgMailAppUiOpDelegate& aOpDelegate )
    {
    CMsgMailSendOp* self = new( ELeave ) CMsgMailSendOp( aDocument,
                                                         aOpDelegate );
    CleanupStack::PushL( self );
    self->ConstructL();
    
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMsgMailSendOp::~CMsgMailSendOp
// -----------------------------------------------------------------------------
//
CMsgMailSendOp::~CMsgMailSendOp()
    {
    Cancel();
    delete iFlags;
    }
    
// -----------------------------------------------------------------------------
// CMsgMailSendOp::HandleStateActionL
// -----------------------------------------------------------------------------
//
void CMsgMailSendOp::HandleStateActionL()
    {
    switch ( iState )
        {
        case EPrepare:
            {
            PrepareL();
            break;
            }
        case ESaveMessage:
            {
            SaveMessageL();
            break;
            }
        case ESendMessage:
            {            
            SendMessageL();
            break;
            }
        case EFinalize:
            {
            Finalize();
            break;
            }            
        default:
            {
            // should never come here
            ASSERT( EFalse );
            break;
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CMsgMailSendOp::SetNextState
// -----------------------------------------------------------------------------
//
void CMsgMailSendOp::SetNextState()
    {
    switch ( iState )
        {
        case EIdleState:
            {
            iState = EPrepare;
            break;
            }        
        case EPrepare:
            {
            iState = ESaveMessage;
            break;
            }
        case ESaveMessage:
            {
            iState = ESendMessage;
            break;
            }
        case ESendMessage:
            {
            iState = EFinalize;
            break;            
            }
        case EFinalize: // fall through
        default:
            {            
            iState = EIdleState;
            break;
            }
        }        
    }
     
// -----------------------------------------------------------------------------
// CMsgMailSendOp::HandleOperationCancel
// -----------------------------------------------------------------------------
//
void CMsgMailSendOp::HandleOperationCancel()
    {
    // nothing to cancel
    }
            
// -----------------------------------------------------------------------------
// CMsgMailSendOp::PrepareL
// -----------------------------------------------------------------------------
//
void CMsgMailSendOp::PrepareL()
	{
    CMsgMailPreferences& prefs = iDocument.SendOptions();
    TMsvId sendingBox = SendingBoxL( prefs );
    SetSchedulingOptionsL( prefs, sendingBox );    
	CompleteStateAction();
	}

// -----------------------------------------------------------------------------
// CMsgMailSendOp::SaveMessageL
// -----------------------------------------------------------------------------
//    
void CMsgMailSendOp::SaveMessageL()
    {
    // we can't handle saving by ourself so call the delegator
    iOpDelegate.DelegateSaveMsgL();
    CompleteStateAction();
    }
    
// -----------------------------------------------------------------------------
// CMsgMailSendOp::SendMessageL
// -----------------------------------------------------------------------------
//    
void CMsgMailSendOp::SendMessageL()
    {
    // We need to do offline check before sendind so that correct note can
    // be displayed
    if ( MailUtils::OfflineProfileActiveL() && !iFlags->GF( EMailFeatureProtocolWlan ))
  	    {
	    iDocument.SendOptions().SetMessageScheduling( 
	    		CMsgMailPreferences::EMsgMailSchedulingNextConn );
	    }
        	
    // start wait note showing at this point
	StartWaitNoteL( WaitNoteTextResourceId(), ETrue, R_WAIT_SEND_NOTE );
	SendNativeMailL();
	CompleteStateAction();
    }    

// -----------------------------------------------------------------------------
// CMsgMailSendOp::Finalize
// -----------------------------------------------------------------------------
//    
void CMsgMailSendOp::Finalize()
    {
    StopWaitNote();
    CompleteStateAction();
    }        

// -----------------------------------------------------------------------------
// CMsgMailSendOp::SendingBoxL
// -----------------------------------------------------------------------------
//
TMsvId CMsgMailSendOp::SendingBoxL( CMsgMailPreferences& aPrefs ) const
    {        
    // Check that box exists
    TMsvId sendingBox = aPrefs.ServiceId();
    CMsvEntrySelection* sendingAccounts =
    	MsvUiServiceUtilities::GetListOfAccountsWithMTML(
        iDocument.Session(),
        iDocument.CurrentEntry().Entry().iMtm,
        ETrue );
    TInt retValue = sendingAccounts->Find( sendingBox );
	delete sendingAccounts;

    if ( retValue == KErrNotFound )
        { // no mailboxes defined        
        HBufC* string = StringLoader::LoadLC( R_MAIL_MAILBOX_MISSING,
                                              CEikonEnv::Static() );		// CSI: 27 # Must be used because of iEikEnv 
																			// is not accessible.
        CAknInformationNote* note =
            new( ELeave ) CAknInformationNote( ETrue );
        note->ExecuteLD( *string );
        CleanupStack::PopAndDestroy( string ); // string
        // unacceptable situation -> leave
        User::Leave( KErrNotFound );
        }
        
    // check own address
    else if ( !CheckOwnAddressL() )
        {
        // consider this case as cancelled send operation
        User::Leave( KErrCancel );
        }
    return sendingBox;    
    }

// -----------------------------------------------------------------------------
// CMsgMailSendOp::SetSchedulingOptionsL
// -----------------------------------------------------------------------------
//
void CMsgMailSendOp::SetSchedulingOptionsL(
    CMsgMailPreferences& aPrefs,
    TMsvId aSendingBox ) const
    {    
    // Overwrite send options if not wlan and offline mode
    CImumInternalApi* api = CreateEmailApiLC( &iDocument.Session() );
    if ( !api->MailboxUtilitiesL().HasWlanConnectionL( aSendingBox ) &&
        MailUtils::OfflineProfileActiveL())
        {
        aPrefs.SetMessageScheduling(
            CMsgMailPreferences::EMsgMailSchedulingNextConn);
        }
    else if(iDocument.IsOnlineL())
        {
        aPrefs.SetMessageScheduling(
            CMsgMailPreferences::EMsgMailSchedulingNow);
        }

    CleanupStack::PopAndDestroy( api );
    api = NULL;
    }

// -----------------------------------------------------------------------------
// CMsgMailSendOp::WaitNoteTextResourceId
// -----------------------------------------------------------------------------
//
TInt CMsgMailSendOp::WaitNoteTextResourceId() const
    {
    //Select correct text for the wait note.
    TInt textResID = R_TEXT_SENDING_MAIL;
    if ( iDocument.SendOptions().MessageScheduling()
            == CMsgMailPreferences::EMsgMailSchedulingNextConn )
        {
        textResID = R_TEXT_SENDING_MAIL_NEXTCON;
        }

    return textResID;
    }

// -----------------------------------------------------------------------------
// CMsgMailSendOp::CheckOwnAddressL
// -----------------------------------------------------------------------------
//
TBool CMsgMailSendOp::CheckOwnAddressL() const
    {
    TBool result( ETrue );

	CEmailAccounts* smtpAccount = CEmailAccounts::NewLC();
    CImSmtpSettings* smtpSet=new( ELeave ) CImSmtpSettings();
    CleanupStack::PushL( smtpSet );
    const TMsvId smtpSetId = iDocument.CurrentEntry().Entry().iServiceId;

    TSmtpAccount accountParams;
    smtpAccount->GetSmtpAccountL( smtpSetId, accountParams );
	smtpAccount->LoadSmtpSettingsL( accountParams, *smtpSet );

    if ( !smtpSet->EmailAddress().Length() )
        {
        TBuf<KMaximumAddressLength> text;
        CAknTextQueryDialog* dlg = new( ELeave ) CAknTextQueryDialog( text );
        if ( dlg->ExecuteLD( R_MAIL_ADDRESS_QUERY ) )
            {
            smtpSet->SetEmailAddressL( text );
            smtpAccount->SaveSmtpSettingsL( accountParams, *smtpSet );
            }
        else
            {
            result = EFalse;
            }
        }

	CleanupStack::PopAndDestroy( 2, smtpAccount ); // CSI: 47,12 # smtpSet, smtpAccount
    return result;
    }
    
// -----------------------------------------------------------------------------
// CMsgMailSendOp::SendNativeMailL
// -----------------------------------------------------------------------------
//    
void CMsgMailSendOp::SendNativeMailL()
    {
    // get entry
    const TMsvEmailEntry& msvEntry = iDocument.Entry();
    TMsvId messageId( msvEntry.Id() );    
    if ( msvEntry.Parent() != KMsvGlobalOutBoxIndexEntryId )
        {
        // MsgEditor can handle moving to outbox so ask delegator's help
        messageId = iOpDelegate.DelegateMoveMsgToOutboxL();
        }
       
    // check, if message sending type is EMsgMailSchedulingNow
    if ( iDocument.SendOptions().MessageScheduling() ==
         CMsgMailPreferences::EMsgMailSchedulingNow )
        {
        // send immediately
        iDocument.SetEntryL( messageId );
        // SendImmediatelyL() is also asynchronous by nature
        iDocument.SendImmediatelyL( messageId );
        }

    } 
    
// End Of File
