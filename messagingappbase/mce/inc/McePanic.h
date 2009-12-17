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
*     Mce's panic definition.
*
*/



#ifndef _MCEPANIC_H
#define _MCEPANIC_H

// DATA TYPES
enum TMceUiPanic
    {
    EMceUiErrCreateNewMessageCommand = 1,
    EMceUiErrCannotDeleteFromMainView,
    EMceUiErrAlreadyDeleting,
    EMceUiErrAlreadyDeletingAccount,
    EMceUiErrCannotDeleteServiceFromMainView = 5,
    EMceUiErrCanDeleteOnlyFolderOrMessagesFromMainView,
    EMceUiErrCannotUnDeleteFromMainView,
    EMceErrNothingToUnDelete,
    EMceUiErrCannotMoveFromMainView,
    EMceUiErrCannotCreateFolder = 10,
    EMceUiErrNoMtms,
    EMceUiErrLocalMtm,
    EMceUiErrRenameNotFolder,
    EMceUiErrRenameNotLocal,
    EMceUiErrAppNotInWindowGroup = 15,
    EMceUiErrOperationNotFound,
    EMceUiErrCannotFetchFromMainView,
    EMceUiErrOperationNull,
    EMceUiErrArrayEmpty,
    EMceUiErrTabsAlreadyActivated = 20,
    EMceUiErrTabsNotActivated,
    EMceUiErrTabsArrayAlreadyExists,
    EMceUiErrTabsArrayDoesNotAlreadyExist,
    EMceListItemArrayEntryNotFound,
    EMceListItemArrayNotMainViewList = 25,
    EMceListItemArrayNotMessageViewList,
    EMceListItemArrayEntryNotMessage,
    EMceListItemArrayEntryNotFolder,
    EMceUiMessageIteratorNotCreated,
    EMceMainViewCannotChangeView = 30,
    EMceMainViewUnknownExtraItem,
    EMceSessionHolderDeletedWhenClients,
    EMceUiErrCanSendViaOnlyBtOrIr

    };

// FUNCTION PROTOTYPES
extern void Panic(TMceUiPanic aPanic);

#endif      // _MCEPANIC_H

// End of File
