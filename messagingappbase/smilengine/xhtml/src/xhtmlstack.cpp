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
* Description:   Provides class methods of classes that are stored to CXhtmlStack.
*
*/



#include "xhtmlstack.h"

// ---------------------------------------------------------
// TXhtmlStyleInfo::TXhtmlStyleInfo
// ---------------------------------------------------------
//
TXhtmlStyleInfo::TXhtmlStyleInfo(  TInt aStyleStart, CParaFormat::TAlignment aPrevAlign ):
    iStyleStart( aStyleStart ),
    iStyleChanged( EFalse ),
    iAlignmentChanged( EFalse ),
    iPrevAlign( aPrevAlign )
    {
	}
	
// ---------------------------------------------------------
// TXhtmlParaInfo::TXhtmlParaInfo
// ---------------------------------------------------------
//
TXhtmlParaInfo::TXhtmlParaInfo( TInt aParaStart, CParaFormat::TAlignment aAlignment ):
    iParaStart( aParaStart ),
    iAlignment( aAlignment )
    {
	}
	
// ---------------------------------------------------------
// TXhtmlListInfo::TXhtmlListInfo
// ---------------------------------------------------------
//
TXhtmlListInfo::TXhtmlListInfo( TInt aListContext ):
    iListIndex( 0 ),
    iListContext( aListContext )
    {
	}
