/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgExpandableControl implementation
*
*/



// ========== INCLUDE FILES ================================

#include <coeinput.h>                      // for TCoeInputCapabilities
#include <barsread.h>                      // for TResourceReader
#include <eiklabel.h>                      // for CEikLabel
#include <eikenv.h>                        // for CEikonEnv
#include <eikedwin.h>                      // for TClipboardFunc
#include <txtrich.h>                       // for CRichText
#include <AknUtils.h>                      // for AknUtils
#include <aknenv.h>                        // for CAknEnv
#include <AknDef.h>
#include <AknsUtils.h>                     // for Skinned drawing
#include <aknbutton.h>                     // for CAknButton

#include <applayout.cdl.h> // LAF
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include "MsgEditorCommon.h"               //
#include "MsgExpandableControl.h"          // for CMsgExpandableControl
#include "MsgExpandableControlEditor.h"    // for CMsgExpandableControlEditor
#include "MsgBaseControlObserver.h"        // for MMsgBaseControlObserver
#include "MsgEditorPanic.h"                // for CMsgEditor panics
#include "MsgEditorLogging.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

_LIT(KMsgDefaultCaption, " ");

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgExpandableControl::CMsgExpandableControl
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgExpandableControl::CMsgExpandableControl()
    {
    }

// ---------------------------------------------------------
// CMsgExpandableControl::CMsgExpandableControl
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgExpandableControl::CMsgExpandableControl( MMsgBaseControlObserver& aBaseControlObserver ) :
    CMsgExpandableTextEditorControl( aBaseControlObserver )
    {
    }

// ---------------------------------------------------------
// CMsgExpandableControl::ConstructFromResourceL
//
// Creates this control from resource.
// ---------------------------------------------------------
//
void CMsgExpandableControl::ConstructFromResourceL( TInt aResourceId )
    {
    BaseConstructL();  // Sets margins only

    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, aResourceId );

    // Create caption
    iCaption = CreateCaptionFromResourceL( reader );

    // Read control details from resource
    ReadControlPropertiesFromResourceL( reader );

    // Create editor
    iEditor = CreateEditorFromResourceL( reader );
    
    iEditor->SetControlType(EMsgExpandableControl); 
    iCtrltype = EMsgExpandableControl;
    iEditor->SetObserver( this );
    iEditor->SetEdwinSizeObserver( this );
    iEditor->AddEdwinObserverL( this );
    iEditor->SetBorder( TGulBorder::ENone );
	//Set the flag to enable partial screen 
    TInt flags = iEditor->AknEditorFlags();
    flags |= EAknEditorFlagEnablePartialScreen;
    iEditor->SetAknEditorFlags( flags );     
    
    SetPlainTextMode( ETrue );

    ResolveLayoutsL();

    CleanupStack::PopAndDestroy();  // reader
    }

// ---------------------------------------------------------
// CMsgExpandableControl::~CMsgExpandableControl
//
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgExpandableControl::~CMsgExpandableControl()
    {
    AknsUtils::DeregisterControlPosition( iCaption );
    delete iCaption;

#ifdef RD_SCALABLE_UI_V2
    if ( iButton )
        {
        AknsUtils::DeregisterControlPosition( iButton );
        delete iButton;
        }    
#endif // RD_SCALABLE_UI_V2
    
    AknsUtils::DeregisterControlPosition( iEditor );
    delete iEditor;
        
    AknsUtils::DeregisterControlPosition( this );
    }

// ---------------------------------------------------------
// CMsgExpandableControl::Editor
//
// Returns a reference to the editor control.
// ---------------------------------------------------------
//
EXPORT_C CEikRichTextEditor& CMsgExpandableControl::Editor() const
    {
    return *iEditor;
    }

// ---------------------------------------------------------
// CMsgExpandableControl::Caption
//
// Returns a reference to the label control.
// ---------------------------------------------------------
//
EXPORT_C CEikLabel& CMsgExpandableControl::Caption() const
    {
    return *iCaption;
    }

// ---------------------------------------------------------
// CMsgExpandableControl::SetTextContentL
//
// Sets text content to the control editor.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControl::SetTextContentL( CRichText& aText )
    {
    iEditor->SetTextContentL( aText );
    
    if ( iControlModeFlags & EMsgControlModeInitialized )
        {
        iControlModeFlags |= EMsgControlModeModified;
        }
    }

// ---------------------------------------------------------
// CMsgExpandableControl::SetTextContentL
//
// Sets text content to the control editor.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControl::SetTextContentL( const TDesC& aText )
    {
    iEditor->SetTextL( &aText );
    
    if ( iControlModeFlags & EMsgControlModeInitialized )
        {
        iControlModeFlags |= EMsgControlModeModified;
        }
    }

// ---------------------------------------------------------
// CMsgExpandableControl::TextContentStrippedL
//
// Copies stripped content to aBuf. Control chars, newlines and if
// aNoExtraSemicolons is ETrue, also two consecutive semicolons are
// replaced with semicolon and space.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControl::TextContentStrippedL( TDes& aBuf, 
                                                           TInt aMaxLen, 
                                                           TBool aNoExtraSemicolons )
    {
    CRichText& text = TextContent();
    TInt len = text.DocumentLength();
    HBufC* buf = HBufC::NewLC( len );
    TPtr ptr = buf->Des();
    text.ExtractSelectively( ptr, 0, len, CPlainText::EExtractVisible );

    if ( aNoExtraSemicolons )
        {
        TText ch0 = 0;
        len = ptr.Length();
        for (TInt i = 0; i < len; i++)
            {
            TText& ch1 = ptr[i];
            if ( i > 0 )
                {
                ch0 = ptr[i - 1];
                }
            if ( ( ch1 == KSemicolon || 
                   ch1 == KArabicSemicolon ) && 
                 ( ch0 == KSemicolon || 
                   ch0 == KArabicSemicolon ) )
                {
                ch1 = ' ';
                }
            }
        }

    ptr.TrimAll();

    len = ptr.Length();
    if ( len > aMaxLen )
        {
        ptr.Delete( aMaxLen, len - aMaxLen );
        }

    aBuf.Copy( ptr );

    CleanupStack::PopAndDestroy( buf );
    }

// ---------------------------------------------------------
// CMsgExpandableControl::NotifyViewEvent
//
// Notifies editor about the scroll event.
// ---------------------------------------------------------
//
void CMsgExpandableControl::NotifyViewEvent( TMsgViewEvent aEvent, TInt aParam )
    {
    TRAP_IGNORE( DoNotifyViewEventL( aEvent, aParam ) );
    }

// ---------------------------------------------------------
// CMsgExpandableControl::DoNotifyViewEventL
// ---------------------------------------------------------
//
void CMsgExpandableControl::DoNotifyViewEventL( TMsgViewEvent aEvent, TInt aParam )
    {
    switch ( aEvent )
        {
        case EMsgViewEventPrepareFocusTransitionUp:
            {
            if ( IsFocused() )
                {
                iEditor->ClearSelectionL();
                }
            break;
            }
        case EMsgViewEventPrepareFocusTransitionDown:
            {
            if ( IsFocused() )
                {
                iEditor->ClearSelectionL();
                }
            break;
            }
        case EMsgViewEventSetCursorFirstPos:
            {
            if ( iEditor->TextView() )
                {
                if ( IsReadOnly() )
                    {
                    // Do not set document position if autohightlight is
                    // used as it cancels current hightlight if used.
                    if ( !( aParam & EMsgViewEventAutoHighlight ) )
                        {
                        iEditor->SetCursorPosL( 0, EFalse );
                        }
                    }
                else
                    {
                    if ( !iEditor->CursorInFirstLine() )
                        {
                        iEditor->SetCursorPosL( 0, EFalse );
                        }
                    }
                }
            break;
            }
        case EMsgViewEventSetCursorLastPos:
            {
            if ( iEditor->TextView() )
                {
                TInt len = iEditor->TextLength();
                
                if ( IsReadOnly() )
                    {
                    // Do not set document position if autohightlight is
                    // used as it cancels current hightlight if used.
                    if ( !( aParam & EMsgViewEventAutoHighlight ) )
                        {
                        iEditor->SetCursorPosL( len, EFalse );
                        }
                    }
                else
                    {
                    if ( !iEditor->CursorInLastLine() )
                        {
                        iEditor->SetCursorPosL( len, EFalse );
                        }
                    }
                }
            break;
            }
        case EMsgViewEventPrepareForViewing:
            {    
            /*
            iEditor->SetSuppressFormatting( EFalse );
            iEditor->NotifyNewFormatL();
            */
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMsgExpandableControl::Reset
//
// Reset contents of the editor.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControl::Reset()
    {
    iEditor->Reset();
    iControlModeFlags |= EMsgControlModeModified;
    }

// ---------------------------------------------------------
// CMsgExpandableControl::SetAndGetSizeL
//
// Sets sizes for the caption and control according to aSize.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControl::SetAndGetSizeL( TSize& aSize )
    {   
#ifdef RD_SCALABLE_UI_V2
    
    iCaption->SetSize( iCaptionLayout.TextRect().Size() );
    
    if ( iButton )
        {
        iButton->SetSize( iButtonLayout.Rect().Size() );
        }
#else
    iCaption->SetSize( iCaptionLayout.TextRect().Size() );
#endif // RD_SCALABLE_UI_V2
    
    TSize editorSize( iEditorLayout.TextRect().Width(),
                      iBaseControlObserver->ViewRect().Height() );    
    iEditor->SetAndGetSizeL( editorSize );
    
    TSize thisSize( aSize.iWidth, 0 );
    if( iEditor->TextLayout() && iEditor->TextLayout()->IsFormattingBand( ) )
        {
        thisSize.iHeight = MsgEditorCommons::MaxBodyHeight( );
        }
    else
        {
        thisSize.iHeight = iSize.iHeight;
        }
    
    // Control height is always atleast one line height
    thisSize.iHeight = Max( MsgEditorCommons::MsgBaseLineDelta(), thisSize.iHeight );
        
    SetSizeWithoutNotification( thisSize );
    aSize = iSize;
    }

// ---------------------------------------------------------
// CMsgExpandableControl::IsFocusChangePossible
//
// Checks if focus up or down depending on aDirection is possible and returns
// ETrue if it is.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgExpandableControl::IsFocusChangePossible( TMsgFocusDirection aDirection ) const
    {
    switch ( aDirection )
        {
        case EMsgFocusUp:
            {
            if ( IsReadOnly() )
                {
                return iEditor->IsFirstLineVisible();
                }
            else
                {
                return IsCursorLocation( EMsgTop );
                }
            }
        case EMsgFocusDown:
            {
            if ( IsReadOnly() )
                {
                return iEditor->IsLastLineVisible();
                }
            else
                {
                return IsCursorLocation( EMsgBottom );
                }
            }
        default:
            {
            break;
            }
        }

    return EFalse;
    }

// ---------------------------------------------------------
// CMsgExpandableControl::SetContainerWindowL
//
// Sets container window.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControl::SetContainerWindowL( const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );
    
    iCaption->SetContainerWindowL( *this );
    iEditor->SetContainerWindowL( *this );

#ifdef RD_SCALABLE_UI_V2
    if ( iButton )
        {
        iButton->SetContainerWindowL( *this );
        }
#endif // RD_SCALABLE_UI_V2
    }

// ---------------------------------------------------------
// CMsgExpandableControl::OfferKeyEventL
//
// Handles key events.
// ---------------------------------------------------------
//
EXPORT_C TKeyResponse CMsgExpandableControl::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                             TEventCode aType )
    {
    return iEditor->OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CMsgExpandableControl::MinimumSize
//
//
// ---------------------------------------------------------
//
EXPORT_C TSize CMsgExpandableControl::MinimumSize()
    {
    return TSize( iSize.iWidth,
                  iEditor->MinimumDefaultHeight() + iMargins.iTop + iMargins.iBottom );
    }

// ---------------------------------------------------------
// CMsgExpandableControl::InputCapabilities
//
// Returns input capabilities.
// ---------------------------------------------------------
//
EXPORT_C TCoeInputCapabilities CMsgExpandableControl::InputCapabilities() const
    {
    if ( iEditor->IsFocused() )
        {
        TCoeInputCapabilities inputCapabilities( TCoeInputCapabilities::ENone, 
                                                 NULL, 
                                                 const_cast<CMsgExpandableControl*>( this ) );
        inputCapabilities.MergeWith( iEditor->InputCapabilities() );
        
        return inputCapabilities;
        }
    else
        {
        return TCoeInputCapabilities::ENone;
        }
    }

// ---------------------------------------------------------
// CMsgExpandableControl::HandleResourceChange
//
// Updates caption text color if skin is changed.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControl::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRAP_IGNORE( ResolveLayoutsL() );
        }
        
    CMsgExpandableTextEditorControl::HandleResourceChange( aType );
    
    // Control size can be updated only after editor has updated
    // virtual height accordong to current font
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        // Control height can change if new layout uses
        // font with different height. After HandleResourceChange
        // controls must know their correct height because
        // control distances are adjusted related to other controls
        // before GetAndSetSize is called. This is the reason why
        // this must be done here.
        TSize desirableSize( iEditorLayout.TextRect().Width(), 
                             iEditor->VirtualHeight() );
        
        if ( desirableSize.iHeight != Size().iHeight )
            {
            TInt maxBodyHeight( iMaxBodyHeight );

            if ( desirableSize.iHeight > maxBodyHeight )
                {
                desirableSize.iHeight = maxBodyHeight;
                }
            
            SetSizeWithoutNotification( desirableSize );
            
            TRAP_IGNORE( iEditor->SetAndGetSizeL( desirableSize ) ); 
            }
        }
    else if ( aType == KAknsMessageSkinChange )
        {   
        TRAP_IGNORE( UpdateCaptionTextColorL() );
        }
    }

// ---------------------------------------------------------
// CMsgExpandableControl::HandleControlEventL
//
//
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
EXPORT_C void CMsgExpandableControl::HandleControlEventL( CCoeControl* aControl, 
                                                          TCoeEvent aEventType )
    {
    CMsgExpandableTextEditorControl::HandleControlEventL( aControl, aEventType );
    
    if ( aEventType == MCoeControlObserver::EEventStateChanged &&
         iButton && 
         aControl == iButton)
        {
        iBaseControlObserver->HandleEditObserverEventRequestL( this, EMsgButtonPressed, this );
        }
    }
#else
EXPORT_C void CMsgExpandableControl::HandleControlEventL( CCoeControl* aControl, 
                                                          TCoeEvent aEventType )
    {
    CMsgExpandableTextEditorControl::HandleControlEventL( aControl, aEventType );
    }
#endif // RD_SCALABLE_UI_V2

// ---------------------------------------------------------
// CMsgExpandableControl::CreateEditorL
//
// Creates the editor for the control.
// ---------------------------------------------------------
//
CMsgExpandableControlEditor* CMsgExpandableControl::CreateEditorL()
    {
    // Create control editor
    CMsgExpandableControlEditor* editor = 
                        new ( ELeave ) CMsgExpandableControlEditor( this, 
                                                                    iControlModeFlags, 
                                                                    iBaseControlObserver );

    CleanupStack::PushL( editor );
    editor->ConstructL();
    CleanupStack::Pop( editor );

    return editor;
    }

// ---------------------------------------------------------
// CMsgExpandableControl::ReadControlPropertiesFromResourceL
//
// Reads control properties from resource.
// ---------------------------------------------------------
//
void CMsgExpandableControl::ReadControlPropertiesFromResourceL( TResourceReader& aReader )
    {
    iControlModeFlags = aReader.ReadUint32();
    iControlId = aReader.ReadInt32();
    iDistanceFromComponentAbove = aReader.ReadInt32();
    iMaxNumberOfChars = aReader.ReadInt32();
    }

// ---------------------------------------------------------
// CMsgExpandableControl::CreateCaptionFromResourceL
//
// Creates caption for the control from resource.
// ---------------------------------------------------------
//
CEikLabel* CMsgExpandableControl::CreateCaptionFromResourceL( TResourceReader& aReader )
    {
    CEikLabel* caption = new ( ELeave ) CEikLabel;
    CleanupStack::PushL( caption );

    HBufC* captionText = aReader.ReadHBufCL();
    
    if ( captionText == NULL )
        {
        caption->SetTextL( KMsgDefaultCaption );
        }
    else
        {
        CleanupStack::PushL( captionText );
        caption->SetTextL( *captionText );
        CleanupStack::PopAndDestroy( captionText );
        }

    caption->CropText();
    
    CleanupStack::Pop( caption );

    return caption;
    }

// ---------------------------------------------------------
// CMsgExpandableControl::CreateEditorFromResourceL
//
// This virtual function does nothing but inherited function of this creates
// the editor for the control from resource.
// ---------------------------------------------------------
//
CMsgExpandableControlEditor* CMsgExpandableControl::CreateEditorFromResourceL( TResourceReader& aReader )
    {
    CMsgExpandableControlEditor* editor = 
            new ( ELeave ) CMsgExpandableControlEditor( this, 
                                                        iControlModeFlags, 
                                                        iBaseControlObserver );

    CleanupStack::PushL( editor );
    editor->SetMaxNumberOfChars( iMaxNumberOfChars );
    editor->ConstructFromResourceL( aReader );
    CleanupStack::Pop( editor );

    return editor;
    }

// ---------------------------------------------------------
// CMsgExpandableControl::PrepareForReadOnly
//
// Prepares read only or non read only state.
// ---------------------------------------------------------
//
void CMsgExpandableControl::PrepareForReadOnly( TBool aReadOnly )
    {
    TRAP_IGNORE( iEditor->PrepareForReadOnlyL( aReadOnly ) );
    }

// ---------------------------------------------------------
// CMsgExpandableControl::CountComponentControls
//
// Returns a number of controls.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgExpandableControl::CountComponentControls() const
    {
    CCoeControl* labelControl = NULL;
    
    if ( iButton )
        {
        labelControl = iButton;
        }
    else
        {
        labelControl = iCaption;
        }
        
    CCoeControl* controls[] = { labelControl, iEditor };

    TInt count = 0;
    for ( TUint ii = 0; ii < sizeof( controls ) / sizeof( CCoeControl* ); ii++ )
        {
        if ( controls[ii] )
            {
            count++;
            }
        }

    return count;
    }

// ---------------------------------------------------------
// CMsgExpandableControl::ComponentControl
//
// Returns a control of index aIndex.
// ---------------------------------------------------------
//
EXPORT_C CCoeControl* CMsgExpandableControl::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* labelControl = NULL;
    
    if ( iButton )
        {
        labelControl = iButton;
        }
    else
        {
        labelControl = iCaption;
        }
        
    CCoeControl* controls[] = { labelControl, iEditor };

    for ( TUint ii = 0; ii < sizeof( controls ) / sizeof( CCoeControl* ); ii++ )
        {
        if ( controls[ii] && aIndex-- == 0 )
            {
            return controls[ii];
            }
        }

    return NULL;
    }

// ---------------------------------------------------------
// CMsgExpandableControl::SizeChanged
//
// Sets positions for the caption and the editor.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControl::SizeChanged()
    {
    // TODO: No point to do all these calculations every time!

    MEBLOGGER_ENTERFN("CMsgExpandableControl::SizeChanged");

    if ( iControlModeFlags & EMsgControlModeSizeChanging )
        {
        MEBLOGGER_WRITE( "EMsgControlModeSizeChanging" );
        }
    else
        {
        TPoint editorPosition( iEditorLayout.TextRect().iTl.iX,
                               iPosition.iY + iEditorTop );
                               
#ifdef RD_SCALABLE_UI_V2
        
        TPoint captionPosition( iCaptionLayout.TextRect().iTl.iX,
                                iPosition.iY + iCaptionTop );

        iCaption->SetPosition( captionPosition );
        iCaption->MakeVisible( captionPosition.iY >= 0 );    
        
        if ( iButton )
            {
            TPoint buttonPosition( iButtonLayout.Rect().iTl.iX,
                                   iPosition.iY + iButtonTop );
                                
            iButton->SetPosition( buttonPosition );
            iButton->MakeVisible( buttonPosition.iY >= 0 );
            }
#else
        TPoint captionPosition( iCaptionLayout.TextRect().iTl.iX,
                                iPosition.iY + iCaptionTop );
        
        iCaption->SetPosition( captionPosition );
        iCaption->MakeVisible( captionPosition.iY >= 0 );
#endif // RD_SCALABLE_UI_V2
        
        TRect editorRect( editorPosition, iEditor->Size() );
        
        if ( editorRect != iEditor->Rect() )
            {
            iEditor->SetRect( editorRect );  // SetPosition cannot be used here
            }

        AknsUtils::RegisterControlPosition( this );
        AknsUtils::RegisterControlPosition( iCaption );
        
#ifdef RD_SCALABLE_UI_V2
        if ( iButton )
            {
            AknsUtils::RegisterControlPosition( iButton );
            }
#endif // RD_SCALABLE_UI_V2
        
        AknsUtils::RegisterControlPosition( iEditor );
        }

    MEBLOGGER_LEAVEFN("CMsgExpandableControl::SizeChanged");
    }

// ---------------------------------------------------------
// CMsgExpandableControl::FocusChanged
//
// This is called when the focus of the control is changed.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControl::FocusChanged( TDrawNow aDrawNow )
    {
    iEditor->SetFocus( IsFocused(), aDrawNow );
    
    if ( aDrawNow == EDrawNow )
        {
        DrawDeferred();
        }
    }

// ---------------------------------------------------------
// CMsgExpandableControl::Draw
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControl::Draw( const TRect& /*aRect*/ ) const
    {
    // All drawing is done by iEditor, iCaption and CMsgHeader
    // class (and CMsgCaptionedHighlight owned by the header)
    }

// ---------------------------------------------------------
// CMsgExpandableControl::ActivateL
//
// Sets caption text color from skin. This is the only common
// synchronization point before the control is drawn to the screen
// for expandable control and all the derived classes.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControl::ActivateL()
    {
    UpdateCaptionTextColorL();
    
    CMsgExpandableTextEditorControl::ActivateL();
    }

// ---------------------------------------------------------
// CMsgExpandableControl::ResolveLayoutsL
//
//
// ---------------------------------------------------------
//
void CMsgExpandableControl::ResolveLayoutsL()
    {
    TAknTextLineLayout editorLineLayout;

    TAknLayoutRect msgTextPane;
    msgTextPane.LayoutRect( MsgEditorCommons::MsgDataPane(),
                            AknLayoutScalable_Apps::msg_text_pane( 0 ).LayoutLine() );
                            
    TAknLayoutRect msgHeaderPane;
    msgHeaderPane.LayoutRect( msgTextPane.Rect(),
                              AknLayoutScalable_Apps::msg_header_pane().LayoutLine() );
                              
    // iFrameLayout is not really used with "scalable layouts"
    iFrameLayout.LayoutRect( msgHeaderPane.Rect(),
                             AknLayoutScalable_Apps::input_focus_pane_cp07().LayoutLine() );
                             
    iCaptionLayout.LayoutText( msgHeaderPane.Rect(),
                               AknLayoutScalable_Apps::msg_header_pane_t1( 0 ).LayoutLine() );
                               
    editorLineLayout = AknLayoutScalable_Apps::msg_header_pane_t2( 0 ).LayoutLine();
    
    iEditorLayout.LayoutText( msgHeaderPane.Rect(),
                              editorLineLayout );
    //partial input in landscape causes rects to overlay when toolbar is invisible
    if (iCaptionLayout.TextRect().Intersects(iEditorLayout.TextRect()))
        {
        TRect headerPane = msgHeaderPane.Rect();
		//include toolbar width
        if (!AknLayoutUtils::LayoutMirrored())
            {
            headerPane.iBr.iX -= GetToolBarRect().Width();
            }
        else
            {
            headerPane.iTl.iX += GetToolBarRect().Width();
            }
        
        iCaptionLayout.LayoutText( headerPane,
                                   AknLayoutScalable_Apps::msg_header_pane_t1( 0 ).LayoutLine() );
        }
    
    iEditorTop = iEditorLayout.TextRect().iTl.iY -
                 msgHeaderPane.Rect().iTl.iY;
    
    iEditor->SetMaximumHeight( MsgEditorCommons::MaxBodyHeight() - iEditorTop * 2 );
                                   
#ifdef RD_SCALABLE_UI_V2
    
    iCaptionTop = iCaptionLayout.TextRect().iTl.iY - msgHeaderPane.Rect().iTl.iY;

    iCaption->SetFont( iCaptionLayout.Font() );

    iCaption->SetAlignment( iCaptionLayout.Align() == CGraphicsContext::ELeft ? EHLeftVTop : 
                                                                                EHRightVTop );                    
    if ( iButton )
        {
        iButtonLayout.LayoutRect( msgHeaderPane.Rect(),
                                  AknLayoutScalable_Apps::bg_button_pane_cp01().LayoutLine() );
        
		//partial input in landscape causes rects to overlay when toolbar is invisible
        if (iButtonLayout.Rect().Intersects(iEditorLayout.TextRect()))
            {
            TRect headerPane = msgHeaderPane.Rect();
			//include toolbar width
            if (!AknLayoutUtils::LayoutMirrored())
                {
                headerPane.iBr.iX -= GetToolBarRect().Width();
                }
            else
                {
                headerPane.iTl.iX += GetToolBarRect().Width();
                }
            
            iButtonLayout.LayoutRect( headerPane,
                                      AknLayoutScalable_Apps::bg_button_pane_cp01().LayoutLine() );
            }    
                              
        iButtonTop = iButtonLayout.Rect().iTl.iY - msgHeaderPane.Rect().iTl.iY;
        
        LayoutButton();
        }
#else

    iCaptionTop = iCaptionLayout.TextRect().iTl.iY -
                  msgHeaderPane.Rect().iTl.iY;

    iCaption->SetFont( iCaptionLayout.Font() );
    iCaption->SetAlignment( iCaptionLayout.Align() == CGraphicsContext::ELeft ? EHLeftVTop : 
                                                                                EHRightVTop );
#endif // RD_SCALABLE_UI_V2
    
    iEditor->SetAlignment( editorLineLayout.iJ );
    }

// ---------------------------------------------------------
// CMsgExpandableControl::HandleFrameSizeChange
// ---------------------------------------------------------
//
void CMsgExpandableControl::HandleFrameSizeChange()
    {
    TRAP_IGNORE( iEditor->HandleFrameSizeChangeL( ) );
    }

// ---------------------------------------------------------
// CMsgExpandableControl::Button
// ---------------------------------------------------------
//
CAknButton* CMsgExpandableControl::Button() const
    {
    return iButton;
    }

// ---------------------------------------------------------
// CMsgExpandableControl::LayoutButton
// ---------------------------------------------------------
//
void CMsgExpandableControl::LayoutButton()
    {
#ifdef RD_SCALABLE_UI_V2
    if ( iButton )
        {
        iButton->SetTextFont( iCaptionLayout.Font() );
        iButton->SetTextColorIds( KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG63 );

        iButton->SetTextHorizontalAlignment( iCaptionLayout.Align() );

        if ( iCaptionLayout.Align() == CGraphicsContext::ELeft )
            {
            iButton->SetIconHorizontalAlignment( CAknButton::ELeft );
            }
        else
            {
            iButton->SetIconHorizontalAlignment( CAknButton::ERight );
            }        
        }
#endif // RD_SCALABLE_UI_V2
    }

// ---------------------------------------------------------
// CMsgExpandableControl::UpdateCaptionTextColorL
//
// Sets the correct text color for caption from currently 
// used theme. 
// ---------------------------------------------------------
//
void CMsgExpandableControl::UpdateCaptionTextColorL()
    {
    TRgb textColor;
    if ( AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                    textColor,
                                    KAknsIIDQsnTextColors,
                                    EAknsCIQsnTextColorsCG6 ) != KErrNone )
        {
        /*
        From "AknsConstants.h":
        // text #6	main area	main area texts	#215
        EAknsCIQsnTextColorsCG6    = 5,
        */
        textColor = AKN_LAF_COLOR_STATIC( 215 );
        }
    
    iCaption->OverrideColorL( EColorLabelText, textColor );
    }

// ---------------------------------------------------------
// CMsgExpandableControl::GetCaptionForFep
// ---------------------------------------------------------
//
void CMsgExpandableControl::GetCaptionForFep( TDes& aCaption ) const
    {
    const TDesC* captionText = Caption().Text();
    
    const TInt maximumLength = aCaption.MaxLength();
    
    if ( captionText->Length() > maximumLength )
        {
        aCaption = captionText->Left( maximumLength );
        }        
    else
        {
        aCaption = *captionText;
        }
    }

// ---------------------------------------------------------
// CMsgExpandableControl::GetToolBarRect
// ---------------------------------------------------------
//
TRect CMsgExpandableControl::GetToolBarRect()
    {
    TRect appRect;
    AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EApplicationWindow, appRect);

    TAknWindowLineLayout lineLayout =
            AknLayoutScalable_Avkon::area_side_right_pane(0);
    TAknLayoutRect toolBarLayoutRect;
    toolBarLayoutRect.LayoutRect(appRect, lineLayout);

    return toolBarLayoutRect.Rect();
    }
//  End of File
