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
* Description: 
*     Defines a dialog for view/edit sending options.
*
*/


#ifndef MSGMAILOPTIONSDLG_H
#define MSGMAILOPTIONSDLG_H

//  INCLUDES

#include <AknForm.h>            // for CAknDialog
#include <e32std.h>
#include <eiklbo.h>             // for MEikListBoxObserver

// FORWARD DECLARATIONS
class CMsgMailEditorDocument;
class CMsgEditorView;
class CAknTitlePane;
class TCoeHelpContext;
//class TListBoxEvent;
class CMailEditorSettingList;

// CLASS DECLARATION

/**
*  Dialog which is used to view or edit current send options.
*  
*/
class CMsgMailOptionsDlg : public CAknDialog, public MEikListBoxObserver
    {                                   
    public:  // New functions
        /*
        * Creates CMsgMailOptionsDlg
        * @param aDocument reference to the document. (Leaves if NULL)
        * @param aHelpSupported ETrue if supports Help
        * Note: After creation call ExecuteLD() to launch the dialog
        */
        static CMsgMailOptionsDlg* NewL(CMsgMailEditorDocument* aDocument, 
            TBool aHelpSupported);
        
        /**
        * From MEikListBoxObserver
        */
        virtual void HandleListBoxEventL(CEikListBox* aListBox, 
        		TListBoxEvent aEventType);
    
    protected: // Constructors and destructor

        CMsgMailOptionsDlg();
      
        /**
        * ConstructL()
        */      
        void ConstructL();

        ~CMsgMailOptionsDlg();


    protected: // Functions from base classes
        
        /**
        * From CAknDialog
        */
        TBool OkToExitL(TInt aButtonId);
        void ProcessCommandL(TInt aCommandId);
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent, 
            TEventCode aType );
        void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);
        /**
        * From CEikDialog
        */
        void PreLayoutDynInitL();
        
        
        void SizeChanged();
        
        /**
        * From CCoeControl
        */
        void HandleResourceChange( TInt aType );
        
        
    protected: //Data
        /**
        * ref: document object.
        */
        CMsgMailEditorDocument* iDocument;
        /**
        * Own: Setting list
        */
        CMailEditorSettingList* iSettingList;

    private: //Data 
                              
        /**
        * ref: Title pane.
        */
        CAknTitlePane*  iTitlePane;
        
        /** 
        * ref: Saved title pane text
        */
        HBufC* iOldTitlePaneText;
        
        /**
        * Help feature flag
        */
        TBool iHelpSupported;
    };

#endif      // MSGMAILOPTIONSDLG_H   

// End of File
