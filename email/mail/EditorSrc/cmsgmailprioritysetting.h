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


#ifndef _MSGMAILEDITOR_PRIORITY_SETTINGITEM_H
#define _MSGMAILEDITOR_PRIORITY_SETTINGITEM_H

//  INCLUDES
#include <aknsettingitemlist.h>
#include <aknradiobuttonsettingpage.h>
#include <msvstd.h>

class CMsgMailEditorDocument;

/**
* Setting item for priority flag
* @since 3.2
*/
class CMsgMailEditorPrioritySettingItem
    : public CAknTextSettingItem
    {
    public: // Constructors and destructor
         
         /**
         * Creates a new instance of this class.
         * @param aIdentifier setting id
         */
		static CMsgMailEditorPrioritySettingItem* NewL( 
		    TInt aIdentifier,
		    CMsgMailEditorDocument& aDocument );
    	
    	~CMsgMailEditorPrioritySettingItem();

    public: // From CAknTextSettingItem

    	void EditItemL( TBool aCalledFromMenu );
    	
    private: // implementation
        void NameArrayL( TInt& aSelected );
        void SaveSettingValueL( TInt aSelected );
        void SetSettingValueL();
        HBufC* GetPriorityTextLC( TMsvPriority aPriority );
        	
    
    private: // C'tor	
    	    	
    	CMsgMailEditorPrioritySettingItem( 
    	    TInt aIdentifier,
    	    CMsgMailEditorDocument& aDocument );
    	void ConstructL();    
    
    private: // Data
        /// Own: setting value 
        RBuf iSettingValue;	
        /// Ref to Mail editor document
        CMsgMailEditorDocument& iDocument;
        CDesCArray* iPriorityArray;    
    };

#endif // _MSGMAILEDITOR_PRIORITY_SETTINGITEM_H

// End of File
