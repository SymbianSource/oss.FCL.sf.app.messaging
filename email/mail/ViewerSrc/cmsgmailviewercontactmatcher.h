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
* Description:  Mail viewer contact matcher.
*
*/

#ifndef C_CMSGMAILVIEWERCONTACTMATCHER_H
#define C_CMSGMAILVIEWERCONTACTMATCHER_H

#include <e32base.h> // CBase
#include <AiwCommon.h> // MAiwNotifyCallback

class CContactMatcher;
class CVPbkContactLinkArray;
class MVPbkStoreContact;
class TAiwSingleItemSelectionDataV3;
class CAiwServiceHandler;
class CVPbkFieldTypeSelector;
class CAknWaitDialog;

/**
 *  Mail Viewer contact matcher. 
 *  Finds possible phonenumber for mail address from phonebook
 *
 *  @lib none
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CMsgMailViewerContactMatcher)
: public CActive, MAiwNotifyCallback
    {
    
public:
    enum TPurpose
        {
        EPurposeNone, // uninitialized
        ECallToContact,
        EMsgToContact
        };
        
public:

    /**
     * NewL
     * @since S60 v3.2
     * @param aServiceHandler for attaching AIW interest
     * @return self
     */
    static CMsgMailViewerContactMatcher* NewL(
        CAiwServiceHandler& aServiceHandler );
    

    ~CMsgMailViewerContactMatcher();

    /**
     * Search contacts that have specified email address.
     * @param aMailAddress searched email address.
     * @param aPurpose determines which fields are used in matching
     * @param aCallBack call-back function to call when match is ready       
     * @since S60 v3.2
     */    
    void FindContactL( const TDesC& aMailAddress,
                       TPurpose aPurpose,
                       TCallBack aCallBack );
    
    /**
    * Method to read match result, may be called after clients callback method
    * is called. Returns NULL if no match is found.
    * @return match result, ownership is transferred to caller
    */
    HBufC* GetDataL() const;
    
    
    /**
    * Method to read name of match result, may be called after clients callback method
    * is called. Returns NULL if no name is found.
    * @return match name, ownership is transferred to caller
    */
    HBufC* GetNameL() const;

private: // From CActive    
	void DoCancel();
	void RunL();
	TInt RunError( TInt aError ); 
	
	// From MAiwNotifyCallback
	TInt HandleNotifyL( TInt aCmdId,
                        TInt aEventId,
                        CAiwGenericParamList& aEventParamList,
                        const CAiwGenericParamList& aInParamList );  

private: // implementation  

    void DeleteExcessMatches( CVPbkContactLinkArray& aLinks ) const;

    CVPbkFieldTypeSelector* CreateFilterLC( TPurpose aPurpose ) const;
    
    TAiwSingleItemSelectionDataV3 SelectionData(
        TPurpose aPurpose,
        CVPbkFieldTypeSelector& aFilter ) const;
    
    void ExecuteSingleItemFetchL( TAiwSingleItemSelectionDataV3 aData,
                                  const CVPbkContactLinkArray& aLinks );
                                  
private:

    CMsgMailViewerContactMatcher( CAiwServiceHandler& aServiceHandler );
    void ConstructL();
    
private: // data

    /// AIW service handler reference
    CAiwServiceHandler& iServiceHandler;

    TPurpose iPurpose;
    
    TCallBack iCallBack;
    
    /// Own: contact matcher
    CContactMatcher* iContactMatcher;
    
    /// Own: contact link array
    CVPbkContactLinkArray* iContactLinks;
    
    /// Own: 
    CVPbkContactLinkArray* iChangedLinks;
    
    /// Own, but self-destructing
    CAknWaitDialog* iWaitDialog;
    
    CIdle* iIdle;
    };

#endif // C_CMSGMAILVIEWERCONTACTMATCHER_H
