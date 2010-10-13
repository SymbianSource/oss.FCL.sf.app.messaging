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
*     Item array for the message items to be displayed by mce.
*     Main view items are kept in memory (CArrayFixFlat<TMceListItem>) when created but
*     Message view list items are fetched from the msgs when needed in MdcaPoint function
*
*/



// INCLUDE FILES

#include <AknUtils.h>       // AknTextUtils
#include <mtudcbas.h>
#include <msvids.h>
#include <NpdApi.h>         // CNotepadApi
#include "MtmRegistryObserver.h" // MMtmUiDataRegistryObserver
#include <StringLoader.h>   // StringLoader
#include <mce.rsg>
#include <SenduiMtmUids.h>  // mtm uids
#include <eikenv.h>
#include <gulicon.h>
#include <e32property.h>

#include "MceOneRowMessageListArray.h"
#include "MceLogText.h"

#include <MTMStore.h>
#include <commonphoneparser.h>

#include <aknappui.h>
#include <akncontext.h>
#include <aknconsts.h>
#include <e32math.h>

#include <bldvariant.hrh>
#include <featmgr.h>
#include <NumberGrouping.h>
#include <mmsconst.h>
#include <mmsgenutils.h>
#include <mmsclient.h>
#include <mtclreg.h>
#include <muiu.mbg>

// security data caging
#include <data_caging_path_literals.hrh>
#include <miutset.h>
#include <msvapi.h>

#include "MceCommands.hrh"
#include "MceOneRowMessageListArrayLoader.h"
#include <smuthdr.h>
#include <messaginginternalpskeys.h> 


// CONSTANTS

// LOCAL CONSTANTS AND MACROS
const TInt KMceTimeHours    = 23;
const TInt KMceTimeMinutes  = 59;
const TInt KMceTimeSeconds  = 59;
const TInt KMceTimeMicroSeconds  = 999999;
const TInt KMceTimeOneDay   = 1;
const TInt KMceTimeOneWeek  = 7;
const TUid KUidMsvMessageListSortOrder  = {0x10005359};

_LIT( KSortSubjectSkipString, "*:*" );
const TChar KWhiteSpaceCharacter(' ');
const TInt KSortSubjectStringLength = 100;
const TInt KMceLoopCounter  = 200;
const TInt KFirstTimeMceLoopCounter  = 20;
//cmail update
#define KUidMsgTypeCmailMtmVal               0x2001F406

// ================= MEMBER FUNCTIONS =======================


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::NewL
// ----------------------------------------------------
CMceOneRowListItemArrayLoader* CMceOneRowListItemArrayLoader::NewL(
    CMsvSessionPtr aSession )
    {
    return CMceOneRowListItemArrayLoader::NewL(
        0x1002, //KMsvGlobalInBoxIndexEntryId,
        aSession );
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::NewL
// ----------------------------------------------------
CMceOneRowListItemArrayLoader* CMceOneRowListItemArrayLoader::NewL(
    TMsvId aFolderId,
    CMsvSessionPtr aSession )
    {
    CMceOneRowListItemArrayLoader* self = new (ELeave) CMceOneRowListItemArrayLoader( aSession );
    CleanupStack::PushL( self );
    self->ConstructL( aFolderId );
    CleanupStack::Pop( self );
    return self;
    }


// ----------------------------------------------------
// C++ default constructor can NOT contain any code that
// might leave.
// ----------------------------------------------------
CMceOneRowListItemArrayLoader::CMceOneRowListItemArrayLoader(
    CMsvSessionPtr aSession )
:   CActive( CActive::EPriorityIdle ),
    iListType( EHCListTypeTime ),
    iSession( aSession ),
    iEmailFramework(EFalse)
    {
    }


// ----------------------------------------------------
// destructor
// ----------------------------------------------------
CMceOneRowListItemArrayLoader::~CMceOneRowListItemArrayLoader()
    {
    iHCListItemArray.Close();
    if ( &iSession )
        {
        iSession->RemoveObserver( *this );
        }
    Cancel();
    delete iFolderEntry;
    delete iEntriesToBeAdded;
    delete iEntriesReceivedDuringListCreation;
    if ( iEmailFramework )
        {
        delete iCmailSelectionList ;
        }
    iMarkedArray.Close();
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::ConstructL
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::ConstructL(
    TMsvId aFolderId )
    {
    iSession->AddObserverL( *this );
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdFfEmailFramework ) )
        {
        iEmailFramework = ETrue;
        }
    
    FeatureManager::UnInitializeLib();
    iFolderEntry = CMsvEntry::NewL(
        *iSession,
        aFolderId,
        TMsvSelectionOrdering(
            KMsvGroupByStandardFolders, EMsvSortByDateReverse ));
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------
// CMceOneRowListItemArrayLoader::StartL
// ---------------------------------------------------------
//
void CMceOneRowListItemArrayLoader::StartL()
    {
    //Before Starting to prepare the list, set the value to zero.
    TInt r = RProperty::Set( KPSUidMuiu, KMuiuOneRowListPopulated, EFalse );
    if ( r != KErrNone )
        {
        User::LeaveIfError( r );
        }
    Cancel();
    InitialiseSearch();
    delete iEntriesToBeAdded;
    iEntriesToBeAdded = NULL;
    iEntriesToBeAdded = iFolderEntry->ChildrenL();
    iEntryToBeAdded = 0;
    LoadMessagesL();
    QueueLoad();
    }


// ---------------------------------------------------------
// CMceOneRowListItemArrayLoader::FinishL
// ---------------------------------------------------------
//
void CMceOneRowListItemArrayLoader::FinishL()
    {
    if(IsActive())
        {
        // First consume the outstanding completion.
        Cancel();
        }
    
    // Loop until all entries and subtitles has been added to iHCListItemArray
    while ( iEntriesToBeAdded && iEntriesToBeAdded->Count() )
        {
        LoadMessagesL();
        }
    }


// ---------------------------------------------------------
// CMceOneRowListItemArrayLoader::DoCancel
// ---------------------------------------------------------
//
void CMceOneRowListItemArrayLoader::DoCancel()
    {
    }


// ---------------------------------------------------------
// CMceOneRowListItemArrayLoader::RunL
// ---------------------------------------------------------
//
void CMceOneRowListItemArrayLoader::RunL()
    {
    TRAP_IGNORE( LoadMessagesL( ) );
    QueueLoad();
    if(!IsActive())
        {
        }
    }


// ---------------------------------------------------------
// CMceOneRowListItemArrayLoader::QueueLoad
// ---------------------------------------------------------
//
void CMceOneRowListItemArrayLoader::QueueLoad()
    {
    if ( IsActive() )
        {
        return;
        }

    if ( iEntriesToBeAdded->Count() || iEntriesReceivedDuringListCreation )
        {
        TRequestStatus *s = &iStatus;
        User::RequestComplete(s, KErrNone);
        SetActive();
        }
    }


// ---------------------------------------------------------
// CMceOneRowListItemArrayLoader::LoadMessagesL
// ---------------------------------------------------------
//
void CMceOneRowListItemArrayLoader::LoadMessagesL()
    {
    if ( iEntriesToBeAdded->Count() )
        {
        if ( iEmailFramework ) 
            {
            TRAP_IGNORE( CreateCmailEntryIndexL() ) ;
            }
        AppendMessagesL();
        }
    else if ( iEntriesReceivedDuringListCreation )
        {
        Cancel();
        // do something
        const TInt count = iEntriesReceivedDuringListCreation->Count();
        for (TInt i=0; i<count; i++)
            {
            EntryCreatedL( (*iEntriesReceivedDuringListCreation)[i] );
            }
        delete iEntriesReceivedDuringListCreation;
        iEntriesReceivedDuringListCreation = NULL;
        }
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::ItemIndex
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::ItemIndex(TMsvId aItemId) const
    {
    TInt foundItem = KErrNotFound;
    const TInt count = iHCListItemArray.Count();
    for( TInt loop = 0; loop < count && foundItem == KErrNotFound ; loop++)
        {
        if ( iHCListItemArray[loop].iEntryId == aItemId &&
            iHCListItemArray[loop].iItemType == EHListItemEntry )
            {
            foundItem = loop;
            }
        }
    return foundItem;
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::HCItemType
// ----------------------------------------------------
//THListItemType CMceOneRowListItemArray::HCItemType( TInt aIndex )
TInt CMceOneRowListItemArrayLoader::HCItemType(TInt aIndex )
    {
    return iHCListItemArray[aIndex].iItemType;
    }


// ----------------------------------------------------
// CMceOneRowListItemArray::ItemId
// ----------------------------------------------------
TMsvId CMceOneRowListItemArrayLoader::ItemId( TInt aIndex ) const
    {
    TMsvId id = KErrNotFound;
    if ( aIndex >= 0 && iHCListItemArray.Count() > aIndex )
        {
        if ( iHCListItemArray[aIndex].iItemType == EHListItemEntry )
            {
            id = iHCListItemArray[aIndex].iEntryId;
            }
        }

    return id;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::SetEntryL
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::SetEntryL( TMsvId aFolderId )
    {
    iHCListItemArray.Reset();

    iFolderEntry->SetEntryL( aFolderId );
    TMsvSelectionOrdering ordering = iFolderEntry->SortType();
    TMsvSorting sorting = iFolderEntry->SortType().Sorting();
    ordering.SetSorting( sorting );
    iFolderEntry->SetSortTypeL( ordering );
    StartL();
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::Count
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::Count()
    {
    TInt count = 0;
    count = iHCListItemArray.Count();
    return count;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::GetEntry
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::GetEntry( TMsvId aEntryId, TMsvEntry& aEntry )
    {
    TMsvId serviceId;
    TInt error = iSession->GetEntry( aEntryId, serviceId, aEntry );
    return error;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::GetEntry
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::GetEntry( TInt aIndex, TMsvEntry& aEntry )
    {
    if ( aIndex < 0 || aIndex > iHCListItemArray.Count() )
        {
        return KErrNotFound;
        }
    TMsvId serviceId;
    TMsvId entryId;
    entryId = iHCListItemArray[aIndex].iEntryId;

    TInt error = iSession->GetEntry( entryId, serviceId, aEntry );
    if ( error == KErrNone )
        {
        if ( iHCListItemArray[aIndex].iItemType == EHListItemSubtitle )
            {
            error = iListType;
            }
        }
    return error;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::GetTimeBasedSubtitle
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::GetTimeBasedSubtitle( TInt aIndex )
    {
    if ( aIndex < 0 || aIndex > iHCListItemArray.Count() )
        {
        return KErrNotFound;
        }
    TInt value = KErrNotSupported;
    if ( iListType == EHCListTypeTime )
        {
        if ( iHCListItemArray[aIndex].iItemType == EHListItemSubtitle )
            {
            value = iHCListItemArray[aIndex].iSubtitleType;
            }
        else
            {
            TInt subtitleIndex = ParentItemIndex( aIndex );
            if ( subtitleIndex >= 0 )
                {
                value = iHCListItemArray[subtitleIndex].iSubtitleType;
                }
            }
        }
    return value;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::GetCurrentEntry
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::GetCurrentEntry( TMsvEntry& aEntry )
    {
    aEntry = iFolderEntry->Entry();
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::CurrentEntryId
// ----------------------------------------------------
TMsvId CMceOneRowListItemArrayLoader::CurrentEntryId()
    {
    return iFolderEntry->EntryId();
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::SetListItemArrayObserver
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::SetListItemArrayObserver( MMceOneRowListItemArrayObserver* aObserver )
    {
    iListItemArrayObserver = aObserver;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::InitialiseSearch
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::InitialiseSearch()
    {
    iHCListItemArray.Reset();
    iCounter = 0;
    iStep = 0;
    for (TInt i=EMondayAdded; i<=EOlderAdded; i++)
        {
        iSubtitleAdded[i] = EFalse;
        }
    UpdateCurrentTime();
    iAddSubtitle = EFalse;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::HandleSessionEventL
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::HandleSessionEventL(
    TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/)
    {
    MCELOGGER_ENTERFN("CMceOneRowListItemArrayLoader::HandleSessionEventL");
// if active...

    if ( !IsActive() )
        {
        MCELOGGER_LEAVEFN("CMceOneRowListItemArrayLoader::HandleSessionEventL not active");
        return;
        }

    TMsvId changedFolderId = (*(TMsvId*) (aArg2));
    if ( changedFolderId != iFolderEntry->EntryId() )
        {
        // not in our folder...
        return;
        }

    switch (aEvent)
        {
        case EMsvEntriesChanged:
        case EMsvEntriesCreated:
            {
            if ( !iEntriesReceivedDuringListCreation )
                {
                iEntriesReceivedDuringListCreation = new( ELeave ) CMsvEntrySelection();
                }
            CMsvEntrySelection* selection = (CMsvEntrySelection*) aArg1;
            const TInt count = selection->Count();
            for (TInt i=0; i<count; i++)
                {
                iEntriesReceivedDuringListCreation->AppendL( (*selection)[i] );
                }
            }
            break;
        case EMsvEntriesDeleted:
            {
            // ?
            }
        case EMsvEntriesMoved:
            {
            }
            break;
        default:
            break;
        };
    MCELOGGER_LEAVEFN("CMceOneRowListItemArrayLoader::HandleSessionEventL");
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::AppendMessagesL
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::AppendMessagesL()
    {
    switch ( iListType )
        {
        case EHCListTypeTime:
            CreateTimeBasedArrayL();
            break;
        case EHCListTypeSender:
            CreateSenderBasedArrayL();
            break;
        case EHCListTypeSubject:
            CreateSubjectBasedArrayL();
            break;
        case EHCListTypeType:
            CreateTypeBasedArrayL();
            break;
        default:
            CreateTimeBasedArrayL();
            break;
        }
    if(iEntriesToBeAdded->Count() == 0)
        {
	//all the entries are populated successfully
        TInt r = RProperty::Set( KPSUidMuiu, KMuiuOneRowListPopulated, ETrue );
        if ( r != KErrNone )
            {
            User::LeaveIfError( r );
            }
        }
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::CreateTimeBasedArrayL
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::CreateTimeBasedArrayL()
    {
    CArrayFixFlat<TInt>* itemsAdded = new (ELeave) CArrayFixFlat<TInt>(4);
    CleanupStack::PushL( itemsAdded );

    TInt entryCount = 0; // divides list building into smaller pieces
    TBool itemAdded = EFalse;
    TInt loopCounter = KMceLoopCounter;
	if (iCounter == 0)
        {
        //For the first time, when the list is about to get populated.
        loopCounter = KFirstTimeMceLoopCounter;
        }	
    // Loop until all entries and subtitles has been added to iHCListItemArray
    while ( iEntriesToBeAdded->Count() )
        { 
         TBool nonCmailEntry = ETrue; 
         if ( iEmailFramework )
            {
            nonCmailEntry = CheckCmailEntryIndexL ((*iEntriesToBeAdded)[0]);
            }             
         if ( nonCmailEntry )
            {
            iEntryToBeAdded = (*iEntriesToBeAdded)[0];
            iEntriesToBeAdded->Delete( 0 );
            
            // First add folders
            if ( iStep == 0 )
                {
                TMsvEntry entry = iFolderEntry->ChildDataL( iEntryToBeAdded );
                // Find folders 
                while ( entry.iType == KUidMsvFolderEntry )
                    {
                    THCListListItem item;
                    item.iTreeItemId = 0;
                    item.iEntryId = entry.Id();
                    item.iItemType = EHListItemEntry;
                    itemAdded = ETrue;
                    iHCListItemArray.AppendL( item );
                    itemsAdded->AppendL( iHCListItemArray.Count()-1 );
                    entryCount++;
                    iCounter++;
                    if ( iEntriesToBeAdded->Count() )
                        {
                        iEntryToBeAdded = (*iEntriesToBeAdded)[0];
                        iEntriesToBeAdded->Delete( 0 );
                        entry = iFolderEntry->ChildDataL( iEntryToBeAdded );
                        }
                    else
                        {
                        iEntryToBeAdded = 0;
                        // All entries has been checked for folder type
                        break;
                        }
                    }
                }
            
            // Add subtitle
            if ( iAddSubtitle )
                {
                THCListListItem item;
                item.iTreeItemId = 0;
                item.iEntryId = iEntryToBeAdded;
                item.iItemType = EHListItemSubtitle;
                item.iSubtitleType = iSubtitleType;
                iHCListItemArray.AppendL( item );
                itemsAdded->AppendL( iHCListItemArray.Count()-1 );
                iAddSubtitle = EFalse;
                entryCount++;
                }
            
            // iCounter still in valid range
            if ( iEntryToBeAdded > 0 )
                {
                TMsvEntry entry = iFolderEntry->ChildDataL( iEntryToBeAdded );
                
                TTime entryTime = entry.iDate;
                GetTime( entryTime );
                TInt subtitleType = TimeBasedSubtitleType( entryTime, EFalse );
                if ( !iSubtitleAdded[subtitleType] )
                    {
                    iAddSubtitle = ETrue;
                    iSubtitleType = subtitleType;
                    iSubtitleAdded[subtitleType] = ETrue;
                    }
                
                // Add entry item if no subtitle added
                if( !iAddSubtitle )
                    {
                    THCListListItem item;
                    item.iTreeItemId = 0;
                    item.iEntryId = entry.Id();
                    item.iItemType = EHListItemEntry;
                    iHCListItemArray.AppendL( item );
                    itemsAdded->AppendL( iHCListItemArray.Count()-1 );
                    item.iSubtitleType = iSubtitleType;
                    itemAdded = ETrue;
                    entryCount++;
                    }
                else
                    {
                    // Subtitle added: breaks out and after that 
                    // active scheduler returns to add subtitle and entries
                    iEntriesToBeAdded->InsertL( 0, iEntryToBeAdded );
                    iStep = 1;
                    break;
                    }
                
                // This breaks building the array after every KMceLoopCounter entries. 
                // Necessary for not slowing down execution when there are many messages 
                // as this is a part of an active scheduler callback.
                if ( entryCount >= loopCounter)
                    {
                    iCounter++;
                    break;
                    }
                }
            }
           else
            {
            iEntriesToBeAdded->Delete( 0 );
            }
        iCounter++;
        }
	loopCounter = KMceLoopCounter;
    iStep = 1;

    // Update Tree list
    if ( iListItemArrayObserver && itemAdded )
        {
        iListItemArrayObserver->MceListItemArrayChangedL( *itemsAdded );
        }
    CleanupStack::PopAndDestroy( itemsAdded );
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::CreateSenderBasedArrayL
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::CreateSenderBasedArrayL()
    {
    CArrayFixFlat<TInt>* itemsAdded = new (ELeave) CArrayFixFlat<TInt>(4);
    CleanupStack::PushL( itemsAdded );
    TBool itemAdded = EFalse;
    TInt i = 0;
    while ( iEntriesToBeAdded->Count() && i < KMceLoopCounter )
        {
        THCListListItem item;
        item.iTreeItemId = 0;
        TBool nonCmailEntry = ETrue ; 
        if (iEmailFramework )
           {
           nonCmailEntry = CheckCmailEntryIndexL ((*iEntriesToBeAdded)[0]);
           }             
        if ( nonCmailEntry )
           {
           TMsvId entryToBeAdded = (*iEntriesToBeAdded)[0];
           iEntriesToBeAdded->Delete( 0 );
           if ( iEntryToBeAdded <= 0 )
               {
               TMsvEntry entry = iFolderEntry->ChildDataL( entryToBeAdded );
               item.iEntryId = entryToBeAdded;
               if ( entry.iType != KUidMsvFolderEntry )
                   {
                   item.iItemType = EHListItemSubtitle;
                   iHCListItemArray.AppendL( item );
                   itemsAdded->AppendL( iHCListItemArray.Count()-1 );
                   i++;
                   }
               item.iItemType = EHListItemEntry;
               itemAdded = ETrue;
               iHCListItemArray.AppendL( item );
               iEntryToBeAdded = entryToBeAdded;
               itemsAdded->AppendL( iHCListItemArray.Count()-1 );
               }
           else
               {
               TMsvEntry previousEntry = iFolderEntry->ChildDataL( iEntryToBeAdded );
               TMsvEntry currentEntry = iFolderEntry->ChildDataL( entryToBeAdded );
               item.iEntryId = currentEntry.Id();
               if ( currentEntry.iType != KUidMsvFolderEntry &&
                    currentEntry.iDetails.Compare(previousEntry.iDetails) != 0 )
                   {
                   item.iItemType = EHListItemSubtitle;
                   iHCListItemArray.AppendL( item );
                   itemsAdded->AppendL( iHCListItemArray.Count()-1 );
                   i++;
                   }
               item.iItemType = EHListItemEntry;
               itemAdded = ETrue;
               iHCListItemArray.AppendL( item );
               iEntryToBeAdded = entryToBeAdded;
               itemsAdded->AppendL( iHCListItemArray.Count()-1 );
               }
           }
         else
           {
            iEntriesToBeAdded->Delete( 0 );
           }
        iCounter++;
        i++;
        }
    if ( iListItemArrayObserver && itemAdded )
        {
        iListItemArrayObserver->MceListItemArrayChangedL( *itemsAdded );
        }
    CleanupStack::PopAndDestroy( itemsAdded );
    }
    
    
// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::CreateSubjectBasedArrayL
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::CreateSubjectBasedArrayL()
    {
    CArrayFixFlat<TInt>* itemsAdded = new (ELeave) CArrayFixFlat<TInt>(4);
    CleanupStack::PushL( itemsAdded );
    TBool itemAdded = EFalse;
    TInt i = 0;
    while ( iEntriesToBeAdded->Count() && i < KMceLoopCounter )
        {
        THCListListItem item;
        item.iTreeItemId = 0;
        TBool nonCmailEntry = ETrue;
        if (iEmailFramework )
           {
           nonCmailEntry = CheckCmailEntryIndexL ((*iEntriesToBeAdded)[0]);
           }             
        if ( nonCmailEntry )
           {
           TMsvId entryToBeAdded = (*iEntriesToBeAdded)[0];
           iEntriesToBeAdded->Delete( 0 );
           
           if ( iEntryToBeAdded <= 0 )
               {
               TMsvEntry entry = iFolderEntry->ChildDataL( entryToBeAdded );
               item.iEntryId = entryToBeAdded;
               if ( entry.iType != KUidMsvFolderEntry )
                   {
                   item.iItemType = EHListItemSubtitle;
                   iHCListItemArray.AppendL( item );
                   itemsAdded->AppendL( iHCListItemArray.Count()-1 );
                   i++;
                   }
               item.iItemType = EHListItemEntry;
               itemAdded = ETrue;
               iHCListItemArray.AppendL( item );
               iEntryToBeAdded = entryToBeAdded;
               itemsAdded->AppendL( iHCListItemArray.Count()-1 );
               }
           else
               {
               TMsvEntry previousEntry = iFolderEntry->ChildDataL( iEntryToBeAdded );
               TMsvEntry currentEntry = iFolderEntry->ChildDataL( entryToBeAdded );
               item.iEntryId = currentEntry.Id();
               // Remove prefix from current entry description
               TBufC<100> tmpString( currentEntry.iDescription.Left( KSortSubjectStringLength ) );
               const TInt length1 = tmpString.Length();
               TInt start = SkipPrefix( tmpString.Des() );
               TPtrC currentMid = currentEntry.iDescription.Mid( start, length1 - start );
           
               // Remove prefix from previous entry description
               TBufC<100> tmpString2( previousEntry.iDescription.Left( KSortSubjectStringLength ) );
               const TInt length2 = tmpString2.Length();
               start = SkipPrefix( tmpString2.Des() );
               TPtrC prevMid = previousEntry.iDescription.Mid( start, length2 - start );
           
               // Compare descriptions
               if ( currentEntry.iType != KUidMsvFolderEntry &&
                   currentMid.CompareF( prevMid ) != 0 )
                   {
                   item.iItemType = EHListItemSubtitle;
                   iHCListItemArray.AppendL( item );
                   itemsAdded->AppendL( iHCListItemArray.Count()-1 );
                   i++;
                   }
               item.iItemType = EHListItemEntry;
               itemAdded = ETrue;
               iHCListItemArray.AppendL( item );
               iEntryToBeAdded = entryToBeAdded;
               itemsAdded->AppendL( iHCListItemArray.Count()-1 );
               }
           }
         else
            {
            iEntriesToBeAdded->Delete( 0 );
            }
        iCounter++;
        i++;
        }
    if ( iListItemArrayObserver && itemAdded )
        {
        iListItemArrayObserver->MceListItemArrayChangedL( *itemsAdded );
        }
    CleanupStack::PopAndDestroy( itemsAdded );
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::CreateTypeBasedArrayL
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::CreateTypeBasedArrayL()
    {
    CArrayFixFlat<TInt>* itemsAdded = new (ELeave) CArrayFixFlat<TInt>(4);
    CleanupStack::PushL( itemsAdded );
    
    TBool itemAdded = EFalse;
    TInt i = 0;
    while ( iEntriesToBeAdded->Count() && i < KMceLoopCounter )
        {
        THCListListItem item;
        item.iTreeItemId = 0;
        TBool nonCmailEntry = ETrue;
        if (iEmailFramework )
           {
           nonCmailEntry = CheckCmailEntryIndexL ((*iEntriesToBeAdded)[0]);
           }             
        if ( nonCmailEntry )
           {    
            TMsvId entryToBeAdded = (*iEntriesToBeAdded)[0];
            iEntriesToBeAdded->Delete( 0 );
            item.iEntryId = entryToBeAdded;
            item.iItemType = EHListItemEntry;
            itemAdded = ETrue;
            iHCListItemArray.AppendL( item );
            itemsAdded->AppendL( iHCListItemArray.Count()-1 );
           }
        else
           {
            iEntriesToBeAdded->Delete( 0 );
           }  
        iCounter++;
        i++;
        }
    if ( iListItemArrayObserver && itemAdded )
        {
        iListItemArrayObserver->MceListItemArrayChangedL( *itemsAdded );
        }
    CleanupStack::PopAndDestroy( itemsAdded );
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::SortType
// ----------------------------------------------------
THCListListType CMceOneRowListItemArrayLoader::SortTypeHCList()
    {
    return iListType;
    }


// ----------------------------------------------------
// CMceMessageListItemArray::SortType
// returns folder's sort type
//
// ----------------------------------------------------
TMsvSelectionOrdering CMceOneRowListItemArrayLoader::SortType(  )
    {
    return iFolderEntry->SortType();
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::SetOrderingL
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::SetOrderingL( TInt aSortType, TBool aOrdering )
    {
    TMsvSorting sorting = EMsvSortByDateReverse;
    TInt grouping = KMsvGroupByStandardFolders;
    switch ( aSortType )
        {
        case EMceCmdSortByType:
            grouping = KMsvGroupByStandardFolders|KMsvGroupByMtm;
            sorting = aOrdering ? EMsvSortByIdReverse: EMsvSortById;
            iListType = EHCListTypeType;
            break;
        case EMceCmdSortByRecipient:
        case EMceCmdSortBySender:
            sorting = aOrdering ? EMsvSortByDetailsReverse: EMsvSortByDetails;
            iListType = EHCListTypeSender;
            break;
        case EMceCmdSortBySubject:
            sorting = aOrdering ? EMsvSortByDescriptionReverse: EMsvSortByDescription;
            iListType = EHCListTypeSubject;
            break;
        case EMceCmdSortByDate:
        default:
            sorting = aOrdering ? EMsvSortByDate: EMsvSortByDateReverse;
            iListType = EHCListTypeTime;
            break;
        }

    const TMsvSelectionOrdering newOrdering( grouping, sorting, EFalse );
    iFolderEntry->SetSortTypeL( newOrdering );
    // Ignore possible error. If this leaves worst thing is that ordering is not saved to root for message iterator.
    TRAP_IGNORE( SaveOrderingL( newOrdering ) );
    iPreviousSelectionOrdering = newOrdering;
    StartL();
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::SaveOrderingL
// Write the sorting order into the message store, MUIU accesses and
// uses this for CMessageIterator
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::SaveOrderingL( const TMsvSelectionOrdering& aNewOrdering ) const
    {
    // TMsvSelectionOrdering class does not have != operator
    if ( !(iPreviousSelectionOrdering == aNewOrdering ) )
        {
        CMsvEntry* entry=iSession->GetEntryL( KMsvRootIndexEntryId ); // Access the entry
        CleanupStack::PushL( entry );
        CMsvStore* store = entry->EditStoreL();
        CleanupStack::PushL( store );
        RMsvWriteStream writeStream;
        writeStream.AssignLC( *store, KUidMsvMessageListSortOrder );
        aNewOrdering.ExternalizeL( writeStream );
        writeStream.CommitL();
        store->CommitL();
        CleanupStack::PopAndDestroy( 3 );
        }
    }


// ----------------------------------------------------
// CMceMessageListItemArray::DeleteDiscard
// return ETrue, if delete command should be discarded
//
// ----------------------------------------------------
TBool CMceOneRowListItemArrayLoader::DeleteDiscard( TMsvId aItemId ) const
    {
    TMsvEntry entry;
    TMsvId serviceId;
    if ( iSession->GetEntry(aItemId,serviceId,entry) != KErrNone )
        {
        return ETrue;
        }

    //mail or syncml folder cannot be deleted
    if ( ( entry.iType == KUidMsvFolderEntry ) &&
        ( entry.iMtm == KSenduiMtmImap4Uid ||
        entry.iMtm == KSenduiMtmSyncMLEmailUid ) )
        {
        return ETrue;
        }

    if ( iFolderEntry->Entry().Id() == KMsvGlobalOutBoxIndexEntryId )
        {
        if ( entry.SendingState() == KMsvSendStateSending )
            {
            return ETrue;
            }

        // Message in waiting state cannot be deleted except e-mail.
        // For e-mail, waiting, resend and scheduled states are presented
        // to user as queued state (by CSmtpMtmUiData). Delete is allowed
        // from the queued state.
        if ( ( entry.SendingState() == KMsvSendStateWaiting ) &&
            ( entry.iMtm != KSenduiMtmSmtpUid ) )
            {
            return ETrue;
            }

        if ( entry.iMtm == KSenduiMtmSmsUid ||
            entry.iMtm == KSenduiMtmMmsUid )
            {
            if ( entry.Connected() || ( entry.SendingState() == KMsvSendStateUnknown )
                || ( entry.SendingState() == KMsvSendStateNotApplicable ) )
                {
                return ETrue;
                }
            }
        }
    return EFalse;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::IsItemFolder
// ----------------------------------------------------
TBool CMceOneRowListItemArrayLoader::IsItemFolder( TMsvId aItemId ) const
    {
    TBool isFolder = EFalse;
    TMsvId serviceId;
    TMsvEntry entry;
    if ( iSession->GetEntry( aItemId, serviceId, entry ) == KErrNone &&
         entry.iType == KUidMsvFolderEntry )
        {
        isFolder = ETrue;
        }
    return isFolder;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::TreeItemId
// ----------------------------------------------------
TAknTreeItemID CMceOneRowListItemArrayLoader::TreeItemId( TInt aIndex )
    {
    return iHCListItemArray[aIndex].iTreeItemId;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::MsvItemId
// ----------------------------------------------------
TMsvId CMceOneRowListItemArrayLoader::MsvItemId( TAknTreeItemID aTreeItemId )
    {
    TMsvId foundItem = KErrNotFound;
    const TInt count = iHCListItemArray.Count();
    for( TInt loop = 0; loop < count && foundItem == KErrNotFound ; loop++)
        {
        if ( iHCListItemArray[loop].iTreeItemId == aTreeItemId &&
            iHCListItemArray[loop].iItemType == EHListItemEntry )
            {
            foundItem = iHCListItemArray[loop].iEntryId;
            }
        }
    return foundItem;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::ParentItemId
// ----------------------------------------------------
TAknTreeItemID CMceOneRowListItemArrayLoader::ParentItemId( TInt aIndex )
    {
    if ( aIndex < 0 || aIndex > iHCListItemArray.Count() )
        {
        return KAknTreeIIDNone;
        }
    TInt i = aIndex;
    TAknTreeItemID parentId = KAknTreeIIDRoot;

    if ( iHCListItemArray[i].iItemType == EHListItemEntry )
        {
        // if item is subtitle row, then return root, but since it is message, search its subtitle row
        while ( i >= 0 && iHCListItemArray[i].iItemType == EHListItemEntry )
            {
            i--;
            }
        if ( i >= 0 )
            {
            parentId = iHCListItemArray[i].iTreeItemId;
            }
        }
    return parentId;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::ParentItemIndex
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::ParentItemIndex( TInt aIndex )
    {
    if ( aIndex < 0 || aIndex > iHCListItemArray.Count() )
        {
        return KErrNotFound;
        }
    TInt i = aIndex;
    if ( iHCListItemArray[i].iItemType == EHListItemEntry )
        {
        // if item is subtitle row, then return root, but since it is message, search its subtitle row
        while ( i >= 0 && iHCListItemArray[i].iItemType == EHListItemEntry )
            {
            i--;
            }
        }
    return i;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::SetTreeItemId
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::SetTreeItemId( TInt aIndex, TAknTreeItemID aTreeItemId )
    {
    iHCListItemArray[aIndex].iTreeItemId = aTreeItemId;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::ResetAllTreeItemId
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::ResetAllTreeItemId()
    {
    TInt i = iHCListItemArray.Count()-1;
    while ( i >= 0 )
        {
        iHCListItemArray[i].iTreeItemId = 0;
        i--;
        }
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::Compare
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::Compare( TAknTreeItemID aFirst, TAknTreeItemID aSecond )
    {
    TInt firstIndex = KErrNotFound;
    TInt secondIndex = KErrNotFound;

    TInt i = iHCListItemArray.Count()-1;
    while ( i >= 0 &&  ( firstIndex < 0 || secondIndex < 0 ) )
        {
        if ( iHCListItemArray[i].iTreeItemId == aFirst )
            {
            firstIndex = i;
            }
        if ( iHCListItemArray[i].iTreeItemId == aSecond )
            {
            secondIndex = i;
            }
        i--;
        }

    if ( firstIndex > KErrNotFound && secondIndex > KErrNotFound )
        {
        return firstIndex - secondIndex;
        }
    else
        {
        return 0;
        }
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::ItemIndexTreeId
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::ItemIndexTreeId( TAknTreeItemID aTreeItemId )
    {
    TInt i = iHCListItemArray.Count()-1;
    while ( i >= 0 && iHCListItemArray[i].iTreeItemId != aTreeItemId )
        {
        i--;
        }
    return i;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::SetItemMarked
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::SetItemMarked( TAknTreeItemID aFocused, TBool aState )
    {
    TInt index = ItemIndexTreeId( aFocused );

    TInt subItemCount = GetSubfolderItemCount( aFocused );

    if ( subItemCount != KErrNotFound && iHCListItemArray[index].iItemType == EHListItemSubtitle )
        {
        while ( index <= subItemCount )
            {
            iHCListItemArray[index].iMarked = aState;
            index++;
            }
        }
    else if ( aFocused == 1 )
        {
        TInt i = iHCListItemArray.Count()-1;
        while ( i >= 0)
            {
            iHCListItemArray[i].iMarked = aState;
            i--;
            }
        }
    else
        {
        // Set item marked
        iHCListItemArray[index].iMarked = aState;
        }
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::GetItemMarkStatusL
// ----------------------------------------------------
TBool CMceOneRowListItemArrayLoader::GetItemMarkStatusL( TMsvId aEntryId )
    {
    TInt markedCount = iMarkedArray.Count();
    TInt index = iMarkedArray.Find( aEntryId );
    if ( index > KErrNotFound )
        {
        iMarkedArray.Remove( index );
        return ETrue;
        }
   
    return EFalse;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::GetSubfolderItemCount
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::GetSubfolderItemCount( TAknTreeItemID aParent )
    {
    TInt itemCount = 0;
    TInt index = ItemIndexTreeId( aParent );
    if ( index != KErrNotFound && iHCListItemArray[index].iItemType == EHListItemSubtitle )
        {
        TInt arrCount = iHCListItemArray.Count()-1;
        while ( index < arrCount && iHCListItemArray[index + 1].iItemType != EHListItemSubtitle )
            {
            index++;
            itemCount++;
            }
        }
    else
        {
        return KErrNotFound;
        }
    return itemCount;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::ResetTreeItemId
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::ResetTreeItemId( TAknTreeItemID aTreeItemId )
    {
    TInt index = ItemIndexTreeId( aTreeItemId );
    if ( index >= 0 )
        {
        iHCListItemArray[index].iTreeItemId = 0;
        }
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::FolderEntry
// ----------------------------------------------------
const TMsvEntry& CMceOneRowListItemArrayLoader::FolderEntry() const
    {
    return iFolderEntry->Entry();
    }


// ----------------------------------------------------
// CMceMessageListItemArray::GetParentFolder
// ----------------------------------------------------
TMsvId CMceOneRowListItemArrayLoader::GetParentFolderId() const
    {
    return iFolderEntry->Entry().Parent();
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::FindFirstUnreadMessageL
// ----------------------------------------------------
TBool CMceOneRowListItemArrayLoader::FindFirstUnreadMessageL( TMsvId& aEntryId ) const
    {
    aEntryId = KErrNotFound;
    TInt numberOfUnreadMessages = 0;
    const TInt itemCount = iFolderEntry->Count();
    for ( TInt loop = 0; loop < itemCount; loop++ )
        {
        if ( (*iFolderEntry)[loop].iType == KUidMsvMessageEntry &&
             (*iFolderEntry)[loop].Unread() )
            {
            if ( aEntryId == KErrNotFound )
                {
                aEntryId = (*iFolderEntry)[loop].Id();
                }
            numberOfUnreadMessages++;
            if ( numberOfUnreadMessages > 1 )
                {
                // we are only interested of first unread message and info that if
                // there are more than one unread messages.
                // So we can stop if there are more than one.
                break;
                }
            }
        }
    return (numberOfUnreadMessages == 1);
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::FindUnreadMessageUnderSubtitleL
// ----------------------------------------------------
TBool CMceOneRowListItemArrayLoader::FindUnreadMessageUnderSubtitleL( TAknTreeItemID aSubtitleID )
    {
    TInt itemCount = GetSubfolderItemCount( aSubtitleID );

    TInt startIndex = ItemIndexTreeId( aSubtitleID ) + 1;
    for ( TInt loop = startIndex ; loop < itemCount + startIndex ; loop++ )
        {
        TMsvId entryId = iHCListItemArray[loop].iEntryId;
        TMsvEntry entry; 
        TInt error = GetEntry(entryId, entry );
        if ( !error && entry.Unread() ) 
            {
            return ETrue;
            }
        }
    return EFalse;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::FolderCount
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::FolderCount()
    {
    TInt numberOfFolders = 0;
    const TInt itemCount = iFolderEntry->Count();
    for ( TInt loop = 0; loop < itemCount; loop++ )
        {
        if ( (*iFolderEntry)[loop].iType == KUidMsvFolderEntry )
            {
            numberOfFolders++;
            }
        else if ( (*iFolderEntry)[loop].iType == KUidMsvMessageEntry )
            {
            // folders are grouped first, so we can stop when first message is reached.
            break; //loop = itemCount;
            }
        }
    return numberOfFolders;
    }


// ----------------------------------------------------
// CMceMessageListItemArray::MessageCount
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::MessageCount()
    {
    TInt numberOfMessages = 0;
    const TInt itemCount = iFolderEntry->Count();
    for ( TInt loop = 0; loop < itemCount; loop++ )
        {
        if ( (*iFolderEntry)[loop].iType == KUidMsvMessageEntry )
            {
            numberOfMessages++;
            }
        }
    return numberOfMessages;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::IsConnected
// return ETrue, if account is connected
//
// ----------------------------------------------------
TBool CMceOneRowListItemArrayLoader::IsConnected() const
    {
    TMsvEntry entry;
    entry = iFolderEntry->Entry();

    TMsvEntry serviceEntry;
    TMsvId serviceId;
    if ( iSession->GetEntry(entry.iServiceId,serviceId,serviceEntry) == KErrNone )
        {
        return serviceEntry.Connected();
        }
    else
        {
        return EFalse;
        }
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::FindEntryIndex
// Finds entry index in message list
//
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::FindEntryIndex( const TDesC& aMatchString ) const
    {
    TInt foundIndex = KErrNotFound;
    TMsvSelectionOrdering ordering( KMsvNoGrouping, EMsvSortByDateReverse, EFalse );
    ordering = iFolderEntry->SortType();

    const TInt count = iFolderEntry->Count();
    const TInt length = aMatchString.Length();

    if ( ( ordering.Sorting() == EMsvSortByDetails ) ||
         ( ordering.Sorting() == EMsvSortByDetailsReverse ) )
        {
        for( TInt loop = 0; loop < count && foundIndex == KErrNotFound ; loop++ )
            {
            if ( aMatchString.CompareF( ( *iFolderEntry )[loop].iDetails.Left( length ) ) == 0 )
                {
                foundIndex = loop;
                }
            }
        }
    else if ( ( ordering.Sorting() == EMsvSortByDescription ) ||
              ( ordering.Sorting() == EMsvSortByDescriptionReverse ) )
        {
        for( TInt loop = 0; loop < count && foundIndex == KErrNotFound ; loop++ )
            {
            TBufC<100> tmpString( ( *iFolderEntry )[loop].iDescription.Left(KSortSubjectStringLength) );
            TInt start = SkipPrefix( tmpString.Des() );
            if ( start < tmpString.Length() && tmpString.Length() >= length )
                {
                TPtrC mid = (*iFolderEntry)[loop].iDescription.Mid( start, length );
                if ( aMatchString.CompareF( mid ) == 0 )
                    {
                    foundIndex = loop;
                    }
                }
            }
        }
    else
        {
        //EMsvSortByReverse, nothing cannot be searched
        }
    return foundIndex;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::SkipPrefix
// Removes the "Re:" or "Fw:" prefixes from the given
// text string
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::SkipPrefix( TPtrC aTextString ) const
    {
    TPtrC searchString( aTextString );
    TPtrC searchStringOrig( aTextString );
    TInt matchInd = searchString.Match( KSortSubjectSkipString );
    TInt matchStart = 0;
    TInt lenRight = searchString.Length() - matchInd - 1;

    // Find the last prefix character
    while ( matchInd != KErrNotFound  && lenRight > 0 )
        {
        TPtrC searchString2 = searchString.Right( lenRight );
        matchStart += matchInd + 1;
        matchInd = searchString2.Match( KSortSubjectSkipString );
        lenRight = searchString2.Length() - matchInd - 1;
        searchString.Set( searchString2 );
        }

    // Skip the white space after the prefix character
    matchInd = searchStringOrig.LocateReverse( KWhiteSpaceCharacter );
    if ( matchInd != KErrNotFound && matchStart == matchInd )
        {
        ++matchStart;
        }
    return matchStart;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::EntryCreatedL
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::EntryCreatedL( TMsvId aEntryId )
    {
    MCELOGGER_ENTERFN("CMceOneRowListItemArrayLoader::EntryCreatedL");
    if ( IsActive() )
        {
        MCELOGGER_WRITE("Already active, ignore");
        MCELOGGER_LEAVEFN("CMceOneRowListItemArrayLoader::EntryCreatedL");
        return 0;
        }
    // how to make sure iFolderEntry is already updated?
    MCELOGGER_WRITE_FORMAT("aEntryId = 0x%x", aEntryId );
    CMsvEntry* folderEntry = CMsvEntry::NewL(
        *iSession,
        iFolderEntry->EntryId(),
        iFolderEntry->SortType() );
    CleanupStack::PushL( folderEntry );

    TInt entryIndex = KErrNotFound;
    TInt i = 0;
    const TInt count = folderEntry->Count();
#ifdef _DEBUG
    while ( i < folderEntry->Count() )
        {
        MCELOGGER_WRITE_FORMAT2("(*folderEntry)[%d].Id() = 0x%x", i, (*folderEntry)[i].Id() );
        i++;
        }
    i = 0;
#endif
    while ( i < folderEntry->Count() && entryIndex == KErrNotFound )
        {
        if ( (*folderEntry)[i].Id() == aEntryId )
            {
            MCELOGGER_WRITE_FORMAT("found entry at index %d", i );
            entryIndex = i;
            }
        i++;
        }
    CleanupStack::Pop( folderEntry );
    delete iFolderEntry;
    iFolderEntry = folderEntry;

    TInt createdCount = 0;

    if ( entryIndex > KErrNotFound )
        {
        createdCount = InsertEntryL( entryIndex, aEntryId );
        }

    MCELOGGER_LEAVEFN("CMceOneRowListItemArrayLoader::EntryCreatedL");
    return createdCount;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::InsertEntryL
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::InsertEntryL( TInt aEntryIndex, TMsvId aEntryId )
    {
    MCELOGGER_ENTERFN("CMceOneRowListItemArrayLoader::InsertEntryL");
    MCELOGGER_WRITE_FORMAT("aEntryId: 0x%x", aEntryId );
    MCELOGGER_WRITE_FORMAT("aEntryIndex: %d", aEntryIndex );
    iDuplicate = EFalse;
    TInt previous = FindPreviousFromArrayL( aEntryId );
    if ( iDuplicate )
        {
        // Entry already exist in array return without creating a new instance
        return 0;    
        }
    TInt next = FindNextFromArrayL( aEntryId );

    if ( iDuplicate )
        {
        // Entry already exist in array return without creating a new instance
        return 0;    
        }

    MCELOGGER_WRITE_FORMAT2("previous index: %d, next index %d", previous, next );
    TBool previousAnchor = EFalse;
    
    if ( iFolderEntry->operator[](aEntryIndex).iType == KUidMsvFolderEntry )
        {
        TInt count = InsertFolderEntryL( aEntryIndex, aEntryId );
        MCELOGGER_LEAVEFN("CMceOneRowListItemArrayLoader::InsertEntryL - folder insert");
        return count;
        }

    TInt needSubtitle = ETrue;

    TMsvId insertId = KErrNotFound;
    
    // Index to end of the array by default
    TInt insertIndex = iHCListItemArray.Count();
    
    // Sorting by type don't have subtitles
    if ( iListType == EHCListTypeType )
        {
        needSubtitle = EFalse;
        if ( next > 0 )
            {
            insertIndex = next-1;
            }
        }

    // Find closest entry with same subtitle 
    if ( needSubtitle )
        {
        // Search subtitle for preceding entry
        if ( previous > KErrNotFound )
            {
            if ( SameSubtitle( aEntryIndex, previous ) )
                {
                MCELOGGER_WRITE_FORMAT("Prev index %d has SAME subtitle", previous );
                needSubtitle = EFalse;
                insertId = iFolderEntry->operator[](previous).Id();
                previousAnchor = ETrue;
                MCELOGGER_WRITE_FORMAT("Id 0x%x has SAME subtitle", insertId );
                }
            }
        // Search subtitle for following entry
        if ( needSubtitle && next > KErrNotFound )
            {
            insertId = iFolderEntry->operator[](next).Id();
            if ( SameSubtitle( aEntryIndex, next ) )
                {
                MCELOGGER_WRITE_FORMAT("Next index %d has SAME subtitle", next );
                needSubtitle = EFalse;
                previousAnchor = EFalse;
                MCELOGGER_WRITE_FORMAT("Id 0x%x has SAME subtitle", insertId );
                }
            }
        }

#ifdef _DEBUG
    TInt tempcount = iHCListItemArray.Count();
    MCELOGGER_WRITE_FORMAT("array before add, count %d", tempcount );
    for( TInt loop = 0; loop < tempcount; loop++)
        {
        if ( iHCListItemArray[loop].iItemType == EHListItemEntry )
            {
            MCELOGGER_WRITE_FORMAT2("%d entry    0x%x", loop, iHCListItemArray[loop].iEntryId );
            }
        else
            {
            MCELOGGER_WRITE_FORMAT2("%d subtitle 0x%x", loop, iHCListItemArray[loop].iEntryId );
            }
        }
#endif


    // First index and no following entry
    if ( aEntryIndex == 0 && insertId <= 0 )
        {
        insertIndex = 0; // to the beginning...
        }

    // If neighbour entry
    if ( insertId > 0 )
        {
        MCELOGGER_WRITE_FORMAT("try to find entry 0x%x", insertId );
        const TInt count = iHCListItemArray.Count();

        // First find entry which already exists in iHCListItemArray
        for( TInt loop = 0; loop < count; loop++)
            {
            if ( iHCListItemArray[loop].iEntryId == insertId &&
                 iHCListItemArray[loop].iItemType == EHListItemEntry )
                {
                MCELOGGER_WRITE_FORMAT("FOUND message index %d", loop );
                insertIndex = loop;

                if ( previousAnchor )
                    {
                    insertIndex++;
                    }
                
                break;
                }
            }
        // next find corresponding subtitle row
        while ( needSubtitle && insertIndex >= 0 
            && iHCListItemArray[insertIndex].iItemType != EHListItemSubtitle )
            {
            insertIndex--;
            }
        MCELOGGER_WRITE_FORMAT("subtitle index %d", insertIndex );
        }

    
    // Insert entry item 
    THCListListItem item;
    item.iTreeItemId = 0;
    item.iEntryId = aEntryId;
    item.iItemType = EHListItemEntry;
    iHCListItemArray.InsertL( item, insertIndex );
    TInt createdCount = 1;
    MCELOGGER_WRITE_FORMAT("Inserted item to %d", insertIndex );

    // Add subtitle if needed
    if ( needSubtitle )
        {
        item.iItemType = EHListItemSubtitle;
        if ( iListType == EHCListTypeTime )
            {
            TTime entryTime = iFolderEntry->operator[](aEntryIndex).iDate;
            GetTime( entryTime );
            item.iSubtitleType = TimeBasedSubtitleType( entryTime, ETrue );
            }
        else
            {
            item.iSubtitleType = 0;
            }
        
        iHCListItemArray.InsertL( item, insertIndex );
        MCELOGGER_WRITE_FORMAT("Inserted subtitle to %d", insertIndex );
        createdCount++;
        }

    // Handle entries that has been received during the list creation
    if ( iEntriesReceivedDuringListCreation && iListItemArrayObserver )
        {
        CArrayFixFlat<TInt>* itemsAdded = new (ELeave) CArrayFixFlat<TInt>(4);
        CleanupStack::PushL( itemsAdded );
        itemsAdded->AppendL( insertIndex );
        if ( needSubtitle )
            {
            itemsAdded->AppendL( insertIndex+1 );
            }
        iListItemArrayObserver->MceListItemArrayChangedL( *itemsAdded );
        CleanupStack::PopAndDestroy( itemsAdded );
        }

#ifdef _DEBUG
    tempcount = iHCListItemArray.Count();
    MCELOGGER_WRITE_FORMAT("array AFTER add, count %d", tempcount );
    for( TInt loop = 0; loop < tempcount; loop++)
        {
        if ( iHCListItemArray[loop].iItemType == EHListItemEntry )
            {
            MCELOGGER_WRITE_FORMAT2("%d entry    0x%x", loop, iHCListItemArray[loop].iEntryId );
            }
        else
            {
            MCELOGGER_WRITE_FORMAT2("%d subtitle 0x%x", loop, iHCListItemArray[loop].iEntryId );
            }
        }
#endif

    MCELOGGER_LEAVEFN("CMceOneRowListItemArrayLoader::InsertEntryL");
    return createdCount;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::SameSubtitle
// ----------------------------------------------------
TBool CMceOneRowListItemArrayLoader::SameSubtitle( TInt aIndex1, TInt aIndex2 )
    {
    TBool sameSubtitle = EFalse;

    // Time based subtitles
    if ( iListType == EHCListTypeTime )
        {

        TTime entryDate1 = iFolderEntry->operator[](aIndex1).iDate;
        GetTime( entryDate1 );
        TTime entryDate2 = iFolderEntry->operator[](aIndex2).iDate;
        GetTime( entryDate2 );
        TInt subtitle1 = TimeBasedSubtitleType( entryDate1, ETrue );
        TInt subtitle2 = TimeBasedSubtitleType( entryDate2, EFalse );

        if ( subtitle1 == subtitle2 )
            {
            sameSubtitle = ETrue;
            }
        }

    // Sender based subtitles
    else if ( iListType == EHCListTypeSender )
        {
        TMsvEntry previousEntry = iFolderEntry->operator[](aIndex1);
        TMsvEntry currentEntry = iFolderEntry->operator[](aIndex2);
        if ( currentEntry.iDetails.Compare(previousEntry.iDetails) == 0 )
            {
            sameSubtitle = ETrue;
            }
        }

    // Subject based subtitles
    else if ( iListType == EHCListTypeSubject )
        {
        TMsvEntry previousEntry = iFolderEntry->operator[](aIndex1);
        TMsvEntry currentEntry = iFolderEntry->operator[](aIndex2);

        // Remove prefix from current entry description
        TBufC<100> tmpString( currentEntry.iDescription.Left( KSortSubjectStringLength ) );
        const TInt length1 = tmpString.Length();
        TInt start = SkipPrefix( tmpString.Des() );
        TPtrC currentMid = currentEntry.iDescription.Mid( start, length1 - start );

        //Remove prefix from previous entry description
        TBufC<100> tmpString2( previousEntry.iDescription.Left( KSortSubjectStringLength ) );
        const TInt length2 = tmpString2.Length();
        start = SkipPrefix( tmpString2.Des() );
        TPtrC prevMid = previousEntry.iDescription.Mid( start, length2 - start );

        // Compare descriptions
        if ( currentMid.CompareF( prevMid ) == 0 )
             {
             sameSubtitle = ETrue;
             }
        }

    // Type based sorting don't have subtitles. Return ETrue so new subtitle are not created
    else if ( iListType == EHCListTypeType )
        {
        sameSubtitle = ETrue;
        }

    return sameSubtitle;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::InsertFolderEntryL
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::InsertFolderEntryL( TInt aEntryIndex, TMsvId aEntryId )
    {
    MCELOGGER_ENTERFN("CMceOneRowListItemArrayLoader::InsertFolderEntryL");
    THCListListItem item;
    item.iTreeItemId = 0;
    item.iEntryId = aEntryId;
    item.iItemType = EHListItemEntry;
    iHCListItemArray.InsertL( item, aEntryIndex );
    MCELOGGER_WRITE_FORMAT("Inserted item to %d", aEntryIndex );
    MCELOGGER_LEAVEFN("CMceOneRowListItemArrayLoader::InsertFolderEntryL");
    return 1;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::EntryDeletedL
// ----------------------------------------------------
TBool CMceOneRowListItemArrayLoader::EntryDeletedL(
    TMsvId aEntryId,
    TAknTreeItemID& aTreeItemId,
    TAknTreeItemID& aParentTreeItemId )
    {
    aParentTreeItemId = KAknTreeIIDNone;
    aTreeItemId = KAknTreeIIDNone;
    TBool deleteParent = EFalse;
    TInt itemIndex = ItemIndex( aEntryId );
    if ( itemIndex >= 0 )
        {
        aTreeItemId = TreeItemId( itemIndex );
        TAknTreeItemID parentTreeItemId = ParentItemId( itemIndex );

        iHCListItemArray.Remove( itemIndex );

        if ( parentTreeItemId > KAknTreeIIDRoot )
            {
            TInt parentIndex = ItemIndexTreeId( parentTreeItemId );
            if ( parentIndex >= 0 )
                {
                TInt i = parentIndex+1;
                TInt count = iHCListItemArray.Count();
                if( i < count && iHCListItemArray[i].iItemType == EHListItemEntry )
                    {
                    i++;
                    }
                if ( i-parentIndex == 1 )
                    {
                    // we must delete parent also
                    iHCListItemArray.Remove( parentIndex );
                    deleteParent = ETrue;
                    }
                }
            aParentTreeItemId = parentTreeItemId;
            }
        }
    return deleteParent;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::GetTime
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::GetTime( TTime& aEntryTime ) const
    {
    TTime homeTime;
    homeTime.HomeTime();

    TTime timeNow;
    timeNow.UniversalTime();

    TTimeIntervalMinutes timeZone;
    if ( homeTime.MinutesFrom(timeNow, timeZone) == KErrNone )
        {
        aEntryTime = aEntryTime + timeZone;
        }
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::GetCurrentTime
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::GetCurrentTime( TTime& aCurrentTime ) const
    {
    TTime timeNow;
    timeNow.HomeTime();
    TDateTime dateTime = timeNow.DateTime();
    dateTime.SetHour( KMceTimeHours );
    dateTime.SetMinute( KMceTimeMinutes );
    dateTime.SetSecond( KMceTimeSeconds );
    dateTime.SetMicroSecond( KMceTimeMicroSeconds );
    aCurrentTime = dateTime;
    }   


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::FindPreviousFromArrayL
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::FindPreviousFromArrayL( TMsvId aId )
    {
    // Get snapshot from message store
    CMsvEntrySelection* children = iFolderEntry->ChildrenL();
    CleanupStack::PushL( children );
    
    // Find index in message store
    TInt index = children->Find( aId );
    TMsvId prevId = KErrNotFound;
    
    if ( index == KErrNotFound )
        {
        CleanupStack::PopAndDestroy( children );
        return KErrNotFound;
        }
    
    TInt foundItem = KErrNotFound;
    // Go through previous entry indexes in array until end or match found
    while ( index >= 0 && foundItem == KErrNotFound )
        {
        prevId = (*children)[index];
        const TInt count = iHCListItemArray.Count();
        for( TInt loop = 0; loop < count && foundItem == KErrNotFound ; loop++)
            {
            // Previous entry exists in list
            if ( iHCListItemArray[loop].iEntryId == prevId &&
                iHCListItemArray[loop].iItemType == EHListItemEntry )
                {
                foundItem = loop;
                }

            // Entry with same Id exists already
            if ( iHCListItemArray[loop].iEntryId == aId &&
                iHCListItemArray[loop].iItemType == EHListItemEntry )
                {
                iDuplicate = ETrue;                
                }                
            }
        if ( foundItem == KErrNotFound )
            {
            index--;
            }
        }
    // Previous item found, return id
    if ( foundItem > KErrNotFound )
        {
        foundItem = children->Find( prevId ); 
        }
    CleanupStack::PopAndDestroy( children );
    return foundItem;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::FindNextFromArrayL
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::FindNextFromArrayL( TMsvId aId )
    {
    CMsvEntrySelection* children = iFolderEntry->ChildrenL();
    CleanupStack::PushL( children );
    
    TInt index = children->Find( aId );
    TMsvId nextId = KErrNotFound;
    
    if ( index == KErrNotFound )
        {
        CleanupStack::PopAndDestroy( children );
        return KErrNotFound;
        }
    
    const TInt childrenCount = children->Count();
    TInt foundItem = KErrNotFound;
    while ( index < childrenCount && foundItem == KErrNotFound )
        {
        nextId = (*children)[index];
        const TInt count = iHCListItemArray.Count();
        for( TInt loop = 0; loop < count && foundItem == KErrNotFound ; loop++)
            {
            if ( iHCListItemArray[loop].iEntryId == nextId &&
                iHCListItemArray[loop].iItemType == EHListItemEntry )
                {
                foundItem = loop;
                }

            if ( iHCListItemArray[loop].iEntryId == aId &&
                iHCListItemArray[loop].iItemType == EHListItemEntry )
                {
                iDuplicate = ETrue;
                }
            }
        if ( foundItem == KErrNotFound )
            {
            index++;
            }
        }
        
    if ( foundItem > KErrNotFound )
        {
        foundItem = children->Find( nextId ); 
        }
    CleanupStack::PopAndDestroy( children );
    return foundItem;
    }


// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::TimeBasedSubtitleType
// ----------------------------------------------------
TInt CMceOneRowListItemArrayLoader::TimeBasedSubtitleType( TTime& aTime, TBool aUpdateCurrentTime )
    {
    if ( aUpdateCurrentTime )
        {
        UpdateCurrentTime();
        }
    TInt timeBasedSubtitleType = 0;
    
    if ( aTime >= iToday )
        {
        timeBasedSubtitleType = ETodayAdded;
        }
    else if ( aTime >= iYesterday )
        {
        timeBasedSubtitleType = EYesterdayAdded;
        }
    else if ( aTime < iTwoWeeksAgo )
        {
        timeBasedSubtitleType = EOlderAdded;
        }
    else if ( aTime < iOneWeekAgo )
        {
        timeBasedSubtitleType = ELastWeekAdded;
        }
    else
        {
        timeBasedSubtitleType = aTime.DayNoInWeek();
        }
    return timeBasedSubtitleType;
    }
    
    
// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::UpdateCurrentTime
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::UpdateCurrentTime()
    {
    TTime timeNow;
    GetCurrentTime( timeNow );
    TLocale locale;
    TDay startOfWeek = locale.StartOfWeek();
    TTimeIntervalDays oneDay( KMceTimeOneDay );
    TInt firstDayInWeek = timeNow.DayNoInWeek() - startOfWeek + 1;
    if ( firstDayInWeek < 0 )
        {
        firstDayInWeek+=ESunday+1;
        }
    TTimeIntervalDays oneWeek( firstDayInWeek );
    iOneWeekAgo = timeNow-oneWeek;
    
    TTimeIntervalDays twoWeeks( KMceTimeOneWeek );
    iTwoWeeksAgo = iOneWeekAgo-twoWeeks;
    iToday = timeNow-oneDay;
    iYesterday = iToday-oneDay;
    }

// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::SaveMarkingL
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::SaveMarkingL( RArray<TAknTreeItemID>& aMarked )
    {
    TInt count( aMarked.Count() );
    iMarkedArray.Reset();
    for (TInt loop = 0; loop < count; loop++ )
        {
        TMsvId id = MsvItemId( aMarked[ loop ] );
        if ( id != KErrNotFound )
            {
            iMarkedArray.AppendL( id );
            }
        }
    }

// ----------------------------------------------------
// CMceOneRowListItemArrayLoader::TodayTime
// ----------------------------------------------------
TTime CMceOneRowListItemArrayLoader::TodayTime () 
    {
	return iToday ;
    }

// ----------------------------------------------------
// CMceMessageListItemArray::CreateEntryIndex()
// ----------------------------------------------------
void CMceOneRowListItemArrayLoader::CreateCmailEntryIndexL() 
    {
    iCmailSelectionList = new( ELeave ) CMsvEntrySelection() ;
     
    //Find email messages 
    CMsvEntrySelection *smtpselection = iFolderEntry->ChildrenWithMtmL( KUidMsgTypeSMTP );
    CleanupStack::PushL( smtpselection );
    
    CMsvEntrySelection *pop3selection = NULL;
    CMsvEntrySelection *imapselection = NULL ;
    CMsvEntrySelection *cmailselection = NULL ;
    
    if(!iEmailFramework)
      {    
        pop3selection = iFolderEntry->ChildrenWithMtmL( KUidMsgTypePOP3 );
        CleanupStack::PushL( pop3selection );      
        imapselection = iFolderEntry->ChildrenWithMtmL( KUidMsgTypeIMAP4 );
        CleanupStack::PushL( imapselection );
      }
    else
      {
      cmailselection = iFolderEntry->ChildrenWithMtmL( TUid::Uid(KUidMsgTypeCmailMtmVal) );
      CleanupStack::PushL( cmailselection );
      }
      
    iCmailSelectionList->Reset();

    if ( smtpselection!=0 && smtpselection->Count()>0 )
        {
        for( TInt i=0 ; i < smtpselection->Count() ; i++ )
            {
            iCmailSelectionList->AppendL( smtpselection->At(i) );
            }
        }
    if(!iEmailFramework)
      {            
      if ( pop3selection!=0 && pop3selection->Count()>0 )
          {
          for( TInt i=0 ; i < pop3selection->Count() ; i++ )
              {
              iCmailSelectionList->AppendL( pop3selection->At(i) );
              }
          }
      if ( imapselection!=0 && imapselection->Count()>0 )
          {
          for( TInt i=0 ; i < imapselection->Count() ; i++ )
              {
              iCmailSelectionList->AppendL( imapselection->At(i) );
              }
          }
      }
    else
      {          
      if ( cmailselection!=0 && cmailselection->Count()>0 )
          {
          for( TInt i=0 ; i < cmailselection->Count() ; i++ )
              {
              iCmailSelectionList->AppendL( cmailselection->At(i) );
              }
          }
      }
    if(iEmailFramework)
      {
      CleanupStack::PopAndDestroy( cmailselection );       
      }
    else
      {
      CleanupStack::PopAndDestroy(imapselection);
      CleanupStack::PopAndDestroy( pop3selection );
      }
    CleanupStack::PopAndDestroy( smtpselection ); 
    }


TBool CMceOneRowListItemArrayLoader::CheckCmailEntryIndexL (TMsvId aId)
    {
    if ( iCmailSelectionList->Find( aId ) == KErrNotFound )
        {
        return ETrue;
        }
    else
        {
        return EFalse; 
        }
    }

//  End of File
