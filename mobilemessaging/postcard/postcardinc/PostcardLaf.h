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
* Description:  PostcardLaf  declaration
*
*/



#ifndef POSTCARDLAF_H
#define POSTCARDLAF_H

// ========== INCLUDE FILES ================================

#include <e32std.h>
#include <e32def.h>
#include <eikenv.h>

// ========== CONSTANTS ====================================

// Postcard bitmaps
_LIT(KPostcardMifFile,              "postcard.mif" );
_LIT(KMmsUiLogFile,					"MMSUI.TXT");
_LIT(KPostcardResourceFile,     	"postcard.rsc");
_LIT(KMsgEditorAppUiResourceFile, 	"msgeditorappui.rsc");

_LIT(KPostcardFilenameNoOp, 		"postcard.jpg");
_LIT(KPostcardFilenameNoOpGif, 		"postcard.gif");
_LIT(KPostcardFilenameScaled, 		"postcard2.jpg");
_LIT(KPostcardFilenameCompressed, 	"postcard3.jpg");

// ========== MACROS =======================================

// ========== LOGGING MACROS ===============================

// =========================================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

// ========== CLASS DECLARATION ============================

// CLASS DECLARATION
/**
* PostcardLaf reads and returns the values of differents components from LAF.
*/
class PostcardLaf
    {
    public:

    /*
    *
    */
    static TRect MainPostcardPane( );

    /*
    *
    */
    static TRect RelativeMainPostcardPane( );

    /*
    *
    */
    static TRect PostcardPane( );

    /*
    *
    */
    static TRect RelativePostcardPane( );

    /*
    *
    */
    static TRect UpperArrow( );

    /*
    *
    */
    static TRect LowerArrow( );

    /*
    *
    */
    static TRect BackBackground( );

    /*
    *
    */
    static TRect FrontBackground( );

    /*
    *
    */
    static TRect FrontBackgroundWithoutImage( );

    /*
    *
    */
    static TRect Stamp( );

    /*
    *
    */
    static TRect InsertImageIcon( );

    /*
    *
    */
    static TRect InsertImageBg( );

    /*
    *
    */
    static TRect Image( );

    /*
    *
    */
    static TRect Address( );

    /*
    *
    */
    static TRect AddressFocus( );

    /*
    *
    */
    static TRect GreetingText( );

    /*
    *
    */
    static TRect GreetingTextFocus( );

    /*
    *
    */
    static TRect WholeMainPaneForDialogs( );


    /*
    *
    */
    static void ActiveFont( TCharFormat& aChar, TCharFormatMask& aCharMask,
    						CParaFormat& aPara, TParaFormatMask& aParaMask );

    /*
    *
    */
    static void MiniatureFont( TCharFormat& aChar, TCharFormatMask& aCharMask,
    						CParaFormat& aPara, TParaFormatMask& aParaMask );
	
    /*
    *
    */
	static TInt BaselineDelta( );
	
    /*
    *
    */
	static TInt BaselineTop( );
	
    /*
    *
    */
	static TInt LeftMargin( );

    /**
    * @since 3.2
    */
    static TInt ScrollBarWidth();
    
    /*
    * @since 3.2
    */
    static TInt GreetingFocusLineCount();
    };

#endif //ifndef POSTCARDLAF

// End of File
