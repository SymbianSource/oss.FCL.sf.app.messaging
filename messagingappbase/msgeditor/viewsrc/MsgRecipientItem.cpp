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
* Description:  MsgRecipientItem implementation
*
*/



// ========== INCLUDE FILES ================================

#include "MsgRecipientItem.h"              // for CMsgRecipientItem
#include "MsgEditorPanic.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

const TInt KMsgRecipientListGranularity = 10;

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgRecipientItem::NewLC
//
//
// ---------------------------------------------------------
//
EXPORT_C CMsgRecipientItem* CMsgRecipientItem::NewLC(
    const TDesC& aName, const TDesC& aAddress )
    {
    CMsgRecipientItem* self = NewL( aName, aAddress );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------
// CMsgRecipientItem::NewL
//
//
// ---------------------------------------------------------
//
EXPORT_C CMsgRecipientItem* CMsgRecipientItem::NewL(
    const TDesC& aName, const TDesC& aAddress )
    {
    CMsgRecipientItem* self = new ( ELeave ) CMsgRecipientItem;

    self->SetNameL( aName );
    self->SetAddressL( aAddress );

    return self;
    }

// ---------------------------------------------------------
// CMsgRecipientItem::CMsgRecipientItem
//
//
// ---------------------------------------------------------
//
CMsgRecipientItem::CMsgRecipientItem() : 
    iContactLink( NULL )
    {
    }

// ---------------------------------------------------------
// CMsgRecipientItem::~CMsgRecipientItem
//
//
// ---------------------------------------------------------
//
CMsgRecipientItem::~CMsgRecipientItem()
    {
    delete iAddress;
    delete iName;
    }

// ---------------------------------------------------------
// CMsgRecipientItem::SetNameL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgRecipientItem::SetNameL( const TDesC& aName )
    {
    HBufC *name = aName.AllocL();
    delete iName;
    iName = name;
    }

// ---------------------------------------------------------
// CMsgRecipientItem::Name
//
//
// ---------------------------------------------------------
//
EXPORT_C HBufC* CMsgRecipientItem::Name()
    {
    return iName;
    }

// ---------------------------------------------------------
// CMsgRecipientItem::SetAddressL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgRecipientItem::SetAddressL( const TDesC& aAddress )
    {
    HBufC *address = aAddress.AllocL();
    delete iAddress;
    iAddress = address;
    }

// ---------------------------------------------------------
// CMsgRecipientItem::Address
//
//
// ---------------------------------------------------------
//
EXPORT_C HBufC* CMsgRecipientItem::Address()
    {
    return iAddress;
    }


// ---------------------------------------------------------
// CMsgRecipientItem::SetContactLink
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgRecipientItem::SetContactLink( 
    const MVPbkContactLink* aContactLink
    )
    {
    iContactLink = aContactLink;
    }

// ---------------------------------------------------------
// CMsgRecipientItem::ContactLink
//
//
// ---------------------------------------------------------
//
EXPORT_C const MVPbkContactLink* CMsgRecipientItem::ContactLink() 
    {
    return iContactLink;
    }


// ---------------------------------------------------------
// CMsgRecipientItem::SetId
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgRecipientItem::SetId( TContactItemId /*aId*/ )
    {
    __ASSERT_DEBUG( EFalse, Panic( EMsgFunctionDeprecated ) );
    }

// ---------------------------------------------------------
// CMsgRecipientItem::Id
//
//
// ---------------------------------------------------------
//
EXPORT_C TContactItemId CMsgRecipientItem::Id()
    {
    __ASSERT_DEBUG( EFalse, Panic( EMsgFunctionDeprecated ) );
    return TContactItemId();
    }

// ---------------------------------------------------------
// CMsgRecipientItem::SetVerified
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgRecipientItem::SetVerified( TBool aVerified )
    {
    iVerified = aVerified;
    }

// ---------------------------------------------------------
// CMsgRecipientItem::IsVerified
//
//
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgRecipientItem::IsVerified()
    {
    return iVerified;
    }

// ---------------------------------------------------------
// CMsgRecipientItem::SetValidated
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgRecipientItem::SetValidated( TBool aValidated )
    {
    iValidated = aValidated;
    }

// ---------------------------------------------------------
// CMsgRecipientItem::IsValidated
//
//
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgRecipientItem::IsValidated()
    {
    return iValidated;
    }

// =========================================================

// ---------------------------------------------------------
// CMsgRecipientList::NewL
//
//
// ---------------------------------------------------------
//
EXPORT_C CMsgRecipientList* CMsgRecipientList::NewL()
    {
    CMsgRecipientList* self = new ( ELeave ) CMsgRecipientList();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self

    return self;
    }

// ---------------------------------------------------------
// CMsgRecipientList::~CMsgRecipientList
//
//
// ---------------------------------------------------------
//
CMsgRecipientList::~CMsgRecipientList()
    {
    if ( iRecipientArray )
        {
        iRecipientArray->ResetAndDestroy();
        }
    delete iRecipientArray;
    }

// ---------------------------------------------------------
// CMsgRecipientList::AppendL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgRecipientList::AppendL( CMsgRecipientItem* aItem )
    {
    iRecipientArray->AppendL( aItem );
    }

// ---------------------------------------------------------
// CMsgRecipientList::InsertL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgRecipientList::InsertL( TInt aIndex, CMsgRecipientItem* aItem )
    {
    iRecipientArray->InsertL( aIndex, aItem );
    }

// ---------------------------------------------------------
// CMsgRecipientList::Count
//
//
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgRecipientList::Count() const
    {
    return iRecipientArray->Count();
    }

// ---------------------------------------------------------
// CMsgRecipientList::At
//
//
// ---------------------------------------------------------
//
EXPORT_C CMsgRecipientItem* CMsgRecipientList::At( TInt aIndex ) const
    {
    return iRecipientArray->At( aIndex );
    }

// ---------------------------------------------------------
// CMsgRecipientList::operator[]
//
//
// ---------------------------------------------------------
//
EXPORT_C CMsgRecipientItem* CMsgRecipientList::operator[]( TInt aIndex ) const
    {
    return iRecipientArray->At( aIndex );
    }

// ---------------------------------------------------------
// CMsgRecipientList::CMsgRecipientList
//
//
// ---------------------------------------------------------
//
CMsgRecipientList::CMsgRecipientList()
    {
    }

// ---------------------------------------------------------
// CMsgRecipientList::ConstructL
//
//
// ---------------------------------------------------------
//
void CMsgRecipientList::ConstructL()
    {
    iRecipientArray = new ( ELeave ) CMsgRecipientArray( KMsgRecipientListGranularity );
    }

//  End of File
