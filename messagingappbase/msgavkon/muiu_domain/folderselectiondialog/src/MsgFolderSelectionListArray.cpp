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
* Description: MsgFolderSelectionListArray implementation
*
*/



// INCLUDE FILES
#include <msvuids.h>
#include <msvapi.h>
#include <gulutil.h>   // KColumnListSeparator
#include "MsgFolderSelectionListArray.h"


//  CONSTANTS
#define KDocumentsEntryIdValue    0x1008
#define KTemplatesEntryIdValue    0x1009
const TInt KMdcaPointTextMaxLength = 100; 


// MODULE DATA STRUCTURES
// these should correspond icon array created in CMsgFolderSelectionDialog::ConstructL
enum {
    EMsgFolderSelectionDialogIconArrayInbox = 0,
    EMsgFolderSelectionDialogIconArrayDocuments,
    EMsgFolderSelectionDialogIconArrayFolder
    };


// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CMsgFolderSelectionListArray::CMsgFolderSelectionListArray
// C++ default constructor can NOT contain any code that might leave.
// -----------------------------------------------------------------------------
//
CMsgFolderSelectionListArray::CMsgFolderSelectionListArray()
    {
    }


// -----------------------------------------------------------------------------
// CMsgFolderSelectionListArray::ConstructL
// Symbian OS default constructor can leave.
// -----------------------------------------------------------------------------
//
void CMsgFolderSelectionListArray::ConstructL( TMsvId aCurrentFolder )
    {
    iSession = CMsvSession::OpenSyncL( *this );

    CMsvEntry* entry = CMsvEntry::NewL(
        *iSession,
        KDocumentsEntryIdValue, // documents
        TMsvSelectionOrdering( KMsvGroupByType, EMsvSortByDescription, ETrue ));
    CleanupStack::PushL( entry );
    TUid folderType = { KUidMsvFolderEntryValue };
    iFolderList = entry->ChildrenWithTypeL( folderType );
    if ( aCurrentFolder != KDocumentsEntryIdValue )
        {
        iFolderList->InsertL( 0, KDocumentsEntryIdValue ); // documents
        }

    if ( aCurrentFolder != KMsvGlobalInBoxIndexEntryIdValue )
        {
        iFolderList->InsertL( 0, KMsvGlobalInBoxIndexEntryIdValue );
        }

    TInt index = iFolderList->Find( aCurrentFolder );
    if ( index != KErrNotFound )
        {
        iFolderList->Delete( index );
        }
    index = iFolderList->Find( KTemplatesEntryIdValue ); // templates
    if ( index != KErrNotFound )
        {
        iFolderList->Delete( index );
        }

    CleanupStack::PopAndDestroy( entry );

    iMdcaPointText = HBufC::NewL( KMdcaPointTextMaxLength );

    }


// -----------------------------------------------------------------------------
// CMsgFolderSelectionListArray::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgFolderSelectionListArray* CMsgFolderSelectionListArray::NewL( TMsvId aCurrentFolder )
    { // static
    CMsgFolderSelectionListArray* self = new(ELeave) CMsgFolderSelectionListArray( );
    CleanupStack::PushL( self );
    self->ConstructL( aCurrentFolder );
    CleanupStack::Pop( self ); // self
    return self;
    }


// Destructor
// -----------------------------------------------------------------------------
// CMsgFolderSelectionListArray::~CMsgFolderSelectionListArray
// Destructorr.
// -----------------------------------------------------------------------------
//
CMsgFolderSelectionListArray::~CMsgFolderSelectionListArray( )
    {
    delete iFolderList;
    delete iMdcaPointText;
    delete iSession;
    }


// ---------------------------------------------------------
// CMsgFolderSelectionListArray::MdcaCount
// ---------------------------------------------------------
//
TInt CMsgFolderSelectionListArray::MdcaCount() const
    {
    return ( iFolderList ? iFolderList->Count() : 0 );
    }

// ---------------------------------------------------------
// CMsgFolderSelectionListArray::MdcaPoint
// ---------------------------------------------------------
//
TPtrC CMsgFolderSelectionListArray::MdcaPoint( TInt aIndex ) const
    {
    TPtr   tempText = iMdcaPointText->Des();
    tempText.Zero();
    TMsvId service;
    TMsvEntry entry;
    if ( iSession->GetEntry( ( *iFolderList)[aIndex], service, entry ) == KErrNone )
        {
        TInt iconIndex = EMsgFolderSelectionDialogIconArrayFolder;
        if ( entry.Id() == KMsvGlobalInBoxIndexEntryIdValue )
            {
            iconIndex = EMsgFolderSelectionDialogIconArrayInbox;
            }
        else if ( entry.Id() == KDocumentsEntryIdValue )
            {
            iconIndex = EMsgFolderSelectionDialogIconArrayDocuments;
            }
        else
            {
            //Does nothing
            }
        tempText.AppendNum( iconIndex );
        tempText.Append( KColumnListSeparator );
        // -2 because there comes iconIndex and column separator in that string
        tempText.Append( entry.iDetails.Left( KMdcaPointTextMaxLength - 2 ) );
        }
    return tempText;
    }

// ---------------------------------------------------------
// CMsgFolderSelectionListArray::ItemId
// ---------------------------------------------------------
//
TMsvId CMsgFolderSelectionListArray::ItemId( TInt aIndex ) const
    {
    return ( *iFolderList )[aIndex];
    }


// ---------------------------------------------------------
// CMsgFolderSelectionListArray::HandleSessionEventL
// ---------------------------------------------------------
//
void CMsgFolderSelectionListArray::HandleSessionEventL(
                                                       TMsvSessionEvent aEvent,
                                                       TAny* /*aArg1*/,
                                                       TAny* /*aArg2*/,
                                                       TAny* /*aArg3*/ )
    {
    switch ( aEvent )
        {
        case EMsvServerReady:
            {
            }
            break;
            
        case EMsvCorruptedIndexRebuilt:
        case EMsvCloseSession:
        case EMsvMtmGroupInstalled:
        case EMsvMtmGroupDeInstalled:
        case EMsvEntriesCreated:
        case EMsvEntriesChanged:
        case EMsvEntriesDeleted:
        default:
            break;
        }
    }

//  End of File

