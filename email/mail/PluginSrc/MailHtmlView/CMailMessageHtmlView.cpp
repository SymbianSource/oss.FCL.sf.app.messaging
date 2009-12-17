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
* Description:  Mail message HTML view
*
*/


// INCLUDE FILES
#include 	"MailLog.h"
#include    "CMailMessageHtmlView.h"
#include 	"msgmailviewer.hrh"
#include    "CMailHtmlBodyControl.h"
#include    "CHtmlMailLoadStateMachine.h"
#include 	"mailmessageutils.h"
#include    <MMailAppUiInterface.h>
#include 	<CMailMessage.h>
#include    <MsgEditorView.h>  // CMsgEditorView
#include 	<MsgMailViewer.rsg>
#include 	<FindItem.hrh>
#include    <FindItemMenu.rsg>
#include 	<centralrepository.h>
#include	<CommonUiInternalCRKeys.h>
#include 	<MMSVATTACHMENTMANAGER.H>
#include    <ImumInternalApi.h>
#include    <ImumInSettingsKeys.h>
#include    <ImumInSettingsData.h>


// Messaging
#include    <SenduiMtmUids.h>
#include    <MIUTSET.h>
#include    <MsgEditor.hrh> // Control types


// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMailMessageHtmlView::CMailMessageHtmlView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMailMessageHtmlView::CMailMessageHtmlView()
    {
    }

// -----------------------------------------------------------------------------
// CMailMessageHtmlView::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMailMessageHtmlView* CMailMessageHtmlView::NewL()
	{
	return new(ELeave) CMailMessageHtmlView ();
	}

// Destructor
CMailMessageHtmlView::~CMailMessageHtmlView()
    {
    delete iHtmlBodyControl;
    }

// -----------------------------------------------------------------------------
// CMailMessageHtmlView::DynInitOptionsMenuL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMailMessageHtmlView::DynInitMenuPaneL(
    TInt aResourceId,CEikMenuPane* aMenuPane)
    {
    if ( aResourceId == R_MSGMAILVIEWER_OPTIONS_MENU2 || 
    	aResourceId == R_FINDITEMMENU_MENU )
    	{
    	TInt index(0);
/*    	TInt value(0);
	    // Create the session
	    CRepository* crSession = CRepository::NewLC( KCRUidCommonUi );
        // Get the value of AutomaticHighlight key
        crSession->Get( KCuiAutomaticHighlight, value );
        
        if (value)
        	{
        	aMenuPane->SetItemDimmed(EFindItemCmdDisableAutoFind, ETrue);
        	}
        else
        	{
        	aMenuPane->SetItemDimmed(EFindItemCmdEnableAutoFind, ETrue);
        	}
        	
   		aMenuPane->SetItemDimmed(EMsgMailViewerCmdCreateContact, 
   			iHtmlBodyControl->BrowserControl().IsFocused());
*/
    	 // + 1 -> next to delete
    	iHtmlBodyControl->BrowserControl().AddOptionMenuItemsL(
    		*aMenuPane, aResourceId, index);	
    	}
    }

// -----------------------------------------------------------------------------
// CMailMessageHtmlView::HandleCommandL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMailMessageHtmlView::HandleCommandL(TInt aCommand)
    {
    TBool consumed(EFalse);
    LOG1("CMailMessageHtmlView::HandleCommandL aCommand:%d", 
	    aCommand);
    iHtmlBodyControl->BrowserControl().HandleCommandL(aCommand);
    return consumed;
    }

// -----------------------------------------------------------------------------
// CMailMessageHtmlView::HandleKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CMailMessageHtmlView::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
    {
    TKeyResponse consumed( EKeyWasNotConsumed );
    LOG2("CMailMessageHtmlView::HandleKeyEventL aKeyEvent:%d  aType:%d", 
	    aKeyEvent.iCode, aType); 
    if ( aKeyEvent.iCode == EKeyLeftArrow && 
        iAppUI->IsNextMessageAvailableL(EFalse) )
        {
        iAppUI->NextMessageL(EFalse);
        consumed = EKeyWasConsumed;
        }
    else if ( aKeyEvent.iCode == EKeyRightArrow && 
        iAppUI->IsNextMessageAvailableL(ETrue))
        {
        iAppUI->NextMessageL(ETrue);
        consumed = EKeyWasConsumed;
        }        
    else
        {
        consumed = iHtmlBodyControl->BrowserControl().OfferKeyEventL(
    	    aKeyEvent, aType );        
        }      

    return consumed;
    }

// -----------------------------------------------------------------------------
// CMailMessageHtmlView::MessageLoadL
// -----------------------------------------------------------------------------
//
void CMailMessageHtmlView::MessageLoadL(TInt aState, CMailMessage& aMessage)
	{
	if (aState == CMailMessage::EHeaderReady)
		{
		if( !HtmlSettingValueL( aMessage ) )
			{
			User::Leave(KErrNotSupported);
			}
		}
	}
// -----------------------------------------------------------------------------
// CMailMessageHtmlView::ViewMessageL
// -----------------------------------------------------------------------------
//
void CMailMessageHtmlView::ViewMessageL(
    CMailMessage& aMessage,
    MMailAppUiInterface& aUICallBack,
    TUint& aParams)
    {
    iAppUI = &aUICallBack;
    iMessage = &aMessage;
    
    // Set data
    CImHeader* header = aMessage.MessageHeader();
    SetHeaderControlTextL(aMessage, *header);
    CreateBrControlL();
	
	iLoadStateMachine = CHtmlLoadStateMachine::NewL( 
			iAppUI, 
			aMessage,
			*iHtmlBodyControl );
	iLoadStateMachine->StartL();		
    aParams |= EMailUseDefaultNaviPane;     
    }

// -----------------------------------------------------------------------------
// CMailMessageHtmlView::CreateBrControlL
// -----------------------------------------------------------------------------
//
void CMailMessageHtmlView::CreateBrControlL()
    {
	CMailHtmlBodyControl* htmlBodyControl = 
		CMailHtmlBodyControl::NewLC( *iMessage, *iAppUI );
    iAppUI->AddControlL(*htmlBodyControl);
    iHtmlBodyControl = htmlBodyControl;
    CleanupStack::Pop(); // htmlBodyControl
    }

// -----------------------------------------------------------------------------
// CMailMessageHtmlView::SetHeaderControlTextL
// -----------------------------------------------------------------------------
//
void CMailMessageHtmlView::SetHeaderControlTextL(
    CMailMessage& aMessage, CImHeader& aHeader)
    {
    if ( MailMessageUtils::IsOutgoingL( aMessage ) )
        {
	    CDesCArray& toRecipients = aHeader.ToRecipients();
	    if (toRecipients.Count())
	    	{
	    	iAppUI->AddControlL(EMsgComponentIdTo);
	    	iAppUI->SetAddressFieldTextL(
            	EMsgComponentIdTo, 
            	toRecipients);
	    	}        
        }
    else
        {
        HBufC* from = aHeader.From().AllocLC();
    	if ( from->Length()	)
    		{
    		iAppUI->AddControlL(EMsgComponentIdFrom);
    		iAppUI->SetAddressFieldTextL(
            	EMsgComponentIdFrom, 
            	*from);
    		}    	
    	CleanupStack::PopAndDestroy(); // from
        }
        
    CDesCArray& ccRecipients = aHeader.CcRecipients();
    if (ccRecipients.Count())
    	{
    	iAppUI->AddControlL(EMsgComponentIdCc);
    	iAppUI->SetAddressFieldTextL(
            EMsgComponentIdCc, 
            ccRecipients);
    	}

    CDesCArray& bccRecipients = aHeader.BccRecipients();
    if (bccRecipients.Count())
    	{
    	iAppUI->AddControlL(EMsgComponentIdBcc);
    	iAppUI->SetAddressFieldTextL(
            EMsgComponentIdBcc, 
            bccRecipients);
    	}

    iAppUI->AddControlL(EMsgComponentIdSubject);
    iAppUI->SetSubjectL(aHeader.Subject());  
    }

// -----------------------------------------------------------------------------
// CMailMessageHtmlView::HtmlSettingValueL
// -----------------------------------------------------------------------------
//
TBool CMailMessageHtmlView::HtmlSettingValueL( CMailMessage& aMessage )
    {
    TBool retValue( EFalse );
    // Use OpenHtml mail setting value in HTML plugin
    TMsvEmailEntry message = aMessage.MessageEntry();    
    if ( message.MHTMLEmail() )
        {
        CImumInternalApi* api = CreateEmailApiLC( aMessage.Session() );
        CImumInSettingsData* settings = 
            api->MailboxServicesL().LoadMailboxSettingsL( 
                message.iServiceId );
        CleanupStack::PushL( settings );    
        
        settings->GetAttr( TImumInSettings::EKeyOpenHtmlMail, retValue );
        
        CleanupStack::PopAndDestroy( settings );
        settings = NULL;
        CleanupStack::PopAndDestroy( api );
        api = NULL;
        }
        
    return retValue;
    }
  
//  End of File  
