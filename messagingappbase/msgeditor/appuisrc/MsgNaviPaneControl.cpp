/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgNaviPaneControl implementation
*
*/



// ========== INCLUDE FILES ================================

#include <coecontrolarray.h>
#include <eiklabel.h>
#include <eikimage.h>

#include <data_caging_path_literals.hrh>
#include <barsread.h>               // TResourceReader
#include <gulicon.h>                // CGulIcon
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <AknUtils.h>
#include <avkon.rsg>
#include <layoutmetadata.cdl.h>     // Layout
#include <applayout.cdl.h>          // LAF
#include <AknStatuspaneUtils.h>     // AknStatuspaneUtils
#include <aknlayoutscalable_apps.cdl.h>
#include <StringLoader.h>
#include <MuiuMessageIterator.h>    // for CMessageIterator
#include <muiumsvuiserviceutilitiesinternal.h>
#include <MsgEditorAppUi.rsg>       // resouce identifiers

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif 

#include <msgeditor.mbg>

#include "MsgNaviPaneControl.h"
#include "MsgNaviPaneControlObserver.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

_LIT( KIconMbmFile, "msgeditor.mbm" );
const TInt KTimeStringMaxLength = 32;

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::CMsgNaviPaneControl
// C++ default constructor.
// ----------------------------------------------------------------------------
//
CMsgNaviPaneControl::CMsgNaviPaneControl()
    {
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::ConstructL
// Symbian 2nd phase constructor.
// ----------------------------------------------------------------------------
//
#ifdef RD_MSG_NAVIPANE_IMPROVEMENT
void CMsgNaviPaneControl::ConstructL( const CCoeControl* aParent )
    {
    SetContainerWindowL( *aParent );
    
    iControlArray = CCoeControlArray::NewL( *this );
    iControlArray->SetControlsOwnedExternally( EFalse );
    iPriorityIndicator = EFalse;
    iMessageIterator = NULL;
    UpdateVarientId();
    SetComponentsToInheritVisibility( ETrue );
    ActivateL();
    }
#else
void CMsgNaviPaneControl::ConstructL( const CCoeControl* /*aParent*/ )
    {
    User::Leave( KErrNotSupported );
    }
#endif


// ---------------------------------------------------------
// CMsgNaviPaneControl::NewL
//
// Symbian two phased constructor
// ---------------------------------------------------------
//
EXPORT_C CMsgNaviPaneControl* CMsgNaviPaneControl::NewL( const CCoeControl* aParent )
    {
    CMsgNaviPaneControl* self = new(ELeave) CMsgNaviPaneControl();
    
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::~CMsgNaviPaneControl
// Destructor.
// ----------------------------------------------------------------------------
//
CMsgNaviPaneControl::~CMsgNaviPaneControl()
    {
    delete iControlArray;
    delete iMessageIterator;
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::SetTimeIndicatorL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMsgNaviPaneControl::SetTimeIndicatorL( TTime aTime, TBool aUtcTime )
    {
    if ( aUtcTime )
        {
        MsvUiServiceUtilitiesInternal::ConvertUtcToLocalTime( aTime );
        }    
    
    iTime = aTime;
    
    UpdateTimeIndicatorL();
    }
        
// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::SetPriorityIndicatorL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMsgNaviPaneControl::SetPriorityIndicatorL( TMsgEditorMsgPriority aPriority )
    {
    iPriority = aPriority;
    
    UpdatePriorityIndicatorL();
    }
        
// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::SetNavigationIndicatorL
// ----------------------------------------------------------------------------
//
EXPORT_C void CMsgNaviPaneControl::SetNavigationIndicatorL( CMsvSession& aSession,
                                                   const TMsvEntry& aCurrentMessage )
    {
    if(!iMessageIterator)
        {
        // Iterator will be deleted once in the destructer only.
        //deleting and creating it again is degrading the performance
 
        iMessageIterator = CMessageIterator::NewL( aSession, aCurrentMessage );
        iMessageIterator->SetMessageIteratorObserver( this );
        }
    UpdateNavigationIndicatorsL();
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::SetNavigationControlObserver
// ----------------------------------------------------------------------------
//
EXPORT_C void CMsgNaviPaneControl::SetNavigationControlObserver( MMsgNaviPaneControlObserver* aObserver)
    {
    iObserver = aObserver;
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::CountComponentControls
// Returns number of child controls.
// ----------------------------------------------------------------------------
//
TInt CMsgNaviPaneControl::CountComponentControls() const
    {
    return iControlArray->Count();
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::ComponentControl
// Returns specified child controls.
// ----------------------------------------------------------------------------
//
CCoeControl* CMsgNaviPaneControl::ComponentControl( TInt aIndex ) const
    {
    return iControlArray->At( aIndex ).iControl;
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::SizeChanged
// ----------------------------------------------------------------------------
//
void CMsgNaviPaneControl::SizeChanged()
    {
    LayoutIndicators();
    }
        
// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::PositionChanged
// ----------------------------------------------------------------------------
//      
void CMsgNaviPaneControl::PositionChanged()
    {
    LayoutIndicators();
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::HandlePointerEventL
// ----------------------------------------------------------------------------
//      
void CMsgNaviPaneControl::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    TBool tactilefeedback = EFalse;
    if ( iObserver &&
         aPointerEvent.iType == TPointerEvent::EButton1Down ||
         aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        CEikImage* leftArrowIndicator = iControlArray->ControlById<CEikImage>( EMsgNaviLeftArrowControlId );
        CEikImage* rightArrowIndicator = iControlArray->ControlById<CEikImage>( EMsgNaviRightArrowControlId );
        
        if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
            {
            iPreviouslyFocusedControl = NULL;
            }
        
        if ( leftArrowIndicator &&
             iLeftArrowTouchRect.Contains( aPointerEvent.iPosition ) )
            {
            tactilefeedback = ETrue;
            if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
                {
                iPreviouslyFocusedControl = leftArrowIndicator;
                }
            else if ( iPreviouslyFocusedControl == leftArrowIndicator )
                {
                iObserver->HandleNavigationControlEventL( 
                            MMsgNaviPaneControlObserver::EMsgNaviLeftArrowPressed );
                }
            }
        else if ( rightArrowIndicator &&
                  iRightArrowTouchRect.Contains( aPointerEvent.iPosition ) )
            {
            tactilefeedback = ETrue;
            if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
                {
                iPreviouslyFocusedControl = rightArrowIndicator;
                }
            else if ( iPreviouslyFocusedControl == rightArrowIndicator )
                {
                iObserver->HandleNavigationControlEventL( 
                            MMsgNaviPaneControlObserver::EMsgNaviRightArrowPressed );
                }
            }
        }
#ifdef RD_TACTILE_FEEDBACK 
     if ( aPointerEvent.iType == TPointerEvent::EButton1Down && tactilefeedback )
        {
        MTouchFeedback* feedback = MTouchFeedback::Instance();
        if ( feedback )
            {
            feedback->InstantFeedback( this, ETouchFeedbackBasic );                        
            }                                     
        }
#endif
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::HandleResourceChange
// ----------------------------------------------------------------------------
//      
void CMsgNaviPaneControl::HandleResourceChange( TInt aType )
    {
    if ( aType == KAknsMessageSkinChange )
        {
        UpdateVarientId();
        TRAP_IGNORE( HandleSkinChangeL() );
        }
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::HandleIteratorEventL
// ----------------------------------------------------------------------------
//      
void CMsgNaviPaneControl::HandleIteratorEventL( TMessageIteratorEvent aEvent )
    {
    if ( aEvent == EFolderCountChanged )
        {
        UpdateVarientId();
        UpdateNavigationIndicatorsL();
        LayoutNavigationIndicators();
        }
    }
    
// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::LayoutIndicators
// Sets correct layout for volume control according current LAF. Determines also
// if either of the arrow icons are overlapping with the pause indicator.
// ----------------------------------------------------------------------------
//
void CMsgNaviPaneControl::LayoutIndicators()
    {
    UpdateVarientId();
    LayoutTimeIndicator();
    LayoutNavigationIndicators();
    LayoutPriorityIndicator();
    }
    
// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::LayoutNavigationIndicators
// ----------------------------------------------------------------------------
//
void CMsgNaviPaneControl::LayoutNavigationIndicators()
    {
    if ( Rect().IsEmpty() )
        {
        return;
        }
        
    CEikImage* leftArrowIndicator = iControlArray->ControlById<CEikImage>( EMsgNaviLeftArrowControlId );
    if ( leftArrowIndicator )
        {
        TAknLayoutRect leftArrowPane;
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            leftArrowPane.LayoutRect( Rect(), AknLayoutScalable_Apps::navi_pane_g3(iVarientId) );
            }
        else
            {
            leftArrowPane.LayoutRect( Rect(), AknLayoutScalable_Apps::navi_pane_g2(iVarientId));
            }
        
        SetIconIndicatorExtent( leftArrowIndicator, leftArrowPane );    
        }
    
    CEikImage* rightArrowIndicator = iControlArray->ControlById<CEikImage>( EMsgNaviRightArrowControlId );
    if ( rightArrowIndicator )
        {
        TAknLayoutRect rightArrowPane;
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            rightArrowPane.LayoutRect( Rect(), AknLayoutScalable_Apps::navi_pane_g2(iVarientId) );
            }
        else
            {
            rightArrowPane.LayoutRect( Rect(), AknLayoutScalable_Apps::navi_pane_g3(iVarientId));
            }
        
        
        SetIconIndicatorExtent( rightArrowIndicator, rightArrowPane );    
        }

    if ( AknLayoutUtils::PenEnabled() )
        {
        TAknLayoutRect leftArrowTouchRect;
        leftArrowTouchRect.LayoutRect( Rect(), AknLayoutScalable_Apps::aid_size_touch_mv_arrow_left(iVarientId) );
    
        TAknLayoutRect rightArrowTouchRect;
        rightArrowTouchRect.LayoutRect( Rect(), AknLayoutScalable_Apps::aid_size_touch_mv_arrow_right(iVarientId) );
        if ( AknLayoutUtils::LayoutMirrored() )
        	{
        		iLeftArrowTouchRect=rightArrowTouchRect.Rect();
        		iRightArrowTouchRect= leftArrowTouchRect.Rect();
        	}
        else
        	{
        		iLeftArrowTouchRect=leftArrowTouchRect.Rect();
        		iRightArrowTouchRect= rightArrowTouchRect.Rect();
        	}
 
        }
    }
        
// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::LayoutPriorityIndicator
// ----------------------------------------------------------------------------
//
void CMsgNaviPaneControl::LayoutPriorityIndicator()
    {
    if ( Rect().IsEmpty() )
        {
        return;
        }
    
    CEikImage* priorityIndicator = iControlArray->ControlById<CEikImage>( EMsgNaviPriorityControlId );
    if ( priorityIndicator )
        {
        TAknLayoutRect priorityPane;
        if(Layout_Meta_Data::IsLandscapeOrientation()) // in landscape mode
            {
            priorityPane.LayoutRect( Rect(), AknLayoutScalable_Apps::navi_pane_mv_g2(2) );      
            }        
        else // portrait mode
            {
            priorityPane.LayoutRect( Rect(), AknLayoutScalable_Apps::navi_pane_mv_g2(1) );      
            }            
        SetIconIndicatorExtent( priorityIndicator, priorityPane );
        }
    }
    
// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::LayoutTimeIndicatorL
// ----------------------------------------------------------------------------
//
void CMsgNaviPaneControl::LayoutTimeIndicator()
    {
    if ( Rect().IsEmpty() )
        {
        return;
        }
    
    CEikLabel* timeIndicator = iControlArray->ControlById<CEikLabel>( EMsgNaviTimeControlId );
    if ( timeIndicator )
        {
        TAknLayoutText timePane;
        timePane.LayoutText( Rect(), AknLayoutScalable_Apps::navi_pane_mv_t1(iVarientId) );
        
        timeIndicator->SetRect( timePane.TextRect() );
        timeIndicator->SetFont( timePane.Font() );
        
        TRgb timeColor( AKN_LAF_COLOR( KRgbBlack.Value() ) );
        
        AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), 
                                   timeColor, 
                                   KAknsIIDQsnIconColors,
                                   EAknsCIQsnIconColorsCG7 );
        
        TRAP_IGNORE( timeIndicator->OverrideColorL( EColorLabelText, timeColor ) );   
        
        TInt align( ELayoutAlignNone );
        switch( timePane.Align() )
            {
            case CGraphicsContext::ELeft:
                {
                align = ELayoutAlignLeft;
                break;
                }
            case CGraphicsContext::ECenter:
                {
                align = ELayoutAlignCenter;
                break;
                }
            case CGraphicsContext::ERight:
                {
                align = ELayoutAlignRight;
                break;
                }
            default:
                {
                break;
                }
            }
            
        timeIndicator->SetLabelAlignment( align );
        }
    }
        
// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::SetIconIndicatorExtent
// Sets given icon extent to specified one.
// ----------------------------------------------------------------------------
//
void CMsgNaviPaneControl::SetIconIndicatorExtent( CEikImage* aIconIndicator,
                                                   const TAknLayoutRect& aExtent) const
    {
    TInt result = AknIconUtils::SetSize( const_cast<CFbsBitmap*>( aIconIndicator->Bitmap() ), 
                                         aExtent.Rect().Size() );
    
    if ( result == KErrNone )
        {
        aIconIndicator->SetRect( aExtent.Rect() );
        }
    else
        {
        aIconIndicator->SetRect( TRect() );
        }
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::InitializeIconIndicatorL
// Creates and initializes given member variable icon control with specified bitmap and mask.
// ----------------------------------------------------------------------------
//
void CMsgNaviPaneControl::InitializeIconIndicatorL( CEikImage*& aIndicator, 
                                                    const TAknsItemID& aItem, 
                                                    const TInt aBitmapIndex,
                                                    const TInt aMaskIndex ) const
    {
    // Initializes member variable
    aIndicator = new( ELeave ) CEikImage;
    aIndicator->SetContainerWindowL( *this );
    
    TParse fileParse;
    User::LeaveIfError( fileParse.Set( KIconMbmFile, &KDC_APP_BITMAP_DIR, NULL ) );
    
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    
    AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(), 
                                 aItem,
                                 KAknsIIDQsnIconColors,
                                 EAknsCIQsnIconColorsCG7,
                                 bitmap, 
                                 mask,
                                 fileParse.FullName(), 
                                 aBitmapIndex,
                                 aMaskIndex,
                                 AKN_LAF_COLOR( KRgbBlack.Value() ) ); 
                                     
    
    aIndicator->SetNewBitmaps( bitmap, mask );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerVolumeIndicatorController::UpdateNavigationIndicatorsL
// ----------------------------------------------------------------------------
//
void CMsgNaviPaneControl::UpdateNavigationIndicatorsL()
    {
    if ( iEikonEnv->StartedAsServerApp() )
        {
        TBool nextMessage = iMessageIterator->NextMessageExists();
        TBool previousMessage = iMessageIterator->PreviousMessageExists();
        
        CEikImage* rightArrowIndicator = iControlArray->ControlById<CEikImage>( EMsgNaviRightArrowControlId );
        CEikImage* leftArrowIndicator = iControlArray->ControlById<CEikImage>( EMsgNaviLeftArrowControlId );
        CEikImage* navigationIndicator = iControlArray->ControlById<CEikImage>( EMsgNaviNavigationControlId );

        if ( AknLayoutUtils::LayoutMirrored() )
            {
            TBool temp = nextMessage;
            nextMessage = previousMessage;
            previousMessage = temp;
            }

        if ( nextMessage )
            {
            if ( !rightArrowIndicator )
                {
                InitializeIconIndicatorL( rightArrowIndicator, 
                                          KAknsIIDQgnIndiNaviArrowRight,
                                          EMbmMsgeditorQgn_indi_navi_arrow_right, 
                                          EMbmMsgeditorQgn_indi_navi_arrow_right_mask );
                                          
                AddToControlArrayL( rightArrowIndicator, EMsgNaviRightArrowControlId );
                }
            }
        else
            {
            RemoveFromControlArray( EMsgNaviRightArrowControlId );
            rightArrowIndicator = NULL;
            }
        
        if ( previousMessage )
            {
            if ( !leftArrowIndicator )
                {
                InitializeIconIndicatorL( leftArrowIndicator, 
                                          KAknsIIDQgnIndiNaviArrowLeft,
                                          EMbmMsgeditorQgn_indi_navi_arrow_left, 
                                          EMbmMsgeditorQgn_indi_navi_arrow_left_mask );
                                          
                AddToControlArrayL( leftArrowIndicator, EMsgNaviLeftArrowControlId );
                }
            }
        else
            {
            RemoveFromControlArray( EMsgNaviLeftArrowControlId );
            leftArrowIndicator = NULL;
            }
        
        if ( rightArrowIndicator || leftArrowIndicator )
            {
            LayoutNavigationIndicators();
            }
        else
            {
            RemoveFromControlArray( EMsgNaviNavigationControlId );
            }
        
        DrawDeferred();
        }
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::UpdatePriorityIndicatorL
// ----------------------------------------------------------------------------
//
void CMsgNaviPaneControl::UpdatePriorityIndicatorL()
    {
    RemoveFromControlArray( EMsgNaviPriorityControlId );
    
    CEikImage* priorityIndicator = NULL;
    
    if ( iPriority == EMsgEditorMsgPriorityLow )
        {
        InitializeIconIndicatorL( priorityIndicator, 
                                  KAknsIIDQgnIndiMcePriorityLow,
                                  EMbmMsgeditorQgn_indi_mce_priority_low, 
                                  EMbmMsgeditorQgn_indi_mce_priority_low_mask );
        }
    else if ( iPriority == EMsgEditorMsgPriorityHigh )
        {
        InitializeIconIndicatorL( priorityIndicator, 
                                  KAknsIIDQgnIndiMcePriorityHigh,
                                  EMbmMsgeditorQgn_indi_mce_priority_high, 
                                  EMbmMsgeditorQgn_indi_mce_priority_high_mask );
        }
    
    if ( priorityIndicator )
        {
        iPriorityIndicator = ETrue;
        AddToControlArrayL( priorityIndicator, EMsgNaviPriorityControlId );
        LayoutPriorityIndicator();
        }
    else
        {
        iPriorityIndicator = EFalse;
        }
    
    DrawDeferred();
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::UpdateTimeIndicatorL
// ----------------------------------------------------------------------------
//
void CMsgNaviPaneControl::UpdateTimeIndicatorL()
    {
    TTime homeTime;
    homeTime.HomeTime();
    
    TDateTime nowDateTime = homeTime.DateTime();    
    TDateTime messageDateTime = iTime.DateTime();
    
    
    HBufC* dateFormat = StringLoader::LoadLC( R_QTN_DATE_SHORT_WITH_ZERO, iCoeEnv );
    TBuf<KTimeStringMaxLength> datestring;
    iTime.FormatL( datestring, dateFormat->Des() );
   

    HBufC* timeFormat = StringLoader::LoadLC( R_QTN_TIME_USUAL,iCoeEnv );
    TBuf<KTimeStringMaxLength> timestring;
    iTime.FormatL( timestring, timeFormat->Des() );


     CDesCArray* array = new( ELeave )CDesCArrayFlat( 2 ); // two items
     CleanupStack::PushL( array );
     array->AppendL( timestring );
     array->AppendL( datestring );
     HBufC* buff = StringLoader::LoadLC( R_QTN_MSG_TIMESTAMP,*array,iCoeEnv );                   
                                         

 
    CEikLabel* timeIndicator = iControlArray->ControlById<CEikLabel>( EMsgNaviTimeControlId );
    if ( !timeIndicator )
        {
        timeIndicator = new( ELeave ) CEikLabel;
        AddToControlArrayL( timeIndicator, EMsgNaviTimeControlId );
        
        timeIndicator->SetContainerWindowL( *this );
        }
    
   	TPtr datetimePtr = buff->Des();
   	AknTextUtils::DisplayTextLanguageSpecificNumberConversion(datetimePtr);
    
    timeIndicator->SetTextL(buff->Des());
  	CleanupStack::PopAndDestroy(4); // buff,array,timeformat,dateformat
  	    
    LayoutTimeIndicator();
    DrawDeferred();
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::AddToControlArrayL
// ----------------------------------------------------------------------------
//
void CMsgNaviPaneControl::AddToControlArrayL( CCoeControl* aControl, TMsgNaviControlId aControlId )
    {
    iControlArray->AppendLC( aControl, aControlId );
    CleanupStack::Pop( aControl );
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::RemoveFromControlArray
// ----------------------------------------------------------------------------
//
void CMsgNaviPaneControl::RemoveFromControlArray( TMsgNaviControlId aControlId )
    {
    CCoeControl* control = iControlArray->RemoveById( aControlId );
    delete control;
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::HandleSkinChangeL
//
// Sets the currently used skin color for shown indicators.
// ----------------------------------------------------------------------------
//
void CMsgNaviPaneControl::HandleSkinChangeL()
    {
    TRgb indicatorColor( AKN_LAF_COLOR( KRgbBlack.Value() ) );
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), 
            indicatorColor, 
            KAknsIIDQsnIconColors,
            EAknsCIQsnIconColorsCG7 );
    
    CEikLabel* timeIndicator = iControlArray->ControlById<CEikLabel>( EMsgNaviTimeControlId );
    if ( timeIndicator )
        {        
        timeIndicator->OverrideColorL( EColorLabelText, indicatorColor );
        }
    
    RemoveFromControlArray( EMsgNaviPriorityControlId );
    UpdatePriorityIndicatorL();
    
    RemoveFromControlArray( EMsgNaviRightArrowControlId );
    RemoveFromControlArray( EMsgNaviLeftArrowControlId );
    RemoveFromControlArray( EMsgNaviNavigationControlId );
    UpdateNavigationIndicatorsL();
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::UpdateVarientId
//
// Depending upon the orientation and Priority of the Message. varientid is set.
// ----------------------------------------------------------------------------
//
void CMsgNaviPaneControl::UpdateVarientId()
    {
        if(Layout_Meta_Data::IsLandscapeOrientation()) 
            {
            if(iPriorityIndicator)
                {
                iVarientId = 4;    
                }
            else
                {
                iVarientId = 3;    
                }                            
            }
        else // portrait mode 
            {
            if(iPriorityIndicator)
                {
                iVarientId = 2;    
                }
            else
                {
                iVarientId = 1;    
                }                  
            }    
    }

// ----------------------------------------------------------------------------
// CMsgNaviPaneControl::GetNavigationIndicator
// ----------------------------------------------------------------------------
//
EXPORT_C CMessageIterator* CMsgNaviPaneControl::GetMessageIterator()
    {
     return iMessageIterator;
    }
//  End of File
