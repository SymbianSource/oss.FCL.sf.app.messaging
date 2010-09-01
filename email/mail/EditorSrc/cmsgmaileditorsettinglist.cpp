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
* Description:  Provides methods for Settings list.
*
*/


// INCLUDE FILES
#include "MsgMailEditorDocument.h"
#include "cmsgmaileditorsettinglist.h"
#include "cmsgmailremotemailboxsetting.h"
#include "cmsgmailshedulingsetting.h"
#include "cmsgmailencodingsetting.h"
#include "cmsgmailprioritysetting.h"
#include "cmsgmailreplytosetting.h"
#include "msgmaileditor.hrh"
#include <MsgMailEditor.rsg>
#include "CMailCRHandler.h"


// ================= MEMBER FUNCTIONS =======================

CMailEditorSettingList::CMailEditorSettingList(
    CMsgMailEditorDocument& aDocument )
    : iDocument( aDocument )
    {
    }

inline void CMailEditorSettingList::ConstructL( TInt aResourceId )
    {
	ConstructFromResourceL( aResourceId );
    }

CMailEditorSettingList* CMailEditorSettingList::NewL(
    CMsgMailEditorDocument& aDocument, TInt aResourceId )
    {
    CMailEditorSettingList* self =
        new( ELeave ) CMailEditorSettingList( aDocument );
    CleanupStack::PushL( self );
	self->ConstructL( aResourceId );
	CleanupStack::Pop(self); // self
    return self;
    }

CMailEditorSettingList::~CMailEditorSettingList()
    {	
    }

// ----------------------------------------------------------------------------
//  From: CAknSettingItemList
// ----------------------------------------------------------------------------
//
CAknSettingItem* CMailEditorSettingList::CreateSettingItemL(
    TInt aSettingId )
	{
	CAknSettingItem* settingItem( NULL );
	switch ( aSettingId )
        {
        case ESettingMailboxInUse:
            {
            settingItem = CMsgMailEditorRemoteMailboxSettingItem::NewL( 
                aSettingId, iDocument );
             
            break;
            }
         
        case ESettingSendMessage:
            {
                // nothing leavable is called after creating this item
                settingItem = CMsgMailEditorShedulingSettingItem::NewL( // CSI: 35 # see comment above
                    aSettingId, iDocument );
            break;
            }
        case ESettingPriority:
            {
            settingItem = CMsgMailEditorPrioritySettingItem::NewL( 
                aSettingId, iDocument );                
            break;            
            }
        case ESettingReplyTo:
            {
            settingItem = CMsgMailEditorReplyToSettingItem::NewL( 
                aSettingId, iDocument );
             
            break;            
            }
        case ESettingEncoding:
            {
            // is encoding setting enabled?
            if ( iDocument.MailCRHandler()->MailEncodingSetting() )
                {
                settingItem = CMsgMailEditorEncodingSettingItem::NewL( 
                    aSettingId, iDocument );
                }
            
            break;
            }
		default:
			{
			// Do nothing
			break;
			}
        }

    // Ownership transferred
	return settingItem;
	}

// ----------------------------------------------------------------------------
//  CMailEditorSettingList::SizeChanged
// ----------------------------------------------------------------------------
//
void CMailEditorSettingList::SizeChanged()
    {
    CAknSettingItemList::SizeChanged();
    if ( ListBox() ) 
        {
        ListBox()->SetRect( Rect() );
        }
    }

//  End of File
