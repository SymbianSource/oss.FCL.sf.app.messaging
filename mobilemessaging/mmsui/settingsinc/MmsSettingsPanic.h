/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
*       MmsSettings Panic definitions.
*
*/



#ifndef __MMSSETTINGSPANIC_H
#define __MMSSETTINGSPANIC_H

//  INCLUDES
#if !defined(__E32BASE_H__)
#include <e32base.h>                     // GLREF_C
#endif


// DATA TYPES

enum TMmsSettingsPanic
    {
    EMmsSettingsNotAMessage,
    EMmsSettingsNotAService,
    EMmsSettingsWrongMtm,
    EMmsSettingsFoldersNotSupported,
    EMmsSettingsNoIconForAttachment,
    EMmsSettingsNoEditorOrViewerFilename,
    EMmsSettingsWrongParameters
    };


// FUNCTION PROTOTYPES

// ----------------------------------------------------
// Panic
// Implemented in MmsSettingsHandler.cpp
// ----------------------------------------------------
GLREF_C void Panic(TMmsSettingsPanic aPanic);
#endif      // __MMSSETTINGSPANIC_H   
            
// End of File
