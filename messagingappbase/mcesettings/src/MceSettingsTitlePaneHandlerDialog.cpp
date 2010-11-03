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
*     Message centre's settings title pane handler
*
*/



// INCLUDE FILES

#include <StringLoader.h>   // StringLoader
#include <akntitle.h>       // CAknTitlePane
#include <aknViewAppUi.h>
#include "MceSettingsTitlePaneHandlerDialog.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CMceSettingsTitlePaneHandlerDialog::Constructor
// ----------------------------------------------------
CMceSettingsTitlePaneHandlerDialog::CMceSettingsTitlePaneHandlerDialog()
: CAknDialog()
    {
    }

// ----------------------------------------------------
// CMceSettingsTitlePaneHandlerDialog::Destructor
// ----------------------------------------------------
CMceSettingsTitlePaneHandlerDialog::~CMceSettingsTitlePaneHandlerDialog()
    {
    delete iOldTitleText;
    iOldTitleText = NULL;
    }

// ----------------------------------------------------
// CMceSettingsTitlePaneHandlerDialog::StoreTitleTextAndSetNewL
// ----------------------------------------------------
void CMceSettingsTitlePaneHandlerDialog::StoreTitleTextAndSetNewL( TInt aResourceId )
    {
    HBufC* text = StringLoader::LoadLC( aResourceId, iEikonEnv );
    CEikStatusPane* sp = ( (CAknAppUi*)iEikonEnv->EikAppUi() )
        ->StatusPane();
    // Fetch pointer to the default title pane control
    CAknTitlePane* title=(CAknTitlePane *)sp->ControlL(
        TUid::Uid( EEikStatusPaneUidTitle ) );

    delete iOldTitleText;
    iOldTitleText = NULL;
    if ( title )
        {
        iOldTitleText = ( *title->Text() ).Alloc();
        title->SetTextL( *text );
        }
    CleanupStack::PopAndDestroy( text ); // text
    }

// ----------------------------------------------------
// CMceSettingsTitlePaneHandlerDialog::StoreTitleTextAndSetNewL
// ----------------------------------------------------
void CMceSettingsTitlePaneHandlerDialog::StoreTitleTextAndSetNewL( const TDesC& aTitleText )
    {
    CEikStatusPane* sp = ((CAknAppUi*)iEikonEnv->EikAppUi())
        ->StatusPane();
    // Fetch pointer to the default title pane control
    CAknTitlePane* title=(CAknTitlePane *)sp->ControlL(
        TUid::Uid( EEikStatusPaneUidTitle ) );

    delete iOldTitleText;
    iOldTitleText = NULL;
    if ( title )
        {
        iOldTitleText = ( *title->Text() ).Alloc();
        title->SetTextL( aTitleText );
        }
    }

// ----------------------------------------------------
// CMceSettingsTitlePaneHandlerDialog::RestoreTitleTextL
// ----------------------------------------------------
void CMceSettingsTitlePaneHandlerDialog::RestoreTitleTextL()
    {
    if ( iOldTitleText )
        {
        CEikStatusPane* sp = ( (CAknAppUi*)iEikonEnv->EikAppUi() )
            ->StatusPane();
        // Fetch pointer to the default title pane control
        CAknTitlePane* title=(CAknTitlePane *)sp->ControlL(
            TUid::Uid( EEikStatusPaneUidTitle ) );

        title->SetTextL( iOldTitleText->Des() );
        }
	}

