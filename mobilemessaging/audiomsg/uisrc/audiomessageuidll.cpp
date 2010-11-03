/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides AudioMessageUiDll methods. 
*
*/



#include <e32std.h>
_LIT( KModuleName, "AudioMessageUi" );

// ---------------------------------------------------------------------------
// Panic
// ---------------------------------------------------------------------------
//
GLDEF_C void Panic( TInt aPanic )
    {
    User::Panic( KModuleName, aPanic );
    }
