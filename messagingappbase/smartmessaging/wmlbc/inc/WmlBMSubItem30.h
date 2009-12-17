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
*     Class includes information about one bookmark. 
*     It has handle to Bookmark engine for adding bookmark.
*
*/



#ifndef CWMLBMSUBITEM_H
#define CWMLBMSUBITEM_H

// INCLUDES
#include "WmlSubItemBase.h"

#include <FavouritesDb.h>

// FORWARD DECLARATIONS

class CFavouritesItem;
class CFavouritesItemList;

// CLASS DECLARATION

/**
* CWmlBMSubItem holds information of a one bookmark item.
*/
NONSHARABLE_CLASS( CWmlBMSubItem ) : public CWmlSubItemBase
    {

    public: // construction / destruction

        /**
        * First stage constructor.
        */
        static CWmlBMSubItem* NewL();

        /**
        * Destructor.
        */  
        ~CWmlBMSubItem();

    public:  //from CWmlSubItemBase

        /**
        * Adds value for bookmark field. 
        * @param aFieldValue Value of the field to be added.
        * @param aFieldEnum OTA parser specific enum of the field to add.
        * @exception Leaves if invalid value or field is being added.
        */ 
        void AddFieldL( const TDesC& aFieldValue, const TInt aFieldEnum );

        /**
        * Returns the count of fields in this item.
        * @return Number of fields. 
        */
        TInt FieldCount() const;

        /**
        * Interface for getting the label and value of a field at 
        * certain index.
        * @param aIndex Pointer to field which data is to be retrieved.
        *               0 <= aIndex < GetFieldCount()
        * @return reference to a class holding both label and value of a field.
        * @exception Leaves if index is out of range. 
        */
        CMsgNameValue* GetFieldDataAtLC( const TInt aIndex );

        /**
        * Check the item validity.
        * @return ETrue if the item is valid, EFalse if it's not.
        */
        TBool IsValidL();

    private: // from CWmlSubItemBase

        /**
        * Initialize all the member variable to their initial state.
        * So if leave occurs nothing about the aborted operation is
        * not saved.
        */
        void DoInitialize();

        /**
        * @see WmlSubItemBase::NameMaxLength
        */
		TInt NameMaxLength();

        /**
        * @see WmlSubItemBase::NameLC
        */
        HBufC* NameLC();
        
        /**
        * @see WmlSubItemBase::SetNameL
        */
        void SetNameL( const TDesC& aName );
        
        /**
        * @See WmlSubItemBase::DefaultNameLC
        */
        HBufC* DefaultNameLC();

		/**
        * @See WmlSubItemBase::DoQueryNewNameLC
        */
		HBufC* DoQueryNewNameLC( const TDesC& aName );

        /**
        * @See WmlSubItemBase::DoRenameQueryL
        */
		TBool DoRenameQueryL( const TDesC& aName );

        /**
        * @See WmlSubItemBase::IsNameValidL
        */
        TBool IsNameValidL( const TDesC& aName, TBool aUpdateList );
        
        /**
        * @See WmlSubItemBase::PreStoreL
        */        
        void PreStoreL( const TBool aShowPreferredQuery );
        
        /**
        * @See WmlSubItemBase::StoreL
        */
        void StoreL();
        
        /**
        * @See WmlSubItemBase::PostStoreL
        */
        void PostStoreL( const TBool aShowNote, 
            const TInt aPreferredInformation, 
            const TBool aCreateBookmarkIfNotSetAsDefault );

    private: // new

        /**
        * Does the construction.
        */        
        void ConstructL();

        /**
        * Get the reference to the bookmark database.
        * If the database is not already created this 
        * creates one.
        * @return a reference to the bookmark database.
        */
        RFavouritesDb& BookmarkDb2L();

    private: // data

        /**
        * Item used for internal storage and for adding bookmark to 
        * Favourites Database.
        */ 
        CFavouritesItem* iBMItem;

        /**
        * Bookmark database. Don't use this directly,
        * use BookmarkDbL method instead.
        */
        RFavouritesDb iBookmarkDb;

		/**
        * Bookmark database session.
        */
        RFavouritesSession iSession;

        /**
        * Indicates if this bookmark is valid.
        * Bookmark is valid when it has at least url defined.
        */
        TBool iIsValid;
		
		/**
		*
		*/
		CFavouritesItemList* iItemList;

    private: // friend declarations

		/// Let the asyncronousitemsaver to access saving methods.
		friend class CWmlAsyncronousItemSaver;

		/// For directly saving bookmark.
		friend class CWmlAPSubItem;
    };

#endif // CWMLBMSUBITEM_H

// End of file
