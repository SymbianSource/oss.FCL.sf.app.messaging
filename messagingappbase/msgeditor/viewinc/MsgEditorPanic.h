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
* Description:  MsgEditorPanic  declaration
*
*/



#ifndef INC_MSGEDITORPANIC_H
#define INC_MSGEDITORPANIC_H

// ========== INCLUDE FILES ================================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

enum TMsgEditorPanic
{
    EMsgNullPointer,
    EMsgIncorrectSelectionItemType,
    EMsgInvalidEntryNumber,
    EMsgInvalidStartPos,
    EMsgDocPosOutOfRange,
    EMsgIncorrectComponentIndex,
    EMsgInheritedFunctionIsMissing,
    EMsgControlNotFocused,
    EMsgIncorrectClipboardFunction,
    EMsgIncorrectEditFunction,
    EMsgNoTextView,
    EMsgScrollBarNotExists,
    EMsgScrollBarAlreadyExists,
    EMsgHeaderNotExists,
    EMsgBodyNotExists,
    EMsgHeaderAlreadyExists,
    EMsgBodyAlreadyExists,
    EMsgIncorrectFormComponent,
    EMsgNoFocusingControlExist,
    EMsgNonFocusingControl,
    EMsgNoFocusInHeaderOrBody,
    EMsgFocusLost,
    EMsgControlIdNotSet,
    EMsgControlIdNotUnique,
    EMsgIncorrectControlId,
    EMsgControlNotFound,
    EMsgRemovedControlFocused,
    EMsgTextViewNotCreated,
    EMsgIconTextTooLong,
    EMsgIconIsMissing,
    EMsgHeaderWidthChanged,
    EMsgBodyWidthChanged,
    EMsgCopyNotPossible,
    EMsgCutNotPossible,
    EMsgPasteNotPossible,
    EMsgUndoNotPossible,
    EMsgSelectAllNotPossible,
    EMsgCorrectFontNotFound,
    EMsgNoSystemFontsFound,
    EMsgRecipientsAlreadySet,
    EMsgCursorPosNotFormatted,
    EMsgFunctionCalledBeforeInitialization,
    EMsgFunctionDeprecated
};

// ========== FUNCTION PROTOTYPES ==========================

GLREF_C void Panic(TMsgEditorPanic aPanic);

// ========== FORWARD DECLARATIONS =========================

// ========== CLASS DECLARATION ============================

#endif

// End of file
