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
* Description:  Mail viewer reply to address matcher.
*
*/


#include "cmsgmailviewerreplytomatcher.h"
#include "cmsgmailviewercontactmatcher.h"
#include "MsgMailViewerAppUi.h"

#include <e32std.h>
#include <MsgAddressControl.h>
#include <SenduiMtmUids.h>

// ---------------------------------------------------------------------------
// CMsgMailViewerReplyToMatcher
// ---------------------------------------------------------------------------
//
CMsgMailViewerReplyToMatcher::CMsgMailViewerReplyToMatcher(
		CMsgMailViewerAppUi& aAppUi, CMsgMailViewerContactMatcher& aMatcher)
    :iContactMacther(aMatcher), 
    iAppUi(aAppUi)
    {
    
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CMsgMailViewerReplyToMatcher* CMsgMailViewerReplyToMatcher::NewL(
		CMsgMailViewerAppUi& aAppUi, CMsgMailViewerContactMatcher& aMatcher)
    {
    CMsgMailViewerReplyToMatcher* self =
        new( ELeave ) CMsgMailViewerReplyToMatcher(aAppUi, aMatcher);
    
    return self;
    }

// ---------------------------------------------------------------------------
// ~CMsgMailViewerReplyToMatcher
// ---------------------------------------------------------------------------
//
CMsgMailViewerReplyToMatcher::~CMsgMailViewerReplyToMatcher()
    {
    delete iData;
    delete iName;
    } 


//----------------------------------------------------------------------------
//  AddressMatchCallbackL()
//----------------------------------------------------------------------------
TInt CMsgMailViewerReplyToMatcher::AddressMatchCallbackL(TAny* aAny)	// CSI: 40 # We must return 
																		// the integer value although this 
																		// is a leaving method.
	{
	static_cast<CMsgMailViewerReplyToMatcher*>(aAny)->NotifyMatchingReadyL();	
	return 0; // one-time TCallBack call
	}
	
//----------------------------------------------------------------------------
//  NotifyMatchingReadyL()
//----------------------------------------------------------------------------
void CMsgMailViewerReplyToMatcher::NotifyMatchingReadyL()
    {
    //Get data which user has selected from contact matcher.
    iData = iContactMacther.GetDataL();
    
    if( iData )
        {
        //Get name for contact.
        iName = iContactMacther.GetNameL();
        iAppUi.DoReplyViaL(iSendingMtmUid, iData, iName);
        }
    else
        {
        iAppUi.DoReplyViaL(iSendingMtmUid, NULL, NULL);
        }
    }
    
        
//----------------------------------------------------------------------------
//  CheckMatchesL()
//----------------------------------------------------------------------------
void CMsgMailViewerReplyToMatcher::CheckMatchesL(TUid aMtmUid, CMsgRecipientArray* aRecs)
    {
    iSendingMtmUid = aMtmUid;
    
    if ( aRecs )
        {
        TInt count( aRecs->Count() );
        
        if(count > 0)
            {
            //We can only have one from-adderess
            iContactMacther.FindContactL(*aRecs->At(0)->Address(), 
            		CMsgMailViewerContactMatcher::EMsgToContact, 
            		TCallBack(AddressMatchCallbackL, this));    
            }
        else
            {
            User::Leave(KErrNotFound);
            }
        }
    }
