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
* Description:  Declares UI class for mail viewer application.
*
*/

#ifndef MSGMAILVIEWERAPPUI_H
#define MSGMAILVIEWERAPPUI_H

// INCLUDES
#include <MMailAppUiInterface.h>
#include <MMessageLoadObserver.h>
#include <akntoolbarobserver.h>
#include <AknWaitNoteWrapper.h>
#include <ConeResLoader.h>
#include <aknnavide.h>  // for CAknNavigationDecorator
#include "MsgMailAppUi.h"
#include "MMsgMailAttachmentDialogObserver.h"
#include <cenrepnotifyhandler.h>

// FORWARD DECLARATIONS
class CMsgMailViewerDocument;
class CMsgAddressControl;
class CAknNavigationDecorator;
class CMsgMailViewerCharConv;
class CMsgMailCharacterSetHandler;
class CMailViewManager;
class CMailMessage;
class CMsgVoIPExtension;
class MMailMsgBaseControl;
class CMsgMailViewerAttachmentsDlg;
class CMailOperation;
class MMsvProgressReporter;
class CMsgMailViewerFindItemHandler;
class CMsgMailViewerContactMatcher;
class CMsgMailViewerContactCaller;
class CAiwServiceHandler;
class CMsgMailViewerReplyToMatcher;

// CLASS DECLARATION

/**
* Application UI class.
*   Msg Mail viewer
*/
class CMsgMailViewerAppUi
  : public CMsgMailAppUi,
	public MAknToolbarObserver,
	public MMailAppUiInterface,
	public MMessageLoadObserver,
	public MCenRepNotifyHandlerCallback,
	public MMsvSingleOpWatcher,
	public MProgressDialogCallback,
	public MMsgMailAttachmentDialogObserver
#ifdef RD_SCALABLE_UI_V2
	,public MAknNaviDecoratorObserver
#endif
	{
    public: // Constructors and destructor
        /**
        * Default constructor.
        */
        CMsgMailViewerAppUi();

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CMsgMailViewerAppUi();

    public: // Functions from base classes
        /**
        * From CMsgEdittorAppUi, returns pointer to document object.
        */
        CMsgMailViewerDocument* Document() const;

        /// From MCenRepNotifyHandlerCallback
        void HandleNotifyInt( TUint32  aId, TInt aNewValue );
        void HandleNotifyError(
        	TUint32 aId, TInt aError,
    		CCenRepNotifyHandler* aHandler );

    public: //Implementation
        void DoReplyViaL( TUid aMtmUid, const HBufC* aMatch = NULL, const HBufC* aName = NULL);

    protected: // From MObjectProvider
        /**
        * Provides access to menubar.
        * @return a pointer to menubar (NULL, if not available)
        */
        TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    private: // Functions from base classes
        /**
        * From MEikMenuObserver, initialise menu pane.
        * This function is called by the UIKON framework just before it
        * displays a menu pane. Its default implementation is empty, and by
        * overriding it, the application can set the state of menu items
        * dynamically according to the state of application data.
        * @param aResourceId resource ID identifying the menu pane being
        *        initialised.
        * @param aMenuPane the in-memory representation of the menu pane.
        */
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event.
        * @return Reponse code (EKeyWasConsumed, EKeyWasNotConsumed).
        */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);

        /**
        * From MMsgEditorLauncher, launches view.
        * Opens received mail.
        */
        void LaunchViewL();

        /**
        * From CMsgEditorAppUi, saves settings, closes viewer.
        * Writes settings in .INI-file. Closes mail editor.
        */
        void DoMsgSaveExitL();


        /**
        * From CMsgEditorAppUi, receives notification
        * when focus has changed.
        */
        void EditorObserver(
            TMsgEditorObserverFunc aFunc,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 );

        /**
        * From CMsgEditorAppUi (MMsgEditorModelObserver),
        * HandleEntryDeletedL is called when the
        * currently open message has been deleted
        * by some other messaging client.
        */
        void HandleEntryDeletedL();

#ifdef RD_SCALABLE_UI_V2
        /**
        * From MAknNaviDecoratorObserver
        * Handles the events coming from next/previous arrows
        */
        void HandleNaviDecoratorEventL( TInt aEventID );
#endif

        // From MMsgMailAttachmentDialogObserver
        void AttachmentDialogExit();

        //From MAknToolbarObserver
        void DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolbar );

		// From MAknToolbarObserver
		void OfferToolbarEventL( TInt aCommand );

	private:
		// From CCoeAppUi
		CArrayFix< TCoeHelpContext >* HelpContextL( ) const;

		// From MMailAppUiInterface
		void AddAttachmentL(
			CMsvAttachment& aAttachmentInfo,
			TBool aCanBeRemoved );
        void AddControlL(TInt aControlType);
        void AddControlL(CCoeControl& aControl);
        void AddControlL(MMailMsgBaseControl& aControl);
        CCoeControl& ParentControl();
        CEikStatusPane* StatusPane();
        void StartWaitNoteL(/*const TDesC& aText*/);
        void StopWaitNote();
        void SetTitleTextL(TInt aResourceId);
        void SetAddressFieldTextL(TInt aControlType, CDesCArray& aText);
        void SetAddressFieldTextL(TInt aControlType, const TDesC& aText);
        void SetBodyTextL(CRichText& aText);
		void SetSubjectL(const TDesC& aText);
		void SetContextBackToMessageL();
		TRect MsgViewRect(TInt aPane);
		void AppUiHandleCommandL(TInt aCommand);
        TBool IsNextMessageAvailableL( TBool aForward );
        void NextMessageL( TBool aForward );
        void ViewMessageComplete();

		// From MMessageLoadObserver
		void MessageLoadingL(TInt aStatus, CMailMessage& aMessage);

		// From MMsvSingleOpWatcher
		void OpCompleted(
		    CMsvSingleOpWatcher& aOpWatcher,
		    TInt aCompletionCode );

        /// From MProgressDialogCallback
        void DialogDismissedL( TInt aButtonId );

    private: // Implementation

		CRichText* BodyTextL();
        void DoDeleteL();
        void DoReplyOrForwardL(const TInt aOperation);
        void DoAttachmentL();
        void DoMoveL();
        void DoCreateContactL(TInt aCommand);
        void DoCreateAttachmentControlL();
        void DoMessageInfoL() const;
        void DoHideToolbar( const TBool aHide );
        void DoConvertToCharsetL( TUint aCharset );
        void DoUpdateNaviPaneL();
        TBool HandleExternalCmdL( TInt aCommand );
        void HandleInternalCmdL( TInt aCommand );
        TBool DoHideReplyToAll() const;
        TBool DoHideReplyToSender() const;
        TBool HasAddressFieldText(CMsgAddressControl* aAddressControl) const;
        TBool HasAnyAddressFieldText() const;
        TBool HasValidFromAddress() const;
        const TDesC& FromOrToAddressL() const;
        void HandleEKeyYesL();
        TUint GetCharsetIdL() const;
        CMsgRecipientArray* GetRecipientArrayL(
            CMsgAddressControl* aAddressControl ) const;
        CMsgAddressControl* GetFirstAddressControl(
            TBool aDirection = ETrue ) const;
        const TDesC& FirstRecipientAddressL(
            CMsgAddressControl* aAddressControl ) const;
        const TDesC& FirstRecipientNameL(
            CMsgAddressControl* aAddressControl ) const;

        void DoReplyOptionsMenuL(CEikMenuPane& aMenuPane);
        void DoRetrieveMessageL();
        void SetAutomaticHighlightL( TInt aValue );
        void DoSetAutomaticHighlightL();
        void SetAddressControlHighlight(
            CMsgAddressControl* aControl, TInt aValue );
        void DynInitOptionsMenu1L(CEikMenuPane& aMenuPane);
        void DynInitOptionsMenu2L(CEikMenuPane& aMenuPane);
        TBool IsDefaultSendKeyValueVoIPL() const;
        void LoadResourceFileL(const TDesC& aResourceName);
        TMsgControlId FocusedControlId() const;
        TBool ValidateServiceL( TUid aUid );
        TBool VoipProfileExistL() const;
        TInt MessageAddresseeCount() const;
        void ShowWaitNoteL( TBool aCanCancel );
        void CloseWaitNote();
        void ForwardOperationL();
        void SetMiddleSoftKeyLabelL(
            const TInt aResourceId, const TInt aCommandId );
        void RemoveMSKCommands();
        TBool SpecialScrolling( TUint aKeyCode );
        TBool CanMakeCallToContactL() const;
        CMsgAddressControl* FocusedOrFirstAddressControl() const;
        TBool IsAttachment() const;
        
        /**
         * Handles Enter key events.
         */
        void HandleEnterKeyL();

    private: //Data

        /**
        * Own: Wait note shown during launch.
        */
        CAknWaitNoteWrapper* iWaitNote;

        /**
        * Own: AIW service handler, use single handler for all
        * AIWs to avoid menu misbehavior.
        */
        CAiwServiceHandler* iServiceHandler;

        /**
        * Own: Contact matcher used when calling or replying to a sender
        */
        CMsgMailViewerContactMatcher* iContactMatcher;

        /**
        * Own: handles CFindItemMenu related tasks
        */
        CMsgMailViewerFindItemHandler* iFindItemHandler;

        /**
        * Own: handles CallUi for contact cooresponding to sender
        */
        CMsgMailViewerContactCaller* iContactCaller;

        /**
        * State of the launching
        */
        TInt iState;

        /**
        * Keeps count of character when changing line breaks to paragraph
        * delimitters.
        */
        TInt iCount;

        /**
        * Own: Copy of the body text, used when converting linebreaks.
        */
        HBufC* iText;

        /**
        * Own: Dynamic Charconverter menu handler
        **/
        CMsgMailCharacterSetHandler* iCharSetHandler;

        /**
        * MUIU Email Local variation flags
        **/
        //TInt iMuiuEmailLVFlags;

        // Plug-in resource loader
        RConeResourceLoader iResourceLoader;

        // Own: Wait dialog
        CAknWaitDialog* iWaitDialog;

        // Own: mail view manager
        CMailViewManager* iViewManager;

        // Own: Contains information of the VoIP profiles.
        CMsgVoIPExtension* iMsgVoIPExtension;

        // Viewer Params
        TUint iParams;
        // Internal Flags
        TUint iFlags;

        //own: attachment dilalog title
        HBufC* iTitle;

        // Internal Flag
        TBool iAttachmentLoadingOn;

        // Ref: to running operation
        CMailOperation* iOperation;

        // Own: Operation watcher
        CMsvSingleOpWatcher* iOperationWatcher;

        // Ref: to progress reporter
        MMsvProgressReporter* iProgressReporter;

        //Not owned
        CMsgMailViewerAttachmentsDlg* iAttachmentdlg;

        TInt iLastControlId;

        CMsgMailViewerReplyToMatcher* iReplyToMatcher;

        CAknWaitDialog*  iOpeningWaitDialog;

        UNIT_TEST(ut_mailviewerappui)

        /**
        * Saved focused control for touch support.
        * Does not own.
        */
        CMsgBaseControl* iFocusedControl;

        //Not owned
        CAknNavigationControlContainer* iNaviPane;

        // ETrue while the message is being reloaded
        TBool	iMessageReload;
        
        // Default state for dialer
        //      ETrue  : Dialer is enabled
        //      EFalse : Dialer is disabled
        TBool   iDefaultDialerState;

    };

#include "MsgMailViewerAppUi.inl" // Inline functions

#endif // MSGMAILVIEWERAPPUI_H

// End of File

