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
* Description:  Remote mailbox setting item and setting page
*
*/


#include "cmsgmailremotemailboxsetting.h"
#include "MsgMailEditorDocument.h"
#include "MailLog.h"
#include <MsgMailEditor.rsg>
#include <StringLoader.h>
#include <ImumInternalApi.h>
#include <ImumInHealthServices.h>
#include <muiuflags.h>
#include <cemailaccounts.h>				// CEmailAccounts

inline void CMsgMailEditorRemoteMailboxSettingItem::ConstructL()
    {
    SetSettingValueL();
    }

CMsgMailEditorRemoteMailboxSettingItem* 
    CMsgMailEditorRemoteMailboxSettingItem::NewL( 
    TInt aIdentifier, CMsgMailEditorDocument& aDocument )
    {
        CMsgMailEditorRemoteMailboxSettingItem* self =
            new( ELeave ) CMsgMailEditorRemoteMailboxSettingItem( 
            aIdentifier,  aDocument );
        CleanupStack::PushL( self );
    	self->ConstructL();
    	CleanupStack::Pop(self); // self
        return self;
    }


CMsgMailEditorRemoteMailboxSettingItem::CMsgMailEditorRemoteMailboxSettingItem(
    TInt aIdentifier,
    CMsgMailEditorDocument& aDocument ): 
    CAknTextSettingItem( aIdentifier, iSettingValue ),  
    iDocument( aDocument )
    {
    // iSettingValue is set later
    }        

CMsgMailEditorRemoteMailboxSettingItem::~CMsgMailEditorRemoteMailboxSettingItem()
    {
    iSettingValue.Close();
    delete iMailBoxIdArray;
    delete iServiceArray;
    }

// ----------------------------------------------------------------------------
//  // From CAknTextSettingItem
// ----------------------------------------------------------------------------
//        
void CMsgMailEditorRemoteMailboxSettingItem::EditItemL( 
    TBool /* aCalledFromMenu */ )
    {
    TPtrC settingName = SettingName();
    TInt currentSelection( KErrNotFound );
    NameArrayL( currentSelection );
  
    CAknSettingPage* dlg = new( ELeave )CAknRadioButtonSettingPage(        
        &settingName, 
        SettingNumber(), 
        EEikCtTextButton,
        SettingEditorResourceId(), 
        SettingPageResourceId(), 
        currentSelection, 
        iMailBoxIdArray );
    SetSettingPage( dlg );
    SettingPage()->SetSettingPageObserver( this ); 
    SettingPage()->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted );
   
    SaveSettingValueL( currentSelection );
    SetSettingValueL();
    LoadL(); // update setting value to listbox
    UpdateListBoxTextL();    
    SetSettingPage( 0 ); // it is deleted now
    }


// ----------------------------------------------------------------------------
//  Create name array
// ----------------------------------------------------------------------------
//    
void CMsgMailEditorRemoteMailboxSettingItem::NameArrayL(
    TInt& aSelected )
    {
    LOG("CMsgMailEditorRemoteMailboxSettingItem::NameArrayL");
    
    // TImumInMboxListRequest does not work so we need to do this hard way
    // POP & IMAP service iRelatedId points to STMP service, so we use that
    TInt64 flags = MImumInHealthServices::EFlagSetHealthyReceiving;
    MImumInHealthServices::RMailboxIdArray mbox = 
        iDocument.GetHealthyMailboxListL( flags );
    TInt itemCount = mbox.Count();
    
    //+1 if itemCount=0
    delete iMailBoxIdArray;
    iMailBoxIdArray = NULL;
    iMailBoxIdArray = new(ELeave)CDesCArrayFlat(itemCount+1);
        
    delete iServiceArray;
    iServiceArray = NULL;
    iServiceArray = new(ELeave) CMsvEntrySelection;
          
    TMsvId id;
    TMsvEntry entry;
    CMsvSession& session = iDocument.Session();   
    for(TInt i=0; i<itemCount; i++)
        {
        session.GetEntry(mbox[i], id, entry);
        iMailBoxIdArray->AppendL(entry.iDetails);
        if ( entry.iDetails == SettingTextL() )
            {
            // set selection index
            aSelected = iMailBoxIdArray->Count() - 1;
            }
        iServiceArray->AppendL( entry.iRelatedId );  
        }
    ASSERT( iServiceArray->Count() == iMailBoxIdArray->Count() ); 
    }

// ----------------------------------------------------------------------------
//  Save selected mbox id to settings
// ----------------------------------------------------------------------------
//  
void CMsgMailEditorRemoteMailboxSettingItem::SaveSettingValueL( 
    TInt aSelected )
    {
    ASSERT( iServiceArray );
    if( aSelected != KErrNotFound )
        {
        ASSERT( aSelected >= 0 && aSelected < iServiceArray->Count() );
        iDocument.SendOptions().SetServiceId( (*iServiceArray)[aSelected] );
        iDocument.SetChanged( ETrue );
        
        //Update header because mailbox is selected.
        UpdateReplyToL(aSelected);
        }
    }
    
// ----------------------------------------------------------------------------
//  Updates Reply-to value for mail header.
// ----------------------------------------------------------------------------
//  
void CMsgMailEditorRemoteMailboxSettingItem::UpdateReplyToL( 
    TInt aSelected )
    {
    //Get smtp account and load smtp settings
	CEmailAccounts* smtpAccount = CEmailAccounts::NewLC();
    CImSmtpSettings* smtpSet=new(ELeave)CImSmtpSettings();
    CleanupStack::PushL(smtpSet);
    
    TInt tempArrayCount = iServiceArray->Count();
    
    
    // Checks that a aSelected is in the leagal range.
    if( !( aSelected >= 0 && aSelected < tempArrayCount ) )
    	{
    	return;
    	}
    
    const TMsvId smtpSetId = (*iServiceArray)[aSelected];

    TSmtpAccount accountParams;
    smtpAccount->GetSmtpAccountL( smtpSetId, accountParams );
	smtpAccount->LoadSmtpSettingsL(accountParams, *smtpSet);
        
    //Check that if reply to address is same as mailbox address
    //do not add "reply to" field for header. Otherwise "reply to" field
    //would always be visible.
    if(smtpSet->ReplyToAddress().Compare(smtpSet->EmailAddress() ) != 0)
        {
        CImHeader& header = iDocument.HeaderL();
        header.SetReplyToL( smtpSet->ReplyToAddress() );
        }    
    else
    	{
        CImHeader& header = iDocument.HeaderL();
        header.SetReplyToL( KNullDesC16 );
    	}
    CleanupStack::PopAndDestroy(2, smtpAccount);		// CSI: 47,12 # smtpAccount, smtpSet   
    
    }

// ----------------------------------------------------------------------------
//  Set current mailbox name to setting list
// ----------------------------------------------------------------------------
//  
void CMsgMailEditorRemoteMailboxSettingItem::SetSettingValueL()
    {
   	CMsgMailPreferences& prefs = iDocument.SendOptions();
	// set current remote mailbox name to listbox
	TMsvId service( prefs.ServiceId() );  //get current Service id
	TMsvId parent(0);
	TMsvEntry child;

	TInt err( iDocument.Session().GetEntry(service, parent, child) );
    
    iSettingValue.Close();
	if (!err)
		{
		iSettingValue.CreateL( child.iDetails );
		}
	else
		{
		HBufC* noRmBox = StringLoader::LoadLC(R_TEXT_NO_RMBOX);
		iSettingValue.CreateL( *noRmBox );
		CleanupStack::PopAndDestroy(noRmBox); //noRmBox
		}
    }

// End of File
