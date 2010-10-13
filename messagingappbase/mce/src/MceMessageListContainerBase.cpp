/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Pure virtua class for message view to handle listbox.
*
*/



// INCLUDE FILES

#include "MceMessageListContainerBase.h"
#include <bldvariant.hrh>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

CMceMessageListContainerBase::CMceMessageListContainerBase( CMceMessageListView& aParentView ) :
    CCoeControl(),
    iOwningView( aParentView )
    {
    }


CMceMessageListContainerBase::~CMceMessageListContainerBase()
    {
    }

// End of file
