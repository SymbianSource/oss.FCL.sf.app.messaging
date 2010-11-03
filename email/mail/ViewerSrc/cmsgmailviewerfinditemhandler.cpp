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
* Description: Handles CItemFinder and CFindItemMenu for mail viewer.
*
*/

#include "MailLog.h"
#include <MsgMailViewer.rsg>
#include "msgmailviewer.hrh"
#include "cmsgmailviewerfinditemhandler.h"
#include "cmsgmailviewercontactmatcher.h"
#include <AiwContactAssignDataTypes.h>  // aiw
#include <AiwServiceHandler.h>          // aiw
#include <finditem.hrh>
#include <FindItemmenu.rsg>
#include <finditemmenu.h>
#include <finditemdialog.h>

// LOCAL CONSTANTS

// ================= MEMBER FUNCTIONS =======================
//

// ---------------------------------------------------------------------------
// CMsgMailViewerFindItemHandler::NewL
// ---------------------------------------------------------------------------
//
CMsgMailViewerFindItemHandler* CMsgMailViewerFindItemHandler::NewL( 
    CItemFinder* aItemFinder )
    {
    CMsgMailViewerFindItemHandler* self = new( 
        ELeave ) CMsgMailViewerFindItemHandler( aItemFinder );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CMsgMailViewerFindItemHandler
// ---------------------------------------------------------------------------
//
CMsgMailViewerFindItemHandler::CMsgMailViewerFindItemHandler(
    CItemFinder* aItemFinder )
    : iFindMode( CItemFinder::ENoneSelected ),
      iItemFinder( aItemFinder )
    {
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerFindItemHandler::~CMsgMailViewerFindItemHandler()
// Destructor
// Frees reserved resources
// ------------------------------------------------------------------------------
//
CMsgMailViewerFindItemHandler::~CMsgMailViewerFindItemHandler()
    {
    LOG("~CMsgMailViewerFindItemHandler destructor");
    delete iFindItemMenu;
    LOG("~CMsgMailViewerFindItemHandler destructor end");
    }


// ------------------------------------------------------------------------------
// CMsgMailViewerFindItemHandler::ConstructL()
// Symbian OS constructor
// ------------------------------------------------------------------------------
//
void CMsgMailViewerFindItemHandler::ConstructL()
    {
    LOG("CMsgMailViewerFindItemHandler::ConstructL");
    SetFindModeL( iFindMode ); // make sure that finder is really in initial mode
    iFindItemMenu = CFindItemMenu::NewL( EMsgMailViewerCmdFindItem );
    iFindItemMenu->AttachItemFinderMenuL( R_MSGMAILVIEWER_OPTIONS_MENU2 );
    iFindItemMenu->SetSenderDescriptorType( CItemFinder::EEmailAddress );    
#if defined(__VOIP) && defined(RD_VOIP_REL_2_2)
    //hide call menu
    iFindItemMenu->SetMenuItemVisibility( CFindItemMenu::ECallItem, EFalse );
#endif // __VOIP && RD_VOIP_REL_2_2
	LOG("CMsgMailViewerFindItemHandler::ConstructL -> End");
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerFindItemHandler::::DynInitMenuPaneL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerFindItemHandler::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane,
    const TDesC& aAddress,
    const TDesC& aName,
    TMsgControlId aFocusedControl )
    {
#if defined(__VOIP) && defined(RD_VOIP_REL_2_2)  
    if ( aFocusedControl == EMsgComponentIdBody )
        {
        //set call menu visible
        iFindItemMenu->SetMenuItemVisibility( CFindItemMenu::ECallItem, ETrue );
        }
    else
        {
        //hide call menu     
        iFindItemMenu->SetMenuItemVisibility( CFindItemMenu::ECallItem, EFalse );
        }
#endif // __VOIP && RD_VOIP_REL_2_2

    if ( aResourceId == R_MSGMAILVIEWER_OPTIONS_MENU2 ||
         aResourceId == R_MSGMAILVIEWER_FINDITEM_MENU)
        {
        // If body is not focused, then we must not give CItemFinder
        // to Find Item -> that way it shows options applicable
        // for the e-mail address given in aAddress
        CItemFinder* itemFinder = NULL;
        if ( aFocusedControl == EMsgComponentIdBody )
            {
            itemFinder = iItemFinder;
            }

        // If FindMode is ENoneSelected(=find deactivated) give empty address
        // string for iFindItemMenu
        const TDesC& address( FindMode() == CItemFinder::ENoneSelected ?
                              KNullDesC : aAddress );
          
        TBool isContextMenu( aResourceId == R_MSGMAILVIEWER_FINDITEM_MENU ?
                             ETrue : EFalse );

        TInt index( isContextMenu ?
                    EFindItemContextMenuPlaceHolder :
                    EFindItemMenuPlaceHolder );
        
        // although method names here refer to "sender", the same functionality
        // applies to any focused address
        iFindItemMenu->SetSenderHighlightStatus(
            IsAddressControl( aFocusedControl ) );
        const TDesC& displayText( aName.Length() > 0 ? aName : aAddress );
        iFindItemMenu->SetSenderDisplayText( displayText );
        iFindItemMenu->AddItemFindMenuL( itemFinder,
                                         aMenuPane,
                                         index,
                                         address,
                                         EFalse,
                                         isContextMenu );
        }
    else
        {
        // all unknown menu panes should be given to UpdateItemFinderMenuL
        iFindItemMenu->UpdateItemFinderMenuL( aResourceId, aMenuPane );
        }
    }

TBool CMsgMailViewerFindItemHandler::IsAddressControl(
    TMsgControlId aFocusedControl ) const
    {    
	return ( aFocusedControl == EMsgComponentIdTo ||
        	 aFocusedControl == EMsgComponentIdFrom ||
        	 aFocusedControl == EMsgComponentIdCc ||
        	 aFocusedControl == EMsgComponentIdBcc );
    }
		 
// ------------------------------------------------------------------------------
// CMsgMailViewerFindItemHandler::SelectionType()
// ------------------------------------------------------------------------------
//
CItemFinder::TItemType
CMsgMailViewerFindItemHandler::SelectionType( TBool aIsFocused ) const
    {
    CItemFinder::TItemType itemType( CItemFinder::ENoneSelected );
    if ( iItemFinder && aIsFocused )
        {        
        itemType = iItemFinder->CurrentItemExt().iItemType;
        }
    return itemType;
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerFindItemHandler::SelectionText()
// ------------------------------------------------------------------------------
//
const TDesC& CMsgMailViewerFindItemHandler::SelectionText(
    TBool aIsFocused ) const
    {
    if ( SelectionType( aIsFocused ) == CItemFinder::ENoneSelected )
        {
        return KNullDesC;
        }
    else
        {
        return *( iItemFinder->CurrentItemExt().iItemDescriptor );
        }
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerFindItemHandler::SetFindModeL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerFindItemHandler::SetFindModeL( TInt aFindFlags )
    {
    if ( iItemFinder )
        {
        iItemFinder->SetFindModeL( aFindFlags );
        iFindMode = aFindFlags;
        }
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerFindItemHandler::FindMode()
// ------------------------------------------------------------------------------
//
TInt CMsgMailViewerFindItemHandler::FindMode() const
    {
    return iFindMode;
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerFindItemHandler::CanHandleCommand()
// ------------------------------------------------------------------------------
//
TBool CMsgMailViewerFindItemHandler::CanHandleCommand( TInt aCommand ) const
    {
    TBool returnValue = EFalse;
    TRAP_IGNORE( returnValue = iFindItemMenu->CommandIsValidL( aCommand ) );
    return returnValue;
    }
    
// ------------------------------------------------------------------------------
// CMsgMailViewerFindItemHandler::HandleFinderCommandL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerFindItemHandler::HandleFinderCommandL( TInt aCommand )
    {
    iFindItemMenu->HandleItemFinderCommandL( aCommand );
    }

// End of File
