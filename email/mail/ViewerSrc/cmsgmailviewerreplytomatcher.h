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
* Description:  Mail Viewer reply to address matcher.
*
*/

#ifndef C_CMSGMAILVIEWERREPLYTOMATCHER_H
#define C_CMSGMAILVIEWERREPLYTOMATCHER_H

#include <e32base.h> // CBase
#include <MsgRecipientItem.h>

class CMsgMailViewerContactMatcher;
class CMsgMailViewerAppUi;


/**
 *  Mail Viewer reply to address matcher.
 *  Finds matches for sender's email address
 *  from phonebook. Uses CMsgMailViewerContactMatcher.
 *
 *  @lib none
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CMsgMailViewerReplyToMatcher)
: public CBase
    {

public:
    /**
     * NewL
     * @since S60 v3.2
     * @param aAppUi reference to application appui.
     * Needed for notifying that matching is done.
     * @param aMatcher reference to CMsgMailViewerContactMatcher.
     * @return self
     */
    static CMsgMailViewerReplyToMatcher* NewL(CMsgMailViewerAppUi& aAppUi, 
    							CMsgMailViewerContactMatcher& aMatcher);


    ~CMsgMailViewerReplyToMatcher();

    /**
     * CheckMatchesL
     * @since S60 v3.2
     * @param aMtmUid Uid of mtm that needs this information.
     * @param aRecs recipient array that has address which is
     * searched/matched from phonebook.
     * Leaves with KErrNotFound if array is empty.
     */
    void CheckMatchesL(TUid aMtmUid, CMsgRecipientArray* aRecs);

private: // implementation

    CMsgMailViewerReplyToMatcher(CMsgMailViewerAppUi& aAppUi, 
    					CMsgMailViewerContactMatcher& aMatcher);
    void ConstructL();


    static TInt AddressMatchCallbackL(TAny* aAny);
    void NotifyMatchingReadyL();


private: // data

    //reference
    CMsgMailViewerContactMatcher& iContactMacther;

    TUid iSendingMtmUid;

    //reference
    CMsgMailViewerAppUi& iAppUi;

    HBufC* iName;
    HBufC* iData;

    };

#endif // C_CMSGMAILVIEWERREPLYTOMATCHER_H
