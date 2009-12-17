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
* Description:  MsgEditorFlags implementation
*
*/



// ========== INCLUDE FILES ================================

#include <s32strm.h>             // streams

#include "MsgEditorFlags.h"      // class

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
// TMsgEditorFlags::TMsgEditorFlags
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C TMsgEditorFlags::TMsgEditorFlags() : iFlags(0L) { }

// ---------------------------------------------------------
// TMsgEditorFlags::TMsgEditorFlags
//
// Copy constructor.
// ---------------------------------------------------------
//
EXPORT_C TMsgEditorFlags::TMsgEditorFlags(const TMsgEditorFlags& aFlags) : iFlags(aFlags.iFlags) { }

// ---------------------------------------------------------
// TMsgEditorFlags::TMsgEditorFlags
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C TMsgEditorFlags::TMsgEditorFlags(TUint32 aFlags) : iFlags(aFlags) { }

// ---------------------------------------------------------
// TMsgEditorFlags::Set
//
// Sets the given flags (several flags may be set at the same time).
// ---------------------------------------------------------
//
EXPORT_C void TMsgEditorFlags::Set(TUint32 aFlag)
{
    iFlags |= aFlag;
}

// ---------------------------------------------------------
// TMsgEditorFlags::Clear
//
// Clears given flags.
// ---------------------------------------------------------
//
EXPORT_C void TMsgEditorFlags::Clear(TUint32 aFlag)
{
    TUint32 mask = ~aFlag;
    iFlags &= mask;
}

// ---------------------------------------------------------
// TMsgEditorFlags::IsSet
//
// Returns ETrue, if all flags defined in aFlag parameter are set.
// ---------------------------------------------------------
//
EXPORT_C TBool TMsgEditorFlags::IsSet(TUint32 aFlag) const
{
    return (iFlags & aFlag) != 0;
}

// ---------------------------------------------------------
// TMsgEditorFlags::operator=
//
// Assignment operator.
// ---------------------------------------------------------
//
EXPORT_C TMsgEditorFlags& TMsgEditorFlags::operator=(const TMsgEditorFlags& aFlags)
{
    if (&aFlags != this)        // self assignment => nothing to do
    {
        iFlags = aFlags.iFlags;
    }
    return *this;
}

// ---------------------------------------------------------
// TMsgEditorFlags::ExternalizeL
//
// Write flags into given stream.
// ---------------------------------------------------------
//
EXPORT_C void TMsgEditorFlags::ExternalizeL(RWriteStream& aStream) const
{
    aStream << iFlags;
}

// ---------------------------------------------------------
// TMsgEditorFlags::InternalizeL
//
// Read flags from given stream.
// ---------------------------------------------------------
//
EXPORT_C void TMsgEditorFlags::InternalizeL(RReadStream& aStream)
{
    aStream >> iFlags;
}

//  End of File
