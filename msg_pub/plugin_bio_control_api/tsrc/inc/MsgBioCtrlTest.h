/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    CMsgBioCtrlTest declaration
*
*/


#ifndef MSGBIOCTRLTEST_H
#define MSGBIOCTRLTEST_H

// INCLUDES
#include <msgbiocontrol.h>    // for CMsgBioControl
#include <eikmobs.h>
#include <MMsgBioControlExtension.h>

// FORWARD DECLARATIONS
#define V_HEIGHT    200
#define V_TOP       100

class CRichBio;

//class 

// CLASS DECLARATION
/**
 *  CMsgBioCtrlTest
 * 
 * Sample Bio control for API Automation
 */
NONSHARABLE_CLASS( CMsgBioCtrlTest ) :  public CMsgBioControl,
                                        public MMsgBioControlExtension,
                                        public MMsgBioControlScrollExtension
    {

    private:

        enum TBioControlApiCommands
            {
            ESetAndGetSizeL = 1,
            ESetMenuCommandSetL = 2,
            EHandleBioCommandL = 4,
            ECurrentLineRect = 8,
            EIsFocusChangePossible = 16,
            EHeaderTextL = 32,
            EOptionMenuPermissionsL = 64,
            EVirtualHeight = 128,
            EVirtualVisibleTop = 256,
            EIsCursorLocation = 512,
            EAllBioCommands = 1023
            };

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
            const RFile* aFile );

         /// Destructor
        ~CMsgBioCtrlTest();

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
         * The application can get the option menu permissions using this
         * function.
         * @return The option menu permission flags. If the flag is off it
         * means that the option menu command is not recommended with this
         * Bio Control.
         */
        TUint32 OptionMenuPermissionsL() const;

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
        
    public: //from MMsgBioControlExtension
     
        TAny* BioControlExtension( TInt aExtensionId );
     
    public: //from MMsgBioControlScrollExtension
     
        TInt ExtScrollL( TInt aPixelsToScroll, TMsgScrollDirection aDirection );
     
        void ExtNotifyViewEvent( TMsgViewEvent aEvent, TInt aParam );        
        
    public: //new functions
        
        /**
         * 
         */
        void ProtectedCallsL( TBool fileBased );
        
        /**
         * 
         */
        void CallNotifyEditorViewL();
        
        /**
         * 
         */
        void RichBioFunctionCalls();

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


    private: //construction

        /**
         * The constructor.
         * @param aObserver MMsgBioControlObserver
         * @param aSession CMsvSession, the Message Server session.
         * @param aId Id of the message in Message Server.
         * @param aEditorOrViewerMode Flags the control as being either editor or viewer
         * @param aFile filehandle.
         */
        CMsgBioCtrlTest(
            MMsgBioControlObserver& aObserver,
            CMsvSession* aSession,
            TMsvId aId,
            TMsgBioMode aEditorOrViewerMode,
            const RFile* aFile );

        /// Second phase constructor.
        void ConstructL();

    private: //private API

        void AddResultsToRichBio(TInt aLabelRes, TInt aValueRes) const;
        
        void AddItemL(TInt aLabelRes, TInt aValueRes) const;

    private:

        /// The viewer control
        CRichBio* iViewer;

        mutable TInt iHandledCommands;
    };

//Menu Observer  
class FakeObserver : public MEikMenuObserver
{
   void HandleAttemptDimmedSelectionL(TInt )
   {
       
   };
   TBool CheckHotKeyNotDimmedL(TInt )
   {
   return ETrue;   
   };
   void RestoreMenuL(CCoeControl* ,TInt ,TMenuType )
   {
       
   };
   void DynInitMenuPaneL(TInt,CEikMenuPane* )
   {
       
   };
   void DynInitMenuBarL(TInt ,CEikMenuBar* )
   {
       
   };
   void HandleSideBarMenuL(TInt ,const TPoint& ,TInt ,const CEikHotKeyTable* )
   {
       
   };
   void OfferKeyToAppL(const TKeyEvent& ,TEventCode )
   {
       
   };
   void SetEmphasis(CCoeControl *, int)
   {
       
   }
   void ProcessCommandL( int )
   {
       
   }
};

#endif // MSGBIOCTRLTEST_H
