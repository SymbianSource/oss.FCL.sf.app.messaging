/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declarations of the CMceSelectEmailDialog
*
*/



#ifndef __MCESETTINGSEMAILSEL_H__
#define __MCESETTINGSEMAILSEL_H__

//  INCLUDES
#include <e32base.h>
#include <msvapi.h>
#include <MTMStore.h>
#include <TSendingCapabilities.h>
#include <e32hashtab.h> 

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  CMceSelectEmailDialog
*/
class CMceSettingsEmailSel : public CBase, public MMsvSessionObserver
{
    public:  // constructors and destructor
    
        /**
        * Two-phased constructor.
        */
        static CMceSettingsEmailSel* NewL();

		// Destructor
		~CMceSettingsEmailSel();
		
		        
        // from MMsvSessionObserver
        void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3 );

    public: // Methods

		IMPORT_C static TBool ShowSelectEmailDlgL(  TMsvId& aSelectedService, TUid& aMtmType,
		                        TSendingCapabilities aRequiredCapabilities = KCapabilitiesForAllServices );

    protected: // Methods
    
    private: // Methods
    
		// Fetch all email mtms    
		void FilterEmailServicesL(
	    	CDesCArrayFlat&         aListItems,
    		CArrayFix<TUid>&        aListItemUids );    
    
		// Constructor
		CMceSettingsEmailSel();    
        
        // 
		TBool ShowSelectDlgL( TMsvId& aSelectedService, 
		                      TUid& aMtmType,
		                      TSendingCapabilities aRequiredCapabilities );
        
        // Load the resource for the dll module        
        void LoadResourceL();
        
        // Unload the resource for the dll module        
        void UnLoadResource(); 
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        // validate capabilities
        TBool QueryCapabilitiesAndValidateAccountL( TUid aMtmUid );  
        
 //Functions introduced for CMail changes
        
        /**
        * Function to Append the Cmail Boxes to the mail list
        */
        void AppendCmailBoxesL(CArrayFix<TInt64 >* atimeItems, CMsvEntry* entry);
        
        /**
        * Function to Add the mail Boxes other than default to the mail list
        */
        void AddOtherEmailBoxesL( CDesCArrayFlat& aListItems, 
                                  CArrayFix<TUid>& aListItemUids, 
                                  CMsvEntry* aEntry);
        
        /**
        * Function to check if the email entry is healthy or not
        * returns ETrue if healthy
        */
        TBool IsMailBoxHealthyL(TMsvEntry entry);
        
        /**
        * Function to check if the entry is email entry or not
        * returns ETrue if email
        */
        TBool IsEmailEntryL(TMsvEntry entry);

        /**
        * Function to get and show all mailboxes with cmail also
        * 
        */
        TBool ShowCmailSelectDlgL( TMsvId& aSelectedService, 
                                   TUid& aMtmType,
                                   CDesCArrayFlat*         aListItems,
                                   CArrayFix<TUid>*        aListItemUids
                                   );

        /**
        * Function to check if the mail box entry is valid to be 
        * appended or not
        * returns ETrue if appendable
        */
        TBool IsMailBoxAppendable(TMsvEntry aEntry , TUid* aUid);

        /**
        * Function to check if the mail box entry is already added or not
        * returns EFalse if already added
        */
        TBool IfMailBoxAlreadyAdded(CDesCArrayFlat& aListItems, 
                                    CDesCArrayFlat* aCmaillist, 
                                    TMsvEntry aTentry);

        /**
        * Function to check if the mail box uid is already added or not
        * returns EFalse if already added
        */
        TBool IfMailBoxUidAlreadyAdded(CArrayFix<TUid>* aListItemsUids, 
                                       TMsvEntry aTentry);
    
    private: // Data

	CMsvSession* iMsvSessionPtr;    
	
    CMtmStore*              iMtmStore;
    
    TInt iResourceOffset;
    
    TSendingCapabilities iRequiredCapabilities;    

//Variables introduced for CMail changes
    
    /**
    * variable to know if CMail Feature is on or not
    */
    TBool iEmailFramework;
  
    /**
     * Array to hold details of Cmail Mailboxes  
    */
    CDesCArrayFlat* iCmaillistItems;
    
    /**
    * Hash table to hold the mapping of cmail boxes with its position in TMsvEntry
    */
    RHashMap<TInt64, TInt >* iCmailBoxes;
    
    /**
     * Array is used to map user selection to correct service in case of CMail
     */
    CArrayFix<TUid>* iCmaillistItemUids ;
    
    /*
    * Variable to hold the default email box index
    */
    TInt iDefaultIndex ;
    
    /*
     * Variable to hold the default email entry
    */ 
    TMsvEntry iTentryDef;
    
    /*
     * Variable to hold the mtm pluginid in case of platform email application registered
     */ 
    TInt iMtmPluginID;
    
};

#endif // __MCESETTINGSEMAILSEL_H__
