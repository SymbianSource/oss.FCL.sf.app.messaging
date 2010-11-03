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
* Description:  MsgEditorLauncher  declaration
*
*/



#ifndef INC_MSGEDITORLAUNCHER_H
#define INC_MSGEDITORLAUNCHER_H

// ========== INCLUDE FILES ================================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

// ========== CLASS DECLARATION ============================

/**
 * Message editor/viewer launcher interface.
 *
 */
class MMsgEditorLauncher
{
public:

    /**
     *
     */
    virtual void LaunchViewL() = 0;
};

#endif

// End of File
