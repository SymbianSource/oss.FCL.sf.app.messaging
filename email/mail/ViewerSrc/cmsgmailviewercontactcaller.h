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
* Description:  Mail viewer contact caller.
*
*/

#ifndef C_CMSGMAILVIEWERCONTACTCALLER_H
#define C_CMSGMAILVIEWERCONTACTCALLER_H

#include <e32base.h> // CBase

// FORWARD DECLARATIONS
class CEikMenuPane;
class CAiwServiceHandler;
class CMsgMailViewerContactMatcher;

/**
 *  Mail Viewer contact caller. 
 *
 *  @lib none
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CMsgMailViewerContactCaller)
    {
public:

    /**
     * NewL
     * 
     * @since S60 v3.2
     * @param service handler for attaching Call interest
     * @param contact matcher reference
     * @return self
     */
    static CMsgMailViewerContactCaller* NewL(
        CAiwServiceHandler& aServiceHandler,
        CMsgMailViewerContactMatcher& aContactMatcher );
    
    ~CMsgMailViewerContactCaller();
    
public:

    /**
    * To be called from owner's DynInitMenuPaneL.
    * @param aResourceId resource id from caller
    * @param aMenuPane menu pane from caller
    * @param aAddress address
    * @param aShowCallOption ETrue if option can be shown in menu
    */
    void DynInitMenuPaneL( TInt aResourceId,
                           CEikMenuPane* aMenuPane,
                           TBool aShowCallOption );
    
    /**
    * Method to test if given command is applicable for this class
    * and at this very moment.
    * @param aCommand command id
    * @return ETrue if command can be handled
    */                       
    TBool CanHandleCommand( TInt aCommand ) const;

    /**
    * Command handler. Use CanHandleCommand first to test if
    * command is applicable.
    * @param aCommand command id
    * @param aEmailAddress address
    */
    void CallToContactL( TInt aCommand, const TDesC& aEmailAddress );
    
private: // implementation

    static TInt MatchDoneL( TAny* aCaller );
    
    void DoCallToContactL();
        
private:

    CMsgMailViewerContactCaller(
        CAiwServiceHandler& aServiceHandler,
        CMsgMailViewerContactMatcher& aContactMatcher );
        
    void ConstructL();
    
private: // data

    TInt iCommand;

    /// AIW  service handler reference
    CAiwServiceHandler& iServiceHandler;
    
    /// contact matcher reference
    CMsgMailViewerContactMatcher& iContactMatcher;
    };

#endif // C_CMSGMAILVIEWERCONTACTCALLER_H
