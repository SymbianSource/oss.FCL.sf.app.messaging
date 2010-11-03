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
* Description:  MuiuDynamicSettingItemEditText.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include "MuiuDynamicSettingItemEditText.h"
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
// CMuiuSettingsEditText::CMuiuSettingsEditText()
// ----------------------------------------------------------------------------
CMuiuSettingsEditText::CMuiuSettingsEditText()
    :
    iUserText( NULL ),
    iItemMaxLength( KMuiuDynMaxSettingsTextLength )
    {
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditText::ConstructL()
// ----------------------------------------------------------------------------
//
void CMuiuSettingsEditText::ConstructL()
    {
    CMuiuSettingBase::ConstructL();
    iUserText = new ( ELeave ) TMuiuSettingsText();
    iEmptyText = new ( ELeave ) TMuiuSettingsText();
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditText::CMuiuSettingsEditText()
// ----------------------------------------------------------------------------
//
CMuiuSettingsEditText::~CMuiuSettingsEditText()
    {
    delete iUserText;
    delete iEmptyText;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditText::NewL()
// ----------------------------------------------------------------------------
//
CMuiuSettingsEditText* CMuiuSettingsEditText::NewL()
    {
    CMuiuSettingsEditText* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditText::NewLC()
// ----------------------------------------------------------------------------
//
CMuiuSettingsEditText* CMuiuSettingsEditText::NewLC()
    {
    CMuiuSettingsEditText* self =
        new ( ELeave ) CMuiuSettingsEditText();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditText::operator=()
// ----------------------------------------------------------------------------
//
CMuiuSettingsEditText& CMuiuSettingsEditText::operator=(
    const CMuiuSettingsEditText& aBaseItem )
    {
    __ASSERT_DEBUG( iItemLabel,
        User::Panic( KMuiuEditTextItemPanic, KErrUnknown ) );
    __ASSERT_DEBUG( aBaseItem.iItemLabel,
        User::Panic( KMuiuEditTextItemPanic, KErrUnknown ) );
    __ASSERT_DEBUG( iUserText,
        User::Panic( KMuiuEditTextItemPanic, KErrUnknown ) );
    __ASSERT_DEBUG( aBaseItem.iUserText,
        User::Panic( KMuiuEditTextItemPanic, KErrUnknown ) );
    __ASSERT_DEBUG( aBaseItem.iEmptyText,
        User::Panic( KMuiuEditTextItemPanic, KErrUnknown ) );

    iItemFlags = aBaseItem.iItemFlags;
    iItemType = aBaseItem.iItemType;
    iItemLabel->Copy( *aBaseItem.iItemLabel );
    iItemId = aBaseItem.iItemId;
    iItemResourceId = aBaseItem.iItemResourceId;
    iItemMaxLength = aBaseItem.iItemMaxLength;
    iUserText->Copy( *aBaseItem.iUserText );
    iEmptyText->Copy( *aBaseItem.iEmptyText );

    return *this;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditText::VisibleText()
// ----------------------------------------------------------------------------
//
const TDesC& CMuiuSettingsEditText::VisibleText() const
    {
    if( iUserText->Length() > 0 )
        {
        return *iUserText;
        }
    return *iEmptyText;
    }

// End of file

