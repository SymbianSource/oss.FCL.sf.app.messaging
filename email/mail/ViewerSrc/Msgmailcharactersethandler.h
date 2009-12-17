/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Character set menu handler
*
*/



#ifndef CMSGMAILCHARACTERSETHANDLER_H
#define CMSGMAILCHARACTERSETHANDLER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CEikMenuPane;

// CLASS DECLARATION

/**
*  Character set handler
* Handle dynamic options in characterset menu.
*
*  @since Series60_25
*/
class CMsgMailCharacterSetHandler : public CBase
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aFirstFreeCommandId First dynamic option menu 
        * item will use this id.
        */
        static CMsgMailCharacterSetHandler* NewL( TInt aFirstFreeCommandId );
        
        /**
        * Destructor.
        */
        ~CMsgMailCharacterSetHandler();

    public: // New functions
        
        /**
        * SetCharacterSetSubMenuItemsL
        * @since Series60_2.6
        * @param aMenuPane MenuPane where items are added
        * @param aActiveCharacterSet Currently active characterset. Character
        *           set with this ID is hide from the options menu.
        */
        void SetCharacterSetSubMenuItemsL(
            CEikMenuPane& aMenuPane, 
            TUint aActiveCharacterSet);
        /**
        * GetCharacterSetId
        * @since Series60_2.5
        * @param aCommandId Options menu command id.
        * @param aCharacterSetId Corresponding CharacterSet Id
        * @return ETrue if aCommandId was consumed
        */
        TBool GetCharacterSetId(TInt aCommandId, TUint& aCharacterSetId);

        /**
        * CharacterSetCount
        * @since Series60_2.6
        * @param aMenuPane MenuPane where item is added
        * Menu item is not added if characterset count is zero.
        */
        void SetCharacterSetMenuItemL(CEikMenuPane& aMenuPane);
       
    private:  // Implementation
        TBool IsAcceptedCharacterSet(TUint aCharacterSetId);
        void AppendItemsToItemArrayL();    
        
        // Item array data structure
        struct TData
            {
            enum { ENominalTextLength=40 }; // see CEikMenuPaneItem::SData
            TInt iCommandId;
            TUint iCharacterSetId;
            TBuf<ENominalTextLength> iText; // Options menu text           
            };

    private:

        /**
        * C++ default constructor.
        */
        CMsgMailCharacterSetHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        /// Variant flag
        TInt iVariationFlag;
        /// Item array
        RArray<TData> iItems;
        /// FirstFreeCommandId
        TInt iFirstFreeCommandId;

    };

#endif      // CMSGMAILCHARACTERSETHANDLER_H   
            
// End of File
