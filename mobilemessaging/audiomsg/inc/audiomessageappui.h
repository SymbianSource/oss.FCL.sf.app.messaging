/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides audiomessage appui prototypes
*
*/




#ifndef _AUDIOMESSAGEAPPUI_H
#define _AUDIOMESSAGEAPPUI_H

// includes needed by inline functions:
#include <MsgEditorView.h>          // CMsgEditorView
#include <MsgAddressControl.h>      // CMsgAddressControl
#include <MsgExpandableControl.h>   // CMsgExpandableControl
#include <MsgBodyControl.h>         // CMsgBodyControl
#include "audiomessagedocument.h"   // CAudioMessageDocument

#include <unimodelconst.h>
#include <MsgEditorAppUi.h>         // CMsgEditorAppUi
#include <cenrepnotifyhandler.h>    // MCenRepNotifyHandlerCallback
#include <MMGFetchVerifier.h>        // MMGFetchVerifier
#include "amsoperationobserver.h"   // MAmsOperationObserver
#include "amsvolobserver.h"         // MAmsVolumeObserver

#include "audiomessage.hrh"
#include <AknNaviDecoratorObserver.h>
#include <eikedwob.h>                   // for MEikEdwinObserver
#include <AknProgressDialog.h>          // for MProgressDialogCallback
#ifdef RD_SCALABLE_UI_V2
#include <akntoolbarobserver.h>
#endif


GLREF_C void Panic( TAmsEditorPanic aPanic ); 

class CAknTitlePane;
class CFindItemMenu;
class CMsgVoIPExtension;
class CMsgExpandableControl;
class CAudioMessageInsertOperation;
class CAudioMessageLaunchOperation;
class CAudioMessageSaveOperation;
class CAudioMessageSendOperation;
class CAudioMessageProgressDialog;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CAknIconArray;
class CAknIndicatorContainer;
class CMessageData;
class CSendUi;
class CAknLocalScreenClearer;
class CAknInputBlock;
class CAudioMessageSendReadReportOperation;
class CMsgMediaInfo;
class CUniAddressHandler;
#ifdef RD_SCALABLE_UI_V2
class CAknToolbar;
#endif


/**
* CAudioMessageAppUi
*
* @lib AudioMessage.exe
* @since S60 v3.1
*/
class CAudioMessageAppUi : 
    public CMsgEditorAppUi,
    public MCenRepNotifyHandlerCallback,
    public MMGFetchVerifier,
    public MAmsOperationObserver,
    public MAmsVolumeObserver,
    public MAknNaviDecoratorObserver,
    public MEikEdwinObserver,
    public MProgressDialogCallback,
#ifdef RD_SCALABLE_UI_V2    
    public MAknToolbarObserver             //toolbar    
#endif    
    {

   private:     // Enumerations

	    enum TAmsEditorFlags
	        {
	        ELaunchSuccessful       = 0x0001,
	        ERunAppShutterAtExit    = 0x0002,
	        EEditorClosing          = 0x0004,
	        EAutohighLightEnabled   = 0x0008,
	        EProgressInUse			= 0x0010,
	        EEditorExiting          = 0x0020
	        };

	    enum TAmsNaviType
			{
			EAmsOnlyLength=0,
			EAmsTextEditorAndLength,
			EAmsMsgArrows
			};


    public:  // Constructors and destructor
        
        /**
         * C++ default constructor.
         */
        CAudioMessageAppUi();

        /**
         * Destructor.
         */
        virtual ~CAudioMessageAppUi();
 				void HandleWsEventL( const TWsEvent& aEvent,CCoeControl* aDestination );

        /**
         * Removes audiofile.
         */
        void RemoveAudioL(  );

        /**
         * From MAmsVolumeObserver.
         *
         * Audio volume changed. Called for by CAudioMessageProgressDlg
         */
   		void UpdateNaviVol( TInt aCurrentVolumeValue ) ;
   		TInt GetVolume();
        /**
         * From MAmsVolumeObserver.
         *
         * Audio routing change. Called for by CAudioMessageProgressDlg
         */
   		void SetSpeakerOutput( TBool aEarpiece );

        /**
         * Saves, if needed and exits
         */        
        void ExitAndSaveL( );

        /**
         * Wrapper on the FetchFileL function of Base Editor.
         * Called by "Insert" functions.
         */
		TBool FetchFileL( TFileName &aFileName );
   		
        /**
         * Fetchs voice file handle from msgstore. 
         * Gets duration from CAudioMessageRecorder 
         * and puts it in document object.
         */
		void FetchAudioFileDataL(  ) ;

        /**
         * Callback for CPeriodic
         */        
		static TInt DelayedExit( TAny* aThis );

        /**
    	 * Tries to exit the application. Called for example by CIdle
     	 */
    	void DoExternalExit( );
        
        /**
        * Callback for MProgressDialogCallback
        */        
        void DialogDismissedL( TInt /*dismissed*/ );
 		
 		// Functions from base classes    		
    public:
    
        /**
         * From MMsgEditorObserver. Calls DoEditorObserverL()
         *
         * For passing events from Base Editor to AudioMessage
         */
        void EditorObserver(
            TMsgEditorObserverFunc aFunc,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 );
        void DoEditorObserverL(
            TMsgEditorObserverFunc aFunc,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 );

        /**
         * From MMGFetchVerifier
         */
        TBool VerifySelectionL( const MDesCArray* aSelectedFiles );

        /**
        * From MAknNaviDecoratorObserver
        * Handles the events coming from arrow presses
        *  related to next/previous message
        */
        void HandleNaviDecoratorEventL( TInt aEventID );
        
        /**
        * From MEikEdwinObserver
        */
    	void HandleEdwinEventL( CEikEdwin* aEdwin, TEdwinEvent aEventType );
	    
	     /**
        * From MMessageIteratorObserver
        * @param See documentation of MMessageIteratorObserver
        */
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
        void HandleIteratorEventL( TMessageIteratorEvent aEvent );
#endif   

    private:
        
        /**
         *     Notification from central repository
         */
       	void HandleNotifyInt( TUint32 aId, TInt aNewValue );

        void DoUpdateSoftKeysL( TInt aSk );
        void UpdateSoftKeysL(  );
        /**
        * Sets automatic highlighting on/off.
        * @param aEnable if ETrue sets automatic highlighting on,
        *                if EFalse sets automatic highlighting off
        */
        void SetFindMode( TBool aEnable );
	      
	    /**
         * From MMsgEditorLauncher
         */
        void LaunchViewL();

	    /**
	     * Route key events to view.
	     * @param aKeyEvent key event
	     * @param aType     event type
	     * @return          response
	     * from CCoeAppUi
	     */
        TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * From CEikAppUi 
        */
        void HandleCommandL( TInt aCommand );

	    /**
	     * Inits the main menu options
	     * @param aMenuId
	     * @parem aMenuPane
	     */
        void DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane );

	    /**
	     * From MAmsOperationObserver
	     * Calls DoEditorObserverL()
	     */
        void EditorOperationEvent(
            TAmsOperationType aOperation,
            TAmsOperationEvent aEvent );
	    /**
	     * Checks the result of the event and shows possible error note.
	     * Calls right function to finalize operation.
	     */
        void DoEditorOperationEventL(
            TAmsOperationType aOperation,
            TAmsOperationEvent aEvent );
            
	    /**
	     * Called when insert operation finishes. Removes wait note.
	     * Updates navipane, fetch clip duration and so on.
	     * Focuses the right object.
	     */
        void DoInsertCompleteL( TBool aErrorNone );

	    /**
	     * Called when launch operation finishes. Removes wait note.
	     * Updates navipane, voice clip icon and duration of the clip.
	     * Focuses the right object.
	     */
        void DoLaunchCompleteL( TAmsOperationEvent aEvent );
	    /**
	     * Called when save operation finishes. 
	     */
        void DoSaveCompleteL();
	    
	    /**
	     * Called after DoSaveComplete or DialogDismissed 
	     */
	    void DoOpComplete2ndPhaseL();
	    
	    /**
	     * Called when send operation finishes. After that application exit
	     */
        void DoSendCompleteL();
	    /**
	     * Called when remove operation finishes. Removes wait note, clean
	     * duration and change icon for clipstatus
	     */
        void DoRemoveCompleteL();
        
	    /**
		 * Reports errors that appears during insertion to appui
	     */
        void ShowInsertErrorsL();

    private:

        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL(); 

    private: // New functions
          
        /**
         * Check if message has any data i.e. either in header ("To"),
         * in message body or as attachments.
         *
         * @return ETrue, message empty,
         *         EFalse, message not empty
         */        
        TBool IsMessageEmpty() const;
   
        /**
         * Updates the address size to CAudioMessageDocument
         */
        void SetAddressSize();
        
        /**
         * Updates the address size to CAudioMessageDocument
         */
        void SetBodySize();

        /**
         * Checks message content and if ok send message
         */        
        void DoUserSendL();
 
        /**
         * Handles message saving after exit 
         */        
        void DoMsgSaveExitL();

        /**
         * Handles delayed (external) exit 
         */        
        void DoDelayedExitL( TInt aDelayTime );

        /**
         * Handles message saving after user has pressed close.
         */        
        void DoBackSaveL();

        /**
         * Does actual saving of message to DRAFT folder.
         */        
        void DoSaveL();

        /**
         * Handles user message info command. 
         */        
        void DoUserMessageInfoL();

        /**
         * Handles user sending settings command. 
         */        
        //void DoUserSendingOptionsL();
 
        /**
         * Handles audio clip play command
         * @param none 
         */ 
        void PlayAudioL();       
        
        /**
         * Handles Delete message commands using by 
         * object CAudioMessaegeInsertOperation.
         * @param none 
         */ 
        void DeleteMessageL();
        
        void DoCallToSenderL();
 
        /**
         * Creates a media info object from given file and
         * performs a preliminary check whether it is ok to 
         * insert the given media file.
         *
         * The media info object is created to "iInsertingMedia"
         * member variable.
         *
         * @param aFileName  Media file to be checked
         * @return ETrue if media info object was created
         *         EFalse otherwise
         */        
        TBool CreateMediaInfoForInsertL( const TDesC& aFileName );

        /**
         * Checks whether the media object in "iInsertingMedia"
         * member variable is SuperDistributable or not. If yes
         * and Full OMA DRM is supported gives "Send without 
         * rights query".
         *
         * Deletes the "iInsertingMedia" variable if query is
         * not accepted.
         *
         * @return ETrue, if it is ok to insert the media
         *         EFalse, otherwise (iInsertingMedia deleted!)
         */
        TBool SuperDistributionCheckForInsertL();

        /**
         * Draws navipane at launch time for viewer mode.
         */
        void InitViewerNaviPaneL();

        /**
         * Draws navipane at launch time for editor mode.
         */
        void InitEditorNaviPaneL();
  
        /**
         * Auxiliary function. Shows a infomation note using notewrappers. 
         *
         * @param aResourceID
         * @param aWaiting, is note waiting or not.
         */  
        void ShowInformationNoteL(TInt aResourceID, TBool aWaiting );

        /**
         * Auxiliary function. Shows an eror note using notewrappers. 
         *
         * @param aResourceID
         * @param aWaiting, is note waiting or not.
         */  
        void ShowErrorNoteL(TInt aResourceID, TBool aWaiting );

        /**
         * Auxiliary function. 
         * Shows a confirmation query.
         *
         * @param aResourceID
         * @return the return value returned by CAknQueryDialog::ExecuteLD
         */  
        TInt ShowConfirmationQueryL( TInt aResourceID );

  
        /**
         * Shows wait note using CAknWaitDialog.
         *
         * @param aResourceID
         */
        TBool ShowWaitNoteL( TInt aResourceId );
        
        /**
         * Remove wait note from screen.
         */
        void RemoveWaitNote();

        /** 
         * Handles text addition to model when needed, updates size
         * when addreses or bodytext edited.
         */        
        void HandleCharInputL();

   
        /**
         * Check if message is forwarded.
         *
         * @return ETrue, if is forward.
         */
        TBool IsForward() const;

        /**
         * Message size according to TS 23.140 v5.5.0
         *
         * @return size in bytes
         */
        TInt MessageSizeInBytes();

        /**
         * Handles selection key press.
         */
        void DoSelectionKeyL();

        /**
         *
         */
        void FinalizeLaunchL();
        
        /**
         *  
         */
        TBool ShowLaunchErrorsL( TInt aError );

        /**
         * Sets an error resource id to be handled later
         * (usually after LaunchViewL).
         *
         * @param aStoreId   IN/OUT  The correct id is stored here
         * @param aNewId     IN      Error id
         */
        void SetErrorResource( TInt& aStoreId, TInt aNewId );
        
 
        /**
         * Determines error "priority"
         */
        TInt ErrorPriority( TInt aErrorId );


        /**
         * Checks if phone is in offline mode or not
         *
         * @return ETrue if phone is in offline mode
         *         Otherwise EFalse.
         */
        TBool IsPhoneOfflineL() const;

        /**
         * Checks whether there's enough disk space
         * to save the message.
         *
         * @return ETrue if there is enough disk space.
         *         Otherwise EFalse.
         */
        TBool CanSaveMessageL() const;
        
        /**
         * Handles user add recipient command. 
         */        
        void DoUserAddRecipientL();
        
        /**
         * Set sender to "from" -field in viewer.
         */        
        void SetSenderL();

        /**
         * Set sender to "subject" -field in viewer.
         */        
        void SetSubjectL();
        
        /**
         * Set recipient(s) to "to" -field in viewer.
         */        
        void InsertRecipientL();
        
        /**
         * Implements reply commands.
         * Launches AMS/SMS/MMS Editor by utilising SendUI.
         * @param aCase EReplyViaAMS, EReplyViaSMS or EReplyViaMMS
         */
        void DoReplyViaL( TInt aCase );

        void DoDoReplyViaMessageL( );


        /**
         * Implements forward command.
         * Launches MMS Editor for replying/forwarding.
         */
        void DoForwardL();

        /** 
         * Launches help application
         */
        void LaunchHelpL();

        /** 
         * Gets help context
         */
        CArrayFix<TCoeHelpContext>* HelpContextL() const;          

        /**
         * Converts number-strings to/from arabic-indic <-> western 
         * NOTE : Leaves string to CleanupStack
         * @param aOrigNumber contains the number to be converted
         * @param aDirection EFalse -> western, ETrue -> arabic-indic if needed
         * @return HBufC* buffer contains converted string
         */
        HBufC* NumberConversionLC( const TDesC& aOrigNumber, TBool aDirection );

       	/**
         * Calls mms mtm ui to launch delivery status dialog after user has choosed
         * EMmsViewerDeliveryStatus from the options menu.
         */
        void OpenDeliveryPopupL( );
        
        /**
         * Implements the move command.
         * Shows a folder selection dialog
         * (CMsgFolderSelectionDialog), moves the message
         * entry into selected folder and exits the viewer.
         */
        void DoMoveMessageL();
        
        /**
         * Returns the control id of the currently focused control.
         * @return Control id of the focused control or
         *         EMsgComponentIdNull if no control focused
         */
        TInt FocusedControlId();

        /**
         *  Active scheduler start for saving
         */
        void BeginActiveWait();

        /**
         *  Active scheduler stop for saving
         */
        void EndActiveWait();
  
        /**
         * Creates tabgroup, puts inbox arrows and possible 
         * priority flags
         * @param aText string in the navipane
         *        
         */       
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT        
        void CreateTabGroupL( const TDesC& aText );   
#endif   //!RD_MSG_NAVIPANE_IMPROVEMENT 
        /**
         * Checks silent mode.
         * @return ETrue if handset is silent.
         */
        TBool IsSilentModeL();

        /**
         * Updates navipane content.
         */
  		void UpdateViewerNaviFieldL( );
  		
  		/**
         * Updates navipane content.
         * @param EAmsOnlyLength=0,EAmsTextEditorAndLength,
	     *        EAmsMsgArrows
         */
  		void UpdateEditorNaviFieldL( TAmsNaviType aNaviType);


        /** 
         * Makes to- field check
         * @param aModified
         * @return ETrue if to-fiels is valid, EFalse otherwise
         */        
	   	TBool CheckNamesL( TBool& aModified );
	   
	   	/**
         * Shows address info (error) note
         *
         * @param aAddress
         * @param aAlias
         */
        void ShowAddressInfoNoteL( const TDesC& aAddress, const TDesC& aAlias );
        
                /**
        * Handles resource change events.
        */
        void HandleResourceChangeL(TInt aType);
        
        void SetTitleIconL();
        void SetTitleIconSize();
  	protected:
	    /**
	     * Deletes current entry and then exits.
	     */
	    void DeleteAndExitL();
	
	    /**
	     * Deletes current entry.
	     */
	    void DeleteCurrentEntryL();
	    
	    /**
	    * Verifies addresses in address control.
	     *
	     * @return ETrue, if verify successful,
	     *         EFalse otherwise
	     */        
	    TBool VerifyAddressesL( TBool& aModified );

    private:
         
        /**
	     * real command handling
	     */ 
        void DoHandleCommandL(TInt aCommand);
        
    	/**
         * Handles user insert audio command. Creates atta and draws audio 
         * (indicator) to display.
         *
         * @param aStatus, 	EAudioInsert =0,EAudioInserted	
         */        
    	void SetAudioClipIconL( TAmsRecordIconStatus aStatus );

	    /**
	     * Saves messages audioclip to user selected folder
	     */
		void SaveAudioClipL();

	    /**
	     * Call FetchFileL with right parameter
         * @param aNew, ETrue for recording a new audio clip,
         *              EFalse for fetching an existing one.
	     */        
	    void InsertAudioL( TBool aNew );
        void DoInsertAudioL( TFileName &aCmdFileName );
        
        /** 
         * Accessor
         * Returns To field control 
         * @return Pointer to To control or
         *         NULL if To control does not exist
         */
        CMsgAddressControl* ToCtrl() const;
         
        /** 
         * Get pointer to From ctrl instance
         *
         * @return Pointer to from control or
         *         NULL if from control does not exist
         */
        CMsgAddressControl* FromCtrl() const; 
		
		void SetNaviPaneL();
		
		void DoEditAmsPriorityL( );
		
        CAknIconArray* RadioButtonArrayL( );
        
        void UpdateIndicatorIcons( TMmsMessagePriority aPriority );
        
        TBool UnsupportedCallTypeOngoingL( /*TBool aShowNote*/ );
        /**
        * This method is called if video or VOIP call is ongoing and
        * information note needs to be shown
        * @param aResourceId
        */
        static void ShowUnsupportedCallTypeNoteL( TInt aResourceId );
		TBool CheckIncomingCall(  );
		
		TBool DoEnterKeyL();
		// Inline functions:
 
        /** 
         * Get pointer to Subject ctrl instance
         *
         * @return Pointer to subject control or
         *         NULL if subject control does not exist
         */
        inline CMsgExpandableControl* SubjectCtrl() const;
   
        /** 
         * Get pointer to body control
         *
         * @return Pointer to body control or
         *         NULL if body control does not exist
         */
        inline CMsgBodyControl* BodyCtrl() const;
        
        /** 
         * Get pointer to document instance
         *
         * @return Pointer to AudioMessageDocument
         */
        inline CAudioMessageDocument* Document() const;

        /**
         * Check if message header is empty
         *
         * @return ETrue, to field is empty,
         *         EFalse, to field has text
         */        
        inline TBool IsHeaderEmpty() const;

        /**
         * Check if message body is empty
         *
         * @return ETrue, message body empty,
         *         EFalse, message body not empty
         */        
        inline TBool IsBodyEmpty() const;

        /**
         * Get reference to file session
         *
         * @return Reference to file session
         */        
        inline RFs& FsSession() const;

        /**
         * Get senders type
         *
         * @return ESenderTypePhoneNumber, ESenderTypeMailAddress,
         *          ESenderTypeNone
		 */
		inline TAmsSenderType SenderType();
        
        /**
        * Sends read report, if message if new and launch was complete.
        */
        void SendReadReportL( TAmsOperationEvent aEvent );
        
        /*
        *  Handles zoom change
        */
        void DoHandleLocalZoomChangeL( TInt aCommand );
        
#ifdef RD_SCALABLE_UI_V2
        /**
         * From MAknToolbarObserver
         * Should be used to set the properties of some toolbar components 
         * before it is drawn.
         * @param aResourceId The resource ID for particular toolbar
         * @param aToolbar The toolbar object pointer
         */
        virtual void DynInitToolbarL(   TInt            aResourceId, 
                                        CAknToolbar*    aToolbar );
        
        /**
         * From MAknToolbarObserver
         * Handles toolbar events for a certain toolbar item.
         * @param aCommand The command ID of some toolbar item.
         */
        virtual void OfferToolbarEventL( TInt aCommand );
#endif

#ifdef RD_SCALABLE_UI_V2
        /**
        * Sets tool bar item visibilities
        * @return TBool
        */        
        void SetViewerToolBarItemVisibilities();
        
         /**
        * Sets tool bar item visibilities
        * @return TBool
        */        
        void SetEditorToolBarItemVisibilities();
        
        /**
        * Returns ETrue if the message is self composed and
        * EFalse otherwise (a received message).
        * @return ETrue if message is self composed,
        *         EFalse otherwise (received)
        */  
        TBool IsOwnMessage( ) const;
        
        TBool HasSender( ) const;
        
        void SetFixedToolbarL( const TInt aResId );
        
#endif
       
        /**
         * Check if find item highlight needs to be enabled on key event.
         * 
         * @param aKeyEvent key event
         * @param aType     event type
         * @return ETrue if highlight was enabled,
         *         EFalse otherwise
         */  
        TBool CheckFindItemHighlightL( const TKeyEvent& aKeyEvent,
                                       TEventCode aType );

    private:     // Data
   		/**
   		* For absorbing keypresses
   		* Own.  
   		*/
        CAknInputBlock* iAbsorber;
        
        /**
   		* For delayed exit
   		* Own.  
		*/
		CPeriodic* iIdle;
        CMmsClientMtm* iMtm;  
        CAknWaitDialog* iWaitDialog;
        TInt iWaitResId;
   	  	/**
     	* Titlepane
     	* Own.  
		*/
        CAknTitlePane* iTitlePane;
        CRepository* iSettingsRepository;
        CRepository* iAudioMesseageRepository;
        TUint32 iEditorFlags;
        TInt iSupportedFeatures;
        
        /**
   		* For inserting voice
   		* Own.  
		*/
        CAudioMessageInsertOperation* iInsertOperation;

        /**
     	* For launching
     	* Own.  
		*/
        CAudioMessageLaunchOperation* iLaunchOperation;

        /**
     	* For saving message
     	* Own.  
		*/
        CAudioMessageSaveOperation* iSaveOperation;

        /**
     	* For sending message
     	* Own.  
		*/
        CAudioMessageSendOperation* iSendOperation;

        CMsgMediaInfo* iInsertingMedia;

        CFindItemMenu* iFindItemMenu;

        CActiveSchedulerWait iWait;
       	CAudioMessageProgressDialog* iProgressDlg;

        CUniAddressHandler* iAddressHandler;
        CAknLocalScreenClearer*    iScreenClearer;
        
        TAmsSenderType iSenderType;
        HBufC* iAlias;
        CAknNavigationControlContainer* iNaviPane;
		TInt iNaviFlags;
 
		CAknIndicatorContainer* iNavi_length;
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT        
        CAknNavigationDecorator* iNavi_next_prev_msg_arrows; 
#endif        
        CAknNavigationDecorator* iIhfVolumeControl;
        CAknNavigationDecorator* iHeadsetVolumeControl;
        CAknNavigationDecorator* iActiveVolumeControl;		
		TInt iCurrentVolume;
        CCenRepNotifyHandler* iNotifyHandler;
        CRepository* iCUiRepository;
        TInt iMyWgId; // Window group id for AudioMessage
        TInt iFSWindowGroupId;// Window group id for FastSwapWindow
        TInt iAknNfySrvUi;
        TInt iSysWgId;
        TInt32 iVoiceKeyHandle; // Voice key capture handle, must be released
		TBool iSpeakerEarpiece;
		TBool iInserting;
        CMsgVoIPExtension* iMsgVoIPExtension;      
      	HBufC* iTimeDuratBase;
      	HBufC* iClipFileName;
        /// Sends read report
        CAudioMessageSendReadReportOperation* iSendReadReportOperation;
        TInt iCurrentSkResId;
        
        CGulIcon* iAppIcon;
        
#ifdef RD_SCALABLE_UI_V2
        CAknToolbar*                iToolbar;
#endif
    };
    
#include "audiomessageappui.inl"

#endif      //  _AUDIOMESSAGEAPPUI_H

