/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*       Overwrite AknListQueryDialog to provide Imum a working touch dialog.
*
*/


#include "ImumListQueryDialog.h"

// ----------------------------------------------------------------------------
// CImumListQueryDialog::CImumListQueryDialog()
// ----------------------------------------------------------------------------
//
CImumListQueryDialog::CImumListQueryDialog( TInt* aIndex )
		: CAknListQueryDialog( aIndex )
    {
    iPreviousIndex = 0;
    }

// ----------------------------------------------------------------------------
// CImumListQueryDialog::~CImumListQueryDialog()
// ----------------------------------------------------------------------------
//
CImumListQueryDialog::~CImumListQueryDialog()
    {
	delete iIdle;
    }

// ----------------------------------------------------------------------------
// CImumListQueryDialog::HandleListBoxEventL()
// ----------------------------------------------------------------------------
//
void CImumListQueryDialog::HandleListBoxEventL( CEikListBox* aListBox,
                                                     TListBoxEvent aEventType )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        switch(aEventType)
            {
            case EEventItemDoubleClicked:
            case EEventItemClicked:
                {
                delete iIdle;
                iIdle = NULL;
                iIdle = CIdle::NewL( CActive::EPriorityIdle );

                if( iIdle && !aListBox->IsMultiselection() )
                	{
                	// if current focus is the same as previous focus
                	// then accept selection
                    TInt tempIndex = aListBox->CurrentItemIndex();
                    
                	if( iPreviousIndex != tempIndex )
	                	{
	                	iPreviousIndex = tempIndex;
	                	}
	                else
	                	{
	                	iIdle->Start( TCallBack( ClosePopupAcceptingChanges, this ) );
	                	}
                	}
                break;
                }
            default:
                {
                // Default functionality from base class
                CAknListQueryDialog::HandleListBoxEventL( aListBox, aEventType );
                break;
                }
            }
        }
    else
        {
        // Default functionality from base class
        CAknListQueryDialog::HandleListBoxEventL( aListBox, aEventType );
        }
    }

// ----------------------------------------------------------------------------
// CImumListQueryDialog::ClosePopup()
// ----------------------------------------------------------------------------
//
TInt CImumListQueryDialog::ClosePopup( TAny *aObj )
    {
    CImumListQueryDialog *popup = ( CImumListQueryDialog* )aObj;
    delete popup->iIdle;
    popup->iIdle = 0;
    popup->CloseState();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CImumListQueryDialog::ClosePopupAcceptingChanges()
// ----------------------------------------------------------------------------
//
TInt CImumListQueryDialog::ClosePopupAcceptingChanges( TAny *aObj )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        CImumListQueryDialog *popup = ( CImumListQueryDialog* )aObj;
        delete popup->iIdle;
        popup->iIdle = NULL;
        TRAP_IGNORE( popup->TryExitL( EAknSoftkeyOk ) );
        return EFalse;
        }
    else
        {
        return EFalse;
        }
    }

// End of File
