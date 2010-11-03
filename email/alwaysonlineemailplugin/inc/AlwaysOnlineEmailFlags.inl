/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     Inline functions for TAlwaysOnlineEmailFlags
*
*
*
*/



inline TAlwaysOnlineEmailFlags::TAlwaysOnlineEmailFlags()
    :    
    iFlags( 0 )
    {
    }

// ---------------------------------------------------------
// TAlwaysOnlineEmailFlags::SetFlag
// ---------------------------------------------------------
//
inline void TAlwaysOnlineEmailFlags::SetFlag( TUint32 aFlag )
    {
    iFlags = iFlags | aFlag;
    }

// ---------------------------------------------------------
// TAlwaysOnlineEmailFlags::ClearMceFlag
// ---------------------------------------------------------
//
inline void TAlwaysOnlineEmailFlags::ClearFlag( TUint32 aFlag )
    {
    iFlags = iFlags & ( ~aFlag );
    }

// ---------------------------------------------------------
// TAlwaysOnlineEmailFlags::MceFlag
// ---------------------------------------------------------
//
inline TBool TAlwaysOnlineEmailFlags::Flag( TUint32 aFlag ) const
    {
    return iFlags & aFlag;
    }

// End of File
