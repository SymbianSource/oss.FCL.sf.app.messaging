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
*      Sms Viewer
*
*/



#ifndef __MSGSMSEDITORPAN_H__
#define __MSGSMSEDITORPAN_H__

// ========== DATA TYPES ===================================

enum TMsgSmsPanic
    {
    EMsgSmsNullPointer = 0,
    EMsgSmsNoSuchComponent,
    EMsgSmsNoMessage,
    EMsgSmsViewerTriesEditorFuncs, 
    EMsgSmsViewerUnknownSmsPduType, 
    EMsgSmsBioNotActive,
    EMsgSmsBioActive,
    EMsgSmsNoGmsControl,
    EMsgSmsNotAllowedForBio,
    EMsgSmsNotForGms,
    EMsgSmsAllowedOnlyForGms,
    EMsgSmsBioBodyOnlyForBio,
    EMsgSmsAiwMenu,
    EMsgSmsAiwProviders,
    EMsgSmsNullPtrBioBodyArg,
    EMsgSmsNoSuchMtmUid
    };

#endif

// End of File
