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
* Description:  View for setting additional mail headers
*
*/



// INCLUDE FILES
#include "CMsgAddMailHeadersDlg.h"
#include "MsgMailPreferences.h"
#include <StringLoader.h>
#include <MsgMailEditor.rsg>
#include <akncheckboxsettingpage.h>
#include <aknsettingitemlist.h>
#include <aknlistquerydialog.h>
#include "MailUtils.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgAddMailHeadersDlg::CMsgAddMailHeadersDlg
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgAddMailHeadersDlg::CMsgAddMailHeadersDlg(
	RPointerArray<TAdditionalHeaderStatus>& aHeaders): iHeaders(aHeaders)
    {
    }

// -----------------------------------------------------------------------------
// CMsgAddMailHeadersDlg::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgAddMailHeadersDlg* CMsgAddMailHeadersDlg::NewLC(
	RPointerArray<TAdditionalHeaderStatus>& aHeaders)
    {
    CMsgAddMailHeadersDlg* self = new(ELeave) CMsgAddMailHeadersDlg(aHeaders);
	CleanupStack::PushL(self);
    return self;
    }


// Destructor
CMsgAddMailHeadersDlg::~CMsgAddMailHeadersDlg()
    {
    }


// -----------------------------------------------------------------------------
// CMsgAddMailHeadersDlg::ExecuteDialogL
// Constructs a CSelectionList array and launches the setting view
// Collects the changed values to iHeaders array
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMsgAddMailHeadersDlg::ExecuteDialogL()
    {       
    TBool rc(EFalse);

    CArrayFixFlat<TInt>* arrayChoices = 
    	new(ELeave) CArrayFixFlat<TInt>( iHeaders.Count() );
    CleanupStack::PushL(arrayChoices);

    CAknListQueryDialog* dlg = new(ELeave) CAknListQueryDialog(arrayChoices);
    dlg->PrepareLC( R_ADDITIONAL_HEADERS_LIST_QUERY );

    CDesCArrayFlat* headersArray = CreateItemTextArrayLC( arrayChoices );
        
    dlg->SetItemTextArray( headersArray );
    dlg->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( headersArray );
        
    CEikListBox* listbox = dlg->ListBox();    
    listbox->SetSelectionIndexesL(arrayChoices);

    if(dlg->RunLD())
        {
        UpdateHeadersArrayL( arrayChoices );
        rc = ETrue;
        }

    CleanupStack::PopAndDestroy(arrayChoices);
    return rc;
    }

// -----------------------------------------------------------------------------
// CMsgAddMailHeadersDlg::CreateItemTextArrayLC
// -----------------------------------------------------------------------------
//
CDesCArrayFlat* CMsgAddMailHeadersDlg::CreateItemTextArrayLC(
    CArrayFixFlat<TInt>* aVisibleItems )
    {
    ASSERT( aVisibleItems );
    CDesCArrayFlat* headersArray = 
        new ( ELeave ) CDesCArrayFlat( iHeaders.Count() );
    CleanupStack::PushL( headersArray );
    
    for ( TInt index(0); index<iHeaders.Count(); ++index )
        {
        if( iHeaders[index]->iStatus == EHeaderVisible )
            {
            aVisibleItems->AppendL( index );
            }
        TInt resource(0);
        switch (iHeaders[index]->iHeaderValue)
            {
            case EMsgComponentIdCc:
                resource = R_QTN_MSG_HEADERS_CC;
                break;
            case EMsgComponentIdBcc:
                resource = R_QTN_MSG_HEADERS_BCC;
                break;
            case EMsgComponentIdSubject:
                resource = R_QTN_MSG_HEADERS_SUBJECT;
                break;
            default:
                __ASSERT_DEBUG( EFalse, User::Invariant() );               
            }
        HBufC* headerText = StringLoader::LoadLC( resource );
        headersArray->AppendL( * headerText );
        CleanupStack::PopAndDestroy(headerText); // headerText            
        }
    return headersArray;        
    }

// -----------------------------------------------------------------------------
// CMsgAddMailHeadersDlg::UpdateHeadersArrayL
// -----------------------------------------------------------------------------
//    
void CMsgAddMailHeadersDlg::UpdateHeadersArrayL(
    CArrayFixFlat<TInt>* aSelectedItems )
    {
    ASSERT( aSelectedItems );
    // Reset visibility
    for ( TInt index(0); index<iHeaders.Count(); ++index )
        {
        iHeaders[index]->iStatus = EHeaderHidden;
        }
    // Set selected items visible
    for ( TInt ii=0; ii<aSelectedItems->Count(); ii++ )
        {
        iHeaders[aSelectedItems->At(ii)]->iStatus = EHeaderVisible;
        }
    // Find invisible items that has some content
    TInt contentCount(0);
    for ( TInt xx=0;xx<iHeaders.Count();xx++ )
    	{
    	if( iHeaders[xx]->iStatus == 
    		EHeaderHidden && iHeaders[xx]->iHasContent )
    		{
    		contentCount++;
    		}
    	}     
    // We are removing control that has some content	    
    if( contentCount )
    	{
    	// plural?
    	TUint queryString = (contentCount > 1) ? 
    	    R_QTN_MSG_QRY_HEADERS_REMOVE : 
    	    R_QTN_MSG_QRY_HEADER_REMOVE;
            
       TInt queryResult = MailUtils::ConfirmationQueryL(
            queryString,
        	R_MAIL_EDITOR_DELETE_MESSAGE);
        
        // Do not remove control, only save visibility of controls
        if( !queryResult )
        	{
        	for ( TInt yy=0;yy<iHeaders.Count();yy++ )
    			{
    			if( iHeaders[yy]->iStatus == 
    				EHeaderHidden && iHeaders[yy]->iHasContent )
    				{
    				iHeaders[yy]->iStatus = EHeaderOnlySave;
    				}	
    			}
        	}       	
    	}
    
    }
//  End of File
