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
* Description:  
*     Messaging Centre application "Delivery Reports" view class
*     implementation.
*
*/



// INCLUDE FILES


#include    <eikmenub.h>
#include    <aknappui.h>
#include    <AknQueryDialog.h>
#include    <featmgr.h>

#include    <avkon.hrh>
#include    <avkon.rsg>
#include    <Sendnorm.rsg>
#include    <eikdialg.h>
#include    <eikenv.h>
#include    <f32file.h>
#include    <StringLoader.h>
#include    <akntitle.h>
//#include    <CPhCltDialer.h>
#include    <mce.rsg>

#include    "MceDeliveryReportView.h"
#include    "MceDeliveryReportControlContainer.h"
#include    "MceApplication.h"
#include    "MceDocument.h"
#include    "mceui.h"
#include    "MceCommands.hrh"       // UID enumeration.
#include    "MceLogEngine.h"

#include	<commonphoneparser.h>
#include	<PhCltTypes.h>
#include	<MuiuMsvUiServiceUtilities.h>
#include	<e32cmn.h>
#include	<msvapi.h>
#include	"MceSessionHolder.h"

// For Muiu split
#include <muiumsvuiserviceutilitiesinternal.h>

// The position of the middle soft key
const TInt KMSKPosition = 3;


// ================= MEMBER FUNCTIONS =======================

CMceDeliveryReportView* CMceDeliveryReportView::NewL( CMceSessionHolder& aSessionHolder )
    {
    CMceDeliveryReportView* self = new(ELeave) CMceDeliveryReportView(aSessionHolder);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CMceDeliveryReportView::CMceDeliveryReportView( CMceSessionHolder& aSessionHolder ) : 
	iSessionHolder(aSessionHolder)
	{
	iSessionHolder.AddClient();
	}

CMceDeliveryReportView::~CMceDeliveryReportView()
    {
    iCoeEnv->RemoveForegroundObserver( *this );

    if (iContainer)
        {
        AppUi()->RemoveFromViewStack(*this,iContainer);

        // Destroy the control container
        delete iContainer;
        }

    
    ClosePopup();

    FeatureManager::UnInitializeLib();
    iSessionHolder.RemoveClient();
    }

CMceUi* CMceDeliveryReportView::MceAppUi()
    {
    // Explicit cast: the app ui must always be of type CMceUi.
    return STATIC_CAST( CMceUi*, CAknView::AppUi() );
    }


void CMceDeliveryReportView::HandleCommandL( TInt aCommandId )
    {
    switch (aCommandId )
        {
        case EMceCmdCall:
            {
            if( iPopup )
                {
                iPopup->StopDisplayingMenuBar();
                ClosePopup();
                }
            TInt index( 0 );
            index = iContainer->ListBox()->CurrentItemIndex();
            TPhCltTelephoneNumber tel;
            tel = MceAppUi()->LogEngine()->Event( index )->Number();

			// Check the validity of the phone number and initiate the call
            TBool validNumber = CommonPhoneParser::ParsePhoneNumber( tel, CommonPhoneParser::EPlainPhoneNumber );            
			if ( validNumber )
            	{
            	TPhCltNameBuffer recipientstring( KNullDesC );
            	iAvkonAppUi->SetKeyEventFlags( CAknAppUiBase::EDisableSendKeyShort | CAknAppUiBase::EDisableSendKeyLong );
			        MsvUiServiceUtilitiesInternal::CallToSenderQueryL( tel, recipientstring, EFalse );
			        iAvkonAppUi->SetKeyEventFlags( 0x00 );
              }
            }
            break;
        case EMceClearDrList:
            {
            CAknQueryDialog* queryDlg = CAknQueryDialog::NewL();
            if( queryDlg->ExecuteLD( R_MCE_CLEAR_LIST_CONFIRMATION_QUERY ) )
                {
                MceAppUi()->LogEngine()->ClearDrListL();
                }
            SetMSKButtonL();
            }
            break;
        case EAknCmdHideInBackground:
            // handled by app ui and sent directly to appui by avkon framework so ignore this by view.
            break;
        default:
            MceAppUi()->HandleCommandL( aCommandId );
            break;
        }    
    }


TUid CMceDeliveryReportView::Id() const
    {
    return KMceDeliveryReportViewId;
    }


void CMceDeliveryReportView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    TInt aPosition = -1;

    if( aMenuPane->MenuItemExists( EAknCmdHelp, aPosition ) )
        {
        // KFeatureIdHelp is used by Feature Mgr to check if help is supported
        if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, EFalse );
            }
        else
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
            }
        }

    switch( aResourceId )
        {
        case R_MCE_DELIVERY_REPORT_SYSTEM_MENU:

            if(  ! MceAppUi()->LogEngine()->
                    IsNumberAvailable( iContainer->ListBox()->CurrentItemIndex() ) )
                {
                aMenuPane->DeleteMenuItem( EMceCmdCall );
                }

            if ( iContainer->ListBox()->Model()->NumberOfItems() == 0 )
                {
                aMenuPane->DeleteMenuItem( EMceClearDrList );
                }
            break;

        default:
            break;
        }
    }


void CMceDeliveryReportView::HandleClientRectChange()
    {
    iContainer->SetRect(ClientRect());
    }

void CMceDeliveryReportView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    if ( iContainer == NULL )
        {
        // Create the control container
        iContainer = CMceDeliveryReportControlContainer::NewL(this, iSessionHolder, ClientRect() );
        AppUi()->AddToViewStackL(*this,iContainer);
        }
    iContainer->ListBox()->SetListBoxObserver(this);


    // Fetch pointer to the default title pane control
    CAknTitlePane* title=(CAknTitlePane *)StatusPane()->ControlL(
        TUid::Uid(EEikStatusPaneUidTitle));
    HBufC* text = StringLoader::LoadLC( R_MCE_DELIVERY_REPORTS_VIEW_TITLE );
    title->SetTextL( *text );
    CleanupStack::PopAndDestroy(); // text
    
    MceAppUi()->SetMceViewActive( EMceDeliveryReportsViewActive );
    
    // Just ignore if this leaves. Worst thing happens here is that user sees empty delivery reports view.
    TRAP_IGNORE( MceAppUi()->LogEngine()->ForegroundGainedL() );
    if( iContainer->LogDisabledL() == KMceDRLogingOff )
        {
        iContainer->PopUpNoteL();
        }
    }

void CMceDeliveryReportView::DoDeactivate()
    {
    if (iContainer)
        {
        // Remove view and its control from the view stack
        AppUi()->RemoveFromViewStack(*this, iContainer);

        // Destroy the container control
        delete iContainer;
        iContainer = NULL;
        }
    }

void CMceDeliveryReportView::ConstructL()
    {
    // Sets up TLS, must be done before FeatureManager is used.
    FeatureManager::InitializeLibL();

    // Init base class
    BaseConstructL( R_MCE_DELIVERY_REPORT_VIEW );
    iCoeEnv->AddForegroundObserverL( *this );
    }

void CMceDeliveryReportView::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType)
    {
    if ( aEventType == EEventItemSingleClicked )
    	{
    	ProcessCommandL( EAknSoftkeyContextOptions );
    	}
    }

void CMceDeliveryReportView::LaunchPopupMenuL( TInt aResourceId )
    {
    ClosePopup();
    iPopup = new ( ELeave ) CEikMenuBar;
    TRAPD( err, DoLaunchPopupL( aResourceId ) );
    if( err )
        {
        ClosePopup();
        User::Leave( err );
        }
    }

void CMceDeliveryReportView::ClosePopup()
    {
    if( iPopup )
        {
        AppUi()->RemoveFromViewStack( *this, iPopup );
        delete( iPopup );
        iPopup = NULL;
        }
    }

void CMceDeliveryReportView::DoLaunchPopupL( TInt aResourceId )
    {
    iPopup->ConstructL( this, 0, aResourceId );
    AppUi()->AddToViewStackL( *this, iPopup, ECoeStackPriorityMenu,
                                             ECoeStackFlagRefusesFocus );
    iPopup->SetMenuType(CEikMenuBar::EMenuContext);
    iPopup->TryDisplayMenuBarL();
    }


void CMceDeliveryReportView::HandleGainingForeground()
    {
     if ( MceAppUi()->MceViewActive( EMceDeliveryReportsViewActive ) )
        {
        // Just ignore if this leaves. Worst thing happens here is that user sees empty delivery reports view.
        TRAP_IGNORE( MceAppUi()->LogEngine()->ForegroundGainedL() );
        }    
    }

void CMceDeliveryReportView::HandleLosingForeground()
    {
    MceAppUi()->LogEngine()->ForegroundLost();
    }

void CMceDeliveryReportView::ProcessCommandL( TInt aCommand )
    {
	TInt index( 0 );
	TBool validNumber = EFalse ;
	index = iContainer->ListBox()->CurrentItemIndex();
	if ( index >= 0 )
		{
		TPhCltTelephoneNumber tel;
		tel = MceAppUi()->LogEngine()->Event( index )->Number();
		// Check the validity of the phone number and initiate the call
		validNumber = CommonPhoneParser::ParsePhoneNumber( tel, CommonPhoneParser::EPlainPhoneNumber );            
		}
    if ( validNumber && aCommand == EAknSoftkeyContextOptions )
        {
        aCommand = EAknSoftkeyOptions;
        MenuBar()->SetMenuTitleResourceId( R_MCE_DELIVERY_REPORT_OK_MENUBAR );
        MenuBar()->SetMenuType(CEikMenuBar::EMenuContext);
        }
    else
        {
        MenuBar()->SetMenuTitleResourceId( R_MCE_DELIVERY_REPORT_MENUBAR );
        MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);
        }
	    
    CAknView::ProcessCommandL( aCommand );
    }

void CMceDeliveryReportView::SetMSKButtonL()
    {
    TInt resourceId = R_MCE_MSK_BUTTON_CONTEXT_OPTIONS;
    if ( iContainer->ListBox()->Model()->NumberOfItems() == 0 ||
		!MceAppUi()->LogEngine()->
			IsNumberAvailable( iContainer->ListBox()->CurrentItemIndex() ) )
        {
        resourceId = R_MCE_MSK_BUTTON_EMPTY;
        }
    CEikButtonGroupContainer* cba = Cba();
    cba->SetCommandL( KMSKPosition, resourceId );
    cba->DrawDeferred();
    }

//  End of File
