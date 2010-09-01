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
*     Message list view's listbox to create object of
*     CMceMessageListBoxItemDrawer.
*
*/



// INCLUDE FILES
#include <aknlists.h>

#include "MceMessageListbox.h"
#include "MceMessageListboxItemDrawer.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
CMceMessageListBox::CMceMessageListBox( CMceMessageListContainer& aMessageListContainer )
: CAknDoubleGraphicStyleListBox(),
    iMessageListContainer( aMessageListContainer )
    {
    }


// Destructor
CMceMessageListBox::~CMceMessageListBox()
    {
    }


// ---------------------------------------------------------
// CMceMessageListBox::CreateItemDrawerL
// ---------------------------------------------------------
//
void CMceMessageListBox::CreateItemDrawerL()
    {
    CFormattedCellListBoxData* cellData = CFormattedCellListBoxData::NewL();
    CleanupStack::PushL( cellData );
    iItemDrawer = new( ELeave ) CMceMessageListBoxItemDrawer(
        Model(),
        iEikonEnv->NormalFont(),
        cellData,
        iMessageListContainer );
    CleanupStack::Pop( cellData );
    }

//  End of File
