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
* Description:  MsgEditorUtilsDll implementation
*
*/




// INCLUDES

#include <e32std.h>             // for GLDEF_C
#include "MsgEditorUtils.pan"   // for MsgEditor panics

_LIT(KMsgEditorUtils,"MsgEditorUtils");

GLDEF_C void Panic(TMsgEditorUtilsPanic aPanic)
    {
    User::Panic(KMsgEditorUtils, aPanic);
    }


//  End of File
