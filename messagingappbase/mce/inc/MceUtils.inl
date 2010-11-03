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
*     Inline functions for TMceFlags
*
*/



// ---------------------------------------------------------
// TMceFlags::SetMceFlag
// This is static function
// (other items were commented in a header).
// ---------------------------------------------------------
//
inline void TMceFlags::SetMceFlag( TUint32 aFlag )
    {
    iFlags = iFlags | aFlag;
    }

// ---------------------------------------------------------
// TMceFlags::ClearMceFlag
// This is static function
// (other items were commented in a header).
// ---------------------------------------------------------
//
inline void TMceFlags::ClearMceFlag( TUint32 aFlag )
    {
    iFlags = iFlags & ( ~aFlag );
    }

// ---------------------------------------------------------
// TMceFlags::MceFlag
// This is static function
// (other items were commented in a header).
// ---------------------------------------------------------
//
inline TBool TMceFlags::MceFlag( TUint32 aFlag ) const
    {
    return iFlags & aFlag;
    }

// End of File
