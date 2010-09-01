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
* Description: SmilDefaultRenderer implementation
*
*/



// INCLUDE FILES
#include <smilregioninterface.h>
#include <data_caging_path_literals.hrh>
#include <gulicon.h>

#include <AknsUtils.h>
#include <AknsConstants.h>

#include "SmilDefaultRenderer.h"

#include <uniutils.mbg>
_LIT( KUniUtilsBitmapFile, "uniutils.mbm" );

#ifdef DEFAULT_DEBUG
    #include "SmilMediaLogging.h"
#endif

// LOCAL FUNCTION PROTOTYPES

// LOCAL CONSTANTS

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSmilDefaultRenderer::CSmilDefaultRenderer
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
CSmilDefaultRenderer::CSmilDefaultRenderer( MSmilMedia* aMedia,
                                            DRMCommon& aDrmCommon,
                                            CDRMHelper& aDrmHelper ) :
    CSmilMediaRendererBase( EMsgMediaUnknown, aMedia, aDrmCommon, aDrmHelper )
    {
    }

// ---------------------------------------------------------
// CSmilDefaultRenderer::ConstructL
// Symbian 2nd phase constructor can leave. Retrieves the correct
// icon for this media type depending on the situation. If correct
// mediatype is not detected then blank default renderer is created.
// Icon size is retrived from LAF if scalable LAF is present. 
// Current icon size & position is defined on MMS LAF but we 
// only use the size so we can use that value.Icon is drawn 
// on its original size or scaled so that aspect ratio is kept 
// and icon is shown as large as possible.
// ---------------------------------------------------------
void CSmilDefaultRenderer::ConstructL( RFile& aFileHandle,
                                       TBool aDRMError, 
                                       TMsgMediaType aMediaType )
    {
#ifdef DEFAULT_DEBUG
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" );
    SMILUILOGGER_ENTERFN( "[SMILUI] Default: ConstructL" );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Default: aDRMError=%d"), aDRMError );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Default: aMediaType=%d"), aMediaType );
#endif

    BaseConstructL( aFileHandle );

    TParse fileParse;
    fileParse.Set( KUniUtilsBitmapFile, &KDC_APP_BITMAP_DIR, NULL );
    
    TAknsItemID itemID( KAknsIIDNone );
    TInt bitmapIndex( -1 );
    TInt maskIndex( 0 );
    
    TBool sendingAllowed( ETrue );
    if ( aDRMError )
        {
        sendingAllowed = SendingDrmFileAllowedL( aFileHandle );
        }

    switch ( aMediaType )
        {
        case EMsgMediaImage:
            {
            if ( aDRMError )
                {
                if ( sendingAllowed )
                    {
                    itemID = KAknsIIDQgnGrafMmsImageDrm;
                    bitmapIndex = EMbmUniutilsQgn_graf_mms_image_drm;
                    maskIndex = EMbmUniutilsQgn_graf_mms_image_drm_mask;    
                    }
                else
                    {
                    itemID = KAknsIIDQgnGrafMmsImageDrmInvalidSendForbid;
                    bitmapIndex = EMbmUniutilsQgn_graf_mms_image_drm_invalid_send_forbid;
                    maskIndex = EMbmUniutilsQgn_graf_mms_image_drm_invalid_send_forbid_mask;
                    }
                }
            else
                {
                itemID = KAknsIIDQgnGrafMmsImageCorrupted;
                bitmapIndex = EMbmUniutilsQgn_graf_mms_image_corrupted;
                maskIndex = EMbmUniutilsQgn_graf_mms_image_corrupted_mask;
                }
            break;
            }
        case EMsgMediaVideo:
            {
            if ( aDRMError )
                {
                if ( sendingAllowed )
                    {
                    itemID = KAknsIIDQgnGrafMmsVideoDrm;
                    bitmapIndex = EMbmUniutilsQgn_graf_mms_video_drm;
                    maskIndex = EMbmUniutilsQgn_graf_mms_video_drm_mask;
                    }
                else
                    {
                    itemID = KAknsIIDQgnGrafMmsVideoDrmInvalidSendForbid;
                    bitmapIndex = EMbmUniutilsQgn_graf_mms_video_drm_invalid_send_forbid;
                    maskIndex = EMbmUniutilsQgn_graf_mms_video_drm_invalid_send_forbid_mask;
                    }
                }
            else
                {
                itemID = KAknsIIDQgnGrafMmsVideoCorrupted;
                bitmapIndex =  EMbmUniutilsQgn_graf_mms_video_corrupted;
                maskIndex =  EMbmUniutilsQgn_graf_mms_video_corrupted_mask;
                }
            break;
            }
        case EMsgMediaSvg:
            {
            if ( aDRMError )
                {
                if ( sendingAllowed )
                    {
                    itemID = KAknsIIDQgnGrafMmsPresDrmInvalidSend;
                    bitmapIndex = EMbmUniutilsQgn_graf_mms_pres_drm_invalid_send;
                    maskIndex = EMbmUniutilsQgn_graf_mms_pres_drm_invalid_send_mask;
                    }
                else
                    {
                    itemID = KAknsIIDQgnGrafMmsPresDrmInvalidSendForbid;
                    bitmapIndex = EMbmUniutilsQgn_graf_mms_pres_drm_invalid_send_forbid;
                    maskIndex = EMbmUniutilsQgn_graf_mms_pres_drm_invalid_send_forbid_mask;
                    }
                }
            else
                {
                itemID = KAknsIIDQgnGrafMmsPresCorrupted;
                bitmapIndex =  EMbmUniutilsQgn_graf_mms_pres_corrupted;
                maskIndex =  EMbmUniutilsQgn_graf_mms_pres_corrupted_mask;
                }
            break;
            }
        default:
            {
            break;
            }
        }
 
    if ( bitmapIndex != -1 )
        {
        TSize regionSize( iMedia->GetRectangle().Size() );

#ifdef DEFAULT_DEBUG
        SMILUILOGGER_WRITEF( _L("[SMILUI] Default: regionSize=%d %d"), regionSize.iWidth, regionSize.iHeight );
#endif
    
        iIcon = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(), 
                                           itemID, 
                                           fileParse.FullName(), 
                                           bitmapIndex, 
                                           maskIndex );
        
        TSize originalSize( RetrieveIconSize() );
        
        TSize iconSize;
            
        if ( originalSize.iHeight > regionSize.iHeight ||
             originalSize.iWidth > regionSize.iWidth )
            {
            iconSize = CalculateIconFitSize( regionSize, originalSize );
            }
        else
            {
            iconSize = originalSize;
            }
       
       iIconRect.SetRect( CalculateIconCenterPosition( regionSize, iconSize ), iconSize );

#ifdef DEFAULT_DEBUG
        SMILUILOGGER_WRITEF( _L("[SMILUI] Default: iIconRect=%d %d"), 
                            iIconRect.Size().iWidth, 
                            iIconRect.Size().iHeight );
#endif  

        User::LeaveIfError( AknIconUtils::SetSize( iIcon->Bitmap(), iIconRect.Size() ) );        
        }
        
#ifdef DEFAULT_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Default: ConstructL" );
#endif
    }

// ---------------------------------------------------------
// CSmilDefaultRenderer::NewL
// Static constructor.
// ---------------------------------------------------------
CSmilDefaultRenderer* CSmilDefaultRenderer::NewL( RFile& aFileHandle,
                                                  MSmilMedia* aMedia,
                                                  DRMCommon& aDrmCommon,
                                                  CDRMHelper& aDrmHelper,
                                                  TMsgMediaType aMediaType, 
                                                  TBool aDRMError )
    {
    CSmilDefaultRenderer* self = new(ELeave) CSmilDefaultRenderer( aMedia,
                                                                   aDrmCommon,
                                                                   aDrmHelper );
    
    CleanupStack::PushL( self );
    self->ConstructL( aFileHandle, aDRMError, aMediaType );
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------
// CSmilDefaultRenderer::~CSmilDefaultRenderer
// Destructor
// ---------------------------------------------------------    
CSmilDefaultRenderer::~CSmilDefaultRenderer()
    {
    delete iIcon;

    // Not owned
    iMedia = NULL; //For LINT
    }

// ---------------------------------------------------------
// CSmilDefaultRenderer::IsVisual
// Default icon is visual (i.e. draws something to the screen.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CSmilDefaultRenderer::IsVisual() const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// CSmilDefaultRenderer::IntrinsicWidth
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CSmilDefaultRenderer::IntrinsicWidth() const
    {
    return iMedia->GetRectangle().Size().iWidth;
    }

// ---------------------------------------------------------
// CSmilDefaultRenderer::IntrinsicHeight
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CSmilDefaultRenderer::IntrinsicHeight() const
    {
    return iMedia->GetRectangle().Size().iHeight;
    }

// ---------------------------------------------------------
// CSmilDefaultRenderer::IntrinsicDuration
// Returns zero as default icon is static.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TSmilTime CSmilDefaultRenderer::IntrinsicDuration() const
    {
    return 0;
    }

// ---------------------------------------------------------
// CSmilDefaultRenderer::Draw
// Performs the drawing of the icon to specific place and size.
// Transition effects are not supported with default renderer.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSmilDefaultRenderer::Draw( CGraphicsContext& aGc, 
                                 const TRect& /*aRect*/, 
                                 CSmilTransitionFilter* /*aTransitionFilter*/, 
                                 const MSmilFocus* aFocus )
    {
#ifdef DEFAULT_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Default: Draw" );
#endif

    if ( iVisible )
        {
        if ( iIcon )
            {            
            //Top left corner of bitmap should be in this coordinate
            TPoint tlc( iMedia->GetRectangle().iTl );
            tlc += iIconRect.iTl;
            
            // Source rectangle.
            TRect sourceRect( TPoint( 0, 0 ), iIconRect.Size() );
            
            // Cast graphics context to CFbsBitGc
            CBitmapContext* tmpBitmapContext = static_cast<CBitmapContext*>( &aGc );

#ifdef DEFAULT_DEBUG
            SMILUILOGGER_WRITEF( _L("[SMILUI] Default: tlc=%d %d"), 
                                 tlc.iX, 
                                 tlc.iY );
            SMILUILOGGER_WRITEF( _L("[SMILUI] Default: sourceRect=%d %d %d %d"), 
                                 sourceRect.iTl.iX, 
                                 sourceRect.iTl.iY,
                                 sourceRect.iBr.iX,
                                 sourceRect.iBr.iY );
#endif
            
            if ( iIcon->Mask() )
                {                   
                tmpBitmapContext->BitBltMasked( tlc,
                                                iIcon->Bitmap(), 
                                                sourceRect,
                                                iIcon->Mask(), 
                                                EFalse );
                }
            else
                {                    
                tmpBitmapContext->BitBlt( tlc, 
                                          iIcon->Bitmap(), 
                                          sourceRect );
                }
            }
            
        DrawFocus( aGc, aFocus );
        }

#ifdef DEFAULT_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Default: Draw" );
#endif
    }

// ---------------------------------------------------------
// CSmilDefaultRenderer::PrepareMediaL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSmilDefaultRenderer::PrepareMediaL()
    {
    }

// ---------------------------------------------------------
// CSmilDefaultRenderer::SeekMediaL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSmilDefaultRenderer::SeekMediaL( const TSmilTime& /*aTime*/ )
    {
    }

// ---------------------------------------------------------
// CSmilDefaultRenderer::ShowMediaL
// Sets renderer visible and draws it to the screen.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSmilDefaultRenderer::ShowMediaL()
    {
#ifdef DEFAULT_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Default: ShowMediaL" );
#endif

    if ( !iVisible )
        {
        iVisible = ETrue;
        iMedia->Redraw();
        }

#ifdef DEFAULT_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Default: ShowMediaL" );
#endif
    }

// ---------------------------------------------------------
// CSmilDefaultRenderer::HideMedia
// Hides the renderer and redraws the occupied area.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSmilDefaultRenderer::HideMedia()
    {
#ifdef DEFAULT_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Default: HideMedia" );
#endif

    if ( iVisible )
        {
        iVisible = EFalse;
        iMedia->Redraw();
        }
    
#ifdef DEFAULT_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Default: HideMedia" );
#endif
    }

// ---------------------------------------------------------
// CSmilDefaultRenderer::FreezeMedia
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSmilDefaultRenderer::FreezeMedia()
    {
    }

// ---------------------------------------------------------
// CSmilDefaultRenderer::ResumeMedia
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSmilDefaultRenderer::ResumeMedia()
    {
    }

// ---------------------------------------------------------
// CSmilDefaultRenderer::Close
// Deletes the renderer.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSmilDefaultRenderer::Close()
    {
    delete this;
    }

// ----------------------------------------------------------------------------
// CSmilDefaultRenderer::CalculateIconFitSize
// Calculates correct icon size so that icon aspect ratio is preserved 
// and it is shown fully on region as large as possible). 
// ----------------------------------------------------------------------------
//
TSize CSmilDefaultRenderer::CalculateIconFitSize( const TSize& aRegion,
                                                  const TSize& aIcon ) const
    {
    TSize result;
    
    if ( aIcon.iWidth != 0 &&
         aIcon.iHeight != 0 )
        {
        TInt targetWidth = aRegion.iWidth;
        TInt scaledWidth = aRegion.iHeight * aIcon.iWidth / aIcon.iHeight;
        
        if ( targetWidth < scaledWidth )
            {
            result.iWidth = aRegion.iWidth;
            result.iHeight = aIcon.iHeight * aRegion.iWidth / aIcon.iWidth;
            }
        else
            {
            result.iWidth = aIcon.iWidth * aRegion.iHeight / aIcon.iHeight; 
            result.iHeight = aRegion.iHeight;
            }
        }
    
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilDefaultRenderer::CalculateIconCenterPosition
// Calculates correct icon position inside overall renderer region so that 
// icon is centeralized according height and width.
// ----------------------------------------------------------------------------
//
TPoint CSmilDefaultRenderer::CalculateIconCenterPosition( const TSize& aRegion,
                                                          const TSize& aIcon ) const
    {
    return TPoint( ( aRegion.iWidth - aIcon.iWidth ) / 2, 
                   ( aRegion.iHeight - aIcon.iHeight ) / 2 );
    }


//  End of File  
