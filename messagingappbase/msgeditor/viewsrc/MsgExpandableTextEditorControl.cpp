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
* Description:  MsgExpandableTextEditorControl implementation
*
*/



// ========== INCLUDE FILES ================================

#include <eikenv.h>                        // for CEikonEnv
#include <eikedwin.h>                      // for TClipboardFunc
#include <txtrich.h>                       // for CRichText
#include <AknUtils.h>                      // for AknUtils
#include <aknenv.h>                        // for CAknEnv
#include <AknDef.h>


#include "MsgExpandableTextEditorControl.h"
#include "MsgEditorCommon.h"               //
#include "MsgExpandableControlEditor.h"    // for CMsgExpandableControlEditor
#include "MsgBaseControlObserver.h"        // for MMsgBaseControlObserver
#include "MsgEditorPanic.h"                // for CMsgEditor panics
#include "MsgEditorLogging.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================
const TInt KCursorPosNotSet = -1;

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::CMsgExpandableTextEditorControl
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgExpandableTextEditorControl::CMsgExpandableTextEditorControl() :
    iCursorPos( KCursorPosNotSet )
    {
    SetComponentsToInheritVisibility( ETrue );
    }

// ---------------------------------------------------------
// CMsgExpandableControl::CMsgExpandableControl
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgExpandableTextEditorControl::CMsgExpandableTextEditorControl( MMsgBaseControlObserver& aBaseControlObserver ) :
    CMsgBaseControl( aBaseControlObserver ),
    iCursorPos( KCursorPosNotSet )
    {
    SetComponentsToInheritVisibility( ETrue );
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::~CMsgExpandableTextEditorControl
//
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgExpandableTextEditorControl::~CMsgExpandableTextEditorControl()
    {
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::SetPlainTextMode
//
// Sets the plain text mode on or off.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableTextEditorControl::SetPlainTextMode( TBool aMode )
    {
    if ( aMode )
        {
        iControlModeFlags |= EMsgControlModePlainTextMode;
        }
    else
        {
        iControlModeFlags &= ~EMsgControlModePlainTextMode;
        }

    iEditor->SetAllowPictures( !( iControlModeFlags & EMsgControlModePlainTextMode ) );
    }

// ---------------------------------------------------------
// CMsgExpandableControl::IsPlainTextMode
//
// Checks if the plain text mode is on and returns ETrue if it is.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgExpandableTextEditorControl::IsPlainTextMode() const
    {
    return iControlModeFlags & EMsgControlModePlainTextMode;
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::TextContent
//
// Returns a reference to text content of the editor.
// ---------------------------------------------------------
//
EXPORT_C CRichText& CMsgExpandableTextEditorControl::TextContent() const
    {
    return *iEditor->RichText();
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::CopyDocumentContentL
//
// Copies aInText to aOutText.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableTextEditorControl::CopyDocumentContentL( CGlobalText& aInText,
                                                                     CGlobalText& aOutText )
    {
    iEditor->CopyDocumentContentL( aInText, aOutText );
    iControlModeFlags |= EMsgControlModeModified;
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::SetMaxNumberOfChars
//
// Sets maximun number of characters.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableTextEditorControl::SetMaxNumberOfChars( TInt aMaxNumberOfChars )
    {
    iEditor->SetMaxNumberOfChars( aMaxNumberOfChars );
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::SetCursorPosL
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableTextEditorControl::SetCursorPosL( TInt aCursorPos )
    {
    if ( iEditor &&
         IsActivated() &&
         ( iEditor->CursorPos() != aCursorPos || 
           !iEditor->IsCursorVisibleL() ) )
        {
        iEditor->SetCursorPosL( aCursorPos, EFalse );
        iBaseControlObserver->HandleBaseControlEventRequestL( this, 
                                                              EMsgEnsureCorrectFormPosition, 
                                                              0 );            
        }
    iCursorPos = aCursorPos;
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::ActivateL
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableTextEditorControl::ActivateL()
    {
    CMsgBaseControl::ActivateL();
    
    if ( iCursorPos != KCursorPosNotSet )
        {
        SetCursorPosL( iCursorPos );
        }

    // slide flow
    if(IsReadOnly())
        {
        // avoid scroll to 0 pos after init
        SetCursorPosL(KCursorPosNotSet);
        }
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::ScrollL
//
// Tries to scroll the given amount of pixels. Restrict scroll
// to top of lines is switched off to allow better scrolling of
// text view. Otherwise scrolling would be limited to top of 
// the text lines. Margin pixels are scrolled "away" when control reaches
// the end.
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
EXPORT_C TInt CMsgExpandableTextEditorControl::ScrollL( TInt aPixelsToScroll, TMsgScrollDirection aDirection )
    {
    TInt pixelsToScroll( 0 );
    
    CTextLayout* textLayout = iEditor->TextLayout();
    
    if ( aDirection == EMsgScrollDown )
        {
        // Negative pixels means scrolling down.
        pixelsToScroll = -aPixelsToScroll;
        }
    else
        {
        // Positive pixels means scrolling up.
        pixelsToScroll = aPixelsToScroll;
        }
    
    if ( pixelsToScroll != 0 )
        {
        iEditor->TextView()->ScrollDisplayPixelsL( pixelsToScroll );
                
        if ( Abs( pixelsToScroll ) % iLineHeight != 0 )
            {
            pixelsToScroll = Abs( pixelsToScroll ) - ( Size().iHeight - iEditor->Size().iHeight );
            }

        }
    
    return Abs( pixelsToScroll );
    }
#else
EXPORT_C TInt CMsgExpandableTextEditorControl::ScrollL( TInt /*aPixelsToScroll*/, TMsgScrollDirection /*aDirection*/ )
    {
    return 0;
    }
#endif // RD_SCALABLE_UI_V2

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::CurrentLineRect
//
// Returns the current control rect.
// ---------------------------------------------------------
//
EXPORT_C TRect CMsgExpandableTextEditorControl::CurrentLineRect()
    {
    TRect lineRect( 0, 0, 0, 0 );
    TRAPD( ret, lineRect = iEditor->CurrentLineRectL() );
    if ( ret != KErrNone )
        {
        return lineRect;
        }

    TRect ctrlRect( Rect() );

    lineRect.iTl.iX = ctrlRect.iTl.iX;
    lineRect.iBr.iX = ctrlRect.iBr.iX;
    // "lineRect" is relative to "ctrlRect" -> Move needed
    lineRect.Move( TPoint( 0, iPosition.iY  ) );

    // "lineRect" should never be outside the "ctrlRect"
    // There seems to be a bug in Edwin when layouting is
    // changed from "full" to "partial". In this case the
    // "lineRect" might get outside "ctrlRect". This
    // causes false alarm and unwanted scrolling in
    // CMsgEditorView::EnsureCorrectFormPosition.
    // -> Has to make sure lineRect won't get outside ctrlRect

    if ( lineRect.iBr.iY > ctrlRect.iBr.iY )
        {
        lineRect.Move( TPoint( 0, ctrlRect.iBr.iY - lineRect.iBr.iY ) );
        }

    return lineRect;
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::SetBaseControlObserver
//
// Sets base control observer.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableTextEditorControl::SetBaseControlObserver( MMsgBaseControlObserver& aBaseControlObserver )
    {
    CMsgBaseControl::SetBaseControlObserver( aBaseControlObserver );
    iEditor->SetBaseControlObserver( aBaseControlObserver );
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::ClipboardL
//
// Handles clipboard operation.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableTextEditorControl::ClipboardL( TMsgClipboardFunc /*aFunc*/ )
    {
    // changed to do nothing because
    // CMsgExpandableControlEditor::Ccpu???? functions handle
    // clipboard operations now.
    }
// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::EditL
//
// Handles editing operation.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableTextEditorControl::EditL( TMsgEditFunc aFunc )
    {
    TUint32 editPermission = EditPermission();

    switch ( aFunc )
        {
        case EMsgUndo:
            {
            if ( editPermission & EMsgEditUndo )
                {
                iEditor->UndoL();
                }
            break;
            }
        case EMsgSelectAll:
            {
            if ( editPermission & EMsgEditSelectAll )
                {
                iEditor->SelectAllL();
                HandleControlEventL( this, MCoeControlObserver::EEventStateChanged );
                }
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EMsgIncorrectEditFunction ) );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::EditPermission
//
// Returns edit permission flags.
// ---------------------------------------------------------
//
EXPORT_C TUint32 CMsgExpandableTextEditorControl::EditPermission() const
    {
    return iEditor->CheckEditPermission();
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::IsCursorLocation
//
// Checks if the cursor location is on the topmost or downmost position
// depending on aLocation and returns ETrue if it is.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgExpandableTextEditorControl::IsCursorLocation( TMsgCursorLocation aLocation ) const
    {
    switch ( aLocation )
        {
        case EMsgTop:
            {
            return iEditor->CursorInFirstLine();
            }
        case EMsgBottom:
            {
            return iEditor->CursorInLastLine();
            }
        default:
            {
            return EFalse;
            }
        }
    }


// CMsgExpandableTextEditorControl::VirtualHeight
//
// Returns approximate height of the control.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgExpandableTextEditorControl::VirtualHeight()
    {
    return iEditor->VirtualHeight();
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::VirtualVisibleTop
//
// Returns a topmost visible text position. Queries the visible top
// from editor and adds control top position differences (= top margin ) 
// into it if control is not visible.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgExpandableTextEditorControl::VirtualVisibleTop()
    {
    return iEditor->VirtualVisibleTop();
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::HandleControlEventL
// 
// Handles state change event from text editor. Checks whether form 
// updating is needed. This is needed atleast when subject field
// is visible at the last row of the view and word is typed using T9 
// so that line is changed in the middle of it. Without this check
// the view is not updated correctly.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableTextEditorControl::HandleControlEventL( CCoeControl* aControl, 
                                                                    TCoeEvent aEventType )
    {
    if ( aControl == iEditor &&
         aEventType == MCoeControlObserver::EEventStateChanged )
        {
        iBaseControlObserver->HandleBaseControlEventRequestL( this, 
                                                              EMsgEnsureCorrectFormPosition, 
                                                              0 );
        iControlModeFlags |= EMsgControlModeModified;
        }
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::HandleEdwinSizeEventL
//
// Handles expandable text editor size event. These event are send only when
// the editor height changes. First desirable heigh is limited to maximum
// body height. This is needed as edwin will report values larger than 
// maximum height. After this a change delta between current and desired 
// heights is calculated. This is rounded to the next full baseline. 
// If editor height is wanted to reduce and there is still unshown content
// then the content is scrolled downward so that it becomes visible. In this
// case the editor or control height is not changed. If editor height is 
// wanted to increase or all the content is current shown then
// control and editor heights are changed. If body max height flag
// is specified for control then the control height is always set to maximum.
// After heights have been changed base control observer (CMsgEditorView)
// is notified.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgExpandableTextEditorControl::HandleEdwinSizeEventL( CEikEdwin* /*aEdwin*/,
                                                                       TEdwinSizeEvent aEventType,
                                                                       TSize aDesirableEdwinSize )
    {
    if ( aEventType == EEventSizeChanging )
        {
        MEBLOGGER_ENTERFN("CMsgExpandableTextEditorControl::HandleEdwinSizeEventL");
        
        // slide flow
        if(IsReadOnly())
            {
            return EFalse;
            }

        aDesirableEdwinSize.iHeight = Min( aDesirableEdwinSize.iHeight, iMaxBodyHeight );
        
        TInt delta = aDesirableEdwinSize.iHeight - iEditor->Size().iHeight;
        MsgEditorCommons::RoundToNextLine( delta, iLineHeight );
        
        if ( delta < 0 && 
             iEditor->VirtualHeight() > iMaxBodyHeight )
            {
            ScrollL( delta, EMsgScrollDown );
            iBaseControlObserver->HandleBaseControlEventRequestL( this, 
                                                                  EMsgUpdateScrollbar, 
                                                                  delta );
            }        
        else if ( delta ||
                  iControlModeFlags & EMsgControlModeForceSizeUpdate )
            {
            if ( iControlModeFlags & EMsgControlModeBodyMaxHeight )
                {
                aDesirableEdwinSize.iHeight = iEditor->MaximumHeight();
                }            
            
            // Performs the real size change if height has really changed
            // or if forced size change is set.
            iSize.iHeight = aDesirableEdwinSize.iHeight;
            MsgEditorCommons::RoundToNextLine( iSize.iHeight, iLineHeight );
            
            if ( delta != 0 )
                {
                iEditor->SetAndGetSizeL( aDesirableEdwinSize );
                }

            iBaseControlObserver->HandleBaseControlEventRequestL( this, 
                                                                  EMsgHeightChanged, 
                                                                  delta );
            }

        MEBLOGGER_LEAVEFN("CMsgExpandableTextEditorControl::HandleEdwinSizeEventL");

        if ( delta < 0 )
            { 
            return ETrue;
            }
        }

    return EFalse;
    }

// ---------------------------------------------------------
// CMsgExpandableTextEditorControl::HandleEdwinEventL
//
// Handles events from expandable text editor. Navigation and format
// change events are handled.
// ---------------------------------------------------------
//
EXPORT_C void CMsgExpandableTextEditorControl::HandleEdwinEventL( CEikEdwin* aEdwin, 
                                                                  TEdwinEvent aEventType)
    {
    if ( aEventType == MEikEdwinObserver::EEventNavigation && aEdwin->TextView() )
        {
        // When cursor is in beginning of text and left key is pressed,
        // FEP places cursor to the end of text. This scrolls end of text
        // to be visible. With pen support this needs to be handled always since
        // select & drag scrolling might change the text position so that form updating
        // is needed.
        iBaseControlObserver->HandleBaseControlEventRequestL( this, 
                                                              EMsgEnsureCorrectFormPosition, 
                                                              0 );
        }
    else if ( aEventType == MEikEdwinObserver::EEventFormatChanged )
        {
        iBaseControlObserver->HandleBaseControlEventRequestL( this, 
                                                              EMsgEnsureCorrectFormPosition, 
                                                              0 );
        }
    }


//  End of File
