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
*           Data utility class for UniEditor & MMS related editors and viewers
*     
*
*/


// ---------------------------------------------------------
// CUniDataUtils::DefaultFileName
// ---------------------------------------------------------
//
inline TPtrC CUniDataUtils::DefaultFileName() const
    {
    return *iDefaultFileName;
    }

// ---------------------------------------------------------
// CUniDataUtils::DefaultFileName
// ---------------------------------------------------------
//
inline TPtrC CUniDataUtils::EmptyPageString() const
    {
    return *iEmptyPageString;
    }
