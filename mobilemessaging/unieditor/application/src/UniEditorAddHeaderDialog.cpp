/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This is used when user wants to change the additional header fields.   
*
*/



//  INCLUDES
#include <aknlists.h>
#include <UniEditor.rsg>
#include "UniEditorAddHeaderDialog.h" 


// -----------------------------------------------------------------------------
// CUniEditorAddHeaderDialog::CUniEditorAddHeaderDialog
// -----------------------------------------------------------------------------
//
CUniEditorAddHeaderDialog::CUniEditorAddHeaderDialog( 
                CListBoxView::CSelectionIndexArray* aSelectionIndexArray ) :   
    CAknListQueryDialog( aSelectionIndexArray )
    {
    }

// -----------------------------------------------------------------------------
// CUniEditorAddHeaderDialog::~CUniEditorAddHeaderDialog
// -----------------------------------------------------------------------------
//
CUniEditorAddHeaderDialog::~CUniEditorAddHeaderDialog()
    {
    }

// -----------------------------------------------------------------------------
// CUniEditorAddHeaderDialog::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CUniEditorAddHeaderDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                        TEventCode aType )
    {
    if( aType == EEventKeyUp )
        {
        UpdateMskL();
        }

    return CAknListQueryDialog::OfferKeyEventL( aKeyEvent, aType );
    }

// -----------------------------------------------------------------------------
// CUniEditorAddHeaderDialog::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CUniEditorAddHeaderDialog::OkToExitL( TInt aButtonId )
    {
    if( aButtonId == EAknSoftkeyMark || aButtonId == EAknSoftkeyUnmark)
        {
        // It seems something has been changed in Avkon so the next line is not needed anymore
        // ToggleL(); // But if this does not work in the future, just uncomment this line
        return EFalse;
        }
    else
        {
        return CAknListQueryDialog::OkToExitL( aButtonId );
        }
    }

// -----------------------------------------------------------------------------
// CUniEditorAddHeaderDialog::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CUniEditorAddHeaderDialog::ProcessCommandL( TInt /*aCommandId*/ )
    {
    }

// -----------------------------------------------------------------------------
// CUniEditorAddHeaderDialog::UpdateMskL
// -----------------------------------------------------------------------------
//
void CUniEditorAddHeaderDialog::UpdateMskL()
    {
    TInt resId = 0;
    TBool selected = IsCurrentSelected();

    if( selected )
        { 
        // It's unmarkable
        resId = R_UNI_MSK_ADDHEADERS_BUTTON_UNMARK;
        }
    else
        { 
        // It's markable
        resId = R_UNI_MSK_ADDHEADERS_BUTTON_MARK;
        }
        
    CEikButtonGroupContainer& cba = ButtonGroupContainer();
    const TInt KMskPosition = 3;
    cba.SetCommandL( KMskPosition, resId );
    cba.DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CUniEditorAddHeaderDialog::ToggleL
// -----------------------------------------------------------------------------
//
void CUniEditorAddHeaderDialog::ToggleL()
    {
    TInt currentItem = View()->CurrentItemIndex();
    CEikListBox* listBox = ListBox();
    
    if( IsCurrentSelected() )
        {
        View()->DeselectItem( currentItem);
        }
    else
        {
        View()->SelectItemL( currentItem);    
        }
        
    listBox->HandleItemAdditionL();
    }

// -----------------------------------------------------------------------------
// CUniEditorAddHeaderDialog::IsCurrentSelected
// -----------------------------------------------------------------------------
//
TBool CUniEditorAddHeaderDialog::IsCurrentSelected()
    {
    return View()->ItemIsSelected( View()->CurrentItemIndex() );
    }

// -----------------------------------------------------------------------------
// CUniEditorAddHeaderDialog::View
// -----------------------------------------------------------------------------
//
CListBoxView* CUniEditorAddHeaderDialog::View()
    {
    return ListBox()->View();
    }

// End of File
