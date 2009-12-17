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
*     Container for main view.
*     Derived from CMceListContainer.
*
*/



#ifndef MCEMAINVIEWLISTCONTAINER_H
#define MCEMAINVIEWLISTCONTAINER_H

//  INCLUDES
#include <coecntrl.h> // CCoeControl
#include <msvapi.h>

#include "mcetemplate.h"
#include "MceUtils.h"
#include "MceListItem.h"

#include <bldvariant.hrh>

class CAknDoubleLargeStyleListBox;
class CMceMainViewListItemArray;
class CMceBitmapResolver;

// CLASS DECLARATION

/**
*  Container for main view.
*/
class  CMceMainViewListContainer :
    public CCoeControl
//    public CMceListContainer
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMceMainViewListContainer* NewL(
            const TRect& aRect,
            CMsvSessionPtr aSession,
            CMceBitmapResolver& aBitmapResolver,
            TMsvId aFolderId,
            TMceListType aListType = EMceListTypeMainView );

        /**
        * Destructor.
        */
        virtual ~CMceMainViewListContainer();

        /**
        * From CoeControl
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;
        
        /**
        * Returns pointer to listbox.
        */
        inline CAknDoubleLargeStyleListBox* ListBox() const;

        /**
        * Returns item array
        * @return Pointer to list item array
        */
        inline CMceMainViewListItemArray*  ListItems() const;
        
        /**
        * Returns list index of the current item
        * @return Index of the current item
        */
        TInt    CurrentItemIndex() const;
        
        /**
        * Tries to find item from the currently opened list and retuns
        * list index if found. KErrNotFound if not found.
        * @param aId: item id to be searched
        * @return List index of the item.
        */
        TInt ItemIndex( TMsvId aId ) const;
        
        /**
        * Sets MSK label according to a focused item
        * Since 3.2
        */
        void SetMskL();
        
        /**
        * Returns message id of the current (=selected) item
        * @return message id
        */
        TMsvId  CurrentItemId() const;
        
        /**
        * Returns mce's info of the current message
        * @return info
        */
        TMceListItem CurrentItemListItem() const;
        
        /**
        * From CCoeControl
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * From CCoeControl
        */
        void SizeChanged();

        /**
        * From CCoeControl
        */
        virtual TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

    protected:
        // From CCoeControl
        void FocusChanged(TDrawNow aDrawNow);

    private:
        /**
        * Constructor.
        */
        CMceMainViewListContainer();
        
        /**
        * Symbian OS constructor.
        */
        void ConstructL(
            const TRect& aRect,
            CMsvSessionPtr aSession,
            CMceBitmapResolver& aBitmapResolver );
            
    private: // Data
        CAknDoubleLargeStyleListBox* iListBox;
        CMceMainViewListItemArray*  iListItems;
        TInt                        iCurrMskResource;
    };

#include "MceMainViewListContainer.inl"

#endif

// End of file
