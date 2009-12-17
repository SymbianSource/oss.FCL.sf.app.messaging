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
* Description:  Handles CItemFinder and CFindItemMenu for mail viewer.
*
*/

#ifndef CMSGMAILVIEWERFINDITEMHANDLER_H
#define CMSGMAILVIEWERFINDITEMHANDLER_H

// INCLUDES
#include <e32base.h>
#include <MsgRecipientItem.h>
#include <ItemFinder.h>
#include <MsgEditor.hrh>

// FORWARD DECLARATIONS
class CFindItemMenu;
class CEikMenuPane;
class CItemFinder;

// CLASS DECLARATION

/**
* Class for handling CItemFinder and CFindItemMenu for mail viewer.
*   
*/
NONSHARABLE_CLASS(CMsgMailViewerFindItemHandler) : public CBase
	{
    public: // Constructors and destructor
    
        /**
        * NewL
        * Creates find item handler
        * @param aItemFinder, ownership is not transferred
        * @return self
        */    
        static CMsgMailViewerFindItemHandler* NewL(
             CItemFinder* aItemFinder );

        /**
        * Destructor.
        */
        ~CMsgMailViewerFindItemHandler();
        
    public: // New functions

        /**
        * To be called from owner's DynInitMenuPaneL.
        * @param aResourceId resource id from caller
        * @param aMenuPane menu pane from caller
        * @param aAddress address, may be empty
        * @param aName alias name of aAddress, may be empty
        * @param aFocusedControl focused control, EMsgComponentIdNull if none
        */
        void DynInitMenuPaneL( TInt aResourceId,
                               CEikMenuPane* aMenuPane,
                               const TDesC& aAddress,
                               const TDesC& aName,
                               TMsgControlId aFocusedControl );
        
        /**
        * Method to test if given command is applicable for this class
        * and at this very moment.
        * @param aCommand command id
        * @return ETrue if command can be handled
        */                       
        TBool CanHandleCommand( TInt aCommand ) const;

        /**
        * Command handler. Use CanHandleCommand first to test if
        * command is applicable.
        * @param aCommand command id
        * @return ETrue if command was consumed by this class
        */
        void HandleFinderCommandL( TInt aCommand );
                              
        /**
        * Returns type of item currently selected by CItemFinder.
        * If item finder doesn't exist or editor isn't focused
        * then ENoneSelected is returned.
        * @param aIsFocus does editor have focus        
        * @return currently selected item type
        */
        CItemFinder::TItemType SelectionType( TBool aIsFocused ) const;
        
        /**
        * Returns text for item currently selected by CItemFinder.
        * If item finder doesn't exist or editor isn't focused
        * then KNullDesC is returned.
        * @param aIsFocus does editor have focus
        * @return currently selected item's text
        */
        const TDesC& SelectionText( TBool aIsFocused ) const;
        
        /**
        * Sets CItemFinder flags. If item finder doesn't exist
        * then flags are just ignored.
        * @param aFindFlags, see CItemFinder::SetFindModelL
        */
        void SetFindModeL( TInt aFindFlags );
        
        /**
        * Gets CItemFinder mode. If not set at all, then 
        * ENoneSelected is returned.
        * @return find mode
        */
        TInt FindMode() const;

    private: // New functions

        void DynInitOptionsMenuL( CEikMenuPane& aMenuPane );
        
        TBool IsAddressControl( TMsgControlId aFocusedControl ) const;
        
    private: // Constructors
                
        /**
        * Default constructor.
        */
        CMsgMailViewerFindItemHandler( CItemFinder* aItemFinder );

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();
                
    private: //Data
    
        // status of finder, maintained by ourselves since CItemFinder
        // has only a setter but we need a getter too
        TInt iFindMode;

        // Not own
        CItemFinder* iItemFinder;

        // Find item, own
        CFindItemMenu* iFindItemMenu;
    };

#endif // CMSGMAILVIEWERFINDITEMHANDLER_H

// End of File

