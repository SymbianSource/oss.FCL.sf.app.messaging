/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       UniMtm panic definitions.
*
*/



#ifndef __UNIMTMPANIC_H
#define __UNIMTMPANIC_H

//  INCLUDES
#include <e32base.h>                     // GLREF_C

// DATA TYPES

enum TUniMtmPanic
    {
    EUniNotAMessage,
    EUniNotAService,
    EUniWrongMtm,
    EUniFoldersNotSupported,
    EUniNoIconForAttachment,
    EUniNoEditorOrViewerFilename,
    EUniWrongParameters,
    EUniActiveInFinalProgress,
    EUniFinalProgressFailed
    };


// FUNCTION PROTOTYPES

// ----------------------------------------------------
// Panic
// Implemented in UniMtmPanic.cpp
// ----------------------------------------------------
GLREF_C void Panic( TUniMtmPanic aPanic );

#endif      // __UNIMTMPANIC_H   
            
// End of File
