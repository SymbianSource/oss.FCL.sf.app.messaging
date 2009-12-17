/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Draws empty rect. Prevents flickering in mce launch.
*
*/



// INCLUDE FILES
#include    <eikenv.h>
#include <aknenv.h>
#include    "MceMainViewEmptyListContainer.h"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
CMceMainViewEmptyListContainer::CMceMainViewEmptyListContainer()
    {
    }

// Symbian OS default constructor can leave.
void CMceMainViewEmptyListContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
    SetRect( aRect );
    ActivateL();
    MakeVisible( ETrue );
    }

// Two-phased constructor.
CMceMainViewEmptyListContainer* CMceMainViewEmptyListContainer::NewL( const TRect& aRect )
    {
    CMceMainViewEmptyListContainer* self = new ( ELeave ) CMceMainViewEmptyListContainer();
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CMceMainViewEmptyListContainer::~CMceMainViewEmptyListContainer()
    {
    }


// ---------------------------------------------------------
// CMceMainViewEmptyListContainer::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CMceMainViewEmptyListContainer::OfferKeyEventL(
    const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    return EKeyWasConsumed;
    }

// ---------------------------------------------------------
// CMceMainViewEmptyListContainer::Draw
// ---------------------------------------------------------
//
void CMceMainViewEmptyListContainer::Draw(const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenColor( AKN_LAF_COLOR ( 0 ) ); // background
    gc.SetPenStyle( CGraphicsContext::ESolidPen );
    gc.SetPenSize( TSize(0,0) ); // no border needed here
    gc.DrawRect( Rect() );
    }

//  End of File
