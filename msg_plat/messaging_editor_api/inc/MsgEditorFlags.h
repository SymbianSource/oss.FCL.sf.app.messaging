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
*     This class contains utility of storing and using flags
*     with message editor model
*
*/



#ifndef INC_MSGEDITORFLAGS_H
#define INC_MSGEDITORFLAGS_H

// ========== INCLUDE FILES ================================

#include <e32std.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class RWriteStream;
class RReadStream;

// ========== CLASS DECLARATION ============================

/**
 * Utility class of storing and using flags with message editor model.
 *
 */
class TMsgEditorFlags
{

public:  // constructors

    /**
     * Constructor.
     */
    IMPORT_C TMsgEditorFlags();

    /**
     * Copy constructor.
     * @param aFlags
     */
    IMPORT_C TMsgEditorFlags(const TMsgEditorFlags& aFlags);

    /**
     * Constructor.
     * @param aMask
     */
    IMPORT_C TMsgEditorFlags(TUint32 aMask);

    /**
     * Assignment operator.
     * @param aFlags
     * @return
     */
    IMPORT_C TMsgEditorFlags& operator=(const TMsgEditorFlags& aFlags);

public:  // management

    /**
     * Sets the given flags (several flags may be set at the same time).
     * @param aMask
     */
    IMPORT_C void Set(TUint32 aMask);

    /**
     * Clears given flags.
     * @param aMask
     */
    IMPORT_C void Clear(TUint32 aMask);

    /**
     * Returns ETrue, if all flags defined in aFlag parameter are set.
     * @param aMask
     * @return
     */
    IMPORT_C TBool IsSet(TUint32 aMask) const;

public:  // storing and restoring

    /**
     * Write flags into given stream.
     * @param aStream
     */
    IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

    /**
     * Read flags from given stream.
     * @param aStream
     */
    IMPORT_C void InternalizeL(RReadStream& aStream);

private:  // data

    TUint32 iFlags;

};

#endif

// End of File
