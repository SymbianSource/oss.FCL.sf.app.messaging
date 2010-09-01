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


#ifndef _MSGMAILEDITOR_ENCODING_SETTINGITEM_H
#define _MSGMAILEDITOR_ENCODING_SETTINGITEM_H

//  INCLUDES
#include <aknsettingitemlist.h>
#include <aknradiobuttonsettingpage.h>
#include <msvstd.h>

class CMsgMailEditorDocument;

/**
* Setting item for encoding setting
* @since 3.2
*/
class CMsgMailEditorEncodingSettingItem
    : public CAknTextSettingItem
    {
    public: // Constructors and destructor
         
         /**
         * Creates a new instance of this class.
         * @param aIdentifier setting id
         */
		static CMsgMailEditorEncodingSettingItem* NewL( 
		    TInt aIdentifier,
		    CMsgMailEditorDocument& aDocument );
    	
    	~CMsgMailEditorEncodingSettingItem();

    public: // From CAknTextSettingItem

    	void EditItemL( TBool aCalledFromMenu );
    	
    private: // implementation
        void NameArrayL( TInt& aSelected );
        void SaveSettingValueL( TInt aSelected );
        void SetSettingValueL();
        HBufC* GetCharsetNameLC();
        	
    
    private: // C'tor	
    	    	
    	CMsgMailEditorEncodingSettingItem( 
    	    TInt aIdentifier,
    	    CMsgMailEditorDocument& aDocument );
    	void ConstructL();    
    
    private: // Data
        /// Own: setting value 
        RBuf iSettingValue;	
        /// Ref to Mail editor document
        CMsgMailEditorDocument& iDocument;
        CDesCArrayFlat* iIdArray;       
    };

/**
* Setting page for Encoding.
* @since 3.2
*/
class CMsgMailEditorEncodingSettingPage
    : public CAknRadioButtonSettingPage
    {
    public: // Constructors and destructor
        
        /// See CAknRadioButtonSettingPage for params
    	CMsgMailEditorEncodingSettingPage( 
    	    TInt aResourceID,
    	    TInt& aCurrentSelectionIndex, 
		    const MDesCArray* aItemArray );

    	CMsgMailEditorEncodingSettingPage( 
    	    const TDesC* aSettingTitleText,
            TInt aSettingNumber, 
    		TInt aControlType,
    		TInt aEditorResourceId, 
    		TInt aResourceID,
    	    TInt& aCurrentSelectionIndex, 
		    const MDesCArray* aItemArray );

        void ConstructL();
        
        ~CMsgMailEditorEncodingSettingPage();

    };

#endif // _MSGMAILEDITOR_ENCODING_SETTINGITEM_H

// End of File
