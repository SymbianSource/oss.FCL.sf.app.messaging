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
* Description:   Implementation of CSmilSVGRenderer class.
*
*/




// INCLUDE FILES
#include <AknUtils.h>
#include <AknsUtils.h>
#include <data_caging_path_literals.hrh>
#include <gulicon.h>

#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include <AknDialog.h>
#include <SVGTUIDialog.h>
#include <SVGEngineInterfaceImpl.h>
#include <SVGTAppObserverUtil.h>

#include <smilmedia.h>
#include <uniutils.mbg>

#include "SmilSVGRenderer.h"
#include "MMediaFactoryFileInfo.h"

//#define SVG_DEBUG
#ifdef SVG_DEBUG
#include "SmilMediaLogging.h"
#endif

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const TInt KSVGDefaultDuration = 5000;

_LIT( KUniUtilsBitmapFile, "uniutils.mbm" );

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::CSmilSVGRenderer
// C++ constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSmilSVGRenderer::CSmilSVGRenderer( MSmilMedia* aMedia, 
                                    MMediaFactoryFileInfo* aFileInfo,
                                    DRMCommon& aDrmCommon,
                                    CDRMHelper& aDrmHelper ) :
    CSmilMediaRendererBase( EMsgMediaSvg, aMedia, aDrmCommon, aDrmHelper ),
    iFileInfo( aFileInfo )
    {
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSmilSVGRenderer::ConstructL( RFile& aFileHandle )
    {
#ifdef SVG_DEBUG
    SMILUILOGGER_WRITE_TIMESTAMP( " --------------------------------" )
    SMILUILOGGER_ENTERFN( "[SMILUI] SVG: ConstructL" )
#endif

    if ( iFileInfo == NULL )
        {
        User::Leave( KErrArgument );
        }
    
    User::LeaveIfError( iFileHandle.Duplicate( aFileHandle ) );
    
    
    BaseConstructL( iFileHandle );   
    User::LeaveIfError( CheckDRMRights() );
    
    TRect mediaRect = iMedia->GetRegion()->GetRectangle();
    
    iThumbnailBitmap = new ( ELeave ) CFbsBitmap;
    User::LeaveIfError( iThumbnailBitmap->Create( mediaRect.Size(), 
                                                  iCoeEnv->ScreenDevice()->DisplayMode() ) );
                                                  
    iThumbnailMask = new ( ELeave ) CFbsBitmap;
    User::LeaveIfError( iThumbnailMask->Create( mediaRect.Size(), 
                                                EGray256 ) );
	                                            
    GenerateThumbnailL();
    
    TParse fileParse;
    fileParse.Set( KUniUtilsBitmapFile, &KDC_APP_BITMAP_DIR, NULL );
    
    iIndicatorIcon = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(), 
                                                KAknsIIDQgnGrafMmsPres,
                                                fileParse.FullName(), 
                                                EMbmUniutilsQgn_graf_mms_pres, 
                                                EMbmUniutilsQgn_graf_mms_pres_mask );
    
    iIndicatorSize = RetrieveIconSize();
    User::LeaveIfError( AknIconUtils::SetSize( iIndicatorIcon->Bitmap(), iIndicatorSize ) );    

    static_cast<CSmilMedia*>( iMedia )->SetFocusable( ETrue );
    
#ifdef SVG_DEBUG  
    SMILUILOGGER_LEAVEFN( "[SMILUI] SVG: ConstructL" )
#endif
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSmilSVGRenderer* CSmilSVGRenderer::NewL( RFile& aFileHandle, 
                                          MSmilMedia* aMedia,
                                          MMediaFactoryFileInfo* aFileInfo,
                                          DRMCommon& aDrmCommon,
                                          CDRMHelper& aDrmHelper )
    {
    CSmilSVGRenderer* self = new( ELeave ) CSmilSVGRenderer( aMedia, 
                                                             aFileInfo, 
                                                             aDrmCommon, 
                                                             aDrmHelper );
    
    CleanupStack::PushL( self );
    self->ConstructL( aFileHandle );
    CleanupStack::Pop( self );

    return self;
    }

    
// -----------------------------------------------------------------------------
// CSmilSVGRenderer::~CSmilSVGRenderer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSmilSVGRenderer::~CSmilSVGRenderer()
    {
    iFileHandle.Close();
    
    delete iThumbnailBitmap;
    delete iThumbnailMask;
    delete iIndicatorIcon;
    
    delete iObserverUtil;
    
    iMedia = NULL; // For LINT
    iFileInfo = NULL; // For LINT
    }


// -----------------------------------------------------------------------------
// CSmilSVGRenderer::IsVisual
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSmilSVGRenderer::IsVisual() const
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::IntrinsicWidth
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSmilSVGRenderer::IntrinsicWidth() const
    {
    return iThumbnailBitmap->SizeInPixels().iWidth;
    }


// -----------------------------------------------------------------------------
// CSmilSVGRenderer::IntrinsicHeight
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSmilSVGRenderer::IntrinsicHeight() const
    {
    return iThumbnailBitmap->SizeInPixels().iHeight;
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::IntrinsicDuration
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TSmilTime CSmilSVGRenderer::IntrinsicDuration() const
    {
    return KSVGDefaultDuration;
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::PrepareMediaL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSmilSVGRenderer::PrepareMediaL()
    {
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::SeekMediaL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSmilSVGRenderer::SeekMediaL( const TSmilTime& /*aTime*/ )
    {
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::ShowMediaL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSmilSVGRenderer::ShowMediaL()
    {
    iVisible = ETrue;
    iMedia->Redraw();
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::HideMedia
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSmilSVGRenderer::HideMedia()
    {
    iVisible = EFalse;
    iMedia->Redraw();
    }
    
    // -----------------------------------------------------------------------------
// CSmilSVGRenderer::FreezeMedia
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSmilSVGRenderer::FreezeMedia()
    {
    }
    
// -----------------------------------------------------------------------------
// CSmilSVGRenderer::ResumeMedia
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSmilSVGRenderer::ResumeMedia()
    {
    }
    
// -----------------------------------------------------------------------------
// CSmilSVGRenderer::Draw
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSmilSVGRenderer::Draw( CGraphicsContext& aGc,
                             const TRect& /*aRect*/,
                             CSmilTransitionFilter* /*aTransitionFilter*/,
                             const MSmilFocus* aFocus )
    {
    if ( iVisible )
        {
        CBitmapContext* tmpBitmapContext = static_cast<CBitmapContext*>( &aGc );
        
        tmpBitmapContext->SetBrushStyle( CGraphicsContext::ENullBrush );
        
        tmpBitmapContext->BitBltMasked( iFocusRect.iTl, 
                                        iThumbnailBitmap,
                                        iThumbnailSize,
                                        iThumbnailMask,
                                        ETrue );
        

        TPoint indicatorPosition( iFocusRect.iTl.iX,
                                  iFocusRect.iBr.iY );
        
        indicatorPosition.iY -= iIndicatorSize.iHeight;
        
        tmpBitmapContext->BitBltMasked( indicatorPosition, 
                                        iIndicatorIcon->Bitmap(),
                                        iIndicatorSize,
                                        iIndicatorIcon->Mask(),
                                        ETrue );
            
        DrawFocus( aGc, aFocus );
        }
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::ActivateRendererL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CSmilSVGRenderer::ActivateRendererL()
    {                   
    CSVGTUIDialog* svgtDialog = CSVGTUIDialog::NewL();
    
    delete iObserverUtil;
    iObserverUtil = NULL;
    
    iObserverUtil = CSVGTAppObserverUtil::NewL( svgtDialog, &iFileHandle );
    
    svgtDialog->ExecuteLD( iFileHandle, this );
    }


// -----------------------------------------------------------------------------
// CSmilSVGRenderer::FetchImage
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSmilSVGRenderer::FetchImage( const TDesC& aUri, RFs& aSession, RFile& aFileHandle )
    {
    TRAPD( error, iFileInfo->GetFileHandleL( aUri, aFileHandle ) );
    
    if ( error != KErrNone )
        {
        error = iObserverUtil->FetchImage( aUri, aSession, aFileHandle );
        }
    
    return error;
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::LinkActivated
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSmilSVGRenderer::LinkActivated( const TDesC& aUri )
    {
    return iObserverUtil->LinkActivated( aUri );
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::LinkActivatedWithShow
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//      
TBool CSmilSVGRenderer::LinkActivatedWithShow( const TDesC& aUri, const TDesC& aShow )
    {
    return iObserverUtil->LinkActivatedWithShow( aUri, aShow );
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::DisplayDowloadMenuL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//                                                    
void CSmilSVGRenderer::DisplayDownloadMenuL()
    {
    iObserverUtil->DisplayDownloadMenuL();
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::GetSmilFitValue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//                                              
void CSmilSVGRenderer::GetSmilFitValue( TDes& aSmilValue )
    {
    iObserverUtil->GetSmilFitValue( aSmilValue );
    }
                                                     
// -----------------------------------------------------------------------------
// CSmilSVGRenderer::CanShowSave
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
TBool CSmilSVGRenderer::CanShowSave()
    {
    return iObserverUtil->CanShowSave();
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::IsSavingDone
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TBool CSmilSVGRenderer::IsSavingDone()
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::CanShowDownload
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TBool CSmilSVGRenderer::CanShowDownload() const
    {
    return iObserverUtil->CanShowDownload();
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::DoSaveL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CSmilSVGRenderer::DoSaveL( TInt aButtonid )
    {
    iObserverUtil->DoSaveL( aButtonid );
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::NewFetchImageData
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//                                       
TInt CSmilSVGRenderer::NewFetchImageData( const TDesC& aUri )
    {
    return iObserverUtil->NewFetchImageData( aUri );
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::AssignEmbededDataL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSmilSVGRenderer::AssignEmbededDataL( const TDesC& aUri )
    {
    iObserverUtil->AssignEmbededDataL( aUri );
    }


// -----------------------------------------------------------------------------
// CSmilSVGRenderer::UpdateScreen
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSmilSVGRenderer::UpdateScreen()
    {    
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::ScriptCall
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSmilSVGRenderer::ScriptCall( const TDesC& /*aScript*/, CSvgElementImpl* /*aCallerElement*/ )
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::FetchFont
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSmilSVGRenderer::FetchFont( const TDesC& aUri, RFs& /*aSession*/, RFile& aFileHandle )
    {
    TRAPD( error, iFileInfo->GetFileHandleL( aUri, aFileHandle ) );
    return error;
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::UpdatePresentation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSmilSVGRenderer::UpdatePresentation( const TInt32&  /*aNoOfAnimation*/ )
    {
    }


// -----------------------------------------------------------------------------
// CSmilSVGRenderer::UpdatePresentation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSmilSVGRenderer::GenerateThumbnailL()
    {
#ifdef SVG_DEBUG
    SMILUILOGGER_ENTERFN( "SVG, GenerateThumbnailL" );
#endif

    TFontSpec spec;
    CSvgEngineInterfaceImpl* svgEngine = CSvgEngineInterfaceImpl::NewL( iThumbnailBitmap, this, spec );
    CleanupStack::PushL( svgEngine );
    
    svgEngine->SetBackgroundColor( KRgbWhite.Value() );
    
    TInt domHandle( KNullHandle );
    MSvgError* result = svgEngine->PrepareDom( iFileHandle, domHandle );
    
    HandleSVGEngineErrorL( result );
        
    result = svgEngine->UseDom( domHandle, iThumbnailBitmap, iThumbnailMask );
    
    HandleSVGEngineErrorL( result );
        
    iThumbnailSize = svgEngine->Size();
    
    if ( iThumbnailSize.iHeight <= 0 &&
         iThumbnailSize.iWidth <= 0 )
        {
        // SVG engine might fail to set proper size so in that case
        // we use the original size.
        iThumbnailSize = svgEngine->GetUnscaledContentSize( domHandle );
        }
    
    if ( iThumbnailSize.iHeight > 0 &&
         iThumbnailSize.iWidth > 0 )
        {
        TRect mediaRect = iMedia->GetRegion()->GetRectangle();
          
        TReal widthRatio = static_cast<TReal>( mediaRect.Width() ) / 
                           static_cast<TReal>( iThumbnailSize.iWidth );
        TReal heightRatio = static_cast<TReal>( mediaRect.Height() ) / 
                            static_cast<TReal>( iThumbnailSize.iHeight );
        
        // Calculate "meet" ratio                    
        TReal scaleFactor = ( widthRatio < heightRatio ) ? widthRatio : heightRatio;
        
        iThumbnailSize.iHeight *= scaleFactor;
        iThumbnailSize.iWidth *= scaleFactor;
        
        svgEngine->SetSvgDimensionToFrameBuffer( iThumbnailSize.iWidth, iThumbnailSize.iHeight );
        
        // Chooses view box for SVG files where it has not bee set.
        svgEngine->ChooseViewBoxIfNotSet( domHandle );    
        svgEngine->InitializeEngine();
        
        // Render the content on consumer provided bitmap
        svgEngine->RenderFrame( NULL, 0 );
        
        iFocusRect = mediaRect;
        
        // Center the focus rect.
        iFocusRect.Shrink( ( mediaRect.Width() - iThumbnailSize.iWidth ) / 2, 
                           ( mediaRect.Height() - iThumbnailSize.iHeight ) / 2 );        
        }
    
    CleanupStack::PopAndDestroy( svgEngine );
    
#ifdef SVG_DEBUG
    SMILUILOGGER_LEAVEFN( "SVG: GenerateThumbnailL" );
    SMILUILOGGER_WRITEF( _L("") );
#endif
    }


// -----------------------------------------------------------------------------
// CSmilSVGRenderer::HandleSVGEngineErrorL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSmilSVGRenderer::HandleSVGEngineErrorL( MSvgError* aError ) const
    {
    if ( !aError )
        {
#ifdef SVG_DEBUG
        SMILUILOGGER_WRITEF( _L("[SMILUI] SVG: Engine error: no memory") );
#endif  
        User::Leave( KErrNoMemory ); 
        }
    else if ( aError->HasError() && !aError->IsWarning() )
        {
#ifdef SVG_DEBUG
        SMILUILOGGER_WRITEF( _L("[SMILUI] SVG: Engine error: %d"), aError->SystemErrorCode() );
#endif  
        User::Leave( aError->SystemErrorCode() ); 
        
        }
    }

// -----------------------------------------------------------------------------
// CSmilSVGRenderer::ExitWhenOrientationChange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSmilSVGRenderer::ExitWhenOrientationChange()
    {
        return ETrue;
    }
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
