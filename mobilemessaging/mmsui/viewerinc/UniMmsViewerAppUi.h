/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     CMmsViewerAppUi class definition.
*
*/



#ifndef MMSVIEWERAPPUI_H
#define MMSVIEWERAPPUI_H

// INCLUDES
#include <finditemengine.h>

#ifdef RD_SCALABLE_UI_V2
#include <akntoolbarobserver.h>
#endif

#include <MuiuMessageIterator.h>

#include <MsgEditorAppUi.h>             // CMsgEditorAppUi

#include <cenrepnotifyhandler.h>
#include <DRMCommon.h>

#include <msgasynccontrolobserver.h>    //MMsgMediaControlObserver
#include <uniobject.h>
#include <unidrminfo.h>                 // CUniDrmInfo
#include <AknNaviDecoratorObserver.h>
#include <aknlongtapdetector.h> // for long tap

#include "MmsViewer.hrh"                // enumerations
#include "MmsViewerOperation.h"
#include "UniMmsSvkEvents.h"               // MMmsSvkEventsObserver

// FUNCTION PROTOTYPES
GLREF_C void Panic( TMmsViewerExceptions aPanic ); 

// FORWARD DECLARATIONS
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
class CAknNavigationDecorator;
#endif
class CAknNavigationControlContainer;
class CAknTitlePane;
class CAknWaitDialog;
class CMdaAudioPlayerUtility;
class MVideoControlStateObserver;
class CFindItemMenu;
class CFindItemEngine;
class CDocumentHandler;
class CDRMRights;

class CAknLocalScreenClearer;
class CMmsViewerLaunchOperation;
class CMmsViewerChangeSlideOperation;
class CMmsViewerHeader;
class CMmsViewerSendReadReportOperation;

class CMsgMediaInfo;
class CMsgMediaControl;
class CMsgAudioControl;
class CMsgVideoControl;
class CMsgImageControl;
#ifdef RD_SVGT_IN_MESSAGING
class CMsgSvgControl;
#endif
class CMmsConformance;
class CUniSlideLoader;
class CUniSmilModel;
class CUniObjectList;

class CRepository;
class CSendingServiceInfo;
class CMsgVoIPExtension;
class CAknInputBlock;
class CPeriodic;
class CAknVolumeControl;
class CAknInputBlock;
class CAknToolbar;
class CAknStylusPopUpMenu;

// CLASS DECLARATION

/**
*  CMmsViewerAppUi
*  Inherited from CMsgEditorAppUi
*/
class CMmsViewerAppUi :
    public CMsgEditorAppUi,
    public MCenRepNotifyHandlerCallback,
    public MMsgAsyncControlObserver,
    public MMmsViewerOperationObserver,     // ViewerOperationEvent
    public MMmsSvkEventsObserver,           // MmsSvkChangeVolumeL
    public MAknServerAppExitObserver,       // for image viewer
#ifdef RD_SCALABLE_UI_V2
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
    public MAknNaviDecoratorObserver,       //for navidecorator
#endif
    public MAknToolbarObserver,             //toolbar
#endif
    public MCoeControlObserver,              // of CAknVolumeControl
    public MUniObjectObserver,
    public MAknLongTapDetectorCallBack
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMmsViewerAppUi();

        /**
        * Destructor.
        */
        virtual ~CMmsViewerAppUi();
        
    protected: 
    
        /**
         * Prepare to exit.
         * from CEikAppUi
         */
        void PrepareToExit();

    private: // New functions        
        
        /**
        * Calls InitNavipaneL, PrepareHeaderL and PrepareBodyL
        * and does some validity checks.
        */
        void DrawSlideL();
        
        /**
        * Set waiting note.
        */
        void SetPendingNote(    TInt                aResourceId, 
                                CUniObject*         aObject = NULL,
                                TInt                aError = KErrNone );

        /**
        * Shows notes related to message opening, 
        * which could not shown until loading was complete
        */
        void ShowPendingNoteL();

        /**
        * Changes slide. Calls ResetViewL and DrawSlideL. Creates
        * a screen blanker to hide the view operations from the user.
        * @param aSlideNumber The number of the slide to be shown next.
        * @param aScreenClearer Screen clearer is run
        */
        void ChangeSlideL( TInt aSlideNumber, TBool aScreenClearer );

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
        * Resets the message view. This function is called
        * when moving from slide to slide.
        * @param TBool ETrue -> only body reseted
        *              EFalse -> body and header reseted
        */
        void ResetViewL();


        // Command functions
        
        /**
        * Implements reply commands.
        * Launches MMS Editor for replying/forwarding.
        * @param aCase either EReplyToSender or EReplyToAll
        */
        void DoReplyL( TInt aCase );

        /**
        * Implements forward command.
        * Launches MMS Editor for replying/forwarding.
        */
        void DoForwardL();

        TBool CanForwardL( TInt& aResourceId );
        TBool CanForwardObjectsL( TInt& aResourceId );
        void CanForwardObjectsInListL( 
                                        CUniObjectList* aObjectList, 
                                        TInt& aSdCount, 
                                        TInt& aFlCount, 
                                        TInt& aNonConfCount, 
                                        TInt& aNormalCount,
                                        TInt& aResourceId ) const;

        /**
        * Implements reply via SMS/Audio message command.
        * Launches SMS/Audio message/Email Editor by utilising SendUI.
        * @param aCase EMmsViewerReplyViaAudio or EMmsViewerReplyViaMail
        */
        void DoReplyViaL( TInt aCase);

        /**
        * Implements the view image command.
        * Launches PhotoAlbum's ImageViewer.
        */
        void DoViewImageL();

        /**
        * Implements the objects command.
        * Opens the Objects view where the user can access
        * individual media elements in a list format.
        */
        void DoObjectsL();

        /**
        * Implements the move command.
        * Shows a folder selection dialog
        * (CMsgFolderSelectionDialog), moves the message
        * entry into selected folder and exits the viewer.
        */
        void DoMoveMessageL();
        
        /**
        * Implements the "Send" key functionality.
        * Uses MsvUiServiceUtilities::CallServiceL to create a call.
        */
        void DoHandleSendKeyL();
        
        /**
        * Implements the message info command.
        * Opens message info dialog provided by MMS ui mtm.
        */
        void DoMessageInfoL();
        
        /**
        * Start SMIL presentation
        */
        void DoShowPresentationL();

        /**
        * Implements the delete command.
        * Shows a confirmation query, deletes the message
        * entry and exits the viewer.
        */
        void DoDeleteAndExitL();

        /**
        * Sets qtn_drm_preview_msg into waiting queue ( SetPendingNote() ),
        * if needed.
        */ 
        void ResolveDrmWaitingNotes();
        
        //Audio related functions:
        //
        
        /**
        * Cancels the automatic play of an audio (or a video)
        * file.
        */
        void CancelAutoplay();
                  
        /**
        * State of an control on the UI has changed
        * @param aControl control
        * @param aState new state
        * @param aState previous state
        */
        void MsgAsyncControlStateChanged(   
                                    CMsgBaseControl& aControl,
                                    TMsgAsyncControlState aNewState,
                                    TMsgAsyncControlState aOldState );
        void DoMsgMediaControlStateChangedL(   
                                    CMsgMediaControl& aControl,
                                    TMsgAsyncControlState aNewState,
                                    TMsgAsyncControlState aOldState );
        
        /**
        * Asynchronous control on the UI has received resource change event.
        *
        * @param aControl control
        * @param aType event type
        */
        void MsgAsyncControlResourceChanged( CMsgBaseControl& aControl, TInt aType );
        
        void DoVideoStateChangeL( TInt aState );
        void DoAudioStateChangeL( TInt aState );

        /**
        * Handles the actual navipane and softkey updating according
        * to the state of audio playback. This function does not set 
        * volume control visible, it is done using ShowVolumeL( ).
        * When playing starts empties the navi pane.
        * Hides the volume control when playing stops.
        */
        void UpdatePanesL( TBool aAudioPlaying, TBool aVideoPlaying );

        /**
        * From MCenRepNotifyHandlerCallback
        */
        void HandleNotifyInt(TUint32 aId, TInt aNewValue);
        void HandleNotifyError(TUint32 aId, TInt error, CCenRepNotifyHandler* aHandler);
        void HandleNotifyGeneric(TUint32 aId);

        /**
        * From MMessageIteratorObserver. 
        * @param See documentation of MMessateIteratorObserver
        */
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
        void HandleIteratorEventL(TMessageIteratorEvent aEvent);
#endif        
        /**
        * Returns ETrue if the message is self composed and
        * EFalse otherwise (a received message).
        * @return ETrue if message is self composed,
        *         EFalse otherwise (received)
        */
        TBool IsOwnMessage() const;
                
        /**
        * Returns ETrue if the message has a sender, EFalse
        * otherwise (either self composed message or received
        * from a secret number).
        * @return ETrue if message has sender
        *         EFalse otherwise
        */
        TBool HasSender() const;
        
        /**
        * Checks whether Reply->To all should be shown.
        * For received messages checks that there is more
        * than one recipient. For sent messages checks that
        * there is at least one recipient.
        * @return ETrue if Reply->To all should be shown
        *         EFalse otherwise
        */
        TBool ShowReplyToAll() const;

        /**
        * Checks whether Reply menu should be shown at all.
        * @return ETrue if Reply menu should be shown
        *         EFalse otherwise
        */
        TBool ShowReplyMenu() const;
        
        // Notes:

        /**
        * Show an information note
        * @param aResourceId The resource if of the note to be shown
        */
        void ShowInformationNoteL( TInt aResourceId );
        
        /**
        * Show an information note
        * @param aResourceId The resource if of the note to be shown
        */
        void ShowErrorNoteL( TInt aResourceId );
        
        /**
        * Show a confirmation query
        * @param aResourceId The resource if of the query to be shown
        */
        TInt ShowConfirmationQueryL( TInt aResourceId ) const;

        TInt ShowConfirmationQueryL(   TInt            aResourceId,
                                        const TDesC&    aString) const;

        /**
        * Auxiliary function. Shows a confirmation query if it is not shown before. 
        *
        * @param aResourceID
        * @return the return value returned by CAknQueryDialog::ExecuteLD
        */  
        TInt ShowGuidedModeConfirmationQueryL( TInt aResourceID );

        /**
        * Shows "Out of disk" error note if disk space would
        * go below critical level when allocating aSize
        * @param aSize Bytes wanted to allocate
        * @return ETrue if below critical after allocation
        *         EFalse if allocation OK
        */
        TBool ShowOutOfDiskNoteIfBelowCriticalLevelL( TInt aSize );

        /**
        * Return a pointer to document-object
        * @return CMmsViewerDocument*
        */        
        CMmsViewerDocument* Document() const;

        /**
        * Shows any DRM related notes that are related to
        * message opening:
        * - "No rights" query
        * - "Expired rights" query
        * - "Preview rights" note
        */
        void ResolveDRMNotesL();

        /**
        * Returns the control id of the currently focused control.
        * @return Control id of the focused control or
        *         EMsgComponentIdNull if no control focused
        */
        TInt FocusedControlId();

        /**
        * Sets automatic highlighting on/off.
        * @param aEnable if ETrue sets automatic highlighting on,
        *                if EFalse sets automatic highlighting off
        */
        void SetFindModeL( TBool aEnable );

 
        /**
        * Starts to play/display media including image.
        * @param aControl control to be played
        */         
        void MediaPlayL(CMsgMediaControl* aControl);

        /**
        * Handle selection key for media object ( excluding 3gpp presentation )
        * This function is entered when Options menu->Play is selected.
        * Resulting action is one of the following: play, embedded play, activate or get rights.
        * @param aMedia control to be be handled
        */         
        void MediaOpenL( CMsgMediaControl* aMedia );

        /**
        * Stops playing media. Does not stop animations. 
        */         
        void MediaStop();

        /**
        * Stops playing single media.
        * @param aControl control to be played
        */         
        void MediaStop(CMsgMediaControl* aControl);

        /**
        * Closes every media control. Closing means freeing file
        * specific resources that other application can use the file.
        */         
        void MediaClose();

        /**
        * Closes single media.
        * @param aControl control to be played
        */         
        void MediaClose(CMsgMediaControl* aControl);

        /**
        * Reloads slide
        */
        void ReloadSlideL();

        /**
        * Resets every control. Frees all resources
        */         
        void MediaReset();

        /**
        * Resets single control. Frees all resources
        */         
        void MediaReset(CMsgMediaControl* aControl);

        /**
        * Checks whether media is initialised and media control can be
        * selected. 
        * This function returns ETrue, if media is DRM protected and
        * no rights exist.
        */         
        TBool MediaInitializedL( CMsgMediaControl*   aMedia );

        /**
        * Checks whether media can be played or tried to be played.
        * This function returns EFalse, if media is DRM protected and
        * no rights exist.
        */         
        TBool CMmsViewerAppUi::MediaCanPlay(const CMsgMediaControl* aMedia,
                                            const CUniObject* aMmsObject ) const;


        /**
        * Checks whether media can be stopped or tried to be played
        */         
        TBool MediaCanStop(CMsgMediaControl* aMedia);

        /**
        * Checks whether media can be paused.
        * This means also that media is really playing.
        */         
        TBool MediaIsRunning(CMsgMediaControl* aMedia);

        /**
        * Returns UI control id
        * @param aControl TMmsContolIds only
        */         
        CMsgMediaControl* MediaControlById(TInt aControl);

        /**
        * Starts animation
        */
        void StartAnimationL();
        
        /**
         * Stops animation
         */
        void StopAnimation();
        
        /**
        * Implements saving text or media object into file system from
        * mms viewer. Saves pure text files directly and otherwise 
        * for multimedia and xHTML files calls DoSaveObjectL()
        */
        void SaveObjectL();
 
        /**
        * Implements multimedia objects and xhtml into file system from
        * mms viewer.
        */
        void DoSaveObjectL(CUniObject& aObject );
 
        /**
        * Shows "Upload" list query and if an item is selected, send the message
        * as upload.
        */
        void UploadL();
        
        /** 
        * From CEikAppUi
        */
        void HandleResourceChangeL(TInt aType);

        /**
        * Opens image into (embedded) image viewer
        */
        void ViewImageL( CMsgMediaControl& aMediaCtrl );

        /**
        * Consumes DRM rights, if DRM protected media. If no DRM rights left 
        * or other error occurs during consuming rights, updates icon
        * @return   ETrue: media was not DRM protected or DRM rights were 
        *                   consumed successfully
        *           EFalse: media was DRM protected and no rights were left 
        *                   or other error occurred
        */
        TBool HandleConsumeRightsL( CMsgMediaControl& aMedia ); 


        /**
        * If image has not rights, error note must be shown.
        * This function handles the situation.
        */
        void HandleImageNoRightsInLaunch( );
        // From MAknLongTapDetectorCallBack
        /**
         * @see MAknLongTapDetectorCallBack
         */
        void HandleLongTapEventL(
            const TPoint& aPenEventLocation, 
            const TPoint& aPenEventScreenLocation );
        
        /**
         *  For Handling iMskId for an embedded object
         */
 		void HandleSelectionKeyL( );


    private: // Functions from base classes
        
        /**
        * From MsgEditorAppUi
        */
        void DoMsgSaveExitL();
            
        /**
        * From MMsgEditorLauncher.
        * Part of the function is implemented in the DoFinaliseLaunchViewL()
        */
        void LaunchViewL();
    
        /**
        * The actual implementation of LaunchViewL
        */
        void DoLaunchViewL();

        /**
        * From MMsgEditorObserver
        * @param See documentation of MMsgEditorObserver
        */
        CMsgBaseControl* CreateCustomControlL( TInt aControlType );
        
        /**
        * From MMsgEditorObserver
        * @param See documentation of MMsgEditorObserver
        */
        void EditorObserver(TMsgEditorObserverFunc aFunc, TAny* aArg1, TAny* aArg2, TAny* aArg3);

        void DoEditorObserverL(TMsgEditorObserverFunc aFunc, TAny* aArg1, TAny* aArg2, TAny* aArg3);

        /**
        * From CCoeAppUi
        * @param See documentation of CCoeAppUi
        * @return TKeyResponse
        */
        TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * From CCoeAppUi
        * @param See documentation of CCoeAppUi
        */
        void HandleWsEventL(const TWsEvent& aEvent,CCoeControl* aDestination);
        
        /**
        * From CEikAppUi
        * @param See documentation of CEikAppUi
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CAknAppUi
        * @param See documentation of CAknAppUi
        */
        void ProcessCommandL( TInt aCommand );

        /**
        * From MEikMenuObserver
        * @param See documentation of MEikMenuObserver
        */
        void DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane );

        /**
        * From MAknServerAppExitObserver. Must not leave.
        * DoHandleServerAppExitL
        */
        void HandleServerAppExit(TInt aReason);        

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
        /**
        * From CCoeControl
        * Handles volume changes from volume of navi pane.
        * When Pen enabled volume can change many steps at one tap.
        */
        void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);
   
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
   
    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL(); 

        /** 
        * Launches help application
        */
        void LaunchHelpL();

        /** 
        * Gets help context
        */
        
        CArrayFix<TCoeHelpContext>* HelpContextL() const;          

        void ViewerOperationEvent(  TMmsViewerOperationType     aOperation,
                                    TMmsViewerOperationEvent    aEvent,
                                    TInt                        aError );
        void DoViewerOperationEventL(   TMmsViewerOperationType     aOperation,
                                        TMmsViewerOperationEvent    aEvent );

            /// loading slide has completed
        void DoViewerOperationEventSlideL();
            /// loading launch has completed
        void DoViewerOperationEventLaunchL( );

        TUint32 ObjectsAvailable();

        void BeginActiveWait( CMmsViewerOperation* aOperation );
        void EndActiveWait();

        /**
        * Launches wait dialog.
        * @param aParam if slide change operation, this is index of new slide
        */
        void LaunchWaitDialogL( TInt aParam = 0 );

        /**
        * Executes part of the launching. See LaunchViewL()
        */
        void DoFinaliseLaunchViewL();

        /**
        * Gets focused CUniObject. 
        * Object is returned only,if is EMsgImageControlId, EMsgVideoControlId, or EMsgAudioControlId.
        * iFocusedObjectType is applicable always.
        * @since 3.0
        */
        CUniObject* GetFocusedMmsObject();

        /**
        * Checks whether silent ringing type is selected.
        * @ret ETrue, silence set
        *      EFalse, silence not set
        */
        TBool IsSilenceL();
        
        /**
        * Calls mms mtm ui to launch delivery status dialog after user has choosed EMmsViewerDeliveryStatus from the options menu.
        *
        * @since    3.0
        */
        void OpenDeliveryPopupL( );
    
        void PrintNeededControls( );

        /*
        * @return ETrue, editor is properly initialised to receive commands
        *         EFalse, editor is not yet properly initialised to receive commands
        */
        TBool IsLaunched() const;

        CUniObject* ObjectByMediaControl( CMsgMediaControl& aControl);
        CUniObject* ObjectByBaseControl( CMsgBaseControl& aControl );

        /*
        * Checks whether description exists in the stored message.
        * If it doesn't exist, description is saved into message store 
        * from first found. Uses SetAndSaveDescriptionL() 
        */ 
        void CheckAndSetDescriptionL();
        
        /*
        * Saves description into received message from
        * given text object. Existence of description is no more
        * checked. 
        * aText text object from which description is saved
        */ 
        void SetAndSaveDescriptionL(const CPlainText& aText);

        /*
        * Update icon or text of MSK CBA button
        */ 
        void UpdateMskL( );
        
        void WaitReleaseOfImageL( CUniObject& aObject );

        /*
        * Check whether media object is lacking rights.
        * If media has no rights 
        * @param aMedia media to check 
        * @return KErrNone, valid drm rights exits or media is not drm protected
        *         other value, no drm rights
        * @since 3.2
        */ 
        TInt MediaHasNoDrmRights( CMsgMediaControl* aMedia );

        /**
        * Implements HandleCommandL()
        * @param See documentation of CEikAppUi
        */
        void DoHandleCommandL( TInt aCommand );

        /**
        * Sends read report if permitted and asked
        */
        void SendReadReportL(   TMmsViewerOperationType     aOperation,
                                TMmsViewerOperationEvent    aEvent );

        /**
        * The function must be called when the volume is changed. 
        * Any checks are not needed from caller of this function.
        * This function changes the volume even, if volume control is not visible 
        * in the navi pane.
        * If changing volume level makes sense this function
        * - changes the volume level into cenrep
        * - show the new volume on the volume control of the navi pane.
        * ( To check exact conditions when volume control is visible in the navi pane, 
        * see .cpp file)
        *         *
        * This function resolves itself only whether the audio or video is playing.
        * Then DoChangeVolumeL() is called, if applicable.
        * After that to show the new volume level in the navi pane ShowVolumeL() is called.
        *
        * @param aVolume: indicates either absolute value, if aChange is EFalse
        *                 or change of volume, if aChange is ETrue.
        * @param aChange: indicates whether aVolume is absolute value or change of 
        *                 the current volume.
        */
        void ChangeVolumeL( TInt aVolume, TBool aChange );

        /**
        * Otherwise like ChangeVolumeL but either audio or video must be playing.
        * This function saves the new volume level into CenRep, if applicable.
        * Sets the new level into audio or video player.
        * ( To check exact conditions when volume control is visible in the navi pane, 
        * see .cpp file)
        *
        * If volume set is set before playback this function can be called as follows
        * DoChangeVolumeL( 0 , ETrue, * )
        * @param aVolume: indicates either absolute value, if aChange is Efalse
        *                 or change of volume, if aChange is Etrue.
        * @param aChange: indicates whether aVolume is absolute value or change of 
        *                 the current volume.
        * @param aAudioPlaying  ETrue audio is playing
        *                       EFalse video is playing
        * @return TInt new absolute volume level
        *              KErrNotFound, if volume level is not known.
        *              In this case ShowVolumeL should not be called
        *              
        */
        TInt DoChangeVolumeL( TInt aVolume, TBool aChange, TBool aAudioPlaying );

        /**
        * Set volume control visible into navi pane, if applicable. 
        * Either audio or video must be playing.
        * ( To check exact conditions when volume control is visible in the navi pane, 
        * see .cpp file)
        *
        * @param aNewVolume: new volume level
        * @param aAudioPlaying  ETrue audio is playing
        *                       EFalse video is playing
        */
        void ShowVolumeL(   TInt  aNewVolume,
                            TBool aAudioPlaying );

        /** 
        * Enables receiving events changes in foreground application status.
        * Resolves those Window groups id, which do no stop animation, audio or
        * video playback. The workgroup ids are assigned to member variables */
        void GetNonStoppingWgIds( );

        /** 
        * After receiving event in change of foreground application status,
        * checks whether new foreground application causes stopping animation,
        * playback of audio or video.
        * Result is set to iIsStopPlaybackWgId.
        */
        void ResolveCurrentWgIdL( );

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
        * Resets flags to enter normal operation state e.g. accept commands
        */
        void ResetOperationState();

        /**
        * Handles delayed (external) exit 
        * @param aDelayTime in microsecods
        */        
        void DoDelayedExit( TInt aDelayTime );

        /**
        * To check whether asyncronous controls are ready and wait note dismissed
        * @return TBool
        */        
        TBool AreControlsReadyAndWaitNoteDismissedL( );
        
#ifdef RD_SCALABLE_UI_V2
        /**
        * Sets tool bar item visibilities
        * @return TBool
        */        
        void SetToolBarItemVisibilities();
#endif

        /**
        * Checks whether DRM rights have been consumed
        * @return TBool
        */                
        TBool IsRightsConsumed( CUniObject& aObject ) const;

        /**
        * Checks whether objects has drm rights left just now
        * @return TBool
        */                
        TBool IsRightsNow( CUniObject& aObject ) const;

        /**
        * Checks whether object has drm rights when downloaded to the current slide
        * @return TBool
        */                
        TBool IsDrmRightsWhenLoaded( CUniObject& aObject ) const;

        /// MUniObjectObserver
        void HandleUniObjectEvent(  CUniObject&  aUniObject,
                                    TUniObjectEvent aUniEvent );
        void SimulateUniObjectEventL( );
        /**
        * Reloads one control without need to reload whole slide.
        *
        * @since S60 5.0
        *
        * @param aMediaControl Control which needs to be reloaded.
        * @param aObject Object related to the control
        */
        void ReloadControlL( CMsgMediaControl* aControl, CUniObject& aObject );

        /**
        * Resolve CUniObject related to media control.
        *
        * @since S60 5.0
        * @param aControl control for which CUniObjects is needed
        * @return CUniObject related to the given control
        */
        CMsgMediaControl* MediaControlByObject( CUniObject&    aUniObject );

        /**
        * Sets title of the mms viewer app into status pane
        */
        void SetTitleIconL();
        
        /**
        * Check whether video call is active or not
        */
        TBool IsVideoCall();

    private:     // From MMmsSvkEventsObserver
                /**
        * Called when volume level should be changed.
        * Set volume control visible into navi pane.
        * This function calls ChangeVolumeL
        * @param aVolumeChange: +1 change volume up
        *                       -1 change volume down
        */
        void MmsSvkChangeVolumeL( TInt aVolumeChange );
        
        /**
        * Callback for CPeriodic
        */        
        static TInt DelayedExitL( TAny* aThis );
                            
    private:     // Data

        enum TMmsViewerFlags
            {
            ELaunchSuccessful           = 0x0010,
			EPresentationHasAudio       = 0x0020,   
			// Two more videos in the same message with smil is now supported 
			// ENoPresentationPlaying      = 0x0040,
            ESuspiciousSmil             = 0x0080,
            EShowGuidedConf             = 0x0100,
            EAutohighLightEnabled       = 0x0200,
            EProhibitNonDrmMusic        = 0x0400,
            EVideoHasAudio              = 0x0800,
            EUploadInitialized          = 0x1000,
            // Character key events are sent to application first, which sets the volume into volume control. 
            // When touch enabled volume control of navi pane is supported, touch events are sent to volume control first.
            // And sets its new volume itself. The latter case is indicated by following flag
            EVolumeControlTriggered     = 0x2000,
            EViewerExiting              = 0x4000,
            EInObjectsView              = 0x8000,
            EInitialFlags = EVideoHasAudio
            };

        enum TViewerState
            {
            ENormal = 0,
            EBusy,
            EReseted
            };

        enum TReplyForwardCase
            {
            EReplyToSender,
            EReplyToAll
            };
            
            // determines which errors are shown during/after phase completion
        enum TOpeningPhase
            {
            EOpeningNone    = 0,
            EOpeningMessage,    
            EOpeningSlide
			};

        //Reference pointers:
        CUniSmilModel*              iSmilModel;

        CMmsClientMtm*              iMtm;
        CAknNavigationControlContainer* iNaviPane;
        CEikButtonGroupContainer*   iButtonGroup;

        //Owned pointers:
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
        CAknNavigationDecorator*    iNaviDecorator; // Overrides the variable in base class
#endif
        CAknNavigationDecorator*    iVolumeDecorator;

        CUniSlideLoader*            iSlideLoader;
        
        CMmsViewerHeader*           iHeader;
        CMmsViewerLaunchOperation*  iLaunchOperation;
        CMmsViewerChangeSlideOperation* iChangeSlideOperation;
        CAknLocalScreenClearer*     iScreenClearer;

        CAknWaitDialog*             iWaitDialog;
        CUniObject*                 iPendingNoteObject;
        TInt                        iPendingNoteError;
        TInt                        iPendingNoteResource;
        TInt                        iPendingNotePriority;
       
        CFindItemMenu*              iFindItemMenu;
        
        CDocumentHandler*           iDocHandler;

        CCenRepNotifyHandler*       iNotifyHandler;
        CRepository*                iCUiRepository;
        CRepository*                iMmsRepository;

        TInt                        iViewerFlags;
        TUint32                     iReleasedObjects;
        TViewerState                iViewerState;

		TInt                        iSupportedFeatures;

        CActiveSchedulerWait        iWait;

        CMsgImageControl*           iImage;
            /// if 3gpp presentation, this is not set
        CMsgAudioControl*           iAudio;
        CMsgVideoControl*           iVideo;
#ifdef RD_SVGT_IN_MESSAGING
        CMsgSvgControl*            iSvgt;
#endif        
        TInt                        iNewSlide;
            /// enum TOpeningPhase
        TInt                        iOpeningState;
        RPointerArray<CSendingServiceInfo> iUploadServices;
        
        // Own: Contains information of the VoIP profiles.
        CMsgVoIPExtension*          iMmsVoIPExtension;        
        // Events from SVK keys 
        CMmsSvkEvents*              iSvkEvents;
		CAknInputBlock*				iInputBlocker;
        CMmsConformance*            iMmsConformance;
        
        /// save focused control for restoring the focus later
        TInt                        iFocusedControlId;
        /// current resource of MSK CBA button
        TInt                        iMskId;
        CMmsViewerOperation*        iActiveOperation;
        CPeriodic*                  iIdle;

        // Pen Support: Target of last pointer event
        CMsgBaseControl*            iPointerTarget;

        /** 
        * When Objects view is entered, iNoTitlePaneUpdate is set to ETrue.
        * If layout change takes place during objects view, title must not 
        * be set into title pane. 
        */
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
        TBool                       iNoTitlePaneUpdate;
#endif
        CGulIcon*                   iTitleIcon;
        
        // Contains prohibit mime types.
        HBufC*                      iProhibitMimeTypeBuffer;
        
        /// Sends read report
        CMmsViewerSendReadReportOperation* iSendReadReportOperation;        

        /** applications, which should not stop playback of animation, audio or video        
        * Wg=Window Group
        */
        TInt                        iFastSwapWgId;   
        TInt                        iSysWgId;
        TInt                        iAknNotifyServerWgId;
        TInt                        iMmsViewerWgId;
        TInt                        iScreenSaverWgId;	
        // Whether playback needs to be stopped
        TBool                       iIsStopPlaybackWgId;
        CAknToolbar*                iToolbar;
        TInt                        iMaxForwardSlideCount;
        CAknTitlePane*              iTitlePane; 
        CGulIcon*                   iAppIcon;
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

#endif      //  MMSVIEWERAPPUI_H

// End of File
