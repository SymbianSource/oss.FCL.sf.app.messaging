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
* Description:  Messae send sheduling setting item and setting page
*
*/


#ifndef _MSGMAILEDITOR_SHEDULING_SETTINGITEM_H
#define _MSGMAILEDITOR_SHEDULING_SETTINGITEM_H

//  INCLUDES
#include <aknsettingitemlist.h>
#include <aknradiobuttonsettingpage.h>

class CMsgMailEditorDocument;

/**
* Setting item for send sheduling
* @since 3.2
*/
class CMsgMailEditorShedulingSettingItem
    : public CAknTextSettingItem
    {
    public: // Constructors and destructor
         
         /**
         * Creates a new instance of this class.
         * @param aIdentifier setting id
         */
		static CMsgMailEditorShedulingSettingItem* NewL( 
		    TInt aIdentifier,
		    CMsgMailEditorDocument& aDocument );
    	
    	~CMsgMailEditorShedulingSettingItem();

    public: // From CAknTextSettingItem

    	void EditItemL( TBool aCalledFromMenu );
    	
    private: // implementation
        void NameArrayL( TInt& aSelected );
        void SaveSettingValueL( TInt aSelected );
        void SetSettingValueL();
        void DoChangeValueL();	
    
    private: // C'tor	
    	    	
    	CMsgMailEditorShedulingSettingItem( 
    	    TInt aIdentifier,
    	    CMsgMailEditorDocument& aDocument );
    	void ConstructL();    
    
    private: // Data
        /// Own: setting value 
        RBuf iSettingValue;	
        /// Ref to Mail editor document
        CMsgMailEditorDocument& iDocument;
        /// Own selection items
        CDesCArrayFlat* iShedulingItems;       
    };

#endif // _MSGMAILEDITOR_SHEDULING_SETTINGITEM_H

// End of File
