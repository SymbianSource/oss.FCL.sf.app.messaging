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
* Description:  IMSSettingsAccessPointItem.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <muiuflags.h>

#include "IMSSettingsItemAccessPoint.h"
#include "ImumUtilsLogging.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIMSSettingsAccessPointItem::CIMSSettingsAccessPointItem()
// ----------------------------------------------------------------------------
//
CIMSSettingsAccessPointItem::CIMSSettingsAccessPointItem()
    :
    iIapRadioButton( KErrNotFound ),
    iIsWizard( EFalse )
    {
    IMUM_CONTEXT( CIMSSettingsAccessPointItem::CIMSSettingsAccessPointItem, 0, KLogUi );

    }

// ----------------------------------------------------------------------------
// CIMSSettingsAccessPointItem::~CIMSSettingsAccessPointItem()
// ----------------------------------------------------------------------------
//
CIMSSettingsAccessPointItem::~CIMSSettingsAccessPointItem()
    {
    IMUM_CONTEXT( CIMSSettingsAccessPointItem::~CIMSSettingsAccessPointItem, 0, KLogUi );

    }

// ----------------------------------------------------------------------------
// CIMSSettingsAccessPointItem::ConstructL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsAccessPointItem::ConstructL()
    {
    IMUM_CONTEXT( CIMSSettingsAccessPointItem::ConstructL, 0, KLogUi );

    CMuiuSettingsLink::ConstructL();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsAccessPointItem::NewL()
// ----------------------------------------------------------------------------
//
CIMSSettingsAccessPointItem* CIMSSettingsAccessPointItem::NewL()
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsAccessPointItem::NewL, 0, utils, KLogUi );

    CIMSSettingsAccessPointItem* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsAccessPointItem::NewLC()
// ----------------------------------------------------------------------------
//
CIMSSettingsAccessPointItem* CIMSSettingsAccessPointItem::NewLC()
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsAccessPointItem::NewLC, 0, utils, KLogUi );

    CIMSSettingsAccessPointItem* self =
        new ( ELeave ) CIMSSettingsAccessPointItem();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsAccessPointItem::operator=()
// ----------------------------------------------------------------------------
//
CIMSSettingsAccessPointItem& CIMSSettingsAccessPointItem::operator=(
    const CIMSSettingsAccessPointItem& aIapItem )
    {
    IMUM_CONTEXT( CIMSSettingsAccessPointItem::operator=, 0, KLogUi );

    iItemFlags = aIapItem.iItemFlags;
    iItemType = aIapItem.iItemType;
    iItemLabel->Copy( *aIapItem.iItemLabel );
    iItemId = aIapItem.iItemId;
    iItemResourceId = aIapItem.iItemResourceId;
    iItemLinkArray = aIapItem.iItemLinkArray;
    iItemSettingText->Copy( *aIapItem.iItemSettingText );
    iItemSubType = aIapItem.iItemSubType;
    iIapRadioButton = aIapItem.iIapRadioButton;
    iIap = aIapItem.iIap;
    iIsWizard = aIapItem.iIsWizard;

    return *this;
    }

// End of File

