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
* Description:  MsgEditorModelObserver  declaration
*
*/



#ifndef INC_MSGEDITORMODELOBSERVER_H
#define INC_MSGEDITORMODELOBSERVER_H

// ========== INCLUDE FILES ================================

#include <msvstd.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

// ========== CLASS DECLARATION ============================

/**
 * Defines error handling functions for messaging editors. The default
 * implementation of these functions is in CMsgEditorAppUi base class,
 * but it can be overridden in messaging editors.
 *
 */
class MMsgEditorModelObserver
{
public:

    /**
     *
     */
    virtual void HandleEntryChangeL() = 0;

    /**
     *
     */
    virtual void HandleEntryDeletedL() = 0;

    /**
     *
     * @param aNewParent
     */
    virtual void HandleEntryMovedL(TMsvId aOldParent, TMsvId aNewParent) = 0;

    /**
     *
     */
    virtual void HandleMtmGroupDeinstalledL() = 0;

    /**
     *
     * @param aErrorCode
     */
    virtual void HandleGeneralErrorL(TInt aErrorCode) = 0;

    /**
     *
     */
    virtual void HandleCloseSessionL() = 0;

    /**
     *
     */
    virtual void HandleServerFailedToStartL() = 0;

    /**
     *
     */
    virtual void HandleServerTerminatedL() = 0;

    /**
     *
     */
    virtual void HandleMediaChangedL() = 0;

    /**
     *
     */
    virtual void HandleMediaUnavailableL() = 0;

    /**
     *
     */
    virtual void HandleMediaAvailableL() = 0;

    /**
     *
     */
    virtual void HandleMediaIncorrectL() = 0;

    /**
     *
     */
    virtual void HandleCorruptedIndexRebuildingL() = 0;

    /**
     *
     */
    virtual void HandleCorruptedIndexRebuiltL() = 0;
};

#endif

// End of File
