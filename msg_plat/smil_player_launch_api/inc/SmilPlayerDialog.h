/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SmilPlayerDialog  declaration
*
*/



#ifndef SMILPLAYERDIALOG_H 
#define SMILPLAYERDIALOG_H

// INCLUDES
#include <AknDialog.h>
#include <remconcoreapitargetobserver.h>
#include <ConeResLoader.h>

// FORWARD DECLARATIONS
class TCoeHelpContext;
class CFindItemMenu;
class CSmilPlayerPresentationController;
class MMediaFactoryFileInfo;
class CMDXMLDocument;
class CRepository;
class CPeriodic;
class CRemConInterfaceSelector;
class CRemConCoreApiTarget;

// CLASS DECLARATION

/**
* CSmilPlayerDialog dialog class
*   This is an interface class for applications to use SmilPlayer.
*   Derived from CAknDialog to enable options menu.
*
* Usage:
*   ----------------------- Copy & Paste -------------------------------
*   #include <smilplayerdialog.h> //to the .cpp file where the following 
*                                 //is going to be used
*
*   CSmilPlayerDialog* dialog = CSmilPlayerDialog::NewL( CMDXMLDocument* aDOM, 
*                                                        MMediaFactoryFileInfo* aFileInfo,
*                                                        const TDesC& aBaseUrl, 
*                                                        TBool aPreview = EFalse,
*                                                        TBool aAudioOn = EFalse );
*
*   or
*
*   CSmilPlayerDialog* dialog = CSmilPlayerDialog::NewL( RFile& aFileHandle,
*                                                        MMediaFactoryFileInfo* aFileInfo,
*                                                        const TDesC& aBaseUrl, 
*                                                        TBool aPreview = EFalse,
*                                                        TBool aAudioOn = EFalse );
*
*   dialog->ExecuteLD();
*   --------------------------------------------------------------------
* 
*   Link agains library: smilplayer.lib.
*   The dialog will be automatically destroyed by ExecuteLD.
*
* @lib smilplayer.lib
* @since 2.0
*/
class CSmilPlayerDialog : public CAknDialog,
                          public MCoeForegroundObserver,
                          public MRemConCoreApiTargetObserver
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aDOM           Pointer to Document Object Handle. Ownership is not 
        *                       transferred.
        * @param aFileInfo      Handle to the media objects file info.
        * @param aBaseUrl       Base URL that is used for resolving relative URLs.
        * @param aPreview       Boolean if player started to preview mode.
        * @param aAudioOn       Boolean if presentation includes the audio.
        *
        * @return Pointer to created CSmilPlayerDialog
        */
        IMPORT_C static CSmilPlayerDialog* NewL( CMDXMLDocument* aDOM, 
                                                 MMediaFactoryFileInfo* aFileInfo,
                                                 const TDesC& aBaseUrl, 
                                                 TBool aPreview = EFalse,
                                                 TBool aAudioOn = EFalse );
                                                 
        /**
        * Two-phased constructor.
        *
        * @param aFileHandle    Handle to the .smil file. Ownership is not transferred.
        * @param aFileInfo      Handle to the media objects file info.
        * @param aBaseUrl       Base URL that is used for resolving relative URLs.
        * @param aPreview       Boolean if player started to preview mode.
        * @param aAudioOn       Boolean if presentation includes the audio.
        *
        * @return Pointer to created CSmilPlayerDialog
        */
        IMPORT_C static CSmilPlayerDialog* NewL( RFile& aFileHandle,
                                                 MMediaFactoryFileInfo* aFileInfo,
                                                 const TDesC& aBaseUrl, 
                                                 TBool aPreview = EFalse,
                                                 TBool aAudioOn = EFalse );
                                                 
        /**
        * Destructor.
        */
        virtual ~CSmilPlayerDialog();
        
    public: // New functions

        /**
        * Initializes the dialog and calls CAknDialog's 
        *   ExecuteLD()
        * This method need to be called to execute the dialog.
        * After exiting the dialog gets automatically destroyed
        * 
        * @since 2.0
        *
        * @return TInt  ID value of the button that closed the dialog.
        */
        IMPORT_C TInt ExecuteLD();

        /**
        * Is called from the presentationcontroller
        * when engine indicates about the stop.
        *
        * @since 2.0
        */
        void PresentationStoppedL();
        
        /**
        * The function is used to start/continue 
        *   playing the presentation
        */
        void PlayTappedL();
       
        /**
        * The function is used to pause the presentation
        */
        void PauseTappedL();
        
        /**
        * Deactivates scrolling.
        */
        void DeactivateScrollingL();
        
        /**
        * Performs selection key event handling
        */
        TKeyResponse HandleSelectionKeyL();
        
    public: // Functions from base classes

        /**
        * From MCoeForegroundObserver Prosesses foreground event.
        *
        * @since 2.0
        */   
        void HandleGainingForeground();

        /**
        * From MCoeForegroundObserver Prosesses foreground event.
        *
        * @since 2.0
        */
        void HandleLosingForeground();

        /**
        * From MEikCommandObserver Prosesses menucommands
        *
        * @since 2.0
        *
        * @param aCommandId     Commant value defined in resources.
        */
        void ProcessCommandL( TInt aCommandId );

        /**
        * From CCoeControl Handles key events
        *
        * @since 2.0
        *
        * @param aKeyEvent      TKeyEvent 
        * @param aType          TEventCode
        *
        * @return TKeyResponse depending on was is consumed or not.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
        /**
        * From MRemConCoreApiTargetObserver 
        * See remconcoreapitargetobserver.h for more information
        *
        * @since 3.0
        *
        */
        void MrccatoCommand( TRemConCoreApiOperationId aOperationId, 
                             TRemConCoreApiButtonAction aButtonAct );

        
    protected:  // Functions from base classes

        /**
        * From MEikMenuObserver Initializes items on the menu
        *
        * @since 2.0
        *
        * @param aMenuId    Current menu's resource Id
        * @param aMenuPane  Pointer to menupane
        */
        void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);

        /**
        * From CEikDialog. Handles button events and checks if the dialog 
        *   can be closed
        *
        * @since 2.0
        *
        * @param aButtonId Id of the 'pressed' button
        *
        * @return   EFalse if Options menu was pressed 
        *           ETrue otherwise
        */
        TBool OkToExitL( TInt aButtonId );

        /**
        * From CCoeControl
        *   This function is called when Help application is launched.  
        *
        * @since 2.0
        *
        * @param aContext Help context
        */
        void GetHelpContext( TCoeHelpContext& aContext) const;
        
        /**
        * From CCoeControl
        *   This function is called when control resources are changed.
        *
        * @since 2.0
        *
        * @param aType  A message UID value.
        */
        void HandleResourceChange( TInt aType );

    protected:  // New functions

        /**
        * DynInitMenuOptionsL Initializes items on the menu
        *
        * @since 2.0
        *
        * @param aMenuPane  Pointer to menupane
        */
        void DynInitMenuOptionsL(CEikMenuPane* aMenuPane);

        /**
        * DynInitMenuOptionsAtNormalEnd Initializes items on the menu
        *
        * @since 2.0
        *
        * @param aMenuPane  Pointer to menupane
        */
        void DynInitMenuOptionsAtNormalEnd(CEikMenuPane* aMenuPane);

    private:

        /**
        * Default C++ constructor.
        */
        CSmilPlayerDialog();

        /**
        * C++ constructor.
        *
        */
        CSmilPlayerDialog( TBool aPreview, TBool aAudioOn );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL( const TDesC& aBaseUrl, 
                         MMediaFactoryFileInfo* aFileInfo,
                         CMDXMLDocument* aDOM );
        
        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL( const TDesC& aBaseUrl, 
                         MMediaFactoryFileInfo* aFileInfo,
                         RFile& aFileHandle );
        
        
        /**
        * Common 2nd phase constructor shared by two overloaded ConstructL functions.
        */
        void CommonConstructL();
        
        /**
        * Called just before Draw function
        */
        void ActivateL();
        
        /**
        * Presentation states of the player
        */
        enum TSmilPlayerPresentationStates
            {
            ESmilPlayerStateNotReady = 0,
            ESmilPlayerStatePlaying,
            ESmilPlayerStatePause,
            ESmilPlayerStateStop
            };

        /**
        * Performs replay functionality.
        */
        void DoReplayL();
        
        /**
        * Changes the state of the player to given one. 
        *
        * @param aState New state
        */
        void ChangeStateL( TSmilPlayerPresentationStates aState );
        
        /**
        * Callback function to stop timer
        * This timer is used for waiting one 
        * second before getting back to the start.
        */
        static TInt DoStopTiming( TAny* aObject );

        /**
        * Callback function to stop timer
        * This timer is used for showing the error note
        * when .smil file is corrupted and player
        * could be opened.
        */
        static TInt DoClosePlayer( TAny* aObject );

        /**
        * Callback function to do volume repeat.
        * This timer is used for repeating remote control
        * server volume up/down command.
        */
        static TInt DoVolumeRepeat( TAny* aObject );
        
        /**
        * Stops the timers of the time indicator.
        * Indirectly called by the callback
        * function DoStopTiming.
        */
        void StopTiming(); // indirectly called

        /**
        * Exits the player.
        * Indirectly called by the callback
        * function DoCloseTiming.
        */
        void ClosePlayer(); // indirectly called

        /**
        * Performs one volume repeat.
        * Indirectly called by the callback
        * function DoVolumeRepeat.
        */
        void VolumeRepeat(); // indirectly called

        /**
        * Auxiliary function. Shows a information note
        * using notewrappers. 
        */  
        void ShowInformationNoteL( TInt aResourceID, TBool aWaiting );

        /** 
        * Launches help application
        */
        void LaunchHelpL() const;

        /**
        * Implements the find item command.
        * Activates the find item common component.
        */
        void DoFindItemL( TInt aCommandId );
        
        /**
        * Sets the correct player layout, when a dialog (e.g. FindUi)
        * is overlapping the presentation.
        */ 
        void LayoutOverlappedPresentation();
        
        /**
        * Sets the correct layout.
        */ 
        void LayoutPresentationL();
        
        /**
        * Returns presentation rectangle.
        */
        TRect PresentationRect() const;
        
        /**
        * Activates scrolling.
        */
        void ActivateScrollingL();
        
        /**
        * Performs key event handling
        */
        TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
        /**
        * Performs scroll key event handling
        */
        TKeyResponse HandleScrollKeyL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Performs volume change key event handling
        */
        TKeyResponse HandleVolumeKeyL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
        /**
        * Shows given menubar
        */
        void ShowMenubarL( TInt aResourceId );
        
        /**
        * Returns whether audio is enabled or not.
        */
        TBool AudioEnabled() const;
        
    private: //data

        enum TSmilPlayerFlags
            {
            EPreview                    = 0x0001,
            EAudioOn                    = 0x0002,
            EHelpSupported              = 0x0004,
            EScrollingActivated			= 0x0008,
            EFeatureSideVolumeKeys      = 0x0010
            };
		
        // Resource loader
        RConeResourceLoader iResLoader;

        // Presentation control
        CSmilPlayerPresentationController* iPresentationController;     

        // State of the presentation
        TSmilPlayerPresentationStates iPresentationState;

        // Timer
        CPeriodic* iTimer;

        // Remote control server command repeat timer.
        CPeriodic* iRCSTimer; 

        // Flags
        TInt iPlayerFlags;

        // Find item menu.
        CFindItemMenu* iFindItemMenu;
        
        // Remote control server interface selector.
        CRemConInterfaceSelector* iInterfaceSelector;
        
        // Remote control server core API.
        CRemConCoreApiTarget* iCoreTarget;
        
        // Remote control server operation to repeat.
        TRemConCoreApiOperationId iRepeatId;
    };

#endif  // SMILPLAYERDIALOG_H

// End of File
