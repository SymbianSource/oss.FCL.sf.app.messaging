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
* Description:  MuiuDynamicSettingItemLink.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include "MuiuDynamicSettingItemLink.h"
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
// CMuiuSettingsLink::CMuiuSettingsLink()
// ----------------------------------------------------------------------------
CMuiuSettingsLink::CMuiuSettingsLink()
    :
    iItemSubType( EMuiuDynSetUndefined ),
    iItemLinkArray( NULL ),
    iItemSettingText( NULL )
    {
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditText::ConstructL()
// ----------------------------------------------------------------------------
//
void CMuiuSettingsLink::ConstructL()
    {
    CMuiuSettingBase::ConstructL();

    iItemSettingText = new ( ELeave ) TMuiuSettingsText();
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsLink::CMuiuSettingsLink()
// ----------------------------------------------------------------------------
CMuiuSettingsLink::~CMuiuSettingsLink()
    {
    // Check if subarray exist
    if ( iItemLinkArray )
        {
        // Check all items in array and delete any subarrays from list
        for ( TInt index = iItemLinkArray->Count(); --index >= 0; )
            {
            // Only certain type of items can contain subarrays
            CMuiuSettingBase* base = ( *iItemLinkArray )[index];

            delete base;
            base = NULL;
            }
        }

    delete iItemLinkArray;
    iItemLinkArray = NULL;
    delete iItemSettingText;
    iItemSettingText = NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsLink::NewL()
// ----------------------------------------------------------------------------
//
CMuiuSettingsLink* CMuiuSettingsLink::NewL()
    {
    CMuiuSettingsLink* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsLink::NewLC()
// ----------------------------------------------------------------------------
//
CMuiuSettingsLink* CMuiuSettingsLink::NewLC()
    {
    CMuiuSettingsLink* self =
        new ( ELeave ) CMuiuSettingsLink();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsLink::operator=()
// ----------------------------------------------------------------------------
//
CMuiuSettingsLink& CMuiuSettingsLink::operator=(
    const CMuiuSettingsLink& aBaseItem )
    {
    __ASSERT_DEBUG( iItemLabel,
        User::Panic( KMuiuBaseItemPanic, KErrUnknown ) );
    __ASSERT_DEBUG( aBaseItem.iItemLabel,
        User::Panic( KMuiuBaseItemPanic, KErrUnknown ) );
    __ASSERT_DEBUG( iItemSettingText,
        User::Panic( KMuiuBaseItemPanic, KErrUnknown ) );
    __ASSERT_DEBUG( aBaseItem.iItemSettingText,
        User::Panic( KMuiuBaseItemPanic, KErrUnknown ) );

    iItemFlags = aBaseItem.iItemFlags;
    iItemType = aBaseItem.iItemType;
    iItemLabel->Copy( *aBaseItem.iItemLabel );
    iItemId = aBaseItem.iItemId;
    iItemResourceId = aBaseItem.iItemResourceId;
    iItemLinkArray = aBaseItem.iItemLinkArray;
    iItemSettingText->Copy( *aBaseItem.iItemSettingText );
    iItemSubType = aBaseItem.iItemSubType;

    return *this;
    }

// End of file

