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
* Description:   Header file of msgchecknames.cpp.
*                Includes alias name matching functions.
*
*/




#ifndef CMSGCHECKNAMES_H
#define CMSGCHECKNAMES_H

//  INCLUDES
#include <cntdb.h>                      //CheckNames
#include <CPbkContactEngine.h>          //CheckNames
#include <MsgRecipientItem.h>           //CheckNames
#include <AiwServiceHandler.h>          // for MAiwNotifyCallback
#include <contactmatcher.h>
#include <CVPbkContactLinkArray.h>
#include <akninputblock.h>
#include <MPbk2StoreConfigurationObserver.h>

// Constants
const TInt KMaxRecipientLength = 100;
// FORWARD DECLARATIONS
class CContactDatabase;
class CContactTextDef;
class CPbk2StoreConfiguration;

// CLASS DECLARATION

/**
* Class, which provides check names functionality in message editors.
*
* @lib MsgEditorAppui.lib
* @since 2.0
*/
class CMsgCheckNames : public CBase, 
                       public MAiwNotifyCallback,                       
                       public MIdleFindObserver,
                       public MAknInputBlockCancelHandler,
                       public MPbk2StoreConfigurationObserver
    {
    public:  // Constructors and destructor

        enum TMsgAddressSelectType
    		{
    		EMsgTypeSms,
            EMsgTypeMms,
            EMsgTypeMail,
            EMsgTypeAll
    		};
		
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMsgCheckNames* NewL();
        
        /**
        * Two-phased constructor. 
        * Leaves object into cleanup stack.
        */
	    IMPORT_C static CMsgCheckNames* NewLC();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CMsgCheckNames();

    public: // New functions
    
        /**
        * Finds the contact items that match the given search string, checks if they
        * are valid and if there are more than one valid matches shows them in a dialog
        * where user can select one or more of the contacts. This function should be
        * used when virtual phonebook is in use.
        *
        * @since 3.1
        *
        * @param aText - contains the search descriptor
        *        aAddressSelectType - determines which one of 
        *                             single selection dialogs provided by phonebook to use.
        *        aRecipientArray - the array to be handled
        *        aIndex - current index of the array
        *
        * @return ETrue if contact was found, otherwise EFalse
        */

        IMPORT_C TBool FindAndCheckByNameL( const TDesC& aText, 
                                            TMsgAddressSelectType aAddressSelectType,
                                            CMsgRecipientArray& aRecipientArray, 
                                            TInt aIndex ); 
        
        /**
        * Fetches the recipients for the editors while hiding the asynchronous AIW handling
        * and the related parameters
        *
        * @since 3.1
        *
        * @param  aRecipientList - is the array that will be filled for the caller
        *         aAddressSelectType - defines the addresstype for selection
        */            
        IMPORT_C void FetchRecipientsL( CMsgRecipientArray& aRecipientList, 
                                        TMsgAddressSelectType aAddressSelectType );

        /**
        * Fetches the recipients for the editors while hiding the asynchronous AIW handling
        * and the related parameters
        *
        * @since 3.2
        *
        * @param aRecipientList - is the array that will be filled for the caller
        *        aAddressSelectType - defines the addresstype for selection
        */         
        IMPORT_C void FetchRecipientsL( CVPbkContactLinkArray& aContactLinkArray,
                            TMsgAddressSelectType aAddressSelectType );
         /**
        * Getalia from ContactDb based on given address.
        *        
        * @since 5.0
        * @param aAddress address (pure real address). 
        * @param aAlias generated alias name (OUT). 
        * @param aMaxLength maximum length of the alias
        */         
 
        IMPORT_C void GetAliasL(const TDesC& aNumber,TDes& aAlias,TInt aMaxLength = KMaxRecipientLength);
                                            
        /**
        * Returns the contact matcher.
        *
        * @since 3.2
        *
        * @return reference to contact matcher.
        */        
        inline CContactMatcher& ContactMatcher() const;
        
    protected:
    
        /**
        * Defines the search fields to be used in the search
        *
        * @since 3.1
        *
        * @param aOwnList - the fields will be added here        
        */
        void DefineSearchFieldsL( CVPbkFieldTypeRefsList& aOwnList );
    
        /**
        * Executes the multiple entry fetch. 
        *
        * @since 3.1
        *
        * @param aData - is the selection data profiling the fetch
        */
        void ExecuteMultipleEntryFetchL( TAiwMultipleEntrySelectionDataV2 aData );
        
        
        /**
        * Executes the multiple entry fetch. 
        *
        * @since 3.1
        *
        * @param aData - is the selection data profiling the fetch
        */            
        void ExecuteSingleItemFetchL( TAiwSingleItemSelectionDataV1 aData );

        /**
        * Puts the found contacts to the given array
        *
        * @since 3.1
        *
        * @param aRecipientArray - the array to which the recipients will be added
        *        aIndex - index of the array, this is where the adding should start
        */
        void PutContactsToArrayL( CMsgRecipientArray& aRecipientArray, TInt aIndex );
      
        /**
        * From MAiwNotifyCallback
        */
        TInt HandleNotifyL( TInt aCmdId, 
                            TInt aEventId,
                            CAiwGenericParamList& aEventParamList,
                            const CAiwGenericParamList& aInParamList );

        /**
        * Converts from TMsgAddressSelectType into TAiwAddressSelectType.
        * Once pbk2 headers are included to builds permanently, this could be removed
        *
        * @since 3.1
        *
        */                
        TAiwAddressSelectType ConvertToPbk2Type( TMsgAddressSelectType aAddressSelectType ); 
                                        
        /**
        * Executes the multiple item fetch. 
        * This includes multiple entry and single item fetches.
        *
        * @since 3.2
        *
        * @param aData - is the selection data profiling the fetch
        */
        void ExecuteMultipleItemFetchL( TAiwMultipleItemSelectionDataV1& aData );  

    public:        
        
        /**
        * From MAknInputBlockCancelHandler. See AknInputBlock.h
        *
        * @since S60 3.2
        */        
        void AknInputBlockCancel();
        
        /**
         * From MPbk2StoreConfigurationObserver Called when the store configuration changes.
         */
        void ConfigurationChanged();

        /**
         * From MPbk2StoreConfigurationObserver  Called when the configuration changed event
         * has been delivered to all observers.
         */
        void ConfigurationChangedComplete();

    protected:  // Functions from base classes
        
        /**
        * From MIdleFindObserver
        *
        * This callback function is called by <code>CIdle::RunL()</code>
        * when nothing of a higher priority can be scheduled.
        * It is called for every 16 items searched to give the application a chance
        * to update its search status.
        * Implementation puts the results of the search into iFoundContacts.
        */
        void IdleFindCallback();
      
    private:

        /**
        * C++ default constructor.
        */
        CMsgCheckNames();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();
        
        /**
        * Custom cleanup item.
        */
		static void CleanupResetAndDestroy(TAny* newItem);
        
        /**
        * Slits the find string.
        */
        CDesCArray* SplitFindStringL( const TDesC& aFindString );

        /**
        * Determines whether advanced match algorithm is needed.
        */
        TBool UseAdvancedMatch( const TDesC& aFindString );

        /**
        * Performs advanced match algorithm,
        */
        void PerformAdvancedMatchL( const TDesC& aFindString,
                                    CVPbkFieldTypeRefsList* aFieldTypes );
                                                                        
        /**
        * Sets default priorities for given address type.
        */
        void SetDefaultPrioritiesL( TAiwAddressSelectType aAddressType, 
                                    RVPbkContactFieldDefaultPriorities& aPriorities ) const;
        /**
        * Performs configuration change handling.
        */
        void DoConfigurationChangedCompleteL();
        
    private:    // Data

        CVPbkContactLinkArray*     iFoundContacts; // Selected contact ids
        CContactMatcher*           iCommonUtils; //the wrapper
        CAiwServiceHandler*        iServiceHandler; //aiw service handler
        TBool                      iUseAdvancedMatch;
        CActiveSchedulerWait       iWait;          // Wait loop for active wait.
        TBool                      iShutdown;
        
        CPbk2StoreConfiguration*   iStoreConfiguration;
    };

#include <MsgCheckNames.inl>

#endif      // CMSGCHECKNAMES_H   
            
// End of File
