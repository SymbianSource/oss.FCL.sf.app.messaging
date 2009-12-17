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
* Description: SmilImageRenderer implementation
*
*/



// INCLUDE FILES
#include <coemain.h>
#include <smilregioninterface.h>
#include <smilpresentation.h>
#include <smilmediainterface.h>

#include <IHLImageFactory.h>
#include <IHLViewerFactory.h>
#include <MIHLFileImage.h>
#include <MIHLBitmap.h>
#include <MIHLImageViewer.h>

#include <AknUtils.h>
#include <aknlayoutscalable_apps.cdl.h>

#include "SmilImageRenderer.h" 

#ifdef IMAGE_DEBUG
    #include "SmilMediaLogging.h"
#endif
// Scrolling is done in 1/4 steps
const TInt KScrollStepFraction = 4;

#ifdef RD_SCALABLE_UI_V2
const TInt KScrollValue = 1;
#endif

// Minimum frame delay
const TInt KMinimumFrameDelay = 100; // 100 ms
const TInt KProcessingDelayOffset = 100; // 100 ms

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CSmilImageRenderer::CSmilImageRenderer
// ----------------------------------------------------------------------------
//
CSmilImageRenderer::CSmilImageRenderer( MSmilMedia* aMedia,
                                        DRMCommon& aDrmCommon,
                                        CDRMHelper& aDrmHelper ) :
    CSmilMediaRendererBase( EMsgMediaImage, aMedia, aDrmCommon, aDrmHelper ),
    iTotalLength( 0 ),
    iPrevState( ENotReady ),
    iCurrState( ENotReady ),
    iProcessingDelay( TInt64( 0 ) )
#ifdef RD_SCALABLE_UI_V2
    , iNotYetDrawn( ETrue )
#endif    
    {
    iSmilObject = NULL;
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::ConstructL
// Checks neccessary parameters and initializes heap member variables. 
// Converts first (and in most cases only) frame of the source image and
// calculates renderer duration.
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::ConstructL( RFile& aFileHandle )
    {
#ifdef IMAGE_DEBUG
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: ConstructL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif

    BaseConstructL( aFileHandle );
    
    User::LeaveIfError( CheckDRMRights() );

    iSourceImage = IHLImageFactory::OpenFileImageL( aFileHandle );
    
    iSourceSize = iSourceImage->Size();
    iAnimation = iSourceImage->IsAnimation();
    
    iDestinationBitmap = IHLBitmap::CreateL();
    
    InitializeViewerL();
    
    SetupScrollingL();
    
    if ( iSourceImage->IsAnimation() )
        {              
        for( TInt frame = 0; frame < iSourceImage->AnimationFrameCount(); frame++ )
            {
            // iTotalLength is on millisecond and delay on microseconds.
            TInt frameDelay = iSourceImage->AnimationFrameDelay( frame ).Int() / 1000;
            if ( frameDelay < KMinimumFrameDelay )
                {
                frameDelay = KMinimumFrameDelay;
                }
            
            // iProvessingDelay is calculated at InitializeViewerL
            frameDelay = Max( I64INT( iProcessingDelay.Int64() ), frameDelay );
            iTotalLength = iTotalLength + frameDelay;
            }
        iMedia->RendererDurationChangedL();
        }
    else
        {
        if ( !iIsScrollable )
            {
            // Memory optimization. This is not needed
            // if image is not scrollable or animation.
            // Releases 1x bitmap size memory + possible mask
            delete iEngine;
            iEngine = NULL;
            
            delete iSourceImage;
            iSourceImage = NULL;
            }        
        }
#ifdef RD_SCALABLE_UI_V2    
    CCoeControl* parentControl = iMedia->Presentation()->GetPlayer()->GetControl( iMedia->Presentation() );
    SetContainerWindowL( *parentControl );
    TRect controlRect = iMedia->GetRegion()->GetRectangle();
    controlRect.Move( parentControl->Position() );
    SetRect( controlRect );
#endif    
        
#ifdef IMAGE_DEBUG  
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: ConstructL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::NewL
// ----------------------------------------------------------------------------
//
CSmilImageRenderer* CSmilImageRenderer::NewL( RFile& aFileHandle,
                                              MSmilMedia* aMedia,
                                              DRMCommon& aDrmCommon,
                                              CDRMHelper& aDrmHelper )
    {
#ifdef IMAGE_DEBUG
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: NewL" )
#endif

    CSmilImageRenderer* self = new (ELeave) CSmilImageRenderer( aMedia,
                                                                aDrmCommon,
                                                                aDrmHelper );
    
    CleanupStack::PushL( self );
    self->ConstructL( aFileHandle );
    CleanupStack::Pop( self );

#ifdef IMAGE_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: NewL" )
#endif

    return self;
    }


// ----------------------------------------------------------------------------
// CSmilImageRenderer::~CSmilImageRenderer
// ----------------------------------------------------------------------------
//
CSmilImageRenderer::~CSmilImageRenderer()
    {
#ifdef IMAGE_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: ~CSmilImageRenderer" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif

    delete iEngine;
    delete iDestinationBitmap;
    delete iSourceImage;

    iMedia = NULL; // For LINT
#ifdef RD_SCALABLE_UI_V2    
    delete iSBFrame;
#endif    
    
#ifdef IMAGE_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: ~CSmilImageRenderer" )
#endif
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::IsVisual
// ----------------------------------------------------------------------------
//
TBool CSmilImageRenderer::IsVisual() const
    {
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::IsOpaque
// Renderer is determined to be opaque (non-transparent) if
// mask is not present.
// ----------------------------------------------------------------------------
//
TBool CSmilImageRenderer::IsOpaque() const
    {
    return !iDestinationBitmap->HasMask();
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::IsScrollable
// ----------------------------------------------------------------------------
//
TBool CSmilImageRenderer::IsScrollable() const
    {
    return iIsScrollable;
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::IsControl
// All renderer are specified to be control on the base class.
// ----------------------------------------------------------------------------
//
TBool CSmilImageRenderer::IsControl() const
    {
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::IntrinsicWidth
// ----------------------------------------------------------------------------
//
TInt CSmilImageRenderer::IntrinsicWidth() const
    {
    return iSourceSize.iWidth;
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::IntrinsicHeight
// ----------------------------------------------------------------------------
//
TInt CSmilImageRenderer::IntrinsicHeight() const
    {
    return iSourceSize.iHeight;
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::IntrinsicDuration
// Returns zero if still image otherwise returns sum of animation delays.
// Duration is specified on milliseconds.
// ----------------------------------------------------------------------------
//
TSmilTime CSmilImageRenderer::IntrinsicDuration() const
    {
#ifdef IMAGE_DEBUG
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: IntrinsicDuration" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iTotalLength=%d"), iTotalLength );
#endif

    TSmilTime result( 0 );
    
    if ( iAnimation )
        {
        result = iTotalLength;
        }

#ifdef IMAGE_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: IntrinsicDuration" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif

    return result;
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::Draw
// Performs drawing only if renderer is visible. Transition filter
// performs its own drawings. Scroll bar and focus drawing is performed
// on functions inherited from base class.
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::Draw( CGraphicsContext& aGc, 
                               const TRect& aRect, 
                               CSmilTransitionFilter* aTransitionFilter, 
                               const MSmilFocus* aFocus)
    {
#ifdef IMAGE_DEBUG
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: Draw" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
    if ( iAnimation )
        {
        SMILUILOGGER_WRITEF( _L("[SMILUI] Image: currentFrame=%d"), iEngine->AnimationFrame() );       
        }
#endif
    
    if ( iCurrState == EPlaying || iCurrState == EPaused )
        {
        aGc.Reset();
        
        if ( aTransitionFilter )
            {
            const CFbsBitmap* bitmap = NULL;
            const CFbsBitmap* mask = NULL;
            bitmap = &iDestinationBitmap->Bitmap();
            if ( iDestinationBitmap->HasMask() )
                {
                mask = &iDestinationBitmap->Mask();
                }
            aTransitionFilter->Draw( aGc, 
                                     aRect, 
                                     bitmap, 
                                     iMedia->GetRectangle().iTl, 
                                     mask );
            }
        else        
            {
            TRect destinationRect( aRect );
            destinationRect.Intersection( iMedia->GetRectangle() );
        
            // Rectangle that indicates what part of the bitmap should be drawn. 
            TRect sourceRect( destinationRect );
        
            // We have to correct the origin as sourceRect is calculated relative
            // to the bitmap top left corner coordinates and not the graphical context
            // top left coordinates into which destinationRect is related.
            sourceRect.Move( -iMedia->GetRectangle().iTl );
            
            // Cast graphics context to CFbsBitGc
            CBitmapContext* tmpBitmapContext = static_cast<CBitmapContext*>( &aGc );
                
            if ( iDestinationBitmap->HasMask() )
                {
                tmpBitmapContext->SetBrushStyle( CGraphicsContext::ENullBrush );
                }
            
            iDestinationBitmap->Draw( *tmpBitmapContext, 
                                      destinationRect.iTl,
                                      sourceRect );
            }

        if ( iIsScrollable )
            {
#ifdef RD_SCALABLE_UI_V2            
            TRAP_IGNORE( DrawScrollBarL() );
#else            
            DrawScrollBars( aGc, iMedia->GetRectangle(), iImageScrollRect );
#endif            
            }
            
        DrawFocus( aGc, aFocus );
        }
      
#ifdef IMAGE_DEBUG  
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: Draw" )
#endif
    }

// ---------------------------------------------------------
// CSmilImageRenderer::DrawScrollBarL
// ---------------------------------------------------------
//
void CSmilImageRenderer::DrawScrollBarL()
    {
#ifdef RD_SCALABLE_UI_V2    
    if ( iNotYetDrawn )
        {
        TInt regionHeight = iMedia->GetRectangle().Height();
        TInt regionWidth = iMedia->GetRectangle().Width();
        TInt imageHeight = iImageScrollRect.Height();
        TInt imageWidth = iImageScrollRect.Width();
        if ( !iSBFrame )
            {
            iSBFrame = new (ELeave) CEikScrollBarFrame( this, this, ETrue );
            iSBFrame->CreateDoubleSpanScrollBarsL(ETrue, EFalse, ETrue, ETrue ); // window owning scrollbar            
         // using this does not draw sbars correctly
         // most likely it has something to do with drawing mechanism
         //   iSBFrame->DrawBackground( EFalse, EFalse );
         
                 //calculating the correct scrollspans   

           
            TInt initialScrollSpan = imageWidth/regionWidth;
            initialScrollSpan *= KScrollStepFraction;
            
            initialScrollSpan += (imageWidth % regionWidth)/(regionWidth/KScrollStepFraction);
            
            if ( (initialScrollSpan * regionWidth) < (KScrollStepFraction*imageWidth))        
                initialScrollSpan += 1;
           
            iScrollSpanX = initialScrollSpan;
            
            initialScrollSpan = imageHeight/regionHeight;
            initialScrollSpan *= KScrollStepFraction;
            
            //how many times the remainder fits into the 1/KScrollStepFraction th of a page
          //  TInt muschi = (imageHeight % regionHeight)/(regionHeight/KScrollStepFraction);
            
            initialScrollSpan += (imageHeight % regionHeight)/(regionHeight/KScrollStepFraction);
            // if it still does not match, add one to round up the result
            if ( (initialScrollSpan * regionHeight) < (KScrollStepFraction*imageHeight))        
                initialScrollSpan += 1;
            
            iScrollSpanY = initialScrollSpan;
            }
        iSBFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOn,CEikScrollBarFrame::EOn );    
         
        TEikScrollBarModel vSbarModel;
        vSbarModel.iThumbPosition = iThumbPosY; 
        vSbarModel.iScrollSpan = iScrollSpanY; 
        TInt indicatorHeight = regionHeight * regionHeight / iImageScrollRect.Height();
        indicatorHeight = Min( indicatorHeight, regionHeight );
        vSbarModel.iThumbSpan = KScrollStepFraction;//indicatorHeight / KSpanDivider;

        TEikScrollBarModel hSbarModel;
        hSbarModel.iThumbPosition = iThumbPosX; 
        hSbarModel.iScrollSpan = iScrollSpanX; 
        TInt indicatorWidth = regionWidth * regionWidth / iImageScrollRect.Width();
        indicatorWidth = Min( indicatorWidth, regionWidth );
        hSbarModel.iThumbSpan =  KScrollStepFraction;//indicatorWidth / KSpanDivider;

        // For EDoubleSpan type scrollbar
        TAknDoubleSpanScrollBarModel hDsSbarModel( hSbarModel );
        TAknDoubleSpanScrollBarModel vDsSbarModel( vSbarModel );

        TEikScrollBarFrameLayout layout;
        layout.iTilingMode = TEikScrollBarFrameLayout::EClientRectConstant;
                                         
 //not functional iSBFrame->TileL( &hDsSbarModel, &vDsSbarModel, rect, rect, layout );
        iSBFrame->Tile( &hDsSbarModel, &vDsSbarModel);
        
        TRect controlRect = iMedia->GetRegion()->GetRectangle();
        controlRect.Move( (iMedia->Presentation()->GetPlayer()->
                        GetControl( iMedia->Presentation()))->Position() );
        
        TAknWindowLineLayout hLayout( AknLayoutScalable_Apps::scroll_pane_cp018( 2 ) ); // laf value
		AknLayoutUtils::LayoutHorizontalScrollBar( iSBFrame, controlRect, hLayout );
		TAknWindowLineLayout vLayout( AknLayoutScalable_Apps::scroll_pane_cp016( 2 ) ); // laf value
		AknLayoutUtils::LayoutVerticalScrollBar( iSBFrame, controlRect, vLayout );
		
		iSBFrame->SetVFocusPosToThumbPos( vDsSbarModel.FocusPosition() );
		iSBFrame->MoveHorizThumbTo( hDsSbarModel.FocusPosition() );
      
        iNotYetDrawn = EFalse;
        }
#else

#endif
    }


// ---------------------------------------------------------
// CSmilImageRenderer::Scroll
// Determines if scrolling is possible to specified direction.
// If it is calculates how many pixels can be scrolled to
// that direction (scrolling is performed on 1/4 of the original
// image size steps but can be smaller if available space limits it).
// Moving of souce rectangle and scrolling rectangle is done
// on mirror image due to they opposite coordinates. Active wait
// is used on scrolling. Active scheduler is started to 
// wait until asynchronous operation triggered by source rectangle
// moving is completed.
// ---------------------------------------------------------
//
void CSmilImageRenderer::Scroll( TInt aDirX, TInt aDirY )
    {
#ifdef IMAGE_DEBUG
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: Scroll" )
#endif

        
    ScrollWithOutThumbUpdate( aDirX, aDirY );

#ifdef RD_SCALABLE_UI_V2
    //first moving in y-direction
    if ( iThumbPosY > 0 && aDirY < 0 ||
        iThumbPosY < iScrollSpanY-KScrollStepFraction && aDirY > 0 )
        {
        iThumbPosY += aDirY;
        iSBFrame->MoveVertThumbTo( iThumbPosY );
        }
      //then in x-direction
    if ( iThumbPosX > 0 && aDirX < 0 ||
        iThumbPosX < iScrollSpanY-KScrollStepFraction && aDirX > 0 )
        {
        iThumbPosX += aDirX;
        iSBFrame->MoveHorizThumbTo( iThumbPosX );
        }
#endif        
      
        
#ifdef IMAGE_DEBUG  
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: Scroll" )
#endif
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::PrepareMediaL
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::PrepareMediaL()
    {
#ifdef IMAGE_DEBUG
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: PrepareMediaL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif

    if ( iCurrState == EReady )
        {
        SetState( EHidden );
        }

#ifdef IMAGE_DEBUG  
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: PrepareMediaL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::SeekMediaL
// Seeking affects only animations. If animation is currently stopped
// to last frame and animation is wanted to start playback from the beginnning
// then only starting the engine again is neccessary. Otherwise speficied frame
// is converted.
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::SeekMediaL(const TSmilTime& aTime)
    {
#ifdef IMAGE_DEBUG  
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: SeekMediaL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif

    if ( iAnimation )
        {
        // aTime is in milliseconds
        TInt time( aTime.Value() );

        TInt frame( 0 );
        if ( time > 0 && iSourceImage->IsAnimation() )
            {
            TInt cumTime( 0 );
            while ( cumTime <= time && frame < iSourceImage->AnimationFrameCount() )
                {
                // cumTime is on millisecond and delay on microseconds.
                cumTime += iSourceImage->AnimationFrameDelay( frame ).Int() / 1000 + 
                           I64INT( iProcessingDelay.Int64() );
                frame++;
                }
            frame--;
            }
        
        ConvertFrameL( frame );
        
        if( aTime == 0 && iCurrState == EPlaying )
            {
#ifdef IMAGE_DEBUG          
            SMILUILOGGER_WRITE( "[SMILUI] Image: iEngine->Play() in SeekMediaL()" )
#endif      
            iEngine->Play();
            }
        }
        
#ifdef IMAGE_DEBUG  
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: SeekMediaL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif    
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::ShowMediaL
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::ShowMediaL()
    {
#ifdef IMAGE_DEBUG      
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: ShowMediaL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif

    if ( iCurrState == EHidden )
        {
        if ( iAnimation )
            {
#ifdef IMAGE_DEBUG          
            SMILUILOGGER_WRITE( "[SMILUI] Image: iEngine->Play() in ShowMedia" );
#endif
            iEngine->Play();
            }
            
        SetState( EPlaying );
        
        iMedia->Redraw();
        }

#ifdef IMAGE_DEBUG          
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: ShowMediaL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif    
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::HideMedia
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::HideMedia()
    {
#ifdef IMAGE_DEBUG      
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: HideMedia" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif

    if ( iCurrState == EPlaying ||
         iCurrState == EPaused )
        {
        if ( iAnimation )
            {
#ifdef IMAGE_DEBUG          
            SMILUILOGGER_WRITE( "[SMILUI] Image: iEngine->Stop()" )
#endif        

            iEngine->Stop();    
            }
        
        SetState( EHidden );
        }
        
    iMedia->Redraw();
#ifdef RD_SCALABLE_UI_V2    
    iNotYetDrawn = ETrue;
    if ( iSBFrame )
        {
        TRAP_IGNORE( iSBFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, 
                                                        CEikScrollBarFrame::EOff ) );
        }
#endif
#ifdef IMAGE_DEBUG  
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: HideMedia" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif    
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::FreezeMedia
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::FreezeMedia()
    {
#ifdef IMAGE_DEBUG      
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: FreezeMedia" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif

    if ( iCurrState == EPlaying )
        {
        SetState( EPaused );
        
        if ( iAnimation )
            {
            iEngine->Stop();
            }
        }

#ifdef IMAGE_DEBUG  
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: FreezeMedia" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif    
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::ResumeMedia
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::ResumeMedia()
    {
#ifdef IMAGE_DEBUG      
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: ResumeMedia" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif

    if ( iCurrState == EPaused )
        {        
        if ( iAnimation )
            {
#ifdef IMAGE_DEBUG          
            SMILUILOGGER_WRITE( "[SMILUI] Image: iEngine->Play() in ResumeMedia" )
#endif        

            iEngine->Play();
            }
            
        SetState( EPlaying );
        
        iMedia->Redraw();
        }

#ifdef IMAGE_DEBUG  
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: ResumeMedia" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif    
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::Close
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::Close()
    {
#ifdef IMAGE_DEBUG      
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: Close()" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif
    
    delete this;

#ifdef IMAGE_DEBUG  
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: Close()" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif    
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::InitializeViewerL
// Specific option is needed to trigger scaling without maintaining
// aspect ratio. On this case also zoom ration cannot be set.Source rectangle
// position is always set to top left corner. 
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::InitializeViewerL()
    {
#ifdef IMAGE_DEBUG      
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: InitializeViewerL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif

    TSize imageSize( iSourceImage->Size() );
    
    TSize viewerSize( iSourceImage->Size() );
    TReal zoomRatio( 1.0 );
    TUint32 options( 0 );
    
    if ( iMedia->GetRegion() )
        {
        TSize regionSize( iMedia->GetRegion()->GetRectangle().Size() );

        switch ( iMedia->GetRegion()->GetFit() )
            {
            // FILL: Scale image to fit region exactly.
            case MSmilRegion::EFill:
                {
                viewerSize = regionSize;
                options = MIHLImageViewer::EOptionIgnoreAspectRatio;
                break;
                }
            // MEET: Image fits in region. Aspect ration maintained.
            case MSmilRegion::EMeet:
                {
                zoomRatio = CalculateMeetRatio( regionSize, imageSize );
                viewerSize = regionSize;
                break;
                }
            // SLICE: Image fills region. Aspect ration maintained.
            case MSmilRegion::ESlice:
                {
                zoomRatio = CalculateSliceRatio( regionSize, imageSize );
                viewerSize = regionSize;
                break;
                }
            case MSmilRegion::EScroll:
            case MSmilRegion::EHidden:
            default:
                {
                // Hidden changed to support scrolling.
                viewerSize = regionSize;
                break;
                }
            }
        }
        
    iEngine = IHLViewerFactory::CreateImageViewerL( viewerSize, 
                                                    *iSourceImage, 
                                                    *iDestinationBitmap, 
                                                    *this, 
                                                    options );
    
    if( options == 0 ) // == keep aspect ration
        {
        User::LeaveIfError( iEngine->SetZoomRatio( zoomRatio ) );
        }
        
    User::LeaveIfError( iEngine->SetSourceRectPosition( TPoint( 0, 0 ) ) );

    TTime startTime;
    startTime.HomeTime();
    
    ConvertFrameL( 0 );
    
    TTime stopTime;
    stopTime.HomeTime();
    
    iProcessingDelay = stopTime.MicroSecondsFrom( startTime ).Int64() / 1000 + KProcessingDelayOffset ;
    
    
#ifdef IMAGE_DEBUG  
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: InitializeViewerL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif    
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::ConvertFrameL
// Triggers active wait (i.e. active scheduler is started to wait
// until frame conver has been finished) if current state is set to not ready.
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::ConvertFrameL( TInt aFrame )
    {
#ifdef IMAGE_DEBUG
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: ConvertFrameL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: currentFrame=%d"), 
                            iEngine->AnimationFrame() );
#endif

    if ( iSourceImage->IsAnimation() &&
         aFrame <= iSourceImage->AnimationFrameCount())
        {
#ifdef IMAGE_DEBUG
        SMILUILOGGER_WRITE( "[SMILUI] Image: iEngine->SetAnimationFrame()" )
#endif
        iError = iEngine->SetAnimationFrame( aFrame );            
        }
        
    if ( iCurrState == ENotReady )
        {
#ifdef IMAGE_DEBUG
        SMILUILOGGER_WRITE( "[SMILUI] Image: BeginActiveWait() starts" )
#endif

        BeginActiveWait(); // Stopped at ViewerBitmapChangedL function
        
#ifdef IMAGE_DEBUG
        SMILUILOGGER_WRITE( "[SMILUI] Image: BeginActiveWait() ends" )
        SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iError=%d"), iError );
#endif

        User::LeaveIfError( iError );
        }


#ifdef IMAGE_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: ConvertFrameL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: currentFrame=%d"), 
                            iEngine->AnimationFrame() );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::SetupScrolling
// Initializes the scroll rectangle and determines if scrolling is allowed.
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::SetupScrollingL()
    {
#ifdef IMAGE_DEBUG
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: SetupScrollingL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif

    iImageScrollRect.SetRect( iMedia->GetRectangle().iTl, TSize( iSourceSize.iWidth * iEngine->ZoomRatio(),
                                                                 iSourceSize.iHeight * iEngine->ZoomRatio() ) );

    if ( iMedia->GetRegion() )
        {
        if ( IsScrollingPossible( EScrollDown ) || 
             IsScrollingPossible( EScrollRight ) )
            {
            iIsScrollable = ETrue;
            User::LeaveIfError( iEngine->SetSourceRectPosition( TPoint( 0,0 ) ) );
            }
        }

#ifdef IMAGE_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: SetupScrollingL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::ActivateL
// Removes activation and notifies renderer that animation loop has ended.
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::ActivateL( const TSmilTime& /*aTime*/ )
    {
    iMedia->CancelActive( this );
    iMedia->RendererAtEndL();
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::ViewerBitmapChangedL
// Stops the engine and sets timer to notify the SMIL engine about the end 
// if this was the last frame of animation.
// State is changed to previous one. Changed bitmap is drawing is
// called if renderer is visible and active wait is ended if neccessary.
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::ViewerBitmapChangedL()
    {
#ifdef IMAGE_DEBUG
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: ViewerBitmapChangedL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif

    if ( iSourceImage->IsAnimation() ) 
        {
#ifdef IMAGE_DEBUG
        SMILUILOGGER_WRITE( "[SMILUI] Image: AnimationFrameChanged " )
#endif

        if ( iEngine->AnimationFrame() == iSourceImage->AnimationFrameCount() - 1 )
            {
#ifdef IMAGE_DEBUG
            SMILUILOGGER_WRITE( "[SMILUI] Image: iMedia->RendererAtEndL()" )
#endif
            
            // Last frame's delay should be counted to the animation duration or
            // else first frame is shown too quickly when looping.
            TSmilTime delay( iSourceImage->AnimationFrameDelay( iEngine->AnimationFrame() ).Int() / 1000 );
            
            // Calls SMIL engine to call ActivateL function after delay time.
            iMedia->AfterL( this, 
                            delay, 
                            EFalse ); 
      
#ifdef IMAGE_DEBUG
            SMILUILOGGER_WRITE( "[SMILUI] Image: iEngine->Stop()" )
#endif
            iEngine->Stop();
            }
        }
    
    if ( iCurrState == ENotReady )
        {
        SetState( ( iPrevState == ENotReady ) ? EReady : iPrevState );
        }
        
    if ( iCurrState == EPlaying || iCurrState == EPaused )
        {
        iMedia->Redraw();
        }

#ifdef IMAGE_DEBUG
    SMILUILOGGER_WRITE( "[SMILUI] Image: EndActiveWait()" )
#endif

    // Does nothing if there is no waiter.
    EndActiveWait();

#ifdef IMAGE_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: ViewerBitmapChangedL" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::ViewerError
// Stores the error and ends active wait if neccessary.
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::ViewerError( TInt aError )
    {
#ifdef IMAGE_DEBUG
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] Image: ViewerError" )
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: iCurrState=%d"), iCurrState );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Image: aError=%d"), aError );
#endif

    iError = aError;
    
    // Does nothing if there is no waiter.
    EndActiveWait();
    
#ifdef IMAGE_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Image: ViewerError" )
#endif
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::CalculateMeetRatio
// Calculates correct zoom ratio for meet fit attribute (i.e. bitmap should preserve
// aspect ratio and be shown fully on region as large as possible). Correct
// zoom ratio is smaller of the width and height original image size and 
// region image size ratios.
// ----------------------------------------------------------------------------
//
TReal CSmilImageRenderer::CalculateMeetRatio( const TSize& aRegion,
                                              const TSize& aImage ) const
    {
    TReal widthRatio = static_cast<TReal>( aRegion.iWidth ) / 
                       static_cast<TReal>( aImage.iWidth );
	TReal heightRatio = static_cast<TReal>( aRegion.iHeight ) / 
	                    static_cast<TReal>( aImage.iHeight );
	return ( widthRatio < heightRatio ) ? widthRatio : heightRatio;
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::CalculateSliceRatio( const TSize& aRegion, 
//                                          const TSize& aImage ) const
// Calculates correct zoom ratio for slice fit attribute (i.e. bitmap should preserve
// aspect ratio and be shown as big as possible so that atleast either height
// or width is shown fully). Correct zoom ratio is larger of the width and 
// height original image size and region image size ratios.
// ----------------------------------------------------------------------------
//
TReal CSmilImageRenderer::CalculateSliceRatio( const TSize& aRegion,
                                               const TSize& aImage ) const
    {
    TReal widthRatio = static_cast<TReal>( aRegion.iWidth ) / 
                       static_cast<TReal>( aImage.iWidth );
	TReal heightRatio = static_cast<TReal>( aRegion.iHeight ) / 
	                    static_cast<TReal>( aImage.iHeight );
	return ( widthRatio > heightRatio ) ? widthRatio : heightRatio;
    }


// ----------------------------------------------------------------------------
// CSmilImageRenderer::SetState
// Old state is saved so that it can be "reloaded" when neccessary.
// ----------------------------------------------------------------------------
//
void CSmilImageRenderer::SetState( TInt aNewState )
    {
    iPrevState = iCurrState;
    iCurrState = aNewState;
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::IsScrollingPossible
// Scrolling determined to be possible if there is atleast one pixel to move
// on specified direction.
// ---------------------------------------------------------------------------- 
//
TBool CSmilImageRenderer::IsScrollingPossible( TDirection aDirection ) const
    {
    TRect sourceRect( iEngine->SourceRect() );
    TSize sourceSize( iEngine->SourceSize() );

    switch ( aDirection )
        {
        case EScrollUp:
            {
            return ( sourceRect.iTl.iY > 0 );
            }
        case EScrollDown:
            {
            return ( sourceRect.iBr.iY < sourceSize.iHeight );
            }
        case EScrollRight:
            {
            return ( sourceRect.iBr.iX < sourceSize.iWidth );
            }
        case EScrollLeft:
            {
            return ( sourceRect.iTl.iX > 0 );
            }
        default:
           {
           break;
           }
        }
        
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CSmilImageRenderer::CalculateScrollStepSize
// Scrolling step (i.e. the amount of pixels to scroll on given direction) is
// calculated to be maximum 1/4 of original image size but no larger than
// available space.
// ---------------------------------------------------------------------------- 
//
void CSmilImageRenderer::CalculateScrollStepSize( TDirection aDirection , 
                                                  TSize& aScrollingStep, TInt aStepSize
                                                   ) const
    {    
    TRect sourceRect( iEngine->SourceRect() );
    TSize sourceSize( iEngine->SourceSize() );

	switch ( aDirection )
		{
		case EScrollLeft:
	        {
	        aScrollingStep.iWidth = aStepSize*sourceRect.Size().iWidth  / KScrollStepFraction;
	        if ( sourceRect.iTl.iX - aScrollingStep.iWidth <= 0 )
	            {
	            aScrollingStep.iWidth = sourceRect.iTl.iX;
	            }
	        aScrollingStep.iWidth = -aScrollingStep.iWidth;
			break;
			}
		case EScrollRight:
			{
			aScrollingStep.iWidth = aStepSize*sourceRect.Size().iWidth  / KScrollStepFraction;
			if ( sourceRect.iBr.iX + aScrollingStep.iWidth >= sourceSize.iWidth )
	            {
	            aScrollingStep.iWidth = sourceSize.iWidth - sourceRect.iBr.iX;
	            }
			break;
			}
		case EScrollUp:
			{
			aScrollingStep.iHeight = aStepSize*sourceRect.Size().iHeight  / KScrollStepFraction;
			if ( sourceRect.iTl.iY - aScrollingStep.iHeight <= 0 )
			    {
			    aScrollingStep.iHeight = sourceRect.iTl.iY;
			    }
			aScrollingStep.iHeight = -aScrollingStep.iHeight;
			break;
			}
		default:
		case EScrollDown:
			{
			aScrollingStep.iHeight = aStepSize*sourceRect.Size().iHeight  / KScrollStepFraction;
			if ( sourceRect.iBr.iY + aScrollingStep.iHeight >= sourceSize.iHeight )
			    {
			    aScrollingStep.iHeight = sourceSize.iHeight - sourceRect.iBr.iY;
			    }
			break;
			}
		}
    }
    
#ifdef RD_SCALABLE_UI_V2    
// -----------------------------------------------------------------------------
// CSmilImageRenderer::HandleScrollEventL
// -----------------------------------------------------------------------------
// 
void CSmilImageRenderer::HandleScrollEventL( 
                    CEikScrollBar* aScrollBar, TEikScrollEvent aEventType )
    {
    //get thumb pos for x and y
    TInt oldThumbPos = 0;
        
        //active wait check here??
        
    //check which scrollbar is being used        
    if ( aScrollBar == iSBFrame->GetScrollBarHandle(CEikScrollBar::EVertical) )
        {
        oldThumbPos = iThumbPosY;
        iThumbPosY = aScrollBar->ThumbPosition();
        }
    else
        {
        oldThumbPos = iThumbPosX;
        iThumbPosX = aScrollBar->ThumbPosition();
        }

    
    if ( AknLayoutUtils::PenEnabled() )
        {
    	switch ( aEventType )
    		{
       		case EEikScrollUp:
   		        
       		    ScrollWithOutThumbUpdate( 0, -KScrollValue );
       		    break;
       		case EEikScrollDown:
   			    ScrollWithOutThumbUpdate( 0, KScrollValue );
       		    break;
   		    case EEikScrollPageUp:
   		        {
   		        ScrollWithOutThumbUpdate( 0, -KScrollStepFraction*KScrollValue );
   		        break;
   		        }
   		    case EEikScrollPageDown:
   		        {
   		        ScrollWithOutThumbUpdate( 0, KScrollStepFraction*KScrollValue );   		     
   		        break;
   		        }
        	case EEikScrollThumbDragVert:
        	    {
                if ( !iWait.IsStarted() )// -ignoring events if busy
                    {
   		            ScrollWithOutThumbUpdate( 0, iThumbPosY - oldThumbPos );
                    }
                else //restoring old thumb value
                    {// in theory the new iThumbPosY might not correspond the the
                    //actual position, but in tests this does not seem to cause problems
                    iThumbPosY = oldThumbPos;
                    }
   	    	    break;
        	    }
        	    
      	    case EEikScrollThumbDragHoriz:
      	        {
      	        if ( !iWait.IsStarted() )// -ignoring events if busy
      	            {
   		            ScrollWithOutThumbUpdate( iThumbPosX-oldThumbPos, 0 ); 
      	            }
      	        else //restoring old thumb value
      	            {
                    iThumbPosX = oldThumbPos;
      	            }
   		        break;
      	        }            	    
        	   		        
       		case EEikScrollLeft:
       		   	{
    	        ScrollWithOutThumbUpdate( -KScrollValue, 0 );   		     
   		        break;
   		        }
       		
    		case EEikScrollRight:
    		   	{
    	        ScrollWithOutThumbUpdate( KScrollValue, 0 );   		     
   		        break;
   		        }
        	case EEikScrollPageLeft:
        	   	{
    	        ScrollWithOutThumbUpdate( -KScrollStepFraction*KScrollValue, 0 );   		     
   		        break;
   		        }
        	case EEikScrollPageRight:
        	   	{
    	        ScrollWithOutThumbUpdate( KScrollStepFraction*KScrollValue, 0 );   		     
   		        break;
   		        }

        	case EEikScrollThumbReleaseHoriz:
        	case EEikScrollThumbReleaseVert:	
            case EEikScrollHome:
        	case EEikScrollTop:
        	case EEikScrollEnd:
        	case EEikScrollBottom:
       		default:
       		    // Do nothing
       			break;
       			}
        }
    }     
#else    
void CSmilImageRenderer::HandleScrollEventL( 
                    CEikScrollBar* /*aScrollBar*/, TEikScrollEvent /*aEventType*/ )
    {  }
#endif
    
// -----------------------------------------------------------------------------
// CSmilImageRenderer::ScrollWithOutThumbUpdate
// -----------------------------------------------------------------------------
// 
void CSmilImageRenderer::ScrollWithOutThumbUpdate( TInt aDirX, TInt aDirY )
    {
    TSize scrollingStep;

    if ( ( aDirX < 0 ) && IsScrollingPossible( EScrollLeft ) )
        {
        SetState( ENotReady );
#ifdef RD_SCALABLE_UI_V2        
        CalculateScrollStepSize( EScrollLeft, scrollingStep, -aDirX );
#else
        CalculateScrollStepSize( EScrollLeft, scrollingStep );
#endif        
        }
    else if ( ( aDirX > 0 ) && IsScrollingPossible( EScrollRight ) )
        {
        SetState( ENotReady );
#ifdef RD_SCALABLE_UI_V2          
        CalculateScrollStepSize( EScrollRight, scrollingStep, aDirX );
#else        
        CalculateScrollStepSize( EScrollRight, scrollingStep );
#endif        
        }
    
    if ( ( aDirY < 0 ) && IsScrollingPossible( EScrollUp ) )
        {
        SetState( ENotReady );
#ifdef RD_SCALABLE_UI_V2          
        CalculateScrollStepSize( EScrollUp, scrollingStep, -aDirY );
#else        
        CalculateScrollStepSize( EScrollUp, scrollingStep );
#endif        
        }
    else if ( ( aDirY > 0 ) && IsScrollingPossible( EScrollDown ) )
        {
        SetState( ENotReady );
#ifdef RD_SCALABLE_UI_V2          
        CalculateScrollStepSize( EScrollDown, scrollingStep, aDirY );
#else        
        CalculateScrollStepSize( EScrollDown, scrollingStep );
#endif        
        }

    if ( iCurrState == ENotReady && iEngine )
        {
        TInt result = iEngine->MoveSourceRect( scrollingStep.iWidth, 
                                               scrollingStep.iHeight );
        if ( result == KErrNone )
            {
            iImageScrollRect.Move( -scrollingStep.iWidth, 
                                   -scrollingStep.iHeight );
            BeginActiveWait();
            
            iMedia->Redraw();
            }
        }
    }
    
//  End of File  

