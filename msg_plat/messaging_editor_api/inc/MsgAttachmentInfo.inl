/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: MsgAttachmentInfo.inl*
*/



#include <e32std.h>

// ----------------------------------------------------------------------------
// CMsgAttachmentInfo::SetCharacterSet
// ----------------------------------------------------------------------------
//
inline void CMsgAttachmentInfo::SetCharacterSet( TInt aCharConvCharSet )
    {
    iCharset = aCharConvCharSet;
    }

// ----------------------------------------------------------------------------
// CMsgAttachmentInfo::CharacterSet
// ----------------------------------------------------------------------------
//
inline TInt CMsgAttachmentInfo::CharacterSet() const
    {
    return iCharset;
    }

// ----------------------------------------------------------------------------
// CMsgAttachmentInfo::SetEmptyAttachment
// ----------------------------------------------------------------------------
//
inline void CMsgAttachmentInfo::SetEmptyAttachment( TBool aValue )
    {
    if ( aValue )
        {
        iAttachmentFlags |= EEmptyAttachment;
        }
    else
        {
        iAttachmentFlags &= ~EEmptyAttachment;
        }
    }
        
// ----------------------------------------------------------------------------
// CMsgAttachmentInfo::IsEmptyAttachment
// ----------------------------------------------------------------------------
//
inline TBool CMsgAttachmentInfo::IsEmptyAttachment()
    {
    return iAttachmentFlags & EEmptyAttachment;
    }


// End of file

