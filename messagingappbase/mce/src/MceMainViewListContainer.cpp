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
*     Derived from CMceMainViewListContainer.
*
*/



// INCLUDE FILES
#include <aknlists.h>
#include <msvids.h>

#include <bldvariant.hrh>


#include <mce.rsg>
#include "MceMainViewListContainer.h"
#include "MceMainViewListItemArray.h"
#include "MceIds.hrh"
#include "MceIds.h"

#include <csxhelp/mce.hlp.hrh>




// ================= MEMBER FUNCTIONS =======================


CMceMainViewListContainer::CMceMainViewListContainer()
    {
    }

CMceMainViewListContainer::~CMceMainViewListContainer()
    {
    delete iListBox;
    delete iListItems;
    }

CMceMainViewListContainer* CMceMainViewListContainer::NewL(
    const TRect& aRect,
    CMsvSessionPtr aSession,
    CMceBitmapResolver& aBitmapResolver,
    //TODO:
    TMsvId /*aFolderId*/,
    //TODO
    TMceListType /*aListType*/ )
    {
    CMceMainViewListContainer* self = new (ELeave)
        CMceMainViewListContainer();
    CleanupStack::PushL(self);
    self->ConstructL( aRect, aSession, aBitmapResolver );
    CleanupStack::Pop(); // self
    return self;
    }



// ----------------------------------------------------
// CMceMainViewListContainer::GetHelpContext
// returns helpcontext as aContext
// ----------------------------------------------------
void CMceMainViewListContainer::GetHelpContext
        (TCoeHelpContext& aContext) const
    {
    aContext.iMajor = KMceApplicationUid;
    aContext.iContext = KMCE_HLP_MESSAGING;
    }

void CMceMainViewListContainer::ConstructL(
    const TRect& aRect,
    CMsvSessionPtr aSession,
    CMceBitmapResolver& aBitmapResolver )
    {
    CreateWindowL();
    SetRect( aRect );

    iListBox = new (ELeave) CAknDoubleLargeStyleListBox ;
//    CEikTextListBox* listBox = new (ELeave) CAknSingleLargeStyleListBox;
    iListBox->ConstructL( this, EAknListBoxSelectionList );

    iListItems = CMceMainViewListItemArray::NewL(
        aSession, KMsvRootIndexEntryId, EMceListTypeMainView, aBitmapResolver );

    iListBox->Model()->SetItemTextArray( iListItems );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    //iSelectedEntries = new (ELeave) CMsvEntrySelection;
    }

// ----------------------------------------------------
// CMceMainViewListContainer::CountComponentControls
// ----------------------------------------------------
TInt CMceMainViewListContainer::CountComponentControls() const
    {
    return 1; // always only listbox
    }

// ----------------------------------------------------
// CMceMainViewListContainer::ComponentControl
// ----------------------------------------------------
CCoeControl* CMceMainViewListContainer::ComponentControl(TInt /*aIndex*/) const
    {
    return iListBox;
    }

// ----------------------------------------------------
// CMceMainViewListContainer::OfferKeyEventL
// ----------------------------------------------------
TKeyResponse CMceMainViewListContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    if ( aType == EEventKeyUp)
        {
        SetMskL();
        }
    if ( iListBox )
        {
        return iListBox->OfferKeyEventL(aKeyEvent, aType);
        }
    else
        {
        return EKeyWasConsumed;
        }
    }

// ----------------------------------------------------
// CMceMainViewListContainer::SizeChanged
// ----------------------------------------------------
void CMceMainViewListContainer::SizeChanged()
    {
    if (iListBox)
        {
        iListBox->SetRect( Rect() );
        }
    }


// ----------------------------------------------------
// CMceMainViewListContainer::CurrentItemIndex
// ----------------------------------------------------
TInt CMceMainViewListContainer::CurrentItemIndex() const
    {
    return iListBox->CurrentItemIndex();
    }

// ----------------------------------------------------
// CMceMainViewListContainer::CurrentItemId
// ----------------------------------------------------
TMsvId CMceMainViewListContainer::CurrentItemId() const
    {
    return iListItems->ItemId( CurrentItemIndex() );
    }

// ----------------------------------------------------
// CMceMainViewListContainer::CurrentItemListItem
// ----------------------------------------------------
TMceListItem CMceMainViewListContainer::CurrentItemListItem() const
    {
    return iListItems->Item( CurrentItemIndex() );
    }

// ----------------------------------------------------
// CMceMainViewListContainer::ItemIndex
// ----------------------------------------------------
TInt CMceMainViewListContainer::ItemIndex( TMsvId aId ) const
    {
    return iListItems->ItemIndex( aId );
    }

// ----------------------------------------------------
// CMceMainViewListContainer::FocusChanged
// ----------------------------------------------------
void CMceMainViewListContainer::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    if (iListBox)
        {
        iListBox->SetFocus(IsFocused());
        }
    }

// ----------------------------------------------------
// CMceMainViewListContainer::SetMskL
// ----------------------------------------------------
void CMceMainViewListContainer::SetMskL()
    {
    TInt currentItem = CurrentItemIndex();
    TInt resourceId = R_MCE_MSK_BUTTON_OPEN;
    
    if ( currentItem == ListItems()->ExtraItemIndex( KMceWriteMessageListIdValue ) )
        {
        resourceId = R_MCE_MSK_BUTTON_SELECT;
        }
        
    if ( resourceId != iCurrMskResource )
        {
        iCurrMskResource = resourceId;
        CEikButtonGroupContainer* cba;
        cba = CEikButtonGroupContainer::Current();
        if ( cba )
            {
            const TInt KMSKPosition = 3;
            cba->SetCommandL( KMSKPosition, iCurrMskResource );
            cba->DrawDeferred();
            }
        }
    }



//  End of File
