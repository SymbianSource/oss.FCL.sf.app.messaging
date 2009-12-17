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
* Description:  MsgBodyControlEditor implementation
*
*/



// ========== INCLUDE FILES ================================

#include <eikenv.h>                        // for CEikonEnv
#include <txtrich.h>                       // for CRichText
#include <baclipb.h>                       // for CClipboard
#include <AknUtils.h>                      // for AknUtils
#include <ItemFinder.h>                    // for automatic highlight

#include <applayout.cdl.h> // LAF
#include <aknlayoutscalable_apps.cdl.h>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif 

#include "MsgEditorCommon.h"               //
#include "MsgBodyControlEditor.h"          // for CMsgBodyControlEditor
#include "MsgBaseControl.h"                // for TMsgEditPermissionFlags
#include "MsgBaseControlObserver.h"        // for MMsgBaseControlObserver
#include "MsgEditorCustomDraw.h"           // for CMsgEditorCustomDraw

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

const TInt KBodyFullFormattingLength = 500;

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgBodyControlEditor::CMsgBodyControlEditor
//
// Constructor.
// ---------------------------------------------------------
//
CMsgBodyControlEditor::CMsgBodyControlEditor( const CCoeControl* aParent,
                                              TUint32& aFlags,
                                              MMsgBaseControlObserver* aBaseControlObserver ) :
    CMsgExpandableControlEditor( aParent, aFlags, aBaseControlObserver ),
    iPreviousItemStart( -1 ),
    iPreviousItemLength( -1 )
    {
    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::~CMsgBodyControlEditor
//
// Destructor.
// ---------------------------------------------------------
//
CMsgBodyControlEditor::~CMsgBodyControlEditor()
    {
 
    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::ConstructL
//
//
// ---------------------------------------------------------
//
void CMsgBodyControlEditor::ConstructL()
    {
    TInt edwinFlags = ENoHorizScrolling | 
                      ENoAutoSelection | 
                      EWidthInPixels | 
                      EAllowUndo | 
                      EResizable |
                      EPasteAsPlainText;

    iFlags |= EMsgControlModeDoNotDrawFrameBgContext;
    ResolveLayouts();
    
    /*if ( iBaseControlObserver &&
         !iBaseControlObserver->ViewInitialized() )
        {
        SetSuppressFormatting( ETrue );
        }*/
    
    iItemFinder = CItemFinder::NewL(  CItemFinder::EUrlAddress | 
                                      CItemFinder::EEmailAddress | 
                                      CItemFinder::EPhoneNumber | 
                                      CItemFinder::EUriScheme );

    iItemFinder->AddObserver( *this );
    iFocusChangedBeforeParseFinish = EFalse;

    CEikRichTextEditor::ConstructL( iParentControl, 0, iMaxNumberOfChars, edwinFlags );
    // 107-24185 : Emoticon support for SMS and MMS
    AddFlagToUserFlags(EAvkonEnableSmileySupport);
    
    // Both global text editor and edwin have it's own formatting layers.
    // CEikRichTextEditor::ConstructL call will set global formatting layers
    // into use for text. This is not correct as edwin formatting layers
    // will hold correct formatting values. Below calls will set edwin
    // formatting layers into use and prevent client based formatting from
    // being based on incorrect formatting layer.
    SetParaFormatLayer( iEikonEnv->SystemParaFormatLayerL()->CloneL() );
    SetCharFormatLayer( iEikonEnv->SystemCharFormatLayerL()->CloneL() );
    
    SetUpperFullFormattingLength( KBodyFullFormattingLength );
    
    SetMaximumHeight( MsgEditorCommons::MaxBodyHeight() );
    
    // Set the wrap width to be that of the current body text pane. 
    // This will make the formatting of text safer if it is performed 
    // before the editor has ever had SetSize() called. WrapWidth is 
    // usually set in HandleSizeChanged()
    if ( iLayout )
        {
        iLayout->SetWrapWidth( iTextLayout.TextRect().Width() );
        }
    
    TRect dataPane = MsgEditorCommons::MsgDataPane();
    
    TMargins8 edwinMargins;
    edwinMargins.iTop = 0;
    edwinMargins.iBottom = 0;
    edwinMargins.iLeft = (TInt8)( iTextLayout.TextRect().iTl.iX - dataPane.iTl.iX );
    edwinMargins.iRight = (TInt8)( dataPane.iBr.iX - iTextLayout.TextRect().iBr.iX );
    SetBorderViewMargins( edwinMargins );
    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::SetAndGetSizeL
//
// Calculates and sets the size of the control and returns new size as
// reference aSize.
// ---------------------------------------------------------
//
void CMsgBodyControlEditor::SetAndGetSizeL( TSize& aSize )
    {
    // This may change the heigh if the content has changed
    SetSize( aSize );
    aSize = iSize;
    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::InsertCharacterL
//
// Inserts a character to the editor.
// ---------------------------------------------------------
//
void CMsgBodyControlEditor::InsertCharacterL( const TChar& aCharacter )
    {
    	
    // 107-24185 : Emoticon support for SMS and MMS
    
    TBuf<1> text;
    text.Append( aCharacter );
    InsertTextL( text );

    //UpdateScrollBarsL();
    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::InsertTextL
//
// Inserts text to the editor.
// ---------------------------------------------------------
//
void CMsgBodyControlEditor::InsertTextL( const TDesC& aText )
    {
    if ( iTextView )
        {
        TCursorSelection selection = iTextView->Selection();
        const TInt selLength = selection.Length();
        const TInt lower = selection.LowerPos();

        if ( selLength )
            {
            // 107-24185 : Emoticon support for SMS and MMS, symbian call
            InsertDeleteCharsL( lower, aText, selection );
            }
        else
            {
            InsertDeleteCharsL( selection.iCursorPos, aText, selection );
            }

        SetCursorPosL( lower + aText.Length(), EFalse );

        NotifyNewFormatL();
        }
    else
        {
        iText->InsertL( 0, aText );
        }
    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::ActivateL
//
// This function is needed for overriding
// CMsgExpandableControlEditor::ActivateL().
// ---------------------------------------------------------
//
void CMsgBodyControlEditor::ActivateL()
    {
    if ( IsActivated() )
        {
        return;
        }
        
    CMsgExpandableControlEditor::ActivateL();      
    
    }


// ---------------------------------------------------------
// CMsgBodyControlEditor::OfferKeyEventL
//
//
// ---------------------------------------------------------
//
TKeyResponse CMsgBodyControlEditor::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( IsReadOnly() )
        {
        switch ( aKeyEvent.iCode )
            {
            case EKeyUpArrow:
            case EKeyDownArrow:
                break;

            default:
                return CEikRichTextEditor::OfferKeyEventL( aKeyEvent, aType );
            }

        SetHighlightL( CursorPos(), 
            ( aKeyEvent.iCode == EKeyUpArrow )
            ? EMsgFocusUp
            : EMsgFocusDown );
        return EKeyWasConsumed;
        }
    else
        {
        return CEikRichTextEditor::OfferKeyEventL( aKeyEvent, aType );
        }
    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::HandlePointerEventL
//
// Handles pointer events. 
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CMsgBodyControlEditor::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    TBool forwardRequest( ETrue );
    
    if ( IsActivated() &&
         IsReadOnly() && 
         ( aPointerEvent.iType == TPointerEvent::EButton1Down ||
           aPointerEvent.iType == TPointerEvent::EButton1Up ) )
        {
        TPoint tapPoint( aPointerEvent.iPosition );
        TInt docPos( TextView()->XyPosToDocPosL( tapPoint ) );
        
        TInt start( 0 );
        TInt length( 0 );
        MParser* parser = iItemFinder;
        
        TBool tappedOverTag( RichText()->CursorOverTag( docPos, parser, start, length ) );
            
        if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
            {
            iPreviousItemStart = -1;
            iPreviousItemLength = -1;
            
            if ( tappedOverTag )
                {
                TPoint relativeTapPoint( aPointerEvent.iPosition - iPosition );
                if ( iItemFinder->ItemWasTappedL( relativeTapPoint ) )
                    {                
                    iPreviousItemStart = start;
                    iPreviousItemLength = length;
                    
                    forwardRequest = EFalse;
                    }
                }
            else
                {
                iItemFinder->ResetCurrentItem();
                }
            }
        else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
            {
            if ( tappedOverTag &&
                 start >= iPreviousItemStart &&
                 length <= iPreviousItemLength )
                {
                TKeyEvent event;
                event.iCode = EKeyDevice3;
                event.iScanCode = EStdKeyDevice3;
                event.iModifiers = 0;
                event.iRepeats = 0;
                
                iCoeEnv->WsSession().SimulateKeyEvent( event );
                
                forwardRequest = EFalse;
                }
            }
        }

    if ( forwardRequest )
        {
        CMsgExpandableControlEditor::HandlePointerEventL( aPointerEvent );
        }                
#ifdef RD_TACTILE_FEEDBACK         
    else if(aPointerEvent.iType == TPointerEvent::EButton1Down)
        {                                
        MTouchFeedback* feedback = MTouchFeedback::Instance();
        if ( feedback )
            {
            feedback->InstantFeedback( this, ETouchFeedbackBasic );
            }
        }
#endif //  RD_TACTILE_FEEDBACK      
    }
#else
void CMsgBodyControlEditor::HandlePointerEventL( const TPointerEvent& /*aPointerEvent*/ )
    {
    }
#endif // RD_SCALABLE_UI_V2


// ---------------------------------------------------------
// CMsgBodyControlEditor::IsFocusChangePossibleL
//
//
// ---------------------------------------------------------
//
TBool CMsgBodyControlEditor::IsFocusChangePossibleL(
    TMsgFocusDirection aDirection )
    {
    CItemFinder::TFindDirection direction = aDirection == EMsgFocusUp
        ? CItemFinder::ENextUp
        : CItemFinder::ENextDown;

    TInt cursorPos = CursorPos();
    iItemFinder->ResolveAndSetItemTypeL( cursorPos );
    TInt nextItem = iItemFinder->PositionOfNextItem( direction );
    TPoint dummy;

    if ( nextItem < KErrNone )
        {
        // Next item not found.
        if ( aDirection == EMsgFocusUp )
            {
            return TextLayout()->FirstDocPosFullyInBand() == 0;
            }
        else
            {
            return TextLayout()->PosInBand( TextLength(),dummy );
            }
        }
    else
        {
        // Next item found. -> Focus change not allowed.
        return 0;
        }
    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::SetupAutomaticFindAfterFocusChangeL
//
//
// ---------------------------------------------------------
//
void CMsgBodyControlEditor::SetupAutomaticFindAfterFocusChangeL( TBool aBeginning )
    {
    iInitTop = aBeginning;
    if ( !iTextParsed )
        {
        iFocusChangedBeforeParseFinish = ETrue;
        }
    else
        {
        if ( aBeginning )
            {
            SetHighlightL( 0, EMsgFocusDown, ETrue ); 
            }
        else
            {
            SetHighlightL( TextLength(), EMsgFocusUp, ETrue ); 
            }
        }
    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::SetHighlightL
//
//
// ---------------------------------------------------------
//
void CMsgBodyControlEditor::SetHighlightL(
    TInt aCursorPos, TMsgFocusDirection aDirection,
    TBool aInit /*= EFalse*/ )
    {
    // Use "link highlight"
    SetHighlightStyleL( EEikEdwinHighlightLink );
    TInt textLength = TextLength();

    TCursorPosition::TMovementType movetype = aDirection == EMsgFocusUp
        ? TCursorPosition::EFPageUp
        : TCursorPosition::EFPageDown;

    CItemFinder::TFindDirection direction = aDirection == EMsgFocusUp
        ? CItemFinder::ENextUp
        : CItemFinder::ENextDown;

    TInt nextItem = KErrNotFound;
    TPoint dummy;
    if ( iTextParsed )
        {
        if ( aInit )
            {
            iItemFinder->NextItemOrScrollL( aCursorPos == 0
                ? CItemFinder::EInitDown
                : CItemFinder::EInitUp );
            }
        else
            {
            nextItem = iItemFinder->PositionOfNextItem( direction );
            }
        }

    if ( nextItem > KErrNotFound )
        {
        if ( !aInit &&
            !TextLayout()->PosInBand( nextItem ,dummy ) )
            {
            // first move one page and then set selection
            SetCursorPosL( CursorPos(), EFalse );
            iTextView->MoveCursorL( movetype, EFalse );
            }

        if ( TextLayout()->PosInBand( nextItem ,dummy ) )
            {
            // next item is visible, just change selection
            iItemFinder->ResolveAndSetItemTypeL( nextItem );

            TInt selectionLength = iItemFinder->CurrentItemExt().iEnd - iItemFinder->CurrentItemExt().iStart + 1;
            TCursorSelection cursorSelection;
            cursorSelection.iCursorPos = nextItem;
            cursorSelection.iAnchorPos = nextItem + selectionLength;
            if ( !TextLayout()->PosInBand( nextItem + selectionLength, dummy ) )
                {
                // Next item does not fit fully on the screen.
                // Have to move page first.
                SetCursorPosL( CursorPos(), EFalse );
                iTextView->MoveCursorL( movetype, EFalse );
                }

            iTextView->SetSelectionL( cursorSelection );
            }

        }
    else
        {
        if ( !aInit )
            {
            switch ( aDirection )
                {
                case EMsgFocusDown:
                    if ( !TextLayout()->PosInBand( textLength, dummy) )
                        {
                        iTextView->MoveCursorL( movetype, EFalse );

                        // Following is a bit tricky since MoveCursor looses
                        // selection so we have to set it again if previous
                        // selection still on the screen.
                        nextItem = iItemFinder->PositionOfNextItem(
                            CItemFinder::ENextUp );
                        if ( nextItem > KErrNotFound )
                            {
                            if ( TextLayout()->PosInBand( nextItem ,dummy ) )
                                {
                                // next item is visible, change selection
                                iItemFinder->ResolveAndSetItemTypeL( nextItem );

                                TInt selectionLength = iItemFinder->CurrentItemExt().iEnd - iItemFinder->CurrentItemExt().iStart + 1;
                                TCursorSelection cursorSelection;
                                cursorSelection.iCursorPos = nextItem;
                                cursorSelection.iAnchorPos = nextItem + selectionLength;
                                iTextView->SetSelectionL( cursorSelection );
                                }
                            else
                                {
                                nextItem = iItemFinder->PositionOfNextItem(
                                    CItemFinder::ENextDown );
                                }
                            }
                        }
                    break;
                default:
                case EMsgFocusUp:
                    if ( !TextLayout()->PosInBand( 0,dummy ) )
                        {
                        iTextView->MoveCursorL( movetype, EFalse );
                        }
                    break;
                };

            }
        }

    }


// ---------------------------------------------------------
// CMsgBodyControlEditor::CursorInLastLine
//
//
// ---------------------------------------------------------
//
TBool CMsgBodyControlEditor::CursorInLastLine()
    {
    if (IsReadOnly())
        {
        TPoint dummy;
        return ( iItemFinder->PositionOfNextItem( CItemFinder::ENextDown ) == KErrNotFound  
            && TextLayout()->PosInBand( TextLength(), dummy ) );

        }
    else
        {
        return CMsgExpandableControlEditor::CursorInLastLine();
        }

    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::HandleParsingComplete
// ---------------------------------------------------------
//
void CMsgBodyControlEditor::HandleParsingComplete()
    {
    TRAP_IGNORE( DoHandleParsingCompleteL() );
    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::DoHandleParsingCompleteL
// ---------------------------------------------------------
//
void CMsgBodyControlEditor::DoHandleParsingCompleteL()
    {
    iTextParsed = ETrue;
    // this needs to be changed so that bottom init can also be set
    if ( iFocusChangedBeforeParseFinish )
        {
        iFocusChangedBeforeParseFinish = EFalse; // just to make sure this is done only once
        if ( iInitTop )
            {
            SetHighlightL( 0, EMsgFocusDown, ETrue ); 
            }
        else
            {
            SetHighlightL( TextLength(), EMsgFocusUp, ETrue ); 
            }
        
        if ( iBaseControlObserver )
            {
            iBaseControlObserver->HandleBaseControlEventRequestL( NULL, 
                                                                  EMsgUpdateScrollbar );
            }
        }
    else
        {
        // In some cases the text view is left to some
        // strange state. Make sure the view is in sync
        // with cursor position.
        SetCursorPosL( CursorPos(), EFalse );
        }
    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::ResolveLayouts
//
//
// ---------------------------------------------------------
//
void CMsgBodyControlEditor::ResolveLayouts()
    {
    TRect dataPane = MsgEditorCommons::MsgDataPane();
    
    TAknLayoutRect msgTextPane;
    msgTextPane.LayoutRect(
        dataPane,
        AknLayoutScalable_Apps::msg_text_pane( 0 ).LayoutLine() );
    TAknLayoutRect msgBodyPane;
    msgBodyPane.LayoutRect(
            msgTextPane.Rect(),
            AknLayoutScalable_Apps::msg_body_pane().LayoutLine() );

    if ( IsReadOnly() )
        {
        iTextLayout.LayoutText(
            msgBodyPane.Rect(),
            AknLayoutScalable_Apps::msg_body_pane_t1( 0 ).LayoutLine() );
        }
    else
        {
        iTextLayout.LayoutText(
            msgBodyPane.Rect(),
            AknLayoutScalable_Apps::msg_body_pane_t1( 1 ).LayoutLine() );            
        }

    
    if ( MsgEditorCustomDraw() )
        {
        MsgEditorCustomDraw()->ResolveLayouts();
        }
    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::LayoutEdwin
// ---------------------------------------------------------
//
void CMsgBodyControlEditor::LayoutEdwin()
    {
    TAknLayoutRect msgTextPane;
    msgTextPane.LayoutRect( MsgEditorCommons::MsgDataPane(),
                            AknLayoutScalable_Apps::msg_text_pane( 0 ).LayoutLine() );
                            
    TAknLayoutRect msgBodyPane;
    msgBodyPane.LayoutRect( msgTextPane.Rect(),
                            AknLayoutScalable_Apps::msg_body_pane().LayoutLine() );
    
    TRect parentRect( msgBodyPane.Rect() );
    parentRect.Move( 0, iParentControl->Position().iY );
    
    DoLayoutEdwin( parentRect, 
                   AknLayoutScalable_Apps::msg_body_pane_t1( 0 ).LayoutLine() );
    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::PrepareFocusTransition
//
// Do not change focus when automatic find parsing has been finished
// if focus transition out of this control has been performed.
// ---------------------------------------------------------
//
void CMsgBodyControlEditor::PrepareFocusTransition()
    {
    if ( IsFocused() )
        {
        iFocusChangedBeforeParseFinish = EFalse;
        }
    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::SetTextSkinColorIdL
// ---------------------------------------------------------
//
void CMsgBodyControlEditor::SetTextSkinColorIdL()
    {
    CEikEdwin::SetTextSkinColorIdL( EAknsCIQsnTextColorsCG6 );
    }

// ---------------------------------------------------------
// CMsgBodyControlEditor::HandleResourceChange
//
// Sets correct highlight extension.
// ---------------------------------------------------------
//
void CMsgBodyControlEditor::HandleResourceChange( TInt aType )
    {
    CMsgExpandableControlEditor::HandleResourceChange( aType );
    
    if ( aType == KAknsMessageSkinChange &&  MsgEditorCustomDraw() )
        {
         MsgEditorCustomDraw()->SkinChanged();;    
    	}
    }

//  End of File
