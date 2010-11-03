/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Inline methods for CUniHeaders
*
*/



// ---------------------------------------------------------
// CUniHeaders::ToRecipients
// ---------------------------------------------------------
//
inline const CDesCArray& CUniHeaders::ToRecipients() const 
    {
    return *iToArray;
    }

// ---------------------------------------------------------
// CUniHeaders::CcRecipients
// ---------------------------------------------------------
//
inline const CDesCArray& CUniHeaders::CcRecipients() const
    {
    return *iCcArray;
    }
    
// ---------------------------------------------------------
// CUniHeaders::BccRecipients
// ---------------------------------------------------------
//
inline const CDesCArray& CUniHeaders::BccRecipients() const
    {
    return *iBccArray;
    }
    
// ---------------------------------------------------------
// CUniHeaders::MessageTypeSetting
// ---------------------------------------------------------
//
inline TInt32 CUniHeaders::MessageTypeSetting() const
    {
    return iMessageTypeSetting;
    }
    
// ---------------------------------------------------------
// CUniHeaders::SetMessageTypeSetting
// ---------------------------------------------------------
//
inline void CUniHeaders::SetMessageTypeSetting( TInt32 aSetting )
    {
    iMessageTypeSetting = aSetting;
    }
    
// ---------------------------------------------------------
// CUniHeaders::MessageTypeLocked
// ---------------------------------------------------------
//
inline TInt32 CUniHeaders::MessageTypeLocking() const
    {
    return iMessageTypeLocking;
    }
    
// ---------------------------------------------------------
// CUniHeaders::SetMessageTypeLocked
// ---------------------------------------------------------
//
inline void CUniHeaders::SetMessageTypeLocking( TInt32 aLocking )
    {
    iMessageTypeLocking = aLocking;
    }

// ---------------------------------------------------------
// CUniHeaders::MessageRoot
// ---------------------------------------------------------
//
inline TMsvAttachmentId CUniHeaders::MessageRoot() const
    {
    return iMessageRoot;
    }
    
// ---------------------------------------------------------
// CUniHeaders::SetMessageRoot
// ---------------------------------------------------------
//
inline void CUniHeaders::SetMessageRoot( TMsvAttachmentId aRoot )
    {
    iMessageRoot = aRoot;
    }

// End of File
