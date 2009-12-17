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
* Description:  Priority setting item and setting page
*
*/


#include "cmsgmailprioritysetting.h"
#include "MsgMailEditorDocument.h"
#include "MailLog.h"
#include <MsgMailEditor.rsg>
#include <StringLoader.h>


inline void CMsgMailEditorPrioritySettingItem::ConstructL()
    {
    SetSettingValueL();
    }

CMsgMailEditorPrioritySettingItem* 
    CMsgMailEditorPrioritySettingItem::NewL( 
    TInt aIdentifier, CMsgMailEditorDocument& aDocument )
    {
    CMsgMailEditorPrioritySettingItem* self =
        new( ELeave ) CMsgMailEditorPrioritySettingItem( 
        aIdentifier,  aDocument );
    CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(self); // self
    return self;
    }


CMsgMailEditorPrioritySettingItem::CMsgMailEditorPrioritySettingItem(
    TInt aIdentifier,
    CMsgMailEditorDocument& aDocument ): 
    CAknTextSettingItem( aIdentifier, iSettingValue ),  
    iDocument( aDocument )
    {
    // iSettingValue is set later
    }        

CMsgMailEditorPrioritySettingItem::~CMsgMailEditorPrioritySettingItem()
    {
    iSettingValue.Close();
    delete iPriorityArray;
    }

// ----------------------------------------------------------------------------
//  // From CAknTextSettingItem
// ----------------------------------------------------------------------------
//        
void CMsgMailEditorPrioritySettingItem::EditItemL( 
    TBool /* aCalledFromMenu */ )
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
        iPriorityArray );
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
void CMsgMailEditorPrioritySettingItem::NameArrayL(
    TInt& aSelected )
    {
    LOG("CMsgMailEditorPrioritySettingItem::NameArrayL");
      
    TInt itemCount(3);			// CSI: 47 # 3 items
    TMsvPriority priority = iDocument.CurrentEntry().Entry().Priority();
    aSelected = priority;
    delete iPriorityArray;
    iPriorityArray = NULL;
    iPriorityArray = new(ELeave)CDesCArrayFlat( itemCount );
                 
    HBufC* highPriority = GetPriorityTextLC( EMsvHighPriority );
    iPriorityArray->AppendL( *highPriority );
    CleanupStack::PopAndDestroy( highPriority );
    
    HBufC* normalPriority = GetPriorityTextLC( EMsvMediumPriority );
    iPriorityArray->AppendL( *normalPriority );
    CleanupStack::PopAndDestroy( normalPriority );    
    
    HBufC* lowPriority = GetPriorityTextLC( EMsvLowPriority );
    iPriorityArray->AppendL( *lowPriority );
    CleanupStack::PopAndDestroy( lowPriority );    

    ASSERT( iPriorityArray ); 
    }

// ----------------------------------------------------------------------------
//  Save selected mbox id to settings
// ----------------------------------------------------------------------------
//  
void CMsgMailEditorPrioritySettingItem::SaveSettingValueL( 
    TInt aSelected )
    {
    CMsvEntry& message = iDocument.CurrentEntry();
    TMsvEntry tentry = message.Entry();
    ASSERT( tentry.iType == KUidMsvMessageEntry );    
    
    TMsvPriority newValue( EMsvMediumPriority );  
    switch ( aSelected )
        {    
        case 0: // high
            {
            newValue = EMsvHighPriority;
            break;
            }
        case 1: // normal
            {
            newValue = EMsvMediumPriority;
            break;
            }
        case 2:				// CSI: 47 # low
            {
            newValue = EMsvLowPriority;
            break;
            }
        default:
            {
            // unknown state
            ASSERT(0);
            }    
        }
    
    tentry.SetPriority( newValue );
    message.ChangeL( tentry );
    iDocument.SetChanged( ETrue );
    }

// ----------------------------------------------------------------------------
//  Set current mailbox name to setting list
// ----------------------------------------------------------------------------
//  
void CMsgMailEditorPrioritySettingItem::SetSettingValueL()
    {
    ASSERT( iDocument.CurrentEntry().Entry().iType == KUidMsvMessageEntry );
    TMsvPriority priority = iDocument.CurrentEntry().Entry().Priority();
    
    HBufC* priorityText = GetPriorityTextLC( priority );
    iSettingValue.Close();
	iSettingValue.CreateL( *priorityText );
	CleanupStack::PopAndDestroy( priorityText );  
    }

// ----------------------------------------------------------------------------
//  get localised text for certain priority
// ----------------------------------------------------------------------------
//
HBufC* CMsgMailEditorPrioritySettingItem::GetPriorityTextLC( 
    TMsvPriority aPriority )
    {
    TInt textResource(0);
    switch ( aPriority )
        {    
        case EMsvHighPriority:
            {
            textResource = R_QTN_MAIL_SETTING_PRIORITY_HIGH;
            break;
            }
        case EMsvMediumPriority:
            {
            textResource = R_QTN_MAIL_SETTING_PRIORITY_NORMAL;
            break;
            }
        case EMsvLowPriority:
            {
            textResource = R_QTN_MAIL_SETTING_PRIORITY_LOW;
            break;
            }
        default:
            {
            // unknown state
            ASSERT(0);
            }
        }
	return StringLoader::LoadLC( textResource );    
    }

// End of File
