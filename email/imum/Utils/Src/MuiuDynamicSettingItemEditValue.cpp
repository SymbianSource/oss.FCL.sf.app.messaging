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
* Description:  MuiuDynamicSettingItemEditValue.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include "MuiuDynamicSettingItemBase.h"
#include "MuiuDynamicSettingsArray.hrh"
#include "MuiuDynamicSettingsArray.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

// ----------------------------------------------------------------------------
// CMuiuSettingsEditValue::CMuiuSettingsEditValue()
// ----------------------------------------------------------------------------
CMuiuSettingsEditValue::CMuiuSettingsEditValue()
    :
    iUserValue( KErrNotFound ),
    iItemMaxLength( KMuiuDynMaxSettingsTextLength ),
    iItemSettingText( 0 )
    {
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditValue::ConstructL()
// ----------------------------------------------------------------------------
//
void CMuiuSettingsEditValue::ConstructL()
    {
    CMuiuSettingBase::ConstructL();

    iItemSettingText = new ( ELeave ) TMuiuSettingsText();
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditValue::CMuiuSettingsEditValue()
// ----------------------------------------------------------------------------
//
CMuiuSettingsEditValue::~CMuiuSettingsEditValue()
    {
    delete iItemSettingText;
    iItemSettingText = NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditValue::NewL()
// ----------------------------------------------------------------------------
//
CMuiuSettingsEditValue* CMuiuSettingsEditValue::NewL()
    {
    CMuiuSettingsEditValue* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditText::NewLC()
// ----------------------------------------------------------------------------
//
CMuiuSettingsEditValue* CMuiuSettingsEditValue::NewLC()
    {
    CMuiuSettingsEditValue* self =
        new ( ELeave ) CMuiuSettingsEditValue();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditValue::operator=()
// ----------------------------------------------------------------------------
//
CMuiuSettingsEditValue& CMuiuSettingsEditValue::operator=(
    const CMuiuSettingsEditValue& aBaseItem )
    {
    __ASSERT_DEBUG( iItemLabel,
        User::Panic( KMuiuEditValueItemPanic, KErrUnknown ) );
    __ASSERT_DEBUG( aBaseItem.iItemLabel,
        User::Panic( KMuiuEditValueItemPanic, KErrUnknown ) );
    __ASSERT_DEBUG( iItemSettingText,
        User::Panic( KMuiuEditValueItemPanic, KErrUnknown ) );
    __ASSERT_DEBUG( aBaseItem.iItemSettingText,
        User::Panic( KMuiuEditValueItemPanic, KErrUnknown ) );

    iItemFlags = aBaseItem.iItemFlags;
    iItemType = aBaseItem.iItemType;
    iItemLabel->Copy( *aBaseItem.iItemLabel );
    iItemId = aBaseItem.iItemId;
    iItemResourceId = aBaseItem.iItemResourceId;
    iItemMaxLength = aBaseItem.iItemMaxLength;
    iItemSettingText->Copy( *aBaseItem.iItemSettingText );
    iUserValue = aBaseItem.Value();

    return *this;
    }

// End of file

