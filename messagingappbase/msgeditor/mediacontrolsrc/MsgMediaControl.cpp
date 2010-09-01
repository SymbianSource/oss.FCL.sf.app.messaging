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
* Description:  
*       Message Editor Base media control.
*
*/



// INCLUDE FILES
#include "MsgMediaControl.h"

#include <avkon.hrh>
#include <AknsConstants.h>                  // KAkn*
#include <data_caging_path_literals.hrh>    // KDC_APP_BITMAP_DIR
#include <AknUtils.h>                       // AknsUtils
#include <AknsDrawUtils.h>                  // AknsDrawUtils
#include <AknBitmapAnimation.h>

#include <MsgEditorCommon.h>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif 

#include "MsgIconControl.h"
#include "MsgFrameControl.h"

// ==========================================================

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const TInt KObserverArrayGranularity = 2;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgMediaControl::CMsgMediaControl
//
// Constructor.
// ---------------------------------------------------------
//
CMsgMediaControl::CMsgMediaControl( MMsgBaseControlObserver& aBaseControlObserver,
                                    TMsgControlId aMediaControlId,
                                    TMsgControlType aMediaControlType )
    :
    CMsgBaseControl( aBaseControlObserver ),
    iState( EMsgAsyncControlStateIdle )
    {
    iBaseLine = MsgEditorCommons::MsgBaseLineDelta();
    iUniqueContentId = ContentAccess::KDefaultContentObject();
    
    iControlId = aMediaControlId;
    iControlType = aMediaControlType;
    }

// ---------------------------------------------------------
// CMsgMediaControl::BaseConstructL
//
// Should be called from derived class
// ---------------------------------------------------------
//
void CMsgMediaControl::BaseConstructL( const CCoeControl& aParent, MMsgAsyncControlObserver* aObserver )
    {
    SetContainerWindowL( aParent );
    
    iObservers = new( ELeave ) CArrayPtrFlat<MMsgAsyncControlObserver>( KObserverArrayGranularity );
    
#ifdef RD_SCALABLE_UI_V2
    User::LeaveIfError( SetHitTest( this ) );
#endif // RD_SCALABLE_UI_V2
    
    if ( aObserver )
        {
        iObservers->AppendL( aObserver );
        }
    
    iIconControl = CMsgIconControl::NewL( *this );
    iFrame = CMsgFrameControl::NewL( *this );
    
    iControlModeFlags |= EMsgControlModeForceFocusStop;
    }

// ---------------------------------------------------------
// CMsgMediaControl::~CMsgMediaControl
//
// Destructor
// ---------------------------------------------------------
//
CMsgMediaControl::~CMsgMediaControl()
    {
    delete iObservers;
    delete iIconControl;
    delete iFrame;
    delete iAnimation;
    }

// ---------------------------------------------------------
// CMsgMediaControl::State
// ---------------------------------------------------------
//
TMsgAsyncControlState CMsgMediaControl::State() const
    {
    return iState;
    }

// ---------------------------------------------------------
// CMsgMediaControl::Error
// ---------------------------------------------------------
//
TInt CMsgMediaControl::Error() const
    {
    return iError;
    }

// ---------------------------------------------------------
// CMsgMediaControl::AddObserverL
// ---------------------------------------------------------
//
void CMsgMediaControl::AddObserverL( MMsgAsyncControlObserver& aObserver )
    {
    iObservers->AppendL( &aObserver );
    }

// ---------------------------------------------------------
// CMsgMediaControl::RemoveObserver
// ---------------------------------------------------------
//        
void CMsgMediaControl::RemoveObserver( MMsgAsyncControlObserver& aObserver )
    {
    for ( TInt current = 0; current < iObservers->Count(); current++ )
        {
        if ( iObservers->At( current ) == &aObserver )
            {
            iObservers->Delete( current );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMsgMediaControl::FocusChanged
//
// Should be called from derived class
// ---------------------------------------------------------
//
void CMsgMediaControl::FocusChanged( TDrawNow aDrawNow )
    {   
    DoUpdateFocus( aDrawNow );
    }

// ---------------------------------------------------------
// CMsgMediaControl::CurrentLineRect
//
// Returns the current control rect
// ---------------------------------------------------------
//
TRect CMsgMediaControl::CurrentLineRect()
    {
    return Rect();
    }

// ---------------------------------------------------------
// CMsgMediaControl::ClipboardL
//
// Handles clipboard operation.
// ---------------------------------------------------------
//
void CMsgMediaControl::ClipboardL(TMsgClipboardFunc /*aFunc*/ )
    {
    User::Leave( KErrNotSupported ); // Media controls does not support clipboard by default
    }

// ---------------------------------------------------------
// CMsgMediaControl::EditL
//
// Handles editing operation.
// ---------------------------------------------------------
//
void CMsgMediaControl::EditL( TMsgEditFunc /*aFunc*/ )
    {
    User::Leave( KErrNotSupported ); // Media control is not editable by default
    }

// ---------------------------------------------------------
// CMsgMediaControl::IsFocusChangePossible
//
// Checks if the focus change up or down is possible and returns ETrue if it is.
// ---------------------------------------------------------
//
TBool CMsgMediaControl::IsFocusChangePossible( TMsgFocusDirection /*aDirection*/ ) const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// CMsgMediaControl::IsCursorLocation
//
// Checks if the cursor location is on the topmost or downmost position and
// returns ETrue if it is.
// ---------------------------------------------------------
//
TBool CMsgMediaControl::IsCursorLocation( TMsgCursorLocation /*aLocation*/ ) const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// CMsgMediaControl::EditPermission
//
// Returns edit permission flags.
// ---------------------------------------------------------
//
TUint32 CMsgMediaControl::EditPermission() const
    {
    return EMsgEditNone;
    }

// ---------------------------------------------------------
// CMsgMediaControl::PrepareForReadOnly
//
// Prepares for read only or non read only state.
// ---------------------------------------------------------
//
void CMsgMediaControl::PrepareForReadOnly( TBool /*aReadOnly*/ )
    {
    }

// ---------------------------------------------------------
// CMmsImageControl::CountComponentControls
//
// Return count of controls be included in this component.
// ---------------------------------------------------------
//
TInt CMsgMediaControl::CountComponentControls() const
    {
    TInt result( 2 );
    if ( iAnimation )
        {
        result++;
        }
    return result;
    }

// ---------------------------------------------------------
// CMmsImageControl::ComponentControl
//
// Return pointer to component in question.
// Note! Image and Icon cannot exist at a same time.
// ---------------------------------------------------------
//
CCoeControl* CMsgMediaControl::ComponentControl( TInt aIndex ) const
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
            result = iAnimation;
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
// CMsgMediaControl::LoadIconL
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgMediaControl::LoadIconL( const TDesC& aFileName,                                   
                                         const TAknsItemID& aId,
                                         const TInt aFileBitmapId,
                                         const TInt aFileMaskId )
    {    
    if ( aFileBitmapId != KErrNotFound )
        {
        iIconControl->LoadIconL( aId,
                                 aFileName,
                                 aFileBitmapId,
                                 aFileMaskId );
        return ETrue;
        }    
    return EFalse;
    }

// ---------------------------------------------------------
// CMsgMediaControl::SetIconSizeL
// ---------------------------------------------------------
//
EXPORT_C void CMsgMediaControl::SetIconSizeL( const TSize& aNewSize )
    {
    if ( aNewSize != iIconControl->Size() )
        {
        iIconControl->SetBitmapSizeL( aNewSize );
        iFrame->SetImageSize( aNewSize );
        
        SetComponentControlExtents();
        }
    }

// ---------------------------------------------------------
// CMsgMediaControl::SetIconVisible
// ---------------------------------------------------------
//
EXPORT_C void CMsgMediaControl::SetIconVisible( TBool aVisibility )
    {
    iIconControl->MakeVisible( aVisibility );
    
    if ( iAnimation )
        {
        iAnimation->MakeVisible( EFalse );
        iAnimation->CancelAnimation();
        }
    
    DoUpdateFocus( EDrawNow );
    }

// ---------------------------------------------------------
// CMsgMediaControl::SetAnimationL
// ---------------------------------------------------------
//
EXPORT_C void CMsgMediaControl::SetAnimationL( CAknBitmapAnimation* aAnimationControl )
    {   
    ReleaseAnimation();
    
    iAnimation = aAnimationControl;
    
    if ( iAnimation )
        {
        iAnimation->SetContainerWindowL( *this );
        iAnimation->MakeVisible( EFalse );
        iAnimation->ActivateL();
        }
    }

// ---------------------------------------------------------
// CMsgMediaControl::SetAnimationSizeL
// ---------------------------------------------------------
//
EXPORT_C void CMsgMediaControl::SetAnimationSizeL( const TSize& aNewSize )
    {
    if ( iAnimation &&
         aNewSize != iAnimation->Size() )
        {
        iFrame->SetImageSize( aNewSize );
        
        TPoint center = Rect().Center();
        iAnimation->SetExtent( TPoint( center.iX - aNewSize.iWidth / 2,
                                       center.iY - aNewSize.iHeight / 2 ),
                               aNewSize );
        SetAnimationBackGroundFrameL();
        }
    }

// ---------------------------------------------------------
// CMsgMediaControl::SetAnimationVisibleL
// ---------------------------------------------------------
//
EXPORT_C void CMsgMediaControl::SetAnimationVisibleL( TBool aVisibility )
    {
    if ( iAnimation )
        {
        iIconControl->MakeVisible( !aVisibility );
        iAnimation->MakeVisible( aVisibility );
        
        if ( aVisibility && DrawableWindow() )
            {
            // Rewind to beginning. Note! Cannot start animation until window is set.
            iAnimation->SetFrameIndexL( 0 );
            
            // Set the correct animation frame background
            SetAnimationBackGroundFrameL();
            }
        else
            {
            iAnimation->CancelAnimation();
            }
        }
    }

// ---------------------------------------------------------
// CMsgMediaControl::IconBitmapId
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgMediaControl::IconBitmapId() const
    {    
    return iIconControl->IconBitmapId();
    }

// ---------------------------------------------------------
// CMsgMediaControl::ReleaseAnimation
// ---------------------------------------------------------
//
EXPORT_C void CMsgMediaControl::ReleaseAnimation()
    {
    if(iAnimation)
        {
        iAnimation->MakeVisible( EFalse );
        delete iAnimation;    
        iAnimation = NULL;
        }        
    }

// ---------------------------------------------------------
// CMsgMediaControl::SizeChanged
// ---------------------------------------------------------
//
void CMsgMediaControl::SizeChanged()
    {
    SetComponentControlExtents();
    
    iFrame->SetRect( Rect() );
    }

// ---------------------------------------------------------
// CMsgMediaControl::Draw
//
// Draws basic skin background to the given area.
// ---------------------------------------------------------
//
void CMsgMediaControl::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    
    if ( !AknsDrawUtils::Background( AknsUtils::SkinInstance(), 
                                     AknsDrawUtils::ControlContext( this ), 
                                     this, 
                                     gc, 
                                     aRect ) )
        {
        gc.SetBrushColor( AKN_LAF_COLOR( 0 ) );
        gc.SetPenStyle( CGraphicsContext::ENullPen );
		gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    	gc.DrawRect( aRect );
        }
    }

// ---------------------------------------------------------
// CMsgMediaControl::HandleResourceChange
// ---------------------------------------------------------
//
void CMsgMediaControl::HandleResourceChange( TInt aType )
    {
    for ( TInt current = 0; current < iObservers->Count(); current++ )
        {
        iObservers->At( current )->MsgAsyncControlResourceChanged( *this, aType );
        }
    
    CMsgBaseControl::HandleResourceChange( aType ); 
    
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        iBaseLine = MsgEditorCommons::MsgBaseLineDelta();
        }
    }
    
// ---------------------------------------------------------
// CMsgMediaControl::HandlePointerEventL
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CMsgMediaControl::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {                    
#ifdef RD_TACTILE_FEEDBACK 
    if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        {     
        MTouchFeedback* feedback = MTouchFeedback::Instance();
        if ( feedback )
            {
            feedback->InstantFeedback( this, ETouchFeedbackBasic );
            }                   
        }
#endif 
    }
#else
void CMsgMediaControl::HandlePointerEventL( const TPointerEvent& /*aPointerEvent*/ )
    {
    }
#endif // RD_SCALABLE_UI_V2

// ---------------------------------------------------------
// CMsgMediaControl::NotifyViewEvent
// ---------------------------------------------------------
//
void CMsgMediaControl::NotifyViewEvent( TMsgViewEvent aEvent, TInt aParam)
    {   
    CMsgBaseControl::NotifyViewEvent( aEvent, aParam );
    }

// ---------------------------------------------------------
// CMsgMediaControl::DoUpdateFocus
//
// Updates the focus (i.e. set frame control visible/invisible).
// Performs immediate redrawing of frame control if that 
// is requested and frame control state is changed. Otherwise
// normal deferred drawing is done. This is activated at 
// CCoeControl::MakeVisible if needed.
// ---------------------------------------------------------
//
void CMsgMediaControl::DoUpdateFocus( TDrawNow aDrawNow )
    {       
    TBool oldVisibility( iFrame->IsVisible() );
    
    if ( IsFocused() && VisiblePlaceholder() )
        {
        iFrame->MakeVisible( ETrue );
        }
    else 
        {
        iFrame->MakeVisible( EFalse );
        }
        
    if ( aDrawNow == EDrawNow &&
         iFrame->IsVisible() != oldVisibility )
        {
        DrawNow();
        }
    }

// ---------------------------------------------------------
// CMsgMediaControl::HitRegionContains
// 
// Evaluates whether control was "hit" by the pointer event.
// Basic media control is hit only when pointer event is received
// for icon area.
// 
// Note! CONE will do final verification if control is really hit so
//       not need to do it here.
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
TBool CMsgMediaControl::HitRegionContains( const TPoint& aPoint, 
                                           const CCoeControl& /*aControl*/ ) const
    {
    TBool result( EFalse );
    
    CCoeControl* visiblePlaceholder = VisiblePlaceholder();
    
    if ( visiblePlaceholder )
        {
        result = visiblePlaceholder->Rect().Contains( aPoint );
        }
        
    return result;
    }
#else
TBool CMsgMediaControl::HitRegionContains( const TPoint& /*aPoint*/, 
                                           const CCoeControl& /*aControl*/ ) const
    {
    return EFalse;
    }
#endif // RD_SCALABLE_UI_V2

// ---------------------------------------------------------
// CMsgMediaControl::SetComponentControlExtents
//
// Updates component controls position & size. 
// Icon & animation controls are centered to the control area.
// ---------------------------------------------------------
//
void CMsgMediaControl::SetComponentControlExtents()
    {
    //Center of the rect where we're drawing
    TPoint center = Rect().Center();
    
    //Size of the icon
    TSize size = iIconControl->BitmapSize();
    
    //Centered top left corner of the whole icon
    TPoint tlc = TPoint( center.iX - size.iWidth / 2,
                         center.iY - size.iHeight / 2 );
    
    iIconControl->SetExtent( tlc, size );
    
    if ( iAnimation )
        {
        TPoint newPosition( center.iX - iAnimation->Size().iWidth / 2,
                            center.iY - iAnimation->Size().iHeight / 2 );
        
        if ( newPosition != iAnimation->Position() )
            {
            iAnimation->SetPosition( newPosition );
                                   
            TRAP_IGNORE( SetAnimationBackGroundFrameL() );
            }
        }
    }

// ---------------------------------------------------------
// CMsgMediaControl::SetAnimationBackGroundFrameL
//
// Sets background bitmap for animation frame. This needs to be
// updated to correct skin background depending current control position.
// ---------------------------------------------------------
//
void CMsgMediaControl::SetAnimationBackGroundFrameL()
    {
    if ( AnimationVisible() )
        {
        const TDisplayMode displayMode( iCoeEnv->ScreenDevice()->DisplayMode() );
        
        CFbsBitmap* bitmap = new( ELeave ) CFbsBitmap;
        CleanupStack::PushL( bitmap );
        
        User::LeaveIfError( bitmap->Create( iAnimation->Size(), displayMode ) );
        
        CFbsBitmapDevice* doubleBufferDev = CFbsBitmapDevice::NewL( bitmap );
        CleanupStack::PushL( doubleBufferDev );    
        
        CFbsBitGc* doubleBufferGc = NULL;
        User::LeaveIfError( doubleBufferDev->CreateContext( doubleBufferGc ) );    
        CleanupStack::PushL( doubleBufferGc );    
        
        if ( AknsDrawUtils::DrawBackground( AknsUtils::SkinInstance(), 
                                            AknsDrawUtils::ControlContext( this ), 
                                            this, 
                                            *doubleBufferGc,
                                            TPoint(), 
                                            iAnimation->Rect(), 
                                            KAknsDrawParamDefault ) )
            {
            CleanupStack::PopAndDestroy( 2 ); // doubleBufferGc, doubleBufferDev
            
            CBitmapFrameData* data = CBitmapFrameData::NewL();
            data->SetBitmapsOwnedExternally( EFalse );
            data->SetBitmap( bitmap );
        
            // Set background frame to animation
            iAnimation->BitmapAnimData()->SetBackgroundFrame( data ); // gets ownership
            CleanupStack::Pop( bitmap );
            
            // Make the change effective
            iAnimation->StartAnimationL();
            }
        else
            {
            CleanupStack::PopAndDestroy( 3 ); // doubleBufferGc, doubleBufferDev, bitmap
            }
        }                
    
    }

// ---------------------------------------------------------
// CMsgMediaControl::AnimationVisible
// ---------------------------------------------------------
//
TBool CMsgMediaControl::AnimationVisible() const
    {
    TBool result( EFalse );
    if ( iAnimation && iAnimation->IsVisible() )
        {
        result = ETrue;
        }
        
    return result;
    }

// ---------------------------------------------------------
// CMsgMediaControl::VisiblePlaceholder
// ---------------------------------------------------------
//
CCoeControl* CMsgMediaControl::VisiblePlaceholder() const
    {
    CCoeControl* result = NULL;
    if ( iIconControl->IsVisible() )
        {
        result = iIconControl;
        }
    else if ( AnimationVisible() )
        {
        result = iAnimation;
        }
        
    return result;
    }

// ---------------------------------------------------------
// ResetIconId
// ---------------------------------------------------------
//        
void CMsgMediaControl::ResetIconId()
    {
    iIconControl->SetIcon( NULL );
    iFrame->ResetFrameSize();
    }

//  End of File
