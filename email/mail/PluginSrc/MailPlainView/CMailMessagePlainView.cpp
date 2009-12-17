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
* Description:  Mail message plain view
*
*/


// INCLUDE FILES
#include 	"MailLog.h"
#include 	"MailUtils.h"
#include    "CMailMessagePlainView.h"
#include 	"MailLoadStateMachine.h"
#include    "mailmessageutils.h"
#include 	<MailPlainView.rsg>
#include    <MMailAppUiInterface.h>
#include    <CMailMessage.h>
#include    <SenduiMtmUids.h>
#include    <miutset.h>
#include    <MsgEditor.hrh> // Control types
#include 	<avkon.hrh>
#include 	<akntabgrp.h>
#include 	<aknnavide.h>
#include 	<StringLoader.h>

// CONSTANTS
_LIT(KMailPlainViewResourceFile, "z:MailPlainView.rsc");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMailMessagePlainView::CMailMessagePlainView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMailMessagePlainView::CMailMessagePlainView() {}

// -----------------------------------------------------------------------------
// CWPEmailAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMailMessagePlainView* CMailMessagePlainView::NewL()
	{
	CMailMessagePlainView* self = new(ELeave) CMailMessagePlainView; 
	return self;
	}

// Destructor
CMailMessagePlainView::~CMailMessagePlainView()
    {
    delete iLoadStateMachine;
    }

// -----------------------------------------------------------------------------
// CMailMessagePlainView::DynInitOptionsMenuL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMailMessagePlainView::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {    }

// -----------------------------------------------------------------------------
// CMailMessagePlainView::HandleCommandL
// -----------------------------------------------------------------------------
//
TBool CMailMessagePlainView::HandleCommandL(TInt /*aCommand*/)
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CMailMessagePlainView::HandleKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CMailMessagePlainView::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,
    TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CMailMessagePlainView::MessageLoadL
// -----------------------------------------------------------------------------
//
void CMailMessagePlainView::MessageLoadL(
	TInt /*aState*/, CMailMessage& /*aMessage*/)
    {
    // Deside here if this message belongs to this plugin.
    // Do nothing. (PlainView plug-in "can" show all messages)
    }

// -----------------------------------------------------------------------------
// CMailMessagePlainView::ViewMessageL
// -----------------------------------------------------------------------------
//
void CMailMessagePlainView::ViewMessageL(CMailMessage& aMessage, 
    MMailAppUiInterface& aUICallBack, TUint& aParams)
    {
    iAppUI = &aUICallBack;          
    // ref to message header
    CImHeader* header = aMessage.MessageHeader();
    SetHeaderControlTextL(aMessage, *header);    
    SetBodyControlTextL(aMessage);
    delete iLoadStateMachine;
    iLoadStateMachine = NULL;    
	iLoadStateMachine = CLoadStateMachine::NewL( 
			iAppUI, 
			aMessage );
	iLoadStateMachine->StartL();		
    aParams |= EMailUseDefaultNaviPane; 
    }
// -----------------------------------------------------------------------------
// CMailMessagePlainView::ResourceFile
// -----------------------------------------------------------------------------
//
const TDesC& CMailMessagePlainView::ResourceFile()
	{
	return KMailPlainViewResourceFile;
	}

// -----------------------------------------------------------------------------
// CMailMessagePlainView::AddCcAndBccConstrolsL
// -----------------------------------------------------------------------------
//
void CMailMessagePlainView::AddCcAndBccConstrolsL( CImHeader& aHeader )
    {
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
    }

// -----------------------------------------------------------------------------
// CMailMessagePlainView::SetBodyControlTextL
// -----------------------------------------------------------------------------
//
void CMailMessagePlainView::SetBodyControlTextL(CMailMessage& aMessage)
    {
	// HTML body is set by iLoadStateMachine
	if ( !aMessage.MessageEntry().MHTMLEmail() )
		{
		iAppUI->SetBodyTextL(*aMessage.MessageBody());
        iAppUI->ViewMessageComplete();
		}
    }

// -----------------------------------------------------------------------------
// CMailMessagePlainView::SetHeaderControlTextL
// -----------------------------------------------------------------------------
//
void CMailMessagePlainView::SetHeaderControlTextL(
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
	    AddCcAndBccConstrolsL( aHeader );	        
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

    iAppUI->AddControlL(EMsgComponentIdSubject);
    iAppUI->SetSubjectL(aHeader.Subject());
    }
    
//  End of File  
