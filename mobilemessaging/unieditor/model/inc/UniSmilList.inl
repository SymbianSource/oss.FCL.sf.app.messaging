/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       CUniSmilList, List of attachments. 
*
*/



// ---------------------------------------------------------
// CUniSmilList::SmilByteSize
//
// Accessor.
// ---------------------------------------------------------
//
inline TInt CUniSmilList::SmilByteSize() const
    {
    return iSmilSize;
    }

// ---------------------------------------------------------
// CUniSmilList::Count
//
// Accessor.
// ---------------------------------------------------------
//
inline TInt CUniSmilList::Count() const
    {
    return iSmilAttachmentArray->Count();
    }

