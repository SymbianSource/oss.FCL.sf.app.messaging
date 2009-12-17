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
* Description:  
*     Base class for WML subitems.
*
*/



#ifndef CWMLSUBITEMBASE_H
#define CWMLSUBITEMBASE_H

// INCLUDES

#include <badesca.h>                // CDesCArray
#include "WMLBC.hrh"

// FORWARD DECLARATIONS

class CMsgNameValue;


// CLASS DECLARATION

/**
* Base class for WML subitems for WML Bio Control's internal use.
* Instances of this class may not be initialized. Class is intented 
* to be inherited.
*/
NONSHARABLE_CLASS( CWmlSubItemBase ) : public CBase
    {

    public: // Construction / Destruction
        
        /**
        * Destructor.
        * Deletes the member iName.
        */
        ~CWmlSubItemBase();

    public:

        /**
        * Contains the logic to store the item to the database. This
        * is template method, which calls the actual implemtation methods
        * in the subclass.
        * @param aShowNote indicates whether to show note after item has 
        * been stored. By default note is shown. 
        * Leaves if the database operation fails.  
        */  
        TBool SaveItemL( TBool aShowNote = ETrue, 
            TBool aShowPreferredQuery = ETrue,
            TBool aPreferredInformation = EFalse,
            TBool aCreateBookmarkIfNotSetAsDefault = EFalse );

        /**
        * Get the item's data.
        * @return reference to an array containing items data.
        */
        CArrayPtrFlat<CMsgNameValue>* LabelsAndValuesLC();  

    public: // virtual methods, which must be implemented in subclasses.  
        
        /**
        * Interface for adding value for certain field. Note: The values 
        * might be converted to the internal data structure and can be 
        * retrieved in viewable format later.
        * @param aFieldValue Value of the field to be added.  
        * @param aFieldEnum OTA parser specific enum of the field to add.
        * @exception Leaves if invalid value or field is being added.
        */ 
        virtual void AddFieldL( 
            const TDesC& aFieldValue, 
            const TInt aFieldEnum ) = 0;

        /**
        * Interface for getting the count of rows needed to display 
        * data in detail view.
        * @return Number of fields in item, aka rows needed in detail
        *         view. 
        */
        virtual TInt FieldCount() const = 0;

        /**
        * Interface for getting the label and value of a field in at 
        * certain index. The texts returned are localized. The texts
        * are stored in HBufCPair object which is created and pushed to
        * cleanup stack.
        * @param aIndex Pointer to field which data is to be retrieved.
        *               0 <= aIndex < GetFieldCount()
        * @return pointer to a class holding both label and value of a field.
        * @exception Leaves if index is out of range. 
        */
        virtual CMsgNameValue* GetFieldDataAtLC( const TInt aIndex ) = 0;
            
        /**
        * Check the item validity.
        * @return ETrue if the item is valid, EFalse if it's not.
        */
        virtual TBool IsValidL() = 0;

    private: // private virtual methods called from SaveItemL-method.

        /**
        * Initialize all the member variable to their initial state.
        * So if leave occurs nothing about the aborted operation is
        * not saved.
        */
        virtual void DoInitialize() = 0;

		/**
		* Get the maximum lenght of the name.
		* @return Max length of the name for this sub item type.
		*/
		virtual TInt NameMaxLength() = 0;

        /**
        * Get the item's name.
        * @return Pointer to the HBufC, which is pushed to the cleanupstack.
        * Name may also be empty.
        */
        virtual HBufC* NameLC() = 0;

        /**
        * Set the item's name.
        * @param aName New name of the item.
        */
        virtual void SetNameL( const TDesC& aName ) = 0;

        /**
        * Get the item's default name.
        * @return Pointer to the HBufC, which is in cleanupstack.
        */
        virtual HBufC* DefaultNameLC() = 0;
    
		/**
		* Display new name query for this type of item.
		* @param aName original name to be displayed in query.
		* @return descriptor got as the query result.
		*/
		virtual HBufC* DoQueryNewNameLC( const TDesC& aName ) = 0;

		/**
		* Display rename query for this type of item.
		* @param aName original name to be displayed in query.
		* @return ETrue if user renamed, EFalse if cancel was pressed. 
		*/
		virtual TBool DoRenameQueryL( const TDesC& aName ) = 0;

        /*
        * Performs the name uniqueness test.
        * @param aName name to check.
		* @param aUpdateList optimizes the checking.
        * @return ETrue if name is unique in the used database.
        * EFalse if the name already exists in the database.
        */
        virtual TBool IsNameValidL( const TDesC& aName, 
			TBool aUpdateList ) = 0;

        /**
        * Does the operations needed before the item is actually 
        * store to the database.
        */
        virtual void PreStoreL( const TBool aShowPreferredQuery ) = 0;

        /**
        * Actually store the item to the database.
        * All the unique checks MUST be done before calling
        * this method. Leaves if the database operation fails.
        */
        virtual void StoreL() = 0;

        /**
        * Does the operation needed after the item has been
        * SUCCESSFULLY stored to the database. This method must
        * not be called if database operation has failed.
        * @param aShowNote indicates if note is shown after store.
        */
        virtual void PostStoreL( const TBool aShowNote, const TInt aPreferredInformation, const TBool aCreateBookmarkIfNotSetAsDefault ) = 0;    

    private: // Methods, which are implemented in this class.

        /**
        * Set all the member variables as initial values.
        */ 
        void Initialize();

        /**
        * Check if the item has name.
        */
        TBool HasNameL();

        /**
        * Set the item's default name in iName.
        */
        void SetDefaultNameL();

        /*
        * Calls the item's IsNameValidL method by givin iName as a parameter.
        * @return ETrue if name is unique in the used database.
        * EFalse if the name already exists in the database.
        */
        TBool IsNameValidL( TBool aUpdateList = ETrue );

        /**
        * Calls the static implementation DoIncrementNameL giving the
        * object name member pointer as a parameter.
        * @see DoIncrementNameL
        */
        void IncrementNameL();

        /**
        * Static Implementation of the name increment.
        * Name increment rules are defined and implemented in CMsgBioControl.
        * @param aName pointer to the HBufC, which is modified.
		* @param aMaxLength maximum length of the descriptor to be grown.
        */
        static HBufC* DoIncrementNameL( const TDesC& aName, TInt aMaxLength );

        /**
        * Calls the static implementation DoQueryNewNameL giving the
        * object name member pointer as a parameter.
        * @see DoQueryNewNameL
        */
        void QueryNewNameL();

        /**
        * Calls the static implementation DoRenameQueryL.
        * @return ETrue if user chose to overwrite, EFalse otherwise.
        * @see DoRenameQueryL
        */
        TBool RenameQueryL();

    private:

        /**
        * Item's original name is stored and restored after storing 
        * Procedure.
        */
        HBufC* iOriginalName;

        /// Pointer to the item name, which is used to create unique name.
        HBufC* iName;
    };

#endif // CWMLSUBITEMBASE_H

// End of file
