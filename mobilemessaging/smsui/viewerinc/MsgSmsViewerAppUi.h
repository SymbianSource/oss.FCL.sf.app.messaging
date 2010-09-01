/*
* Copyright (c) 2002 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*      Sms Viewer
*
*/



#ifndef MSGSMSVIEWERAPPUI_H
#define MSGSMSVIEWERAPPUI_H

//  INCLUDES
#include <MsgEditorAppUi.h>             // CMsgEditorAppUi
#include <msgbiocontrolObserver.h>
#include <RPbkViewResourceFile.h>
#include <PhCltTypes.h>                 // PhCltTypes
#include "MsgSmsViewerPan.h"            // panic-codes
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>

#include <AknNaviDecoratorObserver.h>

#ifdef RD_SCALABLE_UI_V2
#include <akntoolbarobserver.h>
#endif
// FUNCTION PROTOTYPES
GLREF_C void Panic(TMsgSmsPanic aPanic); 

// FORWARD DECLARATIONS
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class CSmsSettings;
class CFindItemMenu;
class CAiwServiceHandler;
class CAiwGenericParamList;
class CGmsWrapper;
class CMsgBioBodyControl;
class CEikRichTextEditor;
class CSmsHeader;
class CMsgVoIPExtension;
class CSendUi;
class CMsgExpandableControl;
class CAknLocalScreenClearer;
#ifdef RD_MSG_FAST_PREV_NEXT
class CMessageIterator;
#endif
#ifdef RD_SCALABLE_UI_V2
class CAknToolbar;
#endif

// CLASS DECLARATION

/**
*  CMsgSmsViewerAppUi
*  Inherited from CMsgEditorAppUi
*/
class CMsgSmsViewerAppUi : public CMsgEditorAppUi,
                           public MMsgBioControlObserver,
                           public MMsvSingleOpWatcher,
						   #ifdef RD_SCALABLE_UI_V2
						     public MAknToolbarObserver,             //toolbar
						   #endif
                           public MCenRepNotifyHandlerCallback,
                           public MAknNaviDecoratorObserver    
   
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMsgSmsViewerAppUi();

        /**
        * Destructor.
        */
        virtual ~CMsgSmsViewerAppUi();

#ifdef RD_MSG_FAST_PREV_NEXT
        /**
        * Change message viewed to a new one
        */
        void EntryChangedL();
#endif // RD_MSG_FAST_PREV_NEXT
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
    protected: // New functions
  
        /**
        * Check if text body part is focused
        * @return TBool about is body focused
        */
        virtual TBool IsBodyFocused() const;        
        
          /**
        * Check if sender part is focused
        * @return TBool about is body focused
        */
        virtual TBool IsSenderFocused() const;          
        
        /**
        * Confirm and delete message
        */
        virtual void DoFileDeleteL();
        
        /**
        * Return a pointer to document-object
        * @return CMsgSmsEditorDocument* a pointer to CMsgSmsViewerDocument-object
        */        
        virtual CMsgSmsViewerDocument* Document() const;
        
         /**
         * Adds bio-specific menu-items, if needed. Called from DynInitMenuPane.
         * @param aMenupane A pointer to the menu pane
         */
        virtual void AddBioCommandsToMenuL(CEikMenuPane* aMenuPane);

         /**
         * Launches dialog and moves entry from folder to folder.
         */      
        virtual void DoMoveEntryL();

         /**
         * Searches text for selected item
         * @param aCommand transfers the command to find item dialog
         */
        virtual void DoSearchL(TInt aCommand);

         /**
         * Activates phoneclient and starts calling to sender
         */
        virtual void DoCallBackToSenderL();
     
          /**
         * For creating contacts to phonebook.
         * @param aCommand a menu command id
         */
        virtual void DoCreateContactCardL( TInt aCommand );

        /**
        * Launch msg info dialog
        */
        virtual void DoMessageInfoL();

        /**
        * Separate the LaunchBioViewL from LaunchViewL to have own function to BIO-launch 
        * @param aTMsvEntry uid of the TMsvEntry in question
        */
        virtual void LaunchBioViewL( const TMsvEntry& aTMsvEntry );
        
        /**
        * Separate the SmsBasedMsgHandlingL from LaunchViewL to have own function for Sms based msg handling. 
        * @param BioType of the TMsvEntry in question
        * @param aStore current message entry's msg store
        */
        virtual void SmsBasedMsgHandlingL( TInt32 aBioType, CMsvStore* aStore );

        /**
         * Returns options menu permission of bio control.
         * @return permissions (the flags are in MMsgBioControl.h)
         */
        virtual TUint32 OptionsMenuPermissionsBioL();

        /**
        * Returns a pointer to current textbody
        * @param aView pointer to CMsgEditorView
        * @return reference to CEikRichTextEditor
        */
        virtual CEikRichTextEditor& BodyEditor() const;

        /**
        * Returns a pointer to current biobody
        * @param aView pointer to CMsgEditorView
        * @return reference to CMsgBioBodyControl
        */
        virtual CMsgBioBodyControl& BioBody() const;

        /**
        * The non-bio dynamic handling of opening options-menu
        * @param aMenuPane pointer to CEikMenuPane
        */
        virtual void DynInitNonBioOptionsMenuL( CEikMenuPane* aMenuPane );

        /**
        * The non-bio dynamic handling of opening context-menu
        * @param aMenuPane pointer to CEikMenuPane
        */
        virtual void DynInitNonBioContextMenuL( CEikMenuPane* aMenuPane );

        /**
        * The bio message dynamic option menu opening.
        * @param aMenuId menu resource ID (different between GSM and CDMA)
        * @param aMenuPane pointer to CEikMenuPane
        */
        virtual void DynInitBioOptionsMenuL(
            TInt aMenuId,
            CEikMenuPane* aMenuPane );

        /**
        * The bio message dynamic context menu opening.
        * @param aMenuPane pointer to CEikMenuPane
        */
        virtual void DynInitBioContextMenuL( CEikMenuPane* aMenuPane );

        /**
        * Forwarding of GMS message.
        * @param aTarget
        */
        virtual void ForwardGmsL(TMsvId aTarget);

        /**
        * Replying to a GMS message.
        * @param aTarget
        */
        virtual void ReplyGmsL(TMsvId aTarget);

        /**
        * Actual forwarding or replying to GMS message.
        * @param aTarget
        * aForward ETrue if forwarding, and EFalse if replying.
        */
        virtual void DoFwdRepGmsL(TMsvId aTarget, TBool aForward);

        /**
        * For saving class 0 entry (actually making it visible)
        * @param aShowNotes for showing / not showing notes during save
        */
        virtual void SaveClass0SmsL( TBool aShowNotes );

        /**
        * Approximates the needed disksize for forwards or replies the current sms.
        * aForward ETrue if forwarding, and EFalse if replying.
        * @return TInt approximation
        */
        virtual TInt ApproximateReplyForwardSize( TBool aForward );
       
        /**
        * Checks diskspace and forwards or replies the current sms.
        * aForward ETrue if forwarding, and EFalse if replying.
        */
        virtual void CheckDiskAndReplyForwardL( TBool aForward );

        /**
        * Launches help application
        */
        virtual void LaunchHelpL();

        /**
        * Returns a help context array.
        * @param aHelpContext 
        * @return CArrayFix<TCoeHelpContext>*
        */
        virtual CArrayFix<TCoeHelpContext>* CreateHelpContextArrayL(const TCoeHelpContext& aHelpContext ) const;

        /**
        * Handles SEND-key
        */
        virtual void DoHandleSendKeyL();

        /**
        * Set the state of automatic highlighting
        * @param aSwitchON, ETrue if automatic highlighting is to used, otherwise EFalse
        */
        virtual void SetAutomaticHighlightL( const TBool aSwitchON );

        /**
        * Reads the shared data value used to initialize automatic highlighting
        * @return TBool, ETrue if automatic highlighting is to be used
        */
        virtual void ReadAutoHlSharedDataValueAndSetNotifyL();

        /** 
        * From MMsvSingleOpWatcher
        * @since 2.6
        */
        virtual void OpCompleted(
            CMsvSingleOpWatcher& aOpWatcher,
            TInt aCompletionCode );

        /**
        * Called by OpCompleted.
        * @since 2.6
        * @param aOperation
        * @param aCompletionCode
        */
        virtual void DoOperationCompletedL(
            CMsvOperation* aOperation,
            TInt aCompletionCode );

        /**
        * Adds the subject field to view
        * @since 2.6
        */
        void AddSubjectControlL();

        /**
        * Sets the content of subject field
        * @since 2.6
        * @param aText, text content to be inserted
        */
        void SetSubjectL( const TDesC& aText );

        /**
        * Returns the pointer to the subject control
        * @since 2.6
        * @return CMsgExpandableControl, pointer to the subject control
        */
        CMsgExpandableControl* SubjectControl() const;

        /**
        * Initializes AIW contact card create menu item
        * @since 3.1U
        * @param aMenuId menu resource ID
        * @param aMenuPane pointer to CEikMenuPane
        */
        void InitAiwContactCardMenuL( TInt aMenuId, CEikMenuPane* aMenuPane );

        /**
        * Initializes AIW contact card sub-menu
        * @since 3.1U
        * @param aMenuPane pointer to CEikMenuPane
        */
        void InitAiwContactCardSubMenuL( CEikMenuPane* aMenuPane );

        /**
        * Initializes AIW create contact card command parameter list
        * @since 3.1U
        * @param aParamList parameter list to initialize
        */
        void InitAiwContactCardParamL( CAiwGenericParamList& aParamList ) const;

        
    protected: // Functions from base classes
        
        /**
        * From MsgEditorAppUi 
        */
        virtual void DoMsgSaveExitL();
            
         /**
        * From MMsgEditorLauncher 
        */
        virtual void LaunchViewL();

        /**
        * From CCoeAppUi 
        * @param aKeyEvent look from CCoeAppUi
        * @param aType look from CCoeAppUi
        * @return TKeyResponse look from CCoeAppUi
        */
        virtual TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

        /**
        * From CEikAppUi 
        * @param aCommand look from CEikAppUi
        */
        virtual void HandleCommandL(TInt aCommand);

        /**
        * From MEikMenuObserver 
        * @param aMenuId look from MEikMenuObserver
        * @param aMenuPane look from MEikMenuObserver
        */
        virtual void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);

        /**
         * From MMsgBioControlObserver
         * @return An index number that can be used as a starting index for Bio
         *         Control specific commands.
         */
        virtual TInt FirstFreeCommand() const; 

        /**
         * From MMsgBioControlObserver
         * The Bio Control can request the editor application to carry out
         * specific commands.
         * @param aCommand The command to be handled.
         * @return KerrNone or KerrNotSupported
         */
        virtual TInt RequestHandleCommandL(TMsgCommands aCommand); 

        /**
         * From  MMsgBioControlObserver
         * Returns ETrue or EFalse whether a requested aCommand is supported
         * by the BIO message editor or not. This command should be used
         * before requesting command handling by RequestHandleCommand().
         * @param aCommand The command
         * @return ETrue if command is support and EFalse if not.
         */
        virtual TBool IsCommandSupported(TMsgCommands aCommand) const; 

        /**
         * From CMsgEditorAppUi
         * This handles the case if sms entry currently viewed is suddenly deleted from MSGS
         */
        virtual void HandleEntryDeletedL();

        /**
        * From CCoeAppUi
        * Returns help context
        */
        virtual CArrayFix<TCoeHelpContext>* HelpContextL() const;  

        /**
        * From MCenRepNotifyHandlerCallback
        * Handles the incoming notifications of key changes
        * @since Series60 3.0
        * @param aId, Key that has changed
        * @param aNewValue, New value of the key    
        */
        void HandleNotifyInt( TUint32 aId, TInt aNewValue );

        /**
        * From MCenRepNotifyHandlerCallback
        * Handles the notifier errors
        * @since Series60 3.0
        * @param aId, Key that has changed
        * @param aNewValue, New value of the key    
        */
        void HandleNotifyError( 
            TUint32 aId, 
            TInt aError, 
            CCenRepNotifyHandler* aHandler );

        /**
        * Reply to sender via MMS or Email
        * @since Series 60 2.6
        * @param aCase, reply via MMS, email, audiomsg or unieditor
        */
        virtual void  DoReplyViaL( TInt aCase );
        
        /**
        * From MAknNaviDecoratorObserver
        * Handles the events coming from arrow presses
        *  related to next/previous message
        */
        void HandleNaviDecoratorEventL( TInt aEventID );            
        
        /**
        * Actual Reply or Forward
        * @since Series 60 5.0
        * @param aForward, reply or forward
        */
        void DoReplyFwdL( TBool aForward );
            
    protected:

        /**
        * By default Symbian OS constructor is private.
        */
        virtual void ConstructL();

    private: // new functions

        /**
        * ConstructL() without calling base class construct
        */
        void ConstructMembersL();

        /**
        * Initialize member data to a default state
        */
        void InitMembers();

        /**
        * Delete member data
        */
        void DestructMembers();
        /**
        * Set title icon
        */
        void SetTitleIconL();
        
        /**
        * Handle resource change
        */
        void HandleResourceChangeL( TInt aType );
        
        /**
        * Set title icon size
        */
        void SetTitleIconSize();
#ifdef RD_MSG_FAST_PREV_NEXT
    private: // from CMsgEditorAppUi
        /**
        * Check if message can fast opened
        */
        TBool CanFastOpenL( const CMessageIterator& aIterator );
#endif // RD_MSG_FAST_PREV_NEXT
        
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
      
#ifdef RD_SCALABLE_UI_V2

         /**
          * From MAknToolbarObserver
          * Should be used to set the properties of some toolbar components 
          * before it is drawn.
          * @param aResourceId The resource ID for particular toolbar
          * @param aToolbar The toolbar object pointer
          */
         //virtual void DynInitToolbarL(   TInt            aResourceId, 
         //                                CAknToolbar*    aToolbar );
         
         /**
          * From MAknToolbarObserver
          * Handles toolbar events for a certain toolbar item.
          * @param aCommand The command ID of some toolbar item.
          */
         virtual void OfferToolbarEventL( TInt aCommand );
#endif
         /**
          * This is quick launch based on already existing view. Hence this function should  
          * be called only for Proper SMS Text message for fast and optimized navigation.
          * In case called for any bio-type messages, this will leave with KErrCancel so that 
          * caller will TRAP the leave and call LaunchViewL() for complete launch.
          * @param None
          */
         void QuicklaunchViewL();
         
         /**
          * This will set the body text and update the control fields if any changes
          * as compared to previous SMS message. 
          * This function is called only for non-bio text messages for fast preview during navigation
          * @param aStore current message entry's msg store
          */
         void QuickSmsMsgNaviHandlingL( CMsvStore* aStore );
         
         /**
         * Checks wheter current find item is highlighted
         * @return ETrue if highlight enabled
         */
         TBool FindItemHighlighted();
         
         /**
         * Returns id of focused control
         * @return id of focused control
         */
         TInt FocusedControlId();
         
    protected:     // Data

        // Note:
        // When adding data members, add initialization into InitMembers().
        // Add construct/destruct into ConstructMembersL()/DestructMembers()
        // functions if needed. Those functions are called during
        // construction/destruction and also during fast open of prev/next
        // message.

        TBool                               iFullyConstructed;
        TPhCltTelephoneNumber	            iNumber;          // phoneclient
        TPhCltNameBuffer                    iRecipientstring; // phoneclient

        // Boolean flags combined into a bitmap. Type bool is used instead of
        // TBool as it's type safe
        struct
            {
            bool iBioMsg:1;
            bool iGms:1;   // picture msg
            bool iIrBt:1;  // infrared or bluetooth message
            bool iValidSenderNumber:1;
            bool iHelpFeatureSupported:1;
            bool iAutomaticHlInitialized:1;
            bool iVoIPNumber:1;    // VoIP supported
            bool iAudioMsgSupported:1;
            bool iSmsRestrictedMode:1;
            bool iAutomaticHlValue:1;
            bool iAutoLockEnabled:1;

            // GMS variation
            bool iSmartMsgSmsGmsMessageReceiving:1;
            bool iSmartMsgSmsGmsMessageSending:1;
            
            // Email flags
            // Email feature supported
            bool iEmailFeatureSupported:1;
            // Email specific Subject control
            // When created, it will newer be removed
            bool iEmailSubjectControlPresent:1;
            // If the message TP-PID is set to interworking with email, the
            // message  is marked as Email message
            bool iIsEmailMessage:1;
            } iFlags;

        CAknNavigationControlContainer*     iNaviPane;
        TInt                                iControlId;
        CEikButtonGroupContainer*           iClass0CBA;       // for Class0 softbuttons
        CFindItemMenu*                      iFindItemMenu;    // find item
        CAiwServiceHandler*                 iAiwServiceHandler; // for create contact card
        CGmsWrapper*                        iGmsModel;
        CSmsHeader*                         iSmsHeader;
        CMsvSingleOpWatcher*                iViewerOperation;   // MessageInfo operation
        // Contains VoIP profile counter.
        CMsgVoIPExtension*                  iMsgVoIPExtension;
        CSendUi*                            iSendUi;
        // Central Repository session
        CRepository*                        iCenRepSession;
        // Notifier to listen changes of offline state
        CCenRepNotifyHandler*               iNotifier;
        CAknLocalScreenClearer*    					iScreenClearer;
#ifdef RD_SCALABLE_UI_V2
        CAknToolbar*                iToolbar;
#endif
        // SMS Icon
        CGulIcon* iAppIcon;
        TInt                                iTypeMsg; //for saving the PDU type of prev sms

    };
#endif // MSGSMSVIEWERAPPUI_H
//  End of File
