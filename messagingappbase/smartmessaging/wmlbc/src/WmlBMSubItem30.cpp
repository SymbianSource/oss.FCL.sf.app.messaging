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
*     Class includes information about one bookmark. 
*     It has handle to Bookmark engine for adding bookmark.
*
*/



// INCLUDE FILES

#include "WmlBMSubItem30.h"               // CWmlBMSubItem
#include "WmlBMUtils30.h"                 // WmlBMUtils
#include "WMLBC.hrh"
#include "WmlBioControl.pan"
#include "WmlLog.h"

#include <FavouritesItemList.h>
#include <WMLBC.rsg>                    // resource identifiers
#include <StringLoader.h>               // StringLoader
#include <msgnamevalue.h>               // CMsgNameValue
#include <MsgBioUtils.h>

// LOCAL CONSTANTS AND MACROS

const TInt KWmlBookmarkFieldCount = 2;
const TInt KUrlMaxLength = 255;
const TInt KNameMaxLength = 50;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWmlBMSubItem::NewL
// ---------------------------------------------------------
//
CWmlBMSubItem* CWmlBMSubItem::NewL()
    {
    CWmlBMSubItem* self = new ( ELeave ) CWmlBMSubItem();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CWmlBMSubItem::~CWmlBMSubItem()
    {
    delete iBMItem;
    iBookmarkDb.Close();
    iSession.Close();
	if ( iItemList ) // itemlist is created only when bookmark is saved.
		{
		delete iItemList;
		}
    }

// ---------------------------------------------------------
// CWmlBMSubItem::AddFieldL
// ---------------------------------------------------------
//
void CWmlBMSubItem::AddFieldL( 
    const TDesC& aFieldValue, 
    const TInt aFieldEnum )
    {
    // Note: Any max length checks are not done in bio control.
    // If the data passes the parser then we accept it.
    // And we depend to the FavouritesDatabase consistency checks.

    TInt length = aFieldValue.Length();

    switch ( aFieldEnum )
        {
        case EWappURL:
            {
            if (iBMItem->Url().Length() == 0 && // no existing URL
                length > 0 &&                   // URL is longer than 0 chars
                length <= KUrlMaxLength)        // URL is no longer than max length
                {
                iBMItem->SetUrlL( aFieldValue );
                iIsValid = ETrue; // url makes bookmark valid.
                }
            break;
            }
        case EWappName:
            {
            // Name is added if:
            // - There's not already a name.
            // - Given name is not empty.
            if ( iBMItem->Name().Length() == 0 && length > 0 )
                {
                // If the name exceeds the maximum name length the name is shortened
                // to the maximum value.
                if (length > KNameMaxLength)
                    {

                    iBMItem->SetNameL( aFieldValue.Left(KNameMaxLength) );    
                    }
                else
                    {
                    iBMItem->SetNameL( aFieldValue );
                    }
                }
            break;    
            }
        default:
            {
            // Internal error, trying to add wrong fields to this item.
            Panic( EIllegalCommandInCurrentState );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CWmlBMSubItem::FieldCount
// ---------------------------------------------------------
//
TInt CWmlBMSubItem::FieldCount() const
    {
    return KWmlBookmarkFieldCount;
    }

// ---------------------------------------------------------
// CWmlBMSubItem::GetFieldDataAtLC
// ---------------------------------------------------------
//
CMsgNameValue* CWmlBMSubItem::GetFieldDataAtLC( const TInt aIndex )
    {
    HBufC* label = NULL;
    HBufC* value = NULL;

    switch ( aIndex ) 
        {
        case EBookmarkNameFieldIndex:
            {
            label = StringLoader::LoadLC( R_QTN_SM_BM_TITLE );
            value = iBMItem->Name().AllocLC();
            break;
            }    
        case EBookmarkAddressFieldIndex:
            {
            label = StringLoader::LoadLC( R_QTN_SM_BM_ADDRESS );
            value = iBMItem->Url().AllocLC();
            break;
            }
        default:
            {
            // Internal error, trying to add wrong fields to this item.
            Panic( EIllegalCommandInCurrentState );
            break;
            }
        }
    CMsgNameValue* textPair = CMsgNameValue::NewL( label, value ); 
    CleanupStack::Pop( 2 ); // value, label
    CleanupStack::PushL( textPair );
    return textPair;
    }

// ---------------------------------------------------------
// CWmlBMSubItem::IsValidL
// ---------------------------------------------------------
//
TBool CWmlBMSubItem::IsValidL()
    {
    return iIsValid;
    }

// ---------------------------------------------------------
// CWmlBMSubItem::DoInitialize
// ---------------------------------------------------------
//
void CWmlBMSubItem::DoInitialize()
    {
	if ( iItemList )
		{
		delete iItemList;
		iItemList = NULL;
		}
    }

// ---------------------------------------------------------
// CWmlBMSubItem::NameMaxLength
//
// ---------------------------------------------------------
//
TInt CWmlBMSubItem::NameMaxLength()
    {
    return KNameMaxLength;
    }

// ---------------------------------------------------------
// CWmlBMSubItem::NameLC
// ---------------------------------------------------------
//
HBufC* CWmlBMSubItem::NameLC()
    {
    const TDesC& name = iBMItem->Name();
    HBufC* nameBuf = name.AllocL();
    CleanupStack::PushL( nameBuf );
    return nameBuf;
    }

// ---------------------------------------------------------
// CWmlBMSubItem::SetNameL
// ---------------------------------------------------------
//
void CWmlBMSubItem::SetNameL( const TDesC& aName )
    {
    iBMItem->SetNameL( aName );
    }

// ---------------------------------------------------------
// CWmlBMSubItem::DefaultNameLC
// ---------------------------------------------------------
//
HBufC* CWmlBMSubItem::DefaultNameLC()
    {
    return StringLoader::LoadLC( R_QTN_SM_BOOKMARK_NAME );
    }

// ---------------------------------------------------------
// CWmlBMSubItem::DoQueryNewNameLC
// ---------------------------------------------------------
//
HBufC* CWmlBMSubItem::DoQueryNewNameLC( const TDesC& aName )
    {
    TBuf<KNameMaxLength> nameBuf( aName );
    MsgBioUtils::TextQueryL(R_WMLBC_QUERY_BM_NAME, nameBuf); 
    HBufC* name = nameBuf.AllocL();
    CleanupStack::PushL( name );
    return name;
    }

// ---------------------------------------------------------
// CWmlBMSubItem::DoRenameQueryL
// ---------------------------------------------------------
//
TBool CWmlBMSubItem::DoRenameQueryL( const TDesC& aName )
    {
    HBufC* query = StringLoader::LoadL( R_QTN_FLDR_RENAME_QUERY, aName );
    CleanupStack::PushL( query );
    TBool result = MsgBioUtils::ConfirmationQueryOkCancelL( *query );
    CleanupStack::PopAndDestroy( query );
    return result;
    }

// ---------------------------------------------------------
// CWmlBMSubItem::IsNameValidL
// ---------------------------------------------------------
//
TBool CWmlBMSubItem::IsNameValidL( const TDesC& aName, TBool aUpdateList )
    {
	// Update the item list when it's NULL or direction to update it is given
	if ( iItemList == NULL || aUpdateList )
		{
		if ( iItemList )
			{
			delete iItemList; // Delete the old itemlist
			iItemList = NULL;
			}
		iItemList = WmlBMUtils::CreateItemListLC(BookmarkDb2L());
		CleanupStack::Pop( iItemList );
		}

    TInt isUnique = EFalse;
    if (WmlBMUtils::Exists(aName, *iItemList) == KErrNotFound)
        {   
        isUnique = ETrue;    
        }
    return isUnique;
    }

// ---------------------------------------------------------
// CWmlBMSubItem::PreStoreL
// ---------------------------------------------------------
//
void CWmlBMSubItem::PreStoreL( const TBool /*aShowPreferredQuery*/ )
    {
	LOG("CWmlBMSubItem::PreStoreL start");
	if ( iItemList == NULL ) // No itemlist exists, create it.
		{
		LOG("CWmlBMSubItem::PreStoreL creating itemlist");
		iItemList = WmlBMUtils::CreateItemListLC(BookmarkDb2L());
		CleanupStack::Pop( iItemList );
		LOG("CWmlBMSubItem::PreStoreL creating itemlist DONE");
		}
	LOG("CWmlBMSubItem::PreStoreL end");
    }

// ---------------------------------------------------------
// CWmlBMSubItem::StoreL
// ---------------------------------------------------------
//
void CWmlBMSubItem::StoreL()
    {
	LOG("CWmlBMSubItem::StoreL start");	
	User::LeaveIfError( WmlBMUtils::DoStoreL( BookmarkDb2L(), *iBMItem ) );
	LOG("CWmlBMSubItem::StoreL end");
    }

// ---------------------------------------------------------
// CWmlBMSubItem::PostStoreL
// ---------------------------------------------------------
//
void CWmlBMSubItem::PostStoreL( const TBool aShowNote, 
    const TInt /*aPreferredInformation*/,
    const TBool /*aCreateBookmarkIfNotSetAsDefault*/)
    {
	LOG("CWmlBMSubItem::PostStoreL start");

    if ( iItemList )
		{
		delete iItemList;
		iItemList = NULL;
		}

    // Show note only if required.
    if ( aShowNote )
        {
        MsgBioUtils::ConfirmationNoteL( R_QTN_SM_NOTE_BOOKMARK_COPIED );    
        }
	LOG("CWmlBMSubItem::PreStoreL end");
    }

// ---------------------------------------------------------
// CWmlBMSubItem::ConstructL.
// ---------------------------------------------------------
//
void CWmlBMSubItem::ConstructL()
    {
	iBMItem = CFavouritesItem::NewL(); // initializes itself using "Default" AP
    iBMItem->SetType( CFavouritesItem::EItem );
    iBMItem->SetParentFolder( KFavouritesRootUid );
    User::LeaveIfError( iSession.Connect() );
    User::LeaveIfError( iBookmarkDb.Open( iSession, KBrowserBookmarks ) );
    }

// ---------------------------------------------------------
// CWmlBMSubItem::BookmarkDb2L
// ---------------------------------------------------------
//
RFavouritesDb& CWmlBMSubItem::BookmarkDb2L()
    {
    return iBookmarkDb;
    }

//  End of File  
















