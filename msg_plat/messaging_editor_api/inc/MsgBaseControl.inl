/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: MsgBaseControl.inl*
*/



// ========== INLINE METHODS ===============================

// ---------------------------------------------------------
// CMsgBaseControl::ControlType
// ---------------------------------------------------------
//
inline TInt CMsgBaseControl::ControlType() const
    {
    return iControlType;
    }

// ---------------------------------------------------------
// CMsgBaseControl::SetControlType
// ---------------------------------------------------------
//
inline void CMsgBaseControl::SetControlType( TInt aType )
    {
    iControlType = aType;
    }
    
// ---------------------------------------------------------
// CMsgBaseControl::ControlModeFlags
// ---------------------------------------------------------
//
inline TUint32 CMsgBaseControl::ControlModeFlags() const
    {
    return iControlModeFlags;
    }

// ---------------------------------------------------------
// CMsgBaseControl::SetControlModeFlags
// ---------------------------------------------------------
//
inline void CMsgBaseControl::SetControlModeFlags( TUint32 aFlags )
    {
    iControlModeFlags = aFlags;
    }
               
//  End of File
