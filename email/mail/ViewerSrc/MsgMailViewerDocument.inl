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
* Description:  Document class of Mail viewer, inline functions
*
*/

// INLINE FUNCTIONS

/// Returns character set
inline TUint CMsgMailViewerDocument::Charset() const
    {
    return iCharSet;
    }

/// Sets character set.
inline void CMsgMailViewerDocument::SetCharSet(const TUint aCharSet)
    {
    iCharSet = aCharSet;
    }

// End of File
