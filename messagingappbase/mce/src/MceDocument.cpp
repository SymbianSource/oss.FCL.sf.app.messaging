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
*     Defines methods for CMceDocument
*
*/



// INCLUDE FILES
#include "MceDocument.h"
#include "mceui.h"

// ================= MEMBER FUNCTIONS =======================

//
// CMceDocument
//

CMceDocument* CMceDocument::NewL(CEikApplication& aApp)
    {
    return new(ELeave) CMceDocument(aApp);
    }

CMceDocument::~CMceDocument()
    {
    }

CMceDocument::CMceDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    }

// ----------------------------------------------------
// CMceDocument::CreateAppUiL
// ----------------------------------------------------
CEikAppUi* CMceDocument::CreateAppUiL()
    {
    return(new(ELeave) CMceUi);
    }

//  End of File
