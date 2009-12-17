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
*     Message Editor Utils Panics
*     Defines panic codes used in MsgEditorUtils.
*
*/





#ifndef MSGEDITORUTILS_PAN
#define MSGEDITORUTILS_PAN

const TInt KMsgBioMessageTypeNotDefined = -2000;
const TInt KMsgControlUidNotDefined = -2001;
const TInt KMsgBadLibraryEntryPoint = -2002;

enum TMsgEditorUtilsPanic
    {
    EMEUControlNotFocused = 0,
    EMEUIncorrectComponentIndex,
    EMEUIncorrectClipboardFunction,
    EMEUIncorrectEditFunction,
    EMEUBioControlNotExist,
    EMEUMsvSessionIsNull,
    EMEUNotFileBase,
    EMEUGeneral,
    EMEUSoftButton,
    EMEUIncrementName,
    EMEUNotSupported,
    EMEULabelLength,
    EMEUNameEmpty,
    EMEUControlAlreadySet,
    EMEUControlNotSet,
    EMEUOnlyOneLibraryAllowed,
    EMEUErrArgument,
    EMEUAsynItemSaverNULL,
	EMEUResourceFileNotFound
    };

GLDEF_C void Panic(TMsgEditorUtilsPanic aPanic); 


#endif //MSGEDITORUTILS_PAN

// End of file
