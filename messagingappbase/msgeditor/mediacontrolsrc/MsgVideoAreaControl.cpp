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
* Description:  
*       MsgEditor video playback area control.
*
*/



// INCLUDE FILES
#include "MsgVideoAreaControl.h"

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
// CMsgVideoAreaControl::CMsgAudioControl
//
// Constructor.
// ---------------------------------------------------------
//
CMsgVideoAreaControl::CMsgVideoAreaControl()
    {
    }

// ---------------------------------------------------------
// CMsgVideoAreaControl::ConstructL
//
//
// ---------------------------------------------------------
//
void CMsgVideoAreaControl::ConstructL( CCoeControl* aParent )
    {
    CreateWindowL( aParent );
    Window().SetNonFading( ETrue );
    Window().SetBackgroundColor( KRgbBlack );
    
    ActivateL();
    }


// ---------------------------------------------------------
// CMsgVideoAreaControl::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgVideoAreaControl* CMsgVideoAreaControl::NewL( CCoeControl* aParent )
    {
    CMsgVideoAreaControl* self = new( ELeave ) CMsgVideoAreaControl;
    
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CMsgVideoAreaControl::~CMsgVideoAreaControl
//
// Destructor
// ---------------------------------------------------------
//
CMsgVideoAreaControl::~CMsgVideoAreaControl()
    {
    }

// ---------------------------------------------------------
// CMsgVideoAreaControl::Draw
//
// Draws video area background.
// ---------------------------------------------------------
//
void CMsgVideoAreaControl::Draw( const TRect& /*aRect*/ ) const
	{
	if ( Window().DisplayMode() == EColor16MA )
		{
		CWindowGc& gc = SystemGc();
		gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
        gc.SetBrushColor( TRgb( 0, 0 ) );
		gc.Clear();  		
		}
	}

//  End of File
