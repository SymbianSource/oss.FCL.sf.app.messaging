/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Derived from CMceOneRowMessageListContainer.
*
*/



// INCLUDE FILES
#include "mceui.h"
#include <aknlists.h>
#include <aknsinglecolumnstyletreelist.h>
#include <AknsBasicBackgroundControlContext.h>
#include <akntreelist.h>                        // CAknTreeList::ESaveFocus


#include <AknIconUtils.h>
#include <avkon.mbg>        // bitmap enums
#include <muiu.mbg>         // bitmap enums
#include <data_caging_path_literals.hrh>

#include "MceOneRowMessageListBitmapResolver.h"
#include "MceOneRowMessageListContainer.h"
#include "MceOneRowMessageListArray.h"
#include "MceOneRowMessageListArrayLoader.h"  // For CMceOneRowListItemArrayLoader::EHListItemSubtitle

#include "MceMessageListView.h"

#include <msvids.h>
#include <msvuids.h>
#include <SenduiMtmUids.h>  // mtm uids
#include "MceIds.h"         // KMceApplicationUid
#include "MceLogText.h"

#include <StringLoader.h>   // stringloader
#include "MceCommands.hrh"
#include <bldvariant.hrh>

#include <csxhelp/mce.hlp.hrh>
#include <csxhelp/mbxs.hlp.hrh>

#include <PtiEngine.h>
#include <featmgr.h>
#include <commonphoneparser.h>

#include <muiumsvuiserviceutilitiesinternal.h>

#include <mce.rsg>
#include <mtudreg.h> 
// CONSTANTS

const TInt KMceListContainerGranuality = 4;
const TInt KMceTextBufferLength        = 10;    // text buffer length
const TInt KMceFocusOffsetTopView      = 0;     // Set focused item top of view
const TInt KMceTextItemSize            = 75;    // Subtitle text buffer size

const TInt KMceColumn1                 = 1;
const TInt KMceColumn2                 = 2;
const TInt KMceColumn3                 = 3;
const TInt KMceFirstItemIndex          = 0;


// The position of the middle soft key
const TInt KMSKPosition = 3;
const TInt KMSKLabel    = 20;

//cmail update
#define KUidMsgTypeCmailMtmVal               0x2001F406
const TUid KMailTechnologyTypeUid = { 0x10001671 };
// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// Constructor
// ----------------------------------------------------
//
CMceOneRowMessageListContainer::CMceOneRowMessageListContainer( CMceMessageListView& aParentView ) :
    CMceMessageListContainerBase( aParentView ),
    iEmailFramework(EFalse)
    {
    }


// ----------------------------------------------------
// Destructor
// ----------------------------------------------------
//
CMceOneRowMessageListContainer::~CMceOneRowMessageListContainer()
    {
    delete iT9Interface;
    delete iT9Timer;
    delete iListItems;
    delete iBitmapResolver;
    delete iBgContext;
    delete iSelectedEntries;
    delete iTreeListBox;
    delete iUiRegistry;
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::NewL
// ----------------------------------------------------
//
CMceOneRowMessageListContainer* CMceOneRowMessageListContainer::NewL(
    CMceMessageListView& aParentView,
    const TRect& aRect,
    CMsvSessionPtr aSession,
    MMtmUiDataRegistryObserver& aRegObserver )
    {
    CMceOneRowMessageListContainer* self = new (ELeave)
        CMceOneRowMessageListContainer( aParentView );
    CleanupStack::PushL(self);
    self->ConstructL( aRect, aSession, aRegObserver );
    CleanupStack::Pop(); // self
    return self;
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::AddDefaultIconsL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::AddDefaultIconsL()
    {
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* bitmapMask = NULL;
    TInt treeIconIndex = KErrNotFound;

    for ( TInt i = EMceBitmapIndexUnknown; i < EMceBitmapIndexLastIconIndex; i++ )
        {
        if ( iBitmapResolver->GetBitmapL( (TMceBitmapArrayIndexes)i, bitmap, bitmapMask ) < 0 )
            {
            treeIconIndex = iTreeListBox->AddIconL(
                bitmap,
                bitmapMask,
                EFalse,
                EAspectRatioPreserved );
            iBitmapResolver->SetTreeIconId( (TMceBitmapArrayIndexes)i, treeIconIndex );
            }
        }
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::ConstructL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::ConstructL(
    const TRect& aRect,
    CMsvSessionPtr aSession,
    MMtmUiDataRegistryObserver& aRegObserver )
    {
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdFfEmailFramework ) )
        {
        iEmailFramework = ETrue;
        }    
    FeatureManager::UnInitializeLib();
    
    CreateWindowL();

    InitComponentArrayL();
    Components().SetControlsOwnedExternally( ETrue );

    // Construct list.
    CAknSingleColumnStyleTreeList* list =
        CAknSingleColumnStyleTreeList::NewL( *this );

    iSelectedEntries = new (ELeave) CMsvEntrySelection;

    iContainerFlags.ClearMceFlag( EMceOneRowFlagOptionsMenuOpen );

    iContainerFlags.ClearMceFlag( EMceOneRowFlagFirstSubtitleOpen );

    // Use list as a component control of the view.
    Components().AppendLC( list, KCoeNoControlId );
    CleanupStack::Pop( list );
    iTreeListBox = list;
    iTreeListBox->EnableTabModeFunctionIndicatorsL( ETrue );
    iTreeListBox->SetFlags( iTreeListBox->Flags() ^ KAknTreeListLooping ^ KAknTreeListMarkable );
    iTreeListBox->AddObserverL( this );

    iBgContext = CAknsBasicBackgroundControlContext::NewL(
        KAknsIIDQsnBgAreaMain, iAvkonAppUi->ApplicationRect(), EFalse );

    iBitmapResolver = CMceOneRowBitmapResolver::NewL( aSession, aRegObserver );
    AddDefaultIconsL();
    iListItems = CMceOneRowListItemArray::NewL( aSession, 0x1000 );
    iListItems->SetListItemArrayObserver( this );

    iTreeListBox->Sort( iListItems, EFalse );

    SetRect( aRect );
    
    TLocale locale;
    iStartOfWeek = locale.StartOfWeek();
    iDialerEvent = EFalse ;
    iUiRegistry = CMtmUiDataRegistry::NewL(*aSession); 
    }


// ----------------------------------------------------
// CMceMessageListContainer::MceListId
// ----------------------------------------------------
TInt CMceOneRowMessageListContainer::MceListId() const
    {
    return EMceListTypeOneRow;
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::CountComponentControls
// ----------------------------------------------------
//
TInt CMceOneRowMessageListContainer::CountComponentControls() const
    {
    return 1; // always only listbox
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::ComponentControl
// ----------------------------------------------------
//
CCoeControl* CMceOneRowMessageListContainer::ComponentControl(TInt /*aIndex*/) const
    {
    return iTreeListBox;
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::SetFolderL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::SetFolderL( TMsvId aFolderId )
    {
    if ( iListItems )
        {
        ResetListbox();
        iContainerFlags.ClearMceFlag( EMceOneRowFlagFirstSubtitleOpen );
        iListItems->SetFolderL( aFolderId );
        }
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::SizeChanged
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::SizeChanged()
    {
    TRect rect = Rect();
    if (iTreeListBox)
        {
        iTreeListBox->SetRect( rect );
        }
    if ( iBgContext )
        {
        iBgContext->SetRect( rect );
        }
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::HandleResourceChange
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    if( aType == KAknsMessageSkinChange )
        {
        // change skin bitmap
        TRAP_IGNORE( iBitmapResolver->SkinChangedL( iTreeListBox ) );
        ResetListbox();
        TRAP_IGNORE( iListItems->RefreshArrayL() );
        }
    
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::HandleTreeListEvent
// ----------------------------------------------------
//
TBool CMceOneRowMessageListContainer::HandleTreeListEvent( CAknTreeList& aList,
    TAknTreeItemID /* aItem */, TEvent aEvent )
    {
    if ( &aList != iTreeListBox )
        {
        return KErrNone;
        }

    switch ( aEvent )
        {
        // One of the nodes in the list is being expanded.
        case ENodeExpanded:
//            TRAP_IGNORE( AddChildrenL( aItem ) );
            ChangeMskCommand( R_MCE_QTN_MSK_COLLAPSE );
            break;
        // One of the nodes in the list is being collapsed.
        case ENodeCollapsed:
//            RemoveChildren( aItem );
            ChangeMskCommand( R_MCE_QTN_MSK_EXPAND );
            break;
        // Simple data row or core data row is selected.
        case EItemSelected:
            TRAP_IGNORE( iOwningView.HandleCommandL(EAknCmdOpen) );
            break;

        // Focused item changes
        case EItemFocused:
            if ( !iMarkingModeEnabled 
                && !iContainerFlags.MceFlag( EMceOneRowFlagOptionsMenuOpen ) )
                {
                TRAP_IGNORE( iOwningView.SetMSKButtonL() );
                }
            else if ( iContainerFlags.MceFlag( EMceOneRowFlagOptionsMenuOpen ) )
                {
                iContainerFlags.ClearMceFlag( EMceOneRowFlagOptionsMenuOpen );
                }
            break;
        // Item is being removed from the list.
        case EItemRemoved:
            break;
        // Hierarchical list has entered in marking mode
        case EMarkingModeEnabled:
            iMarkingModeEnabled = ETrue;
            break;
        // hierarchical list exits the marking mode
        case EMarkingModeDisabled:
            iMarkingModeEnabled = EFalse;
            break;
        case EItemUnmarked:
        case EItemMarked:
            if ( !iMarkingModeEnabled )
                {
                TRAP_IGNORE( iOwningView.SetMSKButtonL() );
                }
            break;
        default:
            break;
        }

    return KErrNone;
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::ResetListbox
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::ResetListbox()
    {
    iTreeListBox->RemoveItem( KAknTreeIIDRoot, EFalse );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::MceListItemArrayChangedL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::MceListItemArrayChangedL(
    CArrayFixFlat<TInt>& aChangedItems )
    {
    const TInt count = aChangedItems.Count();
    for ( TInt i = 0 ; i < count; i++ )
        {
        TInt itemIndex = aChangedItems[i];
        if ( iListItems->HCItemType(itemIndex) == CMceOneRowListItemArrayLoader::EHListItemSubtitle )
            {
            TBuf<KMceTextItemSize> subTitle;
            TInt itemType = iListItems->GetSubtitleTextL( itemIndex, subTitle );
            if ( itemType > EHCListTypeNone )
                {
                TUint32 flags = CAknSingleColumnStyleTreeList::EPersistent;
                if ( !iContainerFlags.MceFlag( EMceOneRowFlagFirstSubtitleOpen ) ) 
                    {
                    flags |= CAknSingleColumnStyleTreeList::EExpanded;
                    iContainerFlags.SetMceFlag( EMceOneRowFlagFirstSubtitleOpen );
                    }
                else
                    {
                    flags &= CAknSingleColumnStyleTreeList::EExpanded;
                    }
                iNodeId = iTreeListBox->AddSubtitleRowL( KAknTreeIIDRoot,
                    subTitle, flags, EFalse );
                iListItems->SetTreeItemId( itemIndex, iNodeId );
                iTreeListBox->SetNonEmpty( iNodeId, ETrue, EFalse );
                UpdateSubtitleIconL( iNodeId, EFalse );
                }
            }
        else
            {
            AddEntryL( itemIndex );
            }
        }

    // current item id haven't been set due list is not ready yet
    if ( iSelectedItemWaitingIndex )
        {
        SetCurrentItemIdL( iSelectedItemWaitingIndex );
        }

    iTreeListBox->Sort( iListItems, CAknTreeList::ESaveFocus, ETrue );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::GetSubtitleIcon
// ----------------------------------------------------
//
TInt CMceOneRowMessageListContainer::GetSubtitleIcon ( TBool setUnread )
    {
    TMceBitmapArrayIndexes treeIconId = EMceBitmapIndexUnknown;
    TMsvSelectionOrdering sortType = iListItems->SortType();
   
    switch ( sortType.Sorting() )
        {
        case EMsvSortByDate:
        case EMsvSortByDateReverse:
            if ( setUnread )
                {
                treeIconId = EMceBitmapIndexSubTimeUnread;        
                }
            else
                {
                treeIconId = EMceBitmapIndexSubTime;        
                }
            break;
    	case EMsvSortByDetails:
	    case EMsvSortByDetailsReverse:
            if ( setUnread )
                {
                treeIconId = EMceBitmapIndexSubSenderUnread;        
                }
            else
                {
                treeIconId = EMceBitmapIndexSubSender;        
                }
	        break;
    	case EMsvSortByDescription:
	    case EMsvSortByDescriptionReverse:
            if ( setUnread )
                {
                treeIconId = EMceBitmapIndexSubSubjectUnread;        
                }
            else
                {
                treeIconId = EMceBitmapIndexSubSubject;        
                }
            break;
        default:
            treeIconId = EMceBitmapIndexSubTime;
            break;
        }    
    return iBitmapResolver->TreeIconId( treeIconId );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::AddChildrenL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::AddChildrenL( TAknTreeItemID aParentId )
    {
    TInt index = iListItems->ItemIndexTreeId( aParentId );
    const TInt count = iListItems->GetSubfolderItemCount( aParentId );

    for ( TInt i = index ; i <= index + count ; i++ )
        {
        TBuf<KMceTextItemSize> subTitle;
        TInt itemType = iListItems->GetSubtitleTextL( i, subTitle );
        if ( iListItems->ParentItemId( i ) == aParentId &&
             itemType == 0 )
            {
            AddEntryL( i );
            }
        }
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::AddEntryL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::AddEntryL( TInt aEntryIndex )
    {
    TMsvEntry entry;
    User::LeaveIfError( iListItems->GetEntry( aEntryIndex, entry ) );
    TInt foundIcon = KErrNotFound;
    TUint32 flags;

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* bitmapMask = NULL;
    foundIcon = iBitmapResolver->GetBitmapL( entry, bitmap, bitmapMask );
    if ( foundIcon < 0 )
        {
        foundIcon = iTreeListBox->AddIconL(
                bitmap,
                bitmapMask,
                EFalse,
                EAspectRatioPreserved );
        iBitmapResolver->AddTreeIconIdL( bitmap, foundIcon, entry.iMtm );
        }

    TAknTreeItemID parentId = iListItems->ParentItemId( aEntryIndex );

    TBuf<KMceTextItemSize> bufferC1;
    TBuf<KMceTextItemSize> bufferC2;
    TBuf<KMceTextItemSize> bufferC3;
    iListItems->GetColumnTextL( aEntryIndex, KMceColumn1, bufferC1 );
    iListItems->GetColumnTextL( aEntryIndex, KMceColumn2, bufferC2 );
    iListItems->GetColumnTextL( aEntryIndex, KMceColumn3, bufferC3 );

    TAknTreeItemID itemId = 0;

    if ( IsItemFolder( entry.Id() ))
        {
        // Set folders not markable
        flags = CAknSingleColumnStyleTreeList::EPersistent
                | CAknSingleColumnStyleTreeList::EMarkingDisabled;

        // Add two column core data row for folders
        itemId = iTreeListBox->AddCoreDataRowL(
        parentId,
        bufferC1,
        bufferC3,
        flags,
        EFalse );
        }
    else
        {
        flags = CAknSingleColumnStyleTreeList::EPersistent;

        // Add three column core data row for entries
        itemId = iTreeListBox->AddCoreDataRowL(
        parentId,
        bufferC1,
        bufferC2,
        bufferC3, //subTitle,
        flags,
        EFalse );

        if ( iListItems->GetItemMarkStatusL( entry.Id()) )
            {
            iTreeListBox->SetMarked( itemId, ETrue, ETrue );
            }
        iTreeListBox->EnableThirdColumn( itemId, bufferC3.Length(), EFalse );
        }  
    
    iListItems->SetTreeItemId( aEntryIndex, itemId );
    if ( foundIcon >= 0 )
        {
        iTreeListBox->SetIcon(
            itemId,
            CAknSingleColumnStyleTreeList::ELeaf,
            foundIcon,
            EFalse );

        TBool attachmentIcon = EFalse; 
         
        if ( entry.Attachment() && entry.iMtm != KSenduiMtmMmsUid 
             && IsMailMtmTechnology(entry.iMtm ))
            {
            iTreeListBox->SetIcon(
                itemId,
                CAknSingleColumnStyleTreeList::EOptionalIcon1,
                iBitmapResolver->TreeIconId( EMceBitmapIndexAttachment ),
                EFalse );
            iTreeListBox->SetIcon(
                itemId,
                CAknSingleColumnStyleTreeList::EHighlightedOptionalIcon1,
                iBitmapResolver->TreeIconId( EMceBitmapIndexAttachmentFocus ),
                EFalse );
            attachmentIcon = ETrue;
            }

        if ( entry.iType == KUidMsvMessageEntry )
            {
            TMsvPriority entryPrio = entry.Priority();
            if ( entryPrio != EMsvMediumPriority )
                {
                TInt prioIcon = 0;
                TInt prioIconFocus = 0;
                if ( entryPrio == EMsvHighPriority )
                    {
                    prioIcon = iBitmapResolver->TreeIconId( EMceBitmapIndexPriorityHigh );
                    prioIconFocus = iBitmapResolver->TreeIconId( EMceBitmapIndexPriorityHighFocus );
                    }
                else
                    {
                    prioIcon = iBitmapResolver->TreeIconId( EMceBitmapIndexPriorityLow );
                    prioIconFocus = iBitmapResolver->TreeIconId( EMceBitmapIndexPriorityLowFocus );
                    }

                if ( attachmentIcon )
                    {
                    iTreeListBox->SetIcon(
                        itemId,
                        CAknSingleColumnStyleTreeList::EOptionalIcon2,
                        prioIcon,
                        EFalse );
                    iTreeListBox->SetIcon(
                        itemId,
                        CAknSingleColumnStyleTreeList::EHighlightedOptionalIcon2,
                        prioIconFocus,
                        EFalse );
                    }
                else
                    {
                    iTreeListBox->SetIcon(
                        itemId,
                        CAknSingleColumnStyleTreeList::EOptionalIcon1,
                        prioIcon,
                        EFalse );
                    iTreeListBox->SetIcon(
                        itemId,
                        CAknSingleColumnStyleTreeList::EHighlightedOptionalIcon1,
                        prioIconFocus,
                        EFalse );
                    }
                }
            }
        }

    // If unread message update subtitle icon
    if ( iTreeListBox->Parent( itemId ) > KAknTreeIIDRoot &&  entry.Unread() )
        {
        UpdateSubtitleIconL( parentId, ETrue );
        }
    iTreeListBox->SetEmphasis( itemId, entry.Unread(), EFalse );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::RemoveChildren
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::RemoveChildren( TAknTreeItemID aParentId )
    {
    const TInt count = iTreeListBox->ChildCount( aParentId );

    for ( TInt i = 0; i < count; i++ )
        {
        TAknTreeItemID removedItem = iTreeListBox->Child( aParentId, 0);
        iTreeListBox->RemoveItem( removedItem, EFalse );
        }
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::MopSupplyObject
// ----------------------------------------------------
//
TTypeUid::Ptr CMceOneRowMessageListContainer::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId && iBgContext )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }


// ----------------------------------------------------
// CMceMainViewListContainer::ItemIndex
// ----------------------------------------------------
TInt CMceOneRowMessageListContainer::ItemIndex( TMsvId aId ) const
    {
    return iListItems->ItemIndex( aId );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::Draw
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::Draw( const TRect& /*aRect*/ ) const
    {
    // Get the standard graphics context.
    CWindowGc& gc = SystemGc();

    // Gets the control's extent.
    TRect rect = Rect();

    // Clears the screen.
    gc.Clear( rect );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::Count
// ----------------------------------------------------
//
TInt CMceOneRowMessageListContainer::Count() const
    {
    return iListItems->MdcaCount();
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::MessageCount
// ----------------------------------------------------
//
TInt CMceOneRowMessageListContainer::MessageCount() const
    {
    return iListItems->MessageCount();
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::FolderCount
// ----------------------------------------------------
//
TInt CMceOneRowMessageListContainer::FolderCount() const
    {
    return iListItems->FolderCount();
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::CurrentFolderId
// ----------------------------------------------------
//
TMsvId CMceOneRowMessageListContainer::CurrentFolderId() const
    {
    TMsvId folderId = 0;
    if ( iListItems )
        {
        folderId = iListItems->FolderId();
        }
    return folderId;
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::SetCurrentItemIdL
// ----------------------------------------------------
TBool CMceOneRowMessageListContainer::SetCurrentItemIdL( TMsvId aEntryId )
    {
    const TInt selectedIndex = ItemIndex( aEntryId );

    if ( selectedIndex != KErrNotFound )
        {
        TAknTreeItemID selectedTreeID = iListItems->TreeItemId( selectedIndex );
        if ( selectedTreeID )
            {
            TAknTreeItemID parentId = iListItems->ParentItemId( selectedIndex );
            if ( !iTreeListBox->IsExpanded( parentId ) )
                {
                iTreeListBox->ExpandNode( parentId, ETrue );
                }
            iTreeListBox->SetFocusedItem( selectedTreeID );
            }
        iSelectedItemWaitingIndex = NULL;
        }
    // Current entry not yet added to the list 
    else if ( selectedIndex == KErrNotFound && aEntryId > 0 )
        {
        iSelectedItemWaitingIndex = aEntryId;
        }

    return (selectedIndex != KErrNotFound);
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::RefreshListbox
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::RefreshListbox()
    {
    iTreeListBox->DrawDeferred();
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::ItemCountChangedL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::ItemCountChangedL( TBool /* aItemsAdded */ )
    {
    ResetListbox();
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::FolderEntry
// ----------------------------------------------------
//
const TMsvEntry& CMceOneRowMessageListContainer::FolderEntry() const
    {
    return iListItems->FolderEntry();
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::CurrentItemId
// ----------------------------------------------------
//
TMsvId CMceOneRowMessageListContainer::CurrentItemId() const
    {
    return iListItems->MsvItemId( iTreeListBox->FocusedItem() );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::CurrentItemSelectionL
// ----------------------------------------------------
//
CMsvEntrySelection* CMceOneRowMessageListContainer::CurrentItemSelectionL()
    {
    return iSelectedEntries->CopyL();
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::CurrentItemSelectionRefreshL
// ----------------------------------------------------
//
CMsvEntrySelection* CMceOneRowMessageListContainer::CurrentItemSelectionRefreshL()
    {
    MarkItemSelectionL();
    return CurrentItemSelectionL();
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::CurrentItemSelectionCount
// ----------------------------------------------------
//
TInt CMceOneRowMessageListContainer::CurrentItemSelectionCount() const
    {
    if ( iListItems->MdcaCount() == 0 )
        {
        return 0;
        }
    TInt markedCount = 0;
    TRAP_IGNORE( markedCount = GetMarkedItemsCountL() );
    return Max( 1, markedCount );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::SelectionCount
// ----------------------------------------------------
//
TInt CMceOneRowMessageListContainer::SelectionCount() const
    {
    TInt markedCount = 0;
    TRAP_IGNORE( markedCount = GetMarkedItemsCountL() );
    return markedCount;
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::ClearSelection
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::ClearSelection()
    {
    if ( iTreeListBox )
        {
        iTreeListBox->SetMarked( KAknTreeIIDRoot, EFalse, ETrue );
        iListItems->SetItemMarked( KAknTreeIIDRoot, EFalse);
        }
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::MarkItemSelectionL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::MarkItemSelectionL()
    {
    RArray<TAknTreeItemID> selection;
    CleanupClosePushL( selection );
    iTreeListBox->GetMarkedItemsL( selection );

    TInt count( selection.Count() );
    iSelectedEntries->Reset();
    // If subtitle is only marked item and focused item is different then add focused item to selection
    if ( count == 0 || ( count == 1 && iTreeListBox->IsNode( selection[0] ) ) )

        {
        TMsvId id = CurrentItemId();
        TAknTreeItemID focused = iTreeListBox->FocusedItem();
        if ( id != KErrNotFound )
            {
            // nothing in selection so add item under "cursor"
            iSelectedEntries->AppendL( id );
            }
        else if ( focused > KAknTreeIIDNone && iTreeListBox->IsNode( focused ) ) // Focused item is subtitle
            {
            TInt index = iListItems->ItemIndexTreeId( focused );
            const TInt childCount = iListItems->GetSubfolderItemCount( focused );
            // Add all items under subtitle
            for (TInt loop = index + 1 ; loop <= childCount + index ; loop++ )
                {
                id = iListItems->MsvItemId( iListItems->TreeItemId( loop ) );
                if ( id != KErrNotFound )
                    {
                    iSelectedEntries->AppendL( id );
                    }
                }
            }
        }
     else if ( count != 0 )
        {
        for (TInt loop=0; loop < count; loop++ )
            {
            TMsvId id = iListItems->MsvItemId( selection[ loop ] );
            if ( id != KErrNotFound )
                {
                iSelectedEntries->AppendL( id );
                }
            }
        }        
        
    CleanupStack::PopAndDestroy( &selection );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::SaveMarkingL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::SaveMarkingL()
    {
    RArray<TAknTreeItemID> selection;
    CleanupClosePushL( selection );
    iTreeListBox->GetMarkedItemsL( selection );
    TInt count( selection.Count() );

    if ( count > 0 )
        {
        iListItems->SaveMarkingL( selection );        
        }
    CleanupStack::PopAndDestroy( &selection );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::ResetCurrentItemL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::ResetCurrentItemL()
    {
    iSelectedItemWaitingIndex = NULL;
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::FindEntry
// ----------------------------------------------------
//
TMsvId CMceOneRowMessageListContainer::FindEntry( const TDesC& aMatchString ) const
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
// CMceOneRowMessageListContainer::FindFirstUnreadMessageL
// ----------------------------------------------------
//
TBool CMceOneRowMessageListContainer::FindFirstUnreadMessageL( TMsvId& aEntryId ) const
    {
    return iListItems->FindFirstUnreadMessageL( aEntryId );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::AddCurrentItemToSelectionL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::AddCurrentItemToSelectionL()
    {
    TAknTreeItemID focused = iTreeListBox->FocusedItem();

    if ( iTreeListBox && focused != KAknTreeIIDNone )
        {
        iTreeListBox->SetMarked( focused, ETrue, ETrue );
        iListItems->SetItemMarked( focused, ETrue);
        }
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::RemoveCurrentItemFromSelection
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::RemoveCurrentItemFromSelection()
    {
    TAknTreeItemID focused = iTreeListBox->FocusedItem();
    if ( iTreeListBox && focused != KAknTreeIIDNone )
        {
        iTreeListBox->SetMarked( iTreeListBox->FocusedItem(),
            EFalse, ETrue );
        iListItems->SetItemMarked( focused, EFalse );
        }
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::RefreshSelectionIndexesL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::RefreshSelectionIndexesL( TBool aForceUpdate )
    {
    RArray<TInt> selection;
    CleanupClosePushL( selection );
    iTreeListBox->GetMarkedItemsL( selection );
    TInt count( selection.Count() );
    if ( count > 0 || aForceUpdate )
        {
        CArrayFixFlat<TInt>* newSelection = new ( ELeave )
            CArrayFixFlat<TInt>( KMceListContainerGranuality );
        CleanupStack::PushL( newSelection );
        const TInt count = iSelectedEntries->Count();
        for ( TInt loop = count; loop >0; loop--)
            {
            TInt index = ItemIndex( (*iSelectedEntries)[loop-1] );
            if ( index > KErrNotFound )
                {
                newSelection->AppendL( index );
                }
            else
                {
                iSelectedEntries->Delete( loop-1 );
                }
            }
        CleanupStack::PopAndDestroy( newSelection );
        }
    CleanupStack::PopAndDestroy( &selection );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::AddAllToSelectionL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::AddAllToSelectionL()
    {
    if ( iTreeListBox )
        {
        // The marking can be removed from every list item by specifying
        // the tree root as the unmarked item.
        iTreeListBox->SetMarked( KAknTreeIIDRoot, ETrue, ETrue );
        }
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::MarkAllReadMessagesL
// ----------------------------------------------------
//
 void CMceOneRowMessageListContainer::MarkAllReadMessagesL()
    {
    if ( iTreeListBox )
        {
        // do not mark folders.
        const TInt count = iListItems->MdcaCount();
        for (TInt i=0; i<count;i++)
            {
            TMsvEntry entry;
            if ( iListItems->GetEntry( i, entry ) == KErrNone &&
                 entry.iType == KUidMsvMessageEntry &&
                 !entry.Unread() )
                {
                TAknTreeItemID treeItemId = iListItems->TreeItemId( i );
                iTreeListBox->SetMarked( treeItemId, ETrue, ETrue );
                }
            }
        }
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::IsItemSelected
// ----------------------------------------------------
//
TBool CMceOneRowMessageListContainer::IsItemSelected( TMsvId aItemId )
    {
    if ( aItemId > 0 )
        {
        TInt itemIndex = ItemIndex( aItemId );
        TAknTreeItemID treeIndex = iListItems->TreeItemId( itemIndex );
        return ( iTreeListBox->IsMarked( treeIndex ) );
        }
    else // Node selected
        {
        TAknTreeItemID treeIndex = iTreeListBox->FocusedItem();
        return ( iTreeListBox->IsMarked( treeIndex ) );
        }
    }


// ----------------------------------------------------
// CMceMessageListContainer::SetAnchorItemIdL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::SetAnchorItemIdL(
    TMessageListOperationType aOperationType )
    {
    if ( iListItems->MdcaCount() == 0 )
        {
        iAnchorItemId = KErrNotFound;
        iAnchorItemIndex = KErrNotFound;
        if ( aOperationType != EMessageListOperationCompleted )
            {
        iLastOperationType = aOperationType;
            }
        return;
        }

    switch ( aOperationType )
        {
        case EMessageListOperationGeneral:
            iAnchorItemIndex = iListItems->ItemIndexTreeId(iTreeListBox->FocusedItem());
            iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
            break;
        case EMessageListOperationConnect:
            iAnchorItemIndex = iListItems->ItemIndexTreeId(iTreeListBox->FocusedItem());
            iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
            break;
        case EMessageListOperationFetchSelected:

            // find first from the marked messages list...? What if that is deleted when connected???
            if ( iSelectedEntries->Count() )
                {
                // pick first one
                iAnchorItemId = ( *iSelectedEntries )[0];
                iAnchorItemIndex = ItemIndex( iAnchorItemId );
                }
            else
                {
                iAnchorItemIndex = iListItems->ItemIndexTreeId(iTreeListBox->FocusedItem());
                iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
                }
            break;
        case EMessageListOperationFetchNew:
            iAnchorItemIndex = iListItems->ItemIndexTreeId(iTreeListBox->FocusedItem());
            iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
            break;
        case EMessageListOperationDelete:
            iAnchorItemIndex = iListItems->ItemIndexTreeId(iTreeListBox->FocusedItem());
            iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
            break;
        case EMessageListOperationCompleted:
            HandleOperationCompletedL();
            break;
        default:
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
// CMceMessageListContainer::HandleMsvSessionEventL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::HandleMsvSessionEventL(
    MMsvSessionObserver::TMsvSessionEvent aEvent,
    CMsvEntrySelection& aSelection,
    TBool aDraw )
    {
    switch ( aEvent )
        {
        case MMsvSessionObserver::EMsvEntriesDeleted:
            {
            EntryDeletedL( aSelection, aDraw );
            }

            break;
        case MMsvSessionObserver::EMsvEntriesCreated:
            {
            EntryCreatedL( aSelection, aDraw );
            }
            break;

        case MMsvSessionObserver::EMsvEntriesChanged:
            {
            TInt count = aSelection.Count();
            for (TInt i=0; i<count; i++)
                {
                UpdateEntryL( aSelection[i] );
                }
            }
            break;
        default:
            break;
        };
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::FocusChanged
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    if ( iTreeListBox )
        {
        iTreeListBox->SetFocus( IsFocused() );
        }
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::MarkedItemsCount
// ----------------------------------------------------
//
TInt CMceOneRowMessageListContainer::MarkedItemsCount()
    {
    TInt count = 0;
    TRAP_IGNORE( count = GetMarkedItemsCountL() );
    return count;
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::GetMarkedItemsCountL
// ----------------------------------------------------
//
TInt CMceOneRowMessageListContainer::GetMarkedItemsCountL() const
    {
    RArray<TInt> selection;
    CleanupClosePushL( selection );
    iTreeListBox->GetMarkedItemsL( selection );
    TInt count( selection.Count() );
    CleanupStack::PopAndDestroy( &selection );
    return count;
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::OfferKeyEventL
// ----------------------------------------------------
//
TKeyResponse CMceOneRowMessageListContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
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

    if ( iTreeListBox )
        {
        if ( aType == EEventKeyUp)
            {
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

                TMsvId currentId = CurrentItemId();
                const TInt itemCount = CurrentItemSelectionCount();
                if ( itemCount != 0 &&
                     ( itemCount != 1 ||
                       currentId != KMceTemplatesEntryIdValue )
                   )
                    {
                    if ( currentId == KErrNotFound || // subtitle is selected, allow delete
                        !iListItems->DeleteDiscard( currentId ) )
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
                        TPhCltTelephoneNumber number;
                        TBuf<KMceVisibleTextLength> recipientstring;
                        CMceUi* mceUi = static_cast<CMceUi*>( CCoeEnv::Static()->AppUi() );
                        if (id != -1)
                        	{
                        validNumber = MceUtils::ValidPhoneNumberL( id, mceUi->Session(), number, &recipientstring );
                        	}
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
                    else   // Enable Sendkey in Mail List
                        {
                        iAvkonAppUi->SetKeyEventFlags( CAknAppUiBase::EDisableSendKeyShort | CAknAppUiBase::EDisableSendKeyLong );
                        TMsvId id = CurrentItemId(); 
                        
                        if ( id != -1)
                        	{
                        	iOwningView.HandleSendkeyInMailboxL( id );
                        	}
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
            if ( iOwningView.IsMSKMailBox() || 
                 iOwningView.IsMSKSyncMlMailBox() || 
                 iListItems->FolderId() == KMsvGlobalInBoxIndexEntryId )
                {
                }  
            }
            
        if ( iListItems->MdcaCount() > 0 && SelectionCount() == 0 )
            {
            iOwningView.SetHideExitCommand( EFalse );
            }
        TKeyResponse ret = iTreeListBox->OfferKeyEventL(aKeyEvent, aType);
        iOwningView.SetCurrentItemId();
        SetAnchorItemIdL(
            CMceMessageListContainerBase::EMessageListOperationGeneral );

        if ( iOwningView.IsMSKMailBox() &&
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


//-------------------------------------------------------------------------------
//CMceMessageListContainer::IsValidSearchKey(const TKeyEvent& aKeyEvent)
//-------------------------------------------------------------------------------
//
TBool CMceOneRowMessageListContainer::IsValidSearchKey( const TKeyEvent& aKeyEvent )
    {
    TBool response = EFalse;
    if ( iInputMethodQwerty )
        {
        response = ETrue;
        }
    if ((( aKeyEvent.iCode >= (TUint)EPtiKey0 &&
           aKeyEvent.iCode <= (TUint)EPtiKey9 )  )
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


// ----------------------------------------------------
// CMceMessageListContainer::StateCanBeChangedByStarKeyL() const
// Exam if it is the situation that * key can change message state
// Returns ETrue if it can
// Public
// ----------------------------------------------------
//
TBool CMceOneRowMessageListContainer::StateCanBeChangedByStarKeyL() const
    {
    TMsvId folderId = iListItems->FolderId();
    TBool isRemoteFolder = iListItems->IsFolderRemote();

    if ( ( iListItems->MessageCount() > 0
        && SelectionCount() == 0
        && !IsCurrentItemFolder() )
        && ( folderId == KMsvGlobalInBoxIndexEntryId  //Inbox
        || folderId == KMceDocumentsEntryId  //Documents
        || iListItems->GetParentFolderId() == KMceDocumentsEntryId //Subfolder of Documents
        || isRemoteFolder ) )  // Mailbox & SyncMl
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


// ----------------------------------------------------
// CMceOneRowMessageListContainer::IsCurrentItemFolder
// ----------------------------------------------------
//
TBool CMceOneRowMessageListContainer::IsCurrentItemFolder() const
    {
    TBool isCurrentFolder = EFalse;
    const TMsvId id = CurrentItemId();
    if ( id != KErrNotFound )
        {
        isCurrentFolder = IsItemFolder( id );
        }
    return isCurrentFolder;
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::IsItemFolder
// ----------------------------------------------------
//
TBool CMceOneRowMessageListContainer::IsItemFolder( TMsvId aItemId ) const
    {
    return iListItems->IsItemFolder( aItemId );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::IsItemFolder
// ----------------------------------------------------
//
TBool CMceOneRowMessageListContainer::IsItemFolder( TInt aIndex ) const
    {
    TBool isCurrentFolder = EFalse;
    const TMsvId id = iListItems->MsvItemId( aIndex );
    if ( id != KErrNotFound )
        {
        isCurrentFolder = iListItems->IsItemFolder( id );
        }
    return isCurrentFolder;
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::IsItemRemote
// ----------------------------------------------------
//
TBool CMceOneRowMessageListContainer::IsItemRemote( TMsvId aItemId ) const
    {
    return iListItems->IsItemRemote( aItemId );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::IsOpenedFolder
// ----------------------------------------------------
//
TBool CMceOneRowMessageListContainer::IsOpenedFolder( TMsvId aFolderId ) const
    {
    return ( iListItems->FolderId() == aFolderId );
    }


// ----------------------------------------------------
// CMceMessageListContainer::DisplayOptionsMenuFromSelectionKey
// ----------------------------------------------------
//
TBool CMceOneRowMessageListContainer::DisplayOptionsMenuFromSelectionKey() const
    {
    TInt itemCount = 0;
    TRAP_IGNORE( itemCount = GetMarkedItemsCountL() );
    return ( (iListItems->MessageCount() + iListItems->FolderCount()) == 0 || itemCount );
    }


// ----------------------------------------------------
// CMceMessageListContainer::GetHelpContext
// returns helpcontext as aContext
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::GetHelpContext
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
// CMceOneRowMessageListContainer::SetSortTypeL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::SetSortTypeL( TInt aSortType, TBool aOrdering )
    {
    SaveMarkingL();
    ResetListbox();
    iListItems->SetOrderingL( aSortType, aOrdering );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::SetMtmUiDataRegistry
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::SetMtmUiDataRegistry( MMtmUiDataRegistryObserver* aRegistryObserver )
    {
    iListItems->SetRegistryObserver( aRegistryObserver );
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::TMessageListItemType
// ----------------------------------------------------
//
CMceMessageListContainerBase::TMessageListItemType CMceOneRowMessageListContainer::CurrentItemType() const
    {
    if ( iTreeListBox &&
         ( iListItems->MessageCount() || iListItems->FolderCount() )
         )
        {
        TAknTreeItemID focusedItem = iTreeListBox->FocusedItem();
        if ( iListItems->MsvItemId( focusedItem ) > KErrNotFound )
            {
            return CMceMessageListContainerBase::EMessageListItemMsvItem;
            }
        else if ( focusedItem == KAknTreeIIDNone )
            {
            return CMceMessageListContainerBase::EMessageListItemUnknown;
            }
        else
            {
            return CMceMessageListContainerBase::EMessageListItemSubtitle;
            }
        }
    return CMceMessageListContainerBase::EMessageListItemUnknown;
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::ProcessCommandL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::ProcessCommandL(TInt /*aCommand*/)
    {
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::SetListEmptyTextL
// ----------------------------------------------------
//
void CMceOneRowMessageListContainer::SetListEmptyTextL(const TDesC& aText )
    {
    iTreeListBox->SetEmptyTextL( aText ); 
    }


// ---------------------------------------------------------------------------
// CMceOneRowMessageListContainer::ChangeMskCommand
// ---------------------------------------------------------------------------
//
void CMceOneRowMessageListContainer::ChangeMskCommand( TInt aLabelResourceId )
    {
    // The MSK commands should not be changed when controlled by the list.
    __ASSERT_DEBUG( !iMarkingModeEnabled, User::Invariant() );

    // Get cba and set MSK command
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    if ( cba )
        {
        TBuf<KMSKLabel> label;
        TRAP_IGNORE( cba->SetCommandL( KMSKPosition, aLabelResourceId ) );
        cba->DrawDeferred();
        }
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::OptionsCommandSupported
// ----------------------------------------------------
TUint CMceOneRowMessageListContainer::OptionsCommandSupported() const
    {
    TUint listCommand = TMessageListCommandNone;

    if ( iTreeListBox )
        {
        TAknTreeItemID focused = iTreeListBox->FocusedItem();
        if ( focused )
            {
            if ( focused != KAknTreeIIDNone )
                {
                TAknTreeItemID parent = iTreeListBox->Parent( focused );
                // Focused item is a node and it is collapsed
                if ( iTreeListBox->IsNode( focused )
                    && !( iTreeListBox->IsExpanded( focused ) ) )
                    {
                    listCommand = TMessageListCommandExpand;
                    }

                // Focused item is expanded node or data row of the expanded node
                if ( ( iTreeListBox->IsNode( focused ) && iTreeListBox->IsExpanded( focused ) ) )
                    {
                    listCommand = TMessageListCommandCollapse1;
                    }
                else if ( iTreeListBox->IsLeaf( focused ) && parent != KAknTreeIIDRoot )
                    {
                    listCommand = TMessageListCommandCollapse2;
                    }
                }
            }
        else
            {
            // No item is focused
            }
        }
    return listCommand;
    }


// ----------------------------------------------------
// CMceOneRowMessageListContainer::HandleOptionsCommandL
// ----------------------------------------------------
void CMceOneRowMessageListContainer::HandleOptionsCommandL( TInt aCommand )
    {
    // Handle list specific options command
    iContainerFlags.ClearMceFlag( EMceOneRowFlagOptionsMenuOpen );
    switch( aCommand)
        {
        case TMessageListCommandCollapse1:
        case TMessageListCommandCollapse2:
            CollapseFocusedNode();
            break;

        case TMessageListCommandExpand:
            ExpandFocusedNode();
            break;
        default:
            break;
        }
    }


// ---------------------------------------------------------------------------
// CMceOneRowMessageListContainer::ExpandFocusedNode
// ---------------------------------------------------------------------------
//
void CMceOneRowMessageListContainer::ExpandFocusedNode()
    {
    if ( iTreeListBox )
        {
        // Get focused item ID.
        TAknTreeItemID focused = iTreeListBox->FocusedItem();

        // Check that focused item is a node and then expand it.
        if ( focused != KAknTreeIIDNone && iTreeListBox->IsNode( focused ) )
            {
            iTreeListBox->ExpandNode( focused, ETrue );
            }
        }
    }


// ---------------------------------------------------------------------------
// CMceOneRowMessageListContainer::CollapseFocusedNode
// ---------------------------------------------------------------------------
//
void CMceOneRowMessageListContainer::CollapseFocusedNode()
    {
    if ( iTreeListBox )
        {
        // Get focused item ID.
        TAknTreeItemID focused = iTreeListBox->FocusedItem();

        // Check that focused item is a node or a leaf
        if ( focused != KAknTreeIIDNone && iTreeListBox->IsNode( focused ) )
            {
            iTreeListBox->CollapseNode( focused, ETrue );
            }
        if ( iTreeListBox->IsLeaf( focused ) )
            {
            iTreeListBox->CollapseNode( iTreeListBox->Parent( focused ), ETrue );
            }
        }
    }


// ---------------------------------------------------------------------------
// CMceOneRowMessageListContainer::DateTimeFormatChangedL
// ---------------------------------------------------------------------------
//
void CMceOneRowMessageListContainer::DateTimeFormatChangedL( TInt aChangeFlag )
    {
    TInt change = aChangeFlag;
    if ( ( change & EChangesMidnightCrossover ))
        {
        ResetListbox();
        iListItems->RefreshArrayL();
        }
    else if ( ( change & EChangesLocale ))
        {
        TLocale locale;
        TDay startOfWeek = locale.StartOfWeek();
        if ( startOfWeek != iStartOfWeek )
            {
            iStartOfWeek = startOfWeek;
            ResetListbox();
            iListItems->RefreshArrayL();
            return;
            }
        
        TMsvSelectionOrdering listType = iListItems->SortType();
        TInt timeColumn = KMceColumn2;
        if ( listType.Sorting() == EMsvSortByDate ||
             listType.Sorting() == EMsvSortByDateReverse )
            {
            timeColumn = KMceColumn3;
            }

        TInt count = iListItems->MdcaCount();
        for (TInt i=0; i<count; i++)
            {
            TAknTreeItemID treeItemId = iListItems->TreeItemId( i );
            TMsvId itemId = iListItems->MsvItemId( treeItemId );
            if ( itemId > KErrNotFound && treeItemId > KAknTreeIIDNone )
                {
                // this is message, update time/date
                TBuf<KMceTextItemSize> buffer;
                iListItems->GetColumnTextL( i, timeColumn, buffer );
                iTreeListBox->SetTextL(
                    treeItemId,
                    buffer,
                    timeColumn-1,
                    EFalse );
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// CMceOneRowMessageListContainer::SetSpecialMskL
// ---------------------------------------------------------------------------
//
TInt CMceOneRowMessageListContainer::SetSpecialMskL()
    {
    TInt resourceId = R_MCE_MSK_BUTTON_EMPTY;
    TAknTreeItemID focusedItem = iTreeListBox->FocusedItem();

    if ( GetMarkedItemsCountL() == 0 )
        {
        // Determine the MSK command depending on the focused item type and state.
        if ( focusedItem == KAknTreeIIDNone )
            {
            // No item focused.
            }
        else if ( iTreeListBox->IsNode( focusedItem ) )
            {
            if ( iTreeListBox->IsExpanded( focusedItem ) )
                {
                resourceId = R_MCE_QTN_MSK_COLLAPSE;
                }
            else
                {
                resourceId = R_MCE_QTN_MSK_EXPAND;
                }
            }
        else if ( iTreeListBox->IsLeaf( focusedItem ) )
            {
            resourceId = R_MCE_MSK_BUTTON_OPEN;
            }
        }
    else
        {
        resourceId = R_MCE_MSK_BUTTON_CONTEXT_OPTIONS;
        }
    return resourceId;
    }


// ---------------------------------------------------------------------------
// CMceOneRowMessageListContainer::UpdateEntryL
// ---------------------------------------------------------------------------
//
void CMceOneRowMessageListContainer::UpdateEntryL( TMsvId aEntryId )
    {
    MCELOGGER_ENTERFN("CMceOneRowMessageListContainer::UpdateEntryL");
    TInt itemIndex = iListItems->ItemIndex( aEntryId );
    MCELOGGER_WRITE_FORMAT("aEntryId: 0x%x", aEntryId );
    MCELOGGER_WRITE_FORMAT("itemIndex : %d", itemIndex );
    if ( itemIndex < 0 )
        {
        TMsvEntry entry;
        if ( iListItems->GetEntry( aEntryId, entry ) == KErrNone )
            {
            MCELOGGER_WRITE("entry found, try to add it");
            CMsvEntrySelection* selection = new(ELeave) CMsvEntrySelection();
            CleanupStack::PushL( selection );
            selection->AppendL( aEntryId );
            EntryCreatedL( *selection, ETrue );
            CleanupStack::PopAndDestroy( selection );
            }

        MCELOGGER_LEAVEFN("CMceOneRowMessageListContainer::UpdateEntryL1");
        return;
        }

    TAknTreeItemID treeItemId = iListItems->TreeItemId( itemIndex );

    TMsvEntry entry;
    User::LeaveIfError( iListItems->GetEntry( aEntryId, entry ) );
    TInt foundIcon = KErrNotFound;

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* bitmapMask = NULL;
    foundIcon = iBitmapResolver->GetBitmapL( entry, bitmap, bitmapMask );
    if ( foundIcon < 0 )
        {
        foundIcon = iTreeListBox->AddIconL(
                bitmap,
                bitmapMask,
                EFalse,
                EAspectRatioPreserved );
        iBitmapResolver->AddTreeIconIdL( bitmap, foundIcon, entry.iMtm );
        }


    TBuf<KMceTextItemSize> bufferC1;
    TBuf<KMceTextItemSize> bufferC2;
    TBuf<KMceTextItemSize> bufferC3;
    iListItems->GetColumnTextL( itemIndex, KMceColumn1, bufferC1 );
    iListItems->GetColumnTextL( itemIndex, KMceColumn2, bufferC2 );
    iListItems->GetColumnTextL( itemIndex, KMceColumn3, bufferC3 );

    iTreeListBox->SetTextL(
        treeItemId,
        bufferC1,
        0,
        EFalse );

    if ( IsItemFolder( entry.Id() ))
        {
        iTreeListBox->SetTextL(
            treeItemId,
            bufferC3,
            1,
            EFalse );
        }
    else
        {
        iTreeListBox->SetTextL(
            treeItemId,
            bufferC2,
            1,
            EFalse );
        iTreeListBox->SetTextL(
            treeItemId,
            bufferC3,
            2,
            EFalse );
        }

    if ( foundIcon >= 0 )
        {
        iTreeListBox->SetIcon(
            treeItemId,
            CAknSingleColumnStyleTreeList::ELeaf,
            foundIcon,
            EFalse );
        TBool attachmentIcon = EFalse;
 
        if ( entry.Attachment() && entry.iMtm != KSenduiMtmMmsUid  
             && IsMailMtmTechnology(entry.iMtm ))
            {
            iTreeListBox->SetIcon(
                treeItemId,
                CAknSingleColumnStyleTreeList::EOptionalIcon1,
                iBitmapResolver->TreeIconId( EMceBitmapIndexAttachment ),
                EFalse );
            iTreeListBox->SetIcon(
                treeItemId,
                CAknSingleColumnStyleTreeList::EHighlightedOptionalIcon1,
                iBitmapResolver->TreeIconId( EMceBitmapIndexAttachmentFocus ),
                EFalse );
            attachmentIcon = ETrue;
            }
        if ( entry.iType == KUidMsvMessageEntry )
            {
            TMsvPriority entryPrio = entry.Priority();
            if ( entryPrio != EMsvMediumPriority )
                {
                TInt prioIcon = 0;
                TInt prioIconFocus = 0;
                if ( entryPrio == EMsvHighPriority )
                    {
                    prioIcon = iBitmapResolver->TreeIconId( EMceBitmapIndexPriorityHigh );
                    prioIconFocus = iBitmapResolver->TreeIconId( EMceBitmapIndexPriorityHighFocus );
                    }
                else
                    {
                    prioIcon = iBitmapResolver->TreeIconId( EMceBitmapIndexPriorityLow );
                    prioIconFocus = iBitmapResolver->TreeIconId( EMceBitmapIndexPriorityLowFocus );
                    }

                if ( attachmentIcon )
                    {
                    iTreeListBox->SetIcon(
                        treeItemId,
                        CAknSingleColumnStyleTreeList::EOptionalIcon2,
                        prioIcon,
                        EFalse );
                    iTreeListBox->SetIcon(
                        treeItemId,
                        CAknSingleColumnStyleTreeList::EHighlightedOptionalIcon2,
                        prioIconFocus,
                        EFalse );
                    }
                else
                    {
                    iTreeListBox->SetIcon(
                        treeItemId,
                        CAknSingleColumnStyleTreeList::EOptionalIcon1,
                        prioIcon,
                        EFalse );
                    iTreeListBox->SetIcon(
                        treeItemId,
                        CAknSingleColumnStyleTreeList::EHighlightedOptionalIcon1,
                        prioIconFocus,
                        EFalse );
                    }
                }
            }
        }
    iTreeListBox->EnableThirdColumn( treeItemId, !IsItemFolder( entry.Id() ), EFalse );
    iTreeListBox->SetEmphasis( treeItemId, entry.Unread(), EFalse );
    TAknTreeItemID parentId = iTreeListBox->Parent( treeItemId );
    if ( parentId > KAknTreeIIDRoot )
        {
        UpdateSubtitleIconL( parentId, EFalse );        
        }
    MCELOGGER_LEAVEFN("CMceOneRowMessageListContainer::UpdateEntryL2");
    }


// ---------------------------------------------------------------------------
// CMceOneRowMessageListContainer::EntryCreatedL
// ---------------------------------------------------------------------------
//
void CMceOneRowMessageListContainer::EntryCreatedL(
    CMsvEntrySelection& aSelection,
    TBool aDraw )
    {
    MCELOGGER_ENTERFN("CMceOneRowMessageListContainer::EntryCreatedL");
    TBool tFocus = false;
    for ( TInt i = 0; i < aSelection.Count(); i++ )
        {
        TMsvId itemId = aSelection[i];
        TMsvEntry tentry;
        iListItems->GetEntry( itemId, tentry);
        if ( iEmailFramework && 
             ( tentry.iMtm == KSenduiMtmImap4Uid || 
               tentry.iMtm == KSenduiMtmPop3Uid ||
               tentry.iMtm == KSenduiMtmSmtpUid ||
               tentry.iMtm.iUid == KUidMsgTypeCmailMtmVal ) )
            {
            continue;
            }
        MCELOGGER_WRITE_FORMAT("itemId: 0x%x", itemId );
        TInt itemsAdded = iListItems->EntryCreatedL( itemId );
        MCELOGGER_WRITE_FORMAT("itemsAdded: %d", itemsAdded );
        if ( itemsAdded < 1 )
            {
            //  item is hidden or otherwise we cound not find it, just ignore.
            break;
            }

        TInt itemIndex = iListItems->ItemIndex( itemId );
        MCELOGGER_WRITE_FORMAT("itemIndex: %d", itemIndex );
        TAknTreeItemID parentId = iListItems->ParentItemId( itemIndex );

        if ( itemsAdded == 2 && parentId == KAknTreeIIDNone )
            {
            TInt parentIndex = iListItems->ParentItemIndex( itemIndex );
            TBuf<KMceTextItemSize> subTitle;
            TInt itemType = iListItems->GetSubtitleTextL( parentIndex, subTitle );
            if ( itemType > EHCListTypeNone )
                {
                TUint32 flags = CAknSingleColumnStyleTreeList::EPersistent
                               | CAknSingleColumnStyleTreeList::EExpanded;
                iNodeId = iTreeListBox->AddSubtitleRowL( KAknTreeIIDRoot,
                    subTitle, flags, EFalse );
                iListItems->SetTreeItemId( parentIndex, iNodeId );
                UpdateSubtitleIconL( iNodeId, EFalse );
                MCELOGGER_WRITE_FORMAT("Added subtitle row : 0x%x", iNodeId );
                iTreeListBox->SetNonEmpty( iNodeId, ETrue, EFalse );
                }
            itemsAdded--;
            }

        if ( itemsAdded == 1 )
	        {
            AddEntryL( itemIndex );
            
            TMsvEntry entry;
            if ( iListItems->GetEntry( itemId, entry ) == KErrNone )
                {
                if (entry.Unread())
                    {
                    iAnchorItemId = itemId;
                    tFocus = true;
                    }
                else
                    {
                    tFocus = false;	
                    }
                }
            MCELOGGER_WRITE_FORMAT("Added entry itemIndex: %d", itemIndex );
            }
	        
              
    	}
    
    iTreeListBox->Sort( iListItems, CAknTreeList::ESaveFocus, ETrue );    
    if (tFocus == true)
    	{
    	SetCurrentItemIdL( iAnchorItemId );    	
    	}
    
    if ( aDraw )
        {
        RefreshListbox();
        }
        
    MCELOGGER_LEAVEFN("CMceOneRowMessageListContainer::EntryCreatedL");
    }


// ---------------------------------------------------------------------------
// CMceOneRowMessageListContainer::EntryDeletedL
// ---------------------------------------------------------------------------
//
void CMceOneRowMessageListContainer::EntryDeletedL(
    CMsvEntrySelection& aSelection,
    TBool aDraw )
    {
    TInt count = aSelection.Count();
    for (TInt i=0; i<count; i++)
        {
        TAknTreeItemID treeItemId = KAknTreeIIDNone;
        TAknTreeItemID parentTreeItemId = KAknTreeIIDNone;
        TBool deleteParent = iListItems->EntryDeletedL( aSelection[i], treeItemId, parentTreeItemId );
        if ( deleteParent )
            {
            // this removes node and child
            iTreeListBox->RemoveItem( parentTreeItemId, EFalse );
            }
        else if ( treeItemId > KAknTreeIIDRoot )
            {
            // delete only one child, there are other children left so leave node untouched
            iTreeListBox->RemoveItem( treeItemId, EFalse );
            if ( parentTreeItemId > KAknTreeIIDRoot )
                {
                UpdateSubtitleIconL( parentTreeItemId, EFalse );        
                }
            }
        }

    if ( aDraw )
        {
        RefreshListbox();
        }
    }


// ---------------------------------------------------------------------------
// CMceOneRowMessageListContainer::UpdateSubtitleIconL
// ---------------------------------------------------------------------------
//
void CMceOneRowMessageListContainer::UpdateSubtitleIconL( TAknTreeItemID aSubtitleID, TBool aReadStatusKnown )
    {
    TBool hasUnread = EFalse;
    if ( aReadStatusKnown )
        {
        hasUnread = aReadStatusKnown;
        }
    else
        {
        hasUnread = iListItems->FindUnreadMessageUnderSubtitleL( aSubtitleID );        
        }
    
    TInt iconID = GetSubtitleIcon( hasUnread );

    iTreeListBox->SetIcon(
        aSubtitleID,
        CAknSingleColumnStyleTreeList::EExpandedNode,
        iconID,
        EFalse );

    iTreeListBox->SetIcon(
        aSubtitleID,
        CAknSingleColumnStyleTreeList::ECollapsedNode,
        iconID,
        EFalse );        
    }


//-------------------------------------------------------------------------------
//CMceOneRowMessageListContainer::HandleShortcutForSearchL( const TKeyEvent& aKeyEvent )
//-------------------------------------------------------------------------------
//
void CMceOneRowMessageListContainer::HandleShortcutForSearchL( const TKeyEvent& aKeyEvent )
    {
    // in remote mailbox key has been pressed, find item if sorted
    // by subject or sender
    TBuf<KMceTextBufferLength> text;
    text.Zero();
    if ( iT9Interface->LastEnteredKey() != ( TInt ) aKeyEvent.iScanCode )
        {                    
        iT9Interface->ClearCurrentWord();
        }      
    iT9Interface->AppendKeyPress( (TPtiKey)aKeyEvent.iScanCode );                
    text = iT9Interface->CurrentWord();
    
    iOwningView.FindAndSelectEntryL( text );
    iT9Timer->Start();       
    }


//-------------------------------------------------------------------------------
//CMceOneRowMessageListContainer::HasHandleStarKeyDoneL()
//------------------------------------------------------------------------------- 
//
TBool CMceOneRowMessageListContainer::HasHandleStarKeyDoneL()
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
            TAknTreeItemID subtitle = iTreeListBox->Parent(
                iTreeListBox->FocusedItem() );
            if ( subtitle > KAknTreeIIDRoot )
                {
                TInt itemIndex = iTreeListBox->VisibleItemIndex( subtitle );
                if ( itemIndex  < 0 )
                    {
                    iTreeListBox->SetFocusedItem( subtitle, 
                        KMceFocusOffsetTopView, ETrue  );                          
                    }
                else
                    {
                    iTreeListBox->SetFocusedItem( subtitle, itemIndex , ETrue );
                    }
                }
            else if ( subtitle == KAknTreeIIDRoot )
                {
                TAknTreeItemID firstItem = iTreeListBox->Child( 
                    KAknTreeIIDRoot, KMceFirstItemIndex );
                iTreeListBox->SetFocusedItem( firstItem, 
                    KMceFocusOffsetTopView, ETrue  );        
                }
            }
        starkeyIsHandled = ETrue;
        } 
    return starkeyIsHandled;
    }


//-------------------------------------------------------------------------------
//CMceOneRowMessageListContainer::MarkReadUnreadWithStarKeyL()
//------------------------------------------------------------------------------- 
//
void CMceOneRowMessageListContainer::MarkReadUnreadWithStarKeyL()
    {             
    if ( CurrentItemType() == EMessageListItemMsvItem )
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
    }


//-------------------------------------------------------------------------------
//CMceOneRowMessageListContainer::IsSortByDate()
//------------------------------------------------------------------------------- 
//
TBool CMceOneRowMessageListContainer::IsSortByDate()
    {                               
    TMsvSelectionOrdering ordering = iListItems->SortType();
    return ( ( ordering.Sorting() == EMsvSortByDateReverse) || ( ordering.Sorting() == EMsvSortByDate ) );
    }


//-------------------------------------------------------------------------------
// CMceOneRowMessageListContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
//-------------------------------------------------------------------------------
void CMceOneRowMessageListContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        iOwningView.SetMSKButtonL();
        iOwningView.SetCurrentItemId();
        if ( ( iOwningView.IsMSKMailBox() ||
        		iListItems->FolderId() == KMsvDraftEntryId ) && 
        		iTreeListBox->ChildCount( KAknTreeIIDRoot ) == 0 ) 
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
    iTreeListBox->HandlePointerEventL( aPointerEvent );
    }


// ---------------------------------------------------------------------------
// CMceOneRowMessageListContainer::SetContainerFlag
// ---------------------------------------------------------------------------
//
void CMceOneRowMessageListContainer::SetContainerFlag( TMceContainerFlags aFlag, TBool aState )
    {
    switch ( aFlag )
        {
        case EMceOneRowFlagOptionsMenuOpen:
            if ( !aState )
                {
                iContainerFlags.ClearMceFlag( EMceOneRowFlagOptionsMenuOpen );        
                }
            else
                {
                iContainerFlags.SetMceFlag( EMceOneRowFlagOptionsMenuOpen );        
                }
            break;

        case EMceOneRowFlagFirstSubtitleOpen:
            if ( !aState )
                {
                iContainerFlags.ClearMceFlag( EMceOneRowFlagFirstSubtitleOpen );        
                }
            else
                {
                iContainerFlags.SetMceFlag( EMceOneRowFlagFirstSubtitleOpen );        
                }
            break;

        default:
            break;
        }
    }


// ---------------------------------------------------------------------------
// CMceOneRowMessageListContainer::UpdateIconArrayL
// ---------------------------------------------------------------------------
//
void CMceOneRowMessageListContainer::UpdateIconArrayL( )
    {
    MCELOGGER_ENTERFN("CMceOneRowMessageListContainer::UpdateIconsInListL");
    
    iBitmapResolver->UpdateIconArrayL( iTreeListBox );
    
    const TInt count = iListItems->MdcaCount();
    
    for ( TInt i = 0; i < count; i++ )
        {
        TMsvId entryId = iListItems->ItemId(i);
        if ( entryId > KErrNotFound ) // If KErrNotFound, then this is subtitle?
            {
            TAknTreeItemID treeItemId = iListItems->TreeItemId( i );

            TMsvEntry entry;
            // TODO: do not leave here
            User::LeaveIfError( iListItems->GetEntry( entryId, entry ) );
            TInt foundIcon = KErrNotFound;

            CFbsBitmap* bitmap = NULL;
            CFbsBitmap* bitmapMask = NULL;
            foundIcon = iBitmapResolver->GetBitmapL( entry, bitmap, bitmapMask );
            if ( foundIcon < 0 )
                {
                foundIcon = iTreeListBox->AddIconL(
                        bitmap,
                        bitmapMask,
                        EFalse,
                        EAspectRatioPreserved );
                iBitmapResolver->AddTreeIconIdL( bitmap, foundIcon, entry.iMtm );
                }

            if ( foundIcon >= 0 )
                {
                iTreeListBox->SetIcon(
                    treeItemId,
                    CAknSingleColumnStyleTreeList::ELeaf,
                    foundIcon,
                    EFalse );
                }
            }
        }
    MCELOGGER_LEAVEFN("CMceOneRowMessageListContainer::UpdateIconsInListL");
    }
// ----------------------------------------------------
// CMceOneRowMessageListContainer::HandleOperationCompletedL
// ----------------------------------------------------
void CMceOneRowMessageListContainer::HandleOperationCompletedL()
    {

       if(iLastOperationType == EMessageListOperationConnect )
         {                  
         iAnchorItemIndex=1;
         iAnchorItemId = iListItems->ItemId( iAnchorItemIndex );
         SetCurrentItemIdL( iAnchorItemId );
         }
    }

// ----------------------------------------------------
// CMceOneRowMessageListContainer::IsMailMtmTechnology
// ----------------------------------------------------
TBool CMceOneRowMessageListContainer::IsMailMtmTechnology( TUid aMtm )const
    {
    TBool isMailMtm = EFalse;
    if ( aMtm.iUid != 0 && aMtm != KUidMsvLocalServiceMtm &&
            iUiRegistry->IsPresent( aMtm) )
        {
        // get MTM technology type
        TUid technologyType = iUiRegistry->TechnologyTypeUid( aMtm);
        isMailMtm = ( KMailTechnologyTypeUid == technologyType );
        }
    return isMailMtm;
    }
//  End of File
