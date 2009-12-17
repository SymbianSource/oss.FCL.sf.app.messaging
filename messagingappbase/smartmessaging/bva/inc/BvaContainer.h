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
*     Container control class that will contain a Bio Control.
*
*/



#ifndef _BVACONTAINER_H_
#define _BVACONTAINER_H_

// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CMsgBioControl;
class CMsgBioControlFactory;
class CEikStatusPane;
class CAknsBasicBackgroundControlContext;

// CLASS DECLARATION

/**
 * Container control class that will contain a Bio Control.
 */
NONSHARABLE_CLASS( CBvaContainer ) :    public CCoeControl,
                                        public MEikScrollBarObserver
    {
    public: // Construction & destruction

       /**
        * Symbian OS second phase constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

        /// Destructor.
        ~CBvaContainer();

    public: // new functions

        /**
         * Loads the Bio Control.
         * @param aFile The data file handle. The Bio Control will read data
         * directly from the file.
         */
        void LoadBioControlL( const RFile& aFile );

        /**
        * Accessor for iBioControl.
        * @return BioControl.
        */
        CMsgBioControl& BioControl();

        /**
         * The header and icon of the application is set using an icon and
         * descriptor obtained from the Bio Control.
         * @param statusPane The status pane.
         */
        void UpdateHeaderAndIconFromBioControlL(CEikStatusPane& statusPane);

        /**
         * Queries the container if it has a valid BIO control set
         *
         * @since S60 3.1
         * @return ETrue if container has BIO control
         *         EFalse if not
         */
        TBool IsBioControl();

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

        void Draw(const TRect& aRect) const;

    private: // from CCoeControl

        /// Called when the size has changed.
        void SizeChanged();

        /**
         * Number of component controls.
         * @return Number of component controls.
         */
        TInt CountComponentControls() const;

       /**
        * Returns a component control.
        * @param aIndex Index of the component control.
        * @return The component control.
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

		// From CCoeControl
        TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

        /**
     	* React to skin, dynamic layout and other resource changes
     	* @param aType The type of change
     	*/
    	void HandleResourceChange(TInt aType);

    public: // from MEikScrollBarObserver

        void HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);

    private: // new functions

        /**
         * Returns the Bio Uid which identifies the type of Smart Message.
         * This function just looks at the file suffix.
         * @param aFileName File name.
         * @param aFs Handle to file server
         * @return Bio Uid.
         */
        static TUid BioUidL(const TFileName& aFileName, RFs& aFs );

		/**
         * Returns the Bio Uid which identifies the type of Smart Message.
         * This function just looks at the file suffix.
         * @param aFile File handle.
         * @return Bio Uid.
         */
        static TUid BioUidL(const RFile& aFile );

        /**
        * Creates the scroll bar.
        */
        void CreateScrollBarL();

        /**
        * Updates the scroll bar.
        */
        void UpdateScrollBarL();

        /**
        * Return virtual height and virtual Y position of the form.
        * @param aHeight
        * @param aPos
        */
        void GetVirtualFormHeightAndPos( TInt& aHeight, TInt& aPos );

         /**
        * Performs view scrolling to given direction.
        *
        * @param aPixelsToScroll Amount of pixels wanted to be scrolled.
        * @param aDirection Scrolling direction.
        * @param aMoveThumb If ETrue scroll thumb is moved at the end of scrolling.
        */
        void ScrollViewL( TInt aPixelsToScroll,
                                  TMsgScrollDirection aDirection,
                                  TBool aMoveThumb );

        /**
        * Convenience function for casting the scroll bar model.
        */
        const TAknDoubleSpanScrollBarModel* AknScrollBarModel() const;

        /**
         * Panic wrapper.
         * @param aCode The panic code.
         */
        void Panic(TInt aCode) const;

    private:

        enum TPanicCodes
            {
            EBioControlNotExist,
            EGeneral,
            EFileExtension
            };

    private: //data

        /**
        * Own: Message specific controller
        */
        CMsgBioControl*         iBioControl;

        /**
        * Own: Biocontrol factory
        */
        CMsgBioControlFactory*  iFactory;

        /**
        * Own: Background Skin
        */
        CAknsBasicBackgroundControlContext* iBgContext;

        /**
        * Own: The scroll bar
        */
        CEikScrollBarFrame* iScrollBar;

        /**
        *From MsgEditorCommons::MsgBaseLineDelta();
        */
        TInt iLineHeight;

        /**
        *From MsgEditorCommons::MsgBaseLineOffset();
        */
        TInt iBaseLineOffset;
    };

#endif // _BVACONTAINER_H_

// End of File
