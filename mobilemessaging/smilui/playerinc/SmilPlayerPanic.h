/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SmilPlayerPanic  declaration
*
*/



#ifndef SMILPLAYERPANIC_H
#define SMILPLAYERPANIC_H

#include <e32def.h>

// ========== DATA TYPES ===================================
enum TSmilPlayerDialogPanic
    {
    ESmilPlayerUnexpectedSharedDataEvent = 0x100,
    ESmilPlayerErrNotSupported,
    ESmilPlayerParsingError,
    ESmilPlayerUnableToSetIconSize,
    ESmilPlayerUnknownState,
    ESmilPlayerPresentationNotReady
    };

// ========== FUNCTION PROTOTYPES ==========================

GLDEF_C void Panic( TSmilPlayerDialogPanic aPanic);

#endif // SMILPLAYERPANIC_H

// End of File
