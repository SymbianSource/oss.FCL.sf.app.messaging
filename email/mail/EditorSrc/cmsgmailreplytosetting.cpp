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
* Description:  Reply-to setting item and setting page
*
*/


#include "cmsgmailreplytosetting.h"
#include "MsgMailEditorDocument.h"
#include "MailLog.h"
#include "MailUtils.h"
#include <MuiuMsvUiServiceUtilities.h>
#include <MsgMailEditor.rsg>
#include <StringLoader.h>

// const
const TInt KMaxEmailAddressLength(100); // KStringLength

inline void CMsgMailEditorReplyToSettingItem::ConstructL()
    {  
    SetSettingValueL();
    }

CMsgMailEditorReplyToSettingItem* 
    CMsgMailEditorReplyToSettingItem::NewL( 
    TInt aIdentifier, CMsgMailEditorDocument& aDocument )
    {
    CMsgMailEditorReplyToSettingItem* self =
        new( ELeave ) CMsgMailEditorReplyToSettingItem( 
        aIdentifier,  aDocument );
    CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(self); // self
    return self;
    }


CMsgMailEditorReplyToSettingItem::CMsgMailEditorReplyToSettingItem(
    TInt aIdentifier,
    CMsgMailEditorDocument& aDocument ): 
    CAknTextSettingItem( aIdentifier, iSettingValue ),  
    iDocument( aDocument )
    {
    // iSettingValue is set later   
    }        

CMsgMailEditorReplyToSettingItem::~CMsgMailEditorReplyToSettingItem()
    {
    iSettingValue.Close();
    }

// ----------------------------------------------------------------------------
//  // From CAknTextSettingItem
// ----------------------------------------------------------------------------
//        
void CMsgMailEditorReplyToSettingItem::EditItemL( 
    TBool /* aCalledFromMenu */ )
    {
    TPtrC settingName = SettingName();
    
    SetInitialValueL();
    CAknTextSettingItem::LoadL(); // update setting value to listbox
    SetSettingPageFlags( CAknTextSettingPage::EZeroLengthAllowed );
        
    CAknSettingPage* dlg = new( ELeave )CMsgMailEditorReplyToSettingPage(        
        &settingName, 
        SettingNumber(), 
        EEikCtTextButton,
        SettingEditorResourceId(), 
        SettingPageResourceId(), 
        InternalTextPtr(), 
        SettingPageFlags() );
    SetSettingPage( dlg );
    SettingPage()->SetSettingPageObserver( this ); // not used
    SettingPage()->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted );

    SaveSettingValueL();
    SetSettingValueL();
    CAknTextSettingItem::LoadL(); // update setting value to listbox
    UpdateListBoxTextL();    
    SetSettingPage( 0 ); // it is deleted now
    }
    
// ----------------------------------------------------------------------------
//  // From CAknTextSettingItem
// ----------------------------------------------------------------------------
//
void CMsgMailEditorReplyToSettingItem::LoadL()
    {
    SetSettingValueL();
    CAknTextSettingItem::LoadL();
    }

// ----------------------------------------------------------------------------
//  Save to settings
// ----------------------------------------------------------------------------
//  
void CMsgMailEditorReplyToSettingItem::SaveSettingValueL()
    {
    // get store for the message
    CMsvStore* store = iDocument.CurrentEntry().EditStoreL();
    CleanupStack::PushL(store);

    CImHeader& header = iDocument.HeaderL();
    TPtr editedText = InternalTextPtr();
#if defined (_UNICODE)
    header.SetReplyToL( editedText );
#else    
    HBufC8* replyto = HBufC8::NewLC( editedText.Length() );
    replyto->Des().Copy( editedText );
    header.SetReplyToL( *replyto );
    CleanupStack::PopAndDestroy( replyto );    
#endif // _UNICODE   
    iDocument.SetChanged( ETrue );
    
    // store message header
    header.StoreL( *store );
    store->CommitL();
    CleanupStack::PopAndDestroy( store );  
    }

// ----------------------------------------------------------------------------
//  Set current reply-to address to setting list
// ----------------------------------------------------------------------------
//  
void CMsgMailEditorReplyToSettingItem::SetSettingValueL()
    {
    // Reply to address
   	CImHeader& header = iDocument.HeaderL();

    iSettingValue.Close();
    iSettingValue.CreateL( header.ReplyTo() );
    }
    
// ----------------------------------------------------------------------------
//  Set initial value to editor
// ----------------------------------------------------------------------------
//  
void CMsgMailEditorReplyToSettingItem::SetInitialValueL()
    {
    // alloc new buffer for new value
    iSettingValue.Close();
    iSettingValue.CreateL( KMaxEmailAddressLength );
    
    // current Reply to address
   	CImHeader& header = iDocument.HeaderL();    
    iSettingValue.Append( header.ReplyTo() );
    }
    
CMsgMailEditorReplyToSettingPage::CMsgMailEditorReplyToSettingPage(
    TInt aResourceID,
    TDes& aText, 
    TInt aTextSettingPageFlags )
    : CAknTextSettingPage( 
        aResourceID, aText, aTextSettingPageFlags )
	{
	}

CMsgMailEditorReplyToSettingPage::CMsgMailEditorReplyToSettingPage(	
    const TDesC* aSettingText, 
    TInt aSettingNumber, 
    TInt aControlType,
    TInt aEditorResourceId, 
    TInt aResourceID,
    TDes& aText, 
    TInt aTextSettingPageFlags )
    : CAknTextSettingPage( 
      aSettingText, 
      aSettingNumber,
      aControlType, 
      aEditorResourceId, 
      aResourceID,
      aText,
      aTextSettingPageFlags )
    {
    }

CMsgMailEditorReplyToSettingPage::~CMsgMailEditorReplyToSettingPage()
	{
	}

// ----------------------------------------------------------------------------
//  From CAknTextSettingPage
// ----------------------------------------------------------------------------
// 
TBool CMsgMailEditorReplyToSettingPage::OkToExitL( TBool aAccept )
    {
    if ( !aAccept )
        {
        return ETrue;
        }    
    
    TBool retVal( ETrue );
    HBufC* editedText = TextControl()->GetTextInHBufL();
    if ( editedText && editedText->Length() )
        {
        CleanupStack::PushL( editedText );
        retVal = MsvUiServiceUtilities::IsValidEmailAddressL( *editedText );
        CleanupStack::PopAndDestroy( editedText );
        }
    if ( !retVal )
        {
        MailUtils::InformationNoteL( R_QTN_MCE_ERROR_MISSING_CHARACTER );
        // Highlight the email address if it is in a wrong format.
        TextControl()->SelectAllL();
        }
   
    return retVal;
    }
// End of File
