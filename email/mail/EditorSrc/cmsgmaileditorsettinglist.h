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
* Description:  Settings list for Mail editor.
*
*/


#ifndef __MSGMAILEDITORSETTINGSLIST_H__
#define __MSGMAILEDITORSETTINGSLIST_H__

// INCLUDES
#include <aknsettingitemlist.h>
#include <msvapi.h>

// FORWARD DECLARATIONS
class CMsgMailEditorDocument;
// CLASS DECLARATION

/**
 * Mail editor Settings list.
 */
class CMailEditorSettingList
    : public CAknSettingItemList
    {
	public: // Constructors and destructors

        /**
         * Creates a new instance of this class.
         * @param aDocument Mail editor document
         * @param aResourceId Array id
         */
		static CMailEditorSettingList* NewL( 
		    CMsgMailEditorDocument& aDocument, 
		    TInt aResourceId );

		/**
         * Destructor.
         */
        ~CMailEditorSettingList();		
		
	public: // From CAknSettingItemList
	
		CAknSettingItem* CreateSettingItemL( TInt aSettingId );
		void SizeChanged();
                                         
	private: // Constructors and destructors	
	
        CMailEditorSettingList( CMsgMailEditorDocument& aDocument );  
        void ConstructL( TInt aResourceId );   

    private:  // data
        // Ref to mail document
        CMsgMailEditorDocument& iDocument;

    };

#endif // __MSGMAILEDITORSETTINGSLIST_H__

// End of File
