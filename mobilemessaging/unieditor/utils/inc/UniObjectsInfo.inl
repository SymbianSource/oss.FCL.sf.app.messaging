/*
* Copyright (c) 6200 Nokia Corporation and/or its subsidiary(-ies).
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
*           Unified Message Editor object info derived from attachment info.
*
*/



// ---------------------------------------------------------
// CUniObjectsInfo::Flags
// ---------------------------------------------------------
//
inline TUint32 CUniObjectsInfo::Flags() const
    {
    return iFlags;
    }

// ---------------------------------------------------------
// CUniObjectsInfo::SetFlags
// ---------------------------------------------------------
//
inline void CUniObjectsInfo::SetFlags ( TUint32 aFlags )
    {
    iFlags = aFlags;
    
    SetEmptyAttachment( iFlags & EMmsSlide );
    }

// ---------------------------------------------------------
// CUniObjectsInfo::Object
// ---------------------------------------------------------
//
inline CUniObject* CUniObjectsInfo::Object() const
    {
    return iObject;
    }

// ---------------------------------------------------------
// CUniObjectsInfo::Type
// ---------------------------------------------------------
//
inline MsgAttachmentUtils::TMsgAttachmentFetchType CUniObjectsInfo::Type() const
    {
    return iType;
    }

// ---------------------------------------------------------
// CUniObjectsInfo::SetType
// ---------------------------------------------------------
//
inline void CUniObjectsInfo::SetType(
    MsgAttachmentUtils::TMsgAttachmentFetchType aType )
    {
    iType = aType;
    }

// ---------------------------------------------------------
// CUniObjectsInfo::AttachmentObject
// ---------------------------------------------------------
//
inline TBool CUniObjectsInfo::AttachmentObject() const
    {
    return iAttachmentObject;
    }

// ---------------------------------------------------------
// CUniObjectsInfo::SlideNumber
// ---------------------------------------------------------
//
inline TInt CUniObjectsInfo::SlideNumber() const
    {
    return iSlideNumber;
    }

// End of File
