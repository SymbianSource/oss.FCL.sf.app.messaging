/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   UniEditor enumerations.    
*
*/



#ifndef __UNIEDITORENUM_H__
#define __UNIEDITORENUM_H__

// ========== DATA TYPES ===================================

enum TUniEditorPanic
    {
    EUniEditorModeFlagsNotSet = 0,
    EUniNullPointer,
    EUniIllegalComponentType,
    EUniNoMessage,
    EUniNoImage,
    EUniUnsupportedCommand,
    EUniUnknownMenuOption,
    EUniIllegalAddress,
    EUniSendWithoutRecipients,
    EUniEntryNotInDraftFolder,
    EUniIllegalMediaObject,
    EUniIllegalLayout,
    EUniIllegalParseResult,
    EUniIllegalMsgSize,
    EUniIllegalMsgSizeLimit,
    EUniIllegalSendingOptions,
    EUniIllegalArgument,
    EUniIllegalMessageType,
    EUniIllegalSmilType,
    EUniCannotInsert,
    EUniAlreadyExists,
    EUniCharConvNotFound,
    EUniIllegalEvent
    };

GLDEF_C void Panic( TUniEditorPanic aPanic );

#endif   // __UNIEDITORENUM_H__

// End of File

 
