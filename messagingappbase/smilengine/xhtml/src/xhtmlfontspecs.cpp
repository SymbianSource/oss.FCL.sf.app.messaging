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
* Description:   Provides CXhtmlFontSpecs class methods.
*
*/



#include <eikenv.h>
#include <gulfont.h> 
#include <fontids.hrh>

#include "xhtmlfontspecs.h"



const TUid KSmallFontUid = { KScreenFontUidAb12 };
const TUid KBigFontUid = { KScreenFontUidAb17 };
const TUid KDefaultFontUid = { KScreenFontUidAb17 };
const TUid KCourierFontUid = { KScreenFontUidAb13 };


// ---------------------------------------------------------
// CXhtmlFontSpecs::CXhtmlFontSpecs 
// ---------------------------------------------------------
//
CXhtmlFontSpecs::CXhtmlFontSpecs()
    {
   	const CEikonEnv* env( CEikonEnv::Static() );
   		
    iSmallFont = env->Font( TLogicalFont( KSmallFontUid ) );
	iDefaultFont = env->Font( TLogicalFont( KDefaultFontUid ) );
	iBigFont = env->Font( TLogicalFont( KBigFontUid ) );
	iCourierFont = env->Font( TLogicalFont( KCourierFontUid ) );
   	}
    
// ---------------------------------------------------------
// CXhtmlFontSpecs::~CXhtmlFontSpecs 
// ---------------------------------------------------------
//
CXhtmlFontSpecs::~CXhtmlFontSpecs()
    {
    }

// ---------------------------------------------------------
// CXhtmlFontSpecs::SetFonts 
// ---------------------------------------------------------
//
void CXhtmlFontSpecs::SetFonts( const CFont* aBigFont,
	                            const CFont* aDefaultFont,
	                            const CFont* aSmallFont,
	                            const CFont* aCourierFont )
    {
    iBigFont = aBigFont;
    iDefaultFont = aDefaultFont;
    iSmallFont = aSmallFont;
    iCourierFont = aCourierFont;
    }
