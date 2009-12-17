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
* Description:  MuiuDynamicSettingItemExtLink.cpp
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
// CMuiuSettingsLinkExtended::CMuiuSettingsLinkExtended()
// ----------------------------------------------------------------------------
CMuiuSettingsLinkExtended::CMuiuSettingsLinkExtended()
    :
    iUserValue( KErrNotFound )
    {
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsLinkExtended::ConstructL()
// ----------------------------------------------------------------------------
//
void CMuiuSettingsLinkExtended::ConstructL()
    {
    CMuiuSettingsLink::ConstructL();
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsLinkExtended::CMuiuSettingsLinkExtended()
// ----------------------------------------------------------------------------
//
CMuiuSettingsLinkExtended::~CMuiuSettingsLinkExtended()
    {
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsLinkExtended::NewL()
// ----------------------------------------------------------------------------
//
CMuiuSettingsLinkExtended* CMuiuSettingsLinkExtended::NewL()
    {
    CMuiuSettingsLinkExtended* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditText::NewLC()
// ----------------------------------------------------------------------------
//
CMuiuSettingsLinkExtended* CMuiuSettingsLinkExtended::NewLC()
    {
    CMuiuSettingsLinkExtended* self =
        new ( ELeave ) CMuiuSettingsLinkExtended();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsLinkExtended::operator=()
// ----------------------------------------------------------------------------
//
CMuiuSettingsLinkExtended& CMuiuSettingsLinkExtended::operator=(
    const CMuiuSettingsLinkExtended& aBaseItem )
    {
    __ASSERT_DEBUG( iItemLabel,
        User::Panic( KMuiuExtLinkItemPanic, KErrUnknown ) );
    __ASSERT_DEBUG( aBaseItem.iItemLabel,
        User::Panic( KMuiuExtLinkItemPanic, KErrUnknown ) );
    __ASSERT_DEBUG( iItemSettingText,
        User::Panic( KMuiuExtLinkItemPanic, KErrUnknown ) );
    __ASSERT_DEBUG( aBaseItem.iItemSettingText,
        User::Panic( KMuiuExtLinkItemPanic, KErrUnknown ) );

    iItemFlags = aBaseItem.iItemFlags;
    iItemType = aBaseItem.iItemType;
    iItemLabel->Copy( *aBaseItem.iItemLabel );
    iItemId = aBaseItem.iItemId;
    iItemResourceId = aBaseItem.iItemResourceId;
    iItemLinkArray = aBaseItem.iItemLinkArray;
    iItemSettingText->Copy( *aBaseItem.iItemSettingText );
    iItemSubType = aBaseItem.iItemSubType;
    iUserValue = aBaseItem.Value();

    return *this;
    }

// End of file

