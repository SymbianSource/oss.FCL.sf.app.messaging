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
* Description:  MsgEditorOperationObserver  declaration
*
*/



#ifndef INC_MSGEDITOROPERATIONOBSERVER_H
#define INC_MSGEDITOROPERATIONOBSERVER_H

// ========== INCLUDE FILES ================================

#include <msvapi.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

// ========== CLASS DECLARATION ============================

/**
 *  ?one_line_short_description.
 *  ?other_description_lines
 */
class MMsgEditorOperationObserver
{
public:
    virtual void OperationCompleted(TMsvOp aCmdId,  /*const TDesC8& aParams*/TMsvId aId) = 0;
    virtual void OperationError(TMsvOp aCmdId, TInt aErrorCode) = 0;
    virtual void OperationObservingInterrupted(TMsvOp aCmdId) = 0;
};

#endif

// End of File
