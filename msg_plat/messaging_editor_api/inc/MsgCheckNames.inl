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
* Description: MsgCheckNames.inl*
*/



// ========== INLINE METHODS ===============================

// ---------------------------------------------------------
// CMsgCheckNames::ContactMatcher
// ---------------------------------------------------------
//
inline CContactMatcher& CMsgCheckNames::ContactMatcher() const
    {
    return *iCommonUtils;
    }
