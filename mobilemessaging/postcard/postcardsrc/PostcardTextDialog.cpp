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
*       Provides CPostcardTextDialog class methods.
*
*/



// INCLUDE FILES
#include <e32def.h>
#include <AknDef.h>
#include <aknappui.h>
#include <featmgr.h>                // for Feature Manager
#include <bldvariant.hrh>           // for Feature Flags
#include <eikmenub.h>               // for CEikMenuBar
#include <ConeResLoader.h>          // for RConeResourceLoader
#include <data_caging_path_literals.hrh> 
#include <csxhelp/postcard.hlp.hrh>

// AVKON
#include <akntitle.h>               // for CAknTitlePane
#include <AknIndicatorContainer.h>	// for CAknIndicatorContainer
#include <aknEditStateIndicator.h>	// for MAknEditStateIndicator
#include <aknenv.h>                 // for AppShutter
#include <AknWaitDialog.h>          // CAknWaitNoteWrapper
#include <aknnotewrappers.h>        // CAknInformationNote
#include <AknColourSelectionGrid.h> // CAknColourSelectionGrid
#include <aknlayout.cdl.h>
#include <AknUtils.h>
#include <applayout.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

// For drawing
#include <AknsConstants.h>           // For skinnable icons
#include <AknsBasicBackgroundControlContext.h> // For skin stuff, too
#include <AknsDrawUtils.h>

// Richtextstuff
#include <eikrted.h>
#include <txtrich.h>
#include <txtetext.h>

// COMMON ENGINE
#include <StringLoader.h>           // for StringLoader::

// MSG EDITOR BASE
#include <MsgEditorAppUi.rsg>
#include <MsgEditorCommon.h>
#include "MsgAttachmentUtils.h"		// for fetchdialogs

#include <NpdApi.h>                 // Notepad API

#include <mmsmsventry.h>

#include <centralrepository.h>      // link against centralrepository.lib
#include <MmsEngineInternalCRKeys.h>// for Central Repository keys

#include "MsgAttachmentUtils.h"

#include <Postcard.rsg>
#include "Postcard.hrh"
#include "PostcardLaf.h"
#include "PostcardTextDialog.h"
#include "PostcardDocument.h"
#include "PostcardAppUi.h"
#include "PostcardTextDialogCustomDraw.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
//  Two-phased constructor
// ---------------------------------------------------------
CPostcardTextDialog* CPostcardTextDialog::NewL( CPostcardAppUi& aAppUi, CPostcardDocument& aDocument, CPlainText& aText, TInt& aRetValue, TKeyEvent aKeyEvent )
    {
    CPostcardTextDialog* self = 
        new (ELeave) CPostcardTextDialog( aAppUi, aDocument, aText, aRetValue, aKeyEvent );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
//  Default destructor
// ---------------------------------------------------------
CPostcardTextDialog::~CPostcardTextDialog()
    {
    // Remove the navidecorator from status pane
    CEikStatusPane *sp = iEikonEnv->AppUiFactory( )->StatusPane( );
    TRAP_IGNORE(
        static_cast<CAknNavigationControlContainer*>
            ( sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) )->Pop( );
    ); // TRAP_IGNORE
    
    delete iNaviDecorator;
    delete iTextEditor;
    delete iCustomDraw;
    }

// ---------------------------------------------------------
// CPostcardTextDialog
// ---------------------------------------------------------
CPostcardTextDialog::CPostcardTextDialog( 
			CPostcardAppUi& aAppUi, 
			CPostcardDocument& aDocument, 
			CPlainText& aText, 
			TInt& aRetValue, 
			TKeyEvent aKeyEvent  ) :
    iAppUi( aAppUi ),   
    iDocument( aDocument ),
    iOrigText( aText ),
    iKeyEvent( aKeyEvent ),
    iRetValue( aRetValue )
    {
    } 

// ---------------------------------------------------------
// ExecLD()
// ---------------------------------------------------------
TInt CPostcardTextDialog::ExecLD( )
    {
    PrepareLC( R_POSTCARD_TEXT_DIALOG );
        
    return CAknDialog::RunLD( );
    }

// ---------------------------------------------------------
//  EPOC constructor
// ---------------------------------------------------------
void CPostcardTextDialog::ConstructL( )
    {
    // Feature manager
    FeatureManager::InitializeLibL( );

    if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        iTextFlags |= EPostcardTextHelpSupported;
        }

    FeatureManager::UnInitializeLib( );

    iMaxLen = iAppUi.MaxTextLength( EPostcardTextEditor );

    // Call the base class' two-phased constructor
    CAknDialog::ConstructL( R_POSTCARD_TEXT_MENUBAR );

    iMenuBar->SetContextMenuTitleResourceId( R_POSTCARD_TEXT_CONTEXT_MENUBAR );
    }

// ----------------------------------------------------
// CPostcardTextDialog::PreLayoutDynInitL
// ----------------------------------------------------
void CPostcardTextDialog::PreLayoutDynInitL( )
    {
    TInt edwinFlags =
        CEikEdwin::ENoHorizScrolling
        | CEikEdwin::ENoAutoSelection
        | CEikEdwin::EWidthInPixels
        | CEikEdwin::EAllowUndo
        | CEikEdwin::ELineCursor
        | CEikEdwin::EResizable
        | CEikEdwin::EInclusiveSizeFixed;

    if( iDocument.MessageType() == EPostcardSent )
    	{
		edwinFlags |= CEikEdwin::EReadOnly;
		edwinFlags |= CEikEdwin::EAvkonDisableCursor;
    	}

    iTextEditor = new (ELeave) CEikRichTextEditor( TGulBorder( TGulBorder::EShallowRaised ) );
    iTextEditor->ConstructL(    this, 
                                0, 
                                iMaxLen, 
                                edwinFlags );    

    TRect mainPane = PostcardLaf::WholeMainPaneForDialogs();
    TInt height = mainPane.Height( );
    TInt diff = height%MsgEditorCommons::MsgBaseLineDelta();
    height -= diff; // decrease the height if it is not dividable by baselinedelta
    mainPane.iBr.iY = height;

    TMargins8 edwinMargins;
    edwinMargins.SetAllValuesTo(2);
    // This needs to be before SetRect() in order to take effect
    iTextEditor->SetBorderViewMargins( edwinMargins );

    // Set scrollbar size
    TInt sbWidth = PostcardLaf::ScrollBarWidth();
    TRect edwinPane = mainPane;
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        edwinPane.iTl.iX += sbWidth;
        }
    else
        {
        edwinPane.iBr.iX -= sbWidth;
        }
    iTextEditor->SetRect( edwinPane );

    SetCharParaFormat();

    CAknAppUiBase* appUi = static_cast<CAknAppUiBase*>( iEikonEnv->EikAppUi() );
    if (AknLayoutUtils::DefaultScrollBarType(appUi) == CEikScrollBarFrame::EDoubleSpan)
        { // Create scrollbar only if double scrollbars supported
        iTextEditor->CreateScrollBarFrameL( );
        iTextEditor->ScrollBarFrame( )->SetTypeOfVScrollBar(CEikScrollBarFrame::EDoubleSpan);
        iTextEditor->ScrollBarFrame( )->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EOn);
        }

    // Layout scrollbar correctly with text editor.
    AknLayoutUtils::LayoutVerticalScrollBar(
        iTextEditor->ScrollBarFrame(),
        mainPane,
        AknLayoutScalable_Apps::scroll_pane_cp06().LayoutLine() );


    iTextEditor->SetContainerWindowL( *this );

	UpdateNavipaneL( ETrue );
    }

// ----------------------------------------------------
// CPostcardTextDialog::PostLayoutDynInitL
// ----------------------------------------------------
void CPostcardTextDialog::PostLayoutDynInitL()
    {
    TInt docLength = iOrigText.DocumentLength();
    if( docLength > 0 )
    	{
    	const TPtrC ptr = iOrigText.Read( 0, docLength );
    	iTextEditor->SetTextL( &ptr );
    	}

    // Set middle softkey button and menu resource to show on button press
    SetMiddleSoftkeyL();
   }

// ---------------------------------------------------------
// HandleEdwinEventL
// ---------------------------------------------------------
void CPostcardTextDialog::HandleEdwinEventL(CEikEdwin* aEdwin,TEdwinEvent aEventType)
	{
	if( aEdwin && aEventType == EEventTextUpdate )
		{ // The functionality has been removed to HandleControlEventL
		}
	}

// ---------------------------------------------------------
// Draw
// ---------------------------------------------------------
void CPostcardTextDialog::Draw(const TRect& /*aRect*/ ) const
	{
    CWindowGc& gc = SystemGc();
    TRect rect( PostcardLaf::WholeMainPaneForDialogs( ) );

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
    gc.SetPenColor( AKN_LAF_COLOR( 12 ) );
	}

// ---------------------------------------------------------
// UpdateNavipaneL
// ---------------------------------------------------------
void CPostcardTextDialog::UpdateNavipaneL( TBool aForce )
	{
	if( !aForce )
		{
		return;
		}

	if( iDocument.MessageType( ) == EPostcardSent ) 
	    {
	    if( !iNaviDecorator )
	        {
            CEikStatusPane *sp = iEikonEnv->AppUiFactory( )->StatusPane( );
	        
	        CAknNavigationControlContainer* container = 
                static_cast<CAknNavigationControlContainer*>
                    ( sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
	        
            HBufC* buf = HBufC::NewLC(0);
            iNaviDecorator = container->CreateMessageLabelL( *buf );
            iNaviDecorator->MakeScrollButtonVisible( EFalse );
            
            container->PushL( *iNaviDecorator );
            CleanupStack::PopAndDestroy( buf ); // buf
	        }
	    return;
	    }

    MAknEditingStateIndicator* editIndi = 
        static_cast< CAknEnv* >( iEikonEnv->Extension( ) )->EditingStateIndicator();
	if( editIndi )
		{
        CAknIndicatorContainer* indiContainer = editIndi->IndicatorContainer( );
        if ( indiContainer )
            {

			TInt cur = iTextEditor->TextLength( );
	        CArrayFixFlat<TInt>* array = new(ELeave) CArrayFixFlat<TInt>( 2 );
	        CleanupStack::PushL( array );
	        array->AppendL( cur );
	        array->AppendL( iMaxLen );
	        HBufC* paneString = 
	            StringLoader::LoadL( R_POSTCARD_CHARACTER_COUNTER, *array, iCoeEnv );
	        CleanupStack::PopAndDestroy( array ); //array
	        CleanupStack::PushL( paneString );

            indiContainer->SetIndicatorState(
                TUid::Uid( EAknNaviPaneEditorIndicatorMessageLength ),  
                EAknIndicatorStateOn );
            indiContainer->SetIndicatorValueL(
                TUid::Uid( EAknNaviPaneEditorIndicatorMessageLength ), 
                *paneString );
                
	        CleanupStack::PopAndDestroy( paneString );   //paneString
            }
		
		}
	}

// ---------------------------------------------------------
// SetCharParaFormat
// ---------------------------------------------------------
void CPostcardTextDialog::SetCharParaFormat()
    {
    // CParaFormat can be created on the stack if it is not
    // used to store tab stops, bullets or borders
    CParaFormat paraFormat;
    TParaFormatMask paraFormatMask;
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;

	PostcardLaf::ActiveFont( charFormat, charFormatMask, paraFormat, paraFormatMask );

    CParaFormatLayer* paraFormatLayer = NULL;
    CCharFormatLayer* charFormatLayer = NULL;

    TRAPD( error,
        {
        paraFormatLayer = CParaFormatLayer::NewL( &paraFormat, paraFormatMask );
        charFormatLayer = CCharFormatLayer::NewL( charFormat, charFormatMask );
        } );

	if( error )
		{
        delete paraFormatLayer;
        delete charFormatLayer;
        return;
		}
	// ownership moves
	iTextEditor->SetParaFormatLayer( paraFormatLayer );
	iTextEditor->SetCharFormatLayer( charFormatLayer );
    }

// ---------------------------------------------------------
// CPostcardTextDialog::ProcessCommandL( TInt aCommandId )
// ---------------------------------------------------------
void CPostcardTextDialog::ProcessCommandL( TInt aCommandId )
    {
    HideMenu( );

    switch ( aCommandId )
        {
        case EPostcardCmdSend:
            SendL( );
            break;
            
        case EPostcardCmdForward:
            ForwardMessageL( );
            break;

       	case EPostcardCmdMessageInfo:
			MessageInfoL( );
        	break;

       	case EPostcardCmdInsertTemplate:
       		InsertFromL( EFalse );
        	break;
        	
       	case EPostcardCmdInsertNote:
	       	InsertFromL( ETrue );
        	break;
        	
        case EMsgDispSizeAutomatic:
        case EMsgDispSizeLarge:
        case EMsgDispSizeNormal:        	
        case EMsgDispSizeSmall:
            iAppUi.HandleLocalZoomChangeL( (TMsgCommonCommands)aCommandId );
            break;  
        case EAknCmdHelp: 
            LaunchHelpL( );
            break;

        case EAknCmdExit:
        case EEikCmdExit:
            if( OkToExitL( aCommandId ) )
                {
            	iRetValue = EPostcardDialogExit;	
                TryExitL( EAknSoftkeyDone );
                }
            break;

        default :
            CAknDialog::ProcessCommandL( aCommandId );
            break;
        }
    }

// ---------------------------------------------------------
// OfferKeyEventL
// ---------------------------------------------------------
TKeyResponse CPostcardTextDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    // If a menu is showing offer key events to it.
    if ( CAknDialog::MenuShowing( ) )
        {
        return CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        }

    TUint code = aKeyEvent.iCode;

    TKeyResponse resp = EKeyWasNotConsumed;

    switch ( code )
        {
        case EKeyOK:
            {
            if ( iDocument.MessageType() != EPostcardSent )
                {
                iMenuBar->SetMenuType( CEikMenuBar::EMenuContext );
                iMenuBar->TryDisplayContextMenuBarL();                
                }
            resp = EKeyWasConsumed;
            break;
            }

        case EKeyBackspace:
        case EKeyDelete:
            {
            resp = iTextEditor->OfferKeyEventL( aKeyEvent, aType );
            // Has to call draw here. Otherwise a couple of characters are left
            // on the screen at the beginning of each line.
            iTextEditor->DrawDeferred();
            UpdateNavipaneL( ETrue );
            CheckTextExceedingL();
            break;
            }
        
        case EKeyYes:
            {
        	if( iDocument.MessageType( ) != EPostcardSent ) 
        	    { // This is draft message
        	    SendL();
       			resp = EKeyWasConsumed;   
   			    break;
        	    }
        	// If message is sent -> intentionally let it fall thru
            }
            //lint -fallthrough
        case EKeyApplication:
        case EKeyPhoneEnd:
        case EKeyPowerOff:
        case EKeyGripOpen:
        case EKeyGripClose:
        case EKeySide:
        case EKeyEscape:
            resp = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
            break;
 
        default:
            {
            resp = iTextEditor->OfferKeyEventL( aKeyEvent, aType );
       		UpdateNavipaneL( ETrue );
    		CheckTextExceedingL();
            }
        }
    return resp;
    }

// ---------------------------------------------------------
// DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane)
// ---------------------------------------------------------
void CPostcardTextDialog::DynInitMenuPaneL(
    TInt aMenuId, 
    CEikMenuPane* aMenuPane)
    {
    switch ( aMenuId )
        {
        case R_POSTCARD_TEXT_OPTIONS:
            DynInitMainMenuL( aMenuPane );
            break;

        case R_POSTCARD_TEXT_CONTEXT_OPTIONS:
            DynInitContextMenuL( aMenuPane );
            break;

        case R_POSTCARD_ZOOM_SUBMENU:
            {
            TInt zoomLevel = KErrGeneral;
            iAppUi.GetZoomLevelL( zoomLevel );
            switch ( zoomLevel )
                {
                case EAknUiZoomAutomatic:
                    aMenuPane->SetItemButtonState( EMsgDispSizeAutomatic,
                                     EEikMenuItemSymbolOn );
                    break;
                case EAknUiZoomNormal:
                    aMenuPane->SetItemButtonState( EMsgDispSizeNormal,
                                     EEikMenuItemSymbolOn );
                    break;
                case EAknUiZoomSmall:
                    aMenuPane->SetItemButtonState( EMsgDispSizeSmall,
                                     EEikMenuItemSymbolOn );
                    break;
                case EAknUiZoomLarge:
                    aMenuPane->SetItemButtonState( EMsgDispSizeLarge,
                                     EEikMenuItemSymbolOn );
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }

    }

// ---------------------------------------------------------
// DynInitMainMenuL
// ---------------------------------------------------------
void CPostcardTextDialog::DynInitMainMenuL( CEikMenuPane* aMenuPane )
    {
    aMenuPane->SetItemDimmed( EPostcardCmdSend, 
        IsMainMenuOptionInvisible( EPostcardCmdSend ));
    aMenuPane->SetItemDimmed( EPostcardCmdInsertSubmenu, 
        IsMainMenuOptionInvisible( EPostcardCmdInsertSubmenu ));
    aMenuPane->SetItemDimmed( EPostcardCmdForward, 
            IsMainMenuOptionInvisible( EPostcardCmdForward ));
    aMenuPane->SetItemDimmed( EPostcardCmdMessageInfo, 
        IsMainMenuOptionInvisible( EPostcardCmdMessageInfo ));
    aMenuPane->SetItemDimmed( EAknCmdHelp, iTextFlags & EPostcardTextHelpSupported?EFalse:ETrue ); 
    aMenuPane->SetItemDimmed( EAknCmdExit, EFalse ); 
    }
    
// ---------------------------------------------------------
// IsMainMenuOptionInvisible
// ---------------------------------------------------------
TBool CPostcardTextDialog::IsMainMenuOptionInvisible( TInt aMenuOptionId )
    {
    TBool retVal = ETrue;
    TPostcardMsgType msgType = iDocument.MessageType( );

    switch ( aMenuOptionId )
        {

        case EPostcardCmdSend:
       		if( msgType != EPostcardSent ) 
       			{
       			if( iAppUi.ContactHasData( ) )
       			    {
    	            retVal = EFalse;
       			    }
       			}
            break;
        case EPostcardCmdInsertSubmenu:
       		if( msgType != EPostcardSent ) 
       			{
 				if( iTextEditor->TextLength( ) < iMaxLen )
 					{
		            retVal = EFalse;					
 					}
       			}
   			break;
        case EPostcardCmdForward:
            if( msgType == EPostcardSent ) 
                {
                retVal = EFalse;                
                }
            break;
       	case EPostcardCmdMessageInfo:
       	    {
			retVal = EFalse;
        	break;        	
       	    }
        case EAknCmdHelp: 
        	if( iTextFlags & EPostcardTextHelpSupported )
        		{
	        	retVal = EFalse;
        		}
            break;

        case EAknCmdExit:
        	retVal = EFalse;
            break;        	
        default:
            ;
        }

    return retVal;
    }


// ---------------------------------------------------------
// DynInitInsertSubMenuL
// ---------------------------------------------------------
void CPostcardTextDialog::DynInitInsertSubMenuL( CEikMenuPane* aMenuPane )
    { // These are always shown when the submenu is shown
    aMenuPane->SetItemDimmed( EPostcardCmdInsertTemplate, EFalse );
    aMenuPane->SetItemDimmed( EPostcardCmdInsertNote, EFalse );
    }

// ---------------------------------------------------------
// DynInitContextMenuL
// ---------------------------------------------------------
void CPostcardTextDialog::DynInitContextMenuL( CEikMenuPane* aMenuPane )
    { // This is always shown when context menu is supported
    TPostcardMsgType msgType = iDocument.MessageType( );
	if( msgType != EPostcardSent ) 
		{
	    aMenuPane->SetItemDimmed( EPostcardCmdInsertSubmenu, 
	    			iTextEditor->TextLength() < iMaxLen ? EFalse : ETrue );
        aMenuPane->SetItemDimmed( EPostcardCmdForward, ETrue );
	    aMenuPane->SetItemDimmed( EPostcardCmdMessageInfo, ETrue );
		}
	else
		{
	    aMenuPane->SetItemDimmed( EPostcardCmdInsertSubmenu, ETrue );
        aMenuPane->SetItemDimmed( EPostcardCmdForward, EFalse );
	    aMenuPane->SetItemDimmed( EPostcardCmdMessageInfo, EFalse );
		}
    }

// ---------------------------------------------------------
// CountComponentControls
// ---------------------------------------------------------
TInt CPostcardTextDialog::CountComponentControls( ) const
    {
    TInt count = 0;
    
    if ( iTextEditor )
        {
        count++;
        }

    return count;
    }

// ---------------------------------------------------------
// ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
CCoeControl* CPostcardTextDialog::ComponentControl( TInt /*aIndex*/ ) const
    {
    CCoeControl* control = NULL;
    
    if ( iTextEditor )
        {
        control = iTextEditor;
        }
        
    return control;
    }

// ---------------------------------------------------------
// OkToExitL(TInt aButtonId)
// called by framework when softkey button is pressed
// ---------------------------------------------------------
TBool CPostcardTextDialog::OkToExitL( TInt aButtonId )
    {
    if( aButtonId == EAknCmdExit )
        {
        return ETrue;
        }
    if( aButtonId == EAknSoftkeyContextOptions )
        {
        // Middle softkey was pressed. Display context menu.
        // MSK is not active in viewer mode.
        iMenuBar->TryDisplayContextMenuBarL();
        return EFalse;
        }
    TBool okToExit = CAknDialog::OkToExitL( aButtonId );
    if( !okToExit )
    	{
    	return okToExit;
    	}
    if( aButtonId == EAknSoftkeyBack ||
    	aButtonId == EAknSoftkeyExit || 
    	aButtonId == EAknSoftkeyDone ||
    	aButtonId == EAknCmdExit ||
    	aButtonId == EEikCmdExit ||
    	aButtonId == EAknSoftkeyCancel ||
    	aButtonId == EPostcardCmdSend )
    	{
    	if( iDocument.MessageType( ) != EPostcardSent )
    		{
		    CPlainText* text = CPlainText::NewL( CEditableText::ESegmentedStorage, KPostcardDefaultGreetingSegment );
	    	CleanupStack::PushL( text );
	    	
			if( iTextEditor->TextLength() )
				{
		    	text->InsertL( 0, iTextEditor->GlobalText( )->Read( 0, iTextEditor->TextLength( ) ) );
				}
	    	iAppUi.SetTextL( *text );
	        // Greeting changed, so we can change field to modified.
            iDocument.SetChanged( EPostcardText );
	    	CleanupStack::Pop( text ); //ownership moves to AppUi
    		}
    	return okToExit;
    	}
    return EFalse;
    }

// ---------------------------------------------------------
// ActivateL()
// ---------------------------------------------------------
void CPostcardTextDialog::ActivateL( )
    {
    SetFocus( ETrue );
    iTextEditor->SetFocus( ETrue );
    if( iDocument.MessageType() == EPostcardSent )
    	{
	    iTextEditor->TextView()->SetCursorVisibilityL( 
	    		TCursor::EFCursorInvisible, TCursor::EFCursorInvisible );
         // Prevent text exceeding confirmation query in viewer mode
        iTextFlags |= EPostcardTextExceeded;
    	}
	else
		{
	    iTextEditor->TextView()->SetCursorVisibilityL( 
	    		TCursor::EFCursorFlashing, TCursor::EFCursorFlashing );
    	}
    iTextEditor->SetObserver( this ); // MCoeControlObserver
    CCoeControl::ActivateL();
    // Swap the custom drawer to our own to show text lines. 
    // Lines are shown only in editor mode.
    // Custom drawer must be set after call to CCoeControl::ActivateL.
    if ( iDocument.MessageType() != EPostcardSent )
        {
        if ( !iCustomDraw )
            {
            const MFormCustomDraw* customDraw = iTextEditor->TextLayout()->CustomDraw();

            iCustomDraw = CPostcardTextDialogCustomDraw::NewL( customDraw, this );
        
            iTextEditor->TextLayout()->SetCustomDraw( iCustomDraw );
            }
        }
    // Set cursor to the end only if it is not sent.
    if( iDocument.MessageType() != EPostcardSent &&
        iTextEditor->TextLength() > 0 )
    	{
	 	iTextEditor->SetCursorPosL( iTextEditor->TextLength(), EFalse );		
    	}
    UpdateNavipaneL( ETrue );
	if( iKeyEvent.iCode != 0 )
		{
		TEventCode type = EEventKey;
        iCoeEnv->SyncNotifyFocusObserversOfChangeInFocus();
        iCoeEnv->SimulateKeyEventL( iKeyEvent, type );						
		}
    CheckTextExceedingL();
    }

// ---------------------------------------------------------
// Panic
// ---------------------------------------------------------
void CPostcardTextDialog::Panic( TInt aReason )
    {
    User::Panic( _L("CPostcardTextDialog"), aReason);
    }

// ---------------------------------------------------------
// HandleResourceChange
// ---------------------------------------------------------
void CPostcardTextDialog::HandleResourceChange(TInt aType )
    {
    CAknDialog::HandleResourceChange( aType );

	if(aType == KEikDynamicLayoutVariantSwitch )
        {
		if( iTextEditor )
			{
            TRect mainPane = PostcardLaf::WholeMainPaneForDialogs();
            TInt height = mainPane.Height( );
            TInt diff = height%MsgEditorCommons::MsgBaseLineDelta();
            height -= diff; // decrease the height if it is not dividable by baselinedelta
            mainPane.iBr.iY = height;

            // Set scrollbar size
            TInt sbWidth = PostcardLaf::ScrollBarWidth();
            TRect edwinPane = mainPane;
            if ( AknLayoutUtils::LayoutMirrored() )
                {
                edwinPane.iTl.iX += sbWidth;
                }
            else
                {
                edwinPane.iBr.iX -= sbWidth;
                }
            iTextEditor->SetRect( edwinPane );

            // Layout scrollbar correctly with text editor.
            AknLayoutUtils::LayoutVerticalScrollBar(
                iTextEditor->ScrollBarFrame(),
                mainPane,
                AknLayoutScalable_Apps::scroll_pane_cp06().LayoutLine() );

            SetCharParaFormat();

            TRAP_IGNORE( iTextEditor->NotifyNewFormatL() );
			}
	    // Line rects needs also refreshing.
	    if ( iCustomDraw )
	        {
	        iCustomDraw->ResolveLayouts();
	        }
        }
	else if( aType == KAknsMessageSkinChange )
		{
		if( iTextEditor )
			{
            SetCharParaFormat( );
			}
		}
    }

// ---------------------------------------------------------
// FocusChanged
// ---------------------------------------------------------
void CPostcardTextDialog::FocusChanged( TDrawNow aDrawNow )
    {
    if ( iTextEditor )
        {
        iTextEditor->SetFocus( IsFocused(), aDrawNow );
        }
    }
 
// ---------------------------------------------------------
// SendL
// ---------------------------------------------------------
void CPostcardTextDialog::SendL( )
	{
	iRetValue = EPostcardDialogSend;	
    TryExitL( EPostcardCmdSend );		
	}

// ---------------------------------------------------------
// ForwardMessageL
// ---------------------------------------------------------
void CPostcardTextDialog::ForwardMessageL( )
    {
    iRetValue = EPostcardDialogForward;    
    TryExitL( EAknSoftkeyDone );        
    }

// ---------------------------------------------------------
// MessageInfoL
// ---------------------------------------------------------
void CPostcardTextDialog::MessageInfoL( )
	{
    TPostcardMsgType msgType = iDocument.MessageType( );
	if( msgType != EPostcardSent ) 
        { // We are dealing with unsent message -> save recipient details for now
        TMmsMsvEntry tEntry = static_cast<TMmsMsvEntry>( iDocument.Entry() );
        tEntry.iDate.UniversalTime();
        tEntry.iDescription.Set( iTextEditor->Text( )->Read( 0, iTextEditor->TextLength( ) ) );
    	tEntry.iBioType = KUidMsgSubTypeMmsPostcard.iUid;
        iDocument.CurrentEntry().ChangeL( tEntry );
        }		
	iAppUi.DoShowMessageInfoL( );
	}

// ---------------------------------------------------------
// InsertFromL
// ---------------------------------------------------------
void CPostcardTextDialog::InsertFromL( TBool aNote )
    {
    
    TInt resId = NULL;
    
    if( aNote )
        {
        resId = R_QTN_NMAKE_FETCH_MEMO_PRMPT;
        }
    else
        {
        resId = R_QTN_MAIL_POPUP_TEXT_SEL_TEMP;        
        }
    
    HBufC* title = iAppUi.ReadFromResourceLC( resId );
    HBufC* buf = NULL;
        
    if( aNote )
        { // Read it from Notepad memos
        buf = CNotepadApi::FetchMemoL( title );
        if( !buf )
            { // Cancel or empty
            CleanupStack::PopAndDestroy( title ); // title
            return;
            }
        CleanupStack::PushL( buf );
        }
    else
        { // Read it from Notepad templates
        buf = CNotepadApi::FetchTemplateL( title );
        if( !buf )
            { // Cancel or empty
            CleanupStack::PopAndDestroy( title ); // title
            return;
            }
        CleanupStack::PushL( buf );
        }
    
    TCursorSelection selection = iTextEditor->TextView( )->Selection( );
    const TInt selLength = selection.Length( );

    if ( selLength )
        {
        iTextEditor->ClearSelectionL( );
        iTextEditor->RichText( )->DeleteL( selection.LowerPos( ), selLength );
        TCursorSelection sel( selection.LowerPos( ), selection.LowerPos( ) );
        iTextEditor->TextView( )->HandleInsertDeleteL( sel, selLength );        
        }
    iTextEditor->RichText( )->InsertL( selection.LowerPos( ), *buf );
    if ( buf->Length() > 1 )
        {
        TCursorSelection sel( selection.LowerPos( ) + buf->Length( ), selection.LowerPos( ) );
        iTextEditor->TextView( )->HandleInsertDeleteL( sel, 0 );
        }
    else
        {
        iTextEditor->TextView( )->HandleCharEditL( CTextLayout::EFCharacterInsert );
        }
    TInt newPos=selection.iCursorPos + buf->Length( );
    iTextEditor->SetCursorPosL( newPos,EFalse );

    CleanupStack::PopAndDestroy( 2, title ); // title, buf
    UpdateNavipaneL( ETrue );
    CheckTextExceedingL( );
    
    }

// ---------------------------------------------------------
// LaunchHelpL
// ---------------------------------------------------------
void CPostcardTextDialog::LaunchHelpL( )
	{
	if( iDocument.MessageType( ) == EPostcardSent ) 
	    {
        iAppUi.LaunchHelpL( KPOSTCARD_HLP_GREETING_VIEW() );
	    }
    else
        {
        iAppUi.LaunchHelpL( KPOSTCARD_HLP_GREETING_EDIT() );    
        }	
	}

// ---------------------------------------------------------
// CheckTextExceedingL
// ---------------------------------------------------------
void CPostcardTextDialog::CheckTextExceedingL( )
	{
	if( iTextEditor->TextLength( ) >= iMaxLen )
		{
		TInt howMany = iTextEditor->TextLength( ) - iMaxLen;
		switch( howMany )
			{
			case 0:
				{
                // No note shown anymore.
				break;
				}
			case 1:
				{
				if( !(iTextFlags & EPostcardTextExceeded ) ) 
					{
    				iAppUi.ShowConfirmationQueryL( R_POSTCARD_GREETING_EXCEEDED, ETrue );
					}
				break;					
				}
			default:
				{
				if( !(iTextFlags & EPostcardTextExceeded ) ) 
					{
    		        HBufC* warningString = 
    		            StringLoader::LoadLC( R_POSTCARD_GREETING_EXCEEDED_MANY, howMany, iCoeEnv );
    				iAppUi.ShowConfirmationQueryL( *warningString, ETrue );
    		        CleanupStack::PopAndDestroy( warningString );
					}
				break;					
				}
			}
		iTextFlags |= EPostcardTextExceeded;			
		}
	else
	    {
	    iTextFlags &= ~EPostcardTextExceeded;
	    }
	SetMiddleSoftkeyL();
	}

// ---------------------------------------------------------
// HandleControlEventL
// ---------------------------------------------------------
void CPostcardTextDialog::HandleControlEventL(CCoeControl* /*aControl*/,TCoeEvent aEventType)
    {
    if( aEventType == EEventStateChanged )
        {
		UpdateNavipaneL( ETrue );
		//  The length check is done in OfferKeyEventL
        }
    }
    

// ---------------------------------------------------------
// SetMiddleSoftkeyL
// ---------------------------------------------------------
void CPostcardTextDialog::SetMiddleSoftkeyL()
    {
    TInt resourceId;
    
    if( iDocument.MessageType() != EPostcardSent && 
        iTextEditor->TextLength() < iMaxLen )
        {
        resourceId = R_POSTCARD_MSK_BUTTON_OPTIONS;
        }
    else
        {
        resourceId = R_POSTCARD_MSK_BUTTON_EMPTY;
        }
        
    if ( resourceId != iMskResource )
        {
        const TInt KMskPosition = 3;
        CEikButtonGroupContainer& cba = ButtonGroupContainer();
        cba.SetCommandL( KMskPosition, resourceId );
        cba.DrawDeferred();
        iMskResource = resourceId;
        }
    }

// End of File

