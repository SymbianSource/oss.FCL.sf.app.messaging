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
* Description:  Send sheduling setting item and setting page
*
*/


#include "cmsgmailshedulingsetting.h"
#include "MsgMailEditorDocument.h"
#include "MailLog.h"
#include <MsgMailEditor.rsg>
#include <StringLoader.h>


inline void CMsgMailEditorShedulingSettingItem::ConstructL()
    {
    SetSettingValueL();
    }

CMsgMailEditorShedulingSettingItem* 
    CMsgMailEditorShedulingSettingItem::NewL( 
    TInt aIdentifier, CMsgMailEditorDocument& aDocument )
    {
    CMsgMailEditorShedulingSettingItem* self =
        new( ELeave ) CMsgMailEditorShedulingSettingItem( 
        aIdentifier,  aDocument );
    CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(self); // self
    return self;
    }


CMsgMailEditorShedulingSettingItem::CMsgMailEditorShedulingSettingItem(
    TInt aIdentifier,
    CMsgMailEditorDocument& aDocument ): 
    CAknTextSettingItem( aIdentifier, iSettingValue ),  
    iDocument( aDocument )
    {
    // iSettingValue is set later
    }        

CMsgMailEditorShedulingSettingItem::~CMsgMailEditorShedulingSettingItem()
    {
    iSettingValue.Close();
    delete iShedulingItems;
    }

// ----------------------------------------------------------------------------
//  // From CAknTextSettingItem
// ----------------------------------------------------------------------------
//        
void CMsgMailEditorShedulingSettingItem::EditItemL( 
    TBool aCalledFromMenu )
    {
    if ( aCalledFromMenu )
        {
        TPtrC settingName = SettingName();
        TInt currentSelection( 0 );
        NameArrayL( currentSelection );
        
        CAknSettingPage* dlg = new( ELeave )CAknRadioButtonSettingPage(        
            &settingName, 
            SettingNumber(), 
            EEikCtTextButton,
            SettingEditorResourceId(), 
            SettingPageResourceId(), 
            currentSelection, 
            iShedulingItems );
        SetSettingPage( dlg );
        SettingPage()->SetSettingPageObserver( this ); // not used
        SettingPage()->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted );
       
        SaveSettingValueL( currentSelection );
        SetSettingValueL();
        LoadL(); // update setting value to listbox
        UpdateListBoxTextL();    
        SetSettingPage( 0 ); // it is deleted now        
        }
    else
        {
        // change value
        DoChangeValueL();
        SetSettingValueL();
        LoadL(); // update setting value to listbox
        UpdateListBoxTextL();        
        }
    }

// ----------------------------------------------------------------------------
//  Create name array
// ----------------------------------------------------------------------------
//    
void CMsgMailEditorShedulingSettingItem::NameArrayL(
    TInt& aSelected )
    {
    LOG("CMsgMailEditorShedulingSettingItem::NameArrayL");
    
    delete iShedulingItems;
    iShedulingItems = NULL;
    iShedulingItems = new(ELeave)CDesCArrayFlat(2);			// CSI: 47 # For two texts.
    
    CCoeEnv* coeEnv = CCoeEnv::Static();
    HBufC* text = StringLoader::LoadLC(R_TEXT_SHED_NOW, coeEnv);
    iShedulingItems->AppendL(*text);
    CleanupStack::PopAndDestroy(text); // text
    text = StringLoader::LoadLC(R_TEXT_SHED_NEXT, coeEnv);
    iShedulingItems->AppendL(*text);
    CleanupStack::PopAndDestroy(text); // text

    CMsgMailPreferences& prefs = iDocument.SendOptions();
    
    aSelected = ( prefs.MessageScheduling() == 
        CMsgMailPreferences::EMsgMailSchedulingNow ) ? 0 : 1;        
    }

// ----------------------------------------------------------------------------
//  Save to settings
// ----------------------------------------------------------------------------
//  
void CMsgMailEditorShedulingSettingItem::SaveSettingValueL( TInt aSelected )
    {
    ASSERT( aSelected >= 0 && aSelected <= 1 );
    
    CMsgMailPreferences& prefs = iDocument.SendOptions();
    if ( aSelected == 0 )
        {
        prefs.SetMessageScheduling(
            CMsgMailPreferences::EMsgMailSchedulingNow);
        }
    else
        {
        prefs.SetMessageScheduling(
            CMsgMailPreferences::EMsgMailSchedulingNextConn);
        }
    iDocument.SetChanged( ETrue );
    }

// ----------------------------------------------------------------------------
//  Set current value to setting list
// ----------------------------------------------------------------------------
//  
void CMsgMailEditorShedulingSettingItem::SetSettingValueL()
    {
	TInt stringResource(R_TEXT_SHED_NOW);
	
	CMsgMailPreferences& prefs = iDocument.SendOptions();
    if( prefs.MessageScheduling() == 
        CMsgMailPreferences::EMsgMailSchedulingNextConn )
        {
		stringResource = R_TEXT_SHED_NEXT;
		}	
	
	HBufC* sheduling = StringLoader::LoadLC( stringResource );
	iSettingValue.Close();
    iSettingValue.CreateL( *sheduling );
    CleanupStack::PopAndDestroy( sheduling );
    }
    
//-----------------------------------------------------------------------------
//  Swap betveen values
//-----------------------------------------------------------------------------
void CMsgMailEditorShedulingSettingItem::DoChangeValueL()
    {
    CMsgMailPreferences& prefs = iDocument.SendOptions();

    if ( prefs.MessageScheduling() == 
        CMsgMailPreferences::EMsgMailSchedulingNow )
        {
        prefs.SetMessageScheduling(
            CMsgMailPreferences::EMsgMailSchedulingNextConn );
        }
    else
        {
        prefs.SetMessageScheduling(
            CMsgMailPreferences::EMsgMailSchedulingNow );
        }

    iDocument.SetChanged(ETrue);
    }    

// End of File
