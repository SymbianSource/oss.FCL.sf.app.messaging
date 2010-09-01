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
*     Item Engine owns the setting items.
*
*/



// INCLUDE FILES

#include "WmlItemEngine.h"              // Own
#include "WmlBioControl.h"				// CWmlBioControl
#include "WmlSubItemBase.h"             // CWmlSubItemBase
#include "WmlFieldParser.h"             // CWmlFieldParser
#include "WMLBC.hrh"                    // for panic codes
#include "WmlLog.h"
#include "WmlBioControl.pan"              // for ::Panic()

#include <WMLBC.rsg>                    // resouce identifiers
#include <bldvariant.hrh>
#include <sysutil.h>
#include <eikenv.h>

// CONSTANTS
const TInt KCheckCurrentSpace = 0;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWmlItemEngine::NewL
// ---------------------------------------------------------
CWmlItemEngine* CWmlItemEngine::NewL()
    {
    CWmlItemEngine* self = new ( ELeave ) CWmlItemEngine;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
CWmlItemEngine::~CWmlItemEngine()
    {
    //array owns its contents.

    if ( iBMArray )
        {
        iBMArray->ResetAndDestroy();
        delete iBMArray;
        }

    // This array only holds pointers to the other arrays.
    delete iItemArray;
    }

// ---------------------------------------------------------
// CWmlItemEngine::ConstructL
// ---------------------------------------------------------
void CWmlItemEngine::ConstructL()
    {
    // Precondition is that item array is NULL.
    __ASSERT_DEBUG( iBMArray == NULL &&
        iItemArray == NULL,
        Panic( EItemArraysNotNull ));

    // Create the array for subitems.
    iBMArray = new (ELeave) CArrayPtrSeg<CWmlSubItemBase>(1);

    // Collection array, which doesn't own it's items.
    iItemArray = new (ELeave) CArrayPtrSeg<CWmlSubItemBase>(1);

    // Postcondition is that item array are created.
    __ASSERT_DEBUG( iBMArray &&
        iItemArray ,
        Panic( EItemArraysNull ));
    }

void CWmlItemEngine::CheckDiskSpaceL()
	{
    RFs& fs = CEikonEnv::Static()->FsSession();
    TInt drive = EDriveC;

	// All the settings go to C-drive
    if ( SysUtil::DiskSpaceBelowCriticalLevelL(&fs, KCheckCurrentSpace, drive) )
        {
        User::Leave( KErrDiskFull );
        }
	}

// ---------------------------------------------------------
// CWmlItemEngine::CreateItemArraysL
// ---------------------------------------------------------
//
void CWmlItemEngine::CreateItemArraysL(
    const CArrayPtrSeg<CParsedField>& aParsedFields )
    {
    // Precondition is that item array don't contain any objects.
    __ASSERT_DEBUG( iBMArray->Count() == 0,
        Panic( EItemArraysNotEmpty ));

    // parse, create iSubItemArray
    CWmlFieldParser* fieldParser = new (ELeave) CWmlFieldParser;
    CleanupStack::PushL( fieldParser );

    fieldParser->ParseFieldsL( aParsedFields,
        *iBMArray);
    CleanupStack::PopAndDestroy( fieldParser );

    // Add all the item's to the iItemArray
    TInt i(0);

    // Bookmarks
    for ( i = 0; i < iBMArray->Count(); i++)
        {
        iItemArray->AppendL( iBMArray->At( i ) );
        }

    // If we don't have any item's leave now.
    if ( iItemArray->Count() == 0 )
        {
        User::Leave( KErrMsgBioMessageNotValid );
        }

    // Set the first item initially to current one.
    SetCurrentItem( 0 );
    }


// ---------------------------------------------------------
// CWmlItemEngine::BookmarkCount()
// ---------------------------------------------------------
TInt CWmlItemEngine::BookmarkCount() const
    {
    return iBMArray->Count();
    }


// ---------------------------------------------------------
// CWmlItemEngine::CurrentItem
// ---------------------------------------------------------
//
CWmlSubItemBase& CWmlItemEngine::CurrentItem() const
    {
    __ASSERT_DEBUG( iCurrentItem, Panic( ENoCurrentItemSet ));
    return *iCurrentItem;
    }

// ---------------------------------------------------------
// CWmlItemEngine::SetCurrentItem
// ---------------------------------------------------------
//
void CWmlItemEngine::SetCurrentItem( const TInt aIndex )
    {
    __ASSERT_DEBUG( aIndex >= 0 && aIndex < iItemArray->Count(),
        Panic( EIndexOutOfBounds ));
    __ASSERT_DEBUG( iItemArray, Panic( EItemArrayNull ));

    iCurrentItem = iItemArray->At( aIndex );

    __ASSERT_DEBUG( iCurrentItem, Panic( ENoCurrentItemSet ));
    }

// ---------------------------------------------------------
// CWmlBioControl::SaveCurrentItemL
// ---------------------------------------------------------
//
void CWmlItemEngine::SaveCurrentItemL()
    {
    __ASSERT_DEBUG( iCurrentItem, Panic( ENoCurrentItemSet ));

	CheckDiskSpaceL();

	iCurrentItem->SaveItemL( ETrue, ETrue, EFalse, ETrue );
	}

//  End of File

