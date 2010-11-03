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
* Description:   UniEditor save operation inline functions    
*
*/



// ---------------------------------------------------------
// CUniEditorSaveOperation::DetachDom
// ---------------------------------------------------------
//
inline CMDXMLDocument* CUniEditorSaveOperation::DetachDom()
    {
    CMDXMLDocument* dom = iDom;
    iDom = NULL;
    return dom;
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::IsForward
// ---------------------------------------------------------
//
inline TBool CUniEditorSaveOperation::IsForward() const
    {
    return TUniMsvEntry::IsForwardedMessage( iDocument.Entry() );
    }

// End of file
