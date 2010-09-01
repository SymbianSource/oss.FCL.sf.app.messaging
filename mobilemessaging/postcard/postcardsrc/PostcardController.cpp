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
* Description:  
*       Provides CPostcardController class methods
*
*/



// INCLUDE FILES
#include <s32file.h>                // for RFileReadStream
#include <data_caging_path_literals.hrh> 

#include <gulicon.h>                // CGulIcon

#include <AknIndicatorContainer.h>
#include <AknLayout.lag>            // AKN_LAYOUT_WINDOW_screen
#include <aknenv.h>                 // for EditingStateIndicator
#include <AknsConstants.h>           // For skinnable icons
#include <AknsBasicBackgroundControlContext.h> // For skin stuff, too
#include <AknsDrawUtils.h>
#include <applayout.cdl.h>                 // LAF
#include <aknlayoutscalable_apps.cdl.h>
#include <avkon.rsg>
#include <AknsUtils.h>              // For CreateIconL
#include <AknUtils.h>               // For AknLayoutUtils
#include <AknIconUtils.h>           // For AknIconUtils
#include <postcard.mbg>				// For Postcard icons

#include "PostcardController.h"
#include "PostcardLaf.h"
#include "PostcardPointerObserver.h"

// ---------------------------------------------------------
// CPostcardController::NewL( )
// ---------------------------------------------------------

CPostcardController* CPostcardController::NewL( CPostcardDocument& aDocument
#ifdef RD_SCALABLE_UI_V2
    , MPocaPointerEventObserver& aObserver
#endif
    )
    {
    CPostcardController* self = new ( ELeave ) CPostcardController( aDocument
#ifdef RD_SCALABLE_UI_V2
        , aObserver
#endif
        );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CPostcardController::ConstructL
// ---------------------------------------------------------
void CPostcardController::ConstructL( )
    {
    CreateWindowL( );
    LoadIconsL( );
    RefreshCoordinates( );

    TRect rectForSkin = PostcardLaf::RelativeMainPostcardPane( );

    iBgContext = CAknsBasicBackgroundControlContext::NewL( 
            KAknsIIDQsnBgAreaMainMessage, rectForSkin, EFalse );
            
    // We don't want to flash image side at startup.
    iFocusedItem = EPostcardNone;   
    }

// ---------------------------------------------------------
// CPostcardController::CPostcardController()
// ---------------------------------------------------------
CPostcardController::CPostcardController( CPostcardDocument& aDocument
#ifdef RD_SCALABLE_UI_V2
    , MPocaPointerEventObserver& aObserver
#endif
    ) :  
    iDocument( aDocument )
#ifdef RD_SCALABLE_UI_V2
    , iEventObserver( aObserver )
#endif
    {
#ifdef RD_SCALABLE_UI_V2
    iPenEnabled = AknLayoutUtils::PenEnabled();
#endif
    }

// ---------------------------------------------------------
// CPostcardController::~CPostcardController()
// ---------------------------------------------------------
CPostcardController::~CPostcardController( )
    {
    delete iFrontBg;
    delete iInsertImageBg;
    delete iImage;
    delete iInsertImage;
        
    delete iBgBg;
    delete iStamp;
    delete iEmptyGreetingFocused;
    delete iEmptyGreeting;
    delete iGreeting;
    delete iEmptyAddressFocused;
    delete iEmptyAddress;
    delete iAddress;
    
    if( iGreetingLines )
        {
        iGreetingLines->ResetAndDestroy( );
        delete iGreetingLines;    
        }
    if( iRecipientLines )
        {
        iRecipientLines->ResetAndDestroy( );
        delete iRecipientLines;
        }
    
    delete iUpperArrow;
    delete iLowerArrow;
    
    delete iBgContext;
    
    }

// ---------------------------------------------------------
// CPostcardController::ActivateL( )
// ---------------------------------------------------------
void CPostcardController::ActivateL( )
    {
    SetRect( PostcardLaf::MainPostcardPane( ) );
    SizeChanged();
    MakeVisible( ETrue );
    SetFocus( ETrue );
    CCoeControl::ActivateL();
    DrawNow( );    
    }

// ---------------------------------------------------------
// CPostcardController::CountComponentControls() const
// ---------------------------------------------------------
TInt CPostcardController::CountComponentControls( ) const
    {
    return 0;
    }

// ---------------------------------------------------------
// CPostcardController::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
CCoeControl* CPostcardController::ComponentControl( TInt /*aIndex*/ ) const
    {
    return NULL;
    }

// ---------------------------------------------------------
// CPostcardController::Draw(const TRect& aRect) const
// ---------------------------------------------------------
void CPostcardController::Draw( const TRect& /*aRect*/ ) const
    {
    // Lets draw the skin stuff + bg stuff first
    DrawBackground( );

    // Now we draw the real content
    switch ( iFocusedItem )
        {
        case EPostcardImage:
            DrawImageSide();
            break;

        case EPostcardText:
        case EPostcardRecipient:
            DrawTextSide(); 
            break;

        case EPostcardNone:
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CPostcardController::DrawBackground( ) const
// ---------------------------------------------------------
void CPostcardController::DrawBackground( ) const
    {
    CWindowGc& gc = SystemGc();
    TRect rect( PostcardLaf::RelativeMainPostcardPane( ) );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    
    if( !AknsDrawUtils::Background( skin, cc, this, gc, rect ) )
		{
	    // clear entire header area.
	    gc.SetBrushColor( AKN_LAF_COLOR( 0 ) );
		gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
		gc.SetPenStyle( CGraphicsContext::ENullPen );
		gc.DrawRect( rect );
		}

    gc.SetBrushStyle(CGraphicsContext::ENullBrush); // To ensure masking
    
    // Background image
    if( iFocusedItem == EPostcardImage )
        {
        DrawIcon( iImage?*iFrontBg:*iInsertImageBg,iFrontBgC);
        }
    else if ( iFocusedItem == EPostcardRecipient ||
              iFocusedItem == EPostcardText )
        {
        DrawIcon( *iBgBg, iBgBgC );
        }
                
    if ( iFocusedItem != EPostcardNone ) // Prevents unnecessary drawing of arrows at startup.
        {
        DrawScrollArrows();
        }
    }

// ---------------------------------------------------------
// CPostcardController::DrawImageSide( ) const
// ---------------------------------------------------------
void CPostcardController::DrawImageSide( ) const
    {
    // Draw focused image / insert icon
    if( iImage )
    	{ // Lets resize the image rect for the image
        TRect imageRect = iImageC;
    	TSize imageSize = iImage->Bitmap()->SizeInPixels();
		TSize rectSize = imageRect.Size( );

        // Image size matches the screen rectangle size unless
        // there has been a layout switch. In that case DrawBitmap()
        // is used to scale bitmap to a screen. Otherwise image is
        // drawn using BitBlt.
        TInt diffX = rectSize.iWidth - imageSize.iWidth;
        TInt diffY = rectSize.iHeight - imageSize.iHeight;
        const TInt KMaxDiff = 2;
        if( Abs( diffX ) > KMaxDiff && Abs( diffY ) > KMaxDiff )
            {
            const TInt KScaling = 1024;
            // Adjust screen rectangle so that it matches the image x/y ratio
            TInt newWidth = rectSize.iWidth * KScaling;
            TInt newHeight =
                ( imageSize.iHeight * newWidth ) / imageSize.iWidth;
            if ( newHeight > rectSize.iHeight * KScaling )
                {
                newHeight = rectSize.iHeight * KScaling;
                newWidth =
                    ( imageSize.iWidth * newHeight ) / imageSize.iHeight;
                }
            imageRect.Shrink( ( rectSize.iWidth - newWidth / KScaling ) / 2,
                ( rectSize.iHeight - newHeight / KScaling ) / 2 );
    	    TRect srcRect( iImage->Bitmap()->SizeInPixels() );
    	    SystemGc().DrawBitmap( imageRect, iImage->Bitmap(), srcRect );
            }
        else
            {
            TInt deltaX = diffX > 0 ? ( diffX + 1 ) / 2 : 0;
            TInt deltaY = diffY > 0 ? ( diffY + 1 ) / 2 : 0;
            imageRect.Shrink( deltaX, deltaY );
            DrawIcon( *iImage, imageRect );
            }
    	}
    else
        {
        DrawIcon( *iInsertImage, iInsertImageC );
        }
    }


// ---------------------------------------------------------
// CPostcardController::DrawTextSide( ) const
// ---------------------------------------------------------
void CPostcardController::DrawTextSide( ) const
    {
	// Draw stamp
	DrawIcon( *iStamp, iStampC );
	// Draw possible greeting and address bitmaps
	if( iGreeting )
	    {
	    DrawIcon( *iGreeting, iGreetingC );
	    }
	if( iAddress )
	    {
	    DrawIcon( *iAddress, iAddressC );
	    }
	// Draw focus lines
    DrawFocus( );
    }

// ---------------------------------------------------------
// CPostcardController::DrawIcon()
// ---------------------------------------------------------
void CPostcardController::DrawIcon( const CGulIcon& aIcon, const TRect& aRect ) const
    {
    if ( aIcon.Mask( ) )
        {
        SystemGc( ).BitBltMasked( aRect.iTl, aIcon.Bitmap( ), aRect.Size( ) , aIcon.Mask( ), EFalse ); // was EFalse
        }
    else
        {
        SystemGc( ).BitBlt( aRect.iTl, aIcon.Bitmap( ), aRect.Size( ) );
        }
    }

// ---------------------------------------------------------------------------
// CPostcardController::DrawScrollArrows()
// ---------------------------------------------------------------------------
void CPostcardController::DrawScrollArrows() const
    {
    // Scroll arrows. Behaviour depends on whether we are in editor or 
    // viewer mode.
    if ( iDocument.MessageType() != EPostcardSent )
        {
        DrawIcon( iFocusedItem != EPostcardImage? *iLowerArrow : *iUpperArrow, 
                  iFocusedItem != EPostcardImage? iLowerArrowC : iUpperArrowC );
        }
    else
        {
        DrawIcon( iFocusedItem != EPostcardImage? *iUpperArrow : *iLowerArrow, 
                  iFocusedItem != EPostcardImage? iUpperArrowC : iLowerArrowC );
        }
    }

// ---------------------------------------------------------
// CPostcardController::ReLoadIconsL()
// ---------------------------------------------------------
void CPostcardController::ReLoadIconsL( )
    {
    // Delete old ones, since new skin might have new icons.
    delete iFrontBg;
    iFrontBg = NULL;
    delete iInsertImageBg;
    iInsertImageBg = NULL;
    delete iInsertImage;
    iInsertImage = NULL;
    delete iBgBg;
    iBgBg = NULL;
    delete iStamp;
    iStamp = NULL;
    delete iEmptyGreetingFocused;
    iEmptyGreetingFocused = NULL;
    delete iEmptyGreeting;
    iEmptyGreeting = NULL;
    delete iEmptyAddressFocused;
    iEmptyAddressFocused = NULL;
    delete iEmptyAddress;
    iEmptyAddress = NULL;
    delete iUpperArrow;
    iUpperArrow = NULL;
    delete iLowerArrow;
    iLowerArrow = NULL;
    
    if ( iBgContext )
        {
        delete iBgContext;
        iBgContext = NULL;
        
        TRect rectForSkin = PostcardLaf::RelativeMainPostcardPane();
        
        iBgContext = CAknsBasicBackgroundControlContext::NewL( 
                KAknsIIDQsnBgAreaMainMessage, rectForSkin, EFalse );
        
        }
    
    // Load icons from active skin context.
    LoadIconsL();
    // Set size and position for the icons.
    RefreshCoordinates();
    // Draw again, since icons or background context might have changed.
    DrawDeferred();
    }

// ---------------------------------------------------------
// CPostcardController::LoadIconsL()
// ---------------------------------------------------------
void CPostcardController::LoadIconsL( )
    {

    TParse parseDrive;
    parseDrive.Set( iDocument.AppFullName( ), NULL, NULL );
    TPtrC drive = parseDrive.Drive( );

    TParse parse;
    parse.Set( KPostcardMifFile, &KDC_APP_BITMAP_DIR, NULL );

    TFileName fileName( parse.FullName( ) );
    fileName.Insert( 0, drive );

    iFrontBg = DoLoadIconL(
        KAknsIIDQgnGrafMmsPostcardFront,
        fileName, 
        EMbmPostcardQgn_graf_mms_postcard_front,
        EMbmPostcardQgn_graf_mms_postcard_front_mask
        );
    iInsertImageBg = DoLoadIconL(
        KAknsIIDQgnGrafMmsPostcardInsertImageBg,
        fileName, 
        EMbmPostcardQgn_graf_mms_postcard_insert_image_bg,
        EMbmPostcardQgn_graf_mms_postcard_insert_image_bg_mask
        );
    iInsertImage = DoLoadIconL(
        KAknsIIDQgnGrafMmsInsertImage,
        fileName, 
        EMbmPostcardQgn_graf_mms_insert_image,
        EMbmPostcardQgn_graf_mms_insert_image
        );

    iBgBg = DoLoadIconL(
        KAknsIIDQgnGrafMmsPostcardBack,
        fileName, 
		EMbmPostcardQgn_graf_mms_postcard_back,
		EMbmPostcardQgn_graf_mms_postcard_back_mask
		);
    iStamp = DoLoadIconL(
        KAknsIIDQgnIndiMmsPostcardStamp,
        fileName, 
		EMbmPostcardQgn_indi_mms_postcard_stamp,
		EMbmPostcardQgn_indi_mms_postcard_stamp_mask
        );
    iEmptyGreetingFocused = DoLoadIconL(
        KAknsIIDQgnPropMmsPostcardGreetingActive,
        fileName, 
		EMbmPostcardQgn_prop_mms_postcard_greeting_active,
		EMbmPostcardQgn_prop_mms_postcard_greeting_active_mask
        );
    iEmptyGreeting = DoLoadIconL(
        KAknsIIDQgnPropMmsPostcardGreetingInactive,
        fileName, 
        EMbmPostcardQgn_prop_mms_postcard_greeting_inactive,
        EMbmPostcardQgn_prop_mms_postcard_greeting_inactive_mask
        );
    iEmptyAddressFocused = DoLoadIconL(
        KAknsIIDQgnPropMmsPostcardAddressActive,
        fileName, 
        EMbmPostcardQgn_prop_mms_postcard_address_active,
        EMbmPostcardQgn_prop_mms_postcard_address_active_mask
        );
    iEmptyAddress = DoLoadIconL(
        KAknsIIDQgnPropMmsPostcardAddressInactive,
        fileName, 
        EMbmPostcardQgn_prop_mms_postcard_address_inactive,
        EMbmPostcardQgn_prop_mms_postcard_address_inactive_mask
        );

    iUpperArrow = DoLoadIconL(
        KAknsIIDQgnIndiMmsPostcardUp,
        fileName, 
        EMbmPostcardQgn_indi_mms_postcard_up,
        EMbmPostcardQgn_indi_mms_postcard_up_mask
        );
    iLowerArrow = DoLoadIconL(
        KAknsIIDQgnIndiMmsPostcardDown,
        fileName, 
        EMbmPostcardQgn_indi_mms_postcard_down,
        EMbmPostcardQgn_indi_mms_postcard_down_mask
        );
    // AppUi fills the other member icon variables
    }

// ---------------------------------------------------------
// CPostcardController::DoLoadIconLC()
// ---------------------------------------------------------
//
CGulIcon* CPostcardController::DoLoadIconL( 
                                const TAknsItemID& aId,
                                const TDesC& aFileName,
                                const TInt aFileBitmapId,
                                const TInt aFileMaskId  )
    {
    CGulIcon* icon = NULL;
    icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(),
        aId,
        aFileName,
        aFileBitmapId,
        aFileMaskId
        );

    return icon;
    }

// ---------------------------------------------------------
// CPostcardController::RefreshCoordinates
// ---------------------------------------------------------
void CPostcardController::RefreshCoordinates( )
    {
    // Read the new coordinates from LAF and sets the sizes
    SetRect( PostcardLaf::MainPostcardPane( ) );    

    iFrontBgC       = PostcardLaf::FrontBackground( );
    AknIconUtils::SetSize( iFrontBg->Bitmap(), iFrontBgC.Size( ), EAspectRatioNotPreserved );
    AknIconUtils::SetSize( iInsertImageBg->Bitmap( ), iFrontBgC.Size( ), EAspectRatioNotPreserved );
    iInsertImageC   = PostcardLaf::InsertImageIcon( );
    AknIconUtils::SetSize( iInsertImage->Bitmap(), iInsertImageC.Size( ), EAspectRatioNotPreserved );
    iInsertImageBgC   = PostcardLaf::FrontBackgroundWithoutImage( );
    iImageC         = PostcardLaf::Image( );
    
    iBgBgC          = PostcardLaf::BackBackground( );
    AknIconUtils::SetSize( iBgBg->Bitmap(), iBgBgC.Size( ), EAspectRatioNotPreserved );
    iStampC         = PostcardLaf::Stamp( );
    AknIconUtils::SetSize( iStamp->Bitmap(), iStampC.Size( ), EAspectRatioNotPreserved );
    iGreetingC      = PostcardLaf::GreetingText( );
    AknIconUtils::SetSize( iEmptyGreeting->Bitmap(), iGreetingC.Size( ), EAspectRatioNotPreserved );
    AknIconUtils::SetSize( iEmptyGreetingFocused->Bitmap(), iGreetingC.Size( ), EAspectRatioNotPreserved );
    iAddressC       = PostcardLaf::Address( );
    AknIconUtils::SetSize( iEmptyAddress->Bitmap(), iAddressC.Size( ), EAspectRatioNotPreserved );
    AknIconUtils::SetSize( iEmptyAddressFocused->Bitmap(), iAddressC.Size( ), EAspectRatioNotPreserved );

    iUpperArrowC    = PostcardLaf::UpperArrow( );
    AknIconUtils::SetSize( iUpperArrow->Bitmap(), iUpperArrowC.Size( ), EAspectRatioNotPreserved );
    iLowerArrowC    = PostcardLaf::LowerArrow( );
    AknIconUtils::SetSize( iLowerArrow->Bitmap(), iLowerArrowC.Size( ), EAspectRatioNotPreserved );

    if ( iBgContext )
        {
        iBgContext->SetRect( PostcardLaf::RelativeMainPostcardPane( ) );
        }
    }

// ---------------------------------------------------------
// CPostcardController::Frontpage
// ---------------------------------------------------------
TBool CPostcardController::Frontpage( )
    {
    return iFocusedItem==EPostcardImage?ETrue:EFalse;
    }

// ---------------------------------------------------------
// CPostcardController::IsFocused( )
// ---------------------------------------------------------
TBool CPostcardController::IsFocused( TPostcardPart& aPart )
    {
    return iFocusedItem==aPart?ETrue:EFalse;
    }

// ---------------------------------------------------------
// CPostcardController::SetFocused( )
// ---------------------------------------------------------
void CPostcardController::SetFocused( TPostcardPart aPart )
    {
    iFocusedItem = aPart;
    }

// ---------------------------------------------------------
// CPostcardController::Focused( )
// ---------------------------------------------------------
TPostcardPart CPostcardController::Focused( )
	{
	return iFocusedItem;
	}

// ---------------------------------------------------------
// CPostcardController::SetBitmap( )
// ---------------------------------------------------------
void CPostcardController::SetBitmap( TPostcardPart aPart, CGulIcon* aIcon )
    {
    switch( aPart )
        {
        case EPostcardImage:
            {
            delete iImage;
            iImage = aIcon;
            break;
            }
        case EPostcardText:
            {
            delete iGreeting;
            iGreeting = aIcon;
            break;
            }
        case EPostcardRecipient:
            {
            delete iAddress;
            iAddress = aIcon;
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CPostcardController::RemoveBitmap( )
// ---------------------------------------------------------
void CPostcardController::RemoveBitmap( TPostcardPart aPart )
    {
    switch( aPart )
        {
        case EPostcardImage:
            {
            delete iImage;
            iImage = NULL;
            break;
            }
        case EPostcardText:
            {
            delete iGreeting;
            iGreeting = NULL;
            break;
            }
        case EPostcardRecipient:
            {
            delete iAddress;
            iAddress = NULL;
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CPostcardController::Move( )
// ---------------------------------------------------------
void CPostcardController::Move( TInt aScanCode )
    {
    TBool mirrored = AknLayoutUtils::LayoutMirrored();
    switch ( aScanCode )
        {
        case EStdKeyLeftArrow:
            MoveHorizontally( mirrored?ETrue:EFalse );
            break;
        case EStdKeyRightArrow:
            MoveHorizontally( mirrored?EFalse:ETrue ); 
            break;
        case EStdKeyUpArrow:    
            MoveVertically( ETrue );
            break;
        case EStdKeyDownArrow:  
            MoveVertically( EFalse );
            break;
        default: // no operation
            break;
        }    
    }

// ---------------------------------------------------------
// CPostcardController::MoveHorizontally( )
// ---------------------------------------------------------
void CPostcardController::MoveHorizontally( TBool aRight )
    {
    if( Focused( ) == EPostcardImage )
        {
        return; // can not move horizontally in the image side
        }
    if( Focused( ) == EPostcardText && aRight )
        {
        SetFocused( EPostcardRecipient );
        DrawDeferred( );
        }
    else if( Focused( ) == EPostcardRecipient && !aRight )
        {
        SetFocused( EPostcardText );
        DrawDeferred( );
        }
    }

// ---------------------------------------------------------
// CPostcardController::MoveVertically( )
// ---------------------------------------------------------
void CPostcardController::MoveVertically( TBool aUp )
    {
    // Behaviour depends on whether we are in editor or 
    // viewer mode. 
    if ( iDocument.MessageType() != EPostcardSent )
        {
        if( Focused( ) == EPostcardImage && aUp )
            {
            SetFocused( EPostcardRecipient );
            DrawDeferred( );
            }
        else if( Focused( ) != EPostcardImage && !aUp )
            {
            SetFocused( EPostcardImage );
            DrawDeferred( );
            }
        }
    else
        {
        if( Focused( ) == EPostcardImage && !aUp )
            {
            SetFocused( EPostcardRecipient );
            DrawDeferred( );
            }
        else if( Focused( ) != EPostcardImage && aUp )
            {
            SetFocused( EPostcardImage );
            DrawDeferred( );
            }
        }
    }

// ---------------------------------------------------------
// CPostcardController::MopSupplyObject
//
// ---------------------------------------------------------
//
TTypeUid::Ptr CPostcardController::MopSupplyObject( TTypeUid aId )
    {   
    if ( aId.iUid == MAknsControlContext::ETypeId && iBgContext)
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }

    return CCoeControl::MopSupplyObject( aId );
    }

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------
// CPostcardController::HandlePointerEventL
//
// ---------------------------------------------------------
//
void CPostcardController::HandlePointerEventL(
    const TPointerEvent& aPointerEvent )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        struct TPocaHitTarget
            {
            const TRect CPostcardController::* iRect; // pointer to a member
            TPocaPointerEvent iEvent;
            TInt iTapType; // tap types the target is interested in
            };

        TPocaHitTarget backArrow;
        TPocaHitTarget frontArrow;
        
        
        // Construct image and text side hit target arrays. Notice that
        // editor and viewer handles navigation arrow differently.
        if ( iDocument.MessageType() == EPostcardSent )
            {
            // In viewer, image side has arrow down.
            backArrow.iRect = &CPostcardController::iUpperArrowC;
            backArrow.iEvent = EPocaPointerEventUpIcon;
            backArrow.iTapType = EPocaPointerEventTypeSingleDown;
            
            frontArrow.iRect = &CPostcardController::iLowerArrowC;
            frontArrow.iEvent = EPocaPointerEventDownIcon;
            frontArrow.iTapType = EPocaPointerEventTypeSingleDown;
            }
        else
            {
            // In editor, image side has arrow up.
            backArrow.iRect = &CPostcardController::iLowerArrowC;
            backArrow.iEvent = EPocaPointerEventDownIcon;
            backArrow.iTapType = EPocaPointerEventTypeSingleDown;

            frontArrow.iRect = &CPostcardController::iUpperArrowC;
            frontArrow.iEvent = EPocaPointerEventUpIcon;
            frontArrow.iTapType = EPocaPointerEventTypeSingleDown;
            }
        
        // Front (image) side hit test targets
        const TPocaHitTarget frontTargets[] =
            {
            frontArrow,
            {&CPostcardController::iFrontBgC, EPocaPointerEventFrontBg,
                EPocaPointerEventTypeSingleUp },
            {NULL} // end marker
            };
        // Back (address) side hit test targets
        const TPocaHitTarget backTargets[] =
            {
            backArrow,
            {&CPostcardController::iGreetingC, EPocaPointerEventGreeting,
                EPocaPointerEventTypeSingleDown },
            {&CPostcardController::iAddressC, EPocaPointerEventAddress,
                EPocaPointerEventTypeSingleDown },
            {NULL} // end marker
            };

        TPocaPointerEventType tapType = EPocaPointerEventTypeNone;
        // Check it was button1
        if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
            {
            tapType = EPocaPointerEventTypeSingleDown;
            }
        else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
            {
            tapType = EPocaPointerEventTypeSingleUp;
            }

        if ( tapType == EPocaPointerEventTypeNone )
            {
            return;
            }
        
        // Do hit testing on targets
        TBool hit = EFalse;
        const TPocaHitTarget* target = iFocusedItem == EPostcardImage ?
            frontTargets : backTargets;
        for( ; target->iRect && !hit; target++ )
            {
            hit = (this->*target->iRect).Contains( aPointerEvent.iPosition );

            if ( hit )
                {
                // There was a pointer event hitting one of our targets
                // Check that we have a valid tap and relay event to
                // observer.

                if ( target->iTapType & tapType)
                    {
                    iEventObserver.PocaPointerEventL( target->iEvent, tapType );
                    }
                }
            }
        }
    }
#endif // RD_SCALABLE_UI_V2

// ---------------------------------------------------------
// CPostcardController::DrawFocus
//
// ---------------------------------------------------------
//
void CPostcardController::DrawFocus( ) const
    {
    CWindowGc& gc = SystemGc();
	gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	gc.SetPenStyle( CGraphicsContext::ENullPen );

    if( iFocusedItem == EPostcardText )
        {
        gc.SetBrushColor( AKN_LAF_COLOR(210) );                        
        }
    else
        {
        gc.SetBrushColor( AKN_LAF_COLOR(86) );            
        }
    for ( TInt i = 0; i < iGreetingLines->Count( ); i++ )
        {
        TRect rect = *( iGreetingLines->At( i ) );
        if( iFocusedItem == EPostcardText ) 
            {
            rect.iBr.iY += 1; // Increase the height by one when focused
            }
        // 1-pixel-high rects are not drawn so in that case use DrawLine
        if( rect.iBr.iY > rect.iTl.iY )
            {
            gc.DrawRect( rect );
            }
        else
            {
            gc.DrawRect( rect );
            }
        }
    if( iFocusedItem == EPostcardRecipient )
        {
        gc.SetBrushColor( AKN_LAF_COLOR(210) );
        }
    else
        {
        gc.SetBrushColor( AKN_LAF_COLOR(86) );            
        }
    for ( TInt i = 0; i < iRecipientLines->Count( ); i++ )
        {
        TRect rect = *( iRecipientLines->At( i ) );
        if( iFocusedItem == EPostcardRecipient ) 
            {
            rect.iBr.iY += 1; // Increase the height by one when focused
            }
        // 1-pixel-high rects are not drawn so in that case use DrawLine
        if( rect.iBr.iY > rect.iTl.iY )
            {
            gc.DrawRect( rect );
            }
        else
            {
            gc.DrawRect( rect );
            }
        }
    }

// ---------------------------------------------------------
// CPostcardController::SetGreetingLines
//
// ---------------------------------------------------------
void CPostcardController::SetGreetingLines( CArrayPtrFlat<TRect>& aGreetingLines )
    {
    if( iGreetingLines )
        {
        iGreetingLines->ResetAndDestroy( );
        delete iGreetingLines;
        iGreetingLines = NULL;        
        }
    iGreetingLines = &aGreetingLines;
    }
        
// ---------------------------------------------------------
// CPostcardController::SetRecipientLines
//
// ---------------------------------------------------------
void CPostcardController::SetRecipientLines( CArrayPtrFlat<TRect>& aRecipientLines )
    {
    if( iRecipientLines )
        {
        iRecipientLines->ResetAndDestroy( );
        delete iRecipientLines;
        iRecipientLines = NULL;
        }
    iRecipientLines = &aRecipientLines;
    }

// End of File  
