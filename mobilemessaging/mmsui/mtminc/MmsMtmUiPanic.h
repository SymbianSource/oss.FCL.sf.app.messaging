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
*       Mtm UI Panic definitions.
*
*/



#ifndef __MMSMTMUIPANIC_H
#define __MMSMTMUIPANIC_H

//  INCLUDES
#if !defined(__E32BASE_H__)
#include <e32base.h>                     // GLREF_C
#endif


// DATA TYPES

enum TMmsMtmUiPanic
    {
    EMmsNotAMessage,
    EMmsNotAService,
    EMmsWrongMtm,
    EMmsFoldersNotSupported,
    EMmsNoIconForAttachment,
    EMmsNoEditorOrViewerFilename,
    EMmsWrongParameters,
    EMmsNullPtr,
    };


// FUNCTION PROTOTYPES

// ----------------------------------------------------
// Panic
// Implemented in MmsMtmUi.cpp
// ----------------------------------------------------
GLREF_C void Panic(TMmsMtmUiPanic aPanic);


#endif      // __MMSMTMUIPANIC_H   
            
// End of File
