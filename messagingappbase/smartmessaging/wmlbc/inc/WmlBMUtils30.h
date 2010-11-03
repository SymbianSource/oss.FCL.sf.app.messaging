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
*     Class offers static utility functions for CWmlBMSubItem.
*
*/




#ifndef WMLBMUTILS_H
#define WMLBMUTILS_H

// INCLUDES
#include <favouritesdb.h>

// DATA TYPES

enum TWmlBookmarkFieldIndex
    {
    EBookmarkNameFieldIndex = 0,
    EBookmarkAddressFieldIndex
    };

// FORWARD DECLARATIONS

class CFavouritesItem;
class CFavouritesItemList;

class CWmlBMSubItem;


// CLASS DECLARATION

/**
* Class offers utility functions for CWmlBMSubItem-class.
* Contains only static methods, which are declared private.
* CWmlBMSubItem-class is declared as a friend class so it
* may access the private methods.
*/
NONSHARABLE_CLASS( WmlBMUtils )
    {

    private: // constructor

        /**
        * Constructor.
        */
        WmlBMUtils(){};

    private: // Methods available only to the friend classes.

        /**
        * Actually store the item to the favourites db.
        * Uniqueness of the name must be quaranteed before
        * calling this method.
        * @param aDb reference to the bookmark db object.
        * @param aItem reference to the favourites item.
        * @return commsdb error code.
        */
        static TInt DoStoreL(RFavouritesDb& aDb, CFavouritesItem& aItem);

        /**
        * Check if the given name already exists.
        * @aName name descriptor.
        * @aItemList list of items among the search is done.
        * @return index of the found item in the list or
        * KErrNone if the item was not found.
        */
        static TInt Exists( const TDesC& aName,
            const CFavouritesItemList& aItemList );

        /**
        * Removes the item from the database.
        * @param aDb reference to the favourites database.
        * @param aUid uid of the item, which is to be removed.
        */
        //static void RemoveL( RFavouritesDb& aDb, const TUint32 aUid );

        /**
        * Create the item list qualifying the supplied name criteria.
        * @param aDb bookmark db where the item are searched.
        * @return reference to the favourites item list object, which is
        * pushed to the cleanupstack.
        */
        static CFavouritesItemList* CreateItemListLC( RFavouritesDb& aDb);

    private: // friend class declaration.

        friend class CWmlBMSubItem;
    };

#endif // WMLBMUTILS_H

//  End of File
