/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       Base class for SMIL Player media renderers
*
*/



#include <eikenv.h>
#include <AknUtils.h>

// DRM
#include <DRMHelper.h>
#include <caf/data.h>

// LAF
#include <AknUtils.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>


#include "SmilMediaRendererBase.h"

//These are from LAF:
const TInt KScrollBarBGColorID = 218;
const TInt KScrollBarFGColorID = 221;
const TInt KScrollBarLineColorID = 0;

using namespace ContentAccess;

// ---------------------------------------------------------
// CSmilMediaRendererBase::CSmilMediaRendererBase
// C++ default constructor. Initializes class member variables.
// ---------------------------------------------------------
//
CSmilMediaRendererBase::CSmilMediaRendererBase( TMsgMediaType aMediaType,
                                                MSmilMedia* aMedia, 
                                                DRMCommon& aDrmCommon, 
                                                CDRMHelper& aDrmHelper ) :
    iMediaType( aMediaType ),
    iDrmCommon( aDrmCommon ),
    iDrmHelper( aDrmHelper ),
    iDrmProtection( DRMCommon::ENoDCFFile ),
    iMedia( aMedia )
    {
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::BaseConstructL
// Base class 2nd phase constructor. Should be called from derived classes if
// base class services are used.
// ---------------------------------------------------------
//
void CSmilMediaRendererBase::BaseConstructL( RFile& aFileHandle )
    {
    HBufC8* drmMime = NULL; // dummy
    TUint drmData = 0;
    
    iDrmCommon.GetFileInfo( aFileHandle,
                            iDrmProtection,
                            drmMime,
                            iDrmUri,
                            drmData );
        
    delete drmMime;
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::~CSmilMediaRendererBase
// Destructor
// ---------------------------------------------------------
//
CSmilMediaRendererBase::~CSmilMediaRendererBase()
    {
    if( iDrmProtection != DRMCommon::ENoDCFFile &&
        iDrmRightConsumed )
        {
        iDrmHelper.Consume2( *iDrmUri,
                             Intent(),
                             CDRMHelper::EFinish );            
        }

    delete iDrmUri;
    delete iEventArray;
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::Close
// ---------------------------------------------------------
//
void CSmilMediaRendererBase::Close()
    {
    delete this;
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::IsVisual
// ---------------------------------------------------------
//
TBool CSmilMediaRendererBase::IsVisual() const
    {
    return EFalse;
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::IsOpaque
// ---------------------------------------------------------
//
TBool CSmilMediaRendererBase::IsOpaque() const
    {
    return EFalse;
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::IsScrollable
// ---------------------------------------------------------
//
TBool CSmilMediaRendererBase::IsScrollable() const
    {
    return EFalse;
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::IsControl
// ---------------------------------------------------------
//
TBool CSmilMediaRendererBase::IsControl() const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::IntrinsicWidth
// ---------------------------------------------------------
//
TInt CSmilMediaRendererBase::IntrinsicWidth() const
    {
    return 0;
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::IntrinsicHeight
// ---------------------------------------------------------
//
TInt CSmilMediaRendererBase::IntrinsicHeight() const
    {
    return 0;
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::IntrinsicDuration
// ---------------------------------------------------------
//
TSmilTime CSmilMediaRendererBase::IntrinsicDuration() const
    {
    return 0;
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::Draw
// ---------------------------------------------------------
//
void CSmilMediaRendererBase::Draw( CGraphicsContext& /*aGc*/,
                                   const TRect& /*aRect*/,
                                   CSmilTransitionFilter* /*aTransitionFilter*/,
                                   const MSmilFocus* /*aFocus*/ )
    {
    }

// ---------------------------------------------------------
// void CSmilMediaRendererBase::Scroll
// ---------------------------------------------------------
//
void CSmilMediaRendererBase::Scroll( TInt /*aDirX*/,
                                     TInt /*aDirY*/ )
    {
    }
    
// ---------------------------------------------------------
// CSmilMediaRendererBase::SetVolume
// --------------------------------------------------------
//
void CSmilMediaRendererBase::SetVolume( TInt /*aVolume*/ )
    {
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::MediaType
// --------------------------------------------------------
//
EXPORT_C TMsgMediaType CSmilMediaRendererBase::MediaType() const
    {
    return iMediaType;
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::ActivateRendererL
// --------------------------------------------------------
//
EXPORT_C void CSmilMediaRendererBase::ActivateRendererL()
    {
    }
    
// ---------------------------------------------------------
// CSmilMediaRendererBase::SendingDrmFileAllowedL
// --------------------------------------------------------
//
TBool CSmilMediaRendererBase::SendingDrmFileAllowedL( RFile& aFileHandle )
    {
    TBool result( ETrue );
    
    if ( iDrmProtection != DRMCommon::ENoDCFFile )
        {
        if ( iDrmProtection == DRMCommon::ESuperDistributable )
            {
            CData* content = CData::NewL( aFileHandle, KDefaultContentObject, EPeek );
            
            TInt sendingAllowed( 0 );
            
            if ( content->GetAttribute( EIsForwardable, sendingAllowed ) == KErrNone )
                {
                if ( sendingAllowed == 0)
                    {
                    result = EFalse;
                    }
                }
            else
                {
                result = EFalse;
                }
            
            delete content;
            }
        else
            {
            result = EFalse;
            }
        }
    
    return result;
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::DrawFocus
// ---------------------------------------------------------
//
void CSmilMediaRendererBase::DrawFocus( CGraphicsContext& aGc,
                                        const MSmilFocus* aFocus ) const
    {
    if ( aFocus )
        {
        TRect rect = iFocusRect.IsEmpty() ? aFocus->Rect() : iFocusRect;

        aGc.Reset();
        aGc.SetClippingRect( rect );

        rect.iTl.iX += 1;
        rect.iTl.iY += 1;
        aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
        aGc.SetPenColor( KRgbGray );
        aGc.DrawRect( rect );
        if (aFocus->IsSelected())
            {
            aGc.SetPenColor( KRgbBlue );
            }
        else
            {
            aGc.SetPenColor( KRgbBlack );
            }
        rect.Move( -1, -1 );
        aGc.DrawRect( rect );
        }
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::DrawScrollBars
// ---------------------------------------------------------
//
void CSmilMediaRendererBase::DrawScrollBars( CGraphicsContext& aGc,
                                             const TRect& aRegionRect,
                                             const TRect& aMediaRect ) const
    {
    aGc.Reset();
    aGc.SetClippingRect( aRegionRect );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );

    TInt regionHeight = aRegionRect.Height();
    TInt regionWidth = aRegionRect.Width();
    TInt mediaHeight = aMediaRect.Height();
    TInt mediaWidth = aMediaRect.Width();

    if ( mediaWidth &&
        ( aMediaRect.iTl.iX < aRegionRect.iTl.iX ||
        aMediaRect.iBr.iX > aRegionRect.iBr.iX ) )
        {
        // Draw horizontal scrollbar...

        // Calculate relative width of the elevator
        // (using simple proportion):
        //
        //   indicatorWidth      region total width
        // ------------------ = --------------------
        // region total width    media total width
        //
        TInt indicatorWidth = regionWidth * regionWidth / mediaWidth;
        indicatorWidth = Min( indicatorWidth, regionWidth );

        // Calculate relative position of the elevator
        // (using simple proportion):
        //
        //    indicatorXPos      media to the left of region     
        // ------------------- = ---------------------------
        // region total width         media total width
        //
        TInt indicatorXPos =
            ( aRegionRect.iTl.iX - aMediaRect.iTl.iX ) * regionWidth / mediaWidth;
        indicatorXPos = Max( 0, indicatorXPos );

        // Calculate the top left Y-coordinate for the scroll bar
        // Leave space for focus + one pixel between focus & scroll bar
        TInt topLeftY = aRegionRect.iBr.iY;
        topLeftY -= KScrollElevatorSize;
        topLeftY -= 1;
        topLeftY -= KFocusSize;

        // Calculate the rect of the scroll bar elevator
        //
        TRect xIndicator( 
            TPoint( aRegionRect.iTl.iX + indicatorXPos, topLeftY ),
            TSize( indicatorWidth, KScrollElevatorSize ) );
    
        // Draw scroll bar background
        aGc.SetPenColor( AKN_LAF_COLOR( KScrollBarBGColorID ) );
        aGc.SetBrushColor( AKN_LAF_COLOR( KScrollBarBGColorID ) );
        aGc.DrawRect( TRect( 
            TPoint( aRegionRect.iTl.iX, xIndicator.iTl.iY ),
            TSize( regionWidth, xIndicator.Height() ) ) );

        // Draw scroll bar elevator
        aGc.SetPenColor( AKN_LAF_COLOR( KScrollBarFGColorID ) );
        aGc.SetBrushColor( AKN_LAF_COLOR( KScrollBarFGColorID ) );
        aGc.DrawRect( xIndicator );

        // Draw scroll bar vertical lines
        aGc.SetPenColor( AKN_LAF_COLOR( KScrollBarLineColorID ) );
        aGc.DrawLine( 
            TPoint( xIndicator.iTl.iX, xIndicator.iTl.iY ),
            TPoint( xIndicator.iTl.iX, xIndicator.iBr.iY ) );
        aGc.DrawLine( 
            TPoint( xIndicator.iBr.iX, xIndicator.iTl.iY ),
            TPoint( xIndicator.iBr.iX, xIndicator.iBr.iY ) );
        
        }

    if ( mediaHeight &&
        ( aMediaRect.iTl.iY < aRegionRect.iTl.iY ||
        aMediaRect.iBr.iY > aRegionRect.iBr.iY ) )
        {
        // Draw vertical scrollbar...


        // Calculate relative height of the elevator
        TInt indicatorHeight = regionHeight * regionHeight / mediaHeight;
        indicatorHeight = Min( indicatorHeight, regionHeight );

        // Calculate relative position of the elevator
        TInt indicatorYPos =
            ( aRegionRect.iTl.iY - aMediaRect.iTl.iY ) * regionHeight / mediaHeight;
        indicatorYPos = Max( 0, indicatorYPos );

        // Determine whether the scroll bar should be
        // on the left or on the right side of the region
        //
        // NOTE: There's one pixel wide margin both on the left
        // and the right side of the actual elevator -> thus
        // "+1" or "-1" for the x-coordinates
        TInt topLeftX = AknLayoutUtils::LayoutMirrored()
            ? aRegionRect.iTl.iX + KFocusSize + 1
            : aRegionRect.iBr.iX - KScrollElevatorSize - 1 - KFocusSize;

        // Calculate the rect of the scroll bar elevator
        //
        TRect yIndicator( 
            TPoint( topLeftX, aRegionRect.iTl.iY + indicatorYPos ),
            TSize( KScrollElevatorSize, indicatorHeight ) );
    
        // Draw scroll bar background
        aGc.SetPenColor( AKN_LAF_COLOR( KScrollBarBGColorID ) );
        aGc.SetBrushColor( AKN_LAF_COLOR( KScrollBarBGColorID ) );
        aGc.DrawRect( TRect( 
            TPoint( yIndicator.iTl.iX, aRegionRect.iTl.iY ),
            TSize( yIndicator.Width(), aRegionRect.Height() ) ) );

        // Draw scroll bar elevator
        aGc.SetPenColor( AKN_LAF_COLOR( KScrollBarFGColorID ) );
        aGc.SetBrushColor( AKN_LAF_COLOR( KScrollBarFGColorID ) );
        aGc.DrawRect( yIndicator );

        // Draw scroll bar horizontal lines
        aGc.SetPenColor( AKN_LAF_COLOR( KScrollBarLineColorID ) );
        aGc.DrawLine( 
            TPoint( yIndicator.iTl.iX, yIndicator.iTl.iY ),
            TPoint( yIndicator.iBr.iX, yIndicator.iTl.iY ) );
        aGc.DrawLine( 
            TPoint( yIndicator.iTl.iX, yIndicator.iBr.iY ),
            TPoint( yIndicator.iBr.iX, yIndicator.iBr.iY ) );

        }

    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::BeginActiveWait
//
// Begins active wait.
// ---------------------------------------------------------
//
void CSmilMediaRendererBase::BeginActiveWait()
    {
    if( !iWait.IsStarted() )
        {
        TRAP_IGNORE( iEikonEnv->EikAppUi()->AddToStackL( this, ECoeStackPriorityCba ) );
        iWait.Start();
        }
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::EndActiveWait
//
// Ends active wait.
// ---------------------------------------------------------
//
void CSmilMediaRendererBase::EndActiveWait()
    {
    if( iWait.IsStarted() )
        {
        iEikonEnv->EikAppUi()->RemoveFromStack( this );
        if ( iEventArray )
            {
            for ( TInt current = 0; current < iEventArray->Count(); current++ )
                {
                iCoeEnv->WsSession().SimulateKeyEvent( iEventArray->At( current ) );
                }
            iEventArray->Reset();
            }
        iWait.AsyncStop();
        }
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::Intent
//
// Returns correct access intent for this media type.
// ---------------------------------------------------------
//
ContentAccess::TIntent CSmilMediaRendererBase::Intent() const
    {
    ContentAccess::TIntent result( ContentAccess::EUnknown );
    
    switch( iMediaType )
        {
        case EMsgMediaAudio:
        case EMsgMediaVideo:
            {
            result = ContentAccess::EPlay;
            break;
            }
        case EMsgMediaImage:
        case EMsgMediaText:
        case EMsgMediaSvg:
            {
            result = ContentAccess::EView;
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
// CSmilMediaRendererBase::ConsumeDRMRightsL
//
// Consumes DRM rights if media object is DRM protected.
// ---------------------------------------------------------
//
TInt CSmilMediaRendererBase::ConsumeDRMRightsL()
    {
    TInt result( DRMCommon::EOk );
    
    if ( iDrmProtection != DRMCommon::ENoDCFFile &&
         !iDrmRightConsumed )
        {
        result = iDrmHelper.Consume2( *iDrmUri,
                                      Intent(),
                                      CDRMHelper::EStart );

        iDrmRightConsumed = ETrue;
        }
        
    return result;
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::CheckDRMRights
//
// Checks DRM rights if media object is DRM protected.
// ---------------------------------------------------------
//
TInt CSmilMediaRendererBase::CheckDRMRights()
    {
    TInt result( DRMCommon::EOk );
    
    if ( iDrmProtection != DRMCommon::ENoDCFFile )
        {
        result = iDrmCommon.CheckContentRights( *iDrmUri,
                                                Intent() );

        }
        
    return result;
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::OfferKeyEventL
//
// Consumes all key events.
// ---------------------------------------------------------
//
TKeyResponse CSmilMediaRendererBase::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                     TEventCode /*aType*/ )
    {
    if ( !iEventArray )
        {
        iEventArray = new( ELeave ) CArrayFixFlat<TKeyEvent>( 2 );
        }
    iEventArray->AppendL( aKeyEvent );
    return EKeyWasConsumed;
    }

// ---------------------------------------------------------
// CSmilMediaRendererBase::RetrieveIconSize
//
// Retrieves correct icon size from LAF.
// ---------------------------------------------------------
//
TSize CSmilMediaRendererBase::RetrieveIconSize() const
    {
    TRect appWindowRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EApplicationWindow, appWindowRect );
    
    TAknLayoutRect mainPane;
    mainPane.LayoutRect( appWindowRect, AknLayoutScalable_Avkon::main_pane( 0 ) );
    
    TAknLayoutRect mainSmilPane;
    mainSmilPane.LayoutRect( mainPane.Rect(), AknLayoutScalable_Apps::main_smil_pane() );
    
    TAknLayoutRect iconLayout;
    iconLayout.LayoutRect( mainSmilPane.Rect(), AknLayoutScalable_Apps::msg_data_pane_g7() );
    
    return iconLayout.Rect().Size();
    }

    
// End of file
