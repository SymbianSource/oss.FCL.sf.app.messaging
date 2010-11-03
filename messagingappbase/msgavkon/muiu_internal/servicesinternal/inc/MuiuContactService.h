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
* Description:   Calls voice call or VoIP call to either of addresses provided. 
*  CMuiuContactService IS NOT YET READY. IMPLEMENTATION IS MISSING. DO NOT USE.
*
*/


#ifndef MUIUCONTACTSERVICE_H
#define MUIUCONTACTSERVICE_H

#include <e32base.h> // CBase
#include <muiumsvuiserviceutilitiesinternal.h>


class CMuiuContactAddressMatcher;
class CMsgVoIPExtension;
class CEikonEnv;
class CItemFinder;
/**
 *  VoIP call:
 *  Calls voice call or VoIP call to either of addresses provided. 
 *  VoIP call is performed, if VoIP is preferred call method
 *  and VoIP profiles exists.
 *  VoIP call is possible to perform using either phone number or email address.
 *  
 *  Voice call: If VoIP is not possible...
 *  ...and phone number is supplied, voice call is performed.
 *  ...and email address is supplied, email address
 *  is used to search for contact. The API searches for first contact mathing 
 *  the address. Other contacts are ignored. If contact has several addresses 
 *  (phone numbers), AIW is used to let user select address, which will be used.
 *  
 *  If contact with phone number could not be found matching the supplied 
 *  emails address, nothing takes place.
 *
 *  @lib muiu_internal.lib
 *  @since S60 v3.2
 */
 
class CMuiuContactService : public CBase
    {
public:
    enum TMuiuPreferredCallType
            {
            EMuiuCallTypeNone = 0,
            EMuiuCallTypeVoice,
            EMuiuCallTypeVoip
            };        
    
public: 

    /**
     * NewL
     * @param aVoipExtension 
     * @return self
     * @since S60 v3.2
     */
    IMPORT_C static CMuiuContactService* NewL( 
        CEikonEnv&          aEnv,
        CMsgVoIPExtension*  aVoipExtension = NULL );
        
    /// Destructor
    IMPORT_C virtual ~CMuiuContactService();

    /**
    * Returns information whether VOIP calls are supported in this build/device.
    * @return ETrue: supported. EFalse: not supported
    * @since S60 v3.2
    */
    IMPORT_C TBool IsVoipSupported() const;    

    /**
    * Returns preferred call type to be tried. Function can be called
    * immediately after construction. 
    * @return Preferred call type 
    * @since S60 v3.2
    */
    IMPORT_C TMuiuPreferredCallType PreferredCallTypeL() const;    

    /**
     * Sets primary (usually sender) address.
     * Setting address resets earlier found address and name.
     * @param aAddress primary address
     * @since S60 v3.2
     */    
    IMPORT_C void SetAddress( const TDesC& aAddress );

    /**
     * Sets contact name of primary address, if known.
     * Setting alias resets earlier found address and name.
     * @param aAddress primary address
     * @since S60 v3.2
     */    
    IMPORT_C void SetName( const TDesC& aName );

    /**
    * Calls to primary address. Operation is synchronous. Secondary address is not 
    * searched from phonebook. Function call resets previous results.
    * Earlier found address and alias are utilised, it they are still available. 
    * @return ETrue: if contact was searched from phonebook.
    * @since S60 v3.2
    */
    IMPORT_C TBool CallL( );    

    /**
    * Call to primary or secondary address. Secondary address is preferred to primary
    * address, if it exists. Operation is synchronous. Secondary address is not 
    * searched from phonebook.
    * @param aSecondaryAddress secondary address. 
    * @return ETrue: if contact was searched from phonebook.
    * @since S60 v3.2
    */
    IMPORT_C TBool CallL( const TDesC& aSecondaryAddress );    

    /**
    * Call to primary or secondary address. Secondary address is preferred to primary
    * address, if it exists. Operation is synchronous. Secondary address is not searched
    * from phonebook.
    * @param aSecondaryItem Function retrieves current item from CItemFinder and uses 
    *         its address. 
    * @return ETrue: if primary address was searched from phonebook.
    * @since S60 v3.2
    */
    IMPORT_C TBool CallL( CItemFinder* aSecondaryItem );    

    /**
    * Method returns information whether contact with phone number was found.
    * @return ETrue: contact found. Otherwise not found or search into
    *         phonebook was not even done.
    * @since S60 v3.2
    */
    IMPORT_C TBool IsContactFound() const;    
    
    /**
    * If contact search was successful, method to read phone number of matched contact.
    * Returns NULL if contact was not found or contact had not phone number
    * @return match result, ownership is transferred to caller.
    * @since S60 v3.2
    */
    IMPORT_C TPtrC FoundContactAddressL() const;    
    
    /**
    * If contact search was successful, method to read phone number of matched contact.
    * Returns NULL if contact was not found or contact had not phone number
    * @return match name, ownership is transferred to caller
    * @since S60 v3.2
    */
    IMPORT_C TPtrC FoundContactNameL() const;

private: // implementation  

    // Callback function of TCallback
    static TInt MatchDone( TAny* aCallService );
                                  
private:

    CMuiuContactService( CEikonEnv&         aEnv ,
                         CMsgVoIPExtension* aVoipExtension );
                         
    void ConstructL();
        
private: // data
       
    TPtrC                       iPrimaryAddress;
    TPtrC                       iPrimaryName;
    TPtrC                       iSecondaryAddress;
        
    CEikonEnv&                  iEikEnv;
    
    /// Management of VoIP data
    CMsgVoIPExtension*          iVoipExtension;
    
    CMuiuContactAddressMatcher* iContactAddressMatcher;  
    HBufC*                      iMatchAddress;
    HBufC*                      iMatchName;
    CActiveSchedulerWait        iWait;
    };

#endif // MUIUCONTACTSERVICE_H
