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



// INCLUDE FILES

#include "WmlBMUtils30.h"
#include <FavouritesItemList.h>         // CFavouritesItemList         

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// WmlBMUtils::DoStoreL
// ---------------------------------------------------------
//
TInt WmlBMUtils::DoStoreL(RFavouritesDb& aDb, CFavouritesItem& aItem)
    {
    return aDb.Add(aItem, ETrue);
    }

// ---------------------------------------------------------
// WmlBMUtils::Exists
// ---------------------------------------------------------
//
TInt WmlBMUtils::Exists( const TDesC& aName, 
            const CFavouritesItemList& aItemList )
    {
    TInt count = aItemList.Count();
    for (TInt i = 0; i < count; i++)
        {
        if (aItemList.At(i)->Name().CompareF(aName) == 0)
            {
            return i;
            }
        }
    return KErrNotFound;
    }

/*// ---------------------------------------------------------
// WmlBMUtils::RemoveL
// ---------------------------------------------------------
//
void WmlBMUtils::RemoveL( RFavouritesDb& aDb, const TUint32 aUid )
    {
    User::LeaveIfError( aDb.Delete( aUid ) );
    }
*/
// ---------------------------------------------------------
// WmlBMUtils::CreateItemList2LC
// ---------------------------------------------------------
//
CFavouritesItemList* WmlBMUtils::CreateItemListLC( 
    RFavouritesDb& aDb)
    {
    CFavouritesItemList* itemList = new (ELeave) CFavouritesItemList;
    CleanupStack::PushL( itemList );
    User::LeaveIfError( 
    	aDb.GetAll(*itemList, KFavouritesNullUid, 
        		   CFavouritesItem::EItem)
        		   );
    return itemList;
    }

// End of file
