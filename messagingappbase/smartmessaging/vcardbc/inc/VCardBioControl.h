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
* Description:   Bio control for Business Cards
*
*/



#ifndef VCARDBIOCONTROL_H
#define VCARDBIOCONTROL_H

// INCLUDES
#include <msgbiocontrol.h>    // for CMsgBioControl
#include <MMsgBioControlExtension.h> // MMsgBioControlExtension
#include "vcardbiocontrol.hrh"

// FORWARD DECLARATIONS

class CRichBio;
class CVCardVpbUtil;
class MVPbkStoreContactField;
class MVPbkStoreContact;

// CLASS DECLARATION

/**
 * Bio control for vCards and Compact Business Cards.
 */
NONSHARABLE_CLASS(CVCardBioControl): 	public CMsgBioControl,
                                        public MMsgBioControlExtension,
                                        public MMsgBioControlScrollExtension
    {
    public: // construction & destruction

        /**
         * Two-phased constructor
         * @param aObserver Reference to the Bio control observer.
         * @param aSession Reference to Message Server session.
         * @param aId Id of the message.
         * @param aEditorOrViewerMode Flags the new Bio control as editor or
         *        viewer.
         * @param aFileName A reference to a file name of file based bio
         *        control.
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
        ~CVCardBioControl();

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
         * Any command can be given as an argument, but only Bio specific
         * commands should be handled. Other commands should be ignored by
         * this method.
         * @param aCommand Id of command to be handled.
         */
        TBool HandleBioCommandL(TInt aCommand);

        /**
        * Returns a rectangle slice of the bio controls viewing area.
        * It is used by the CMsgEditorView class for scrolling the screen.
        * The position is given relative to the bio controls top left
        * corner.
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
         * It is used by the scrolling framework.
         * @return Height of the text in pixels.
         */
        TInt VirtualHeight();

        /**
         * Gives the cursor position in pixels.
         * It is used by the scrolling framework.
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
         * A CCoeControl virtual for handling key events.
         * @param aKeyEvent The key event.
         * @param aType TEventCode
         * @return EKeyWasConsumed or EKeyWasNotConsumed
         */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType);

        /**
         * Returns a control's a context sensitive help.
         * @param aHelpContext Help context.
         */
        void GetHelpContext(TCoeHelpContext& aHelpContext) const;

        /**
         * Sets a bio control dimmed.
         * @param aDimmed ETrue to dim the control,
         * EFalse to set the control as not dimmed.
         */
        void SetDimmed(TBool aDimmed);

    public: //from MMsgBioControlExtension

        TAny* BioControlExtension( TInt aExtensionId );

    public: //from MMsgBioControlScrollExtension

        TInt ExtScrollL( TInt aPixelsToScroll, TMsgScrollDirection aDirection );

        void ExtNotifyViewEvent( TMsgViewEvent aEvent, TInt aParam );

    protected: // from CCoeControl

        /**
         * Gives the number of component controls.
         * @return The number of component controls.
         */
        TInt CountComponentControls() const;

        /**
         * For accessing a component control.
         * @param aIndex Index of the control to be returned.
         */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
         * This is called by the CONE framework, and gives this control a
         * chance to manage the layout of its sub controls.
         */
        void SizeChanged();

        /**
         * This is called when focus is lost or gained, and is used
         * for setting the focus of the list box.
         * @param aDrawNow Whether to draw now.
         */
        void FocusChanged(TDrawNow aDrawNow);

        /**
         * Sets the container window for this control. The container
         * control uses this function to set the same window for this control.
         * @param aContainer Reference to the container.
         */
        void SetContainerWindowL(const CCoeControl& aContainer);

        /**
         * Handles resource change events.
         */
        void HandleResourceChange(TInt aType);
        
    private: // enums

        /// These are the menu commands for this bio control.
        enum TMenuCommand
            {
            EAddToPhonebook = 0
            };

    private: // construction

        /**
         * The constructor.
         * @param aObserver MMsgBioControlObserver
         * @param aSession CMsvSession, the Message Server session.
         * @param aId Id of the message in Message Server.
         * @param aEditorOrViewerMode Flags the control as being either editor
         *        or viewer.
         * @param aFileName not used
         * @param aFile file handle
         */
        CVCardBioControl(
            MMsgBioControlObserver& aObserver,
            CMsvSession* aSession,
            TMsvId aId,
            TMsgBioMode aEditorOrViewerMode,
            const RFile* aFile);

        /// Second phase constructor.
        void ConstructL();

    private: // new functions

        /**
         * Adds the vCard (phonebook contact item) to Phonebook.
         */
        void AddToPhoneBookL();

        /**
         * Resolves the file handle of the input data file. It also finds
         * out if the data is a vCard or a Compact Business card.
         * @param aFile Reference that gets the file handle.
         * @param aBCType Reference that gets the type of business card bio
         *        control.
         */
        void ResolveFileHandleAndTypeL(
            RFile& aFile,
            TVCardBCBusinessCardType& aBCType);

        /**
         * Adds the CVPbkContactItem field text to the viewer, including the
         * labels.
         * @param aContact Reference to the Pbk contact.
         */
        void AddVPbkFieldsToViewerL(const MVPbkStoreContact& aContact);

        /**
         * Checks if the field is to be shown or not.
         * @param aField Reference to the contact item.
         * @return ETrue if the field is to be shown.
         */
        static TBool FieldToBeShown(const MVPbkStoreContactField& aField);

        /**
         * This is needed because the menuPane adding is done in a different
         * way in BVA than in SMS Editor/Viewer.
         * @param aMenuPane Reference to menu pane
         * @param aStringRes String resource id.
         * @param aCommandOffset Offset of comand id from "first free command"
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

        /**
        * Panics the application.
        * Calls User::Panic with the classname and class-specific
        * errorcode.
        * @param aReason Class-specific error code telling what went wrong.
        */
        static void Panic(TInt aReason);

    private: // hidden away

        /// The default constructor is prohibited
        CVCardBioControl();

        /// Copy contructor prohibited.
        CVCardBioControl(const CVCardBioControl& aSource);

        /// Assignment operator prohibited.
        const CVCardBioControl& operator=(const CVCardBioControl& aSource);

    private:

        /// The Viewer component.
        CRichBio* iViewer;

        // own
        RFile iFileHandle;

        /// A Phonebook contact item is owned here.
        MVPbkStoreContact* iVpbContact;

        /// ETrue if BIO Control is lauched through BVA
        TBool iIsFileBased;

        CVCardVpbUtil* iVpbUtil;
    };
#endif // VCARDBIOCONTROL_H
// end of file
