/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*      Notification Viewer
*
*/



#ifndef NOTVIEWERAPPUI_H
#define NOTVIEWERAPPUI_H

//  INCLUDES
#include <bldvariant.hrh>
#include <finditemengine.h>             // FindItem stuff 
#include <MsgEditorAppUi.h>             // CMsgEditorAppUi
#include <ConeResLoader.h>              // ResLoader
#include <cenrepnotifyhandler.h> // link against CenRepNotifHandler.lib
#include <MuiuMsvSingleOpWatcher.h>     // Single operation watcher
#include <muiumsvuiserviceutilitiesinternal.h> // TMuiuAddressType

#include <MsvPrgReporter.h>             // MMsvProgressDecoder
#include <AknNaviDecoratorObserver.h>   // MAknNaviDecoratorObserver
#include <txtfrmat.h>

#ifdef RD_SCALABLE_UI_V2
#include <akntoolbarobserver.h>
#include "NotUi.hrh"
#endif

#include "NotViewer.hrh"                // Panic codes

// FUNCTION PROTOTYPES
GLREF_C void Panic(TNotViewerExceptions aPanic); 

// FORWARD DECLARATIONS
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
class CAknNavigationDecorator;
#endif
class CAknNavigationControlContainer;
class CEikRichTextEditor;
class CRichText;
class CNotViewerDocument;
class CMmsNotificationClientMtm;
class CSendUi;
class CFindItemMenu;
class CFindItemEngine;
class CMsgAddressControl;
class CMmsSettingsHandler;
class CRepository;
class CMsgVoIPExtension;
class CAknInputBlock;
class CUniAddressHandler;
#ifdef RD_SCALABLE_UI_V2
class CAknToolbar;
#endif


// CLASS DECLARATION
/**
*  CNotViewerAppUi
*  Inherited from CMsgEditorAppUi
*/
class CNotViewerAppUi : public CMsgEditorAppUi,     
                        public MMsvSingleOpWatcher,
                        public MCenRepNotifyHandlerCallback,
                        public MMsvProgressDecoder
#ifdef RD_SCALABLE_UI_V2
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
                        , public MAknNaviDecoratorObserver       //pen support for navi decorator
#endif
                        , public MAknToolbarObserver             //toolbar
#endif
    {

    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CNotViewerAppUi();

        /**
        * Destructor.
        */
        virtual ~CNotViewerAppUi();

    public: // From base classes

        /**
        * EntryChangedL (from CMsgEditorAppUi)
        * Called by Document when the TMsvEntry (usually status field)
        * has been changed. This function updates the field in richtext body.
        * (by calling these AddXXXFieldL functions)
        */
        void EntryChangedL( );

        /**
        * From MMsvSingleOpWatcher, called when operation has completed
        */
        virtual void OpCompleted(
            CMsvSingleOpWatcher& aOpWatcher,
            TInt aCompletionCode );

        /**
        * Called by OpCompleted when user has selected to delete notification
        * both from server and locally. It initializes CAknInputBlock,
        * starts the local delete process and initializes iIdle
        */
        void DoDeleteOpCompletedL( );
            
        /**
        * From MMsvProgressDecoder
        * decode progress
        * @param aProgress: progress buffer
        * @param aReturnString: String to be displayed to the user usually in the CMsvWrappableProgressDialog dialog.
        * @param aTotalEntryCount: total entries to be processes
        * @param aEntriesDone: entries processes so far
        * @param aCurrentEntrySize: total size of the entries to be processes
        *           used to display progress bar (for example mail size to be fetched is 42000 bytes)
        *           If for example deleting messges then this should be number of messages to be deleted
        * @param aCurrentBytesTrans: number of bytes received so far
        *           used to display progress bar (for example received bytes is 21000 bytes, then progress bar half done)
        *           If for example deleting messges then this should be number of messages to deleted so far
        * @param aInternal: used only in CImapConnectionOp
        */
        TInt DecodeProgress (
            const TDesC8& aProgress,
            TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans, TBool aInternal );

    private: // New functions
        
        /**
        * The handling of opening options-menu
        * @param aMenuPane pointer to CEikMenuPane
        */
        void DynInitOptionsMenuL( CEikMenuPane* aMenuPane );

        /**
        * The handling of opening reply-menu
        * @param aMenuPane pointer to CEikMenuPane
        */
        void DynInitReplyMenuL( CEikMenuPane* aMenuPane );

        /**
        * The handling of opening context sensitive menu
        * @param aMenuPane pointer to CEikMenuPane
        */
        void DynInitContextMenuL( CEikMenuPane* aMenuPane );

        /**
        * Starts the fetching of MMS message. Calls the FetchMessageL
        * function of NotificationClientMtm
        */
        void FetchL( );

        /**
        * Turns Viewer into Editor for forwarding the notification.
        * Calls Forward function of the base class CMsgEditorAppUi.
        * In the end updates the body text by calling EntryChangedL.
        */
        void DoForwardL( );

        /**
        * First verifies all the addresses in the TO field. Then calls
        * SendL function of NotificationClientMtm to send the forward request.
        * Exists the Viewer in the end.
        */
        void SendL( );

        /**
        * Replying to a message. Uses CSendAppUi to open either MMS or SMS Editor.
        * Exists the Notification Viewer after opening the Editor.
        * Overrides the function in base class.
        * @param aReplyMms IN If ETrue, opens MMS Editor, otherwise SMS Editor.
        */
        void UniReplyL( TBool aReplyMms );

        /**
        * Deletes the notification that is currently open in the Notification Viewer.
        * There are three different alternatives: For all the notifications it calls
        * DeleteNotificationL function of NotificationClientMtm. For unsent Forward
        * Request just Exit is called (as there's nothing saved in the MCE for unsent
        * messages). For sent Forward Request DeleteAndExitL function is called.
        */
        void DeleteNotificationL( );
        
        /**
        * Calls MsvUiServiceUtilities's CallServiceL function to make a phone call
        * either the sender of the notification or focused number in body field
        */
        void CallToNumberL( );
     
        /**
        * Calls MmsAddressHandler's AddRecipientL function to add recipients into 
        * the To field. Moves focus there if recipients were added.
        */
        void AddRecipientL( );

        /**
        * Calls VerifyAddressesL to verify the addresses in the To field.
        */
        void CheckNamesL( );        

        /**
        * VerifyAddressesL()
        * Calls MmsAddressHandler's VerifyAddressesL function to verify every address.
        * @param aModified OUT ETrue, if some of the address where modified during verification
        * @retun ETrue if all the addresses are OK
        */
        TBool VerifyAddressesL( TBool& aModified );

        /**
        * Calls HlpLauncher's LaunchHelpApplicationL function to launch Help application.
        */
        void LaunchHelpL( );

        /**
        * Launch msg info dialog
        * If the address control has been modified since the message has been saved the previous
        * time, AddressHandler's CopyAddressToMtmL function is called to update the Client MTM.
        * Then the message is saved by calling Client MTM's SaveMessageL function.
        * Then UI MTM's InvokeAsyncFunctionL function is called with command id
        * KMtmUiFunctionMessageInfo.
        */
        void MessageDetailsL( );

        /**
        * Returns a pointer to current richtext
        * @return reference to CEikRichText
        */
        CRichText& RichText( ) const;

        /**
        * Return a pointer to document-object
        * Overrides the function in base class.
        * @return CNotEditorDocument* a pointer to CNotViewerDocument-object
        */        
        CNotViewerDocument* Document( ) const;
        
        /**
        * Adds a label and value pair.
        * @param aLabel text to displayed in label text format.
        * @param aValue text to displayed in text format.
        */
        void AddItemL( const TDesC& aLabel, const TDesC& aValue );

        /**
        * Applies the format of value and label text to viewer.
        * @param aFormat Character format to be applied. 
        * @param aMask Character format mask
        * @aIsLabel Determines is the text label or value. 
        */
        void ApplyFormat( TCharFormat& aFormat, TCharFormatMask& aMask, TBool aIsLabel );

        /**
        * Add text to viewer, checks and removes linefeeds and carriage returns
        * @param aText text to be added
        * @param aFormat Character format
        * @param aMask Character format mask
        */
        void AppendTextL( const TDesC& aText,
            const TCharFormat& aFormat,
            const TCharFormatMask& aMask );

        /**
        * Returns the control id of the currently focused control.
        * @return Control id of the focused control or
        *         EMsgComponentIdNull if no control focused
        */
        TInt FocusedControlId();

    protected: // Functions from base classes

        /**
        * From CMsgEditorAppUi
        * This handles the case if mms entry currently viewed is suddenly deleted from MSGS
        * In other words, just Exit is called.
        */
        void HandleEntryDeletedL( );

        /**
        * From CMsgEditorAppUi
        * This handles the case if mms entry currently viewer is changed (status changed etc.)
        */
        void HandleEntryChangeL();
    
        /**
        * From MMsgEditorObserver
        */
    	void EditorObserver(
	        TMsgEditorObserverFunc aFunc, TAny* aArg1, TAny* aArg2, TAny* aArg3);

        void DoEditorObserverL(TMsgEditorObserverFunc aFunc, TAny* aArg1, TAny* aArg2, TAny* aArg3);

        /**
        * From MAknNaviDecoratorObserver
        * Handles the events coming from arrow presses
        *  related to next/previous message
        */
#ifdef RD_SCALABLE_UI_V2
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
        void HandleNaviDecoratorEventL( TInt aEventID );   
#endif
#endif        
    private: // Functions from base classes

        /**
        * From MsgEditorAppUi 
        * Calls just Exit() as nothing is really saved in Notification Viewer when
        * exit is pressed.
        */
        void DoMsgSaveExitL( );
            
        /**
        * DoSaveL( )
        * This is called only before the sending of a Forward Request is started.
        * It first calls AddressHandler's function CopyAddressesToMtmL and then calls
        * ClientMtm's SaveMessageL to save the latest changes. Then AddressHandler's
        * MakeDetails is called. 
        */
        void DoSaveL( );

        /**
        * From MMsgEditorLauncher 
        */
        void LaunchViewL( );

        /**
        * From CCoeAppUi 
        * @param aKeyEvent - check CCoeAppUi
        * @param aType - check CCoeAppUi
        * @return TKeyResponse - check CCoeAppUi
        */
        TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * From CEikAppUi 
        * @param aCommand - check CEikAppUi
        */
        void HandleCommandL( TInt aCommand );

        /**
        * DoSelectionKeyL 
        * Handles selection and enter keys.
        * If the focus is on To field - AddRecipientL is called.
        * Otherwise opens the context sensitive menu.
        * @return ETrue, selection or enter key was handled
        *         EFalse, selection or enter key was not handled
        */
        TBool DoSelectionKeyL();

        /**
        * DoHandleSendKeyL 
        */
        void DoHandleSendKeyL();


        /**
        * From MEikMenuObserver 
        * @param aMenuId look from MEikMenuObserver
        * @param aMenuPane look from MEikMenuObserver
        */
        void DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane );

        /**
        * From CCoeAppUi
        * Returns help context
        */
        CArrayFix<TCoeHelpContext>* HelpContextL( ) const;  

        /**
        * AddSubjectFieldL
        * Adds message field into the rich text body. Gets the right sentence from the
        * resource file according to the current message status and calls AddItemL.
        */
        void AddMessageFieldL( );

        /**
        * AddSubjectFieldL
        * Adds subject field into the rich text body
        */
        void AddSubjectFieldL( );

        /**
        * AddTextFieldL
        * Adds text field into the rich text body
        */
        void AddTextFieldL( );

        /**
        * AddExpiryFieldL
        * Adds expiry field into the rich text body
        */
        void AddExpiryFieldL( );

        /**
        * AddSizeFieldL
        * Adds size field into the rich text body
        */
        void AddSizeFieldL( );

        /**
        * AddClassFieldL
        * Adds class field into the rich text body
        */
        void AddClassFieldL( );

        /**
        * AddPriorityFieldL
        * Adds priority field into the rich text body
        */
        void AddPriorityFieldL( );

        /**
        * ShowConfirmationQueryL
        * Creates CAknQueryDialog with resource aResourceId and executes it.
        * User is shown a confirmation query.
        * Shows confirmation query with question aResourceId
        * @param IN aResourceId id of the confirmation resource
        * @return OUT return value of dlg->ExecuteLD
        */
        TInt ShowConfirmationQueryL( TInt aResourceId ) const;

        /**
        * ShowInformationNoteL
        * Shows information note for user
        * @param aResourceID
        * @param aWaiting, is note waiting or not.
        */  
        void ShowInformationNoteL(TInt aResourceID, TBool aWaiting );
        

        /**
        * AddressControl
        * Returns pointer to the current AddressControl (either To or From).
        */
        CMsgAddressControl* AddressControl( );

        /**
        * UpdateTitlePane
        * This is called only when there is Forward Request open.
        * Updates the title text in the Title pane. In case of unsent Forward
        * Request, changes the CBA to Options-Cancel.
        */
        void UpdateTitlePaneL( );        
        
        /**
        * MessageStatus
        * Gets the status of the current message according to the expiry time and
        * iMtmData2 field. Not applicable for Forward Requests.
        * @return OUT TNotViewerStatus - the status of the notification.
        */
        TNotViewerStatus MessageStatus( ) const;

        /**
        * GetTypeL
        * Checks if the entry is a notification or Forward Request according to
        * the iMtmData1 field and sets the right bit in iFlags.
        * Also calls MessageStatus to update the status of iMsgStatus.
        * Leaves with KErrNotSupported, if unknown notifcation type flags set in 
        * TMsvEntry::iMtmData1.
        */
        void GetTypeL( );


        /**
        * Sets automatic highlighting on/off.
        * @param aEnable if ETrue sets automatic highlighting on,
        *                if EFalse sets automatic highlighting off
        */
        void SetFindModeL( TBool aEnable );

        /**
        * From MMessageIteratorObserver
        * @param See documentation of MMessateIteratorObserver
        */
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
        void HandleIteratorEventL(TMessageIteratorEvent aEvent);
#endif
        /**
        * Initializes navipane according to message content
        */
        void InitNavipaneL();

        /**
        * Sub function for InitNavipaneL();
        * @param aText string in the right hand side of navipane
        */
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
        void CreateTabGroupL( const TDesC& aText );
#endif

        /**
        * Checks if the message has been deleted from MMBox already
        * @result ETrue - already deleted
        *         EFalse - not deleted yet
        */
        TBool AlreadyDeletedFromServer( );

        /**
        * Checks if the MMS AP Settings are ok
        * @result ETrue - they were ok, it's ok to continue
        *         EFalse - Not ok and cancel was selected
        */
        TBool CheckAPSettingsL( );

#ifdef RD_SCALABLE_UI_V2
        /**
         * From MAknToolbarObserver
         * Should be used to set the properties of some toolbar components 
         * before it is drawn.
         * @param aResourceId The resource ID for particular toolbar
         * @param aToolbar The toolbar object pointer
         */
        virtual void DynInitToolbarL( TInt aResourceId, 
                                               CAknToolbar* aToolbar );
        
        /**
         * From MAknToolbarObserver
         * Handles toolbar events for a certain toolbar item.
         * @param aCommand The command ID of some toolbar item.
         */
        virtual void OfferToolbarEventL( TInt aCommand );

        /**
        * OperationOngoing returns status whether operation is going on.
        * Needed by MsgStatus
        */
        TBool OperationOngoing( const TMsvEntry& aEntry ) const;

        /**
        * OperationFinished returns status whether operation has finished
        * successfully or unsuccessfully
        * Needed by MsgStatus
        */
        TBool OperationFinished( const TMsvEntry& aEntry ) const;
        
        /// From CCoeAppUi
        void HandleForegroundEventL(TBool aForeground);

#endif
   

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        void HandleNotifyInt(TUint32 aId, TInt aNewValue);
        void HandleNotifyError(TUint32 aId, TInt error, CCenRepNotifyHandler* aHandler);
        void HandleNotifyGeneric(TUint32 aId);

	    /**
	     * Starts CPeriodic to call DelayedExit after aDelay
	     */
		void DoDelayedExitL( const TInt aDelay );

	    /**
	     * Tries to exit the application. Called by CIdle
	     */
	    void DoInternalExitL( );

	    /**
	    * Callback for CIdle
	    */        
		static TInt DelayedExit( TAny* aThis );

        /** 
        * From CEikAppUi
        */
        void HandleResourceChangeL(TInt aType);

        /**
        * Sets title of the mms viewer app into status pane
        */
        void SetTitleIconL();

    private:     // Data

        CMmsNotificationClientMtm*  iMtm;           // Client MTM of MMS Notifications

        CMmsSettingsHandler*		iSettingsHandler; // Settings handler
        CUniAddressHandler*         iAddressHandler; // Wrapper for address handling
		CPeriodic*                  iIdle;          // Idle stuff
        CAknInputBlock*             iAbsorber;      // For absorbing keypresses

        CCenRepNotifyHandler*       iNotifyHandler;
        CRepository*                iCUiRepository;
        CRepository*                iMuiuSettRepository;

        CSendUi*                    iSendUi;        // For replying to the sender
        CFindItemMenu*              iFindItemMenu;

        CAknNavigationControlContainer* iNaviPane;
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
        CAknNavigationDecorator*    iNaviDecorator; // Overrides the variable in base class
#endif
        CMsvSingleOpWatcher*        iOperation;

        // Own: Contains information of the VoIP profiles.
        CMsgVoIPExtension*          iMsgVoIPExtension;

        HBufC*                      iAlias;         // The alias name of the sender
        TMuiuAddressType            iSenderType;    // Phone, email, none
        TNotViewerStatus            iMsgStatus;     // Waiting, expired, forwarding etc.
        TInt                        iFlags;         // Help, auto find, notification etc.
        TMsvId                      iMsgId;         // Id of the message
        TCharFormat                 iLabelFormat;
        TCharFormatMask             iLabelMask; 
        TCharFormat                 iValueFormat;
        TCharFormatMask             iValueMask; 
#ifdef RD_SCALABLE_UI_V2
        CAknToolbar*                iToolbar;
#endif
        CAknTitlePane*              iTitlePane; 
        CGulIcon*                   iAppIcon;
        // Remote address is shown in the From field
        TBool                       iRemoteAlias;
    };

#endif // NotVIEWERAPPUI_H

//  End of File
