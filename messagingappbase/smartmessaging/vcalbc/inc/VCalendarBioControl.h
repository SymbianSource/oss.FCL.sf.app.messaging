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
*     Bio control for vCalendar messages.
*
*/



#ifndef VCALENDARBIOCONTROL_H
#define VCALENDARBIOCONTROL_H

// INCLUDES
#include <msgbiocontrol.h>      // for CMsgBioControl
#include <MMsgBioControlExtension.h> // for MMsgBioControlExtension
#include <calprogresscallback.h> // for MCalProgressCallBack

// FORWARD DECLARATIONS
class CRichBio;
class CCalSession;
class CCalEntryView;
class CCalEntry;
class CAiwServiceHandler;

// CLASS DECLARATION

/**
 * Bio control for Calendar messages.
 */
NONSHARABLE_CLASS(CVCalendarBioControl) : 
    public CMsgBioControl, 
    public MCalProgressCallBack,
    public MMsgBioControlExtension,
    public MMsgBioControlScrollExtension
    {
    public:

        /**
         * Two-phased constructor
         * @param aObserver Reference to the Bio control observer.
         * @param aSession Reference to Message Server session.
         * @param aId Id of the message.
         * @param aEditorOrViewerMode Flags the new Bio control as editor or viewer.
         * @param aFile filehandle.         
         * @return The newly created object.
         */
        IMPORT_C static CMsgBioControl* NewL(
            MMsgBioControlObserver& aObserver,
            CMsvSession* aSession,
            TMsvId aId,
            TMsgBioMode aEditorOrViewerMode,
            const RFile* aFile);

         /// Destructor
        ~CVCalendarBioControl();

    public: //from MMsgBioControl

        /**
         * Calculates and sets size for a Bio control according to aSize.
         * @param aSize A reference to the suggested size and new size.
         */
        void SetAndGetSizeL(TSize& aSize);

        /**
         * Adds a menu command.
         * @param aMenuPane Reference to the CEikMenuPane of the application.
         */
        void SetMenuCommandSetL(CEikMenuPane& aMenuPane);

        /*
         * The command handler of the bio control.
         * The commands usually originate from a bio specific menu item being
         * selected by the user.
         * @param aCommand Id of command to be handled.
         */
        TBool HandleBioCommandL(TInt aCommand);

        /**
        * Returns a rectangle slice of the bio controls viewing area.
        * It is used by the CMsgEditorView class for scrolling the screen.
        * @return TRect
        */
        TRect CurrentLineRect() const;

        /**
         * This is used by the body container for managing focus and
         * scrolling.
         * @param aDirection The direction to be checked.
         */
        TBool IsFocusChangePossible(TMsgFocusDirection aDirection) const;

        /**
         * Returns the header text.
         * @return The header text.
         */
        HBufC* HeaderTextL(void) const;

        /**
         * Gives the height of the text in pixels.
         * It is used by the scrolling framework of Editor Base.
         * @return Height of the text in pixels.
         */
        TInt VirtualHeight();

        /**
         * Gives the cursor position in pixels.
         * It is used by the scrolling framework of Editor Base.
         * @return Cursor position in pixels.
         */
        TInt VirtualVisibleTop();

        /**
         * Tells whether the cursor is in the topmost or bottom position.
         * It is used by the scrolling framework.
         * @param aLocation Specifies either top or bottom.
         * @return ETrue if the cursor is in the part specified by aLocation.
         */
        TBool IsCursorLocation(TMsgCursorLocation aLocation) const;
    
    public: // from CCoeControl
    
        /**
         * Returns a bio control's a context sensitive help.
         * @param aHelpContext Help context.
         */
        void GetHelpContext(TCoeHelpContext& aHelpContext) const;
        

    protected: // from CCoeControl

        /**
         * Gives the number of component controls.
         * @return The number of component controls.
         */
        TInt CountComponentControls() const;

        /**
         * For accessing a component control.
         * @param aIndex Index of the component to be returned.
         */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
         * This is called by the CONE framework, and gives this control a
         * chance to manage the layout of its subcontrols.
         */
        void SizeChanged();

        /**
         * This is called when focus is lost or gained, and is used
         * for setting the focus of the list box.
         * @param aDrawNow Whether to draw now or not.
         */
        void FocusChanged(TDrawNow aDrawNow);

        /**
         * Sets the container window for this control. The container
         * control uses this function to set the same window for this control.
         * @return aContainer Reference to the container.
         */
        void SetContainerWindowL(const CCoeControl& aContainer);

        /**
         * A CCoeControl virtual for handling key events.
         * @param aKeyEvent The key event.
         * @param aType TEventCode
         * @return EKeyWasConsumed or EKeyWasNotConsumed
         */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType);
        
        /**
         * Handles resource change events.
         */
        void HandleResourceChange( TInt aType );
            
    public: //from MMsgBioControlExtension

        TAny* BioControlExtension( TInt aExtensionId );

    public: //from MMsgBioControlScrollExtension

        TInt ExtScrollL( TInt aPixelsToScroll, TMsgScrollDirection aDirection );        
        
        void ExtNotifyViewEvent( TMsgViewEvent aEvent, TInt aParam );

    private: //new functions

        /**
         * Parses the data and produces an Agenda entry.
         * the vCalendar file.
         */
        void ImportVCalendarL();

        /**
         * Resolve the file handle of the data file that is used as input.
         * @param aFile A reference that gets the file handle.
         */
        void ResolveFileL( RFile& aFile );

        /**
         * This is needed because the menuPane adding is done in a different
         * way in BVA than in SMS Editor/Viewer.
         * @param aMenuPane Reference to the menu pane.
         * @param aStringRes String resource ID of the command text.
         * @param aCommandOffset Offset in the Options list.
         */
        void FileBasedAddMenuItemL(CEikMenuPane& aMenuPane,
            TInt aStringRes, TInt aCommandOffset);

        /**
         * Opens a read stream. Puts a stream cleanup (closing) item on the
         * cleanup stack.
         * @param aStream The stream that is to be opened.
         * @param aFileName File name to which the stream is to be opened.
         */
        static void OpenStreamLC(RFileReadStream& aStream,
            TFileName& aFileName);
        
    private: //construction

        /**
         * The constructor.
         * @param aObserver MMsgBioControlObserver
         * @param aSession CMsvSession, the Message Server session.
         * @param aId Id of the message in Message Server.
         * @param aEditorOrViewerMode Flags the control as being either editor or viewer
         * @param aFileName *not used*
         * @param aFile file handle to vcal content.
         */
        CVCalendarBioControl(
            MMsgBioControlObserver& aObserver,
            CMsvSession* aSession,
            TMsvId aId,
            TMsgBioMode aEditorOrViewerMode,
            const RFile* aFile);

        /// Second phase constructor.
        void ConstructL();

    private: // from MCalProgressCallBack
    

        /** Progress callback.
        This calls the observing class with the percentage complete of the current operation.
        @param aPercentageCompleted The percentage complete. */
        void Progress(TInt aPercentageCompleted);
	
    	/** Progress callback.    	
    	This calls the observing class when the current operation is finished.    	
    	@param aError The error if the operation failed, or KErrNone if successful. */
    	void Completed(TInt aError);
	
	    /** Asks the observing class whether progress callbacks are required.	
	    @return If the observing class returns EFalse, then the Progress() function will not be called. */
	    TBool NotifyProgress();
	    

    private: //hidden away

        /// The default constructor is hidden away
        CVCalendarBioControl();

        /// Copy contructor prohibited.
        CVCalendarBioControl(const CVCalendarBioControl& aSource);

        /// Assignment operator prohibited.
        const CVCalendarBioControl& operator=(const CVCalendarBioControl& aSource);

    private:

        /// The viewer control
        CRichBio* iViewer;

        /// Calendar session
        CCalSession* iSession;

        /// calender entry view for saving purposes
        CCalEntryView* iCalEntryView;
        
        /// CCalEntryView initialization error code 
        TInt iCalEntryInitErrno;

        /// Contains imported calendar entries. 
        RPointerArray<CCalEntry>    iCalEntryArray;
		
		/// ETrue if BIO Control is lauched through BVA
		TBool iIsFileBased;	
		
		///  Filebased filehandle
		RFile iFileHandle;	
		
		/*
		 * @var iCalAiwServiceHandler.
		 * @brief Servicehandler to access calendaraiwprovider.
		 */
		CAiwServiceHandler* iCalAiwServiceHandler;
    };
#endif // VCALENDARBIOCONTROL_H

// End of file
