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
*     Handles different mce bitmaps.
*     First collects all the bitmaps from different mtm's and also local
*     folder's bitmaps to icon array.
*
*/



// INCLUDE FILES
#include <mtudcbas.h>       // cbasemtmuidata
#include "MtmRegistryObserver.h" // mmtmuidataregistryobserver
#include <msvuids.h>
#include <msvids.h>         // fixed entry ids
#include <gulicon.h>        // CGulIcon
#include <AknIconArray.h>   // cakniconarray
#include <fbs.h>            // CFbsFont
#include <muiu.mbg>         // bitmap enums
#include <avkon.mbg>        // bitmap enums
#include <mtud.hrh>         // EMtudEntryStateOpen
#include <SenduiMtmUids.h>  // mtm uids
#include "MceUtils.h"
#include "MceListItem.h"
#include "MceBitmapResolver.h"
#include "MceIds.h"         //kmcedocumentsentryid

// security data caging
#include <data_caging_path_literals.hrh>
#include <bldvariant.hrh>
#include <AknIconUtils.h>


// LOCAL CONSTANTS AND MACROS

_LIT( KMceDirAndFileMuiu,"muiu.mbm" );

const TInt KMceIconArrayGranularity = 10;

// ================= MEMBER FUNCTIONS =======================

CMceBitmapResolver* CMceBitmapResolver::NewL(
    CMsvSessionPtr aSession,
    MMtmUiDataRegistryObserver& aRegObserver,
    TBool aLargeIcons )
    { // static
    CMceBitmapResolver* self=new(ELeave) CMceBitmapResolver(
        aSession, aRegObserver, aLargeIcons );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CMceBitmapResolver::CMceBitmapResolver(
    CMsvSessionPtr aSession,
    MMtmUiDataRegistryObserver& aRegObserver,
    TBool aLargeIcons) :
    iRegObserver(aRegObserver),
    iSession( aSession),
    iLargeIcons( aLargeIcons ),
    iBitmapsLoaded( 0 ),
    iDescriptionLength( KMceVisibleTextLength )
    {
    }

CMceBitmapResolver::~CMceBitmapResolver()
    {
    if ( iIcons )
        {
        iIcons->ResetAndDestroy();
        }
    delete iIcons;
    delete iIconIndexes;
    // iSession deleted by Ui class.
    }


// ----------------------------------------------------
// CMceBitmapResolver::ConstructL
// ----------------------------------------------------
void CMceBitmapResolver::ConstructL()
    {
    // security data caging
    TParse fp;
    fp.Set( AknIconUtils::AvkonIconFileName(), &KDC_RESOURCE_FILES_DIR , NULL );
    iFilenameAvkon = fp.FullName();
    
    fp.Set( KMceDirAndFileMuiu, &KDC_APP_BITMAP_DIR , NULL );
    iFilenameMuiu = fp.FullName();

    iIcons = new(ELeave) CAknIconArray( KMceIconArrayGranularity );
    iIconIndexes = new(ELeave) CArrayFixFlat<TMceBitmapListItem>(
        KMceIconArrayGranularity );

    TMceBitmapListItem listItem;
    listItem.iExtraItem = EFalse;
    listItem.iMtm.iUid = KUidMsvLocalServiceMtmValue;
    listItem.iType.iUid = KUidMsvFolderEntryValue;

    listItem.iId = KErrNone;
    listItem.iExtraItem = ETrue; 
    
    CreateAndAppendIconL( KAknsIIDQgnIndiMarkedAdd, iFilenameAvkon, EMbmAvkonQgn_indi_marked_add, &listItem );
    
    CreateAndAppendIconL( KAknsIIDQgnPropMceWriteSub, EMbmMuiuQgn_prop_mce_write_sub, &listItem );
    CreateAndAppendIconL( KAknsIIDQgnPropMceRemoteSub, EMbmMuiuQgn_prop_mce_remote_sub, &listItem );
    CreateAndAppendIconL( KAknsIIDQgnPropMceRemoteNewSub, EMbmMuiuQgn_prop_mce_remote_new_sub, &listItem );
    CreateAndAppendIconL( KAknsIIDQgnPropMceDrSub, EMbmMuiuQgn_prop_mce_dr_sub, &listItem );

    listItem.iExtraItem = EFalse;
    listItem.iId = KMsvGlobalInBoxIndexEntryId;
    CreateAndAppendIconL( KAknsIIDQgnPropMceInboxSub, EMbmMuiuQgn_prop_mce_inbox_sub, &listItem );
    CreateAndAppendIconL( KAknsIIDQgnPropMceInboxNewSub, EMbmMuiuQgn_prop_mce_inbox_new_sub, &listItem );

    listItem.iId = KMceDocumentsEntryId;
    CreateAndAppendIconL( KAknsIIDQgnPropMceDocumentsSub, EMbmMuiuQgn_prop_mce_documents_sub, &listItem );
    CreateAndAppendIconL( KAknsIIDQgnPropMceDocumentsNewSub, EMbmMuiuQgn_prop_mce_documents_new_sub, &listItem );

    listItem.iId = KMsvDraftEntryId;
    CreateAndAppendIconL( KAknsIIDQgnPropMceDraftsSub, EMbmMuiuQgn_prop_mce_drafts_sub, &listItem );
    listItem.iId = KMsvSentEntryId;
    CreateAndAppendIconL( KAknsIIDQgnPropMceSentSub, EMbmMuiuQgn_prop_mce_sent_sub, &listItem );
    listItem.iId = KMsvGlobalOutBoxIndexEntryId;
    CreateAndAppendIconL( KAknsIIDQgnPropMceOutboxSub, EMbmMuiuQgn_prop_mce_outbox_sub, &listItem );

    listItem.iId = KErrNone;
    CreateAndAppendIconL( KAknsIIDQgnPropMceUnknownRead, EMbmMuiuQgn_prop_mce_unknown_read, &listItem );

    CreateAndAppendIconL( KAknsIIDQgnPropMceTemplate, iFilenameAvkon, EMbmAvkonQgn_prop_folder_temp, &listItem );
    CreateAndAppendIconL( KAknsIIDQgnPropFolderSmall, iFilenameAvkon, EMbmAvkonQgn_prop_folder_small, &listItem );
    CreateAndAppendIconL( KAknsIIDQgnPropFolderSmallNew, iFilenameAvkon, EMbmAvkonQgn_prop_folder_small_new, &listItem );
    CreateAndAppendIconL( KAknsIIDQgnIndiAttachementAdd, iFilenameMuiu, EMbmMuiuQgn_indi_attach_add, &listItem, ETrue );
    CreateAndAppendIconL( KAknsIIDQgnIndiConnectionOnAdd, iFilenameMuiu, EMbmMuiuQgn_indi_connection_on_add, &listItem, ETrue );

    CreateAndAppendIconL( KAknsIIDQgnIndiConnectionAlwaysAdd, iFilenameMuiu, EMbmMuiuQgn_indi_connection_always_add, &listItem, ETrue );
    CreateAndAppendIconL( KAknsIIDQgnIndiConnectionInactiveAdd, iFilenameMuiu, EMbmMuiuQgn_indi_connection_inactive_add, &listItem, ETrue );
    CreateAndAppendIconL( KAknsIIDQgnIndiConnectionInactiveAdd, iFilenameMuiu, EMbmMuiuQgn_indi_connection_on_roam_add, &listItem, ETrue );
    CreateAndAppendIconL( KAknsIIDQgnIndiMcePriorityHigh, iFilenameAvkon, EMbmAvkonQgn_indi_mce_priority_high, &listItem );
    CreateAndAppendIconL( KAknsIIDQgnIndiMcePriorityLow, iFilenameAvkon, EMbmAvkonQgn_indi_mce_priority_low, &listItem );
    

    // add one dummy item which will be replaced when querying message items icons
    CreateAndAppendIconL( KAknsIIDQgnPropMceUnknownRead, EMbmMuiuQgn_prop_mce_unknown_read );
    
    iDescriptionLength = MceUtils::DescriptionLengthL();
    }

// ----------------------------------------------------
// CMceBitmapResolver::CreateAndAppendIconL
// ----------------------------------------------------
void CMceBitmapResolver::CreateAndAppendIconL(  TAknsItemID aId, TInt aBitmapIndex, const TMceBitmapListItem* aListItem /* = NULL */ )
    {
    CreateAndAppendIconL( aId, iFilenameMuiu, aBitmapIndex, aListItem );
    }

// ----------------------------------------------------
// CMceBitmapResolver::CreateAndAppendIconL
// ----------------------------------------------------
void CMceBitmapResolver::CreateAndAppendIconL(
    TAknsItemID aId,
    const TDesC& aFileName,
    TInt aBitmapIndex,
    const TMceBitmapListItem* aListItem /* = NULL */,
    TBool aColorSkinnedIcon /*= EFalse */ )
    {
    MAknsSkinInstance* skins = AknsUtils::SkinInstance();
  
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    
    if ( aListItem && 
         aListItem->iExtraItem && 
         aListItem->iType.iUid > 0 &&
         aListItem->iType.iUid != KUidMsvFolderEntryValue
       )
        {
        // this is extra item and it has application uid, try to load it!
        TUid uid = aListItem->iType;
        AknsUtils::CreateAppIconLC( 
            skins, 
            uid,
            EAknsAppIconTypeList, 
            bitmap,
            mask );
        CleanupStack::Pop(2);
        }
        
    if ( !bitmap )    
        {
        if ( aColorSkinnedIcon )
            {
            TRgb rgb; // What to add here?
                    
            AknsUtils::CreateColorIconL(
                skins, 
                aId, 
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG13,
                bitmap,
                mask, 
                aFileName, 
                aBitmapIndex, 
                aBitmapIndex + 1,
                rgb );  
            }
        else
            {
            AknsUtils::CreateIconL( skins, aId, bitmap,
                mask, aFileName, aBitmapIndex, aBitmapIndex + 1 );  
            }
        }

    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
  
    CleanupStack::PushL( icon );
    if ( aListItem && aListItem->iExtraItem && aListItem->iId >= KMceFirstExtraItemId )
        {
        iIcons->InsertL( iIconIndexes->Count(), icon );
        }
    else
        {
        iIcons->AppendL( icon );
        }
    CleanupStack::Pop( icon );

    if ( aListItem )
        {
        iIconIndexes->AppendL( *aListItem );
        }
    iBitmapsLoaded = iIconIndexes->Count();
    }


// ----------------------------------------------------
// CMceBitmapResolver::IconArray
// ----------------------------------------------------
CArrayPtrFlat<CGulIcon>* CMceBitmapResolver::IconArray()
    {
    return iIcons;
    }

// ----------------------------------------------------
// CMceBitmapResolver::BitmapIndex
// ----------------------------------------------------
TInt CMceBitmapResolver::BitmapIndex( const TMceBitmapListItem& aItem ) const
    {
    TInt foundIndex = KErrNotFound;
    if ( aItem.iExtraItem )
        {
        if ( aItem.iId == KMceWriteMessageId )
            {
            foundIndex = EMceBitmapIndexWrite;
            }
        else if ( aItem.iId == KMceDeliveryReportsId )
            {
            foundIndex = EMceBitmapIndexDr;
            }
        else if ( aItem.iId == KMceNoMailboxesListId )
            {
            foundIndex = EMceBitmapIndexRemote;
            }
        else
            {
            const TInt count = iIconIndexes->Count();
            for ( TInt i = 0; i < count; i++)
                {
                const TMceBitmapListItem item = (*iIconIndexes)[i];
                if ( (*iIconIndexes)[i].iId == aItem.iId )
                    {
                    foundIndex = i;
                    }
                }
            if ( foundIndex == KErrNotFound )
                {
                foundIndex = EMceBitmapIndexUnknown;
                }
            }
        }
    if ( foundIndex == KErrNotFound &&
        aItem.iMtm.iUid == KUidMsvLocalServiceMtmValue &&
        aItem.iType.iUid == KUidMsvFolderEntryValue )
        {
        foundIndex = LocalEntryBitmapIndex( aItem.iId );
        }

    if ( foundIndex == KErrNotFound &&
         aItem.iType.iUid == KUidMsvServiceEntryValue )
        {
        // item is service entry (remote mailbox usually) so this
        // icon is loaded from mtm uid data when needed and
        // its index is last in icon array
        foundIndex = iIcons->Count()-1;
        }

    return ( foundIndex == KErrNotFound ?
        EMceBitmapIndexUnknown :
        foundIndex );
    }

#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
// ----------------------------------------------------
// CMceBitmapResolver::BitmapIndex
// ----------------------------------------------------
TInt CMceBitmapResolver::BitmapIndex( const TMsvEntry& aEntry )
    {
    TInt foundIndex = KErrNotFound;
    const TMsvId id = aEntry.Id();
    if ( aEntry.iMtm == KUidMsvLocalServiceMtm &&
         aEntry.iType == KUidMsvFolderEntry )
        {
        foundIndex = LocalEntryBitmapIndex( id );        

        // check if folder has unread messages
        if ( id ==  KMsvGlobalInBoxIndexEntryId ||
            id ==  KMceDocumentsEntryId ||
            id >= KFirstFreeEntryId
            )
            {
            TBool unreadMessages = EFalse;
            TRAPD(err, ( unreadMessages = HasUnreadMessagesL( id ) ) );
            if (err == KErrNone && unreadMessages )
                {
                foundIndex++;
                }
            }
        }

    // special handling for remote mailboxes to make main view appear faster in startup
    if ( aEntry.iType == KUidMsvServiceEntry &&
         ( aEntry.iMtm == KSenduiMtmImap4Uid ||
           aEntry.iMtm == KSenduiMtmPop3Uid )
         )
        {
        foundIndex = EMceBitmapIndexRemote;
        TBool unreadMessages = EFalse;
        TRAPD(err, ( unreadMessages = HasUnreadMessagesL( id ) ) );
        if (err == KErrNone && unreadMessages )
            {
            foundIndex++;
            }
        }

    if ( foundIndex == KErrNotFound )
        {
        TRAPD(err, ( foundIndex = LoadBitmapL( aEntry ) ) );
        if (err != KErrNone )
            {
            foundIndex = KErrNotFound;
            }
        }

    return ( foundIndex == KErrNotFound ?
        EMceBitmapIndexUnknown : foundIndex );
    }

#else
// ----------------------------------------------------
// CMceBitmapResolver::BitmapIndex
// ----------------------------------------------------
TInt CMceBitmapResolver::BitmapIndex( const TMsvEntry& aEntry )
    {
    TInt messageCount = 0;
    return BitmapIndex( aEntry, messageCount );
    }
// ----------------------------------------------------
// CMceBitmapResolver::BitmapIndex
// ----------------------------------------------------
TInt CMceBitmapResolver::BitmapIndex( const TMsvEntry& aEntry, TInt& aMessageCount )
    {
    TInt foundIndex = KErrNotFound;
    aMessageCount = 0;
    TInt unreadCount = 0;
    TBool unreadMessages = EFalse;
    const TMsvId id = aEntry.Id();
    if ( aEntry.iMtm == KUidMsvLocalServiceMtm &&
         aEntry.iType == KUidMsvFolderEntry )
        {
        foundIndex = LocalEntryBitmapIndex( id );        

        // check if folder has unread messages
        if ( id ==  KMsvGlobalInBoxIndexEntryId ||
            id ==  KMceDocumentsEntryId ||
            id == KMsvDraftEntryIdValue ||
            id == KMsvSentEntryIdValue ||
            id == KMsvGlobalOutBoxIndexEntryIdValue ||
            id >= KFirstFreeEntryId
            )
            {
            TRAPD(err, ( unreadMessages = HasUnreadMessagesL( id, aMessageCount, unreadCount ) ) );
            if (err == KErrNone && unreadMessages )
                {
                foundIndex++;
                }
            }
        }

    // special handling for remote mailboxes to make main view appear faster in startup
    if ( aEntry.iType == KUidMsvServiceEntry &&
         ( aEntry.iMtm == KSenduiMtmImap4Uid ||
           aEntry.iMtm == KSenduiMtmPop3Uid )
         )
        {
        foundIndex = EMceBitmapIndexRemote;
        TRAPD(err, ( unreadMessages = HasUnreadMessagesL( id, aMessageCount, unreadCount ) ) );
        if (err == KErrNone && unreadMessages )
            {
            foundIndex++;
            }
        }

    if ( foundIndex == KErrNotFound )
        {
        TInt err = KErrNone;
        if ( aEntry.iType == KUidMsvServiceEntry ||
            aEntry.iType == KUidMsvFolderEntry)
            {
            TRAP(err, ( unreadMessages = HasUnreadMessagesL( id, aMessageCount, unreadCount ) ) );
            }
        TRAP(err, ( foundIndex = LoadBitmapL( aEntry, unreadCount ) ) );
        if (err != KErrNone )
            {
            foundIndex = KErrNotFound;
            }
        }

    return ( foundIndex == KErrNotFound ?
        EMceBitmapIndexUnknown : foundIndex );
    }
#endif // RD_MSG_NAVIPANE_IMPROVEMENT
// ----------------------------------------------------
// CMceBitmapResolver::LoadBitmapL
// ----------------------------------------------------
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
TInt CMceBitmapResolver::LoadBitmapL( const TMsvEntry& aEntry, TInt aPriority )
#else
TInt CMceBitmapResolver::LoadBitmapL( const TMsvEntry& aEntry, TBool aHasUnreadMessages )
#endif
    {
    CBaseMtmUiData* uiData=iRegObserver.GetMtmUiDataL( aEntry.iMtm );
    if ( uiData )
        {
        TInt flags = 0;
        if ( aEntry.iType == KUidMsvServiceEntry ||
            aEntry.iType == KUidMsvFolderEntry)
            {
            TBool unreadMessages = EFalse;
            TInt err = KErrNone;
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
            TRAP( err, ( unreadMessages = HasUnreadMessagesL( aEntry.Id() ) ) );
#else
            unreadMessages = aHasUnreadMessages;
#endif // RD_MSG_NAVIPANE_IMPROVEMENT
            
            // leave error codes are filtered, because it is not important to know
            // if there are unread messages or not, 
            // just different icon would be displayed
            if (err == KErrNone && unreadMessages )
                {
                flags = EMtudEntryStateOpen; // includes unread messages
                }

            //check for sub folders           

            TMsvSelectionOrdering selectionOrdering( KMsvGroupByType, EMsvSortByNone, ETrue );
	        CMsvEntry* entry = iSession->GetEntryL( aEntry.Id() );
            entry->SetSortTypeL( selectionOrdering ); 
            CleanupStack::PushL( entry );
            TInt index = entry->Count()-1;
            while ( ( index>=0 ) && ( (*entry)[index].iType != KUidMsvFolderEntry ) )
                {
                index--;
                }
            CleanupStack::PopAndDestroy( entry );
            if( index != -1 )
                {                
                flags |= EMtudEntryStateSubFolder;
                }

            }

        const CBaseMtmUiData::CBitmapArray* bitmaps = NULL;
        
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
        if ( aPriority == KMceMmsPriority )
            {
            bitmaps = &uiData->ContextIcon( aEntry, ETrue );
            }
        else
            {
            bitmaps = &uiData->ContextIcon( aEntry, flags );
            }
#else
        bitmaps = &uiData->ContextIcon( aEntry, flags );
            
#endif        
 
        if ( bitmaps )
            {
            CGulIcon* tempBmp = NULL;
            
            if ( bitmaps->Count() == 1 )
                {
                tempBmp = CGulIcon::NewL( bitmaps->At( 0 ) );
                }
            else if ( bitmaps->Count() > 1 )
                {
                tempBmp = CGulIcon::NewL( bitmaps->At( 0 ), bitmaps->At( 1 ) );
                }
            
            if ( tempBmp )
                {
                CleanupStack::PushL( tempBmp );
                tempBmp->SetBitmapsOwnedExternally( ETrue );
                
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
                if ( aPriority == KMceMmsPriority )
                    {
                    iIcons->AppendL( tempBmp );
                    }
                else
                    {
                    TInt index = iIcons->Count() - 1;
                    iIcons->AppendL( tempBmp ); // Array will be of count 22
                    // index greater than 20 so 21 or 22
                    if ( index > iBitmapsLoaded )
                        {
                        while ( iIcons->Count() != iBitmapsLoaded+1 )
                            {
                            CGulIcon* iconToBeDelete = iIcons->At( index );
                            iIcons->Delete( index );
                            delete iconToBeDelete;
                            index--;
                            }
                        }
                    else
                        {
                        CGulIcon* iconToBeDelete = iIcons->At( index );
                        iIcons->Delete( index );
                        delete iconToBeDelete;
                        }
                    }
#else
                TInt index = iIcons->Count() - 1;
                iIcons->AppendL( tempBmp ); // Array will be of count 22
                // index greater than 20 so 21 or 22
                if ( index > iBitmapsLoaded )
                    {
                    while ( iIcons->Count() != iBitmapsLoaded+1 )
                        {
                        CGulIcon* iconToBeDelete = iIcons->At( index );
                        iIcons->Delete( index );
                        delete iconToBeDelete;
                        index--;
                        }
                    }
                else
                    {
                    CGulIcon* iconToBeDelete = iIcons->At( index );
                    iIcons->Delete( index );
                    delete iconToBeDelete;
                    }
                    

#endif                
                CleanupStack::Pop(); // tempBmp
                }
            }
        else
            {
            return KErrNotFound;
            }
        }
    else
        {
        // something is wrong
        return EMceBitmapIndexUnknown;
        }
    return iIcons->Count() - 1;
    }

#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
// ----------------------------------------------------
// CMceBitmapResolver::HasUnreadMessagesL
// ----------------------------------------------------
TBool CMceBitmapResolver::HasUnreadMessagesL( const TMsvId aId )
    {
    TBool unreadMessages = EFalse;
    CMsvEntry* entry = iSession->GetEntryL( aId );
    CleanupStack::PushL( entry );

    const TInt count = entry->Count();
    for ( TInt loop = 0; !unreadMessages && loop < count; loop++ )
        {
        if ( (*entry)[loop].iType.iUid == KUidMsvFolderEntryValue )
            {
            unreadMessages = HasUnreadMessagesL( (*entry)[loop].Id() );
            }
        else
            {
            unreadMessages = (*entry)[loop].Unread();
            }
        }

    CleanupStack::PopAndDestroy( entry );

    return unreadMessages;
    }
#else

// ----------------------------------------------------
// CMceBitmapResolver::HasUnreadMessagesL
// ----------------------------------------------------
TBool CMceBitmapResolver::HasUnreadMessagesL( 
    const TMsvId aFolderId, TInt& aMessages, TInt& aUnreadMessages )
    {
    CMsvEntry* entry = iSession->GetEntryL( aFolderId );
    CleanupStack::PushL( entry );

    const TInt count = entry->Count();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        const TMsvEntry& tEntry = (*entry)[loop];
        if ( tEntry.iType == KUidMsvFolderEntry )
            {
            HasUnreadMessagesL( tEntry.Id(), aMessages, aUnreadMessages );
            }
        else if ( tEntry.iType == KUidMsvMessageEntry )
            {
            aMessages++;
            if ( tEntry.Unread() )
                {
                aUnreadMessages++;
                }
            }
        }

    CleanupStack::PopAndDestroy(); // entry
    return aUnreadMessages;
    }
    
#endif // RD_MSG_NAVIPANE_IMPROVEMENT

// ----------------------------------------------------
// CMceBitmapResolver::LocalEntryBitmapIndex
// ----------------------------------------------------
TInt CMceBitmapResolver::LocalEntryBitmapIndex( TMsvId aEntryId ) const
    {
    TInt foundIndex = EMceBitmapIndexFolderSmall;
    switch( aEntryId )
        {
        case KMsvGlobalInBoxIndexEntryId:
            foundIndex = EMceBitmapIndexInbox;
            break;
        case KMsvGlobalOutBoxIndexEntryId:
            foundIndex = EMceBitmapIndexOutbox;
            break;
        case KMsvDraftEntryId:
            foundIndex = EMceBitmapIndexDrafts;
            break;
        case KMsvSentEntryId:
            foundIndex = EMceBitmapIndexSent;
            break;
        case KMceDocumentsEntryId:
            foundIndex = EMceBitmapIndexDocuments;
            break;
        case KMceTemplatesEntryId:
            foundIndex = EMceBitmapIndexFolderTemp;
            break;
        default:
            // set in foundIndex declaration
            break;
        }
    return foundIndex;
    }

// ----------------------------------------------------
// CMceBitmapResolver::ChangeBitmapInIconArrayL
// ----------------------------------------------------
void CMceBitmapResolver::ChangeBitmapInIconArrayL(  )
    {   
    TBool colorSkinnedBitmap = EFalse;
    TAknsItemID id; 
    id.iMajor = 0;
    id.iMinor = 0;
    TMceBitmapListItem bitmapListItem;

    TInt count = iIcons->Count();
    TInt bitmapIndex = 0;
    TPtrC fileName( iFilenameMuiu );
    MAknsSkinInstance* skins = AknsUtils::SkinInstance();

    for ( TInt cc=count; --cc>=0; )
        {
        colorSkinnedBitmap = EFalse;
        switch( cc )
            {
            case EMceBitmapIndexWrite:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_prop_mce_write_sub;
                id = KAknsIIDQgnPropMceWriteSub;
                break;
            case EMceBitmapIndexRemote:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_prop_mce_remote_sub;
                id = KAknsIIDQgnPropMceRemoteSub;
                break;
            case EMceBitmapIndexRemoteNew:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_prop_mce_remote_new_sub;
                id = KAknsIIDQgnPropMceRemoteNewSub;
                break;
            case EMceBitmapIndexDr:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_prop_mce_dr_sub;
                id = KAknsIIDQgnPropMceDrSub;
                break;
            case EMceBitmapIndexInbox:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_prop_mce_inbox_sub;
                id = KAknsIIDQgnPropMceInboxSub;
                break;
            case EMceBitmapIndexInboxNew:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_prop_mce_inbox_new_sub;
                id = KAknsIIDQgnPropMceInboxNewSub;
                break;
            case EMceBitmapIndexDocuments:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_prop_mce_documents_sub;
                id = KAknsIIDQgnPropMceDocumentsSub;
                break;
            case EMceBitmapIndexDocumentsNew:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_prop_mce_documents_new_sub;
                id = KAknsIIDQgnPropMceDocumentsNewSub;            
                break;
            case EMceBitmapIndexDrafts:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_prop_mce_drafts_sub;
                id = KAknsIIDQgnPropMceDraftsSub;
                break;
            case EMceBitmapIndexSent:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_prop_mce_sent_sub;
                id = KAknsIIDQgnPropMceSentSub;
                break;
            case EMceBitmapIndexOutbox:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_prop_mce_outbox_sub;
                id = KAknsIIDQgnPropMceOutboxSub;
                break;
            case EMceBitmapIndexUnknown:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_prop_mce_unknown_read;
                id = KAknsIIDQgnPropMceUnknownRead;
                break;
            case EMceBitmapIndexFolderTemp:
                fileName.Set( iFilenameAvkon );
                bitmapIndex = EMbmAvkonQgn_prop_folder_temp;
                id = KAknsIIDQgnPropMceTemplate;
                break;
            case EMceBitmapIndexFolderSmall:
                fileName.Set( iFilenameAvkon );
                bitmapIndex = EMbmAvkonQgn_prop_folder_small;
                id = KAknsIIDQgnPropFolderSmall;
                break;
            case EMceBitmapIndexFolderSmallNew:
                fileName.Set( iFilenameAvkon );
                bitmapIndex = EMbmAvkonQgn_prop_folder_small_new;
                id = KAknsIIDQgnPropFolderSmallNew;
                break;
            case EMceBitmapIndexAttachment:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_indi_attach_add;
                id = KAknsIIDQgnIndiAttachementAdd;
                colorSkinnedBitmap = ETrue;
                break;
            case EMceBitmapIndexMailboxConnectionOn:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_indi_connection_on_add;
                id = KAknsIIDQgnIndiConnectionOnAdd;
                colorSkinnedBitmap = ETrue;
                break;
            case EMceBitmapIndexMessageSelected:
                fileName.Set( iFilenameAvkon );
                bitmapIndex = EMbmAvkonQgn_indi_marked_add;
                id = KAknsIIDQgnIndiMarkedAdd;
                colorSkinnedBitmap = ETrue;
                break;
            case EMceBitmapIndexAlwaysAdd:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_indi_connection_always_add;
                id = KAknsIIDQgnIndiConnectionAlwaysAdd;
                colorSkinnedBitmap = ETrue;
                break;
            case EMceBitmapIndexAlwaysRoaming:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_indi_connection_on_roam_add;
                id = KAknsIIDQgnIndiConnectionInactiveAdd;
                colorSkinnedBitmap = ETrue;
                break;
            case EMceBitmapIndexPriorityHigh:
                fileName.Set( iFilenameAvkon );
                bitmapIndex = EMbmAvkonQgn_indi_mce_priority_high;
                id = KAknsIIDQgnIndiMcePriorityHigh;
                colorSkinnedBitmap = ETrue;
                break;
            case EMceBitmapIndexPriorityLow:
                fileName.Set( iFilenameAvkon );
                bitmapIndex = EMbmAvkonQgn_indi_mce_priority_low;
                id = KAknsIIDQgnIndiMcePriorityLow;
                colorSkinnedBitmap = ETrue;
                break;
            case EMceBitmapIndexInactiveAdd:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_indi_connection_inactive_add;
                id = KAknsIIDQgnIndiConnectionInactiveAdd;
                colorSkinnedBitmap = ETrue;
                break;
            case EMceBitmapIndexReplaceMtmIcon:
            default:
                fileName.Set( iFilenameMuiu );
                bitmapIndex = EMbmMuiuQgn_prop_mce_unknown_read;
                id = KAknsIIDQgnPropMceUnknownRead;
                break;
            } 
        
        if ( cc < iIconIndexes->Count() )
            {
            bitmapListItem = (*iIconIndexes)[cc];
            }
        else
            {
            bitmapListItem.iExtraItem = EFalse;
            }
        
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;
        
        if ( bitmapListItem.iExtraItem &&
             bitmapListItem.iId >= KMceFirstExtraItemId
           )
            {
            if ( bitmapListItem.iType.iUid > 0 &&
                 bitmapListItem.iType.iUid != KUidMsvFolderEntryValue )
                {
                // this is extra item and it has application uid, try to load it!
                TUid uid = bitmapListItem.iType;
                AknsUtils::CreateAppIconLC( 
                    skins, 
                    uid,
                    EAknsAppIconTypeList, 
                    bitmap,
                    mask );
                CleanupStack::Pop(2);
                }
            else
                {
                // do no changes, this is extremly dirty way but once in the lifetime...
                continue;
                }
            }
            
        if ( !bitmap )
            {
            if ( colorSkinnedBitmap )
                {
                TRgb rgb; // What to add here?
                        
                AknsUtils::CreateColorIconL(
                    skins, 
                    id, 
                    KAknsIIDQsnIconColors,
                    EAknsCIQsnIconColorsCG13,
                    bitmap,
                    mask, 
                    fileName, 
                    bitmapIndex, 
                    bitmapIndex + 1,
                    rgb );  
                
                }
            else
                {
                AknsUtils::CreateIconL( skins, id, bitmap,
                mask, fileName, bitmapIndex, bitmapIndex + 1 );
                }
            }


        CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
     
        CleanupStack::PushL( icon );
        CGulIcon* iconToBeDelete = iIcons->At( cc );
        iIcons->At( cc ) = icon;
        delete iconToBeDelete;
        iconToBeDelete = NULL;

        CleanupStack::Pop( icon );
    
        }  
    }

// ----------------------------------------------------
// CMceBitmapResolver::LocalEntryBitmapIndex
// ----------------------------------------------------
TInt CMceBitmapResolver::MtmIconIndex()
    {
    return iBitmapsLoaded;
    }

// ----------------------------------------------------
// CMceBitmapResolver::DescriptionLength
// ----------------------------------------------------
TInt CMceBitmapResolver::DescriptionLength()
    {
    return iDescriptionLength;
    }

//  End of File
