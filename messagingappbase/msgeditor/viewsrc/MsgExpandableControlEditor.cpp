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
* Description:  MsgExpandableControlEditor implementation
*
*/



// ========== INCLUDE FILES ================================

#include <eikappui.h>                      // for CEikAppUi
#include <eikenv.h>                        // for CEikonEnv
#include <txtrich.h>                       // for CRichText
#include <baclipb.h>                       // for CClipboard
#include <gulfont.h>                       // for TLogicalFont
#include <AknUtils.h>                      // for AknUtils
#include <barsread.h>                      // for TResourceReader
#include <ItemFinder.h>                    // for automatic highlight

#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknSettingCache.h>
#include <AknsConstants.h>
#include <applayout.cdl.h> // LAF
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknsFrameBackgroundControlContext.h> // FrameBackgroundControlContext
#include <AknsDrawUtils.h>

#include "MsgEditorCommon.h"               //
#include "MsgExpandableControlEditor.h"    // for CMsgExpandableControlEditor
#include "MsgExpandableControl.h"          // for CMsgExpandableControl
#include "MsgBaseControl.h"                // for TMsgEditPermissionFlags
#include "MsgBaseControlObserver.h"        // for MMsgBaseControlObserver
#include "MsgEditorPanic.h"                // for MsgEditor panics
#include "MsgEditorLogging.h"
#include "MsgEditorCustomDraw.h"           // for CMsgEditorCustomDraw

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================
// OPEN:
const TInt KEdwinTopMargin    = 0;
const TInt KEdwinBottomMargin = 0;
const TInt KEdwinLeftMargin   = 0;
const TInt KEdwinRightMargin  = 0;
const TInt KExpFullFormattingLength = 500;

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgExpandableControlEditor::CMsgExpandableControlEditor
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgExpandableControlEditor::CMsgExpandableControlEditor(
    TUint32&                 aFlags,
    MMsgBaseControlObserver* aBaseControlObserver)
    :
    CEikRichTextEditor( TGulBorder::ENone ),
    iFlags( aFlags ),
    iBaseControlObserver( aBaseControlObserver )
    {
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::CMsgExpandableControlEditor
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgExpandableControlEditor::CMsgExpandableControlEditor(
    const CCoeControl*       aParent,
    TUint32&                 aFlags,
    MMsgBaseControlObserver* aBaseControlObserver )
    :
    CEikRichTextEditor( TGulBorder::ENone ),
    iParentControl( aParent ),
    iFlags( aFlags ),
    iBaseControlObserver( aBaseControlObserver )
    {
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::~CMsgExpandableControlEditor
//
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgExpandableControlEditor::~CMsgExpandableControlEditor()
    {
    delete iItemFinder;
    delete iFrameBgContext;
    delete iCustomDraw;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::ConstructL
//
// Constructs and initializes this editor.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControlEditor::ConstructL()
    {
    TInt edwinFlags = ENoHorizScrolling | 
                      ENoAutoSelection | 
                      EWidthInPixels | 
                      EAllowUndo | 
                      EResizable | 
                      EPasteAsPlainText;

    iFlags = EMsgControlModeDoNotDrawFrameBgContext; 
    ResolveLayouts();
    
    /*if ( iBaseControlObserver &&
         !iBaseControlObserver->ViewInitialized() )
        {
        SetSuppressFormatting( ETrue );
        }*/
    
    TInt numberOfLines( 0 );

    if ( iFlags & EMsgControlModeSizeFixedToOneLine )
        {
        // make control fixed size and allow horizontal scrolling.
        edwinFlags &= ~EResizable;
        edwinFlags &= ~ENoHorizScrolling;
        numberOfLines = 1;
        }

    CEikRichTextEditor::ConstructL(
        iParentControl,
        numberOfLines,
        iMaxNumberOfChars,
        edwinFlags );
    
    // Emoticon support for MMS Subject field
    AddFlagToUserFlags(EAvkonEnableSmileySupport);
    
    // Both global text editor and edwin have it's own formatting layers.
    // CEikRichTextEditor::ConstructL call will set global formatting layers
    // into use for text. This is not correct as edwin formatting layers
    // will hold correct formatting values. Below calls will set edwin
    // formatting layers into use and prevent client based formatting from
    // being based on incorrect formatting layer.
    SetParaFormatLayer( iEikonEnv->SystemParaFormatLayerL()->CloneL() );
    SetCharFormatLayer( iEikonEnv->SystemCharFormatLayerL()->CloneL() );
    
    SetUpperFullFormattingLength( KExpFullFormattingLength );
    
    SetMaximumHeight( MsgEditorCommons::MaxBodyHeight() );
    
    // Set the wrap width to be that of the current header text pane. 
    // This will make the formatting of text safer if it is performed 
    // before the editor has ever had SetSize() called. WrapWidth is 
    // usually set in HandleSizeChanged()
    if ( iLayout )
        {
        iLayout->SetWrapWidth( iTextLayout.TextRect().Width() );
        }
        
    // TODO: Get from LAF or something... Currently 
    // all of these are zero. OK?!
    TMargins8 edwinMargins;
    edwinMargins.iTop = KEdwinTopMargin;
    edwinMargins.iBottom = KEdwinBottomMargin;
    edwinMargins.iLeft = KEdwinLeftMargin;
    edwinMargins.iRight = KEdwinRightMargin;
    SetBorderViewMargins( edwinMargins );

    iBgContext = AknsDrawUtils::ControlContext( this );
    
    // NOTE: iItemFinder can be created by derived class if needed.
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::ConstructFromResourceL
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControlEditor::ConstructFromResourceL(
    TResourceReader& aReader )
    {
    // This used to be font id. Not used anymore. Have to read it, though.
    /*TInt fontId =*/ aReader.ReadInt32();

    ConstructL();

    if ( !IsReadOnly() )
        {
        ReadAknResourceL( aReader );
        }
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::SetAndGetSizeL
//
// Sets size for this editor. Height of given size aSize may change if the
// editor contains text.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControlEditor::SetAndGetSizeL( TSize& aSize )
    {
    TSize controlSize( aSize.iWidth, aSize.iHeight );
    
    if( TextLayout() && TextLayout()->IsFormattingBand() )
        { 
        // Partial formatting on-going -> needs whole body height
        //controlSize.iHeight = MaximumHeight();
        }   
    else
        {
        TInt position( 0 );
        AknLayoutUtils::GetEdwinVerticalPositionAndHeightFromLines( 
                                                controlSize.iHeight,
                                                AknLayoutScalable_Apps::msg_header_pane_t2( 0 ).LayoutLine(),
                                                MsgEditorCommons::MsgBaseLineDelta(),
                                                iNumberOfLines,
                                                position,
                                                controlSize.iHeight );
        }    
    controlSize.iHeight +=iEditortop;
    if ( controlSize != iSize )
        {
        // This may change the height if the content has changed.
        SetSize( controlSize );
        }
    
    aSize = controlSize;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::CheckEditPermission
//
// Checks and returns control's edit permissions.
// ---------------------------------------------------------
//
EXPORT_C TUint32 CMsgExpandableControlEditor::CheckEditPermission() const
    {
    TUint32 editPermission( CMsgBaseControl::EMsgEditNone );

    if ( CanUndo() )
        {
        editPermission |= CMsgBaseControl::EMsgEditUndo;
        }

    if ( Selection().Length() )
        {
        // Selection is on
        editPermission |= CMsgBaseControl::EMsgEditCopy;
        if ( !IsReadOnly() )
            {
            editPermission |= CMsgBaseControl::EMsgEditCut;
            }
        if ( Selection().Length() == TextLength() )
            {
            editPermission |= CMsgBaseControl::EMsgEditUnSelectAll;
            }
        }

    if ( TextLength() )
        {
        editPermission |= CMsgBaseControl::EMsgEditSelectAll;
        }

    editPermission |= CMsgBaseControl::EMsgEditPaste;

    return editPermission;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::CursorInFirstLine
//
// Checks if the cursor is on the first line and return ETrue if it is.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgExpandableControlEditor::CursorInFirstLine()
    {
    TBool cursorOnFirstLine = EFalse;
    TBool posFormatted = iLayout->PosIsFormatted( 0 );
    if ( iTextView && posFormatted )
        {
        TTmDocPos docPos;
        iTextView->GetCursorPos( docPos );
        TTmDocPosSpec docPosSpec( docPos );
        TTmPosInfo2 posInfo;
        TTmLineInfo lineInfo;
        if ( iLayout->FindDocPos( docPosSpec, posInfo, &lineInfo ) )
            {
            cursorOnFirstLine = ( lineInfo.iLineNumber == 0 );
            }
        }
    return cursorOnFirstLine;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::CursorInLastLine
//
// Checks if the cursor is on the last line and return ETrue if it is.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgExpandableControlEditor::CursorInLastLine()
    {
    TBool cursorOnLastLine = EFalse;
    TBool posFormatted = iLayout->PosIsFormatted( TextLength() );

    if ( iTextView && posFormatted )
        {
        TInt numOfLines = iLayout->NumFormattedLines();
        if ( numOfLines )
            {
            TTmDocPos docPos;
            iTextView->GetCursorPos( docPos );
            TTmDocPosSpec docPosSpec( docPos );
            TTmPosInfo2 posInfo;
            TTmLineInfo lineInfo;
            if ( iLayout->FindDocPos( docPosSpec, posInfo, &lineInfo ) )
                {
                cursorOnLastLine = ( lineInfo.iLineNumber == numOfLines - 1 );
                }
            }
        else
            {
            // Editor is empty.
            cursorOnLastLine = ETrue;
            }
        }

    return cursorOnLastLine;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::IsFirstLineVisible
//
// Checks if first text line is visible.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgExpandableControlEditor::IsFirstLineVisible()
    {
    if( TextLayout() )
        {
        TPoint dummy;
        if( TextLayout()->PosInBand( 0, dummy ) )
            {
            if( iTextView )
                {
                TTmDocPos docPos( 0, ETrue );
                TTmDocPosSpec docPosSpec( docPos );
                TTmPosInfo2 posInfo;
                TTmLineInfo lineInfo;
                
                if ( iLayout->FindDocPos( docPosSpec, posInfo, &lineInfo ) )
                    {
                    // posInfo.iEdge specifies coordinate position of
                    // intersection of the character edge with the baseline.
                    TInt fontTop = posInfo.iEdge.iY - TextFont()->AscentInPixels();
                    return ( fontTop >= 0 && fontTop <= MsgEditorCommons::EditorViewHeigth() );
                    }
                }
            else
                {
                return ETrue;
                }
            }
        return EFalse;
        }

    return ETrue;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::IsLastLineVisible
//
// Checks if last text line is visible.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgExpandableControlEditor::IsLastLineVisible()
    {
    if( TextLayout() )
        {
        TPoint dummy;
        if( TextLayout()->PosInBand( TextLength(),dummy ) )
            {
            if ( iTextView )
                {
                TTmDocPos docPos( TextLength(), EFalse );
                TTmDocPosSpec docPosSpec( docPos );
                TTmPosInfo2 posInfo;
                TTmLineInfo lineInfo;
                
                if ( iLayout->FindDocPos( docPosSpec, posInfo, &lineInfo ) )
                    {
                    return ( posInfo.iEdge.iY >= 0 && 
                             posInfo.iEdge.iY <= MsgEditorCommons::EditorViewHeigth() );
                    }
                }
            else
                {
                return ETrue;
                }
            }
        return EFalse;
        }

    return ETrue;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::CurrentLineRect
//
// Returns the current control rect.
// ---------------------------------------------------------
//
EXPORT_C TRect CMsgExpandableControlEditor::CurrentLineRectL()
    {
    if ( !iTextView )
        {
        User::Leave( KErrGeneral );
        }

    TTmDocPos docPos;
    iTextView->GetCursorPos( docPos );
    iTextView->FinishBackgroundFormattingL();

    TInt cursorPos = Min( docPos.iPos, TextLength() );

    if ( !iLayout->PosIsFormatted( cursorPos ) )
        {
        iTextView->SetDocPosL( cursorPos, Selection().Length() );  // after this cursorPos should be formatted
        ForceScrollBarUpdateL();
        }

    TRect cursorPosRect;

    if ( iTextView->IsPictureFrameSelected( cursorPosRect, cursorPos ) )
        {
        cursorPosRect.Move( TPoint( 0, -iTextView->ViewRect().iTl.iY ) );
        }
    else
        {
        TTmDocPosSpec docPosSpec( docPos );
        TTmPosInfo2 posInfo;
        TTmLineInfo lineInfo;
        if ( iLayout->FindDocPos( docPosSpec, posInfo, &lineInfo ) )
            {
            cursorPosRect = lineInfo.iInnerRect;
            }
        //TODO: else what?! We should never get to else branch...
        }

    return cursorPosRect;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::DefaultHeight
//
// Returns a default one line height of the editor.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgExpandableControlEditor::DefaultHeight() const
    {
    return DefaultHeight( EFalse );
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::ClipboardL
//
// Handles a clipboard operation.
// ---------------------------------------------------------
//
void CMsgExpandableControlEditor::ClipboardL(
    CEikEdwin::TClipboardFunc aClipboardFunc )
    {
    __ASSERT_DEBUG( iTextView, Panic( EMsgNoTextView ) );

    CEikRichTextEditor::ClipboardL( aClipboardFunc );
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::CcpuCanCut
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgExpandableControlEditor::CcpuCanCut() const
    {
    TUint32 editPermission = CheckEditPermission();

    if ( iBaseControlObserver )
        {
        TRAP_IGNORE( 
            {
            if ( iBaseControlObserver->HandleEditObserverEventRequestL( this, EMsgDenyCut ) )
                {
                editPermission &= ~CMsgBaseControl::EMsgEditCut;
                }
            } );
        }

    return editPermission & CMsgBaseControl::EMsgEditCut;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::CcpuCanCopy
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgExpandableControlEditor::CcpuCanCopy() const
    {
    TUint32 editPermission = CheckEditPermission();

    if (iBaseControlObserver)
        {
        TRAP_IGNORE( 
            {
            if ( iBaseControlObserver->HandleEditObserverEventRequestL( this, EMsgDenyCopy ) )
                {
                editPermission &= ~CMsgBaseControl::EMsgEditCopy;
                }
            } );
        }

    return editPermission & CMsgBaseControl::EMsgEditCopy;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::CcpuCanPaste
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgExpandableControlEditor::CcpuCanPaste() const
    {
    TUint32 editPermission = CheckEditPermission();

    if ( !CEikEdwin::CcpuCanPaste() )
        {
        editPermission &= ~CMsgBaseControl::EMsgEditPaste;
        }

    if ( iBaseControlObserver )
        {
        TRAP_IGNORE( 
            {
            if ( iBaseControlObserver->HandleEditObserverEventRequestL( this, EMsgDenyPaste ) )
                {
                editPermission &= ~CMsgBaseControl::EMsgEditPaste;
                } 
            } );
        }

    return editPermission & CMsgBaseControl::EMsgEditPaste;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::CcpuCanUndo
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgExpandableControlEditor::CcpuCanUndo() const
    {
    return CheckEditPermission() & CMsgBaseControl::EMsgEditUndo;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::CcpuCutL
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControlEditor::CcpuCutL()
    {
    CEikRichTextEditor::ClipboardL( ECut );
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::CcpuCopyL
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControlEditor::CcpuCopyL()
    {
    CEikRichTextEditor::ClipboardL( ECopy );
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::CcpuPasteL
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControlEditor::CcpuPasteL()
    {
    CEikRichTextEditor::ClipboardL( EPaste );
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::CcpuUndoL
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControlEditor::CcpuUndoL()
    {
    UndoL();
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::SetMaxNumberOfChars
//
// Sets maximun number of characters.
// ---------------------------------------------------------
//
void CMsgExpandableControlEditor::SetMaxNumberOfChars( TInt aMaxNumberOfChars )
    {
    iMaxNumberOfChars = aMaxNumberOfChars;
    if ( Text() )
        {
        SetTextLimit( iMaxNumberOfChars );
        }
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::SetTextContentL
//
// Sets aText content for the editor.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControlEditor::SetTextContentL( CRichText& aText )
    {
    MEBLOGGER_ENTERFN("CMsgExpandableControlEditor::SetTextContentL");

    MEBLOGGER_WRITE("#1");
    MEBLOGGER_WRITE_TIMESTAMP("");

    if ( IsPlainTextMode() )
        {
        CRichText* richText = RichText();

        MEBLOGGER_WRITEF( _L( "MEB: SetTextContentL: Copying text, length %d" ), aText.DocumentLength() );
        
        // Ignore the original formatting on plain text mode and set the
        // new rich text object to use current formatting parameters.
        aText.SetGlobalParaFormat( richText->GlobalParaFormatLayer() );
        aText.SetGlobalCharFormat( richText->GlobalCharFormatLayer() );
        
        CEikEdwin::CopyDocumentContentL( aText, *richText );
        TInt documentLength = richText->DocumentLength();
        
        MEBLOGGER_WRITEF( _L( "MEB: SetTextContentL: Copied text, length %d" ), documentLength );

        if ( iMaxNumberOfChars && 
             documentLength > iMaxNumberOfChars )
            {
            richText->DeleteL( iMaxNumberOfChars, documentLength - iMaxNumberOfChars );
            documentLength = richText->DocumentLength();
            }

        const TChar LF( 0x0A );
        const TChar CR( 0x0D );

        TInt i = 0;
        while ( i < documentLength )
            {
            TPtrC ptr = richText->Read( i, 1 );
            TInt remove = 0;
            TBool addParDeli = EFalse;
            if ( ptr.Locate( CEditableText::EPictureCharacter ) != KErrNotFound )
                {
                remove++;
                }
            else if ( ptr.Locate( CR ) != KErrNotFound )
                {
                // "CR"
                remove++;
                addParDeli = ETrue;
                if ( i < documentLength - 1 &&
                    aText.Read( i + 1, 1 ).Locate( LF ) != KErrNotFound )
                    {
                    // "CR+LF"
                    remove++;
                    }
                }
            else if ( ptr.Locate( LF ) != KErrNotFound )
                {
                // "LF"
                remove++;
                addParDeli = ETrue;
                }

            if ( remove )
                {
                MEBLOGGER_WRITEF( _L( "MEB: SetTextContentL: removing %d characters" ), remove );
                richText->DeleteL( i, remove );
                MEBLOGGER_WRITEF( _L( "MEB: SetTextContentL: removed %d characters" ), remove );
                documentLength -= remove;
                if ( addParDeli )
                    {
                    MEBLOGGER_WRITEF( _L( "MEB: SetTextContentL: Inserting paragraph delimeter" ) );
                    richText->InsertL( i, CEditableText::EParagraphDelimiter );
                    MEBLOGGER_WRITEF( _L( "MEB: SetTextContentL: Inserted paragraph delimeter" ) );
                    documentLength++;
                    i++;
                    }
                }
            else
                {
                i++;
                }
            }
        }
    else
        {
        CopyDocumentContentL( aText, *GlobalText() );
        }

    SetAmountToFormatL( ETrue );

    MEBLOGGER_WRITE("#2");
    MEBLOGGER_WRITE_TIMESTAMP("");

    MEBLOGGER_LEAVEFN("CMsgExpandableControlEditor::SetTextContentL");
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::MinimumDefaultHeight
//
// Returns a minimum one line height of the editor.
// ---------------------------------------------------------
//
TInt CMsgExpandableControlEditor::MinimumDefaultHeight() const
    {
    return DefaultHeight( ETrue );
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::Reset
//
// Clear contents.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControlEditor::Reset()
    {
    if ( iTextView )
        {
        TInt documentLength = RichText()->DocumentLength();
        
        RichText()->Reset();
        
        iTextView->SetPendingSelection( TCursorSelection( 0, 0 ) );
        TRAP_IGNORE( iTextView->HandleInsertDeleteL( TCursorSelection( 0, 0 ), 
                                                     documentLength ) );
        }
    else
        {
        RichText()->Reset();
        }
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::SetBaseControlObserver
//
// Sets base control observer.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControlEditor::SetBaseControlObserver(
    MMsgBaseControlObserver& aBaseControlObserver )
    {
    iBaseControlObserver = &aBaseControlObserver;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::OfferKeyEventL
//
// Handles key event.
// ---------------------------------------------------------
//
EXPORT_C TKeyResponse CMsgExpandableControlEditor::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    if ( IsReadOnly() )
        {
        TCursorPosition::TMovementType movetype;

        switch ( aKeyEvent.iCode )
            {
            case EKeyUpArrow:
                movetype = TCursorPosition::EFPageUp;
                break;

            case EKeyDownArrow:
                movetype = TCursorPosition::EFPageDown;
                break;

            default:
                return CEikRichTextEditor::OfferKeyEventL( aKeyEvent, aType );
            }

        iTextView->MoveCursorL( movetype, EFalse );

        return EKeyWasConsumed;
        }
    else
        {
        return CEikRichTextEditor::OfferKeyEventL( aKeyEvent, aType );
        }
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::HandleResourceChange
//
// Sets correct highlight extension.
// ---------------------------------------------------------
//
void CMsgExpandableControlEditor::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        ResolveLayouts();
        LayoutEdwin();
        
        TRAP_IGNORE( 
            {
            if ( iTextView )
                {
                TCursorSelection currentSelection = iTextView->Selection() ;
                
                if ( currentSelection.Length() > 0 )
                    {
                    iTextView->SetSelectionL( currentSelection );
                    }
                else
                    {
                    iTextView->SetDocPosL( currentSelection.iCursorPos, EFalse );
                    }
                }                    
            } );
        }
        
    CEikRichTextEditor::HandleResourceChange( aType );
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::IsPlainTextMode
//
// Checks if the plain text mode is on and returns ETrue if it is.
// ---------------------------------------------------------
//
TBool CMsgExpandableControlEditor::IsPlainTextMode() const
    {
    return iFlags & EMsgControlModePlainTextMode;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::LayoutEdwin
// ---------------------------------------------------------
//
void CMsgExpandableControlEditor::LayoutEdwin()
    {
    TAknLayoutRect msgTextPane;
    msgTextPane.LayoutRect( MsgEditorCommons::MsgDataPane(),
                            AknLayoutScalable_Apps::msg_text_pane( 0 ).LayoutLine() );
                            
    TAknLayoutRect msgHeaderPane;
    msgHeaderPane.LayoutRect( msgTextPane.Rect(),
                              AknLayoutScalable_Apps::msg_header_pane().LayoutLine() );
    
    TRect parentRect( msgHeaderPane.Rect() );
    parentRect.Move( 0, iParentControl->Position().iY );
    
    DoLayoutEdwin( parentRect, 
                   AknLayoutScalable_Apps::msg_header_pane_t2( 0 ).LayoutLine() );
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::DoLayoutEdwin
//
// Performs the layout setting for edwin according to the given
// parameters. Used from LayoutEdwin or some of it's derived versions.
// ---------------------------------------------------------
//
void CMsgExpandableControlEditor::DoLayoutEdwin( const TRect& aEdwinParent, 
                                                 const TAknTextLineLayout& aLayout )
    {
    TPoint position( iPosition );
    
    // Layout edwin as two lines to get the base line delta override activated.
    TInt lineNumber( 0 );
    AknLayoutUtils::LayoutEdwin( this,
                                 aEdwinParent,
                                 aLayout,
                                 2,
                                 MsgEditorCommons::MsgBaseLineDelta(),
                                 EAknsCIQsnTextColorsCG6,
                                 lineNumber );
    
    // Resize the edwin to the correct size. Also set the original position.
    TInt notUsed( 0 );
    TInt height( 0 );
    AknLayoutUtils::GetEdwinVerticalPositionAndHeightFromLines( 
                                                aEdwinParent.Size().iHeight,
                                                aLayout,
                                                MsgEditorCommons::MsgBaseLineDelta(),
                                                iNumberOfLines,
                                                notUsed,
                                                height );
        
    TRect editorRect( position, TSize( iSize.iWidth, height ) );
    SetRect( editorRect );
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::AdjustLineHeightL
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControlEditor::AdjustLineHeightL()
    {
    // nop
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::TextFont
// ---------------------------------------------------------
//
EXPORT_C const CFont* CMsgExpandableControlEditor::TextFont() const
    {
    return iTextLayout.Font();
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::DefaultHeight
// ---------------------------------------------------------
//
TInt CMsgExpandableControlEditor::DefaultHeight( TBool /*aOneToOne*/ ) const
    {
    return TextFont()->HeightInPixels();
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::VirtualHeight
//
// Returns approximate height of text. In band formatting mode
// we have to calculate the average chars per line and 
// average line in view values by ourself as we might get called
// in a callback when edwin does not contain up-to-date values.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgExpandableControlEditor::VirtualHeight()
    {
    TInt heightInPixels( 0 );
    TInt msgBaseLineDelta( MsgEditorCommons::MsgBaseLineDelta() );
    
    if ( iLayout->IsFormattingBand() )
        {
        TInt docLength = iLayout->DocumentLength();
        
        TInt avgCharsPerLine = iLayout->FormattedLength() / iLayout->NumFormattedLines();
        TInt avgLinesInViewRect= Max( 1, iTextView->ViewRect().Height() * 
                                         iLayout->NumFormattedLines() / 
                                         iLayout->FormattedHeightInPixels() );
        
        const TInt approxTotalLines = Max( docLength / avgCharsPerLine, 
                                           iText->ParagraphCount() );
        const TInt approxLineHeight = iTextView->ViewRect().Height() / avgLinesInViewRect;
        heightInPixels = approxTotalLines * approxLineHeight;
        }
    else
        {
        if ( iTextView )
            {
            // Force formatting to complete so that 
            // correct height can be retrieved.
            TRAP_IGNORE( iTextView->FinishBackgroundFormattingL() );
            }
        
        heightInPixels = iLayout->FormattedHeightInPixels();        
        }
    
    heightInPixels = Max( heightInPixels, iLayout->FormattedHeightInPixels() );
    MsgEditorCommons::RoundToPreviousLine( heightInPixels, msgBaseLineDelta );    
    
    return heightInPixels;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::VirtualVisibleTop
//
// Returns a topmost visible text position. In band formatting mode
// we have to calculate the average chars per line value 
// by ourself as we might get called in a callback when edwin 
// does not contain up-to-date value.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgExpandableControlEditor::VirtualVisibleTop()
    {
    TInt ret( 0 );
    
    TInt msgBaseLineDelta( MsgEditorCommons::MsgBaseLineDelta() );
    
    if ( iLayout->IsFormattingBand() )
        {
        TInt avgCharsPerLine = iLayout->FormattedLength() / iLayout->NumFormattedLines();
        
        TInt topLeftDocPos = iLayout->FirstDocPosFullyInBand();
        TInt approxTopVisibleLine = topLeftDocPos / avgCharsPerLine;
        
        if ( approxTopVisibleLine == 0 && 
             topLeftDocPos > 0 )
            {
            ret =  iLayout->PixelsAboveBand();
            }
        else
            {
            TInt avgLinesInViewRect= Max( 1, iTextView->ViewRect().Height() * 
                    iLayout->NumFormattedLines() / 
                    iLayout->FormattedHeightInPixels() );

            TInt approxLineHeight = iTextView->ViewRect().Height() / avgLinesInViewRect;
            ret = approxTopVisibleLine * approxLineHeight;
            }
        }
    else
        {
        // Round to the last full line height. Division removes the fraction since
        // result is integer (i.e. non floating point value).
        ret = iLayout->PixelsAboveBand();
        }

    ret = Max( ret, iLayout->PixelsAboveBand() );
    MsgEditorCommons::RoundToPreviousLine( ret, msgBaseLineDelta );    
    
    return ret;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::ActivateL
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControlEditor::ActivateL()
    {
    if ( IsActivated() )
        {
        return;
        }
        
    TInt cursorPos = 0;

    if ( iTextView )
        {
        cursorPos = CursorPos();
        }
    
    LayoutEdwin();
    SetTextSkinColorIdL();
    
    CEikRichTextEditor::ActivateL();
    
    iTextView->SetDocPosL( cursorPos );
 if ( !iCustomDraw )
        {
        // Swap the custom drawer to our own to show text lines.
        const MFormCustomDraw* customDraw = TextLayout()->CustomDraw();
    
        iCustomDraw = CMsgEditorCustomDraw::NewL( customDraw, this, iControlType);
        
        TextLayout()->SetCustomDraw( iCustomDraw );
        }
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::ItemFinder
// ---------------------------------------------------------
//
EXPORT_C CItemFinder* CMsgExpandableControlEditor::ItemFinder() const
    {
    return iItemFinder;
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::FocusChanged
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableControlEditor::FocusChanged( TDrawNow aDrawNow )
    {
    CEikEdwin::FocusChanged( aDrawNow );
    
    TRAP_IGNORE( DoFocusChangedL() );
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::DoFocusChangedL
// ---------------------------------------------------------
//
void CMsgExpandableControlEditor::DoFocusChangedL()
    {
    SetTextSkinColorIdL();
    
    if( ! ( iFlags & EMsgControlModeDoNotDrawFrameBgContext ) &&
        ! static_cast<const CMsgBaseControl*>( iParentControl )->IsReadOnly() )
        {
        if( IsFocused( ) )
            { 
            // The focus arrives so lets so the frame
            // It is drawn between the outer and inner rects
            TRect outer, inner;
            OuterAndInnerRects( outer, inner );
            
            if( !iFrameBgContext )
                {
                iFrameBgContext = 
                    CAknsFrameBackgroundControlContext::NewL( KAknsIIDQsnFrInput, 
                                                              outer, 
                                                              inner, 
                                                              EFalse );
                iFrameBgContext->SetParentContext( iBgContext );
                }
            else
                {
                iFrameBgContext->SetFrameRects( outer, inner );        
                }
            SetSkinBackgroundControlContextL( iFrameBgContext );
            }
        else
            { // Focus leaves so lets remove the skin bg control contex
            SetSkinBackgroundControlContextL( iBgContext );
            }
        }
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::HandleFrameSizeChangeL
// ---------------------------------------------------------
//
void CMsgExpandableControlEditor::HandleFrameSizeChangeL()
    {
    if( IsFocused( ) )
        { 
        // We have focus so lets resize the frame context
        if( ! (iFlags & EMsgControlModeDoNotDrawFrameBgContext ) &&
            ! static_cast<const CMsgBaseControl*>( iParentControl )->IsReadOnly() ) 
            {
            // The size of the frame has changed so lets find out
            // the outer and inner rects again
            TRect outer, inner;
            OuterAndInnerRects( outer, inner );

            if( !iFrameBgContext )
                {
                iFrameBgContext = 
                    CAknsFrameBackgroundControlContext::NewL( KAknsIIDQsnFrInput, 
                                                              outer, 
                                                              inner, 
                                                              EFalse );
                iFrameBgContext->SetParentContext( iBgContext );
                }
                
            iFrameBgContext->SetFrameRects( outer, inner );
            SetSkinBackgroundControlContextL( iFrameBgContext );
            }
        }
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::OuterAndInnerRects
// ---------------------------------------------------------
//
void CMsgExpandableControlEditor::OuterAndInnerRects( TRect& aOuter, TRect& aInner )
    {
    TRect rect( Rect( ) );
    TRect focused ( iParentControl->Rect( ) );

    // If we are out of screen, start it just from the baseline offset
    TInt top = MsgEditorCommons::MsgBaseLineOffset();
    if ( focused.iTl.iY < top )
        {
        focused.iTl.iY = top;
        }
    TInt maxBodyHeight( MsgEditorCommons::EditorViewHeigth() );
    if ( focused.iBr.iY > maxBodyHeight )
        {
        focused.iBr.iY = maxBodyHeight;
        }

    aOuter.SetRect( rect.iTl.iX-1, 
                    focused.iTl.iY,
                    rect.iBr.iX, 
                    focused.iBr.iY );

    TAknLayoutRect tl;
    TAknLayoutRect br;
    tl.LayoutRect( aOuter, AknLayoutScalable_Avkon::input_focus_pane_g2().LayoutLine() );
    br.LayoutRect( aOuter, AknLayoutScalable_Avkon::input_focus_pane_g5().LayoutLine() );

    aInner.SetRect( aOuter.iTl.iX - 1,
                    aOuter.iTl.iY + tl.Rect().Height(),
                    aOuter.iBr.iX,
                    aOuter.iBr.iY - br.Rect().Height() );    
    }


// ---------------------------------------------------------
// CMsgExpandableControlEditor::ResolveLayouts
//
//
// ---------------------------------------------------------
//
void CMsgExpandableControlEditor::ResolveLayouts()
    {
    TAknLayoutRect msgTextPane;
    msgTextPane.LayoutRect( MsgEditorCommons::MsgDataPane(),
                            AknLayoutScalable_Apps::msg_text_pane( 0 ).LayoutLine() );
                            
    TAknLayoutRect msgHeaderPane;
    msgHeaderPane.LayoutRect( msgTextPane.Rect(),
                              AknLayoutScalable_Apps::msg_header_pane().LayoutLine() );
                              
    iTextLayout.LayoutText( msgHeaderPane.Rect(),
                            AknLayoutScalable_Apps::msg_header_pane_t2( 0 ).LayoutLine() );
        TRect dataPane = MsgEditorCommons::MsgDataPane();


        TAknTextLineLayout editorLineLayout;
        TAknLayoutText    editorLayout;
                                                                  
        editorLineLayout = AknLayoutScalable_Apps::msg_header_pane_t2( 0 ).LayoutLine();
        
        editorLayout.LayoutText( msgHeaderPane.Rect(), editorLineLayout );

       iEditortop = editorLayout.TextRect().iTl.iY - msgHeaderPane.Rect().iTl.iY;       
    if ( iCustomDraw )
        {
            iCustomDraw->ResolveLayouts();
        }
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::PrepareForReadOnlyL
// ---------------------------------------------------------
//
void CMsgExpandableControlEditor::PrepareForReadOnlyL( TBool aReadOnly )
    {
    if ( IsReadOnly() == aReadOnly )
        {
        return;
        }
    
    SetReadOnly( aReadOnly );
    SetTextSkinColorIdL();
    
    if ( aReadOnly )
        {
        AddFlagToUserFlags( CEikEdwin::EAvkonDisableCursor );
        }
    else
        {
        RemoveFlagFromUserFlags( CEikEdwin::EAvkonDisableCursor );
        }
    
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::SetTextSkinColorIdL
// ---------------------------------------------------------
//
void CMsgExpandableControlEditor::SetTextSkinColorIdL()
    {
    CEikEdwin::SetTextSkinColorIdL( EAknsCIQsnTextColorsCG6 );
    }

// ---------------------------------------------------------
// CMsgExpandableControlEditor::IsCursorVisibleL
// ---------------------------------------------------------
//
TBool CMsgExpandableControlEditor::IsCursorVisibleL()
    {
    TBool result = EFalse;
    if ( iTextView )
        {
        TPoint cursorPos;
        
        if ( iTextView->DocPosToXyPosL( CursorPos(), cursorPos  ) && 
             iTextView->ViewRect().Contains( cursorPos ) )
            {
            result = ETrue;
            }
        }
    return result;
    }
// ---------------------------------------------------------
// CMsgExpandableControlEditor::MsgEditorCustomDraw
// ---------------------------------------------------------
//
CMsgEditorCustomDraw* CMsgExpandableControlEditor::MsgEditorCustomDraw() const
    {
        return iCustomDraw;
    }
    
    
// ---------------------------------------------------------
// CMsgExpandableControlEditor::SetControlType
// ---------------------------------------------------------
//
void  CMsgExpandableControlEditor::SetControlType(TInt actrltype)
    {
    iControlType = actrltype;
    }
//  End of File
