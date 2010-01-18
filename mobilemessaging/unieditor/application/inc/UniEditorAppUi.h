/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Unified SMS and MMS Editor Application UI class definition.      
*
*/



#ifndef _UNIEDITORAPPUI_H
#define _UNIEDITORAPPUI_H

//  INCLUDES

#include <gmxmlparser.h>                // for MMDXMLParserObserver & CMDXMLParser

#include <MsgEditorAppUi.h>             // for CMsgEditorAppUi
#include <MsgEditorView.h>
#include <MsgAddressControl.h>
#include <MsgBodyControl.h>             // for CMsgBodyControl
#include <MsgEditor.hrh>
#include <msgimagecontrol.h>            // for CMsgImageControl (needed in .inl)
#include <msgasynccontrolobserver.h>

#include <MsgRecipientItem.h>           // for CMsgRecipientArray
#include <MsgAttachmentUtils.h>
#include <MMGFetchVerifier.h>
#include <akntoolbarobserver.h>
#include <AknServerApp.h>               // for MAknServerAppExitObserver
#include <AknProgressDialog.h>          // for MProgressDialogCallback
#include <peninputclient.h>
#include <peninputsrveventhandler.h>
#include <cenrepnotifyhandler.h>    // MCenRepNotifyHandlerCallback

#include <eikedwob.h>                   // for MEikEdwinObserver

#include <aknlongtapdetector.h> // for long tap
// Needed by inline functions
#include "uniobjectlist.h"
#include "unidatamodel.h"
#include "UniSendingSettings.h"

#include "UniEditorHeader.h"            // THeadersFields,  
#include "UniEditorApp.hrh"             // TUniEditorPanicIds
#include "UniEditorOperation.h"         // MUniEditorOperatorObserver

// CONSTANTS

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class TFilename;
class TChar;
class CAknTitlePane;
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class CAknWaitDialog;
class CUniEditorDocument;
class CAknLocalScreenClearer;
class CRepository;
class CSelectionItemList;
class CEikImage; // for context icon
class CAknInfoPopupNoteController;
class CDocumentHandler;
class CAknIconArray;
class CAknInputBlock;
class CMDXMLDocument;
class CAknToolbar;

class CMsgMediaInfo;
class CMsgImageControl;
class TMsgControlsIds;

class CUniClientMtm;
class CUniSmilModel;
class CUniSlideLoader;
class CUniEditorLaunchOperation;
class CUniEditorSaveOperation;
class CUniEditorSendOperation;
class CUniEditorChangeSlideOperation;
class CUniEditorInsertOperation;
class CUniEditorVCardOperation;
class CUniEditorHeader;
class CAknToolbarExtension;
class CAknStylusPopUpMenu;

// CLASS DECLARATION

/**
* CUniEditorAppUi
*
* @since 3.2
*/
class CUniEditorAppUi : 
    public CMsgEditorAppUi,
    public MMGFetchVerifier,
    public MMsgAsyncControlObserver,
    public MUniEditorOperationObserver,
    public MAknToolbarObserver,
    public MMDXMLParserObserver,
    public MAknServerAppExitObserver,
    public MEikEdwinObserver,
    public MProgressDialogCallback,
    public MPenUiActivationHandler,
    public MCenRepNotifyHandlerCallback,
    public MAknLongTapDetectorCallBack
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CUniEditorAppUi();

        /**
        * Destructor.
        */
        virtual ~CUniEditorAppUi();

        /**
        * Removes object from screen.  (Objectsview callback method)
        * @param aRegion, region to remove the object from
        * @param aLayout, layout before removed from model
        */
        void RemoveObjectL( TUniRegion aRegion, TUniLayout aLayout );

        /**
        * Changes object order on screen. (Objectsview callback method)
        * @param aLayout, new layout
        */
        void ChangeOrderL( TUniLayout aLayout );

        /**
        * If there is image control, sets image release/lock
        * according to parameter. Otherwise does nothing.
        * @param aRelease, ETrue = release/unlock, EFalse = lock
        */
        void ReleaseImage( TBool aRelease );

        /**
        * Saves, if needed and exits
        */        
        void ExitAndSaveL();
        
        /**
        * Wrapper on the FetchFileL function of Base Editor.
        * Called by "Insert" functions.
        *
        * @param aFetchType, EImage or ENewImage or
        *                    EAudio or ENewAudio or
        *                    EVideo or ENewVideo
        */
        TBool FetchFileL( MsgAttachmentUtils::TMsgAttachmentFetchType aFetchType );

        /**
        * Sets focus to first focusable control on body.
        */
        void SetFocusToBodyBeginningL();
        
        /**
        * Callback for CPeriodic
        */        
        static TInt DelayedExitL( TAny* aThis );
        
        /**
        * Callback for MProgressDialogCallback
        */        
        void DialogDismissedL( TInt /*dismissed*/ );
        
        /**
        * Exit Unieditor without save during critimal memory condition
        */   
        void ExitWithoutSave();
    public: // Functions from base classes
    
        /**
        * From MMsgEditorObserver. Calls DoEditorObserverL()
        *
        * For passing events from Base Editor to MMS Editor
        */
        void EditorObserver( TMsgEditorObserverFunc aFunc,
                             TAny* aArg1,
                             TAny* aArg2,
                             TAny* aArg3 );
        
        /**
        * Handles the editor observer event.
        */
        void DoEditorObserverL( TMsgEditorObserverFunc aFunc,
                                TAny* aArg1,
                                TAny* aArg2,
                                TAny* aArg3 );

        /**
        * From MMGFetchVerifier
        */
        TBool VerifySelectionL( const MDesCArray* aSelectedFiles );

        /**
         * From MAknToolbarObserver
         */
        void DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolbar );
        
        /**
         * From MAknToolbarObserver
         */
        void OfferToolbarEventL( TInt aCommand );
    
        /**
         * From MApaServerAppExitObserver
         */
        void HandleServerAppExit( TInt aReason );
    
        /**
        * From MEikEdwinObserver
        */
        void HandleEdwinEventL( CEikEdwin* aEdwin, TEdwinEvent aEventType );
        
        /**
         * From CMsgEditorAppUi
         */
        void HandleEntryMovedL( TMsvId aOldParent, TMsvId aNewParent );
        
        /*
        * From MPenUiActivationHandler
        */
        void OnPeninputUiDeactivated();
        
        /*
        * From MPenUiActivationHandler
        */
        void OnPeninputUiActivated();       
    
    private: // Functions from base classes
            
        /**
        * From MMsgEditorLauncher
        */
        void LaunchViewL();

        /**
        * From CCoeAppUi 
        */
        TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * From CEikAppUi 
        */
        void HandleCommandL( TInt aCommand );
        
        /**
        * Performs the command.
        */
        void DoHandleCommandL( TInt aCommand );
        
        /**
         * Performs the command.
         */
        void DoOfferToolbarEventL( TInt aCommand );
        
        /**
        * From CAknAppUi.
        *
        * Handles navipane updating when launched from idle and
        * when returning from preview.
        *
        * @param aForeground, ETrue if app is foreground.
        */
        void HandleForegroundEventL(TBool aForeground);

        /**
        * From MEikMenuObserver 
        */
        void DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane );
        
        /**
        * Helper function to be called in normal unimode and upload mode.
        * Handles the menu options based on the focused media object
        */
        void DynInitFocusedMediaBasedOptionsL( CEikMenuPane* aMenuPane );
        
        /**
        * From MUniEditorOperationObserver. Called when operation 
        * state is changed.
        */
        void EditorOperationEvent( TUniEditorOperationType aOperation,
                                   TUniEditorOperationEvent aEvent );

        /**
        * Performs the real operation event handling
        */
        void DoEditorOperationEventL( TUniEditorOperationType aOperation,
                                      TUniEditorOperationEvent aEvent );

        /**
        * From MUniEditorOperationObserver. Query is wanted to be shown 
        * to the user.
        */
        TBool EditorOperationQuery( TUniEditorOperationType aOperation,
                                    TUniEditorOperationQuery aEvent );        

        
        /**
        * Shows the query to the user.
        */
        TBool DoEditorOperationQueryL( TUniEditorOperationType aOperation,
                                       TUniEditorOperationQuery aEvent );        

        /**
        * Performs the change slide operation event handling.
        */
        void DoChangeSlideCompleteL();
        
        /**
        * Performs the insert operation event handling.
        */
        void DoInsertCompleteL( TUniEditorOperationEvent aEvent );
        
        /**
        * Performs the 2nd phase of insert operation event handling.
        */
        void DoInsertComplete2ndPhaseL();
        
        /**
        * Performs launch operation event handling.
        */
        void DoLaunchCompleteL();
        
        /**
        * Performs save operation event handling.
        */
        void DoSaveCompleteL();
        
        /**
        * Performs the 2nd phase of saving operation event handling.
        */
        void DoSavingComplete2ndPhase();
        
        /**
        * Performs send operation event handling.
        */
        void DoSendCompleteL();
        
        /**
        * Performs 2nd phase of send operation event handling.
        */
        void DoSendComplete2ndPhase();
        
        /**
        * Performs vCard insert operation event handling.
        */
        void DoVCardCompleteL();
        
        /**
        * Shows insert errors
        */
        void ShowInsertErrorsL();
        
        /**
        * From MMsgAsyncControlObserver. Called when asynchronous control
        * state is changed
        */
        void MsgAsyncControlStateChanged( CMsgBaseControl& aControl,
                                          TMsgAsyncControlState aNewState,
                                          TMsgAsyncControlState aOldState );
        
        /**
        * From MMsgAsyncControlObserver. Called when asynchronous control
        * receives resource change event.
        */
        void MsgAsyncControlResourceChanged( CMsgBaseControl& aControl, TInt aType );
        
        /**
        * 2nd phase constructor.
        */
        void ConstructL(); 
    	// From MAknLongTapDetectorCallBack
        /**
         * @see MAknLongTapDetectorCallBack
         */
        void HandleLongTapEventL(
            const TPoint& aPenEventLocation, 
            const TPoint& aPenEventScreenLocation );

    private:
    
        enum TUniEditorInsertTextType
            {
            ETemplateText,
            EMemoText
            };
        
        enum TUniEditorInsertOtherType
            {
            ETextFile,
            EOtherFile
            };
            
    private: // New functions
          
        /**
        * Update the msglength information in navipane.
        * Reads the current size from document.
        *
        * @param aForceDraw, forces drawing of size label. 
        */
        void MsgLengthToNavipaneL( TBool aForceDraw = EFalse );    
        
        /**
        * Calculates the SMS Messages Characters left in the current segment and Total No. of segments in the message
        * Total No. of segments in the message
        *
        * @Return value
        * charsLeft: Characters left in the current segment
        * msgsParts: Total No of segments 
        */
        void CalculateSMSMsgLen(TInt& charsLeft, TInt& msgsParts);

        /**
        * Check if message has any data i.e. either in header ("To"),
        * in message body or as attachments.
        *
        * @return ETrue, message empty,
        *         EFalse, message not empty
        */        
        TBool IsMessageEmpty() const;

        /**
        * Draw the "not viewable" icons
        * (editable 3GPP SMIL or non-editable SMIL).
        *
        * @param aEditable, ETrue should be set if SMIL is Template SMIL
        */
        void InsertPresentationL( TBool aEditable );

        /**
        * Updates the address size to CUniEditorDocument
        */
        void SetAddressSize();

        /**
        * Updates the subject size to CUniEditorDocument
        */
        void SetSubjectSize();

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
        * @param aDelayTime in microsecods
        */        
        void DoDelayedExit( TInt aDelayTime );

        /**
        * Handles message saving after user has pressed close.
        */        
        void DoBackSaveL();

        /**
        * Does actual saving of message to DRAFT folder.
        *
        * @param aSaveType, Controls how the saving is done
        *                   e.g. keep entry hidden etc.
        */        
        void DoSaveL( TUniSaveType aSaveType ); 

        /**
        * Verifies addresses in address control.
        *
        * @return ETrue, if verify successful,
        *         EFalse otherwise
        */        
        TBool VerifyAddressesL( TBool& aModified );

        /**
        * Handles user move and select page commands. 
        */        
        void DoUserMoveOrSelectPageL( TBool aMovePage );
        
        /**
        * Handles user objects view command. 
        */        
        void DoUserObjectsViewL( TBool aFocusAttachments );

        /**
        * Handles user add recipient command. 
        */        
        void DoUserAddRecipientL();

        /**
        * Handles user message info command. 
        */        
        void DoUserMessageInfoL();

        /**
        * Handles user sending settings command. 
        */        
        void DoUserSendingOptionsL();

        /**
        * Handles user insert media command. 
        */        
        void DoUserInsertMediaL();
        
        /**
        * Handles user insert image command. Creates attachment
        * and draws image to display. 
        *
        * @param aFetchType, ENewImage or EImage 
        */        
        void DoUserInsertImageL( MsgAttachmentUtils::TMsgAttachmentFetchType aFetchType );

        /**
        * Handles user insert video command. Creates attachment
        * and draws image to display. 
        *
        * @param aFetchType, ENewVideo or EVideo
        */        
        void DoUserInsertVideoL( MsgAttachmentUtils::TMsgAttachmentFetchType aFetchType );

        /**
        * Handles user insert audio command. Creates atta and draws audio 
        * (indicator) to display.
        *
        * @param aFetchType, ENewAudio or EAudio 
        */        
        void DoUserInsertAudioL( MsgAttachmentUtils::TMsgAttachmentFetchType aFetchType );

        /**
        * Starts insert operation
        *
        * @param aAddSlide Indicates whether new slide should be added.
        * @param aAddAsAttachment Indicates that file is inserted as attachment.
        */
        void DoStartInsertL( TBool aAddSlide, TBool aAddAsAttachment );

        /**
        * Creates a media info object from given file and
        * performs a preliminary check whether it is ok to 
        * insert the given media file.
        *
        * The media info object is created to "iInsertingMedia"
        * member variable.
        *
        * @param aMediaType Media type to be checked
        * @param aFileName  Media file to be checked
        * @return ETrue if media info object was created
        *         EFalse otherwise
        */        
        TBool CreateMediaInfoForInsertL( TMsgMediaType aMediaType,
                                         const TDesC& aFileName );

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
        * Checks whether it is ok to insert the media object
        * in "iInsertingMedia" member variable.
        *
        * Deletes the "iInsertingMedia" variable if inserting
        * is not allowed.
        *
        * @return ETrue, if it is ok to insert the media
        *         EFalse, otherwise (iInsertingMedia deleted!)
        */
        TBool CheckMediaInfoForInsertL();

        /**
        * Handles user insert slide command. 
        */
        void DoUserInsertSlideL();
        
        /**
        * Handles user remove slide command. 
        */
        void DoUserRemoveSlideL();

        /**
        * Does the actual slide removal. First removes
        * All objects from the slide, then removes the
        * actual slide and finally changes the current slide
        */        
        void DoRemoveSlideL();

        /**
        * Handles user slide change command, i.e. 
        * this is called when the user scrolls out of
        * the current slide.
        * 
        * @param aSlideNum  Number of the next slide to be drawn
        */        
        void DoUserChangeSlideL( TInt aSlideNum );

        /**
        * Sets focus after slide is drawn
        */        
        void DoSetFocusL();

        /**
        * Sets text to title pane
        */        
        void SetTitleL();

        /**
        * Handles user remove image, audio of video media object
        * @param aMediaControlId media control id
        * @param aRegion region the control resides
        * @param aConfirmResource confirm query to be shown to user
        */        
        void DoUserRemoveMediaL(    TMsgControlId   aMediaControlId,
                                    TUniRegion      aRegion );

        /**
        * Handles user insert text command.
        *
        * @param aType Text type wanted to be inserted.
        */
        void DoUserInsertTextL( TUniEditorInsertTextType aType );
        
        /**
        * Handles user insert other file command.
        *
        * @param aType Other file type wanted to be inserted.
        */
        void DoUserInsertOtherFileL( TUniEditorInsertOtherType aType );
        
        /**
        * Handles user insert vCard command.        
        */
        void DoUserInsertVCardL();
        
//        /**
//        * Handles user insert SVG command.        
//        */
//        void DoUserInsertSvgL();
        
        /**
        * Draws navipane at launch time.
        */
        void InitNaviPaneL();

        /**
        * Updates following navi indicators to both avkon editor pane and navidecorator pane
        * - Priority low / high
        */
        void UpdateIndicatorIconsL();

        /**
        * Auxiliary function. Shows a infomation note using notewrappers. 
        *
        * @param aResourceID
        * @param aWaiting, is note waiting or not.
        */  
        void ShowInformationNoteL(TInt aResourceID, TBool aWaiting );

        /**
        * Auxiliary function. Shows an error note using notewrappers. 
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
        TInt ShowConfirmationQueryL( const TDesC& aText ) const;
        TInt ShowConfirmationNoQuestionQueryL( const TDesC& aText ) const;
        TInt ShowConfirmableInfoL( TInt aResourceID );
        TInt ShowConfirmableInfoL( const TDesC& aText ) const;
        TInt ShowConfirmableInfoErrorL( const TDesC& aText ) const;

        /**
        * Auxiliary function. Shows a confirmation query if it is not shown before. 
        *
        * @param aResourceID
        * @return the return value returned by CAknQueryDialog::ExecuteLD
        */  
        TInt ShowGuidedModeConfirmationQueryL( TInt aResourceID );

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
        * Checks whether text object should be added to smilmodel when
        * after characters are added/removed
        */        
        void UpdateSmilTextAttaL();

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
        * Performs launch finalization.
        */
        void FinalizeLaunchL();
        
        /**
        * Shows possible notes related to editor launching. 
        * These notes include both error notes and notes related to content & MMS creation mode.
        *
        * @aParam aShutDown  Returns ETrue    if launching can proceed
        *                            EFalse   if launching should be aborted immediatelly.
        */
        void ShowLaunchNotesL( TBool& aShutDown );

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
        * Launches help application
        */
        void LaunchHelpL();

        /** 
        * Gets help context
        */
        CArrayFix<TCoeHelpContext>* HelpContextL() const;          

        /** 
        * User selects a new template and launches smil editor
        */
        void NewTemplateL();

        /** 
        * Opens smil editor for template message opened from drafts folder
        */
        void OpenTemplateL();

        /** 
        * Called when user wants to play created presentation
        * Starts creating SMIL DOM
        */
        void PlayPresentationL();
        
        /** 
        * When the SMIL DOM creation is ready, this function is called by the callback funtion
        * to start the SMILPlayer dialog.
        */
        void DoCompletePlayPresentationL();
        
        /** 
        * Removes template from message.
        */
        void RemoveTemplateL();

        /**
        * Handles user change order command.
        */
        void DoUserChangeOrderL( TUniLayout aLayout );

        /**
        * Begins active scheduler loop
        */
        void BeginActiveWait( CUniEditorOperation* aOperation );
        
        /**
        * Stops active scheduler loop
        */
        void EndActiveWait();
        
        /**
        * Returns available objects as a bit mask
        */
        TUint32 ObjectsAvailable();
        
        /**
        * Handles resource change events.
        */
        void HandleResourceChangeL(TInt aType);
    
        /**
        * ParseFileCompleteL
        * Call back function used to inform client of Parser when SMIL dom is ready for preview
        */
        void ParseFileCompleteL();

        /** 
        * Accessor
        * Get pointer to image control
        *
        * @return Pointer to image control or
        *         NULL if image control does not exist
        */
        inline CMsgImageControl* ImageCtrl() const;

        /** 
        * Accessor
        * To check whether To field is ON CURRENT SLIDE.
        * Address handling should take place through CUniEditorHeader
        *
        * @return Pointer to image control or
        *         NULL if image control does not exist
        */
        inline CMsgAddressControl* ToCtrl() const;

        /** 
        * Accessor
        * To check whether Subject field is on ON CURRENT SLIDE
        * Subject handling should take place through CUniEditorHeader
        *
        * @return Pointer to image control or
        *         NULL if image control does not exist
        */
        inline CMsgExpandableControl* SubjectCtrl() const;
   
        /** 
        * Accessor
        * Get pointer to body control
        *
        * @return Pointer to body control or
        *         NULL if body control does not exist
        */
        inline CMsgBodyControl* BodyCtrl() const;

        /** 
        * Accessor
        * Get pointer to address control rich text editor
        *
        * @return Pointer to address control editor or
        *         NULL if address control does not exist
        */
        inline CEikRichTextEditor* RecipientCtrlEditor(CUniBaseHeader::THeaderFields aField) const;

        /** 
        * Accessor
        * Get pointer to subject control rich text editor
        *
        * @return Pointer to subject control editor or
        *         NULL if subject control does not exist
        */
        inline CEikRichTextEditor* SubjectCtrlEditor() const;

        /** 
        * Accessor
        * Get pointer to body control rich text editor
        *
        * @return Pointer to body control editor or
        *         NULL if body control does not exist
        */
        inline CEikRichTextEditor* BodyCtrlEditor() const;

        /** 
        * Accessor
        * Get pointer to document instance
        *
        * @return Pointer to UniEditorDocument
        */
        inline CUniEditorDocument* Document() const;

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
        * Accessor
        * Get reference to file session
        *
        * @return Reference to file session
        */        
        inline RFs& FsSession() const;
    
        /**
        * Implements AddHeaders command
        */
        void DoUserAddHeadersL();
        
        /**
        * Collects statistics about additional headers after user
        * has closed the multiselection list.
        * @param aIndexesOut list of (un)selected items. Values are TBool 
        * @param aVariation flags of variation
        * @param aNewSelectionFlags OUT contains headers, which user selected
        * @param aRemoveFlags OUT contains headers to be removed from UI
        * @param aRemovewarningFlags OUT contains headers to be removed from UI,
        *        which contains data
        * @param aAddFlags OUT contains headers to be added to the UI
        * 
        *        Every flag is a combination of EMmsFeatureSubject, EMmsFeatureCc
        *        and EMmsFeatureBcc of enum TMmsFeatures
        */
        void AddHeadersCollectStatisticsL( CArrayFix<TInt>& aIndexesOut, 
                                           TInt aVariation,
                                           TInt& aNewSelectionFlags,
                                           TInt& aRemoveFlags,
                                           TInt& aRemoveWarningFlags,
                                           TInt& aAddFlags) const;


        /**
        * Returns count of headers, which have flag set
        * @param aFlags combination of flags of EMmsFeatureSubject, EMmsFeatureCc
        * and EMmsFeatureBcc of enum TMmsFeatures
        * @return count of headers
        */
        TInt AddHeadersCount( TInt aFlags ) const;
        
        /**
        * Set scroll positions of editor controls to the end of
        * of the text (address controls and body text if before image)
        * or beginning of body text, if body text is after media object.
        * This function is usually called after inserting media has been successful.
        */
        void SetCursorPositionsForInsertL();

        /**
        * Add icon of priority to navi pane
        */
        void PriorityToNaviPane();
        
        /**
        * Removes text from the current page
        */
        void RemoveCurrentTextObjectL();
        
        /**
        * Checks body message type.
        */
        void CheckBodyForMessageTypeL();
        
        /**
        * Checks header message type.
        */
        void CheckHeaderForMessageTypeL();
        
        /**
        * Checks if the body content is SMS conformant
        */
        TBool IsBodySmsCompatibleL( TBool aInAutoMode = ETrue );
        
        /**
        * Set iSmsSize and EUnicodeMode
        */
        void CheckSmsSizeAndUnicodeL();
        
        /**
        * Checks if there are too many recipients and shows a note
        * @return ETrue - if recipients OK otherwise EFalse
        */
        TBool CheckMaxRecipientsAndShowNoteL( TBool aSendCheck = EFalse );
        
        /**
        * Returns SMS plugin
        */
        inline CUniEditorPlugin* SmsPlugin();
        
        /**
        * Returns MMS plugin
        */
        inline CUniEditorPlugin* MmsPlugin();        
        
        /**
        * Returns whether editor is fully launched or not.
        */
        inline TBool IsLaunched() const;
        
        /**
        * Creates a string from integer
        * If size is less than 10 kilobytes -> string with one decimal
        * If size is 0 or 10k or more -> integer string
        * 
        * @since    3.1
        * @param    aSizeInBytes    IN size to be converted to string
        *                           in bytes
        * @param    aSize   IN/OUT the size string
        * @param    aInteger    OUT the integer part of the string
        * @param    aFragment   OUT the decimal part of the string
        */
        void CreateMmsNaviSizeStringL( const TInt aSizeInBytes,
                                       TDes& aSize,
                                       TInt& aInteger,
                                       TInt& aFragment );
                
        /**
        * Handles insert other file command from toolbar.
        */
        void DoToolbarInsertOtherL();
        
        /**
        * Shows list query
        */
        TBool ShowListQueryL( TInt aResourceID, TInt& aSelectedIndex ) const;
        
        /**
        * Hides popup note
        */
        void HidePopupNote();
        
        /**
        * Shows popup note
        */
        void ShowPopupNoteL( TBool aMms );
        
        /**
        * Plays focused item
        */
        void PlayFocusedItemL();
        
        /**
        * Checks the size of the sms body and shows notes
        * If aSendCheck is ETrue, checks only if the body exceeds the max size
        */
        void CheckMaxSmsSizeAndShowNoteL( TBool aSendCheck = EFalse );
        
        /**
        * Sets or removes edwin control's maximum size 
        */
        void SetOrRemoveMaxSizeInEdwin();
        
        /**
        * Checks the focused item and updates the middle softkey accordingly
        */
        void UpdateMiddleSoftkeyL();
        
        /**
        * Sets input mode to address fields
        */
        void SetInputModeToAddressFields( TInt aInputMode );
        
        /**
        * Handles user priority change command.
        */
        void DoEditMmsPriorityL();
        
        /**
        * Creates radio button array
        */
        CAknIconArray* RadioButtonArrayL();
        
        /**
        * Confirms SMS message sending in multiple parts
        */
        TBool ConfirmSmsSendInMultiplePartsL();
        
        /**
        * Sets title icons sizes.
        */
        void SetTitleIconsSizeL();
        
        /**
        * Tells if email addresses are accepted by the message
        * For example locked sms + EmailOverSms or not-locked sms
        */
        TBool AcceptEmailAddresses();
        
        /**
        * Tells if the message is locked in sms mode
        * (either by restricted reply or temporarily in sending options)
        */
        TBool IsHardcodedSms();
        
        /**
        * Resolves control id of the focused control after the launch.
        */
        TMsgControlId ResolveLaunchFocusedControlL();
        
        /**
        * Sets correct message type locking to MTM
        */
        void SetMessageTypeLockingL();
        
        /**
        * Activates input blocker properly.
        *
        * @param aActiveObjectToCancel Pointer to an active object that is called when 
        *                              input blocker is cancelled. Can be NULL.
        */
        void ActivateInputBlockerL( CActive* aActiveObjectToCancel );
        
        /**
        * Deactivates input blocker properly.
        */
        void DeactivateInputBlocker();
        
        /**
        * Updates toolbar items status to correct one.
        */
        void UpdateToolbarL();
        
        /**
        * Check whether message is locked to SMS mode and if
        * it includes an attachment (i.e. vCard or vCal). Performs
        * actions related to changes from in and out of this state.
        */
        void CheckLockedSmsWithAttaL();
        
        /**
         * Sets fixed toolbar dimmed.
         */
        void SetFixedToolbarDimmed();
        
        /**
         * Cleanup item for setting toolbar enabled.
         */
        static void EnableFixedToolbar( TAny* aAny );
        
          /**
         * Cleanup item for Updating fixed toolbar Dimmed.
         */
         static void UpdateFixedToolbar( TAny* aAny );
        /**
         * Cleanup item for.Disabling SendKey
         */
        static void DisableSendKey( TAny* aAny );
        
        
        
        /**
         * Creates fixed toolbar.
         * 
         * @param aDimmed ETrue if all toolbar items are wanted to be set dimmed.
         *                EFalse otherwise.
         */
        void CreateFixedToolbarL( TBool aSetDimmed );
        
        /**
        * Sets extension button to dimmed/undimmed.
        * 
        * @param aExtension Toolbar extension
        * @param aButtonId Toolbar button id
        * @param aDimmed ETrue/EFalse Dimmed/Undimmed.
        */
        void SetExtensionButtonDimmed( CAknToolbarExtension* aExtension, TInt aButtonId, TBool aDimmed );
        
        /**
         * Enables toolbar.
         */
        void DoEnableFixedToolbar();
        /**
         * Update toolbar.
         */
        void DoUpdateFixedToolbar();
                
        /**
         * Handles enter key processing.
         */
        TBool DoEnterKeyL();
        
        /**
        * Verifys and if valid to show SMS Size, then sends it to the PenInput server
        */
        void SmsMsgLenToPenInputL();
        
        /**
        * Verifys whether Updated Message Length is required to send to Pen Input or not
        */
        TBool UpdateMsgLenToPenInput();
        
        /**
         *     Notification from central repository
         */
       	void HandleNotifyInt( TUint32 aId, TInt aNewValue );
    private:     // Enumerations

    enum TUniEditorFlags
        {
        EUseEstimatedSmilSize   = 0x0001,
        ELaunchSuccessful       = 0x0002,
        EShowGuidedConf         = 0x0004,
        EEditorExiting          = 0x0008,
        ERunAppShutterAtExit    = 0x0010,
        EEditorClosing          = 0x0020,
        EInsertAddsSlide        = 0x0040,   
        EMaxRecipientShown      = 0x0080,   // max recipient note already shown
        EShowInfoPopups         = 0x0100,   // show "message type changed" notes
        EMaxSmsSizeNoteShown    = 0x0200,   // max size of sms note is shown
        ESubjectOkInSms         = 0x0400,   // if defined subject is ok to show also in sms auto mode
        EHideSizeCounter        = 0x0800,   // hide size counters in navipane
        ESmsCounterShownLast    = 0x1000,    // last time a counter was updated it was sms counter
        EShowSmsSentInManyParts = 0x2000,    // ask the query "message will be sent in x parts"
        EDoNotUpdateTitlePane   = 0x4000,    // set while Objects view is open so the title pane remains ok if layout is changed
        EShowSmsSizeNoteNextTime = 0x8000, // when max sms size reached -> show this note after next letter
        EToolbarHidden          = 0x10000,
        ELockedSmsWithAtta      = 0x20000,
        EMsgEditInProgress      = 0x40000,
        ESmsSizeWarningNoteShown = 0x80000, // Korean req: 415-5434
        EHideMessageTypeOption   = 0x100000
        };

    private:     // Data

        TInt                       iDisplaySize;       // Current msg size in navipane. Kb/Mb

        CAknLocalScreenClearer*    iScreenClearer;
        CPeriodic*                 iIdle;

        CUniSmilModel*             iSmilModel;         // Fetch pointer from document at launch
        CUniClientMtm*             iMtm;               // Fetch pointer from document at launch

        CMDXMLParser*              iParser;    // SMIL Parser fore presentation preview
        CMDXMLDocument*            iDom;        // Dom created by parser for smil player

        CAknInputBlock*            iInputBlocker;    // AknInputBlocker

        CAknWaitDialog*            iWaitDialog;
        TInt                       iWaitResId;

        TInt                       iOriginalSlide;
        TInt                       iNextSlide;
        TMsgControlId              iNextFocus;
        CAknTitlePane*             iTitlePane;
        HBufC*                     iTitleSms;
        HBufC*                     iTitleMms;
        TInt                       iSmsBodyLength;
        TInt                       iSmsSubjectLength;
        TInt                       iPrevSmsLength; 

        HBufC*                     iSmsLengthFormatBuffer;
        HBufC*                     iMmsLengthFormatBuffer;

        CGulIcon*                  iIconSms;
        CGulIcon*                  iIconMms;
        
        CAknInfoPopupNoteController* iPopupNote;
        HBufC*                     iPopupChangedMmsBuffer;
        HBufC*                     iPopupChangedSmsBuffer;

        CRepository*               iMuiuSettRepository;

        TUint32                    iEditorFlags;
        TInt                       iSupportedFeatures;

        CUniSlideLoader*           iSlideLoader;

        CUniEditorHeader*          iHeader;
        CUniEditorChangeSlideOperation* iChangeSlideOperation;
        CUniEditorInsertOperation* iInsertOperation;
        CUniEditorLaunchOperation* iLaunchOperation;
        CUniEditorSaveOperation*   iSaveOperation;
        CUniEditorSendOperation*   iSendOperation;
        CUniEditorOperation*       iActiveOperation;
        CUniEditorVCardOperation*  iVCardOperation;

        CMsgMediaInfo*             iInsertingMedia;
        TMsgMediaType              iInsertingType;
        
        CActiveSchedulerWait       iWait;
        
        CAknNavigationControlContainer* iNaviPane;

        CDocumentHandler*           iDocHandler;
        TInt                        iMskResId;
        
        // Used to connect pointer down and up events to correct control. Not owned.
        CMsgBaseControl*            iFocusedControl;
        CMsgBaseControl*            iStoredBodyControl;
        
        CAknToolbar*                iFixedToolbar;
        
        RPeninputServer             iPeninputServer;
        CCenRepNotifyHandler*       iNotifyHandler;
        CRepository*                iInputTxtLangRepository;
        TBool                       iMsgLenToVKB;
        
        //Turkish SMS-PREQ2265 specific...
        TInt                        iNumOfPDUs;
        TInt                        iCharsLeft;
        TBool                       iNLTFeatureSupport;
		HBufC*                      iPrevBuffer;
        TBool						iSettingsChanged;    
        
        //Korean Req: 415-5434
        HBufC*                     iPopupSmsSizeAboveLimitBuffer;
        HBufC*                     iPopupSmsSizeBelowLimitBuffer;
		TBool                      iFinalizeLaunchL;
        // Own: For long tap detecting
        CAknLongTapDetector* iLongTapDetector;

        // For long tap handling
        TBool iTapConsumed;     
      /** 
      * Contact image selection stylus popup menu 
      * Own. 
      */  
    	CAknStylusPopUpMenu* iEmbeddedObjectStylusPopup;
    };

#include "UniEditorAppUi.inl"

#endif      //  _UniEditorAPPUI_H

// End of File

