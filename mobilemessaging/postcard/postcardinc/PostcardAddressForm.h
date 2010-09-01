/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*     Postcard application address dialog
*
*/



#ifndef __POSTCARDADDRESSFORM_H__
#define __POSTCARDADDRESSFORM_H__

//  INCLUDES
#include <AknForm.h>    // CAknForm
#include <ConeResLoader.h>
#include <coecntrl.h>

#include <eikrted.h>
#include <eikedwin.h>
#include <eikedwob.h>

#include "PostcardContact.h"
#include "Postcard.hrh"

// FORWARD DECLARATIONS
class CAknNavigationDecorator;
class CContactCard;
class CContactItem;
class CEikMenuPane;
class CPostcardAppUi;
class CPostcardDocument;

/**
 * Postcard Address Form.
 */
class CPostcardAddressForm :    public CAknForm, 
                                public MEikEdwinObserver
    {
    public:  // Constants
        enum
            {
             // Name, info, street, zip, city, state, country
            ENumAddrFields = 7
            };
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * param IN aAppUi reference to AppUi object
        * param IN aDocument reference to document object
        * param IN aContactItem the item to be edited
        * param OUT aRetValue if for example Send was pressed in the dialog it's returned here
        * param IN aFirstActive the field that should be activated first
        * param IN aKeyEvent possible keyevent to simulate when dialog opened
        * @return pointer to created CPostcardAddressForm
        */
        static CPostcardAddressForm* NewL( 	CPostcardAppUi& aAppUi, 
        									CPostcardDocument& aDocument,
        									CContactCard& aContactItem,
        									TInt& aRetValue,
        									TInt aFirstActive = -1,
        									TKeyEvent aKeyEvent = TKeyEvent() );

        /**
         * Destructor.
         */
        ~CPostcardAddressForm();

    public:
    
        /**  
        * @return CAknDialog::RunLD()
        */
        TInt ExecLD( );

    public: // Functions from base classes

        /**
        * From MEikCommandObserver Prosesses menucommands
        * @param aCommandId - id of the command
        */
        void ProcessCommandL( TInt aCommandId );

        /**
        * From CCoeControl Handles key events
        * @param aKeyEvent  - keyevent to be handled
        * @param aType      - keyeventtype to be handled
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
               TEventCode aType );

        /**
        * HandleResourceChange
        */ 
        void HandleResourceChange(TInt aType);

        /**
        * from CEikDialog
        */
        void PreLayoutDynInitL();

        /**
        * from CEikDialog
        * Fills the fields.
        */
        void PostLayoutDynInitL();

        /**
         * from MCoeControlObserver
         * Updates the length indicator and checks if the field is too long
         */
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

    protected:  // Functions from base classes

        /**
        * From MEikMenuObserver Initializes items on the menu
        *
        * @param aMenuId    Current menu's resource Id
        * @param aMenuPane  Pointer to menupane
        */
        void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);

        /**
        * From CEikDialog Handles button events and looks if the dialog 
        *   can be closed. First calls CAknDialog::OkToExitL. If that returns
        *   true, and aButtonId is either EAknSoftkeyBack, EAknSoftkeyExit,
        *   EAknSoftkeyDone, EAknCmdExit, EEikCmdExit or EPostcardCmdSend,
        *   If a message is not yet sent, then recipient is added to the message 
        *   and ETrue is returned.
        *   Otherwise returns false;
        * @param aButtonId Id of the 'pressed' button
        *
        * @return   ETrue if it's OK to exit
        *           EFalse otherwise
        */
        TBool OkToExitL( TInt aButtonId );

        /**
        * If aEventType is EEventTextUpdate -> Updates navipane and checks the length of the field
        */
		void HandleEdwinEventL( CEikEdwin* aEdwin, TEdwinEvent aEventType );

        /**
        * Changes the edwin event observer and updates navipane
        */
		void LineChangedL(TInt aControlId);

    protected:  // New functions

        /**
        * DynInitMainMenuL Initializes items on the menu
        *
        * @param aMenuPane  Pointer to menupane
        */
        void DynInitMainMenuL(CEikMenuPane* aMenuPane);

        /**
        * DynInitInsertSubMenuL Initializes items on the menu
        *
        * @param aMenuPane  Pointer to menupane
        */
        void DynInitInsertSubMenuL(CEikMenuPane* aMenuPane);

        /**
        * DynInitContextMenuL Initializes items on the menu
        *
        * @param aMenuPane  Pointer to menupane
        */
        void DynInitContextMenuL(CEikMenuPane* aMenuPane);

        /**
        * IsMainMenuOptionInvisible
        * Returns true if menuoption aMenuOptionId is visible in Mainmenu
        * @param aMenuOptionId  menu option to be checked
        * @return TBool         True if option is invisible
        */
        TBool IsMainMenuOptionInvisible( TInt aMenuOptionId );

        /**
        * LaunchHelpL
        *   This function is called when Help application is launched.
        */
        void LaunchHelpL( );

    private:  // From CAknForm

        /**
        * Calls CAknForm::SetInitialCurrentLine() if it's not been called yet
        */
        void SetInitialCurrentLine();

        /**
        * Does nothing at the moment
        */
        void HandleControlStateChangeL(TInt aControlId);

	private:	// Own ones
	
        /**
        * Opens the add recipient dialog
        */
		void DoSelectionKeyL( );

        /**
        * Changes the value of iRetVal and tries to exit the dialog
        */
		void SendL( );

        /**
         * Changes the value of iRetVal and tries to exit the dialog
         */
        void ForwardMessageL( );

        /**
        * Updates current TMsvEntry and calls AppUi's DoShowMessageInfoL
        */
		void MessageInfoL( );

        /**
        * Checks the length of current text field and shows an info note if necessary
        */
		void CheckTextExceedingL( );
		
        /**
        * Uses CPostcardContact to show the recipient fetch dialog
        * Uses CPostcardRecipientWrapper to fetch the right detail of the contact item
        * Adds the value of the contact item into the address fields
        */
		void DoAddRecipientL( );
		
		/**
		* Updates recipient to entry.
		*/
		void UpdateRecipientToEntryL();
		
        /**
        * Adds the right fields of aContact to right fields of the dialog
        */
		void UpdateFieldsL( CContactItem& aContact );
		
        /**
        * Add the aLocation fields of aContact to right fields of the dialog
        */
        void UpdateFieldsL( CPostcardContact::TLocation aLocation );

        /**
        * Returns true if none of the fields have any text
        */
		TBool IsEmpty( );

        /**
        * Cleans navipane.
        */
        void CleanNavipane();

        /**
        * Forces all visible lines to be redrawn on the screen
        */
        void RefreshDisplay( );

        /**
        * Set middle softkey according to the mode (viewer / edit)
        * and whether address is empty or not
        */        
        void SetMiddleSoftkeyL();

    protected:  // Constructors
	    CPostcardAddressForm( 	CPostcardAppUi& aAppUi, 
	    						CPostcardDocument& aDocument,
	    						CContactCard& aContactItem,
	    						TInt& aRetValue,
	    						TInt aFirstActive,
	    						TKeyEvent aKeyEvent );

        /**
        * EPOC constructor
        */
        void ConstructL();

        /**
        * Goes thru the fields. Hides the hidden ones.
        * Shows error note of an empty field if necessary.
        * Changes the focused field to the right one.
        * Updates navipane. Simulate keys if necessary.
        */
        void ActivateL();

    protected: // data
        /// Ref: Contact item under editing
        CContactCard& iContactItem;

    private:  // Data

		enum PostcardAddressFlags 
			{
			EPostcardAddressHelpSupported = 1,
			EPostcardAddressTextExceeded = 2
			};

		CPostcardAppUi& iAppUi;
		CPostcardDocument& iDocument;

        CAknNavigationDecorator*    iNaviDecorator;

		TKeyEvent		iKeyEvent;
		TInt&			iRetValue;
        TInt            iAddrFlags;
        TInt			iMaxLen;
        TInt			iPreviousControl;
        TInt			iFirstActive;

        CPostcardContact* iContact; // interface to contact database

        TInt iMskResource; // current middle softkey resource ID
     };

#endif // __CPbkContactEditorDlg_H__

// End of File
