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
*     List container user by message view.
*     Derived from CMceMessageListContainer.
*
*/



// INCLUDE FILES

#include "mceui.h"
#include "MceMessageListContainer.h"
#include "MceMessageListbox.h"
#include "MceMessageListView.h"
#include "MceMessageListItemArray.h"
#include "MceBitmapResolver.h"
#include "MceCommands.hrh"
#include "MceIds.hrh"
#include <mce.rsg>
#include "McePropertySubscriber.h"
#include <PtiEngine.h>
#include <AvkonInternalCRKeys.h> // KCRUidAvkon

#include <msvids.h>
#include <msvuids.h>      // KUidMsvMessageEntry

#include <bldvariant.hrh>

#include <csxhelp/mce.hlp.hrh>
#include <csxhelp/mbxs.hlp.hrh>
#include "MceIds.h"
#include <MuiuMsvUiServiceUtilities.h>

#include <commonphoneparser.h>
#include <featmgr.h>

// For Muiu split
#include <muiumsvuiserviceutilitiesinternal.h>

// CONSTANTS

const TInt KMceTextBufferLength        = 10; // text buffer length
const TInt KMceListContainerGranuality = 4;
// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// Constructor
// ----------------------------------------------------
CMceMessageListContainer::CMceMessageListContainer( 
    CMceMessageListView& aParentView ) 
    :
    CMceMessageListContainerBase( aParentView )
    {
    }

// ----------------------------------------------------
// Destructor
// ----------------------------------------------------
CMceMessageListContainer::~CMceMessageListContainer()
    {
    delete iT9Interface;
    delete iT9Timer;      
    
    if (iQwertyModeStatusSubscriber)
        {
        iQwertyModeStatusSubscriber->StopSubscribe();
        }
    iQwertyModeStatusProperty.Close();
    delete iQwertyModeStatusSubscriber;
    
    if ( iListBox )
        {
        // icon array is deleted by CMceUi class
        iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( NULL );
        }
    
    delete iListBox;
    delete iListItems;
    delete iSelectedEntries;
    }

// ----------------------------------------------------
// CMceMessageListContainer::NewL
// ----------------------------------------------------
CMceMessageListContainer* CMceMessageListContainer::NewL(
    const TRect& aRect,
    CMsvSessionPtr aSession,
    TMsvId aFolderId,
    CMceBitmapResolver& aBitmapResolver,
    CMceMessageListView& aParentView )
    {
    CMceMessageListContainer* self = new (ELeave) CMceMessageListContainer(
        aParentView );
    CleanupStack::PushL(self);
    self->ConstructL( aRect, aSession, aBitmapResolver, aFolderId );
    CleanupStack::Pop(); // self
    return self;
    }

// ----------------------------------------------------
// CMceMessageListContainer::ConstructL
// ----------------------------------------------------
void CMceMessageListContainer::ConstructL(
    const TRect& aRect,
    CMsvSessionPtr aSession,
    CMceBitmapResolver& aBitmapResolver,
    TMsvId aFolderId )
    {
    CreateWindowL();
    SetRect( aRect );

    iListBox = new (ELeave) CMceMessageListBox( *this );
    iListBox->ConstructL( this, EAknListBoxStylusMarkableList );
    iListBox->ItemDrawer()->SetItemMarkReverse( ETrue );
    iListBox->EnableMSKObserver( EFalse );

    iListItems = CMceMessageListItemArray::NewL(
        aSession, aFolderId, EMceListTypeMessage, aBitmapResolver );

    iListBox->Model()->SetItemTextArray( iListItems );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    
    iListBox->SetListBoxObserver( &iOwningView );

    iSelectedEntries = new (ELeave) CMsvEntrySelection;
    
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    iListBox->ItemDrawer()->FormattedCellData()
        ->SetIconArray( aBitmapResolver.IconArray() );
    
    iPreviousInputMethod = EFalse;
    
    User::LeaveIfError( iQwertyModeStatusProperty.Attach( 
        KCRUidAvkon, KAknQwertyInputModeActive ));
  
    HandleQwertyModeChangeNotification();
    iQwertyModeStatusSubscriber = CMcePropertySubscriber::NewL(
        TCallBack( QwertyModeChangeNotification, this ), iQwertyModeStatusProperty );

    iQwertyModeStatusSubscriber->SubscribeL();
    iDialerEvent = EFalse ;

    }
    
// ----------------------------------------------------
// CMceMessageListContainer::MceListId
// ----------------------------------------------------
TInt CMceMessageListContainer::MceListId() const
    {
    return EMceListTypeTwoRow;
    }
    

// ----------------------------------------------------
// CMceMessageListContainer::CountComponentControls
// ----------------------------------------------------
TInt CMceMessageListContainer::CountComponentControls() const
    {
    return 1; // always only listbox
    }

// ----------------------------------------------------
// CMceMessageListContainer::ComponentControl
// ----------------------------------------------------
CCoeControl* CMceMessageListContainer::ComponentControl(TInt /*aIndex*/) const
    {
    return iListBox;
    }

// ----------------------------------------------------
// CMceMessageListContainer::SetFolderL
// ----------------------------------------------------
void CMceMessageListContainer::SetFolderL( TMsvId aFolderId )
    {
    if ( iListItems )
        {
        iListItems->SetFolderL( aFolderId );
        }
    }
    
// ----------------------------------------------------
// CMceMessageListContainer::Count
// ----------------------------------------------------
TInt CMceMessageListContainer::Count() const
    {
    return iListItems->MdcaCount();
    }
    
// ----------------------------------------------------
// CMceMessageListContainer::MessageCount
// ----------------------------------------------------
TInt CMceMessageListContainer::MessageCount() const
    {
    return iListItems->MessageCount();
    }
    
// ----------------------------------------------------
// CMceMessageListContainer::FolderCount
// ----------------------------------------------------
TInt CMceMessageListContainer::FolderCount() const
    {
    return iListItems->FolderCount();
    }
    
// ----------------------------------------------------
// CMceMessageListContainer::CurrentFolderId
// ----------------------------------------------------
TMsvId CMceMessageListContainer::CurrentFolderId() const
    {
    TMsvId folderId = 0;
    if ( iListItems )
        {
        folderId = iListItems->FolderId();
        }
    return folderId;
    }

// ----------------------------------------------------
// CMceMessageListContainer::SetCurrentItemIdL
// ----------------------------------------------------
TBool CMceMessageListContainer::SetCurrentItemIdL( TMsvId aEntryId )
    {
    const TInt selectedIndex = ItemIndex( aEntryId );
    if ( selectedIndex != KErrNotFound )
        {
        iAnchorItemIndex = selectedIndex; 
        iAnchorItemId = aEntryId;
        
		//calulate the top item index
		TInt topIndex = iListBox->View()->CalcNewTopItemIndexSoItemIsVisible( selectedIndex );
		iListBox->SetTopItemIndex( topIndex );
		iListBox->SetCurrentItemIndex( selectedIndex );
		iListBox->UpdateScrollBarsL();
		iListBox->DrawDeferred();
//        iListBox->SetCurrentItemIndex( selectedIndex );
        }
    return (selectedIndex != KErrNotFound);
        
    }
    
// ----------------------------------------------------
// CMceMessageListContainer::RefreshListbox
// ----------------------------------------------------
void CMceMessageListContainer::RefreshListbox()
    {
    iListBox->DrawDeferred();     
    }

// ----------------------------------------------------
// CMceMessageListContainer::ItemCountChangedL
// ----------------------------------------------------
void CMceMessageListContainer::ItemCountChangedL( TBool aItemsAdded )
    {
    if ( aItemsAdded )
        {
        iListBox->HandleItemAdditionL();     
        }
    else
        {
        iListBox->HandleItemRemovalL();
        }
    }
    
    
// ----------------------------------------------------
// CMceMessageListContainer::FolderEntry
// ----------------------------------------------------
const TMsvEntry& CMceMessageListContainer::FolderEntry() const
    {
    return iListItems->FolderEntry();
    }

// ----------------------------------------------------
// CMceMessageListContainer::CurrentItemIndex
// ----------------------------------------------------
TInt CMceMessageListContainer::CurrentItemIndex() const
    {
    return iListBox->CurrentItemIndex();
    }

// ----------------------------------------------------
// CMceMessageListContainer::CurrentItemId
// ----------------------------------------------------
TMsvId CMceMessageListContainer::CurrentItemId() const
    {
    return iListItems->ItemId( CurrentItemIndex() );
    }

// ----------------------------------------------------
// CMceMessageListContainer::CurrentItemListItem
// ----------------------------------------------------
TMceListItem CMceMessageListContainer::CurrentItemListItem() const
    {
    return iListItems->Item( CurrentItemIndex() );
    }


// ----------------------------------------------------
// CMceMessageListContainer::CurrentItemSelectionL
// ----------------------------------------------------
CMsvEntrySelection* CMceMessageListContainer::CurrentItemSelectionL()
    {
    return iSelectedEntries->CopyL();
    }

// ----------------------------------------------------
// CMceMessageListContainer::CurrentItemSelectionRefreshL
// ----------------------------------------------------
CMsvEntrySelection* CMceMessageListContainer::CurrentItemSelectionRefreshL()
    {
    MarkItemSelectionL();
    return CurrentItemSelectionL();
    }

// ----------------------------------------------------
// CMceMessageListContainer::CurrentItemSelectionCount
// ----------------------------------------------------
TInt CMceMessageListContainer::CurrentItemSelectionCount() const
    {
    if ( iListItems->MdcaCount() == 0 )
        {
        return 0;
        }
    return Max( 1, iListBox->View()->SelectionIndexes()->Count() );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::SelectionCount
// ----------------------------------------------------
TInt CMceMessageListContainer::SelectionCount() const
    {
    return iListBox->View()->SelectionIndexes()->Count();
    }

// ----------------------------------------------------
// CMceMessageListContainer::ClearSelection
// ----------------------------------------------------
void CMceMessageListContainer::ClearSelection()
    {
    iSelectedEntries->Reset();
    iListBox->View()->ClearSelection();
    }

// ----------------------------------------------------
// CMceMessageListContainer::ItemIndex
// ----------------------------------------------------
TInt CMceMessageListContainer::ItemIndex( TMsvId aId ) const
    {
    return iListItems->ItemIndex( aId );
    }


// ----------------------------------------------------
// CMceMessageListContainer::MarkItemSelectionL
// ----------------------------------------------------
void CMceMessageListContainer::MarkItemSelectionL()
    {
    const CListBoxView::CSelectionIndexArray* selection =
        iListBox->View()->SelectionIndexes();
    TInt count = selection->Count();
    iSelectedEntries->Reset();
    if ( count != 0 )
        {
        for (TInt loop=0; loop < count; loop++ )
            {
            TMsvId id = iListItems->ItemId( (*selection)[loop] );
            if ( id != KErrNotFound )
                {
                iSelectedEntries->AppendL( id );
                }
            }
        }
    else
        {
        TMsvId id = CurrentItemId();
        if ( id != KErrNotFound )
            {
            // nothing in selection so add item under "cursor"
            iSelectedEntries->AppendL( id );
            }
        }
    }

// ----------------------------------------------------
// CMceMessageListContainer::AddItemToSelectionL
// ----------------------------------------------------
void CMceMessageListContainer::AddItemToSelectionL( TMsvId aEntryId )
    {
    if ( aEntryId > KMsvRootIndexEntryId )
        {
        iSelectedEntries->AppendL( aEntryId );
        }
    }
// ----------------------------------------------------
// CMceMessageListContainer::AddCurrentItemToSelectionL
// ----------------------------------------------------
void CMceMessageListContainer::AddCurrentItemToSelectionL()
    {
    iListBox->View()->SelectItemL( CurrentItemIndex() );
    AddItemToSelectionL( CurrentItemId() );
    }

// ----------------------------------------------------
// CMceMessageListContainer::RemoveItemFromSelection
// ----------------------------------------------------
void CMceMessageListContainer::RemoveItemFromSelection( TMsvId aEntryId )
    {
    TInt index = iSelectedEntries->Find( aEntryId );
    if ( index >= 0 )
        {
        iSelectedEntries->Delete( index );
        }
    }

// ----------------------------------------------------
// CMceMessageListContainer::RemoveCurrentItemFromSelection
// ----------------------------------------------------
void CMceMessageListContainer::RemoveCurrentItemFromSelection()
    {
    iListBox->View()->DeselectItem( CurrentItemIndex() );
    RemoveItemFromSelection( CurrentItemId() );
    }

// ----------------------------------------------------
// CMceMessageListContainer::RefreshSelectionIndexesL
// ----------------------------------------------------
void CMceMessageListContainer::RefreshSelectionIndexesL( TBool aForceUpdate )
    {
    const CListBoxView::CSelectionIndexArray* selection = 
        iListBox->View()->SelectionIndexes();

    if ( selection->Count() > 0 || aForceUpdate )
        {
        CArrayFixFlat<TInt>* newSelection = new ( ELeave )
            CArrayFixFlat<TInt>( KMceListContainerGranuality );
        CleanupStack::PushL( newSelection );
        const TInt count = iSelectedEntries->Count();
        for ( TInt loop = count; loop >0; loop--)
            {
            TInt index = iListItems->ItemIndex( (*iSelectedEntries)[loop-1] );
            if ( index > KErrNotFound )
                {
                newSelection->AppendL( index );
                }
            else
                {
                iSelectedEntries->Delete( loop-1 );
                }
            }
        iListBox->SetSelectionIndexesL( newSelection );
        CleanupStack::PopAndDestroy( newSelection );
        }
    
    }

// ----------------------------------------------------
// CMceMessageListContainer::AddAllToSelectionL
// ----------------------------------------------------
void CMceMessageListContainer::AddAllToSelectionL()
    {
    CArrayFixFlat<TInt>* indexArray = new( ELeave ) CArrayFixFlat<TInt>(
        KMceListContainerGranuality );
    CleanupStack::PushL( indexArray );
    iSelectedEntries->Reset();
    const TInt count = iListItems->MdcaCount();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        TMsvId id = iListItems->ItemId( loop );
        if ( !iListItems->IsItemFolder( id ) )
            {
            indexArray->AppendL( loop );
            iSelectedEntries->AppendL( id );
            }
        }
    iListBox->SetSelectionIndexesL( indexArray );
    CleanupStack::PopAndDestroy( indexArray );
    }
    

// ----------------------------------------------------
// CMceMessageListContainer::MarkAllReadMessagesL
// ----------------------------------------------------
void CMceMessageListContainer::MarkAllReadMessagesL()
    {
    CArrayFixFlat<TInt>* msgIndexArray = new( ELeave ) CArrayFixFlat<TInt>( 4 );
    CleanupStack::PushL( msgIndexArray );
    iSelectedEntries->Reset();
    iListBox->View()->ClearSelection();

    const TInt count = iListItems->MdcaCount();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        TMsvEntry entry;
        TMsvId entryId = iListItems->ItemId( loop );
        if ( iListItems->GetEntry( entryId, entry ) == KErrNone )
            {
            if ( entry.iType == KUidMsvMessageEntry &&
                 !entry.Unread() )
                {
                msgIndexArray->AppendL( loop );
                }
            }
        }
      
    if ( msgIndexArray->Count() > 0 )
        {
        iListBox->SetSelectionIndexesL( msgIndexArray );        
        iListBox->DrawDeferred();
        }    
    CleanupStack::PopAndDestroy( msgIndexArray );
    }


// ----------------------------------------------------
// CMceMessageListContainer::IsItemSelected
// ----------------------------------------------------
TBool CMceMessageListContainer::IsItemSelected( TMsvId aItemId )
    {
    // Find selected items
    const CListBoxView::CSelectionIndexArray* items =
         iListBox->SelectionIndexes();
    TInt itemIndex = iListItems->ItemIndex( aItemId );
    TKeyArrayFix key(0, ECmpTInt);
    TInt pos;

    // Is item in selection array
    if ( items->Find(itemIndex, key, pos) != KErrNone )
        {
        return EFalse;
        }
    else
        {
        return ETrue;        
        }
    }


// ----------------------------------------------------
// CMceMessageListContainer::SetMskL
// ----------------------------------------------------
void CMceMessageListContainer::SetMskL()
    {
    TInt currentItem = CurrentItemIndex();
    TInt resourceId = currentItem == 0 ?
        R_MCE_MSK_BUTTON_SELECT:R_MCE_MSK_BUTTON_OPEN;

    if ( resourceId != iCurrMskResource )
        {
        iCurrMskResource = resourceId;
        CEikButtonGroupContainer* cba;
        cba = CEikButtonGroupContainer::Current();
        const TInt KMSKPosition = 3;
        cba->SetCommandL( KMSKPosition, iCurrMskResource );
        cba->DrawDeferred();
        }
    }


// ----------------------------------------------------
// CMceMessageListContainer::FocusChanged
// ----------------------------------------------------
void CMceMessageListContainer::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    if (iListBox)
        {
        iListBox->SetFocus(IsFocused());
        }
    }


// ----------------------------------------------------
//  CMceMessageListContainer::MarkedItemsCount()
// ----------------------------------------------------
TInt CMceMessageListContainer::MarkedItemsCount()
    {
    return iListBox->View()->SelectionIndexes()->Count();
    }


// ----------------------------------------------------
// CMceMessageListContainer::OfferKeyEventL
// ----------------------------------------------------
TKeyResponse CMceMessageListContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {

    if ( !iT9Interface )
        {        
        iT9Interface = CPtiEngine::NewL();
        if ( iInputMethodQwerty )
            {
            iT9Interface->SetInputMode( EPtiEngineQwerty );
            iT9Interface->ActivateLanguageL( User::Language(), EPtiEngineQwerty );        
            }
        else
            {
            iT9Interface->SetInputMode( EPtiEngineMultitapping );
        iT9Interface->ActivateLanguageL( User::Language(), EPtiEngineMultitapping );        
            }
        iPreviousInputMethod = iInputMethodQwerty;
        }
    else
        {
        if ( iInputMethodQwerty != iPreviousInputMethod )
            {
            if ( iInputMethodQwerty )
                {
                iT9Interface->SetInputMode( EPtiEngineQwerty );
                iT9Interface->ActivateLanguageL( User::Language(), EPtiEngineQwerty );        
                }
            else
                {
                iT9Interface->SetInputMode( EPtiEngineMultitapping );
                iT9Interface->ActivateLanguageL( User::Language(), EPtiEngineMultitapping );        
                }
            iPreviousInputMethod = iInputMethodQwerty;
            }
        }
    
    if ( !iT9Timer )
        {
        iT9Timer = CMceMessageListContainerKeyTimer::NewL( *iT9Interface );
        }

    if ( iListBox )
        {
  
        if ( aType == EEventKeyUp )
            {
            iOwningView.SetMSKButtonL();
            iDialerEvent = EFalse ; 
            }

        if ( aType == EEventKeyDown && iDialerEvent )
            {
            iDialerEvent = EFalse ;
            }
        if ( aType == EEventKey )
            {
            
            iOwningView.CancelMailboxTimer();

            if ( aKeyEvent.iCode == EKeyLeftArrow || aKeyEvent.iCode == EKeyRightArrow )
                {
                return EKeyWasNotConsumed;
                }

            if ( aKeyEvent.iCode == EKeyBackspace )
                {
                MarkItemSelectionL();
                SetAnchorItemIdL( 
                    CMceMessageListContainerBase::EMessageListOperationGeneral );
                const TInt itemCount = CurrentItemSelectionCount();
                if ( itemCount != 0 &&
                     ( itemCount != 1 ||
                       CurrentItemId() != KMceTemplatesEntryIdValue )
                   )
                    {
                    if ( !iListItems->DeleteDiscard( CurrentItemId() ) )
                        {
                        //if outbox and mms/sms connected, not delete
                        iOwningView.HandleCommandL( EMceCmdDelete );
                        }
                    }
                return EKeyWasConsumed;
                }
            else if ( aKeyEvent.iCode == EKeyOK ||  aKeyEvent.iCode == EKeyEnter )
                {
                if ( aKeyEvent.iModifiers & EModifierShift ||
                       aKeyEvent.iModifiers & EModifierCtrl )
                    {
                    if ( iListItems->FolderId() != KMsvGlobalOutBoxIndexEntryIdValue &&
                         IsCurrentItemFolder() )
                        {
                        return EKeyWasNotConsumed;
                        }
                    }
                else
                    {
                    iOwningView.SetHideExitCommand( ETrue );
                    if ( iListItems->FolderId() == KMsvGlobalOutBoxIndexEntryIdValue ||
                        iOwningView.SyncMlOutboxInbox( KMsvGlobalOutBoxIndexEntryId ) ||
                        CurrentItemSelectionCount() == 0 ||
                        iOwningView.CheckMMSNotificationOpenL() )
                        {
                        if(iListItems->FolderId() == KMsvGlobalOutBoxIndexEntryIdValue)
                            {
                            iOwningView.SetContextCommandFlag(ETrue);
                            }
                        // list is empty, open options menu
                        iOwningView.ProcessCommandL( EAknSoftkeyOptions );
                        return EKeyWasConsumed;
                        }
                    }
                }

            else if ( aKeyEvent.iCode == EKeyYes )
                {
                if (!iDialerEvent && aKeyEvent.iRepeats == 0) 
	                {
                // send key has been pressed in Inbox or in Documents folder
                TMsvId folderId = iListItems->FolderId();
                if ( folderId != KMsvDraftEntryId &&
                    folderId != KMsvGlobalOutBoxIndexEntryId &&
                    folderId != KMsvSentEntryId &&       
                    folderId != KMceTemplatesEntryId                   
                    && ( CurrentItemSelectionCount() == 1 )  )
                    {       
                    if ( !( iListItems->IsFolderRemote() ) )
                        {
                        TMsvId id = CurrentItemId();
                        TBool validNumber = EFalse;
                        TBuf<KMceVisibleTextLength> recipientstring;
                        TPhCltTelephoneNumber number;
                        CMceUi* mceUi = static_cast<CMceUi*>( iEikonEnv->AppUi() );
                        validNumber = MceUtils::ValidPhoneNumberL( id, mceUi->Session(), number, &recipientstring );
                        if (KErrNotFound != number.Match( recipientstring ))
                            {
                            recipientstring = KNullDesC;
                            }

                        //
                        // Callback number is supported in CDMA mode. If there isn't a phone
                        // book match for the sender, it gets confusing to display the sender's
                        // number (contained in recipientstring) and the callback number
                        // (contained in the number field) via CallToSenderQueryL.
                        // Therefore, if the recipientstring differs from the phone number
                        // to be used to call the sender and recipientstring is also a phone
                        // number, we won't display recipientstring when running in CDMA mode.
                        //
                        if ( FeatureManager::FeatureSupported( KFeatureIdProtocolCdma ) )
                            {
                            if ( CommonPhoneParser::IsValidPhoneNumber( recipientstring, CommonPhoneParser::ESMSNumber ) )
                                {
                                recipientstring = KNullDesC;
                                }
                            }

                        if ( validNumber )
                            {                        
	                            iAvkonAppUi->SetKeyEventFlags( CAknAppUiBase::EDisableSendKeyShort | CAknAppUiBase::EDisableSendKeyLong ); 
                            MsvUiServiceUtilitiesInternal::CallToSenderQueryL( number, recipientstring, EFalse );
	                            iAvkonAppUi->SetKeyEventFlags( 0x00 ); 
                            }
                        }
                    else   //(iiListItems->IsFolderRemote()) Enable Sendkey in Mail List
                        {
                        iAvkonAppUi->SetKeyEventFlags( CAknAppUiBase::EDisableSendKeyShort | CAknAppUiBase::EDisableSendKeyLong );
                        iOwningView.HandleSendkeyInMailboxL( CurrentItemId() );  
                        iAvkonAppUi->SetKeyEventFlags( 0x00 );
                        }
                    }           
                SetAnchorItemIdL( 
                    CMceMessageListContainerBase::EMessageListOperationGeneral );
	                }
                return EKeyWasConsumed;
                }           

            else if ( IsValidSearchKey( aKeyEvent ) )
                {
                if ( !( FeatureManager::FeatureSupported(KFeatureIdJapanese) ||
		    FeatureManager::FeatureSupported(KFeatureIdChinese) )  )
                    {                
                    HandleShortcutForSearchL( aKeyEvent );
                    }   
                }
            else if (  aKeyEvent.iCode == (TUint)EPtiKeyStar )
                {              
                if ( HasHandleStarKeyDoneL() ) // * key
                    {
                    return EKeyWasConsumed;
                    }
                }  
            }

        if ( iListItems->MdcaCount() > 0 && iListBox->View()->SelectionIndexes()->Count() == 0 )
        	{
        	iOwningView.SetHideExitCommand( EFalse );
        	}
        TKeyResponse ret = iListBox->OfferKeyEventL(aKeyEvent, aType);
        iOwningView.SetCurrentItemId();
        SetAnchorItemIdL( 
            CMceMessageListContainerBase::EMessageListOperationGeneral );
        if ( ( aKeyEvent.iModifiers & EModifierShift ||
              aKeyEvent.iModifiers & EModifierCtrl ) &&
             ( aKeyEvent.iCode == EKeyOK || 
               aKeyEvent.iCode == EKeyEnter ||
               aKeyEvent.iCode == EKeyUpArrow ||
               aKeyEvent.iCode == EKeyDownArrow ) )
            {
            MarkItemSelectionL();
            iOwningView.SetMSKButtonL();
            }
        else if ( iOwningView.IsMSKMailBox() &&
             ( aKeyEvent.iCode == EKeyUpArrow || aKeyEvent.iCode == EKeyDownArrow ) )
            {
            iOwningView.SetMSKButtonL();                    
            }
        return ret;
        }
    else
        {
        return EKeyWasNotConsumed;
        }

    }

// ----------------------------------------------------
// CMceMessageListContainer::IsCurrentItemFolder
// ----------------------------------------------------
TBool CMceMessageListContainer::IsCurrentItemFolder() const
    {
    TBool isCurrentFolder = EFalse;
    const TMsvId id = CurrentItemId();
    if ( id != KErrNotFound )
        {
        isCurrentFolder = iListItems->IsItemFolder( id );
        }
    return isCurrentFolder;
    }
    
    
// ----------------------------------------------------
// CMceMessageListContainer::IsCurrentItemFolder
// ----------------------------------------------------
TBool CMceMessageListContainer::IsItemFolder( TMsvId aItemId ) const
    {
    return iListItems->IsItemFolder( aItemId );
    }


// ----------------------------------------------------
// CMceMessageListContainer::IsItemFolder
// ----------------------------------------------------
TBool CMceMessageListContainer::IsItemFolder(TInt aIndex) const
    {
    TBool isCurrentFolder = EFalse;
    const TMsvId id = iListItems->ItemId( aIndex );
    if ( id != KErrNotFound )
        {
        isCurrentFolder = iListItems->IsItemFolder( id );
        }
    return isCurrentFolder;
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::IsItemRemote
// ----------------------------------------------------
TBool CMceMessageListContainer::IsItemRemote( TMsvId aItemId ) const
    {
    return iListItems->IsItemRemote( aItemId );
    }


// ----------------------------------------------------
// CMceMessageListContainer::IsOutboxOpened
// ----------------------------------------------------
TBool CMceMessageListContainer::IsOutboxOpened() const
    {
    return IsOpenedFolder( KMsvGlobalOutBoxIndexEntryIdValue );
    }


// ----------------------------------------------------
// CMceMessageListContainer::
// ----------------------------------------------------
TBool CMceMessageListContainer::IsOpenedFolder( TMsvId aFolderId ) const
    {
    return ( iListItems->FolderId() == aFolderId );
    }
    
    
// ----------------------------------------------------
// CMceMessageListContainer::DisplayOptionsMenuFromSelectionKey
// ----------------------------------------------------
TBool CMceMessageListContainer::DisplayOptionsMenuFromSelectionKey() const
    {
    return ( iListItems->MdcaCount() == 0 || iListBox->View()->SelectionIndexes()->Count() );
    }


// ----------------------------------------------------
// CMceMessageListContainer::GetHelpContext
// returns helpcontext as aContext
// ----------------------------------------------------
void CMceMessageListContainer::GetHelpContext
        (TCoeHelpContext& aContext) const
    {
    TMsvId folderId = iListItems->FolderId();
    aContext.iMajor = KMceApplicationUid;

    TBool isRemoteFolder = iListItems->IsFolderRemote();

    /*if ( iOwningView.SyncMlOutboxInbox( KMsvGlobalInBoxIndexEntryId ) )
        {
        aContext.iContext = KMCE_HLP_MESS_SYNCI;
        }
    else if ( iOwningView.SyncMlOutboxInbox( KMsvGlobalOutBoxIndexEntryId ) )
        {
        aContext.iContext = KMCE_HLP_MESS_SYNCO;
        }

    else if ( iOwningView.IsSyncMl() )
        {
        aContext.iContext = KMCE_HLP_MESS_SYNC;
        }

    else*/
    if ( folderId == KMsvDraftEntryId )
        {
        aContext.iContext = KMCE_HLP_DRAFTS;
        }
    else if ( folderId == KMsvGlobalOutBoxIndexEntryId )
        {
        aContext.iContext = KMCE_HLP_OUTBOX;
        }
    else if ( folderId == KMsvGlobalInBoxIndexEntryId )
        {
        aContext.iContext = KMCE_HLP_INBOX;
        }
    else if ( folderId == KMsvSentEntryId )
        {
        aContext.iContext = KMCE_HLP_SENT_ITEMS;
        }
    else if ( folderId == KMceDocumentsEntryId )
        {
        aContext.iContext = KMCE_HLP_DOCUMENTS;
        }
    else if ( folderId == KMceTemplatesEntryId )
        {
        aContext.iContext = KMCE_HLP_TEMPLATES;
        }
    else if ( !isRemoteFolder )
        {
        // own folder
        aContext.iContext = KMCE_HLP_FOLDER;
        }
    else
        {
        // remote account
        TBool isConnected = iListItems->IsConnected();
        if ( isConnected )
            {
            aContext.iContext = KMCE_HLP_MAILBOX_ONLINE;
            }
        else
            {
            aContext.iContext = KMCE_HLP_MAILBOX_OFFLINE;
            }
        }

    }

// ----------------------------------------------------
// CMceMessageListContainer::SyncMlOutboxOpened
// ----------------------------------------------------
TBool CMceMessageListContainer::SyncMlOutboxOpened() const
    {
    return ( iOwningView.SyncMlOutboxInbox( KMsvGlobalOutBoxIndexEntryId ) );
    }



// ----------------------------------------------------
// CMceMessageListContainer::HandleResourceChange
// ----------------------------------------------------
void CMceMessageListContainer::HandleResourceChange( TInt aType )
    {    
    CCoeControl::HandleResourceChange( aType );
    if ( iListBox && aType == KEikDynamicLayoutVariantSwitch )        
        {
        
	    iListBox->ItemDrawer()->SetItemMarkReverse( ETrue );
        SetRect( iOwningView.ClientRect() );
        }        
    }


// ----------------------------------------------------
// CMceMessageListContainer::SizeChanged
// ----------------------------------------------------
void CMceMessageListContainer::SizeChanged()
    {
    if (iListBox)
        {
        iListBox->SetRect( Rect() );
	    iListBox->ItemDrawer()->SetItemMarkReverse( ETrue );
        }
    }

// ----------------------------------------------------
// CMceMessageListContainer::ProcessCommandL
// ----------------------------------------------------
void CMceMessageListContainer::ProcessCommandL(TInt aCommand)
    {
    if ( aCommand == EAknSoftkeyShiftMSK )
        {
        // Pass the MSK marking command as a key event to controls
        const CListBoxView::CSelectionIndexArray* items =
            iListBox->SelectionIndexes();
        
        // check what state item under "cursor" is        
        TKeyArrayFix key(0, ECmpTInt);
        TInt pos;
        if ( !items->Find(CurrentItemIndex(), key, pos))
            {
            RemoveCurrentItemFromSelection();
            }
        else
            {
            AddCurrentItemToSelectionL();
            }
        iOwningView.SetMSKButtonL();            
        }
    }
    
 
// ----------------------------------------------------
// CMceMessageListContainer::StateCanBeChangedByStarKeyL() const
// Exam if it is the situation that * key can change message state
// Returns ETrue if it can
// Public
// ----------------------------------------------------  
TBool CMceMessageListContainer::StateCanBeChangedByStarKeyL() const
    {
    TMsvId folderId = iListItems->FolderId();
    TBool isRemoteFolder = iListItems->IsFolderRemote();
    
    if ( ( iListItems->MdcaCount() > 0 && 
        iListBox->View()->SelectionIndexes()->Count() == 0 && 
        !IsCurrentItemFolder() ) && 
        ( folderId == KMsvGlobalInBoxIndexEntryId ||  //Inbox
        folderId == KMceDocumentsEntryId ||     //Documents
        iListItems->GetParentFolderId() == KMceDocumentsEntryId || //Subfolder of Documents
        isRemoteFolder ) )  // Mailbox & SyncMl
        {
        if ( iOwningView.IsSyncMl() )
            {            
            return( iOwningView.ListContainer()->FolderEntry().iRelatedId  == KMsvGlobalInBoxIndexEntryId );//Inbox of SyncMail     
            }            
        return ETrue;
        }
     else 
        {                   
        return EFalse;           
        }                
    }
 
// TODO: maybe worth of adding new function for these listbox->ItemDrawer()->SetItemMarkXXX function?
//      MarkItemSelectionL(); does selection but not mark
//      AddCurrentItemToSelectionL(); selects and marks


//-------------------------------------------------------------------------------
// CMceMessageListContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
//-------------------------------------------------------------------------------
void CMceMessageListContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        iOwningView.SetMSKButtonL();
        iOwningView.SetCurrentItemId();
        if ( ( iOwningView.IsMSKMailBox() || 
        		iListItems->FolderId() == KMsvDraftEntryId ) && 
        		iListBox->CurrentItemIndex() < 0 ) 
            {
            iOwningView.ProcessCommandL( EAknSoftkeyContextOptions );
            }
        }
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        if ( iOwningView.IsMSKMailBox() )
            {
            iOwningView.CancelMailboxTimer();
            }
        }
    iListBox->HandlePointerEventL( aPointerEvent );
    }


//-------------------------------------------------------------------------------
//CMceMessageListContainer::IsValidSearchKey(const TKeyEvent& aKeyEvent)
//-------------------------------------------------------------------------------
TBool CMceMessageListContainer::IsValidSearchKey( const TKeyEvent& aKeyEvent )
    {
    TBool response = EFalse;
    if ( iInputMethodQwerty )
        {
        response = ETrue;
        }
    if ((( aKeyEvent.iCode >= (TUint)EPtiKey0 && 
           aKeyEvent.iCode <= (TUint)EPtiKey9 ) ) 
           && !aKeyEvent.iRepeats )
        {
        response = ETrue;
        }
    if ( aKeyEvent.iCode == EKeyUpArrow || 
    	 aKeyEvent.iCode == EKeyDownArrow )
    	{
    	response = EFalse;
    	}
    return response;
    }


//-------------------------------------------------------------------------------
//CMceMessageListContainer::HandleQwertyModeChangeNotification()
//-------------------------------------------------------------------------------
TInt CMceMessageListContainer::HandleQwertyModeChangeNotification()
    {
    TInt err = iQwertyModeStatusProperty.Get( iInputMethodQwerty );
    if ( err )
        {
        // Set defaut inputmethod. 0 = No Qwerty -> EPtiEngineMultitapping
        iInputMethodQwerty = 0; 
        }
    return err;        
    }


//-------------------------------------------------------------------------------
//CMceMessageListContainer::QwertyModeChangeNotification(TAny* aObj)
//-------------------------------------------------------------------------------
TInt CMceMessageListContainer::QwertyModeChangeNotification( TAny* aObj )
    {
    TInt err = KErrArgument;
    if ( aObj != NULL )
        {
        err = static_cast<CMceMessageListContainer*>(aObj)->HandleQwertyModeChangeNotification();
        }
    return err;        
    }

// ----------------------------------------------------
// CMceMessageListContainer::
// ----------------------------------------------------
void CMceMessageListContainer::SetSortTypeL( TInt aSortType, TBool aOrdering )
    {
    iListItems->SetOrderingL( aSortType, aOrdering );
    }

// ----------------------------------------------------
// CMceMessageListContainer::
// ----------------------------------------------------
void CMceMessageListContainer::SetMtmUiDataRegistry( MMtmUiDataRegistryObserver* aRegistryObserver )
    {
    iListItems->SetRegistryObserver( aRegistryObserver );
    }
    
// ----------------------------------------------------
// CMceMessageListContainer::
// ----------------------------------------------------
TMsvId CMceMessageListContainer::FindEntry( const TDesC& aMatchString ) const
    {
    TMsvId foundEntry = KErrNotFound;
    TInt index = iListItems->FindEntryIndex( aMatchString );
    if ( index >= 0 )
        {
        foundEntry = iListItems->ItemId( index );
        }
    return foundEntry;
    
    }
    
// ----------------------------------------------------
// CMceMessageListContainer::
// ----------------------------------------------------
TBool CMceMessageListContainer::FindFirstUnreadMessageL( TMsvId& aEntryId ) const
    {
    return iListItems->FindFirstUnreadMessageL( aEntryId );
    }
    
// ----------------------------------------------------
// CMceMessageListContainer::
// ----------------------------------------------------
void CMceMessageListContainer::ResetCurrentItemL()
    {
    if ( iListItems->MdcaCount() )
        {
		//calulate the top item index
		TInt topIndex = iListBox->View()->CalcNewTopItemIndexSoItemIsVisible( 0 );
		iListBox->SetTopItemIndex( topIndex );
		iListBox->SetCurrentItemIndex( 0 );
        }
    iListBox->UpdateScrollBarsL();
    }
    
// ----------------------------------------------------
// CMceMessageListContainer::
// ----------------------------------------------------
CMceMessageListContainerBase::TMessageListItemType CMceMessageListContainer::CurrentItemType() const
    {
    if ( iListItems->MdcaCount() )
        {
        return CMceMessageListContainerBase::EMessageListItemMsvItem;
        }
    return CMceMessageListContainerBase::EMessageListItemUnknown;
    }
    
// ----------------------------------------------------
// CMceMessageListContainer::
// ----------------------------------------------------
void CMceMessageListContainer::SetListEmptyTextL(const TDesC& aText)
    {
    iListBox->View()->SetListEmptyTextL( aText );
    }
    
// ----------------------------------------------------
// CMceMessageListContainer::
// ----------------------------------------------------
void CMceMessageListContainer::SetAnchorItemIdL( 
    TMessageListOperationType aOperationType )  
    {
    if ( iListItems->MdcaCount() == 0 )
        {
        iAnchorItemId = KErrNotFound;
        iAnchorItemIndex = KErrNotFound;
        iLastOperationType = aOperationType;
        return;
        }
        
    switch ( aOperationType )
        {
        case EMessageListOperationGeneral:
            iAnchorItemIndex = iListBox->CurrentItemIndex(); 
            iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
        break;
        case EMessageListOperationConnect:
            iAnchorItemIndex = iListBox->CurrentItemIndex(); 
            iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
        break;
        case EMessageListOperationFetchSelected:
            // find first from the marked messages list...? What if that is deleted when connected???
            if ( iSelectedEntries->Count() )
                {
                // pick first one
                iAnchorItemId = (*iSelectedEntries)[0];
                iAnchorItemIndex = iListItems->ItemIndex( iAnchorItemId );
                }
            else
                {
                iAnchorItemIndex = iListBox->CurrentItemIndex(); 
                iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
                }
        break;
        case EMessageListOperationFetchNew:
            iAnchorItemIndex = iListBox->CurrentItemIndex(); 
            iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
        break;
        case EMessageListOperationDelete:
            iAnchorItemIndex = iListBox->CurrentItemIndex(); 
            iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
        break;
        case EMessageListOperationCompleted:
            HandleOperationCompletedL();
        break;
        default:
        // assert debug ?
        break;
        }
        
#ifdef _DEBUG
    RDebug::Print(_L("CMceMessageListContainer1: iAnchorItemId 0x%x, iAnchorItemIndex %d"), iAnchorItemId, iAnchorItemIndex);
#endif    
    if ( aOperationType != EMessageListOperationCompleted )        
        {
        iLastOperationType = aOperationType;
        }
    }
    
// ----------------------------------------------------
// CMceMessageListContainer::
// ----------------------------------------------------
void CMceMessageListContainer::HandleOperationCompletedL()
    {
    if ( /*iLastOperationType == EMessageListOperationConnect &&*/
         iAnchorItemIndex == KErrNotFound )
        {
        // set focus to first one...
        if ( iListItems->MdcaCount() )
            {
            iListBox->SetCurrentItemIndex( 0 );
            iAnchorItemIndex = 0;
            iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
            }
        RefreshListbox();
        }
    SetAnchorItemIdL( EMessageListOperationGeneral );
    }
    
// ----------------------------------------------------
// CMceMessageListContainer::
// ----------------------------------------------------
void CMceMessageListContainer::HandleMsvSessionEventL( 
    MMsvSessionObserver::TMsvSessionEvent aEvent,
    CMsvEntrySelection& aSelection,
    TBool aDraw )
    {
#ifdef _DEBUG
    RDebug::Print(_L("CMceMessageListContainer2: iAnchorItemId 0x%x, iAnchorItemIndex %d"), iAnchorItemId, iAnchorItemIndex);
#endif
    switch ( aEvent )
        {
        case MMsvSessionObserver::EMsvEntriesDeleted:
            {
/*            if ( iOwningView.IsMSKMailBox() ) // focus setting only for email
                {
                }
            else*/
                {
                ItemCountChangedL( EFalse ); 
                TInt selectedIndex = ItemIndex( iAnchorItemId );
                if ( selectedIndex == KErrNotFound )
                    {
                    // we could not find anchor item, lets try to set new focus using index
                    
                    if ( iAnchorItemIndex > iListItems->MdcaCount() - 1 )
                        {
                        iAnchorItemIndex = iListItems->MdcaCount() - 1;
                        }
                    iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
                    }
                if ( aDraw )
                    {
                    SetCurrentItemIdL( iAnchorItemId );
                    }
                iAnchorItemIndex = iListBox->CurrentItemIndex();
                }
            if ( aDraw )
                {
                RefreshListbox();
                }
            }
        break;
        case MMsvSessionObserver::EMsvEntriesCreated:    
        case MMsvSessionObserver::EMsvEntriesChanged:
            {
            
            ItemCountChangedL( ETrue );
            
            if ( iLastOperationType != EMessageListOperationConnect &&
                 iAnchorItemId != KErrNotFound )
                {
                if ( iOwningView.IsMSKMailBox() ) // focus setting only for email
                    {
                    TInt selectedIndex = ItemIndex( iAnchorItemId );
                    if ( selectedIndex == KErrNotFound )
                        {
                        // we could not find anchor item, lets try to set new focus using index
                        if ( iAnchorItemIndex > iListItems->MdcaCount() - 1 )
                            {
                            iAnchorItemIndex = iListItems->MdcaCount() - 1;
                            }
                        iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
                        }
                    if (selectedIndex != iListBox->CurrentItemIndex() && 
                  		aEvent == MMsvSessionObserver::EMsvEntriesChanged )
	                    {
	                    iAnchorItemIndex = iListBox->CurrentItemIndex();
	                    iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
	                    }
                    if(aEvent==MMsvSessionObserver::EMsvEntriesCreated)
                        {
                               TInt selectedIndex =0;
                               iAnchorItemIndex=0;
                               iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
                        }
                    if ( aDraw )
                        {
                        SetCurrentItemIdL( iAnchorItemId );
                        }
                    iAnchorItemIndex = iListBox->CurrentItemIndex();
    #ifdef _DEBUG
                    RDebug::Print(_L("CMceMessageListContainer3: iAnchorItemId 0x%x, iAnchorItemIndex %d"), iAnchorItemId, iAnchorItemIndex);
    #endif
                    //SetCurrentItemIdL( iAnchorItemId );
                    }
                else
                    {
                    if ( aSelection.Count() )
                        {
                        // is this enough?
                        SetCurrentItemIdL( aSelection.At(0) );
                        }
                    }
                }
            }
        
        
        break;
        case MMsvSessionObserver::EMsvEntriesMoved:
        break;
        default:
        break;
        };
    }

// ----------------------------------------------------
// CMceMessageListContainer::OptionsCommandSupported
// ----------------------------------------------------
TUint CMceMessageListContainer::OptionsCommandSupported() const
    {
    return TMessageListCommandNone;
    }
    
// ----------------------------------------------------
// CMceMessageListContainer::HandleOptionsCommandL
// ----------------------------------------------------
void CMceMessageListContainer::HandleOptionsCommandL( TInt /*aCommand*/ )
    {
    // Should we use assert debug here?
    }

// ----------------------------------------------------
// CMceMessageListContainer::DateTimeFormatChangedL
// ----------------------------------------------------
void CMceMessageListContainer::DateTimeFormatChangedL( TInt /*aChangeFlag*/ )
    {
    RefreshListbox();
    }

// ---------------------------------------------------------------------------
// CMceMessageListContainer::SetMskL
// ---------------------------------------------------------------------------
//
TInt CMceMessageListContainer::SetSpecialMskL()
    {
    return NULL;
    }

//-------------------------------------------------------------------------------
//CMceMessageListContainer::HandleShortcutForSearch( const TKeyEvent& aKeyEvent )
//-------------------------------------------------------------------------------
void CMceMessageListContainer::HandleShortcutForSearchL( const TKeyEvent& aKeyEvent )
    {
   // in remote mailbox key has been pressed, find item if sorted
    // by subject or sender
    TInt Len = 0;
    TBuf<KMceTextBufferLength> text;
    text.Zero();
    if ( iT9Interface->LastEnteredKey() != ( TInt ) aKeyEvent.iScanCode )
        {                    
        iT9Interface->ClearCurrentWord();
        }      
    iT9Interface->AppendKeyPress( (TPtiKey)aKeyEvent.iScanCode );                
    Len = iT9Interface->CurrentWord().Length(); 
    if ( Len <  KMceTextBufferLength )
        {
        text = iT9Interface->CurrentWord();
        }
    else
        {
        text = iT9Interface->CurrentWord().Left(KMceTextBufferLength);
        }
    
    iOwningView.FindAndSelectEntryL( text );
    iT9Timer->Start();       
    }

//-------------------------------------------------------------------------------
//CMceMessageListContainer::HasHandleStarKeyDoneL()
//------------------------------------------------------------------------------- 
TBool CMceMessageListContainer::HasHandleStarKeyDoneL()
    {
    TBool starkeyIsHandled = EFalse;
    if ( StateCanBeChangedByStarKeyL() )
        {
        if ( IsSortByDate() )
            {
            MarkReadUnreadWithStarKeyL();
            }
        else
            {
            // if sort by anything else other than by Date, handle focus change
            iListBox->SetCurrentItemIndex( 0 );
            iListBox->DrawDeferred();            
            }
        starkeyIsHandled = ETrue;
        } 
    return starkeyIsHandled;
    }
    
//-------------------------------------------------------------------------------
//CMceMessageListContainer::MarkReadUnreadWithStarKeyL()
//------------------------------------------------------------------------------- 
void CMceMessageListContainer::MarkReadUnreadWithStarKeyL()
    {             
    MarkItemSelectionL();
    if( iOwningView.IsItemUnread( CurrentItemId() ) ) 
        {
        iOwningView.HandleCommandL( EMceCmdMarkAsRead );
        }
    else
        {
        iOwningView.HandleCommandL( EMceCmdMarkAsUnread );
        }   
    }

//-------------------------------------------------------------------------------
//CMceMessageListContainer::IsSortByDate()
//------------------------------------------------------------------------------- 
TBool CMceMessageListContainer::IsSortByDate()
    {                               
    TMsvSelectionOrdering ordering = iListItems->SortType();
    return ( ( ordering.Sorting() == EMsvSortByDateReverse) || ( ordering.Sorting() == EMsvSortByDate ) );
    }

// ---------------------------------------------------------------------------
// CMceMessageListContainer::SetContainerFlag
// ---------------------------------------------------------------------------
void CMceMessageListContainer::SetContainerFlag( TMceContainerFlags /*aFlag*/, TBool /*aState*/ )
    {
    }

void CMceMessageListContainer::UpdateIconArrayL()
    {
    // do nothing for two row list.
    }

//  End of File
