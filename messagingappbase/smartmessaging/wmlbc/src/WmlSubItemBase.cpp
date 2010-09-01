/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Base class for WML subitems.
*
*/




// INCLUDE FILES

#include "WmlSubItemBase.h"             // CWmlSubItemBase
#include "WmlBioControl.pan"              // Panic()

#include <msgnamevalue.h>               // CMsgNameValue
#include <MsgBioUtils.h>

// ==================== LOCAL FUNCTIONS ====================

/// Helper method when an array is pushed to CleanupStack
static void CleanupResetAndDestroy(TAny* aSelf)
    {
    __ASSERT_DEBUG(aSelf, User::Panic( _L("wmlbc.dll"), ENullPtr ));    
    CArrayPtrSeg<CMsgNameValue>* self = 
        static_cast<CArrayPtrSeg<CMsgNameValue>* >(aSelf);
    self->ResetAndDestroy();
    delete self;
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWmlSubItemBase::~CWmlSubItemBase
// ---------------------------------------------------------
//
CWmlSubItemBase::~CWmlSubItemBase()
    {
    delete iOriginalName;
    delete iName;
    }

// ---------------------------------------------------------
// CWmlSubItemBase::SaveItemL
// Contains logic for storing either bookmark or accespoint.
// ---------------------------------------------------------
//

TBool CWmlSubItemBase::SaveItemL( const TBool aShowNote,
    const TBool aShowPreferredQuery,
    const TInt aPreferredInformation,
    const TBool aCreateBookmarkIfNotSetAsDefault)
    {

    // Set all the member variables to the initial state.
    Initialize();

    // Name checking part

    // If the item don't have name get the default name
    // and increment it as long as it's unique.
    if (!HasNameL())
        {
        SetDefaultNameL();
        while (!IsNameValidL( EFalse )) // Don't update the list from database
            {
            IncrementNameL();
            }
        QueryNewNameL();
        }

    // Keep asking as long as the name is not unique.
    while ( !IsNameValidL() )
        {
        if ( !RenameQueryL() )
            {   
            // Operation stops if user didn't want to rename.
            return EFalse;
            }
        IncrementNameL();
        while (!IsNameValidL( EFalse )) // Dont' update the list from database
            {
            IncrementNameL();
            }
        QueryNewNameL();
        }

    // Actual storing part
   
    PreStoreL( aShowPreferredQuery );

    // Set the new name as item's name.
    SetNameL( *iName );
    StoreL();
    // Set the original name back as the item's name.
    SetNameL( *iOriginalName );

    PostStoreL( aShowNote, aPreferredInformation, aCreateBookmarkIfNotSetAsDefault );

    return ETrue;
    }

// ---------------------------------------------------------
// CWmlSubItemBase::Initialize
// Set all the member variables to the initial state.
// ---------------------------------------------------------
//
void CWmlSubItemBase::Initialize()
    {
    // Set the variables to the default values, because if
    // leave occurs at some point of the SaveItem-operation, 
    // variables MUST NOT keep their current state.

    delete iName;
    iName = NULL;

    delete iOriginalName; 
    iOriginalName = NULL;

    // Let the actual implementation initialize itself.
    DoInitialize();
    }

// ---------------------------------------------------------
// CWmlSubItemBase::~CWmlSubItemBase
// ---------------------------------------------------------
//
TBool CWmlSubItemBase::HasNameL()
    {
    // Assert that iName and iOriginalName are NULL before this operation.
    __ASSERT_DEBUG(iName == NULL, 
        Panic(ENameNotNull));
    __ASSERT_DEBUG(iOriginalName == NULL, 
        Panic(EOriginalNameNotNull));

    // Get the name from the bmItem.
    HBufC* name = NameLC();
    CleanupStack::Pop( name );

    // Put the retrieved name to the iName member variable.
    // This is changed during the storing sequence.
    iName = name;

    // Allocate the original name to the member variable.
    // This is not changed during the storing sequence and
    // is restored to the item's name in PostStoreL
    iOriginalName = name->AllocL();

    // Check that there's actually something.
    if (iName->Length() == 0)
        {
        return EFalse;
        }

    return ETrue;
    }

// ---------------------------------------------------------
// CWmlSubItemBase::~CWmlSubItemBase
// ---------------------------------------------------------
//
void CWmlSubItemBase::SetDefaultNameL()
    {
    delete iName;
    iName = NULL;
    iName = DefaultNameLC();
    CleanupStack::Pop( iName );
    }

// ---------------------------------------------------------
// CWmlSubItemBase::~CWmlSubItemBase
// ---------------------------------------------------------
//
TBool CWmlSubItemBase::IsNameValidL( TBool aUpdateList )
    {
    return IsNameValidL( *iName, aUpdateList );
    }

// ---------------------------------------------------------
// CWmlSubItemBase::IncrementNameL()
// Calls the static implementation method by giving
// iName as a parameter.
// ---------------------------------------------------------
//
void CWmlSubItemBase::IncrementNameL()
    {
	TInt maxLength = NameMaxLength();
    HBufC* newName = DoIncrementNameL(*iName, maxLength);
    
    delete iName;
    iName = NULL;
    
    iName = newName;
    }

// ---------------------------------------------------------
// CWmlSubItemBase::DoIncrementNameL()
// ---------------------------------------------------------
//
HBufC* CWmlSubItemBase::DoIncrementNameL( const TDesC& aName, TInt aMaxLength )
    {
	// Create buffer descriptor that is used to append a running number
	// to given name. The lenght of the buffer is set to maximum because 
	// IncrementNameL doesn't realloc the buffer.
	HBufC* buf = HBufC::NewLC( aMaxLength );
    TPtr ptr = buf->Des();
	ptr = aName;
    MsgBioUtils::IncrementNameL( ptr, aMaxLength );
	CleanupStack::Pop( buf );
    return buf;
    }

// ---------------------------------------------------------
// CWmlSubItemBase::QueryNewNameL()
// Calls the static implementation method by giving
// iName as a parameter.
// ---------------------------------------------------------
//
void CWmlSubItemBase::QueryNewNameL()
    {
    HBufC* newName = DoQueryNewNameLC(*iName);
    CleanupStack::Pop( newName );

    delete iName; 
    iName = NULL;

    iName = newName;
    }

TBool CWmlSubItemBase::RenameQueryL()
    {
    return DoRenameQueryL(*iName);
    }

CArrayPtrFlat<CMsgNameValue>* CWmlSubItemBase::LabelsAndValuesLC()
    {
    TInt count = FieldCount();

    CArrayPtrFlat<CMsgNameValue>* data = 
        new (ELeave) CArrayPtrFlat<CMsgNameValue>( count );
    CleanupStack::PushL( TCleanupItem(CleanupResetAndDestroy, data ) );

    for (TInt i = 0; i < count; i++)
        {
        CMsgNameValue* fieldData = GetFieldDataAtLC( i );
        if ( fieldData->Value().Length() > 0 )
            {
            data->AppendL( fieldData );
            CleanupStack::Pop( fieldData );
            }
        else
            {
            CleanupStack::PopAndDestroy( fieldData );
            }
        }
    return data;
    }

//  End of File  
