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
* Description:  MuiuDynamicSettingItemBase.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include "MuiuDynamicSettingItemBase.h"
#include "MuiuDynamicSettingsArray.hrh"
#include "MuiuDynamicSettingsArray.h"
#include "MuiuDynamicHeader.h"

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
// CMuiuSettingBase::CMuiuSettingBase()
// ----------------------------------------------------------------------------
CMuiuSettingBase::CMuiuSettingBase()
    :
    iItemType( EMuiuDynSetUndefined ),
    iItemLabel( NULL ),
    iItemId( TUid::Uid( 0 ) ),
    iItemResourceId( 0 )
    {
    }

// ----------------------------------------------------------------------------
// CMuiuSettingBase::ConstructL()
// ----------------------------------------------------------------------------
//
void CMuiuSettingBase::ConstructL()
    {
    iItemLabel = new ( ELeave ) TMuiuSettingsText();
    }



// ----------------------------------------------------------------------------
// CMuiuSettingBase::~CMuiuSettingBase()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase::~CMuiuSettingBase()
    {
    delete iItemLabel;
    iItemLabel = NULL;
    }


// ----------------------------------------------------------------------------
// CMuiuSettingBase::NewL()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CMuiuSettingBase::NewL()
    {
    CMuiuSettingBase* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }


// ----------------------------------------------------------------------------
// CMuiuSettingBase::NewLC()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CMuiuSettingBase::NewLC()
    {
    CMuiuSettingBase* self =
        new ( ELeave ) CMuiuSettingBase();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingBase::operator=()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase& CMuiuSettingBase::operator=(
    const CMuiuSettingBase& aBaseItem )
    {
    __ASSERT_DEBUG( iItemLabel,
        User::Panic( KMuiuBaseItemPanic, KErrUnknown ) );
    __ASSERT_DEBUG( aBaseItem.iItemLabel,
        User::Panic( KMuiuBaseItemPanic, KErrUnknown ) );

    iItemFlags = aBaseItem.iItemFlags;
    iItemType = aBaseItem.iItemType;
    iItemLabel->Copy( *aBaseItem.iItemLabel );
    iItemId = aBaseItem.iItemId;
    iItemResourceId = aBaseItem.iItemResourceId;

    return *this;
    }

// End of file

