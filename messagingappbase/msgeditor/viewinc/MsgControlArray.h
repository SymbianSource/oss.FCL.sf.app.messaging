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
* Description:  MsgControlArray  declaration
*
*/



#ifndef INC_MSGCONTROLARRAY_H
#define INC_MSGCONTROLARRAY_H

// ========== INCLUDE FILES ================================

#include <e32base.h>                       // for CArrayPtrFlat

#include "MsgBaseControl.h"                // for CMsgBaseControl

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

// ========== CLASS DECLARATION ============================

/**
 * Defines an array that contains all the controls inherited from the CMsgBaseControl.
 *
 */
class CMsgControlArray : public CArrayPtrFlat <CMsgBaseControl>
{

public:

    /**
     * Constructor.
     * @param aGranularity
     */
    CMsgControlArray(TInt aGranularity);

    /**
     * Destructor.
     */
    ~CMsgControlArray();

    /**
     * Finds a control from the array based on control id aControlId and returns
     * its array index.
     * @param aControlId
     * @return
     */
    TInt ComponentIndexFromId(TInt aControlId) const;

    /**
     * Deletes a control from the array.
     * @param aIndex
     */
    void DeleteComponent(TInt aIndex);

private:

    /**
     * Constructor (not available).
     */
    CMsgControlArray();

};

#endif

// End of File
