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
*		Container class for bio controls
*
*/



#ifndef MSGBIOBODYCONTROL_H
#define MSGBIOBODYCONTROL_H


// INCLUDES

#include <MsgBaseControl.h>  // for CMsgBaseControl
#include <mmsgbiocontrol.h>  // for MMsgBioControl
#include <msvstd.h>          // for TMsvId
#include <msvapi.h>
#include <MsgEditor.hrh>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CMsgBioControl;
class MMsgBioControlObserver;
class CMsgBioControlFactory;
class RFs;
class TMsgHeaderBadgeInfo;
class CGulIcon;
class CMsvSession;
class CEikMenuPane;
class CMsgBioControl;
class CMsvSession;


// CLASS DECLARATION

/**
 * The container class for bio controls, which is contained in the CMsgBody
 * class of the editor base classes.
 */
class CMsgBioBodyControl :
    public CMsgBaseControl,
    public MMsgBioBodyControl
    {

    public:

        /**
         * Two-phased constructor.
         * @param aBioControlObserver Reference to the Bio control observer.
         * @param aBioMsgType The type of Bio message.
         * @param aId Id of the message.
         * @param aIsEditor Flags the new Bio control as editor or viewer.
         * @return The newly created CMsgBioBodyControl.
         */
        IMPORT_C static CMsgBioBodyControl* NewL(
            MMsgBioControlObserver& aBioControlObserver,
            const TUid& aBioMsgType,
            TMsvId aId,
            TMsgBioMode aEditorOrViewerMode,
            CMsvSession& aSession);

        /**
         * The destructor.
         */
        IMPORT_C ~CMsgBioBodyControl();

        /**
         * From CCoeControl. Handles commands.
         * @param The command
         */
        IMPORT_C TBool HandleBioCommandL(TInt aCommand);

        /**
         * Returns the header text. (ownership transferred)
         * @return The header text.
         */
        IMPORT_C HBufC* HeaderTextL(void) const;

        /**
         * This is called by the application for allowing the Bio control to
         * add a menu item.
         * @param aMenuPane Reference to the applications menu.
         */
        IMPORT_C void SetMenuCommandSetL(CEikMenuPane& aMenuPane);

        /**
         * Gives access to the Bio Control.
         * This is needed only in special cases.
         * @return Reference to the Bio Control.
         */
        IMPORT_C CMsgBioControl& BioControl(void);

    public: // from CMsgBaseControl

        /**
         * Returns the current control rect. From CMsgBaseControl.
         * @return The current control rect.
         */
        IMPORT_C TRect CurrentLineRect();

        /**
         * From CMsgBaseControl. Calculates, sets and gives size of control.
         * Calculates and sets the size of the control and returns new size as
         * reference aSize.
         * @param aSize The size
         */
        IMPORT_C void SetAndGetSizeL(TSize& aSize);

        /**
         * From CMsgBaseControl. Handles clipboard operation.
         * @param aFunc The clipboard command to be handled.
         */
        IMPORT_C void ClipboardL(TMsgClipboardFunc aFunc);

        /**
         * From CMsgBaseControl. Handles editing operation.
         * @param aFunc The editing command to be handled.
         */
        IMPORT_C void EditL(TMsgEditFunc aFunc);

        /**
         * From CMsgBaseControl. Returns edit permission flags.
         * @return Edit permission flags
         */
        IMPORT_C TUint32 EditPermission() const;

        /**
         * From CMsgBaseControl. Checks if focus change is possible.
         * @param aDirection The direction of focus to check for.
         * @return ETrue or EFalse
         */
        IMPORT_C TBool IsFocusChangePossible(
            TMsgFocusDirection aDirection) const;

        /**
         * Checks if cursor location is on the topmost or downmost position.
         * @param aLocation Top or bottom to check for.
         * @return ETrue or EFalse
         */
        IMPORT_C TBool IsCursorLocation(TMsgCursorLocation aLocation) const;

        /**
         * Returns approximate height of the control.
         * @return ?description
         */
        IMPORT_C TInt VirtualHeight();

        /**
         * Returns a topmost visible text position.
         * @return ?description
         */
        IMPORT_C TInt VirtualVisibleTop();

        /**
         * Performs the internal scrolling of control if needed.
         * Default implementation does not perform any scrolling and returns that
         * zero pixels were scrolled.
         * @since 3.2
         * @param aPixelsToScroll Amount of pixels to scroll.
         * @param aDirection      Scrolling direction.
         * @return Amount of pixels the where scrolled. Zero value means the component cannot
         *         be scrolled to that direction anymore and view should be moved.
         */
         IMPORT_C TInt ScrollL( TInt aPixelsToScroll, TMsgScrollDirection aDirection );

        /**
         * Prepares control for viewing.
         * @since 3.2
         * @param aEvent	The event type
         * @param aParam Event related parameters
         */
        IMPORT_C void NotifyViewEvent( TMsgViewEvent aEvent, TInt aParam );

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
            
    public: // from MMsgBioBodyControl

        /**
        * 
        * @param aRequest event type.
        * @param aDelta
        */
        IMPORT_C TBool HandleBaseControlEventRequestL( 
            TMsgBioControlEventRequest aRequest,
            TInt aDelta = 0);

    protected: // from CMsgBaseControl

        /**
         * Implementation of pure virtual in CMsgBaseControl. Prepares for read
         * only or non read only state. This implementation does not do anything.
         * @param aReadOnly Prepare for read only or non read only.
         */
        void PrepareForReadOnly(TBool aReadOnly);

    protected: // from CCoeControl

        /**
         * From CCoeControl. Gives the number of sub controls.
         * @return The number of sub controls.
         */
        TInt CountComponentControls() const;

        /**
         * From CCoeControl. Returns a pointer to a certain sub control.
         * @param aIndex Index of the control to be returned.
         */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
         * From CCoeControl. Called by CONE framework.
         * Gives this control a chance to manage layout of its sub controls.
         */
         void SizeChanged();

        /**
         * Handles the change of focus.
         * @param aDrawNow
         */
        void FocusChanged(TDrawNow aDrawNow);

        /**
         * From CCoeControl. Parent sets the container window using this.
         * @param aContainer The parent control.
         */
         void SetContainerWindowL(const CCoeControl& aContainer);

    private: // construction

        /**
         * Second phase constructor.
         * @param aObserver MMsgBioControlObserver
         * @param aBioMsgType Bio Message type
         * @param aId Id of the message in Message Server.
         * @param aIsEditor Flags the control as being either editor or viewer
         */
        void ConstructL(
            MMsgBioControlObserver& aBioControlObserver,
            const TUid& aBioMsgType,
            TMsvId aId,
            TMsgBioMode aEditorOrViewerMode);

        /**
         * Constructor
         */
        CMsgBioBodyControl(CMsvSession& aSession);

    private: // not available

        /**
         * The default constructor is hidden away from outsiders
         */
        CMsgBioBodyControl();

        /**
         * Copy contructor prohibited.
         */
        CMsgBioBodyControl(const CMsgBioBodyControl& aSource);

        /**
         * Assignment operator prohibited.
         */
        const CMsgBioBodyControl& operator=(
            const CMsgBioBodyControl& aSource);

    protected:

        /**
         * Owns the factory. The factory needs to exist during the whole life
         * of the Bio Control.
         */
        CMsgBioControlFactory* iBioControlFactory;

        /**
         * Owns the Bio Control.
         */
        CMsgBioControl*     iBioControl;

    private:

        /**
         * The session is owned here.
         */
        CMsvSession&         iSession;
        };

#endif // MSGBIOBODYCONTROL_H

// End of File
