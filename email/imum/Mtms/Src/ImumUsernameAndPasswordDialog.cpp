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
* Description: 
*       CImumUsernameAndPasswordDialog implementation file.
*
*
*/


// INCLUDE FILES
#include <layoutmetadata.cdl.h>

#include "ImumUsernameAndPasswordDialog.h"
#include "ImumMtmLogging.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CImumUsernameAndPasswordDialog::CImumUsernameAndPasswordDialog( TTone aTone /*= ENoTone*/ )
: CAknMultiLineDataQueryDialog( aTone )
    {
    IMUM_CONTEXT( CImumUsernameAndPasswordDialog::CImumUsernameAndPasswordDialog, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }


// Two-phased constructor.
CImumUsernameAndPasswordDialog* CImumUsernameAndPasswordDialog::NewL(
    TDes& aUsername, TDes& aPassword )
    {
    IMUM_STATIC_CONTEXT( CImumUsernameAndPasswordDialog::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CImumUsernameAndPasswordDialog* dialog=new(ELeave) CImumUsernameAndPasswordDialog();
    CleanupStack::PushL(dialog);
    dialog->SetDataL(aUsername, aPassword);
    CleanupStack::Pop(dialog);
    IMUM_OUT();
    return dialog;
    }

// Destructor
CImumUsernameAndPasswordDialog::~CImumUsernameAndPasswordDialog()
    {
    IMUM_CONTEXT( CImumUsernameAndPasswordDialog::~CImumUsernameAndPasswordDialog, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }

void CImumUsernameAndPasswordDialog::SetInitialCurrentLine()
    {
    IMUM_CONTEXT( CImumUsernameAndPasswordDialog::SetInitialCurrentLine, 0, KImumMtmLog );
    IMUM_IN();
    
    CAknMultiLineDataQueryDialog::SetInitialCurrentLine();

    // If the username contains data...
    if ( FirstData( TPtr( 0,0 ) ). Length() > 0 )
        {
        // ...For portrait mode, multiline query is shown
        if ( !Layout_Meta_Data::IsLandscapeOrientation() )
            {
            // In case the username is already set, switch the
            // focus to password line
            TRAP_IGNORE( TryChangeFocusToL( EMultilineSecondLine ) );
            }
        // ...In landscape mode, username and password queries are
        // shown separately
        else
            {
            // Show the ok key
            TRAP_IGNORE( UpdateLeftSoftKeyL() );
            }
        }
    IMUM_OUT();
    }



//  End of File



