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
*     Handles different mce bitmaps.
*     First collects all the bitmaps from different mtm's and also local
*     folder's bitmaps to icon array.
*
*/



// INCLUDE FILES
#include "MceOneRowMessageListBitmapResolver.h"
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
#include <aknsinglecolumnstyletreelist.h>

#include "MceUtils.h"

// security data caging
#include <data_caging_path_literals.hrh>
#include <bldvariant.hrh>
#include <AknIconUtils.h>


// LOCAL CONSTANTS AND MACROS

_LIT( KMceDirAndFileMuiu,"muiu.mbm" );

const TInt KMceIconArrayGranularity = 10;

// ================= MEMBER FUNCTIONS =======================

CMceOneRowBitmapResolver* CMceOneRowBitmapResolver::NewL(
    CMsvSessionPtr aSession,
    MMtmUiDataRegistryObserver& aRegObserver )
    { // static
    CMceOneRowBitmapResolver* self=new(ELeave) CMceOneRowBitmapResolver(
        aSession,
        aRegObserver );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

CMceOneRowBitmapResolver::CMceOneRowBitmapResolver(
    CMsvSessionPtr aSession,
    MMtmUiDataRegistryObserver& aRegObserver )
    :
    iRegObserver( aRegObserver ),
    iSession( aSession ),
    iBitmapsLoaded( 0 )
    {
    }

CMceOneRowBitmapResolver::~CMceOneRowBitmapResolver()
    {
    if ( iIcons )
        {
        iIcons->ResetAndDestroy();
        }
    delete iIcons;
    iIconArray.Close();
    }


// ----------------------------------------------------
// CMceOneRowBitmapResolver::ConstructL
// ----------------------------------------------------
void CMceOneRowBitmapResolver::ConstructL()
    {
    // security data caging
    TParse fp;
    fp.Set( AknIconUtils::AvkonIconFileName(), &KDC_RESOURCE_FILES_DIR , NULL );
    iFilenameAvkon = fp.FullName();

    fp.Set( KMceDirAndFileMuiu, &KDC_APP_BITMAP_DIR , NULL );
    iFilenameMuiu = fp.FullName();

    iIcons = new(ELeave) CAknIconArray( KMceIconArrayGranularity );
    ChangeBitmapInIconArrayL();

    iIconArray.Reset();
    for ( TInt i = EMceBitmapIndexUnknown; i < EMceBitmapIndexLastIconIndex; i++ )
        {
        TMceHierListIconEntry iconEntry;
        iconEntry.iBitmapPointer = 0;
        iconEntry.iIconId = 0;
        iIconArray.AppendL( iconEntry );
        }

    }

// ----------------------------------------------------
// CMceOneRowBitmapResolver::CreateAndAppendIconL
// ----------------------------------------------------
void CMceOneRowBitmapResolver::CreateAndAppendIconL(
    MAknsSkinInstance* aSkin,
    TAknsItemID aId,
    const TDesC& aFileName,
    TInt aBitmapIndex, 
    TMceOneRowBitmapLoadType aBitmapType )
    {
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    
    if ( aBitmapType == EMceOneRowBitmapLoadNormal )
        {
        AknsUtils::CreateIconL( 
            aSkin, 
            aId, 
            bitmap,
            mask, 
            aFileName, 
            aBitmapIndex, 
            aBitmapIndex + 1 );  
        
        }
    else if ( aBitmapType == EMceOneRowBitmapLoadSkinned )
        {
        AknsUtils::CreateColorIconL(
            aSkin,
            aId,
            KAknsIIDQsnIconColors,
            EAknsCIQsnIconColorsCG13,
            bitmap,
            mask,
            aFileName,
            aBitmapIndex,
            aBitmapIndex + 1,
            KRgbBlack );
        }
    else
        {
        AknsUtils::CreateColorIconL(
            aSkin,
            aId,
            KAknsIIDQsnIconColors,
            EAknsCIQsnIconColorsCG16,
            bitmap,
            mask,
            aFileName,
            aBitmapIndex,
            aBitmapIndex + 1,
            KRgbBlack );
        }
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );

    CleanupStack::PushL( icon );
    iIcons->AppendL( icon );
    CleanupStack::Pop(); // icon

    iBitmapsLoaded = iIcons->Count();
    }


// ----------------------------------------------------
// CMceOneRowBitmapResolver::IconArray
// ----------------------------------------------------
CArrayPtrFlat<CGulIcon>* CMceOneRowBitmapResolver::IconArray()
    {
    return iIcons;
    }

// ----------------------------------------------------
// CMceOneRowBitmapResolver::BitmapIndex
// ----------------------------------------------------
TInt CMceOneRowBitmapResolver::BitmapIndex( const TMsvEntry& aEntry )
    {
    TInt foundIndex = KErrNotFound;
    const TMsvId id = aEntry.Id();
    if ( aEntry.iMtm == KUidMsvLocalServiceMtm &&
         aEntry.iType == KUidMsvFolderEntry )
        {
        foundIndex = LocalEntryBitmapIndex( id );

        // check if folder has unread messages
        if ( id ==  KMsvGlobalInBoxIndexEntryId ||
//            id ==  KMceDocumentsEntryId ||
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

// ----------------------------------------------------
// CMceOneRowBitmapResolver::LoadBitmapL
// ----------------------------------------------------
TInt CMceOneRowBitmapResolver::LoadBitmapL( const TMsvEntry& aEntry )
    {
    CBaseMtmUiData* uiData = iRegObserver.GetMtmUiDataL(aEntry.iMtm);

    if ( uiData )
        {
        TInt flags = 0;
        if ( aEntry.iType.iUid == KUidMsvServiceEntryValue ||
            aEntry.iType == KUidMsvFolderEntry)
            {
            TBool unreadMessages = EFalse;
            TRAPD( err, ( unreadMessages = HasUnreadMessagesL( aEntry.Id() ) ) );
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
        bitmaps = &uiData->ContextIcon( aEntry, flags );

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


// ----------------------------------------------------
// CMceOneRowBitmapResolver::GetBitmapL
// ----------------------------------------------------
TInt CMceOneRowBitmapResolver::GetBitmapL( TMsvEntry& aEntry, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
    {
    TInt index = BitmapIndex( aEntry );
    if ( index >= 0 )
        {
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* bitmapMask = NULL;
        CGulIcon* foundIcon = iIcons->At( index );
        bitmap = foundIcon->Bitmap();
        bitmapMask = foundIcon->Mask();
        aBitmap = bitmap;
        aMask = bitmapMask;
        
        // if this is found from the array already
        index = KErrNotFound;
        const TInt iconCount = iIconArray.Count();
        for (TInt i = 0; i < iconCount && index == KErrNotFound; i++)
            {
            if ( iIconArray.operator[](i).iBitmapPointer == bitmap )
                {
                index = iIconArray.operator[](i).iIconId;
                }
            }
        }
        
    return (index < 0 ? KErrNotFound : index );
    }

// ----------------------------------------------------
// CMceOneRowBitmapResolver::GetBitmapL
// ----------------------------------------------------
TInt CMceOneRowBitmapResolver::GetBitmapL( TMceBitmapArrayIndexes aBitmapIndex, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
    {
    // or should it be ok just return iIconArray.operator[](aBitmapIndex).iIconId;
    CGulIcon* foundIcon = iIcons->At( aBitmapIndex );
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* bitmapMask = NULL;
    bitmap = foundIcon->Bitmap();
    bitmapMask = foundIcon->Mask();
    aBitmap = bitmap;
    aMask = bitmapMask;
        
    // if this is found from the array already
    TInt index = KErrNotFound;
    const TInt iconCount = iIconArray.Count();
    for (TInt i = 0; i < iconCount && index == KErrNotFound; i++)
        {
        if ( iIconArray.operator[](i).iBitmapPointer == bitmap )
            {
            index = iIconArray.operator[](i).iIconId;
            }
        }
    return index;
    }

// ----------------------------------------------------
// CMceOneRowBitmapResolver::AddTreeIconIdL
// ----------------------------------------------------
TInt CMceOneRowBitmapResolver::AddTreeIconIdL( 
    CFbsBitmap* aBitmap, 
    TInt aTreeIconIndex,
    TUid aMtmUid )
    {
    TMceHierListIconEntry iconEntry;
    iconEntry.iBitmapPointer = aBitmap;
    iconEntry.iIconId = aTreeIconIndex;
    iconEntry.iMtmUid = aMtmUid;
    iIconArray.AppendL( iconEntry );
    return iIconArray.Count();
    }
    
// ----------------------------------------------------
// CMceOneRowBitmapResolver::SetTreeIconId
// ----------------------------------------------------
void CMceOneRowBitmapResolver::SetTreeIconId( TMceBitmapArrayIndexes aBitmapIndex, TInt aTreeIconIndex )
    {
    iIconArray.operator[](aBitmapIndex).iIconId = aTreeIconIndex;
    }
    
// ----------------------------------------------------
// CMceOneRowBitmapResolver::TreeIconId
// ----------------------------------------------------
TInt CMceOneRowBitmapResolver::TreeIconId( TMceBitmapArrayIndexes aBitmapIndex )
    {
    return iIconArray.operator[](aBitmapIndex).iIconId;
    }
    


// ----------------------------------------------------
// CMceOneRowBitmapResolver::HasUnreadMessagesL
// ----------------------------------------------------
TBool CMceOneRowBitmapResolver::HasUnreadMessagesL( const TMsvId aId )
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

    CleanupStack::PopAndDestroy(); // entry

    return unreadMessages;
    }

// ----------------------------------------------------
// CMceOneRowBitmapResolver::LocalEntryBitmapIndex
// ----------------------------------------------------
TInt CMceOneRowBitmapResolver::LocalEntryBitmapIndex( TMsvId aEntryId ) const
    {
    TInt foundIndex = EMceBitmapIndexFolderSmall;
    switch( aEntryId )
        {
        case 0x1009: //KMceTemplatesEntryId:
            foundIndex = EMceBitmapIndexFolderTemp;
            break;
        default:
            // set in foundIndex declaration
            break;
        }
    return foundIndex;
    }

// ----------------------------------------------------
// CMceOneRowBitmapResolver::ChangeBitmapInIconArrayL
// ----------------------------------------------------
void CMceOneRowBitmapResolver::ChangeBitmapInIconArrayL(  )
    {
    if ( !iIcons )
        {
        // should not happen!
        return;
        }
        
    iIcons->ResetAndDestroy();
    
    MAknsSkinInstance* skins = AknsUtils::SkinInstance();
    CreateAndAppendIconL( 
        skins, 
        KAknsIIDQgnPropMceUnknownRead, 
        iFilenameMuiu, 
        EMbmMuiuQgn_prop_mce_unknown_read, 
        EMceOneRowBitmapLoadNormal );
    
    CreateAndAppendIconL( 
        skins, 
        KAknsIIDQgnPropMceTemplate, 
        iFilenameAvkon, 
        EMbmAvkonQgn_prop_folder_temp, 
        EMceOneRowBitmapLoadNormal );
        
    CreateAndAppendIconL( 
        skins, 
        KAknsIIDQgnPropFolderSmall, 
        iFilenameAvkon, 
        EMbmAvkonQgn_prop_folder_small, 
        EMceOneRowBitmapLoadNormal );
        
    CreateAndAppendIconL( 
        skins, 
        KAknsIIDQgnPropFolderSmallNew, 
        iFilenameAvkon, 
        EMbmAvkonQgn_prop_folder_small_new, 
        EMceOneRowBitmapLoadNormal );
        
    CreateAndAppendIconL( 
        skins, 
        KAknsIIDQgnIndiAttachementAdd, 
        iFilenameMuiu, 
        EMbmMuiuQgn_indi_attach_add, 
        EMceOneRowBitmapLoadSkinned );
        
    CreateAndAppendIconL( 
        skins, 
        KAknsIIDQgnIndiAttachementAdd, 
        iFilenameMuiu, 
        EMbmMuiuQgn_indi_attach_add, 
        EMceOneRowBitmapLoadSkinnedHighlight );
        
    CreateAndAppendIconL( 
        skins, 
        KAknsIIDQgnIndiMcePriorityHigh, 
        iFilenameAvkon, 
        EMbmAvkonQgn_indi_mce_priority_high, 
        EMceOneRowBitmapLoadSkinned );
        
    CreateAndAppendIconL( 
        skins, 
        KAknsIIDQgnIndiMcePriorityHigh, 
        iFilenameAvkon, 
        EMbmAvkonQgn_indi_mce_priority_high, 
        EMceOneRowBitmapLoadSkinnedHighlight );
        
    CreateAndAppendIconL( 
        skins, 
        KAknsIIDQgnIndiMcePriorityLow, 
        iFilenameAvkon, 
        EMbmAvkonQgn_indi_mce_priority_low, 
        EMceOneRowBitmapLoadSkinned );
        
    CreateAndAppendIconL( 
        skins, 
        KAknsIIDQgnIndiMcePriorityLow, 
        iFilenameAvkon, 
        EMbmAvkonQgn_indi_mce_priority_low, 
        EMceOneRowBitmapLoadSkinnedHighlight );

    CreateAndAppendIconL( 
        skins,
        KAknsIIDQgnPropMceSubSender,
        iFilenameMuiu,
        EMbmMuiuQgn_prop_mce_sub_sender,
        EMceOneRowBitmapLoadNormal );

    CreateAndAppendIconL( 
        skins,
        KAknsIIDQgnPropMceSubSenderUnread,
        iFilenameMuiu,
        EMbmMuiuQgn_prop_mce_sub_sender_unread,
        EMceOneRowBitmapLoadNormal );

    CreateAndAppendIconL( 
        skins,
        KAknsIIDQgnPropMceSubSubject,
        iFilenameMuiu, 
        EMbmMuiuQgn_prop_mce_sub_subject,
        EMceOneRowBitmapLoadNormal);

    CreateAndAppendIconL( 
        skins,
        KAknsIIDQgnPropMceSubSubjectUnread,
        iFilenameMuiu, 
        EMbmMuiuQgn_prop_mce_sub_subject_unread,
        EMceOneRowBitmapLoadNormal ); 

    CreateAndAppendIconL( 
        skins,
        KAknsIIDQgnPropMceSubTime,
        iFilenameMuiu, 
        EMbmMuiuQgn_prop_mce_sub_time,
        EMceOneRowBitmapLoadNormal );

    CreateAndAppendIconL( 
        skins,
        KAknsIIDQgnPropMceSubTimeUnread,
        iFilenameMuiu, 
        EMbmMuiuQgn_prop_mce_sub_time_unread,
        EMceOneRowBitmapLoadNormal );

    // add one dummy item which will be replaced when querying message items icons
    CreateAndAppendIconL( 
        skins, 
        KAknsIIDQgnPropMceUnknownRead, 
        iFilenameMuiu, 
        EMbmMuiuQgn_prop_mce_unknown_read, 
        EMceOneRowBitmapLoadNormal );

    }

// ----------------------------------------------------
// CMceOneRowBitmapResolver::SkinChangedL
// ----------------------------------------------------
void CMceOneRowBitmapResolver::SkinChangedL( CAknSingleColumnStyleTreeList* aListbox )
    {
    // first update default icons...
    ChangeBitmapInIconArrayL();
    // then assign new bitmaps to tree listbox
    TInt i = 0;
    for ( i = EMceBitmapIndexUnknown; i < EMceBitmapIndexLastIconIndex; i++ )
        {
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* bitmapMask = NULL;
        if ( GetBitmapL( (TMceBitmapArrayIndexes)i, bitmap, bitmapMask ) < 0 )
            {
            aListbox->AssignIconL(
                iIconArray.operator[](i).iIconId,
                bitmap,
                bitmapMask,
                EFalse,
                EAspectRatioPreserved );
            iIconArray.operator[](i).iBitmapPointer = bitmap;
            }
        }
        
    // and then remove all the messaging icons
    const TInt count = iIconArray.Count();
    for ( i = iIconArray.Count()-1; i > EMceBitmapIndexLastIconIndex-1; i-- )
        {
        aListbox->RemoveIconL( iIconArray.operator[](i).iIconId );
        iIconArray.Remove( i );
        }
    // and finally start refresh of the list, that will re-load all the messaging icons
    }


// ----------------------------------------------------
// CMceOneRowBitmapResolver::UpdateIconArrayL
// ----------------------------------------------------
void CMceOneRowBitmapResolver::UpdateIconArrayL( CAknSingleColumnStyleTreeList* aListbox )
    {
    for ( TInt i = iIconArray.Count()-1; i > EMceBitmapIndexLastIconIndex-1; i-- )
        {
        if ( !MceUtils::IsMtmBuiltIn( iIconArray.operator[](i).iMtmUid ) )
            {
            aListbox->RemoveIconL( iIconArray.operator[](i).iIconId );
            iIconArray.Remove( i );
            }
        }
    }

//  End of File
