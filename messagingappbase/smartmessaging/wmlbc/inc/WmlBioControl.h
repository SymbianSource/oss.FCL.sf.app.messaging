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
*     Bio control for Wap bookmarks and access points.
*
*/



#ifndef CWMLBIOCONTROL_H
#define CWMLBIOCONTROL_H

// INCLUDES
#include "WMLBC.hrh"

#include <msgbiocontrol.h>          // CMsgBioControl
#include <MMsgBioControlExtension.h> // MMsgBioControlExtension;

// FORWARD DECLARATIONS

class RMsvReadStream;
class CParsedField;
class CRichBio;
class CWmlStateFactory;
class CWmlItemEngine;

// CONSTANTS

// CLASS DECLARATION

/**
* Bio Control for viewing WAP accesspoints,
* MMS settings and bookmarks.
*/
NONSHARABLE_CLASS(CWmlBioControl):  public CMsgBioControl,
                                    public MMsgBioControlExtension,
                                    public MMsgBioControlScrollExtension
    {

    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        *
        * <b>IMPORTANT NOTICE:</b>
        * This signature must be EXACTLY THIS as
        * CMsgBioControlFactory::DoCreateControlL uses this function and
        * <b>EXACTLY THIS SIGNATURE</b> to create the control.
        *
        * @param aObserver Reference to the owning editor (normally), so that
        *                  control can communicate with it when handling (menu)
        *                  commands.
        * @param aSession Pointer to session in Message Server.
        *                 Cannot be NULL for CWmlBioControl.
        * @param aId Message Server entry ID.
        * @param aIsEditor Whether the control is part of editor or viewer.
        * @param aFile    Handle to the file resource, not used by WML
        * @return Pointer to the created CWMLBioControl object.
        * @exception Leaves if aSession is NULL
        */
        IMPORT_C static CMsgBioControl* NewL(
            MMsgBioControlObserver& aObserver,
            CMsvSession* aSession,
            TMsvId aId,
            TMsgBioMode aIsEditor,
            const RFile* aFile);

        /**
        * Destructor.
        */
        ~CWmlBioControl();

    public: // from MMsgBioControl

        /**
        * Calculates and sets size for a Bio control according to aSize.
        * The height of the Bio control may be less or more than requested by
        * aSize, but the width must be exactly the same. If width of the Bio
        * control is not the same as given by aSize, the width must be reset
        * back to requested one.
        * @param aSize Size. Set the desired to to the aSize.
        */
        void SetAndGetSizeL( TSize& aSize );

        /**
        * This is called by the container to allow the Bio control to add
        * a menu item.
        * @param aMenuPane Reference to the applications menu.
        */
        void SetMenuCommandSetL( CEikMenuPane& aMenuPane );

        /**
        * The command handler. The Bio Control should only handle its own
        * commands that it has set using the function SetMenuCommandSetL().
        * @param aCommand ID of command to be handled.
        * @return If the command is handled, it returns ETrue, and vice versa
        */
        TBool HandleBioCommandL(TInt aCommand);

        /**
        * Returns a rectangle slice of the bio controls viewing area.
        * It is used by the CMsgEditorView class for scrolling the screen.
        * @return The position relative to the bio control's top left corner
        */
        TRect CurrentLineRect() const;

        /**
        * Returns true if Focus change is possible down or up from this
        * from this control.
        * @param aDirection whether to query possibility to go up or down.
        * @return possibility to change the focus, ETrue or EFalse.
        */
        TBool IsFocusChangePossible( TMsgFocusDirection aDirection ) const;

        /**
        * The container application obtains a header text from the
        * bio control.
        * @return Pointer to the buffer containing the header text.
        *         Ownership is passed to the container.
        */
        HBufC* HeaderTextL() const;

        /**
        * The application can get the option menu permissions using this
        * function.
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
        * Get the position of invinsible cursor.
        * @return cursor position in pixels.
        */
        TInt VirtualVisibleTop();
        
        /**
        * Check if the cursor location is desired.
        * @param aLocation Desired cursor location.
        * @return ETrue if the cursor location is desired,
        * EFalse otherwise.
        */
        TBool IsCursorLocation(TMsgCursorLocation aLocation) const;

    public: // from CCoeControl

        /**
        * Forwards key event to the correct child control.
        * @param aKeyEvent event from the parent.
        * @param aType event type.
        * @return EKeyWasConsumed if key was used, EKeyWasNotConsumed if not.
        */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );
    
        /**
        * Returns a control's a context sensitive help.
        * @param aHelpContext Help context.
        */
        void GetHelpContext(TCoeHelpContext& aHelpContext) const;

        /**
        * Handles resource change events.
        */
        void HandleResourceChange( TInt aType );
        
    public: //from MMsgBioControlExtension

        TAny* BioControlExtension( TInt aExtensionId );

    public: //from MMsgBioControlScrollExtension

        TInt ExtScrollL( TInt aPixelsToScroll, TMsgScrollDirection aDirection );        
        
        void ExtNotifyViewEvent( TMsgViewEvent aEvent, TInt aParam );        

    protected: // from CCoeControl

        /**
        * Get the count of the child controls.
        * @return Count of controls in this component.
        */
        TInt CountComponentControls() const;

        /**
        * Get the pointer to the specified child control.
        * @param aIndex index of desired control.
        * @return pointer to component in question.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * Called when size is changed.
        */
        void SizeChanged();
        
        /**
        * Called when focus is changed.
        * @param aDrawNow Flag for immediate update of screen (ignored).
        */
        void FocusChanged( TDrawNow aDrawNow );

        /**
        * Called when the component is created to set the parent window
        * component.
        * @param aContainer parent control.
        */
        void SetContainerWindowL( const CCoeControl& aContainer );
        
    private: // New

        /**
        * Constructor, hidden away from outsiders.
        */
        CWmlBioControl();

        /**
        * ConstuctL creates all the instances needed.
        */
        void ConstructL();

        /**
        * Constructor sets up member variables.
        * @param aObserver Reference to the owning editor (normally), so that
        *                  control can communicate with it when handling (menu)
        *                  commands.
        * @param aSession Pointer to session in Message Server.
        *                 Cannot be NULL for CWmlBioControl.
        * @param aId Message Server entry ID.
        * @param aIsEditor Whether the control is part of editor or viewer.
        * @param aFileName Name of the file if control is file based. Not used
        *                  for WML.
        */
        CWmlBioControl(
            MMsgBioControlObserver& aObserver,
            CMsvSession* aSession,
            TMsvId aId,
            TMsgBioMode aIsEditor,
            const RFile* aFile);
            
        /**
        * Copy constructor
        * NOT USED, hidden from outsiders
        */
        CWmlBioControl( const CWmlBioControl& aSource );

        /**
        * Operator =
        * NOT USED, hidden from outsiders
        */
        const CWmlBioControl& operator=( const CWmlBioControl& aSource );

    private: // New

        /**
        * Read the data from the store.
        * @param aId Message server entry id.
        * @return a pointer to the array containing read obejcts.
        */
        CArrayPtrSeg<CParsedField>* ReadStoreLC( 
            const TMsvId aId );

        /**
        * Read WAP Parser output into the iParsedFieldArray.
        * @param aReadStream reference to the opened readstream containing data.
        * @return pointer to the array containing the read objects.
        */
        static CArrayPtrSeg<CParsedField>* InternalizeFieldDataL( 
            RMsvReadStream& aReadStream );

        /**
        * Updates the header text if necessary.
        */
        void UpdateHeaderTextL() const;
        
    protected: // Data

        /// Richbio for displaying the received WAP APs/bookmarks.
        CRichBio* iRichBio;

        /// State factory object, owned.
        CWmlStateFactory* iStateFactory;
       
    private:

        /// Pointer to the CWmlItemEngine.
        CWmlItemEngine* iItemEngine;

    protected: // friend declarations

        /** 
        * This class contains multiple states and 
        * state pattern is used to control the transitions
        * between the states. States are defined friend classes
        * so that it's not needed to export the interface needed
        * to access all necessary information.
        */
        friend class CWmlStateFactory;
        friend class CWmlState;
        friend class CWmlStateSingle;
    };

#endif // CWMLBIOCONTROL_H

// End of file
