/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgBodyControl implementation
*
*/



// ========== INCLUDE FILES ================================

#include <txtrich.h>                       // for CRichText
#include <eikenv.h>                        // for CEikonEnv
#include <gulfont.h>                       // for TLogicalFont
#include <AknUtils.h>                      // for AknUtils
#include <aknenv.h>                        // for CAknEnv
#include <ItemFinder.h>                    // for automatic highlight
#include <AknsUtils.h>                     // for Skinned drawing
#include <AknDef.h>
#include <StringLoader.h>                   // for StringLoader (load and foramt strings from resources)
#include <MsgEditorAppUi.rsg>

#include <applayout.cdl.h> // LAF
#include <aknlayoutscalable_apps.cdl.h>

#include "MsgEditorCommon.h"               //
#include "MsgEditorView.h"                 // for CMsgEditorView
#include "MsgBodyControl.h"                // for CMsgBodyControl
#include "MsgBaseControlObserver.h"        // for MMsgBaseControlObserver
#include "MsgBodyControlEditor.h"          // for CMsgBodyControlEditor
#include "MsgEditorPanic.h"                // for MsgEditor panics

#include "MsgEditorLogging.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgBodyControl::CMsgBodyControl
//
// Constructor.
// ---------------------------------------------------------
//
CMsgBodyControl::CMsgBodyControl( MMsgBaseControlObserver& aBaseControlObserver ) : 
    CMsgExpandableTextEditorControl( aBaseControlObserver )
    {
    }

// ---------------------------------------------------------
// CMsgBodyControl::~CMsgBodyControl
//
// Destructor.
// ---------------------------------------------------------
//
CMsgBodyControl::~CMsgBodyControl()
    {
    AknsUtils::DeregisterControlPosition( iEditor );
    delete iEditor;
    delete iCaptionText;
    AknsUtils::DeregisterControlPosition( this );
    }

// ---------------------------------------------------------
// CMsgBodyControl::NewL
//
// Factory method that creates this control.
// ---------------------------------------------------------
//
EXPORT_C CMsgBodyControl* CMsgBodyControl::NewL( CCoeControl* aParent )
    {
    __ASSERT_DEBUG( aParent, Panic( EMsgNullPointer ) );

    CMsgEditorView* obs = static_cast<CMsgEditorView*>( aParent );
    CMsgBodyControl* self = new ( ELeave ) CMsgBodyControl( *obs );
    self->SetMopParent( aParent );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CMsgBodyControl::ConstructL
//
// Creates the body control editor for the this control and sets necessary details.
// ---------------------------------------------------------
//
void CMsgBodyControl::ConstructL()
    {
    iControlType = EMsgBodyControl;

    iEditor = new ( ELeave ) CMsgBodyControlEditor( this, 
                                                    iControlModeFlags, 
                                                    iBaseControlObserver );
    iEditor->ConstructL();
    
    iEditor->SetControlType(iControlType);   
    iEditor->SetObserver( this );
    iEditor->SetEdwinSizeObserver( this );
    iEditor->AddEdwinObserverL( this );
    iEditor->SetMaxNumberOfChars( 0 );
    iEditor->SetBorder( TGulBorder::ENone );
    iEditor->SetAknEditorFlags( EAknEditorFlagUseSCTNumericCharmap |
                                EAknEditorFlagEnablePictographInput |
                                EAknEditorFlagAllowEntersWithScrollDown |
                                EAknEditorFlagEnablePartialScreen                          
                                );   
    
    SetPlainTextMode( ETrue );
    
    ResolveFontMetrics();
    SetIcfPromptTextL();
    CItemFinder* itemFinder = iEditor->ItemFinder();
    if ( itemFinder )
        {
        // for automatic highlight
        itemFinder->SetEditor( (CEikRichTextEditor**)&iEditor );
        }
    }

// ---------------------------------------------------------
// CMsgBodyControl::Editor
//
// Returns a reference to the editor control.
// ---------------------------------------------------------
//
EXPORT_C CEikRichTextEditor& CMsgBodyControl::Editor() const
    {
    return *iEditor;
    }

// ---------------------------------------------------------
// CMsgBodyControl::SetTextContentL
//
// Sets text content to the control editor.
// ---------------------------------------------------------
//
EXPORT_C void CMsgBodyControl::SetTextContentL( CRichText& aText )
    {   
	CItemFinder* autofind = ItemFinder();
	
	TBool alreadyBandFormatting( EFalse );
	if ( iEditor->TextLayout( )->IsFormattingBand( ) )
        {
        alreadyBandFormatting = ETrue;
        }
	
    iEditor->SetTextContentL( aText );
    
    // Text added after control has been set visible and 
    // there is enough text to change editor to do band formatting.
    // We have to update the editor size to maximum body control size
    // and enable force control size updating to start size change really
    // happening.
    if ( iControlModeFlags & EMsgControlModeInitialized &&
         !alreadyBandFormatting &&
         iEditor->TextLayout( )->IsFormattingBand( ) )
        {
        iControlModeFlags |= EMsgControlModeForceSizeUpdate;
        
        TSize newSize( iSize.iWidth, iMaxBodyHeight );
        iEditor->SetSize( newSize );
        
        iControlModeFlags &= ~EMsgControlModeForceSizeUpdate;
        }
    
	if ( autofind ) // for automatic highlight
		{
		autofind->SetEditor( (CEikRichTextEditor**)&iEditor );
        }
        
    iControlModeFlags |= EMsgControlModeModified;
    }

// ---------------------------------------------------------
// CMsgBodyControl::InsertCharacterL
//
// Inserts a character to the editor.
// ---------------------------------------------------------
//
EXPORT_C void CMsgBodyControl::InsertCharacterL( const TChar& aCharacter )
    {
    BodyControlEditor()->InsertCharacterL( aCharacter );
    iControlModeFlags |= EMsgControlModeModified;
    }


// ---------------------------------------------------------
// CMsgBodyControl::InsertTextL
//
// Inserts text to the editor.
// ---------------------------------------------------------
//
EXPORT_C void CMsgBodyControl::InsertTextL( const TDesC& aText )
    {
    BodyControlEditor()->InsertTextL( aText );
    iControlModeFlags |= EMsgControlModeModified;

    //iBaseControlObserver->HandleBaseControlEventRequestL(this, EMsgEnsureCorrectFormPosition);
    }

// ---------------------------------------------------------
// CMsgBodyControl::NotifyViewEvent
// ---------------------------------------------------------
//
void CMsgBodyControl::NotifyViewEvent( TMsgViewEvent aEvent, TInt aParam )
    {
    TRAP_IGNORE( DoNotifyViewEventL( aEvent, aParam ) );
    }

// ---------------------------------------------------------
// CMsgBodyControl::DoNotifyViewEventL
// ---------------------------------------------------------
//
void CMsgBodyControl::DoNotifyViewEventL( TMsgViewEvent aEvent, TInt /*aParam*/ )
    {
    switch ( aEvent )
        {
        case EMsgViewEventPrepareFocusTransitionUp:
            {
            if ( IsFocused() && iEditor->TextView() )
                {
                iEditor->ClearSelectionL();
                
                iEditor->TextView()->FinishBackgroundFormattingL();
                if ( iEditor->TextLayout()->FirstDocPosFullyInBand() != 0 )
                    {
                    iEditor->NotifyNewDocumentL();
                    }

                if ( IsReadOnly() )
                    {
                    iEditor->SetCursorPosL( 0, EFalse );
                    }
                }
            
            BodyControlEditor()->PrepareFocusTransition();
            break;
            }
        case EMsgViewEventPrepareFocusTransitionDown:
            {
            if ( IsFocused() && iEditor->TextView() )
                {
                iEditor->ClearSelectionL();
                
                iEditor->TextView()->FinishBackgroundFormattingL();
                if ( !iEditor->CursorInLastLine() )
                    {
                    iEditor->TextView()->SetDocPosL( iEditor->TextLength(), EFalse );
                    
                    // fills the screen
                    TViewYPosQualifier yPosQualifier;
                    yPosQualifier.SetFillScreen();
                    iEditor->TextView()->HandleGlobalChangeL( yPosQualifier );
                    }
                    
                if ( IsReadOnly() )
                    {
                    iEditor->SetCursorPosL( iEditor->TextLength(), EFalse );
                    }
                }
            
            BodyControlEditor()->PrepareFocusTransition();
            break;
            }
        case EMsgViewEventSetCursorFirstPos:
            {
            if ( iEditor->TextView() && 
                 !IsReadOnly() &&
                 !iEditor->CursorInFirstLine() )
                {
                iEditor->SetCursorPosL( 0, EFalse );
                }
            break;
            }
        case EMsgViewEventSetCursorLastPos:
            {
            if ( iEditor->TextView() && 
                 !IsReadOnly() &&
                 !iEditor->CursorInLastLine() )
                {
                iEditor->SetCursorPosL( iEditor->TextLength(), EFalse );
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
// CMsgBodyControl::Reset
// 
// Resets the editor content and notifies view if EMsgControlModeBodyMaxHeight
// is enabled. Explicit function call is needed as control height is not actually
// changing on CMsgBodyControl::HandleEdwinSizeEventL.
// ---------------------------------------------------------
//
EXPORT_C void CMsgBodyControl::Reset()
    {
    iEditor->Reset();       
    
    if ( iControlModeFlags & EMsgControlModeBodyMaxHeight &&
         iControlModeFlags & EMsgControlModeInitialized )
        {
        TRAP_IGNORE( iBaseControlObserver->HandleBaseControlEventRequestL( this, 
                                                                           EMsgHeightChanged ) );
        }
    
    iControlModeFlags |= EMsgControlModeModified;
    }

// ---------------------------------------------------------
// CMsgBodyControl::SetAndGetSizeL
//
// Sets size for the editor and this control.
// ---------------------------------------------------------
//
EXPORT_C void CMsgBodyControl::SetAndGetSizeL( TSize& aSize )
    {
    TSize bodySize( aSize );

    if ( iControlModeFlags & EMsgControlModeBodyMaxHeight )
        {
        bodySize.iHeight = iMaxBodyHeight;
        }
    else
        {
        if ( iEditor->TextLayout() )
            {
            if ( iEditor->TextLayout()->IsFormattingBand() )
                {
                // With band formatting the idea is that it is only
                // used when there is more than screen size of text.
                // If this is false then formatting limits should be
                // change. This is the reason we can safely use 
                // max body height with it.
                bodySize.iHeight = iMaxBodyHeight;
                }
            else
                {
                // Force background formatting to stop so that correct 
                // height can be retrieved
                if ( iEditor->TextView() )
                    {
                    iEditor->TextView()->FinishBackgroundFormattingL();
                    }
                    
                bodySize.iHeight = iEditor->TextLayout()->NumFormattedLines() * iLineHeight;
                }    
           }
        }

    iEditor->SetAndGetSizeL( bodySize );
    
    MsgEditorCommons::RoundToNextLine( bodySize.iHeight, iLineHeight );
        
    SetSizeWithoutNotification( bodySize );

    aSize = bodySize;
    }

// ---------------------------------------------------------
// CMsgBodyControl::IsFocusChangePossible
//
// Checks if the focus change up or down is possible and returns ETrue if it is.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgBodyControl::IsFocusChangePossible( TMsgFocusDirection aDirection ) const
    {
    TBool changeFocusPossible = EFalse;
    
    if ( IsReadOnly() )
        {
        TRAPD( error, changeFocusPossible = BodyControlEditor()->IsFocusChangePossibleL( aDirection ) );
        if ( error != KErrNone )
            {
            changeFocusPossible = ETrue;
            }
        }
    else
        {
        if ( IsCursorLocation( aDirection == EMsgFocusUp ? EMsgTop : 
                                                           EMsgBottom ) )
            {
            changeFocusPossible = ETrue;
            }
        }
    
    return changeFocusPossible;
    }

// ---------------------------------------------------------
// CMsgBodyControl::PrepareForReadOnly
//
// Sets the editor read only or editable.
// ---------------------------------------------------------
//
void CMsgBodyControl::PrepareForReadOnly( TBool aReadOnly )
    {
    TRAP_IGNORE( iEditor->PrepareForReadOnlyL( aReadOnly ) );
    }

// ---------------------------------------------------------
// CMsgBodyControl::OfferKeyEventL
//
// Handles key events.
// ---------------------------------------------------------
//
EXPORT_C TKeyResponse CMsgBodyControl::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                       TEventCode aType )
    {    
    return iEditor->OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CMsgBodyControl::InputCapabilities
//
// Returns input capabilities.
// ---------------------------------------------------------
//
EXPORT_C TCoeInputCapabilities CMsgBodyControl::InputCapabilities() const
    {
    if ( iEditor->IsFocused() )
        {
        TCoeInputCapabilities inputCapabilities( TCoeInputCapabilities::ENone, 
                                                 NULL, 
                                                 const_cast<CMsgBodyControl*>( this ) );
        inputCapabilities.MergeWith( iEditor->InputCapabilities() );
        
        return inputCapabilities;
        }
    else
        {
        return TCoeInputCapabilities::ENone;
        }
    }

// ---------------------------------------------------------
// CMsgBodyControl::HandleResourceChange
//
//
// ---------------------------------------------------------
//
void CMsgBodyControl::HandleResourceChange( TInt aType )
    { 
    CMsgExpandableTextEditorControl::HandleResourceChange( aType );
        
	if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        ResolveFontMetrics();

        CItemFinder* itemFinder = iEditor->ItemFinder();
        if ( itemFinder && 
             itemFinder->CurrentItemExt().iItemType != CItemFinder::ENoneSelected )
            {
            // Refreshes the item finder highlight if present highlighted
            TRAP_IGNORE( itemFinder->ResolveAndSetItemTypeL() );
            }
        }
    }

// ---------------------------------------------------------
// CMsgBodyControl::CountComponentControls
//
// Returns a number of controls.
// ---------------------------------------------------------
//
TInt CMsgBodyControl::CountComponentControls() const
    {
    CCoeControl* controls[] = { iEditor };

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
// CMsgBodyControl::ComponentControl
//
// Returns a control of index aIndex.
// ---------------------------------------------------------
//
CCoeControl* CMsgBodyControl::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* controls[] = { iEditor };

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
// CMsgBodyControl::SizeChanged
//
// Sets position for the editor.
// ---------------------------------------------------------
//
void CMsgBodyControl::SizeChanged()
    {
    MEBLOGGER_ENTERFN("CMsgBodyControl::SizeChanged");

    if ( iControlModeFlags & EMsgControlModeSizeChanging )
        {
        MEBLOGGER_WRITE("EMsgControlModeSizeChanging");
        }
    else
        {
        TPoint editorPosition( Position() );
        editorPosition.iY += iEditorTop;
        iEditor->SetExtent( editorPosition, iEditor->Size() );
    
        AknsUtils::RegisterControlPosition( this );
        AknsUtils::RegisterControlPosition( iEditor );
        }

    MEBLOGGER_LEAVEFN("CMsgBodyControl::SizeChanged");
    }

// ---------------------------------------------------------
// CMsgBodyControl::FocusChanged
//
// This is called when the focus of the control is changed.
// ---------------------------------------------------------
//
void CMsgBodyControl::FocusChanged( TDrawNow aDrawNow )
    {
    // For automatic highlight
    iEditor->SetFocus( IsFocused(), aDrawNow );
    if ( aDrawNow == EDrawNow )
        {
        DrawDeferred();
        }    
    }

// ---------------------------------------------------------
// CMsgBodyControl::SetContainerWindowL
//
// Sets container window.
// ---------------------------------------------------------
//
void CMsgBodyControl::SetContainerWindowL( const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );
    
    iEditor->SetContainerWindowL( aContainer );
    }

// ---------------------------------------------------------
// CMsgBodyControl::CheckCorrectControlYPos
//
//
// ---------------------------------------------------------
//
void CMsgBodyControl::CheckCorrectControlYPosL()
    {
    // SJK 06.09.2004: Added "IsReadOnly" check. Fixing KHAI-5KLH8L.
    if ( IsReadOnly() && iEditor->TextView() && IsFocused() )
        {
        TRect viewRect = iBaseControlObserver->ViewRect();
        TInt delta = viewRect.iTl.iY - iEditor->Rect().iTl.iY;
        iEditor->TextView()->FinishBackgroundFormattingL();
        TInt msgBaseLineDelta( iLineHeight );

        if ( delta % msgBaseLineDelta )
            {
            if ( delta < 0 )
                {
                delta = ( delta - delta % msgBaseLineDelta );
                }
            else
                {
                delta = ( delta - delta % msgBaseLineDelta ) + msgBaseLineDelta;
                }
            }

        if ( ( ( delta < 0) && 
            ( iEditor->TextLayout()->FirstDocPosFullyInBand() != 0 ) )
            || ( delta > 0 ) )
            {
            // these three lines must be here in order to
            // keep text filled with the view.
            TViewYPosQualifier yPosQualifier;
            yPosQualifier.SetFillScreen();
            iEditor->TextView()->HandleGlobalChangeL( yPosQualifier );

            iBaseControlObserver->HandleBaseControlEventRequestL(
                this, EMsgScrollForm, delta );
            }
        }
    }

// ---------------------------------------------------------
// CMsgBodyControl::ItemFinder
//
//
// ---------------------------------------------------------
//
CItemFinder* CMsgBodyControl::ItemFinder()
	{
	return IsReadOnly() ? iEditor->ItemFinder() : 
	                      NULL;
	}
	
// ---------------------------------------------------------
// CMsgBodyControl::SetupAutomaticFindAfterFocusChangeL
//
//
// ---------------------------------------------------------
//
void CMsgBodyControl::SetupAutomaticFindAfterFocusChangeL( TBool aBeginning )
    {
    BodyControlEditor()->SetupAutomaticFindAfterFocusChangeL( aBeginning );
    }

// ---------------------------------------------------------
// CMsgBodyControl::ResolveFontMetrics
//
//
// ---------------------------------------------------------
//
void CMsgBodyControl::ResolveFontMetrics()
    {
    TAknLayoutText textLayout;
    TAknTextLineLayout textLineLayout;
    
    TAknLayoutRect msgTextPane;
    msgTextPane.LayoutRect( MsgEditorCommons::MsgDataPane(),
                            AknLayoutScalable_Apps::msg_text_pane( 0 ).LayoutLine() );
    TAknLayoutRect msgBodyPane;
    msgBodyPane.LayoutRect( msgTextPane.Rect(),
                            AknLayoutScalable_Apps::msg_body_pane().LayoutLine() );
    
    if ( IsReadOnly() )
        {
        textLineLayout = AknLayoutScalable_Apps::msg_body_pane_t1( 0 ).LayoutLine();
        }
    else
        {
        textLineLayout = AknLayoutScalable_Apps::msg_body_pane_t1( 1 ).LayoutLine();
        }
    
    textLayout.LayoutText( msgBodyPane.Rect(),
                           textLineLayout );
                           
    iEditorTop = textLayout.TextRect().iTl.iY - msgBodyPane.Rect().iTl.iY;
    
    // Set editor alignment
    iEditor->SetAlignment( textLineLayout.iJ );
    
    iEditor->SetMaximumHeight( MsgEditorCommons::MaxBodyHeight() - iEditorTop );
    }

// ---------------------------------------------------------
// CMsgBodyControl::BodyControlEditor
// ---------------------------------------------------------
//
CMsgBodyControlEditor* CMsgBodyControl::BodyControlEditor() const
    {
    return static_cast<CMsgBodyControlEditor*>( iEditor );
    }

// ---------------------------------------------------------
// CMsgBodyControl::GetCaptionForFep
// Returns the ICF Prompt text of Body control to FEP
// ---------------------------------------------------------
//
void CMsgBodyControl::GetCaptionForFep( TDes& aCaption ) const
    {    
    const TInt maximumLength = aCaption.MaxLength();
    
    if ( iCaptionText->Length() < maximumLength )
        {
        TPtr ptr = iCaptionText->Des();
        aCaption.Copy( ptr ); 
        }                   
    }
// ---------------------------------------------------------
// CMsgBodyControl::SetIcfPromptTextL()
// Loads ICF Prompt text of Message Body control
// ---------------------------------------------------------
//
void CMsgBodyControl::SetIcfPromptTextL()
    {
    
    if(iCaptionText)
      {
      delete iCaptionText;    
      iCaptionText = NULL;            
      }        
    iCaptionText =   StringLoader::LoadLC( R_QTN_MSG_ICF_PROMPT_TEXT_MESSAGE, iCoeEnv );              
    CleanupStack::Pop( iCaptionText );

    }

//  End of File
