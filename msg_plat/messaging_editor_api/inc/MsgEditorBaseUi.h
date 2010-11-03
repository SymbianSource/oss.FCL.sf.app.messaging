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
* Description:  MsgEditorBaseUi  declaration
*
*/



#ifndef INC_MSGEDITORBASEUI_H
#define INC_MSGEDITORBASEUI_H

// ========== INCLUDE FILES ================================

#include <aknappui.h>                   // CAknAppUi
//#include <msvstore.h>                   // RMsvWriteStream

#include "MsgEditorLauncher.h"          // MsgEditorLauncher

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

// ========== CLASS DECLARATION ============================

/**
 * Base class for Message editor/viewer application ui module.
 *
 */
class CMsgEditorBaseUi :
    public CAknAppUi,
    public MMsgEditorLauncher
{

public:

    /**
     * Constructor.
     */
    IMPORT_C CMsgEditorBaseUi();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CMsgEditorBaseUi();

    /**
     *
     */
    virtual void LaunchViewL() = 0;

protected:

    TInt iEditorBaseFeatures;

	enum TEditorBaseFlags
		{
		EDisableMSKSupport      = 1,
		EStayInViewerAfterReply = 2
		};
};

#endif

// End of File
