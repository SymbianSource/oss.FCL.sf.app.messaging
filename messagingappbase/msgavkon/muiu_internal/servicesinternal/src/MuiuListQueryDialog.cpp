/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CMuiuListQueryDialog is used when SMS viewer or MMS viewer is open
*              : and user want to call back to sender or to phone number which is
*              : highlighted in message.
*
*/





//  INCLUDES
#include "MuiuListQueryDialog.h" 


// -----------------------------------------------------------------------------
// CMuiuListQueryDialog::CMuiuListQueryDialog
// 
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMuiuListQueryDialog::CMuiuListQueryDialog( TInt* aIndex )
:   CAknListQueryDialog( aIndex )
    {
    }


// -----------------------------------------------------------------------------
// CMuiuListQueryDialog::~CMuiuListQueryDialog
// 
// Destructor.
// -----------------------------------------------------------------------------
//
CMuiuListQueryDialog::~CMuiuListQueryDialog()
    {
    }


// -----------------------------------------------------------------------------
// CMuiuListQueryDialog::OfferKeyEventL
//  
//
// -----------------------------------------------------------------------------
//
TKeyResponse CMuiuListQueryDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                   TEventCode aType )
    {
    if ( aKeyEvent.iCode == EKeyYes )
        {
        TryExitL( EAknSoftkeyOk );
        return EKeyWasConsumed;
        }
    return CAknListQueryDialog::OfferKeyEventL( aKeyEvent, aType );
    }


// End of File
