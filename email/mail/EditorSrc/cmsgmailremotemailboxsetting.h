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


#ifndef _MSGMAILEDITORREMOTEMAILBOXSETTINGITEM_H
#define _MSGMAILEDITORREMOTEMAILBOXSETTINGITEM_H

//  INCLUDES
#include <aknsettingitemlist.h>
#include <aknradiobuttonsettingpage.h>
#include <msvstd.h>
#include <ImumInternalApi.h>

class CMsgMailEditorDocument;

/**
* Setting item for remote mail box in use
* @since 3.2
*/
class CMsgMailEditorRemoteMailboxSettingItem
    : public CAknTextSettingItem
    {
    public: // Constructors and destructor
         
         /**
         * Creates a new instance of this class.
         * @param aIdentifier setting id
         */
		static CMsgMailEditorRemoteMailboxSettingItem* NewL( 
		    TInt aIdentifier,
		    CMsgMailEditorDocument& aDocument );
    	
    	~CMsgMailEditorRemoteMailboxSettingItem();

    public: // From CAknTextSettingItem
        void EditItemL( TBool aCalledFromMenu );
    	
    private: // implementation
        void NameArrayL( TInt& aSelected );
        void SaveSettingValueL( TInt aSelected );
        void SetSettingValueL();
        void UpdateReplyToL(TInt aSelected );
    
    private: // C'tor	
    	    	
    	CMsgMailEditorRemoteMailboxSettingItem( 
    	    TInt aIdentifier,
    	    CMsgMailEditorDocument& aDocument );
    	void ConstructL();    
    
    private: // Data
        /// Own: setting value 
        RBuf iSettingValue;	
        /// Ref to Mail editor document
        CMsgMailEditorDocument& iDocument;
        CDesCArray* iMailBoxIdArray;
        CMsvEntrySelection* iServiceArray;        
    };

#endif // _MSGMAILEDITORREMOTEMAILBOXSETTINGITEM_H

// End of File
