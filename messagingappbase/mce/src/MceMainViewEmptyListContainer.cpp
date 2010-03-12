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
#include <eikenv.h>
#include <aknenv.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
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
    iBgContext = CAknsBasicBackgroundControlContext::NewL(
                KAknsIIDQsnBgAreaMainMessage, aRect, ETrue);
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
    delete iBgContext;
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
// CMceMainViewEmptyListContainer::MopSupplyObject
// ---------------------------------------------------------
//
TTypeUid::Ptr CMceMainViewEmptyListContainer::MopSupplyObject(TTypeUid aId)
    {
    if (aId.iUid == MAknsControlContext::ETypeId)
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject(aId);
    }
// ---------------------------------------------------------
// CMceMainViewEmptyListContainer::SizeChanged
// ---------------------------------------------------------
//
void CMceMainViewEmptyListContainer::SizeChanged()
    {
    if ( iBgContext )
        {
        iBgContext->SetRect( Rect() );
        iBgContext->SetParentPos( PositionRelativeToScreen() );
       }
    }
// ---------------------------------------------------------
// CMceMainViewEmptyListContainer::Draw
// ---------------------------------------------------------
//
void CMceMainViewEmptyListContainer::Draw(const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    TRect aRect = Rect();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );

    if( !AknsDrawUtils::Background( skin, cc, this, gc, aRect ) )
        {
        // Same as CCoeControl draw for blank controls
        CGraphicsContext& gcBlank = SystemGc();
        gcBlank.SetPenStyle( CGraphicsContext::ENullPen );
        gcBlank.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gcBlank.DrawRect( aRect );
      }

    }

//  End of File
