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
* Description:  Implements a dialog for view/edit sending options
*
*/


// INCLUDE FILES
#include "MsgMailUIDs.h"
#include "MsgMailOptionsDlg.h"
#include "MsgMailEditorAppUi.h"
#include "MsgMailEditorDocument.h"
#include "msgmaileditor.hrh"
#include "cmsgmaileditorsettinglist.h"
#include "csxhelp/mail.hlp.hrh"
#include <akntitle.h>
#include <hlplch.h>
#include <MsgEditor.hrh>
#include <MsgMailEditor.rsg>
#include <StringLoader.h>


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMsgMailOptionsDlg::NewL
// -----------------------------------------------------------------------------
CMsgMailOptionsDlg* CMsgMailOptionsDlg::NewL(CMsgMailEditorDocument* aDocument,
    TBool aHelpSupported)
    {
    ASSERT(aDocument);
    CMsgMailOptionsDlg* self = new( ELeave ) CMsgMailOptionsDlg;
    self->iDocument = aDocument;
    self->ConstructL();
    self->iHelpSupported = aHelpSupported;
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CMsgMailOptionsDlg::CMsgMailOptionsDlg()
    {}

// -----------------------------------------------------------------------------
// CMsgMailOptionsDlg::ConstructL()
// -----------------------------------------------------------------------------
inline void CMsgMailOptionsDlg::ConstructL()
    {
    CAknDialog::ConstructL( R_SEND_OPTIONS_MENUBAR );
    CEikStatusPane *sp =
        static_cast<CAknAppUi*> ( iEikonEnv->EikAppUi() )->StatusPane();
    iTitlePane = static_cast<CAknTitlePane *> ( sp->ControlL(
        TUid::Uid(EEikStatusPaneUidTitle)));

    iOldTitlePaneText = iTitlePane->Text()->AllocL();
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC(reader, R_SEND_OPTIONS_TITLE_TEXT);
    iTitlePane->SetFromResourceL(reader);
    CleanupStack::PopAndDestroy(); // CSI: 12 # reader
    }

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CMsgMailOptionsDlg::~CMsgMailOptionsDlg()
    {
    delete iSettingList;

    if ( iOldTitlePaneText && iTitlePane )
        {
        // Code scanner warning ignored, since existance of data
        // member is checked before accessing
        iTitlePane->SetText(iOldTitlePaneText); // CSI: 23 # See comment above
        }
    }

//-----------------------------------------------------------------------------
//  CMsgMailOptionsDlg::OkToExitL()
//-----------------------------------------------------------------------------
TBool CMsgMailOptionsDlg::OkToExitL(TInt aButtonId)
    {
    if ( aButtonId == EAknSoftkeyBack )
        {
        return ETrue;
        }
    else if( aButtonId == EMsgMailSendOptionsChange )
        {
        ProcessCommandL( aButtonId );
        return EFalse;
        }

    return CAknDialog::OkToExitL(aButtonId);
    }

//-----------------------------------------------------------------------------
//  CMsgMailOptionsDlg::ProcessCommandL()
//-----------------------------------------------------------------------------
void CMsgMailOptionsDlg::ProcessCommandL(TInt aCommandId)
    {
    const TBool calledFromMenu( MenuShowing() );
    CAknDialog::ProcessCommandL(aCommandId);

    if ( aCommandId == EAknCmdHelp && iHelpSupported )
        {
        
        CArrayFix<TCoeHelpContext>* help =
                            new( ELeave ) CArrayFixFlat<TCoeHelpContext>( 1 );
        
        TCoeHelpContext helpContext;
        

		helpContext = TCoeHelpContext( TUid::Uid( KUidMsgMailEditor ),
				KMAIL_HLP_SENDING_OPTIONS() );
		
		help->AppendL(helpContext);
		
		// Launch help
		HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
				help  );

        }
    else if ( aCommandId == EMsgMailEditorSendOptionsCmdExit )
        {
        TryExitL(EAknSoftkeyOk);
        }
	else if ( aCommandId == EMsgMailSendOptionsChange )
		{
        TInt index = iSettingList->ListBox()->CurrentItemIndex();
        iSettingList->EditItemL( index, calledFromMenu );
        //Update settings after editing. This needs to be done because
        //changing mailbox affects also on reply-to setting.
        iSettingList->LoadSettingsL();
        iSettingList->DrawNow();
        
        }
    }

//-----------------------------------------------------------------------------
//  CMsgMailOptionsDlg::OfferKeyEventL()
//-----------------------------------------------------------------------------
TKeyResponse CMsgMailOptionsDlg::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    ASSERT( iSettingList );
    
    TKeyResponse response = iSettingList->OfferKeyEventL( aKeyEvent, aType );
   	
    if ( response == EKeyWasNotConsumed )
        {
	    // Let the base class handle the event
	    response = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        }
        
    if (aType == EEventKey && aKeyEvent.iCode == EKeyEscape)
        { // must exit application (e.g. End key)
        ProcessCommandL(EMsgMailEditorSendOptionsCmdExit);
        }        
        
    return response;
    }

//-----------------------------------------------------------------------------
//  CMsgMailOptionsDlg::DynInitMenuPaneL()
//-----------------------------------------------------------------------------
void CMsgMailOptionsDlg::DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane)
    {
    if (aMenuId == R_SEND_OPTIONS_MENU)
        {
        aMenuPane->SetItemDimmed( EAknCmdHelp, !iHelpSupported);
        }
    }

//-----------------------------------------------------------------------------
// From CEikDialog
//-----------------------------------------------------------------------------
void CMsgMailOptionsDlg::PreLayoutDynInitL()
    {
    iSettingList = CMailEditorSettingList::NewL(
        *iDocument, R_SERVICE_SETTINGS_LIST );
    ASSERT( iSettingList );

    iSettingList->ListBox()->SetListBoxObserver( this );
    iSettingList->SetMopParent( this );
	iSettingList->MakeVisible( ETrue );
	iSettingList->ActivateL();
    }


//-----------------------------------------------------------------------------
//  CMsgMailOptionsDlg::SizeChanged()
//-----------------------------------------------------------------------------
void CMsgMailOptionsDlg::SizeChanged()
    {
    CAknDialog::SizeChanged();

    if ( iSettingList ) 
        {
        TRect parent;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, parent );
        AknLayoutUtils::LayoutControl(
            iSettingList, parent, AknLayout::list_gen_pane( 0 ) );
        }
    }

//-----------------------------------------------------------------------------
//  CMsgMailOptionsDlg::SizeChanged()
//-----------------------------------------------------------------------------    
void CMsgMailOptionsDlg::HandleResourceChange( TInt aType )
    {
    // We mustn't call SizeChanged for all resource changes since that
    // isn't needed for KAknsMessageSkinChange events and would slow down
    // skin change operation. CAknDialog and other controls can handle
    // resizing on their own.
    
    CAknDialog::HandleResourceChange(aType);
    
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    if ( statusPane )
		{
		statusPane->HandleResourceChange( aType );
		}	
    if ( iSettingList )
		{
		iSettingList->HandleResourceChange( aType );
		}    
    }

//-----------------------------------------------------------------------------
//  CMsgMailOptionsDlg::HandleListBoxEventL()
//-----------------------------------------------------------------------------    
void CMsgMailOptionsDlg::HandleListBoxEventL(CEikListBox* aListBox, 
        		TListBoxEvent aEventType)
	{
	iSettingList->HandleListBoxEventL( aListBox, aEventType );
	
	// Refresh settings listbox when items are edited
	if ( aEventType == EEventItemDoubleClicked )
		{
		iSettingList->LoadSettingsL();
		iSettingList->DrawNow();
		}
	
	}
//  End of File
