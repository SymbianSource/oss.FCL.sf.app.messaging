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
* Description:  Encoding setting item and setting page
*
*/


#include "cmsgmailencodingsetting.h"
#include "MsgMailEditorDocument.h"
#include "MailLog.h"
#include <MsgMailEditor.rsg>
#include <StringLoader.h>
#include <charconv.h>


inline void CMsgMailEditorEncodingSettingItem::ConstructL()
    {
    SetSettingValueL();
    }

CMsgMailEditorEncodingSettingItem* 
    CMsgMailEditorEncodingSettingItem::NewL( 
    TInt aIdentifier, CMsgMailEditorDocument& aDocument )
    {
    CMsgMailEditorEncodingSettingItem* self =
        new( ELeave ) CMsgMailEditorEncodingSettingItem( 
        aIdentifier,  aDocument );
    CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(self); // self
    return self;
    }

CMsgMailEditorEncodingSettingItem::CMsgMailEditorEncodingSettingItem(
    TInt aIdentifier,
    CMsgMailEditorDocument& aDocument ): 
    CAknTextSettingItem( aIdentifier, iSettingValue ),  
    iDocument( aDocument )
    {
    // iSettingValue is set later
    }        

CMsgMailEditorEncodingSettingItem::~CMsgMailEditorEncodingSettingItem()
    {
    iSettingValue.Close();
    delete iIdArray;
    }

// ----------------------------------------------------------------------------
//  // From CAknTextSettingItem
// ----------------------------------------------------------------------------
//        
void CMsgMailEditorEncodingSettingItem::EditItemL( 
    TBool /* aCalledFromMenu */ )
    {
    TPtrC settingName = SettingName();
    TInt currentSelection( 0 );
    NameArrayL( currentSelection );
  
    CAknSettingPage* dlg = new( ELeave )CMsgMailEditorEncodingSettingPage(        
        &settingName, 
        SettingNumber(), 
        EEikCtTextButton,
        SettingEditorResourceId(), 
        SettingPageResourceId(), 
        currentSelection, 
        iIdArray );
    SetSettingPage( dlg );
    SettingPage()->SetSettingPageObserver( this ); // not used
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
void CMsgMailEditorEncodingSettingItem::NameArrayL( TInt& aSelected )
    {
    LOG("CMsgMailEditorEncodingSettingItem::NameArrayL");
    RFs& fs = CCoeEnv::Static()->FsSession();
    CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* charSets =
        CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC( fs );
    CCnvCharacterSetConverter* charConv = CCnvCharacterSetConverter::NewLC();
    		
    TInt charsetCount = charSets->Count();
    // +1 for "autom" string
    delete iIdArray;
    iIdArray = NULL;
    iIdArray = new( ELeave )CDesCArrayFlat( charsetCount+1 );
   
    CMsgMailPreferences& prefs = iDocument.SendOptions();
    TUid charId = prefs.SendingCharacterSet();

	HBufC* autom = StringLoader::LoadLC( R_QTN_MBXS_SETT_DEFENC_AUTOMATIC );
	iIdArray->AppendL( *autom );
	CleanupStack::PopAndDestroy(autom); // autom
	
    for ( TInt i(0); i<charsetCount; i++)
        {
        TUint cId = (*charSets)[i].Identifier();
    	// set focus
    	if ( cId == charId.iUid )
    		{
    		// -1 because of autom is located at 0 index
    		aSelected = i-1;
    		}
        
        HBufC8* charaSetName = 
        	charConv->ConvertCharacterSetIdentifierToStandardNameL(
                cId, fs );
        
        if ( charaSetName )
            {
            CleanupStack::PushL( charaSetName );
            HBufC* nameBuf = HBufC::NewLC( charaSetName->Length() );
            nameBuf->Des().Copy( *charaSetName );
			iIdArray->AppendL( *nameBuf );
            CleanupStack::PopAndDestroy(2, charaSetName);	// CSI: 47,12 # nameBuf, charaSetName
            }
        
        }
    CleanupStack::PopAndDestroy( 2, charSets );		// CSI: 47,12 # charSets, charConv
    }

// ----------------------------------------------------------------------------
//  Save selected mbox id to settings
// ----------------------------------------------------------------------------
//  
void CMsgMailEditorEncodingSettingItem::SaveSettingValueL( 
    TInt aSelected )
    {
    ASSERT( iIdArray );
    ASSERT( iIdArray->Count() >= 0 && aSelected < iIdArray->Count() );
    
    TPtrC name = (*iIdArray)[aSelected];
	HBufC8* charsetName = HBufC8::NewLC( name.Length() );
	charsetName->Des().Copy( name );
    
    CCnvCharacterSetConverter* charConv = CCnvCharacterSetConverter::NewLC();
    
    RFs& fs = CCoeEnv::Static()->FsSession();
	TUint charsetId = 
		charConv->ConvertStandardNameOfCharacterSetToIdentifierL(
		*charsetName, fs);

    CleanupStack::PopAndDestroy(2, charsetName );	// CSI: 47,12 # charConv, charsetName  
		
	// Update selected ID to settings
	TUid charId;
	charId.iUid = charsetId;
	iDocument.SendOptions().SetSendingCharacterSet( charId );    

    iDocument.SetChanged( ETrue );
    }

// ----------------------------------------------------------------------------
//  Set current character set name to setting list
// ----------------------------------------------------------------------------
//  
void CMsgMailEditorEncodingSettingItem::SetSettingValueL()
    {
    HBufC* charset = GetCharsetNameLC();
    iSettingValue.Close();		
	iSettingValue.CreateL( *charset );
	CleanupStack::PopAndDestroy( charset );
    }
    
// ----------------------------------------------------------------------------
//  Get current character set name
// ----------------------------------------------------------------------------
//
HBufC* CMsgMailEditorEncodingSettingItem::GetCharsetNameLC()
    {
	CMsgMailPreferences& prefs = iDocument.SendOptions();
	TUint charsetId = prefs.SendingCharacterSet().iUid;
	
	HBufC* returnName(NULL);
	if (charsetId)
		{
    	RFs& fs = CCoeEnv::Static()->FsSession();
    	CCnvCharacterSetConverter* charConv =
    		CCnvCharacterSetConverter::NewLC();
    		
		HBufC8* charSetName = charConv->
			ConvertCharacterSetIdentifierToStandardNameL(
			charsetId, fs);
		CleanupStack::PopAndDestroy(charConv); // charConv	
		
		if (charSetName)
			{
			CleanupStack::PushL( charSetName );
			// 1 because length might be zero 
			returnName = HBufC::NewL( Max(charSetName->Length(), 1) );
			returnName->Des().Copy( *charSetName );
			CleanupStack::PopAndDestroy(charSetName); // charSetName
			CleanupStack::PushL( returnName );
			}
		else
		    {
		    ASSERT(0); // panic
		    }	
		}
    else
        {
        returnName = StringLoader::LoadLC( R_QTN_MBXS_SETT_DEFENC_AUTOMATIC );
        }
    return returnName;
    }


CMsgMailEditorEncodingSettingPage::CMsgMailEditorEncodingSettingPage(
    TInt aResourceID,
    TInt& aCurrentSelectionIndex, 
	const MDesCArray* aItemArray )
    : CAknRadioButtonSettingPage( 
        aResourceID, aCurrentSelectionIndex, aItemArray )
	{
	}

CMsgMailEditorEncodingSettingPage::CMsgMailEditorEncodingSettingPage(	
    const TDesC* aSettingText, 
    TInt aSettingNumber, 
    TInt aControlType,
    TInt aEditorResourceId, 
    TInt aResourceID,
    TInt& aCurrentSelectionIndex, 
	const MDesCArray* aItemArray)
    : CAknRadioButtonSettingPage( 
      aSettingText, 
      aSettingNumber,
      aControlType, 
      aEditorResourceId, 
      aResourceID,
      aCurrentSelectionIndex,
      aItemArray )
    {
    }

CMsgMailEditorEncodingSettingPage::~CMsgMailEditorEncodingSettingPage()
	{
	}

void CMsgMailEditorEncodingSettingPage::ConstructL()
	{
	CAknRadioButtonSettingPage::ConstructL();
	}

// End of File
