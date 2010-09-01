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
* Description:   BIO control for Ringing Tones.
*
*/



#ifndef CRINGINGTONEBIOCONTROL_H
#define CRINGINGTONEBIOCONTROL_H

//  INCLUDES
#include <msgbiocontrol.h>           // for CMsgBioControl
#include <MMsgBioControlExtension.h> // MMsgBioControlExtension;
#include <msgasynccontrolobserver.h> // observer interface for CMsgAudioControl
#include <eikcmobs.h>                // MEikCommandObserver


// FORWARD DECLARATIONS
class CEikButtonGroupContainer;
class CEikRichTextEditor;
class CMsgAudioControl;

// CLASS DECLARATION

/**
 * Bio control for Ringing Tones.
 */
class CRingingToneBioControl :
    public CMsgBioControl,
    public MEikCommandObserver,
    public MMsgAsyncControlObserver
    {

    public:  // Constructor and destructor

        /**
         * Two-phased constructor
         * @param aObserver Reference to the Bio control observer.
         * @param aSession Reference to Message Server session.
         * @param aId Id of the message.
         * @param aEditorOrViewerMode Flags the new Bio control as editor or
         *        viewer.
         * @param aFile filehandle.
         * @return The newly created object.
         */
        IMPORT_C static CMsgBioControl* NewL(
            MMsgBioControlObserver& aObserver,
            CMsvSession* aSession,
            TMsvId aId,
            TMsgBioMode aEditorOrViewerMode,
            const RFile* aFile);

        /**
        * Destructor.
        */
        ~CRingingToneBioControl();

   public: // Functions from MEikCommandObserver

        /**
        * From MEikCommandObserver
        * Handle prosess command.
        * @param aCommandId command id.
        */
        void ProcessCommandL( TInt aCommandId );

    public: // Functions from MMsgBioControl

        /**
        * From MMsgBioControl Calculates and sets size for a Bio control
        * according to aSize.
        * The height of the Bio control may be less or more than requested by
        * aSize, but the width must be exactly the same. If width of the Bio
        * control is not the same as given by aSize, the width must be reset
        * back to requested one.
        * @param aSize Size A reference to the suggested size and new size..
        */
        void SetAndGetSizeL( TSize& aSize );

        /**
        * From MMsgBioControl This is called by the container to allow the Bio
        * control to add a menu item.
        * @param aMenuPane Reference to the application's menu.
        */
        void SetMenuCommandSetL( CEikMenuPane& aMenuPane );

        /**
        * From MMsgBioControl Returns a rectangle slice of the bio controls
        * viewing area. It is used by the CMsgEditorView class for scrolling
        * the screen.
        * @return TRect to show viewing area
        */
        TRect CurrentLineRect() const;

        /**
        * From MMsgBioControl Returns true if Focus change is possible.
        * @param aDirection The direction to be checked.
        * @return ETrue if it is possible and vice versa
        */
        TBool IsFocusChangePossible( TMsgFocusDirection aDirection ) const;

        /**
        * From MMsgBioControl The container application obtains a header text
        * from the bio control.
        * @return The header text.
        */
        HBufC* HeaderTextL()  const;

        /**
        *  From MMsgBioControl The command handler.
        * The Bio Control should only handle its own commands that it has set
        * using the function SetMenuCommandSetL().
        * @param aCommand ID of command to be handled.
        * @return If the command is handled, it returns ETrue, and vice versa
        */
        TBool HandleBioCommandL(TInt aCommand);

        /**
        * The application can get the option menu recommendations using this
        * function. The function comes from MMsgBioControl. This is the
        * default implementation which returns the flags
        * KMsgBioCallBackToSender and KMsgBioCreateContactCard. Bio Controls
        * should override this if it is not ok.
        * @return The option menu permission flags. If the flag is off it
        * means that the option menu command is not recommended with this
        * Bio Control.
        */
        TUint32 OptionMenuPermissionsL() const;

        /**
        * Get the virtual height of the control's content.
        * @return virtual height in pixels.
        */
        TInt VirtualHeight();

        /**
        * Get the position of invisible cursor.
        * @return cursor position in pixels.
        */
        TInt VirtualVisibleTop();

    public: // Functions from CCoeControl

        /**
        * A CCoeControl virtual for handling key events.
        * @param aKeyEvent The key event.
        * @param aType TEventCode
        * @return EKeyWasConsumed or EKeyWasNotConsumed
        */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

        /**
        * Returns a bio control's a context sensitive help.
        * @param aHelpContext Help context.
        */
        void GetHelpContext(TCoeHelpContext& aHelpContext) const;

	public: //from MMsgAsyncControlObserver
	
		void MsgAsyncControlStateChanged( CMsgBaseControl& aControl,
                                              TMsgAsyncControlState aNewState,
                                              TMsgAsyncControlState aOldState );
                                              
		void MsgAsyncControlResourceChanged( CMsgBaseControl& aControl,
											 TInt aType );
    

		
    protected: // Functions from CCoeControl

        /**
        * From CCoeControl Handles a change to the control's resources 
		* of type aType which are shared across the environment, 
		* e.g. color scheme change, or in this case, skin change.
        * @param aType Event type
        */
		void HandleResourceChange( TInt aType );

        /**
        * From CCoeControl Gives the number of sub controls.
        * @return Count of controls be included in this component
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl Returns a pointer to a certain sub control.
        * @param aIndex Index for control
        * @return Pointer to component in question.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl This is called by the CONE framework, and gives
        * this control a chance to manage the layout of its sub controls.
        */
        void SizeChanged();

        /**
        * From CCoeControl This is called when focus is lost or gained, and
        * is used for setting the focus of the list box.
        * @param aDrawNow
        */
        void FocusChanged( TDrawNow aDrawNow );

        /**
        * From CCoeControl Sets the container window for this control. The
        * container control uses this function to set the same window for
        * this control.
        * @param aContainer container
        */
        void SetContainerWindowL( const CCoeControl& aContainer );

        /**
        * Used to draw Ringing tone icon
        */
        void Draw(const TRect& aRect) const;

		void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    private: // Constructors

        /**
         * The constructor.
         * @param aObserver MMsgBioControlObserver
         * @param aSession CMsvSession, the Message Server session.
         * @param aId Id of the message in Message Server.
         * @param aEditorOrViewerMode Flags the control as being either editor
         *        or viewer.
         * @param aFile file handle
         */
        CRingingToneBioControl(
            MMsgBioControlObserver& aObserver,
            CMsvSession* aSession,
            TMsvId aId,
            TMsgBioMode aEditorOrViewerMode,
            const RFile* aFile);

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    private:    //New functions

		/**
        * Loads a ringing tone.
        */
		void OpenFileL();
		
        /**
        * Plays ringin tone. Because support for playing OTA files are not
        * supported in WINS, command plays nothing
        */
        void PlaybackL();

        /**
        * Stops playing ringin tone. 
        */
		void StopPlayback();

        /**
        * Creates richeditor for ringing tone title.
        */
        void CreateRichEditorL();

        /**
        * add song title as a item to aItemArray
        * @param aItemArray Array for listbox
        */
        void AddSongTitleToArrayL(CDesCArray& aItemArray) const;

        /**
        * Seek if string contains back slash
        * @param aTitle Descriptor where to find back slash
        * @return ETrue if back slash was found, else EFalse
        */
        TBool IsBackSlash(TDes& aTitle);

        /**
        * Seek if string consists solely single dot or double dot
        * @param aTitle Descriptor where to find dots
        * @return ETrue if only dots are found, else EFalse
        */
        TBool IsOnlyDots(TDes& aTitle);

        /**
        * Finds out if file already exists
        * @param aTitle Filename without path or file extent
        * @return ETrue if file already exists, else EFalse
        */
        TBool ExistsL(TDes& aTitle);

        /**
        * Finds out is the filename valid for saving
        * @param aTitle Filename without path or file extent
        * @param aNotes Flag to indicate if the notes are shown
        * @return ETrue if file name is valid, else EFalse
        */
        TBool IsValidL(TDes& aTitle, TBool aNotes);

        /**
        *
        * @param aTitle Filename without path or file extent
        * @return EFalse if user pressed cancel, else ETrue
        */
        TBool AskNameL(TDes& aTitle);

         /**
        *
        * @param aTitle Filename without path or file extent
        * @return EFalse if user pressed cancel, else ETrue
        */
        TBool AskAndValidNameL(TDes& aTitle);

        /**
        * Add ringing tone to the file system
        */
        TBool SaveToneL(TDes& aTitle);

        /**
        * Called when user like to add Tone to composer.
        */
        void TrySaveToneL();

        /**
        * Creates target path for saving tone to filesystem
        * @param aName File name to be added in path
        * @return Target path
        */
        HBufC* CreatePathForSavingLC(const TDesC& aName);

        /**
        * User interactivity, ask if liked to rename saveable tone,
        * then ask the new name for tone.
        * @return true if the new name is available, otherwise false.
        */
        TBool RenameQueryL(TDes& aTitle);

        /**
        * Set Default name with increments to the iSaveTitle
        */
        void SetDefaultNameL(TDes& aName);

        /**
        * Help function to handle name query
        * @param aName ringing tone name
        * @return ETrue if it is OK, else EFalse
        */
        TBool NameQueryL(TDes& aName);

        /**
        * Rip title form ringing tone format by using composer engine
        */
        void TakeTitleL();

        /**
        * Increments numbers after file name, e.g.Ringing Tone1->Ringing Tone2
        * @param aFileName ringing tone name
        */
        void IncrementFileNameL(TDes& aFileName);

        /**
        * Find out the target path
        * @return target path
        */
        HBufC* TargetPathLC();

        /**
        * Checks is the ringing tone message in valid format. Leaves with *** if not.
        */
        void CheckMsgValidityL();

        /**
        * Loads ringing tone icon bitmap and mask, and fills transparent area with white color and
        * saves the new bitmap to temp folder.
        */
        void FillBitmapMaskL();

        /**
        * Correts CEikRichTextEditor's Font and set the alignment to centered.
        */
        void CorrectFormatToRichEditorL();

        /**
        * An information note is popped.
        * @param aStringResource The string resource id.
        */
        void InformationNoteL(TInt aStringResource);

        /**
        * Gets profile settings from shared data.
        */
        void GetAndSetRingingToneVolumeL();

    private: // Hidden away

        /**
        * C++ default constructor, hidden away from outsiders.
        */
        CRingingToneBioControl();

        /**
        * Copy-constructor is prohibited.
        */
        CRingingToneBioControl(const CRingingToneBioControl& aSource);

        /**
        * Assignment operator is prohibited.
        */
        const CRingingToneBioControl& operator=(
            const CRingingToneBioControl& aSource );

    private: // data

        ///Ringing tone name
        HBufC* iToneTitle;

        /// control for showing Ringing tone name
        CEikRichTextEditor* iRichEditor;

		/*
		* Audio player
		* Owned
		*/
		CMsgAudioControl *iAudioControl;
		
		//audio control states
		TMsgAsyncControlState iPlayerState;
		
		//Object to handle softkeys in playtime
        CEikButtonGroupContainer *iStopCba;
    };

#endif      //CRINGINGTONEBIOCONTROL_H

// End of File
