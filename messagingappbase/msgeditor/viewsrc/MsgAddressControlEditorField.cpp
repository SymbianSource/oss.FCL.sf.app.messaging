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
* Description:  MsgAddressControlEditorField implementation
*
*/



// ========== INCLUDE FILES ================================

#include "MsgAddressControlEditorField.h"       // for CMsgAddressControlEditorField

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgAddressControlEditorField::CMsgAddressControlEditorField
//
// Constructor.
// ---------------------------------------------------------
//
CMsgAddressControlEditorField::CMsgAddressControlEditorField() : 
    iContactLink( NULL )
    {
    }

// ---------------------------------------------------------
// CMsgAddressControlEditorField::~CMsgAddressControlEditorField
//
// Destructor.
// ---------------------------------------------------------
//
CMsgAddressControlEditorField::~CMsgAddressControlEditorField()
    {
    delete iEntryString;
    delete iName;
    delete iAddress;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditorField::SetEntryStringL
//
//
// ---------------------------------------------------------
//
void CMsgAddressControlEditorField::SetEntryStringL( const TDesC& aEntryString )
    {
    delete iEntryString;
    iEntryString = NULL;
    iEntryString = aEntryString.AllocL();
    }

// ---------------------------------------------------------
// CMsgAddressControlEditorField::SetNameL
//
//
// ---------------------------------------------------------
//
void CMsgAddressControlEditorField::SetNameL( const TDesC& aName )
    {
    delete iName;
    iName = NULL;
    iName = aName.AllocL();
    }

// ---------------------------------------------------------
// CMsgAddressControlEditorField::SetAddressL
//
//
// ---------------------------------------------------------
//
void CMsgAddressControlEditorField::SetAddressL( const TDesC& aAddress )
    {
    delete iAddress;
    iAddress = NULL;
    iAddress = aAddress.AllocL();
    }

// ---------------------------------------------------------
// CMsgAddressControlEditorField::SetVerified
//
//
// ---------------------------------------------------------
//
void CMsgAddressControlEditorField::SetVerified( TBool aVerified )
    {
    iVerified = aVerified;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditorField::SetValidated
//
//
// ---------------------------------------------------------
//
void CMsgAddressControlEditorField::SetValidated( TBool aValidated )
    {
    iValidated = aValidated;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditorField::SetContactLink
//
//
// ---------------------------------------------------------
//
void CMsgAddressControlEditorField::SetContactLink( const MVPbkContactLink* aContactLink )
    {
    iContactLink = aContactLink;
    }


// ---------------------------------------------------------
// CMsgAddressControlEditorField::Link
//
//
// ---------------------------------------------------------
//
const MVPbkContactLink* CMsgAddressControlEditorField::ContactLink() const
    {
    return iContactLink;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditorField::Name
//
//
// ---------------------------------------------------------
//
HBufC* CMsgAddressControlEditorField::Name() const
    {
    return iName;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditorField::Address
//
//
// ---------------------------------------------------------
//
HBufC* CMsgAddressControlEditorField::Address() const
    {
    return iAddress;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditorField::IsVerified
//
//
// ---------------------------------------------------------
//
TBool CMsgAddressControlEditorField::IsVerified() const
    {
    return iVerified;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditorField::IsValidated
//
//
// ---------------------------------------------------------
//
TBool CMsgAddressControlEditorField::IsValidated() const
    {
    return iValidated;
    }


// ---------------------------------------------------------
// CMsgAddressControlEditorField::Value
//
//
// ---------------------------------------------------------
//
TInt CMsgAddressControlEditorField::Value( TPtr& aValueText )
    {
    if ( aValueText.MaxLength() < iEntryString->Length() )
        {
        return iEntryString->Length();
        }
    else
        {
        aValueText = iEntryString->Des();
        return 0;
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditorField::InternalizeL
//
//
// ---------------------------------------------------------
//
void CMsgAddressControlEditorField::InternalizeL( RReadStream& aStream )
    {
    iEntryString = HBufC::NewL( aStream, KMsgMaxStringLength );
    iName = HBufC::NewL( aStream, KMsgMaxStringLength );
    iAddress = HBufC::NewL( aStream, KMsgMaxStringLength );
    iVerified = aStream.ReadInt32L();
    iValidated = aStream.ReadInt32L();
    }

// ---------------------------------------------------------
// CMsgAddressControlEditorField::ExternalizeL
//
//
// ---------------------------------------------------------
//
void CMsgAddressControlEditorField::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream << iEntryString->Des();
    aStream << iName->Des();
    aStream << iAddress->Des();
    aStream.WriteInt32L( iVerified );
    aStream.WriteInt32L( iValidated );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditorField::Type
//
//
// ---------------------------------------------------------
//
TUid CMsgAddressControlEditorField::Type() const
    {
    return KMsgAddressControlEditorFieldUid;
    }

// =========================================================
// class TMsgAddressControlEditorFieldFactory functions
// =========================================================

// ---------------------------------------------------------
// TMsgAddressControlEditorFieldFactory::TMsgAddressControlEditorFieldFactory
//
// Constructor.
// ---------------------------------------------------------
//
TMsgAddressControlEditorFieldFactory::TMsgAddressControlEditorFieldFactory()
    {
    }

// ---------------------------------------------------------
// TMsgAddressControlEditorFieldFactory::~TMsgAddressControlEditorFieldFactory
//
// Destructor.
// ---------------------------------------------------------
//
TMsgAddressControlEditorFieldFactory::~TMsgAddressControlEditorFieldFactory()
    {
    }

// ---------------------------------------------------------
// TMsgAddressControlEditorFieldFactory::NewFieldL
//
// Creates a new field and returns a pointer to it.
// ---------------------------------------------------------
//
CTextField* TMsgAddressControlEditorFieldFactory::NewFieldL( TUid aFieldType )
    {
    CTextField* field = NULL;
    if ( aFieldType == KMsgAddressControlEditorFieldUid )
        {
        field = (CTextField*) new ( ELeave ) CMsgAddressControlEditorField;
        }
    return field;
    }

//  End of File
