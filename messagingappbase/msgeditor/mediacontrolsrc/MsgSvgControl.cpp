/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       MsgEditor svgt UI control - a Message Editor Base control.
*
*/

 

// INCLUDE FILES
#include "MsgSvgControl.h"

#include <AknDialog.h>
#include <aknlayoutscalable_apps.cdl.h>

#include <AknUtils.h>
#include <msgsvgutils.h>
#include <AknsDrawUtils.h>

#include <gulicon.h>
#include <MsgEditorCommon.h>

#include "MsgFrameControl.h"
#include "MsgIconControl.h"

#include "MsgMediaControlLogging.h"

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
// CMsgSvgControl::CMsgSvgControl
//
// Constructor.
// ---------------------------------------------------------
//
CMsgSvgControl::CMsgSvgControl( MMsgBaseControlObserver& aBaseControlObserver ) : 
    CMsgMediaControl( aBaseControlObserver, EMsgComponentIdSvg, EMsgSvgControl )
    {
    }

// ---------------------------------------------------------
// CMsgSvgControl::ConstructL
//
// ---------------------------------------------------------
//
void CMsgSvgControl::ConstructL( CMsgEditorView& aParent, MMsgAsyncControlObserver* aObserver )
    {
    BaseConstructL( aParent, aObserver );    
	
	SetSize( CalculateControlSize( MsgEditorCommons::MsgDataPane().Size() ) );
	
	iSvgUtils = new( ELeave ) CMsgSvgUtils;
    }

// ---------------------------------------------------------
// CMsgSvgControl::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
#ifdef RD_SVGT_IN_MESSAGING
EXPORT_C CMsgSvgControl* CMsgSvgControl::NewL( CMsgEditorView& aParent,
                                                   MMsgAsyncControlObserver* aObserver )
    {

    CMsgSvgControl* self = new( ELeave ) CMsgSvgControl( aParent );
    
    CleanupStack::PushL( self );
    self->ConstructL( aParent, aObserver );
    CleanupStack::Pop( self );
    
    return self;
    }
#else
EXPORT_C CMsgSvgControl* CMsgSvgControl::NewL( CMsgEditorView& /*aParent*/,
                                               MMsgAsyncControlObserver* /*aObserver*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL; //getting rid of warning
    }
#endif        

// ---------------------------------------------------------
// CMsgSvgControl::~CMsgSvgControl
//
// Destructor
// ---------------------------------------------------------
//
CMsgSvgControl::~CMsgSvgControl()
    {
    delete iSvgIndicator;
    delete iSvgUtils;
    
    iFileHandle.Close();
    }


// ---------------------------------------------------------
// CMsgSvgControl::LoadL
//
// ---------------------------------------------------------
//
void CMsgSvgControl::LoadL( RFile& aFileHandle )
    {
    iFileHandle.Close();
    User::LeaveIfError( iFileHandle.Duplicate( aFileHandle ) );
    
    DoLoadL();    
    }

// ---------------------------------------------------------
// CMsgSvgControl::DoLoadL
//
// ---------------------------------------------------------
//
void CMsgSvgControl::DoLoadL()
    {        
    TSize thumbnailSize( Size() );
    thumbnailSize = TSize( Max( 0, thumbnailSize.iWidth - iFrame->FrameBorderSize().iWidth ),
		                   Max( 0, thumbnailSize.iHeight - iFrame->FrameBorderSize().iHeight ) );
		                   
    // Use svgt thumbnail creation directly 
    CFbsBitmap* thumbnailBitmap = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( thumbnailBitmap );
    
    User::LeaveIfError( thumbnailBitmap->Create( thumbnailSize,
                                                 iCoeEnv->ScreenDevice()->DisplayMode() ) );
                                                  
    CFbsBitmap* thumbnailMask = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( thumbnailMask );
    
    User::LeaveIfError( thumbnailMask->Create( thumbnailSize,
                                               EGray256 ) );
	                                            
    CGulIcon* thumbnailIcon = CGulIcon::NewL( thumbnailBitmap, thumbnailMask );
    CleanupStack::Pop( 2, thumbnailBitmap );
    
    iIconControl->SetIcon( thumbnailIcon );
    
    // Incase this is not fulfilling the requirements, it might be an
    // option to go and check for GenerateThumbnailL() in the msgcommonutils

    iSvgUtils->GenerateThumbnailL( iFileHandle,
                                   thumbnailIcon->Bitmap(),
                                   thumbnailIcon->Mask(),
                                   EFalse,
                                   thumbnailSize );

    // Resize bitmaps to actual content size.
    thumbnailBitmap->Resize( thumbnailSize );
    thumbnailMask->Resize( thumbnailSize );
        
    iIconControl->SetSize( thumbnailSize );
    iIconControl->MakeVisible( ETrue );
    
    iFrame->SetImageSize( thumbnailSize );
    
    if ( iSvgIndicator )
        {
        iSvgIndicator->MakeVisible( ETrue );
        }    
    
    SetState( EMsgAsyncControlStateReady );
    }

// ---------------------------------------------------------
// CMsgSvgControl::Cancel
// ---------------------------------------------------------
//
void CMsgSvgControl::Cancel()
    {
    }

// ---------------------------------------------------------
// CMsgSvgControl::LoadIndicatorIconL
// ---------------------------------------------------------
//
EXPORT_C void CMsgSvgControl::LoadIndicatorIconL( const TAknsItemID& aId,
                                                  const TDesC& aFileName,
                                                  const TInt aFileBitmapId,
                                                  const TInt aFileMaskId )
    {
    delete iSvgIndicator;
    iSvgIndicator = NULL;
    
    iSvgIndicator = CMsgIconControl::NewL( *this );
    iSvgIndicator->LoadIconL( aId,
                              aFileName,
                              aFileBitmapId,
                              aFileMaskId );    

    TAknLayoutRect indicatorRect;
    indicatorRect.LayoutRect( MsgEditorCommons::MsgDataPane(), 
                              AknLayoutScalable_Apps::msg_data_pane_g7().LayoutLine() );
    
    iSvgIndicator->SetBitmapSizeL( indicatorRect.Rect().Size() );
    
    if ( iIconControl &&
         iIconControl->IsVisible() )
        {
        iSvgIndicator->MakeVisible( ETrue );
        }
    }

// ---------------------------------------------------------
// CMsgSvgControl::SetIconExtent
//
// Updates the icon position & size. Icon control is centered
// to the control area.
// ---------------------------------------------------------
//
void CMsgSvgControl::SetIconExtent()
    {
    if ( iSvgIndicator )
        {
        TSize size = iSvgIndicator->BitmapSize();
        
        TPoint position( iIconControl->Position() );
        position.iY += iIconControl->Size().iHeight;
        position.iY -= size.iHeight;
    
        iSvgIndicator->SetExtent( position, size );
        }
    }

// ---------------------------------------------------------
// CMsgSvgControl::SizeChanged
// ---------------------------------------------------------
//
void CMsgSvgControl::SizeChanged()
    {  
    CMsgMediaControl::SizeChanged();
    
    SetIconExtent();       
    }
        
// ---------------------------------------------------------
// CMsgSvgControl::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* CMsgSvgControl::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* result = NULL;
    
    switch ( aIndex )
        {
        case 0:
            {
            result = iFrame;
            break;
            }
        case 1:
            {
            result = iIconControl;
            break;
            }   
        case 2:
            {
            result = iSvgIndicator;
            break;
            }
        default:
            {
            break;
            }
        }
        
    return result;            
    }

// ---------------------------------------------------------
// CMsgSvgControl::CountComponentControls
//
// ---------------------------------------------------------
//
TInt CMsgSvgControl::CountComponentControls() const
    {
    TInt result( 2 );
    if ( iSvgIndicator )
        {
        result++;
        }
        
    return result;
    }


// ---------------------------------------------------------
// CMsgSvgControl::Draw
//
// ---------------------------------------------------------
//
void CMsgSvgControl::Draw( const TRect& aRect ) const
    {
    if ( iSvgIndicator )
        {
        CWindowGc& gc = SystemGc();
        
        if ( !AknsDrawUtils::BackgroundBetweenRects( AknsUtils::SkinInstance(), 
                                                     AknsDrawUtils::ControlContext( this ), 
                                                     this, 
                                                     gc, 
                                                     aRect,
                                                     iIconControl->Rect() ) )
            {
            gc.SetBrushColor( AKN_LAF_COLOR( 0 ) );
            gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
            gc.SetPenStyle( CGraphicsContext::ENullPen );
            gc.DrawRect( aRect );
            }
        
        gc.SetBrushColor( KRgbWhite.Value() );
        gc.SetPenStyle( CGraphicsContext::ENullPen );
	    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	    gc.DrawRect( iIconControl->Rect() );
        }
    else
        {
        CMsgMediaControl::Draw( aRect );
        }
    }

// ---------------------------------------------------------
// CMsgSvgControl::Close
// ---------------------------------------------------------
//
void CMsgSvgControl::Close()
    {
    SetState( EMsgAsyncControlStateIdle );
    }

// ---------------------------------------------------------
// CMsgSvgControl::SetAndGetSizeL
//
// Calculates and sets the size of the control and returns new size as
// reference aSize.
// ---------------------------------------------------------
//
void CMsgSvgControl::SetAndGetSizeL( TSize& aSize )
    {
    aSize = CalculateControlSize( aSize );
    SetSize( aSize );
    }

// ---------------------------------------------------------
// CMsgSvgControl::HandleResourceChange
// ---------------------------------------------------------
//
void CMsgSvgControl::HandleResourceChange( TInt aType )
    {
    CMsgMediaControl::HandleResourceChange( aType ); 
    
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetSize( CalculateControlSize( MsgEditorCommons::MsgDataPane().Size() ) );
        
        TRAP_IGNORE( DoLoadL() );
        }
    }


// ---------------------------------------------------------
// CMsgSvgControl::PlayL
// ---------------------------------------------------------
//    
void CMsgSvgControl::PlayL()
    {   
    }
  
// ---------------------------------------------------------
// CMsgSvgControl::Stop
// ---------------------------------------------------------
//    
void CMsgSvgControl::Stop()
    {
    }
    
// ---------------------------------------------------------
// CMsgSvgControl::PauseL
// ---------------------------------------------------------
//    
void CMsgSvgControl::PauseL()
    {
    }

// ---------------------------------------------------------
// CMsgSvgControl::CalculateControlSize
// ---------------------------------------------------------
//    
TSize CMsgSvgControl::CalculateControlSize( TSize aProposedSize ) const
    {
    TAknLayoutRect controLayout;
    controLayout.LayoutRect( MsgEditorCommons::MsgDataPane(),
                             AknLayoutScalable_Apps::msg_data_pane_g1().LayoutLine() );
    
    TSize controlSize = controLayout.Rect().Size();
    
    // Make sure controlSize is multiple of iBaseLine
    controlSize.iHeight = iBaseLine * ( controlSize.iHeight / iBaseLine );
		                 
    if ( controlSize.iHeight > aProposedSize.iHeight )
        {
        controlSize.iHeight = aProposedSize.iHeight;
        }
    
    if ( controlSize.iWidth > aProposedSize.iWidth )
        {
        controlSize.iWidth = aProposedSize.iWidth;
        }
    
    return controlSize;
    }

//  End of File

