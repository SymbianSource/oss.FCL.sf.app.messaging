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
*       MsgEditor image media control - a Message Editor Base control.
*
*/



// ---------------------------------------------------------
// CMsgImageControl::SetMaxHeight
// ---------------------------------------------------------
//
inline void CMsgImageControl::SetMaxHeight( TInt aMaxHeight )
    {
    if ( aMaxHeight > 0 )
        {
        iMaxSize.iHeight = aMaxHeight;
        }
    }

// ---------------------------------------------------------
// CMsgImageControl::SetMaxSize
// ---------------------------------------------------------
//
inline void CMsgImageControl::SetMaxSize( TSize aMaxSize )
    {
    iMaxSize = aMaxSize;
    }

//  End of File
