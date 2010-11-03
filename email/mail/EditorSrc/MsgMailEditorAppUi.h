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
* Description:  Declares UI class for Mail application.
*
*/


#ifndef MSGMAILEDITORAPPUI_H
#define MSGMAILEDITORAPPUI_H

// INCLUDES
#include <RPbkViewResourceFile.h>
#include <MsgAttachmentUtils.h>
#include <akntoolbarobserver.h>
#include <msvstd.h>
#include "MsgMailAppUi.h"
#include "mmsgmailappuiopdelegate.h"
#include "cmsgmailbaseop.h"



// FORWARD DECLARATIONS
class CMsgMailEditorDocument;
class CMsgMailSendOptionsDlg;
class CMsgExpandableControl;
class CMsgAddressControl;
class CPbkContactEngine;
class CMsgRecipientItem;
class CAknWaitDialog;
class CMsgRecipientList;
class TAdditionalHeaderStatus;
class CMsgMailViewAttachmentsDlg;
class CMsgCheckNames;
class CMsgMailEditorHeader;
class CMsgMailNaviPaneUtils;
class CAknLocalScreenClearer;

/**
* Application UI class.
*/
class CMsgMailEditorAppUi
    : public CMsgMailAppUi,
      public MAknToolbarObserver,
      public MMsgMailAppUiOpDelegate,
      public MMsgMailOpObserver
    {
    public: // // Constructors and destructor
        /**
        * Default constructor
        */
        CMsgMailEditorAppUi();

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CMsgMailEditorAppUi();    
 
        /**
        * inline
        * Return pointer to mail editor document.
        * @return CMsgMailEditorDocument.
        * Ownership not transferred.
        */        
        inline CMsgMailEditorDocument* Document() const;
        
    public: //New functions  
        /**
         * Fetches attachment from other application and
         * adds it to message.
         * @param aCommand Type of fetch operation
         * we want to proceed e.g. EMsgMailEditorCmdInsertImage.
         */
        void DoFetchFileL( TInt aCommand );
        
        /**
         * Displays toolbar Insert Other selection list.
         * If option is selected proceeds to fetching.
         */
        void DoShowToolBarOtherL();

    public: // Functions from base classes  
        /// From MMsgEditorLauncher
        void LaunchViewL();

        //From MAknToolbarObserver 
        void DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolbar );
        		
		// From MAknToolbarObserver
		void OfferToolbarEventL( TInt aCommand );
        
        // From CEikAppUi
        void HandleCommandL(TInt aCommand);
        TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,
            TEventCode aType);
       
        
    protected: // Functions from base classes
    	// From MObjectProvider
        TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
        
        // From CEikAppUi        
        void HandleSystemEventL( const TWsEvent& aEvent );
	
    private: // Functions from base classes
		// From CCoeAppUi
		CArrayFix< TCoeHelpContext >* HelpContextL( ) const;
		
        /// From MEikMenuObserver
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
        
        /// From MMsgEditorObserver.
        void EditorObserver(
            TMsgEditorObserverFunc aFunc,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 );  
                                            
        /// From MMsgMailAppUiOpDelegate
        void DelegateSaveMsgL();        
        TMsvId DelegateMoveMsgToOutboxL();
        CMsgBodyControl& DelegateInitLaunchL();
        void DelegateExecuteViewL();
        void DelegateFinalizeLaunchL();
        
        /// From MMsgMailOpObserver
        void HandleOpCompleted( const CMsgMailBaseOp& aOp,
                                TInt aResult );

    private: // implementation
    
        void HandleAttaAddedL();    
        void SetTitlePaneL( TInt aResourceId );
		void DoCreateControlsL();
        void DoAttachmentL();
        void DoCheckEncodingL();
        void DoMsgSaveExitL();
        void PrepereToExitL();
        void DoSendMessageL();
        void DoSendOptionsL();
        void DoHideToolbar( const TBool aHide );
        void DoShowSelectionListL();
        void DoShowExtensionToolBar();

        void DoShowToolBarTextL();
        void DoBackstepL();
        void DoAddRecipientL();
        void DoAdditionalHeadersL();
        void DoGetRecipientsL();
        void DoRemoveDuplicatesL();
        void DoHandleCommandL( TInt aCommand );
        void EditorObserverL(
            TMsgEditorObserverFunc aFunc,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 );
        TBool CheckRecipientsL(TBool aHighLight);
        void ShowIncorrectAddressErrorL(const TDesC& aTitle,
            const TDesC& aAddress) const;
        TBool IsEmptyL() const;
        void SetSubjectL(const TDesC& aText) const;
        TBool AreAddressFieldsEmptyL() const;
        void UpdateMessageSizeL();
        void ShowSelectedMailboxL();
        void CheckRecipientsInFieldL(CMsgAddressControl* aAddressField,
            TBool& aHighlight, TBool& aResult);
        void RemoveDuplicatesL(CMsgAddressControl* aAddressField);
        void RemoveDuplicatesL(CMsgAddressControl* aAddressField, 
        	CMsgAddressControl* aLaterAddressField);
        void AddRecipientArrayL(CMsgAddressControl* aControl,
            const CDesCArray& aArray);
        void UpdateHeaderVisibilityL(
        	RPointerArray<TAdditionalHeaderStatus>& aHeaders);
        void UpdatePriorityIndicatorL();	
        void DoMessageInfoL();
        void InitPhoneBookL();
        void SearchUnverifiedStringL(const TDesC& aText, 
            CMsgAddressControl* aControl);
        TBool AllAddressValidL();
        void SetFocusToFirstFreeFieldL();
        void StopWaitNote();
        void StartWaitNoteLC( 
            TInt aStringResourceId, 
            TBool aDelayOff = EFalse );
        void StartWaitNoteLC( 
            TInt aStringResourceId, 
            TBool aDelayOff, 
            TInt aResouceID );
        HBufC* StringLoaderLoadLC( TInt aTextResource );
        TBool SpecialScrolling( TUint aKeyCode );
        void RotateFocus( TBool aDirection, TInt aCurrentFocus );
        void SetMiddleSoftKeyLabelL( 
            const TInt aResourceId, const TInt aCommandId );
        void RemoveMSKCommands();
        void CheckFocusForMskL(TInt aControl);
        TBool CheckGlobalReplyToSettingL();
        void SetInputModeToAddressFields( TInt aInputMode );
        void SetFocusAfterAddRecipientL( TMsgControlId aFocusBeforeAdd );
        
#ifdef RD_SCALABLE_UI_V2
        void ControlTouchedL( const CMsgBaseControl& aControl );
#endif

        /*
         * Removes unneeded prefixes from subject text
         */
        void HandleSubjectL();

        /**
         * Handles enter key events.
         */
        void HandleEnterKeyL();
        
    private: // Data
        
        /**
        * Own: address finder
        */    
        CMsgCheckNames* iCheckNames;

        /**
        * Own: A wait dialog
        */
        CAknWaitDialog* iWaitNote;    
        
        /**
         * Mail editor UI header handler
         * Own.
         */        
        CMsgMailEditorHeader* iMailEditorHeader;
        
        /**
        * Middle soft key command
        */
        TInt iCurrentMskCommandId;

        /**
        * Navi pane utils. Own. 
        */        
        CMsgMailNaviPaneUtils* iNaviPaneUtils;
        
        /**
        * Attachment dialog. Own. 
        */
        CMsgMailViewAttachmentsDlg* iAttachmentDlg;    
        
        /**
        * Saved focused control for touch support. 
        * Does not own.
        */
        CMsgBaseControl* iFocusedControl;
        
        /**
        * Operation object for sending a message. Own.
        */
        CMsgMailBaseOp* iSendOp;
        
        /**
        * Operation object for launching editor. Own.
        */
        CMsgMailBaseOp* iLaunchOp;
        
        /**
        * Operation object for fetching (attaching) files. Own.
        */        
        CMsgMailBaseOp* iFetchFileOp;
        
        /**
         * Document is deleted when the editor is closed.
         * ETrue  : Always delete document
         * EFalse : Default behaviour 
         */
        TBool iForceDeleteDocument;
        
        /**
        * Screen clearer. Own. 
        */    
        CAknLocalScreenClearer* iScreenClearer;        
        
    };

// Inline functions
#include "MsgMailEditorAppUi.inl"

#endif

// End of File
