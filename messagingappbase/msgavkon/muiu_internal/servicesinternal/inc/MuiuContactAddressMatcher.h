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
* Description:   Searches contact using email address as search filter from address books.
*
*/


#ifndef MUIUCONTACTADDRESSMATCHER_H
#define MUIUCONTACTADDRESSMATCHER_H

#include <e32base.h> // CBase
#include <AiwCommon.h> // MAiwNotifyCallback

class CContactMatcher;
class CVPbkContactLinkArray;
class MVPbkStoreContact;
class TAiwSingleItemSelectionDataV3;
class CAiwServiceHandler;
class CVPbkFieldTypeSelector;
class CAknWaitDialog;
class CEikonEnv;

/**
 *  Find outs contact by using email address as search filter. 
 *  The API searches for first contact mathing the address.
 *  Other contacts are ignored. If contact has several addresses (phone numbers), 
 *  AIW is used to let user select address, which will be used.  
 *
 *  @lib muiu_internal.lib
 *  @since S60 v3.2
 */
class CMuiuContactAddressMatcher
: public CActive, MAiwNotifyCallback
    {
            
public:

    /**
     * NewL
     * @since S60 v3.2
     * @param aServiceHandler for attaching AIW interest
     * @return self
     */
    static CMuiuContactAddressMatcher* NewL( 
        CEikonEnv& aEikonEnv);    

    virtual ~CMuiuContactAddressMatcher();

    /**
     * Search contact that have specified email address.
     * @param aMailAddress searched email address.
     * @param aCallBack call-back function to call when match is ready
     * @return ETrue, address was email address: search begins. 
     *                aCallBack will be called.
     *         EFalse, address was either phone number of empty. 
     *                aCallback will not called
     * @since S60 v3.2
     */    
    TBool FindContactL( const TDesC& aMailAddress,
                       TCallBack aCallBack );
    
    /**
    * Method to read phone number of matched contact, may be called 
    * after clients callback method is called. Returns NULL if address was not found.
    * @return match result, ownership is transferred to caller
    * @since S60 v3.2
    */
    HBufC* GetAddressL() const;    
    
    /**
    * Method to read contact name of matched contact, may be called
    * after clients callback method is called. Returns NULL if address was not found.
    * @return match name, ownership is transferred to caller
    * @since S60 v3.2
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

    void DeleteExcessMatchesL( CVPbkContactLinkArray& aLinks ) const;

    CVPbkFieldTypeSelector* CreateFilterLC( ) const;
    
    TAiwSingleItemSelectionDataV3 SelectionData(
        CVPbkFieldTypeSelector& aFilter ) const;
    
    void ExecuteSingleItemFetchL( TAiwSingleItemSelectionDataV3 aData,
                                  const CVPbkContactLinkArray& aLinks );
                                  
private:

    CMuiuContactAddressMatcher( CEikonEnv& aEikonEnv );
    void ConstructL();
    
private: // data

    /// AIW service handler reference
    CAiwServiceHandler*     iServiceHandler;
    
    TCallBack               iCallBack;
    
    /// Own: contact matcher
    CContactMatcher*        iContactMatcher;
    
    /// Own: contact link array
    CVPbkContactLinkArray*  iContactLinks;
    
    /// Own: 
    CVPbkContactLinkArray*  iChangedLinks;
    
    /// Own, but self-destructing
    CAknWaitDialog*         iWaitDialog;
    
    CIdle*                  iIdle;
    
    CEikonEnv&              iEikEnv;
    // 'handle' to muiu_internal.rsc esourse file
    TInt                    iResourceOffset;
    };

#endif // MUIUCONTACTADDRESSMATCHER_H
