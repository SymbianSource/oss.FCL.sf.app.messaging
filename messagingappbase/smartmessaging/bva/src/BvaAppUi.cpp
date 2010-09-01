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
* Description:   Declares UI class for BVA application.
*
*/



#include <akntitle.h>
#include <msgbiocontrol.h>
#include <aknnotewrappers.h>      // CAknInformationNote
#include <bva.rsg>
#include "BvaContainer.h"
#include "BvaDocument.h"
#include "BvaAppUi.h"
#include "bvalog.h"

#include <featmgr.h>
#include <hlplch.h>                 // for HlpLauncher
#include <AiwCommon.hrh>
// CONSTANTS
const TInt KBvaFirstFreeCommandId = 10000;
_LIT(KBVAAppUI,"BVA AppUI");

// ================= MEMBER FUNCTIONS =======================
void CBvaAppUi::ConstructL()
    {
    LOG("CBvaAppUi::ConstructL begin");
    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );
    iContainer = new (ELeave) CBvaContainer;
    iContainer->ConstructL(ClientRect());
    AddToStackL( iContainer );
    iEikonEnv->AppUiFactory()->MenuBar()->SetContextMenuTitleResourceId( R_BVA_CONTEXT_MENUBAR );
    iFileHandleSize = -1;
	FeatureManager::InitializeLibL();
    
	//assume that the loading of BC can fail with any arbitrary
    //leave code or not to happen at all...
    iBioControlLoadFailed = ETrue;
	LOG("CBvaAppUi::ConstructL end");
    }

void CBvaAppUi::LoadBioControlL( const RFile& aFile )
	{
	LOG("CBvaAppUi::LoadBioControlL begin");
    TInt err0 = aFile.Size( iFileHandleSize );
	if ( err0 )
		{
		iFileHandleSize = 0;
		}

	TRAPD(err, iContainer->LoadBioControlL( aFile ));

    if( err == KErrMsgBioMessageNotValid )
        {
        // no note is displayed as that is handled by the launching application
        // Exit application.
        HandleCommandL(EAknCmdExit);
        }

    else if ( err == KErrNone )
        {
        //set the flag to EFalse as the BIO control has been instantiated
        //succesfully
        iBioControlLoadFailed = EFalse;

        //
        // Update the heading
        //
        HBufC* text = iContainer->BioControl().HeaderTextL();
        CleanupStack::PushL(text);
        CAknTitlePane* titlePane = STATIC_CAST(
            CAknTitlePane*,
            StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle )));
        titlePane->SetTextL( *text );
        CleanupStack::PopAndDestroy(text); // text
        }

    else
        {
        User::LeaveIfError(err);
        }
    LOG("CBvaAppUi::LoadBioControlL end");
	}

CBvaAppUi::~CBvaAppUi()
    {
	if (iContainer)
		{
		RemoveFromStack(iContainer);
		delete iContainer;
		}
    FeatureManager::UnInitializeLib();
    }

void CBvaAppUi::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane)
    {
	__ASSERT_ALWAYS(aMenuPane,Panic(EMenuPaneNull));
    if (aResourceId == R_BVA_MENU)
        {
        // disable aiw menu item for calender, enabled in vcalbc
        aMenuPane->SetItemDimmed( KAiwCmdSaveToCalendar, ETrue );

        // Check that datafile exist
        if (!NoDataFile())
            {
            TUint32 permBio = 0;
            if(iContainer->IsBioControl())
                {
                permBio = iContainer->BioControl().OptionMenuPermissionsL();
                }

            if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) || !( permBio & EMsgBioHelp ))
                {
                aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
                }
            if(iContainer->IsBioControl())
                {
                iContainer->BioControl().SetMenuCommandSetL(*aMenuPane);
                }
            }
        }
    else if ( aResourceId == R_BVA_CONTEXT_MENU )
        {
        // Check that datafile exist
        if ( !NoDataFile() )
            {
            if(iContainer->IsBioControl())
                {
                iContainer->BioControl().SetMenuCommandSetL( *aMenuPane );
                }
            }
        }

    }

TKeyResponse CBvaAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    if (NoDataFile())
        {
        // someone launched this app the wrong way.
        return EKeyWasNotConsumed;
        }

    return iContainer->OfferKeyEventL(aKeyEvent, aType);
    }

void CBvaAppUi::HandleCommandL(TInt aCommand)
    {
    LOG("CBvaAppUi::HandleCommandL begin");
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        case EAknCmdExit:
        case EEikCmdExit: // seems that currently it is this one
            {
            //closing the BVA application is not possible
            //until the BIO control has been created, unless the
            //instantiation of the control has failed altogether!
            if( iBioControlLoadFailed || iContainer->IsBioControl() )
                {
                if (iDoorObserver   )
                    {
                    iDoorObserver->NotifyExit(MApaEmbeddedDocObserver::ENoChanges);
                    }
                Exit();
                }
            break;
            }
        case EAknCmdHelp:
            {
            LaunchHelpL();
            break;
            }
        case EAknSoftkeyContextOptions:
            ProcessCommandL(EAknSoftkeyContextOptions);
        break;	
        default:
            break;
        }
    if ( aCommand >=  FirstFreeCommand() && !NoDataFile() )
        {
        iContainer->BioControl().HandleBioCommandL(aCommand);
        }
    LOG("CBvaAppUi::HandleCommandL end");
    }

TInt CBvaAppUi::FirstFreeCommand() const
    {
    return KBvaFirstFreeCommandId;
    }

TInt CBvaAppUi::RequestHandleCommandL(TMsgCommands aCommand)
    {

    switch(aCommand)
    	{
    	case EMsgClose:
    		{
    		HandleCommandL(EAknCmdExit);
    		break;
    		}
    	//intentional fallthrough
    	case EMsgNone:
    	case EMsgSave:
    	case EMsgReply:
    	case EMsgForwad:
    	case EMsgSend:
    	case EMsgAddressBook:
    		{
			break;
			}
		}

    return KErrNone;
    }

TBool CBvaAppUi::IsCommandSupported(TMsgCommands /*aCommand*/) const
    {
    //Nothing supported
    return EFalse;
    }

TBool CBvaAppUi::NoDataFile()
    {
	return (iFileHandleSize == 0);
    }

// ----------------------------------------------------------------------------
// CBvaAppUi::LaunchHelpL
// launch help using context
//
// ----------------------------------------------------------------------------
void CBvaAppUi::LaunchHelpL()
    {
    // activate Help application
    CArrayFix<TCoeHelpContext>* helpContext = AppHelpContextL();
    HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
    }


// ----------------------------------------------------------------------------
// CBvaAppUi::HelpContextL
// returns help context
//
// ----------------------------------------------------------------------------
CArrayFix<TCoeHelpContext>* CBvaAppUi::HelpContextL() const
	{

    TCoeHelpContext bioControlHelpContext;
	if(iContainer->IsBioControl())
	    {
    	iContainer->BioControl().GetHelpContext( bioControlHelpContext );

    	if(!bioControlHelpContext.IsNull())
			{
			return CreateHelpContextArrayL(bioControlHelpContext);
			}
		// if help context not available for bio control
		else
			{
			return NULL;
			}
	    }
	return NULL;
	}

CArrayFix<TCoeHelpContext>* CBvaAppUi::CreateHelpContextArrayL(
	const TCoeHelpContext& aHelpContext ) const
	{
	CArrayFix<TCoeHelpContext>* help =
		new(ELeave) CArrayFixFlat<TCoeHelpContext>(1);
	CleanupStack::PushL(help);
	help->AppendL(aHelpContext);
	CleanupStack::Pop(help);
	return help;
	}

void CBvaAppUi::Panic(TBVAPanic aCode)
    {
    User::Panic(KBVAAppUI, aCode);
    }


// End of File
