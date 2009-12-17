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
* Description:  MsgEditorAppUiExtension implementation
*
*/



// ========== INCLUDE FILES ================================

#include "MsgEditorAppUiExtension.h"    
#include "MsgEditorAppUi.h"

// ========== EXTERNAL DATA STRUCTURES =====================
// ========== EXTERNAL FUNCTION PROTOTYPES =================
// ========== CONSTANTS ====================================
// ========== MACROS =======================================
// ========== LOCAL CONSTANTS AND MACROS ===================
// ========== MODULE DATA STRUCTURES =======================
// ========== LOCAL FUNCTION PROTOTYPES ====================
// ========== LOCAL FUNCTIONS ==============================
// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgEditorAppUiExtension::CMsgEditorAppUiExtension
//
// Constructor.
// ---------------------------------------------------------
EXPORT_C CMsgEditorAppUiExtension::CMsgEditorAppUiExtension(CAknAppUi* aAppUi)
    :
    iSettingCachePlugin(aAppUi)
    {}


//  End of File
