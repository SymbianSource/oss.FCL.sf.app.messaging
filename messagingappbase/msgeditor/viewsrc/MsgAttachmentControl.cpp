/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgAttachmentControl implementation
*
*/



// ========== INCLUDE FILES ================================

#include <barsread.h>                      // for TResourceReader
#include <txtrich.h>                       // for CRichText

#include <eikenv.h>                        // for CEikonEnv
#include <eikappui.h>                      // for CEikAppUi
#include <eikedwin.h>                      // for TClipboardFunc
#include <eiklabel.h>

#include <AknUtils.h>                      // for AknUtils
#include <aknbutton.h>                     // for CAknButton

#include <MsgAttachmentModel.h>
#include <MsgAttachmentInfo.h>

#include <gulicon.h>
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <aknlayoutscalable_apps.cdl.h>

#include <data_caging_path_literals.hrh>
#include <msgeditor.mbg>
#include <aknlayoutscalable_avkon.cdl.h>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif 

#include "MsgAttachmentControl.h"             // for CMsgAttachmentControl
#include "MsgExpandableControlEditor.h"       // for CMsgExpandableControlEditor
#include "MsgBaseControlObserver.h"        // for MMsgBaseControlObserver
#include "MsgEditorView.h"
#include "MsgEditorPanic.h"                // for CMsgEditor panics
#include "MsgEditorCommon.h"               // for KArabicSemicolon
#include "MsgEditorCustomDraw.h"           // for CMsgEditorCustomDraw

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// Default color for EAknsCIQsnIconColorsCG13 group icons
const TInt KDefaultAttachmentColor = 215; 

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

const TInt KAttachmentArrayGranularity = 10;

_LIT( KAttachmentListSeparator, ", " );
_LIT( KIconMbmFile, "msgeditor.mbm" );
_LIT( KStripList, "\x000A\x000D\x2029\x2028\x0085" );

const TText KZeroWidthJoiner = 0x200D;
const TText KLRMarker = 0x200E;
const TText KRLMarker = 0x200F;

const TInt KScrollStartDelay = 1500000; // 1,5 s
const TInt KScrollEndDelay = 500000;    // 0,5 s
const TInt KScrollRestartDelay = 3000000; // 3 s.
const TInt KScrollTickDelay = 135000; // 0,135s
const TInt KScrollJumpAreaDivider = 15;

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================


// ---------------------------------------------------------
// CMsgAttachmentControl::CMsgAttachmentControl
//
// Constructor.
// ---------------------------------------------------------
//
CMsgAttachmentControl::CMsgAttachmentControl( 
                                    MMsgBaseControlObserver& aBaseControlObserver,
                                    CMsgEditorView& aView ) : 
    CMsgExpandableControl( aBaseControlObserver ),
    iScrollPos( EScrollPosBeginning )
    {
    iControlType = EMsgAttachmentControl;
    iControlId = EMsgComponentIdAttachment;
    
    SetMopParent( &aView );
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::ConstructL
//
// 2nd phase constructor. After base class initialization creates
// attachment array that holds the attachment name strings. Empty
// caption control is also created for base class purposes. NOTE! This is 
// something that would be good to get rid off. Initializes editor control
// and sets itself as it's edwin event, edwin size event and
// control event observer. Creates editor is set as readonly and cursor is
// hidden. Focus must stop to this control when navigating. No formatting
// is allowed. Label icon and if pen input (i.e. touch screen) is enabled
// then also button are created. Initializes marquee scroll timer and
// layout related parameters.
// ---------------------------------------------------------
//
void CMsgAttachmentControl::ConstructL()
    {
    BaseConstructL();  // Sets margins only.
    
    iAttachmentArray = new( ELeave ) CDesCArrayFlat( KAttachmentArrayGranularity );

    // Create empty caption. (Needed by base class.)
    iCaption = new( ELeave ) CEikLabel;

    // Create editor.
    iEditor = new( ELeave ) CMsgExpandableControlEditor( this, 
                                                         iControlModeFlags, 
                                                         iBaseControlObserver );
    iEditor->SetControlType(iControlType);   
    iEditor->ConstructL();
    iEditor->SetObserver( this );
    iEditor->SetEdwinSizeObserver( this );
    iEditor->AddEdwinObserverL( this );
    iEditor->SetReadOnly( ETrue );
    iEditor->AddFlagToUserFlags( CEikEdwin::EAvkonDisableCursor );
    
	// To Dis-able Smiley support for attachment field.
    iEditor->RemoveFlagFromUserFlags(CEikEdwin::EAvkonEnableSmileySupport);
    
    iControlModeFlags |= EMsgControlModeForceFocusStop;
    
    SetPlainTextMode( ETrue );
    
    iLabelIcon = CreateAttachmentIconsL();
    
#ifdef RD_SCALABLE_UI_V2
    if ( AknLayoutUtils::PenEnabled() )
        {
        CreateButtonL();
        }
#endif // RD_SCALABLE_UI_V2
    
    iScrollTimer = CPeriodic::NewL( EPriorityNormal );
    
    ResolveLayoutsL();
    }

// ----------------------------------------------------------------------------
// CMsgAttachmentControl::NewL
// Symbian two phased constructor
// ----------------------------------------------------------------------------
//
EXPORT_C CMsgAttachmentControl* CMsgAttachmentControl::NewL( 
                                                MMsgBaseControlObserver& aBaseControlObserver,
                                                CMsgEditorView& aView )
    {
    CMsgAttachmentControl* self = new( ELeave ) CMsgAttachmentControl( aBaseControlObserver,
                                                                       aView );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::~CMsgAttachmentControl
//
// Destructor.
// ---------------------------------------------------------
//
CMsgAttachmentControl::~CMsgAttachmentControl()
    {
    if ( iAttachmentArray )
        {
        iAttachmentArray->Reset();
        }
        
    delete iAttachmentArray;
    delete iLabelIcon;
    delete iScrollTimer;
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::AppendAttachmentL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentControl::AppendAttachmentL( const TDesC& aName )
    {
    InsertAttachmentL( aName, Count() );
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::InsertAttachmentL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentControl::InsertAttachmentL( const TDesC& aName, TInt aIndex )
    {
    AddToAttachmentArrayL( aName, aIndex );
    
    UpdateControlL();
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::AddAttachmentL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentControl::AddAttachmentsL( const MDesCArray& aNames )
    {
    Reset();
    
    for ( TInt i = 0; i < aNames.MdcaCount(); i++ )
        {
        AddToAttachmentArrayL( aNames.MdcaPoint( i ), Count() );
        }
        
    UpdateControlL();
    }


// ---------------------------------------------------------
// CMsgAttachmentControl::AddAttachmentL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentControl::AddAttachmentsL( const CMsgAttachmentModel& aModel )
    {
    Reset();
    
    TParse* parser = new( ELeave ) TParse ;
    CleanupStack::PushL( parser );
    
    for ( TInt i = 0; i < aModel.NumberOfItems(); i++ )
        {
        parser->Set( aModel.AttachmentInfoAt( i ).FileName(),NULL, NULL );
        AddToAttachmentArrayL( parser->NameAndExt(), Count() );
        }
    
    CleanupStack::PopAndDestroy( parser ); 
    UpdateControlL();
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::RemoveAttachmentL
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentControl::RemoveAttachmentL( const TDesC& aName )
    {
    HBufC* convertedFileName = AknTextUtils::ConvertFileNameL( aName );
    CleanupStack::PushL( convertedFileName );
    
    TPtr convertedFileNamePtr = convertedFileName->Des();
    
    AknTextUtils::StripCharacters( convertedFileNamePtr, KAknStripListControlChars );
    
    TInt pos( KErrNotFound );
    if ( iAttachmentArray->Find( *convertedFileName, pos ) == 0 )
        {
        RemoveAttachmentL( pos );
        }
    
    CleanupStack::PopAndDestroy( convertedFileName );
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::RemoveAttachmentL
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentControl::RemoveAttachmentL( TInt aIndex )
    {
    iAttachmentArray->Delete( aIndex );
    UpdateControlL();
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::Reset
//
// from CMsgBaseControl.
// Same as above but non-leaving version.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentControl::Reset()
    {
    if ( iAttachmentArray )
        {
        iAttachmentArray->Reset();
        }
    if ( iEditor )
        {
        iEditor->Reset();
        }
    }


// ---------------------------------------------------------
// CMsgAttachmentControl::Count
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgAttachmentControl::Count() const
    {
    return iAttachmentArray->MdcaCount();
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::Attachment
// ---------------------------------------------------------
//
EXPORT_C TPtrC CMsgAttachmentControl::Attachment( TInt aIndex ) const
    {
    return iAttachmentArray->MdcaPoint( aIndex );
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::ConstructFromResourceL
//
// Creates this control from resource.
// ---------------------------------------------------------
//
void CMsgAttachmentControl::ConstructFromResourceL( TInt aResourceId )
    {
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, aResourceId );

    // Read some information about control from resource.
    ReadControlPropertiesFromResourceL( reader );
    
    ConstructL();
    
    CleanupStack::PopAndDestroy();  // reader
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::EditPermission
//
// Checks and returns control's edit permissions. Edit permissions are needed
// to check in order to know whether some key press is allowed to pass
// to the control or not.
// ---------------------------------------------------------
//
TUint32 CMsgAttachmentControl::EditPermission() const
    {
    return CMsgBaseControl::EMsgEditNone;
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::NotifyViewEvent
//
// Builds up text fields for the control i.e. reads verified recipients and
// creates corresponding text fields for them.
// ---------------------------------------------------------
//
void CMsgAttachmentControl::NotifyViewEvent( TMsgViewEvent aEvent, TInt aParam )
    {
    if ( IsReadOnly() )
        {
        aParam |= EMsgViewEventAutoHighlight;
        }

    if ( aEvent != EMsgViewEventSetCursorFirstPos &&
         aEvent != EMsgViewEventSetCursorLastPos )
        {
        CMsgExpandableControl::NotifyViewEvent( aEvent, aParam );
        }
    }


// ---------------------------------------------------------
// CMsgAttachmentControl::PrepareForReadOnly
// ---------------------------------------------------------
//
void CMsgAttachmentControl::PrepareForReadOnly( TBool aReadOnly )
    {
    if ( aReadOnly )
        {
        TRAP_IGNORE( iEditor->SetHighlightStyleL( EEikEdwinHighlightLink ) );
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::CountComponentControls
//
// Returns a number of controls.
// ---------------------------------------------------------
//
TInt CMsgAttachmentControl::CountComponentControls() const
    {
    TInt result( 1 );
    
    if ( iButton )
        {
        result++;
        }
        
    return result;
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::ComponentControl
//
// Returns a control of index aIndex.
// ---------------------------------------------------------
//
CCoeControl* CMsgAttachmentControl::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* result = NULL;
    switch ( aIndex )
        {
        case 0:
            {
            result = iEditor;
            break;
            }
        case 1:
            {
            result = iButton;
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
// CMsgAttachmentControl::OfferKeyEventL
//
// Handles key events. The most of the keys are passed to the control editor.
// ---------------------------------------------------------
//
TKeyResponse CMsgAttachmentControl::OfferKeyEventL(
    const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/ )
    {
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::HandleResourceChange
//
// Handles resouce change events. Event are passed to the base
// class and if layout has been switched new layouts are resolved
// from LAF.
// ---------------------------------------------------------
//
void CMsgAttachmentControl::HandleResourceChange( TInt aType )
    {
    TRAP_IGNORE( 
        {
        DoHandleResourceChangeBeforeL( aType );
    
        CMsgExpandableControl::HandleResourceChange( aType );
        
        DoHandleResourceChangeAfterL( aType );
        } );    
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::HandlePointerEventL
//
// Pointer event handling is disabled on editor
// ---------------------------------------------------------
//
void CMsgAttachmentControl::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( !iEditor->Rect().Contains( aPointerEvent.iPosition ) )
        {
        CMsgExpandableControl::HandlePointerEventL( aPointerEvent );
        }
#ifdef RD_TACTILE_FEEDBACK 
    else if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        MTouchFeedback* feedback = MTouchFeedback::Instance();
        if ( feedback )
            {
            feedback->InstantFeedback( this, ETouchFeedbackBasic );                        
            }                                     
        }
#endif
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::FocusChanged
//
// This is called when the focus of the control is changed.
// ---------------------------------------------------------
//
void CMsgAttachmentControl::FocusChanged( TDrawNow aDrawNow )
    {
    CMsgExpandableControl::FocusChanged( aDrawNow );
        
    if ( IsReadOnly() )
        {
        TRAP_IGNORE( UpdateFocusL() );
        }
    
    if ( iAutomaticScrolling )
        {
        StopAutomaticScrolling();
        
        if ( IsFocused() )
            {
            StartAutomaticScrolling( KScrollStartDelay );
            }
        else
            {
            // Reset text to the beginning when focus is lost.
            iScrollPos = EScrollPosBeginning;
            TRAP_IGNORE( SetTextForEditorL( EFalse, ETrue ) );                
            
            // Reset the text view to it's original position.
            iEditor->TextView()->SetLeftTextMargin( 0 );
            }
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::Draw
// Attachment icon is drawn to the line indicated by LAF x 
// coordinate and controls current y coordinate. 
// Attachment icon is also centered vertically to current 
// control height.
// ---------------------------------------------------------
//
void CMsgAttachmentControl::Draw( const TRect& /*aRect*/ ) const
    {
    if ( !iButton )
        {
        TPoint tlc( iIconLayout.Rect().iTl.iX, Position().iY );
        tlc.iY += ( Size().iHeight - iIconLayout.Rect().Size().iHeight ) / 2;
    
        SystemGc().BitBltMasked( tlc,
                                 iLabelIcon->Bitmap(), 
                                 iIconLayout.Rect().Size(),
                                 iLabelIcon->Mask(),
                                 EFalse );
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::ActivateL
// ---------------------------------------------------------
//
void CMsgAttachmentControl::ActivateL()
    {
    CMsgExpandableControl::ActivateL();
    
    // Needs to be called after CEikEdwin::ActivateL as
    // panics if CEikEdwin::iTextView is not present
    iEditor->SetWordWrapL( EFalse );
    
    // Cursor is set to the beginning of text.
    iEditor->SetCursorPosL( 0, EFalse );
    
    if ( iEditor->TextView() )
        {
        const CFont* font = iEditorLayout.Font();
        TInt scrollJump( font->WidthZeroInPixels() );
        if ( scrollJump == 0 )
            {
            // Font gave us invalid scrollJump have to make
            // estimate according to scroll area.
            scrollJump = iEditorLayout.TextRect().Width() / KScrollJumpAreaDivider;
            }
        iEditor->TextView()->SetHorizontalScrollJump( scrollJump );
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::UpdateControlL
// ---------------------------------------------------------
//
void CMsgAttachmentControl::UpdateControlL()
    {
    SetTextForEditorL( ETrue, ETrue );
    
    if ( iScrollTimer->IsActive() )
        {
        // Restart scrolling when attachment list is modified.
        iScrollPos = EScrollPosBeginning;
        StartAutomaticScrolling( KScrollStartDelay );        
        }
    
    DrawDeferred();
    iControlModeFlags |= EMsgControlModeModified;
    }


// ---------------------------------------------------------
// CMsgAttachmentControl::SetTextForEditorL
// ---------------------------------------------------------
//
void CMsgAttachmentControl::SetTextForEditorL( TBool aContentChanged, TBool aClipText )
    {
    TInt count = iAttachmentArray->Count();
    TInt length = 0;
    
    for ( TInt i = 0; i < count; i++ )
        {
        length += iAttachmentArray->MdcaPoint( i ).Length();
        length += KAttachmentListSeparator().Length();
        }

    HBufC* buf = HBufC::NewLC( length );
    TPtr temp = buf->Des();
    
    for ( TInt j = 0; j < count; j++ )
        {
        temp.Append( iAttachmentArray->MdcaPoint( j ) );
        if ( j < count - 1 )
            {
            temp.Append( KAttachmentListSeparator );
            }
        }
        
    const CFont* font = iEditorLayout.Font();

    TInt clipWidth = 0;
    
    if ( iEditor->TextView() ) 
        {
        clipWidth = iEditor->TextView()->ViewRect().Width();
        }
    else
        {
        clipWidth = iEditorLayout.TextRect().Width();
        }
    
    TBool enableAutomaticScrolling( ETrue );
    if ( temp.Length() == 0 )
        {
        // Disable all processing if field is empty.
        enableAutomaticScrolling = EFalse;
        aContentChanged = EFalse;
        }
        
    if ( aContentChanged )
        {
        // Attachment array has been updated.
        TInt fits = font->TextCount( temp, clipWidth );

        if ( fits >= temp.Length() )
            {
            // All text fits to the field. No automatic scrolling.
            enableAutomaticScrolling = EFalse;
            }
        }
    
    if ( enableAutomaticScrolling )
        {
        EnableAutomaticScrolling();
        
        if ( aClipText )
            {
            ClipTextL( temp, clipWidth );
            }
        }
    else
        {
        DisableAutomaticScrolling();
        }
    
    // Replaces old text with new text.
    iEditor->SetTextL( buf );

    if ( IsActivated() && iEditor->CursorPos() != 0 )
        {
        // Cursor is set to the beginning of text.
        // Should not be called for non-activated control.
        iEditor->SetCursorPosL( 0, EFalse );
        }        
    
    CleanupStack::PopAndDestroy( buf );
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::ResolveLayoutsL
// 
// ---------------------------------------------------------
//
void CMsgAttachmentControl::ResolveLayoutsL()
    {
    TAknLayoutRect msgTextPane;
    msgTextPane.LayoutRect( MsgEditorCommons::MsgDataPane(),
                            AknLayoutScalable_Apps::msg_text_pane( 0 ).LayoutLine() );

    TAknLayoutRect msgHeaderPane;
    msgHeaderPane.LayoutRect( msgTextPane.Rect(),
                              AknLayoutScalable_Apps::msg_header_pane().LayoutLine() );
    
    iIconLayout.LayoutRect( msgHeaderPane.Rect(),
                            AknLayoutScalable_Apps::msg_header_pane_g1().LayoutLine() );
    
    CMsgExpandableControl::ResolveLayoutsL();
    
	//partial input in landscape causes rects to overlay when toolbar is invisible    
    if (iIconLayout.Rect().Intersects(iEditorLayout.TextRect()))
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
        
        iIconLayout.LayoutRect( headerPane,
                                AknLayoutScalable_Apps::msg_header_pane_g1( 0 ).LayoutLine() );
        LayoutButton();
        }    
    
    if ( iButton )
        {
        iButton->SetSize( iButton->Size() );
        }
    else
        {
        User::LeaveIfError( AknIconUtils::SetSize( iLabelIcon->Bitmap(), 
                                                   iIconLayout.Rect().Size() ) );
        }
     if ( iEditor->MsgEditorCustomDraw() )
        {
            iEditor->MsgEditorCustomDraw()->ResolveLayouts();
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::CreateAttachmentIconsL
// ---------------------------------------------------------
//
CGulIcon* CMsgAttachmentControl::CreateAttachmentIconsL()
    {
    TParse fileParse;
    fileParse.Set( KIconMbmFile, &KDC_APP_BITMAP_DIR, NULL );
    
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    
    AknsUtils::CreateColorIconLC( AknsUtils::SkinInstance(),
                                  KAknsIIDQgnIndiMceAttach,
                                  KAknsIIDQsnIconColors,
                                  EAknsCIQsnIconColorsCG13,
                                  bitmap, 
                                  mask,
                                  fileParse.FullName(), 
                                  EMbmMsgeditorQgn_indi_mce_attach,
                                  EMbmMsgeditorQgn_indi_mce_attach_mask,
                                  AKN_LAF_COLOR( KDefaultAttachmentColor ) ); 
    
    CGulIcon* result = CGulIcon::NewL( bitmap, mask ); // Ownership transferred
    CleanupStack::Pop( 2 ); // bitmap, mask
    
    return result;
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::CreateButtonL
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CMsgAttachmentControl::CreateButtonL()
    {
    delete iButton;
    iButton = NULL;
    
    CGulIcon* buttonIcon = CGulIcon::NewL( iLabelIcon->Bitmap(), iLabelIcon->Mask() );
    buttonIcon->SetBitmapsOwnedExternally( ETrue ); // bitmaps owned by iLabelIcon
    
    CleanupStack::PushL( buttonIcon );
    
    iButton = CAknButton::NewL( buttonIcon,
                                NULL,
                                NULL,
                                NULL,
                                KNullDesC,
                                KNullDesC,
                                KAknButtonSizeFitText,
                                0 );
        
    CleanupStack::Pop( buttonIcon ); // buttonIcon owned by iButton
    
    iButton->SetIconScaleMode( EAspectRatioPreserved );
    
    iButton->SetObserver( this );
    
    if ( DrawableWindow() )
        {
        iButton->SetContainerWindowL( Window() );
        iButton->ActivateL();
        }
    }
#else
void CMsgAttachmentControl::CreateButtonL()
    {
    }
#endif // RD_SCALABLE_UI_V2


// ---------------------------------------------------------
// CMsgAttachmentControl::DoScrollTextL
// ---------------------------------------------------------
//
TInt CMsgAttachmentControl::DoScrollTextL( TAny* aObject )
    {
    // cast, and call non-static function
    CMsgAttachmentControl* control = static_cast<CMsgAttachmentControl*>( aObject );
    TRAPD( error ,control->ScrollTextL() );
    if ( error != KErrNone )
        {
        control->StopAutomaticScrolling();
        User::Leave( error );
        }
    return KErrNone;
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::ScrollTextL
// ---------------------------------------------------------
//
void CMsgAttachmentControl::ScrollTextL()
    {
    TBool scrollText( ETrue );
    if ( iScrollPos == EScrollPosBeginning )
        {
        SetTextForEditorL( EFalse, EFalse );
        }
    else if ( iScrollPos == EScrollPosEnd )
        {
        // Indicates that text has been scrolled to the end and should be restarted.
        SetTextForEditorL( EFalse, ETrue );
        iScrollPos = EScrollPosBeginning;
        scrollText = EFalse;
        
        // Reset the text view to it's original position.
        iEditor->TextView()->SetLeftTextMargin( 0 );
        DrawNow();
        
        StartAutomaticScrolling( KScrollRestartDelay );
        }
    
    if ( scrollText )
        {
        TInt scrolledPixels = iEditor->TextView()->ScrollDisplayL( TCursorPosition::EFRight );
        if ( scrolledPixels == 0 )
            {
            // Indicates that text has been scrolled to the end. 
            iScrollPos = EScrollPosEnd;
            StartAutomaticScrolling( KScrollEndDelay ); 
            }
        else
            {
            iScrollPos = EScrollPosOther;
            }
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::DisableAutomaticScrolling
// ---------------------------------------------------------
//
void CMsgAttachmentControl::DisableAutomaticScrolling()
    {
    iAutomaticScrolling = EFalse;    
    StopAutomaticScrolling();
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::EnableAutomaticScrolling
// ---------------------------------------------------------
//
void CMsgAttachmentControl::EnableAutomaticScrolling()
    {
    iAutomaticScrolling = ETrue;
    
    if ( !iScrollTimer->IsActive() && 
         IsFocused() )
        {
        StartAutomaticScrolling( KScrollStartDelay );
        }
    }


// ---------------------------------------------------------
// CMsgAttachmentControl::StartAutomaticScrolling
// ---------------------------------------------------------
//
void CMsgAttachmentControl::StartAutomaticScrolling( TInt aStartDelay )
    {
    StopAutomaticScrolling();
    iScrollTimer->Start( aStartDelay,
                         KScrollTickDelay,
                         TCallBack( DoScrollTextL, this ) );
    }
        
// ---------------------------------------------------------
// CMsgAttachmentControl::StopAutomaticScrolling
// ---------------------------------------------------------
//
void CMsgAttachmentControl::StopAutomaticScrolling()
    {
    iScrollTimer->Cancel();
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::ClipTextL
// ---------------------------------------------------------
//
void CMsgAttachmentControl::ClipTextL( TPtr aText, TInt aClipWidth )
    {
    const CFont* font = iEditorLayout.Font();
    
    TInt fits = font->TextCount( aText, aClipWidth );
        
    if ( fits < aText.Length() )
        {
        // Clipping is actually done only if text is at the beginning.
        TInt ellipsisWidth = font->CharWidthInPixels( KEllipsis );

        fits = font->TextCount( aText, aClipWidth - ellipsisWidth );
        
        // Work out the text directionality before the truncation point.
        // First put the text in reverse order and then call
        // TBidiText::TextDirectionality. This effectively tells the
        // directionality of the last strongly directional character
        // in the text.
        HBufC* temp = aText.Left( fits ).AllocL();
        TPtr tempPtr = temp->Des();

        TInt i = 0;
        TInt j = tempPtr.Length() - 1;
        while ( i < j )
            {
            TText t = tempPtr[i];
            tempPtr[i++] = tempPtr[j];
            tempPtr[j--] = t;
            }
        
        TBidiText::TDirectionality dir = TBidiText::TextDirectionality( tempPtr );

        delete temp;

        TText dirMarker = ( dir == TBidiText::ERightToLeft ) ? KRLMarker : 
                                                               KLRMarker;
        
        // Insert zero width joiner if necessary to get the correct glyph form
        // before truncation.
        TText lastChar = aText[fits - 1];
        TText next = aText[fits];
        
        TInt ellipsisPos = fits;
        
        if ( CFont::CharactersJoin( lastChar, KZeroWidthJoiner ) &&
             CFont::CharactersJoin( lastChar, next ) )
            {
            aText[fits] = KZeroWidthJoiner;
            ellipsisPos++;
            }
            
        aText.SetMax();
        
        aText[ellipsisPos] = KEllipsis;    

        // Inserting direction marker after the ellipsis ensures that
        // the ellipsis is shown on the correct side of the text
        // before it.
        aText[ellipsisPos + 1] = dirMarker;

        aText.SetLength( ellipsisPos + 2 );
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::UpdateFocusL
// ---------------------------------------------------------
//
void CMsgAttachmentControl::UpdateFocusL()
    {
    TCharFormat charFormat;
    TCharFormatMask applyMask;

    iEditor->RichText()->GetCharFormat( charFormat,
                                        applyMask,
                                        0,
                                        iEditor->TextLength() );
            
    if ( !IsFocused() )
        {
        TRgb textColor( KRgbBlack );
        AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), 
                                   textColor, 
                                   KAknsIIDQsnTextColors, 
                                   EAknsCIQsnTextColorsCG6 );
                                   
        charFormat.iFontPresentation.iTextColor = textColor;
        applyMask.SetAttrib( EAttColor );               
        
        charFormat.iFontPresentation.iHighlightStyle = TFontPresentation::EFontHighlightNone;
        applyMask.SetAttrib( EAttFontHighlightStyle );
        }
    else
        {
        TRgb textColor( KRgbWhite );
        AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), 
                                   textColor, 
                                   KAknsIIDQsnHighlightColors, 
                                   EAknsCIQsnTextColorsCG3 );
                                   
        charFormat.iFontPresentation.iTextColor = textColor;
        applyMask.SetAttrib( EAttColor );               
           
        TRgb highlightColor( KRgbBlue );
        AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), 
                                   highlightColor, 
                                   KAknsIIDQsnHighlightColors, 
                                   EAknsCIQsnHighlightColorsCG1 );
                                   
        charFormat.iFontPresentation.iHighlightColor = highlightColor;
        applyMask.SetAttrib( EAttFontHighlightColor );

        charFormat.iFontPresentation.iHighlightStyle = TFontPresentation::EFontHighlightNormal;
        applyMask.SetAttrib( EAttFontHighlightStyle );
        }
    
    iEditor->RichText()->ApplyCharFormatL( charFormat,
                                           applyMask,
                                           0,
                                           iEditor->TextLength() );    
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::DoHandleResourceChangeBeforeL
//
// Contains functionality that base class handle resource change 
// is depending on to be completed before it can be executed properly.
// ---------------------------------------------------------
//
void CMsgAttachmentControl::DoHandleResourceChangeBeforeL( TInt aType )
    {
    switch( aType )
        {
        case KEikDynamicLayoutVariantSwitch:
            {
#ifdef RD_SCALABLE_UI_V2
            if ( AknLayoutUtils::PenEnabled() )
                {
                if ( !iButton )
                    {
                    CreateButtonL();
                    }
                }
            else
                {
                delete iButton;
                iButton = NULL;
                }
#endif // RD_SCALABLE_UI_V2
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::DoHandleResourceChangeAfterL
//
// Contains functionality that is dependant of base class handle
// resource change to complete before these can be executed properly.
// ---------------------------------------------------------
//
void CMsgAttachmentControl::DoHandleResourceChangeAfterL( TInt aType )
    {
    switch( aType )
        {
        case KEikDynamicLayoutVariantSwitch:
            {
            StopAutomaticScrolling();
            iScrollPos = EScrollPosBeginning;
            SetTextForEditorL( EFalse, ETrue );  
            
            break;
            }
        case KAknsMessageSkinChange:
            {
            CGulIcon* icon = CreateAttachmentIconsL();
            CleanupStack::PushL( icon );
            
            if ( iButton )
                {
                CGulIcon* buttonIcon = CGulIcon::NewL( icon->Bitmap(), icon->Mask() );
                buttonIcon->SetBitmapsOwnedExternally( ETrue ); // bitmaps owned by iLabelIcon
    
                iButton->State()->SetIcon( buttonIcon ); // buttonIcon ownership transferred
                }
            
            // Make the change here as button will take the old size from
            // old icons that must be still valid when setting to button icon.
            delete iLabelIcon;
            iLabelIcon = icon;
            
            CleanupStack::Pop( icon );
            
            ResolveLayoutsL();
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::AddToAttachmentArrayL
// ---------------------------------------------------------
//
void CMsgAttachmentControl::AddToAttachmentArrayL( const TDesC& aName, TInt aIndex )
    {
    HBufC* convertedFileName = 
                AknTextUtils::ConvertFileNameL( aName );
    CleanupStack::PushL( convertedFileName );
    
    TPtr convertedFileNamePtr = convertedFileName->Des();
    
    AknTextUtils::StripCharacters( convertedFileNamePtr, KStripList );
    
    iAttachmentArray->InsertL( aIndex, *convertedFileName );
    
    CleanupStack::PopAndDestroy( convertedFileName );
    }

// ---------------------------------------------------------
// CMsgAttachmentControl::LayoutButton
// ---------------------------------------------------------
//    
void CMsgAttachmentControl::LayoutButton()
    {
#ifdef RD_SCALABLE_UI_V2
    CMsgExpandableControl::LayoutButton();
    
    if ( iButton )
        {
        TMargins8 margins;
        TRect buttonRect = iButtonLayout.Rect();
        TRect iconRect = iIconLayout.Rect();
        margins.iTop = iconRect.iTl.iY - buttonRect.iTl.iY;
        margins.iBottom = buttonRect.iBr.iY - iconRect.iBr.iY;
        margins.iLeft = iconRect.iTl.iX - buttonRect.iTl.iX;
        margins.iRight = buttonRect.iBr.iX - iconRect.iBr.iX;
        
        iButton->SetMargins( margins );
        }
#endif // RD_SCALABLE_UI_V2
    }


//  End of File
