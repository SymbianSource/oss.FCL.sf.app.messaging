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
* Description:  MsgDummyHeaderCleaner implementation
*
*/



// ========== INCLUDE FILES ================================

#include <eikenv.h>                        // for CEikonEnv
#include <AknUtils.h>                      // for AknUtils
#include <aknenv.h>                        // for CAknEnv
#include <AknDef.h>
  
#include <applayout.cdl.h> // LAF
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include <AknsDrawUtils.h>                 // for Skinned drawing
#include <AknsBasicBackgroundControlContext.h>					

#include "MsgEditorCommon.h"
#include "MsgBaseControl.h"
#include "MsgHeader.h"
#include "MsgDummyHeaderCleaner.h"

// ---------------------------------------------------------
// CMsgDummyHeaderCleaner::CMsgDummyHeaderCleaner
// ---------------------------------------------------------
CMsgDummyHeaderCleaner::CMsgDummyHeaderCleaner( const CMsgHeader* aParent )
    : iParent( aParent )
    {
    }

// ---------------------------------------------------------
// CMsgDummyHeaderCleaner::ConstructL
// ---------------------------------------------------------
void CMsgDummyHeaderCleaner::ConstructL( )
    {
    SetContainerWindowL( *iParent );

    // Lets check the position of the dashed bar
    TRect dataPane = MsgEditorCommons::MsgDataPane();
    iDashBar.LayoutRect(
        dataPane,
        AknLayoutScalable_Avkon::bg_list_pane_g4( 2 ).LayoutLine() );
    }

// ---------------------------------------------------------
// CMsgDummyHeaderCleaner::Draw
// ---------------------------------------------------------
void CMsgDummyHeaderCleaner::Draw(const TRect& /*aRect*/) const
    {
    CWindowGc& gc = SystemGc();

    TRect rect( iParent->Rect() );

    // The rect clearing will be divided into two parts:
    // First one is the rect on the left side of the dashed bar in the header
    // ie. the area on top of the label (eg. To)
    // The second one is the rect on right side of the dashed bar
    // ie. on top of the field itself (eg. To )
    TRect rectLabel(
        0,
        0,
        rect.iBr.iX,//iDashBar.Rect( ).iTl.iX-1,
        MsgEditorCommons::MsgBaseLineOffset( ) );

    TRect rectField(
        iDashBar.Rect( ).iBr.iX+1,
        0,
        rect.iBr.iX,
        MsgEditorCommons::MsgBaseLineOffset( ) );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );

    // Either draw the skin context or just clear it
    if ( !AknsDrawUtils::Background( skin, cc, this, gc, rectLabel ) )
        {
        gc.SetBrushColor( AKN_LAF_COLOR( 0 ) );
		gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
		gc.SetPenStyle( CGraphicsContext::ENullPen );
		gc.DrawRect( rectLabel );
        }
        
    if ( !AknsDrawUtils::Background( skin, cc, this, gc, rectField ) )
        {
        gc.SetBrushColor( AKN_LAF_COLOR( 0 ) );
		gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
		gc.SetPenStyle( CGraphicsContext::ENullPen );
		gc.DrawRect( rectField );
		}
    }

// ---------------------------------------------------------
// CMsgDummyHeaderCleaner::HandleResourceChange
// ---------------------------------------------------------
void CMsgDummyHeaderCleaner::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
	if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect dataPane = MsgEditorCommons::MsgDataPane();
        iDashBar.LayoutRect(
            dataPane,
            AknLayoutScalable_Avkon::bg_list_pane_g4( 2 ).LayoutLine() );
        }
    }

//  End of File
