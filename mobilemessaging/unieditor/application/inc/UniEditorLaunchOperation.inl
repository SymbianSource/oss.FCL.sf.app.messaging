/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   UniEditor launch operation inline functions    
*
*/



// ---------------------------------------------------------
// CUniEditorDocument::DetachHeader
// ---------------------------------------------------------
//
inline CUniEditorHeader* CUniEditorLaunchOperation::DetachHeader()
    {
    CUniEditorHeader* header = iHeader;
    iHeader = NULL;
    return header;
    }

// ---------------------------------------------------------
// CUniEditorLaunchOperation::DetachSlideLoader
// ---------------------------------------------------------
//
inline CUniSlideLoader* CUniEditorLaunchOperation::DetachSlideLoader()
    {
    CUniSlideLoader* slideLoader = iSlideLoader;
    iSlideLoader = NULL;
    return slideLoader;
    }

// End of file
