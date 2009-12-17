/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       MsgEditor bitmap container control.
*
*/



// INCLUDE FILES
#include "MsgBitmapControl.h"

#include <AknsDrawUtils.h>          // AknsDrawUtils
#include <aknenv.h>

#include <MIHLBitmap.h>             // MIHLBitmap

// ==========================================================

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgBitmapControl::CMsgBitmapControl
//
// Constructor.
// ---------------------------------------------------------
//
CMsgBitmapControl::CMsgBitmapControl() 
    {
    }

// ---------------------------------------------------------
// CMsgBitmapControl::~CMsgBitmapControl
//
// Destructor
// ---------------------------------------------------------
//
CMsgBitmapControl::~CMsgBitmapControl()
    {
    }

// ---------------------------------------------------------
// CMsgBitmapControl::SetBitmap
//
// Starts the image loading.
// ---------------------------------------------------------
//
void CMsgBitmapControl::SetBitmap( MIHLBitmap* aBitmap )
    {
    iBitmap = aBitmap;
    }

// ---------------------------------------------------------
// CMsgBitmapControl::Draw
// Draws the bitmap.
// ---------------------------------------------------------
//
void CMsgBitmapControl::Draw( const TRect& aRect ) const
    {   
    CWindowGc& gc = SystemGc();
    
    // Draw the bitmap background either bitmap is not set or it has mask present.
    if ( !iBitmap || iBitmap->HasMask() )
        {   
        if ( !AknsDrawUtils::Background( AknsUtils::SkinInstance(), 
                                   AknsDrawUtils::ControlContext( this ), 
                                   this, 
                                   gc, 
                                   aRect, 
                                   KAknsDrawParamNoClearUnderImage ) )
            {
            gc.SetBrushColor( AKN_LAF_COLOR( 0 ) );
            gc.SetPenStyle( CGraphicsContext::ENullPen );
		    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    	    gc.DrawRect( aRect );
            }
        }
        
    if ( iBitmap )
        {
        iBitmap->Draw( gc, Position(), Size() );
        }
    }

//  End of File
