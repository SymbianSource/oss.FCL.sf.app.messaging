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
*       CNotViewerDocument, Not Viewer application document class.
*       The document is part of the Symbian OS application framework.
*
*/




// ---------------------------------------------------------
// CNotViewerDocument::SetHeaderModified
//
// Mutator.
// ---------------------------------------------------------
//
inline void CNotViewerDocument::SetHeaderModified( TBool aModified )
    {
    iHeaderModified = aModified;
    }

// ---------------------------------------------------------
// CNotViewerDocument::HeaderModified
//
// Accessor.
// ---------------------------------------------------------
//
inline TBool CNotViewerDocument::HeaderModified() const
    {
    return iHeaderModified;
    }

