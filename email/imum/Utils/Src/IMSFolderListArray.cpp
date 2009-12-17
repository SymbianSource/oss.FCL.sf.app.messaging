/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IMSFolderListArray.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <miuthdr.h>
#include <msvuids.h>

#include "IMSFolderSubscription.h"
#include "IMSFolderListArray.h"
#include "Imas.hrh"
#include "ImumUtilsLogging.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
const TInt KImumITextLength = 100;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS


// ----------------------------------------------------------------------------
// CImapFolderListArray::NewL
// ----------------------------------------------------------------------------
CImapFolderListArray* CImapFolderListArray::NewL(
    CMsvSession& aMsvSession,
    TMsvId aMailboxId )
    {
    IMUM_STATIC_CONTEXT( CImapFolderListArray::NewL, 0, utils, KLogUi );

    CImapFolderListArray* self=new (ELeave) CImapFolderListArray(
        aMsvSession, aMailboxId );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::~CImapFolderListArray
// ----------------------------------------------------------------------------
CImapFolderListArray::~CImapFolderListArray()
    {
    IMUM_CONTEXT( CImapFolderListArray::~CImapFolderListArray, 0, KLogUi );

    delete iText;
    delete iEntry;
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::ConstructL
// ----------------------------------------------------------------------------
void CImapFolderListArray::ConstructL()
    {
    IMUM_CONTEXT( CImapFolderListArray::ConstructL, 0, KLogUi );

    RefreshFolderListArrayL();

    iText = HBufC::NewL( KImumITextLength );
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::CImapFolderListArray
// ----------------------------------------------------------------------------
CImapFolderListArray::CImapFolderListArray(
    CMsvSession& aMsvSession,
    TMsvId aMailboxId)
    :
    iSession( aMsvSession ),
    iMailboxId( aMailboxId )
    {
    IMUM_CONTEXT( CImapFolderListArray::CImapFolderListArray, 0, KLogUi );

    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::MdcaPoint
// ----------------------------------------------------------------------------
TPtrC CImapFolderListArray::MdcaPoint( TInt aIndex ) const
    {
    IMUM_CONTEXT( CImapFolderListArray::MdcaPoint, 0, KLogUi );

    const TMsvEmailEntry emailEntry( (*iEntry)[aIndex] );

    TPtr tempText = iText->Des();
    tempText.Zero();

    TInt iconIndex = 0;
    if ( ContextHasChildFolders( emailEntry.Id() ) )
        {
        iconIndex = ( emailEntry.LocalSubscription() ? EImumFolderSubfoldersSubbed : EImumFolderSubfoldersUnsubbed );
        }
    else
        {
        iconIndex = ( emailEntry.LocalSubscription() ? EImumFolderSingleSubbed : EImumFolderSingleUnsubbed );
        }
    tempText.AppendNum( iconIndex );
    tempText.Append( KColumnListSeparator );
    tempText.Append( emailEntry.iDetails.Left( KImasImailServiceNameLength ) );

    return tempText;
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::MdcaCount
// ----------------------------------------------------------------------------
TInt CImapFolderListArray::MdcaCount() const
    {
    IMUM_CONTEXT( CImapFolderListArray::MdcaCount, 0, KLogUi );

    return iCount;
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::Id
// ----------------------------------------------------------------------------
TMsvId CImapFolderListArray::Id( TInt aIndex )
    {
    IMUM_CONTEXT( CImapFolderListArray::Id, 0, KLogUi );

    return (*iEntry)[aIndex].Id();
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::ChangeFolderL
// ----------------------------------------------------------------------------
void CImapFolderListArray::ChangeFolderL( TMsvId aFolderId )
    {
    IMUM_CONTEXT( CImapFolderListArray::ChangeFolderL, 0, KLogUi );

    iEntry->SetEntryL( aFolderId );

    CMsvEntrySelection* sel = iEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
    iCount = sel->Count();
    delete sel;
    sel = NULL;

    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::ContextHasChildren
// ----------------------------------------------------------------------------
TBool CImapFolderListArray::ContextHasChildren( TMsvId aId ) const
    {
    IMUM_CONTEXT( CImapFolderListArray::ContextHasChildren, 0, KLogUi );

    return ContextHasChildFolders( aId );
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::ContextHasChildFolders
// ----------------------------------------------------------------------------
TBool CImapFolderListArray::ContextHasChildFolders( TMsvId aId ) const
    {
    IMUM_CONTEXT( CImapFolderListArray::ContextHasChildFolders, 0, KLogUi );

    TBool hasFolders = EFalse;
    TRAPD( error, hasFolders = DoContextHasChildFoldersL( aId ) );
    if ( error != KErrNone )
        {
        return EFalse;
        }
    return hasFolders;
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::DoContextHasChildFoldersL
// ----------------------------------------------------------------------------
TBool CImapFolderListArray::DoContextHasChildFoldersL( TMsvId aId ) const
    {
    IMUM_CONTEXT( CImapFolderListArray::DoContextHasChildFoldersL, 0, KLogUi );

    TMsvSelectionOrdering selectionOrdering(KMsvGroupByType, EMsvSortByDetails, ETrue);
    CMsvEntry* entry = CMsvEntry::NewL(iSession, aId, selectionOrdering);
    CleanupStack::PushL( entry );
    TInt index=entry->Count()-1;
    while (index>=0 && (*entry)[index].iType!=KUidMsvFolderEntry)
        {
        index--;
        }

    CleanupStack::PopAndDestroy( entry );

    if(index!=-1)
        {
        return ETrue;
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::GetContextChildrenL
// ----------------------------------------------------------------------------
CMsvEntrySelection* CImapFolderListArray::GetContextChildrenL( TMsvId aId, CMsvEntry& aEntry ) const
    {
    IMUM_CONTEXT( CImapFolderListArray::GetContextChildrenL, 0, KLogUi );

    TMsvSelectionOrdering selectionOrdering(KMsvGroupByType, EMsvSortByDetails, ETrue);
    aEntry.SetEntryL( aId );// = CMsvEntry::NewL(iSession, aId, selectionOrdering);
    aEntry.SetSortTypeL( selectionOrdering );
    CMsvEntrySelection* sel = aEntry.ChildrenWithTypeL( KUidMsvFolderEntry );
    CleanupStack::PushL( sel );

    TInt i;
    TInt cnt = sel->Count();

    for( i=0; i<cnt; i++ )
        {
        if( ContextHasChildren( sel->At(i) ) )
            {
            //Note! recursion
            CMsvEntrySelection* selection = GetContextChildrenL( sel->At(i), aEntry );
            CleanupStack::PushL( selection );
            TInt c = selection->Count();

            TInt j;
            for( j=0; j<c; j++ )
                {
                sel->AppendL( selection->At(j) );
                }
            CleanupStack::PopAndDestroy();//selection
            }
        }


    CleanupStack::Pop();//sel
    return sel;
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::FoldersUpdated
// ----------------------------------------------------------------------------
TBool CImapFolderListArray::FoldersUpdated() const
    {
    IMUM_CONTEXT( CImapFolderListArray::FoldersUpdated, 0, KLogUi );

    return ( iEntry->Count() > 0 ? ETrue : EFalse );
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::RefreshFolderListArrayL
// ----------------------------------------------------------------------------
void CImapFolderListArray::RefreshFolderListArrayL()
    {
    IMUM_CONTEXT( CImapFolderListArray::RefreshFolderListArrayL, 0, KLogUi );

    if( iEntry )
        {
        delete iEntry;
        iEntry = NULL;
        }

    iEntry = iSession.GetEntryL( iMailboxId );
    const TMsvSelectionOrdering originalOrdering=iEntry->SortType();
    TMsvSelectionOrdering newOrdering=originalOrdering;
    newOrdering.SetShowInvisibleEntries(ETrue);
    newOrdering.SetGroupByType(ETrue);
    newOrdering.SetSorting( EMsvSortByDetails );
    iEntry->SetSortTypeL(newOrdering);

    CMsvEntrySelection* sel = iEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
    iCount = sel->Count();
    delete sel;
    sel = NULL;
    }

// End of File
