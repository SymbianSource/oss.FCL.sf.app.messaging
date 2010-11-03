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
* Description: PostcardTextDialog  declaration
*
*/



#ifndef POSTCARDTEXTDIALOG_H 
#define POSTCARDTEXTDIALOG_H

// INCLUDES
#include <AknForm.h>        // for CAknDialog
#include <ConeResLoader.h>
#include <aknnavide.h>
#include <eikrted.h>
#include <txtrich.h>
#include <eikedwob.h>
#include <coecobs.h>

#include "Postcard.hrh"

// FORWARD DECLARATIONS
class CPostcardTextDialogCustomDraw;
class CPostcardAppUi;
class CPostcardDocument;
struct TKeyEvent;

// CLASS DECLARATION
/**
* CPostcardTextDialog dialog class
*   The dialog will be automatically destroyed by ExecuteLD.
*/
class CPostcardTextDialog :     public CAknDialog, 
                                public MEikEdwinObserver
    {

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @return pointer to created CPostcardTextDialog
        */
        static CPostcardTextDialog* NewL( 	CPostcardAppUi& aAppUi, 
        									CPostcardDocument& aDocument,
        									CPlainText& aText, 
        									TInt& aRetValue,
        									TKeyEvent aKeyEvent = TKeyEvent() );

        /**
        * Destructor.
        * Removes the dialog from EikonEnv Stack. Closes
        * the resource loader. Changes the title back to
        * the original. Deletes presentation controller
        * and wrapper.
        */
        ~CPostcardTextDialog();
        
    public:

        /**  
        * @return CAknDialog::RunLD()
        */
        TInt ExecLD( );

        /**
        * UpdateNavipaneL
        */
        void UpdateNavipaneL( TBool aForce );

        /**
        * SetCharParaFormat Set character and paragraph formats
        */
        void SetCharParaFormat();

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
        * If aType is dynamic layout change -> rereads the size of the text field from LAF
        */ 
        void HandleResourceChange(TInt aType);

        /**
        * from CEikDialog
        * Constructs Text Editor.
        * Reads the font from LAF.
        * Creates scrollbar and updates navipane.
        */
        void PreLayoutDynInitL();

        /**
        * from CEikDialog
        * Sets the position of the cursor in the end of the text.
        * Set the editor as readonly if necessary.
        */
        void PostLayoutDynInitL();

        /**
         * from MCoeControlObserver
         */
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

    protected:  // Functions from base classes

        /** 
        * From CCoeControl
        */
        void FocusChanged(TDrawNow aDrawNow);

        /**
        * From MEikMenuObserver Initializes items on the menu
        *
        * @param aMenuId    Current menu's resource Id
        * @param aMenuPane  Pointer to menupane
        */
        void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);

        /**
        * From CEikDialog 
        * Checks if it ok to exit with aButtonId
        * If it is, takes the text from the text editor
        * and passes it to AppUi object.
        * @param aButtonId Id of the 'pressed' button
        * @return   ETrue if it's OK to exit
        *           EFalse otherwise
        */
        TBool OkToExitL( TInt aButtonId );

        /** 
        * From CCoeControl
        * Draws skin background if available or just clears the background.
        */
		void Draw(const TRect &aRect) const;

        /** 
        * If aEventType is EEventTextUpdate, checks the length of the text field
        * and updates navipane.
        */
		void HandleEdwinEventL(CEikEdwin* aEdwin,TEdwinEvent aEventType);

    protected:  // New functions

        /**
        * DynInitMainMenuL Initializes items on the menu
        * @param aMenuPane  Pointer to menupane
        */
        void DynInitMainMenuL(CEikMenuPane* aMenuPane);

        /**
        * DynInitInsertSubMenuL Initializes items on the menu
        * @param aMenuPane  Pointer to menupane
        */
        void DynInitInsertSubMenuL(CEikMenuPane* aMenuPane);

        /**
        * DynInitContextMenuL Initializes items on the menu
        * @param aMenuPane  Pointer to menupane
        */
        void DynInitContextMenuL(CEikMenuPane* aMenuPane);

        /**
        * IsMainMenuOptionInvisible
        * Returns true if menuoption aMenuOptionId is invisible in Mainmenu
        * @param aMenuOptionId  menu option to be checked
        * @return TBool         True if option is invisible
        */
        TBool IsMainMenuOptionInvisible( TInt aMenuOptionId );

        /**
        * LaunchHelpL
        * Calls AppUi object's LaunchHelpL with right context id.
        */
        void LaunchHelpL( );

        
    private:

        /**
        * Default C++ constructor.
        */
        CPostcardTextDialog( 	CPostcardAppUi& aAppUi, 
        						CPostcardDocument& aDocument, 
        						CPlainText& aText, 
        						TInt& aRetValue, 
        						TKeyEvent aKeyEvent  );

        /**
        * EPOC constructor
        */
        void ConstructL();

        /**
        * Focuses itself. Sets cursor visible if necessary.
        * Adds edwin observer. Activates base class (CCoeControl).
        * Sets the position of the cursor in the end of the text.
        * Updates navipane. Simulates a keyevent if such an event
        * was passed as a parameter in the construction phase.
        */
        void ActivateL();

        /**
        * From CoeControl.
        * Returns 1 or 2.
        */
        TInt CountComponentControls() const;

       /**
        * From CCoeControl.
        * 1 is text editor.
        * 2 is possible scrollbar.
        */
        CCoeControl* ComponentControl(TInt aIndex) const;


    private: //New functions

        /**
        *    Panic
        * @param aReason - the reason of the panic
        */
        void Panic(TInt aReason);

        /**
        * Sets the return parameter iRetValue as Send 
        * and tries to exit the dialog.
        */
		void SendL( );
		
        /**
         * Sets the return parameter iRetValue as Forward
         * and tries to exit the dialog.
         */
        void ForwardMessageL( );
		
        /**
        * Updates the current TMsvEntry and
        * calls AppUi object's ShowMessageInfoL method.
        */
		void MessageInfoL( );
		
        /**
        * Calls NotePad's Fetch-API to fetch a note or template and inserts 
        * it in the position of the cursor. If there's currently
        * highlighted text, inserts the text in the place of the text.
        * @param aNote - If ETrue, fetches note, if EFalse, fetches template
        */
		void InsertFromL( TBool aNote );
		
        /**
        * Checks if the text is exceeded by 1 or more characters
        * and shows an information note.
        */
		void CheckTextExceedingL( );

        /**
        * Sets middle softkey either to empty or to context menu. Empty in viewer
        * mode and when maximum amount characters inserted.
        */
        void SetMiddleSoftkeyL();

    private: //data

		enum PostcardTextFlags 
			{
			EPostcardTextHelpSupported = 1,
			EPostcardTextExceeded = 2
			};

		CPostcardAppUi& 	iAppUi;
		CPostcardDocument& 	iDocument;
		CPlainText& 		iOrigText;

        CEikRichTextEditor*	iTextEditor;    // Richtext editor
        CPostcardTextDialogCustomDraw* iCustomDraw;  // For drawing lines to editor
        CAknNavigationDecorator*    iNaviDecorator;

		TKeyEvent			iKeyEvent;
		
		TInt&				iRetValue;
        TInt            	iTextFlags;
        TInt				iMaxLen;
        
        TInt                iMskResource;

    };

#endif  // POSTCARDTEXTDIALOG_H

// End of File
