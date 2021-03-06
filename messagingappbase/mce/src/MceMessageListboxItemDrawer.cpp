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
*     Needed because we have to override function Properties so that
*     folders cannot be selected.
*
*/



// INCLUDE FILES
#include "MceMessageListContainer.h"
#include "MceMessageListboxItemDrawer.h"
#include "MceIds.h"


// ================= MEMBER FUNCTIONS =======================

// C++ constructor can NOT contain any code that
// might leave.
//
CMceMessageListBoxItemDrawer::CMceMessageListBoxItemDrawer(
    MTextListBoxModel* aTextListBoxModel,
    const CFont* aFont,
    CFormattedCellListBoxData* aFormattedCellData,
    CMceMessageListContainer& aMessageListContainer )
    :
    CFormattedCellListBoxItemDrawer( aTextListBoxModel,
       aFont, aFormattedCellData ),
    iMessageListContainer( aMessageListContainer )
    {
    }



// Destructor
CMceMessageListBoxItemDrawer::~CMceMessageListBoxItemDrawer()
    {
    }


// ---------------------------------------------------------
// CMceMessageListBoxItemDrawer::Properties
// ---------------------------------------------------------
//
TListItemProperties CMceMessageListBoxItemDrawer::Properties(TInt aItemIndex) const
    {
    TListItemProperties properties;
    properties.SetColor( iTextColor );
	//only for MyFolders set the Hidden Selection, no folder should be able to be selected
    if ( iMessageListContainer.CurrentFolderId() == KMceDocumentsEntryId )
        {
    	properties.SetHiddenSelection( iMessageListContainer.IsItemFolder( aItemIndex )  ||
            iMessageListContainer.SyncMlOutboxOpened() );
	}
    return properties;
    }

//  End of File
