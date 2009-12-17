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


#ifndef _MSGMAILEDITOR_REPLYTO_SETTINGITEM_H
#define _MSGMAILEDITOR_REPLYTO_SETTINGITEM_H

//  INCLUDES
#include <aknsettingitemlist.h>
#include <akntextsettingpage.h>
#include <msvstd.h>

class CMsgMailEditorDocument;

/**
* Setting item Reply-to setting
* @since 3.2
*/
class CMsgMailEditorReplyToSettingItem
    : public CAknTextSettingItem
    {
    public: // Constructors and destructor
         
         /**
         * Creates a new instance of this class.
         * @param aIdentifier setting id
         */
		static CMsgMailEditorReplyToSettingItem* NewL( 
		    TInt aIdentifier,
		    CMsgMailEditorDocument& aDocument );
    	
    	~CMsgMailEditorReplyToSettingItem();

    public: // From CAknTextSettingItem

    	void EditItemL( TBool aCalledFromMenu );
    	void LoadL();
    	
    private: // implementation
        void SaveSettingValueL();
        void SetSettingValueL();
        void SetInitialValueL();	
    
    private: // C'tor	
    	    	
    	CMsgMailEditorReplyToSettingItem( 
    	    TInt aIdentifier,
    	    CMsgMailEditorDocument& aDocument );
    	void ConstructL();    
    
    private: // Data
        /// Own: setting value 
        RBuf iSettingValue;	
        /// Ref to Mail editor document
        CMsgMailEditorDocument& iDocument;     
    };

/**
* Setting page for Reply-to setting.
* @since 3.2
*/
class CMsgMailEditorReplyToSettingPage
    : public CAknTextSettingPage
    {
    public: // Constructors and destructor
        
        /// See CAknTextSettingPage for params
    	CMsgMailEditorReplyToSettingPage( 
    	    TInt aResourceID,
    	    TDes& aText, 
    		TInt aTextSettingPageFlags = 0 );

    	CMsgMailEditorReplyToSettingPage( 
    	    const TDesC* aSettingTitleText,
            TInt aSettingNumber, 
    		TInt aControlType,
    		TInt aEditorResourceId, 
    		TInt aResourceID,
    	    TDes& aText, 
            TInt aTextSettingPageFlags = 0 );

    	
    	~CMsgMailEditorReplyToSettingPage();
    	
    protected: // From CAknTextSettingPage
    
        TBool OkToExitL( TBool aAccept );

    };

#endif // _MSGMAILEDITOR_REPLYTO_SETTINGITEM_H

// End of File
