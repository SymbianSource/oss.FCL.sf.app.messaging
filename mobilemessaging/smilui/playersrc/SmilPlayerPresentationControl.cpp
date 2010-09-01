/*
* Copyright (c) 2004,2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SmilPlayerPresentationControl implementation
*
*/


// INCLUDE FILES
#include <eikenv.h>

#include "SmilPlayerPresentationControl.h"
#include "SmilPlayerPresentationController.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationControl::CSmilPlayerPresentationControl
// C++ constructor 
// ----------------------------------------------------------------------------
//
CSmilPlayerPresentationControl::CSmilPlayerPresentationControl(
                                CSmilPlayerPresentationController* aParent ) :
    iParent( aParent )
    {
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationControl::ConstructL
// Symbian 2nd phase constructor. Creates timer.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationControl::ConstructL()
    {
    SetContainerWindowL( *iParent );
    MakeVisible( ETrue );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationControl::NewL
// Symbian two phased constructor
// ----------------------------------------------------------------------------
//    
CSmilPlayerPresentationControl*
CSmilPlayerPresentationControl::NewL( CSmilPlayerPresentationController* aParent )
    {
    CSmilPlayerPresentationControl* self = 
            new(ELeave) CSmilPlayerPresentationControl( aParent );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationControl::~CSmilPlayerPresentation
// Destructor
// ----------------------------------------------------------------------------
//
CSmilPlayerPresentationControl::~CSmilPlayerPresentationControl()
    {
    iParent = NULL; // For LINT;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationControl::Draw
// Draws the presentation if parsed. Draw area coordinates are adjusted for SMIL
// engine to start from 0,0 before SMIL engine is asked to draw the presentation
// to bitmap buffer. When bitmap buffer is drawn to the screen only specified
// rectangle is drawn to the specified position.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationControl::Draw( const TRect& aRect ) const
    {
    TRect presRect( aRect );
    presRect.Move( -Rect().iTl );
    
    DoDrawPresentation( aRect.iTl, presRect, presRect );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationControl::RedrawArea
// Invalidates the appropriate window area. Given area comes from SMIL engine
// and must be converted from SMIL engine internal coordinates ( where presentation
// starts always from 0,0 ) to presentation control coordinates ( that might lay
// anywhere within the window).
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationControl::RedrawArea( const TRect& aArea ) const
    {
    TRect controlRect( aArea );
    controlRect.Move( Rect().iTl );
    Window().Invalidate( controlRect );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationControl::DoDrawPresentation
// Performs presentation drawing. First SMIL Engine is asked to draw given presentation
// area to presentation controller double buffer and after that double buffer is drawn
// to screen. This prevent flickering.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationControl::DoDrawPresentation( const TPoint& aDestination,
                                                         const TRect& aSourceRect,
                                                         const TRect& aPresentationRect ) const
    {
    SystemGc().SetClippingRect( Rect() );
    
    CFbsBitGc* buffercontext = iParent->BufferContext();
    buffercontext->Reset();
    
    CSmilPresentation* pres = iParent->Presentation();
    if ( pres )
        {
        if ( pres->IsTransparent() )
            {
            //needed to draw white background for transparent objects text)
            buffercontext->SetPenStyle( CGraphicsContext::ENullPen );
            buffercontext->SetBrushStyle( CGraphicsContext::ESolidBrush );
            buffercontext->SetBrushColor( KRgbWhite );
            buffercontext->DrawRect( aPresentationRect );
            }
        
        pres->Draw( *buffercontext, aPresentationRect );
        }
    
    SystemGc().BitBlt( aDestination, iParent->BufferBitmap(), aSourceRect );  
    
    SystemGc().CancelClippingRect();
    }

// End of File  
