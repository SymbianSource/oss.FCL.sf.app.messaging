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
* Description:  MsgBaseControl  declaration
*
*/



#ifndef CMSGBASECONTROL_H
#define CMSGBASECONTROL_H

// ========== INCLUDE FILES ================================

#include <eikbctrl.h>                      // for CEikBorderedControl

#include "MsgEditor.hrh"                   // for typedefs
#include "MsgEditorView.h"

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class MMsgBaseControlObserver;
class CItemFinder;

// ========== CLASS DECLARATION ============================

/**
* Defines a base class for message editor/viewer controls. All header and
* body controls must be inherited from this class.
*/
class CMsgBaseControl : public CEikBorderedControl
    {
    public:

        enum TMsgEditPermissionFlags
            {
            EMsgEditNone          = 0x00000000,    // No flags set
            EMsgEditCopy          = 0x00000001,    // Copying text to the clipboard is possible
            EMsgEditCut           = 0x00000002,    // Cutting text to the clipboard is possible
            EMsgEditPaste         = 0x00000004,    // Pasting text from the clipboard is possible
            EMsgEditSelectAll     = 0x00000008,    // Select all is possible
            EMsgEditUnSelectAll   = 0x00000010,    // Unselect all is possible
            EMsgEditUndo          = 0x00000020,    // Undo is possible
            EMsgFirstReservedFlag = 0x00001000,    // Reserved for standard controls
            EMsgFirstFreeFlag     = 0x00010000,    // The first free flag for derived classes
            EMsgEditAll           = 0xFFFFFFFF     // All flags set
            };

        enum TMsgViewEventFlags
            {
            EMsgViewEventNone          = 0x00000000,    // No flags set
            EMsgViewEventAutoHighlight = 0x00000001     // Autohighlight enabled for the control
            };

    public:

        /**
        * Constructor.
        * @param aBaseControlObserver
        */
        IMPORT_C CMsgBaseControl( MMsgBaseControlObserver& aBaseControlObserver );

        /**
        * Constructor.
        */
        IMPORT_C CMsgBaseControl();

        /**
        * Destructor.
        */
        IMPORT_C ~CMsgBaseControl();

        /**
        * Creates this control from resource.
        * @param aResourceId
        */
        IMPORT_C virtual void ConstructFromResourceL( TInt aResourceId );

        /**
        * Prepares control for viewing.
        * @param aEvent
        * @param aParam
        */
        IMPORT_C virtual void NotifyViewEvent( TMsgViewEvent aEvent, TInt aParam );

        /**
        * Resets (= clears) contents of the control.
        */
        IMPORT_C virtual void Reset();

        /**
        * Sets the modified flag of the control.
        * @param aModified
        */
        IMPORT_C virtual void SetModified( TBool aModified );

        /**
        * Return ETrue if contents of control has been modified.
        * @return
        */
        IMPORT_C virtual TBool IsModified() const;

        /**
        * Sets the control read only or not.
        * @param aReadOnly
        */
        IMPORT_C void SetReadOnly( TBool aReadOnly );

        /**
        * Returns the read only status of the control.
        * @return
        */
        IMPORT_C TBool IsReadOnly() const;

        /**
        * Returns the control id.
        * @return
        */
        IMPORT_C TInt ControlId() const;

        /**
        * Sets the control id.
        * @param aControlId
        */
        IMPORT_C void SetControlId( TInt aControlId );

        /**
        * Returns a distance from above to this control in pixels.
        * @return
        */
        IMPORT_C TInt DistanceFromComponentAbove() const;

        /**
        * Sets a distance from above control in pixels.
        * @param aDistance
        */
        IMPORT_C void SetDistanceFromComponentAbove( TInt aDistance );

        /**
        * Returns the margins of the control.
        * @return
        */
        IMPORT_C TMargins Margins() const;

        /**
        * Sets margins for the control.
        * @param aMargins
        */
        IMPORT_C void SetMargins( const TMargins& aMargins );

        /**
        * Sets base control observer.
        * @param aBaseControlObserver
        */
        IMPORT_C virtual void SetBaseControlObserver(
            MMsgBaseControlObserver& aBaseControlObserver );

        /**
        * Returns approximate height of the control.
        * @return
        */
        IMPORT_C virtual TInt VirtualHeight();

        /**
        * Returns a topmost visible text position.
        * @return
        */
        IMPORT_C virtual TInt VirtualVisibleTop();

        /**
        * Returns type of control.
        * @return
        */
        inline TInt ControlType() const;

        /**
        * Sets type of control.
        * @param aType
        */
        inline void SetControlType( TInt aType );

        /**
        * Returns control mode flags.
        * @return
        */
        inline TUint32 ControlModeFlags() const;

        /**
        * Sets control mode flags.
        * @param aFlags
        */
        inline void SetControlModeFlags ( TUint32 aFlags );

        /**
        * Default implementation returns NULL 
        */
        IMPORT_C virtual CItemFinder* ItemFinder();

        /**
        * Should be called when focus is changed to body. 
        * Note: this default implementation is empty
        * @param aBeginning: ETrue if search started from the beginning of the body text, EFalse if from the end
        */
        IMPORT_C virtual void SetupAutomaticFindAfterFocusChangeL( TBool aBeginning );
        
        /**
        * Performs the internal scrolling of control if needed. 
        * Default implementation does not perform any scrolling and returns that
        * zero pixels where scrolled.
        *
        * @since 3.2
        * 
        * @param aPixelsToScroll Amount of pixels to scroll.
        * @param aDirection      Scrolling direction.
        *
        * @return Amount of pixels the where scrolled. Zero value means the component cannot
        *         be scrolled to that direction anymore and view should be moved.
        */
        IMPORT_C virtual TInt ScrollL( TInt aPixelsToScroll, TMsgScrollDirection aDirection );        
        
    public:  // pure virtuals

        /**
        * Returns the current control rect.
        * @return
        */
        virtual TRect CurrentLineRect() = 0;

        /**
        * Calculates and sets the size of the control and returns new size as
        * reference aSize.
        * @param aSize
        */
        virtual void SetAndGetSizeL( TSize& aSize ) = 0;

        /**
        * Handles clipboard operation.
        * @param aFunc
        */
        virtual void ClipboardL( TMsgClipboardFunc aFunc ) = 0;

        /**
        * Handles editing operation.
        * @param aFunc
        */
        virtual void EditL( TMsgEditFunc aFunc ) = 0;

        /**
        * Checks if the focus change up or down is possible and returns ETrue
        * if it is.
        * @param aDirection
        * @return
        */
        virtual TBool IsFocusChangePossible( TMsgFocusDirection aDirection ) const = 0;

        /**
        * Checks if the cursor location is on the topmost or downmost position and
        * returns ETrue if it is.
        * @param aLocation
        * @return
        */
        virtual TBool IsCursorLocation( TMsgCursorLocation aLocation ) const = 0;

        /**
        * Returns edit permission flags.
        * @return
        */
        virtual TUint32 EditPermission() const = 0;
    
    public: // From CCoeControl
    
        /**
        * For handling dynamic layout switch.
        */
        IMPORT_C void HandleResourceChange( TInt aType );
        
    protected: // pure virtuals

        /**
        * Prepares for read only or non read only state.
        * @param aReadOnly
        */
        virtual void PrepareForReadOnly( TBool aReadOnly ) = 0;

    protected:

        /**
        * Sets default margins for the control.
        */
        IMPORT_C void BaseConstructL();

        /**
        * Returns a pointer to the normal text font.
        * @return
        */
        IMPORT_C const CFont* NormalTextFontL() const;

        /**
        * Returns a pointer to the normal label font.
        * @return
        */
        IMPORT_C const CFont* LabelFontL() const;

    protected:  // from CCoeControl

        /**
        * Returns a number of controls.
        * @return
        */
        IMPORT_C TInt CountComponentControls() const;

        /**
        * Returns a control of index aIndex.
        * @param aIndex
        * @return
        */
        IMPORT_C CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * This function must be implemented in derived control and it must set
        * positions for its controls using SetExtent function.
        *
        * SetExtent function must be used for setting positions because when this
        * is called the first time, positions are not set yet. Hence, SetPosition
        * cannot be used because it assumes that positions are already set.
        */
        IMPORT_C void SizeChanged();

        /**
        * This is called when the focus of the control is changed.
        * @param aDrawNow
        */
        IMPORT_C void FocusChanged( TDrawNow aDrawNow );
        
    protected:

        MMsgBaseControlObserver* iBaseControlObserver;
        TInt                     iControlId;
        TInt                     iDistanceFromComponentAbove;
        TMargins                 iMargins;
        TUint32                  iControlModeFlags;
        TInt                     iControlType;
        TInt                     iLineHeight;
        TInt                     iMaxBodyHeight;
    
    private:
    
        TInt                    iReserved;
    };

#include <MsgBaseControl.inl>

#endif // CMSGBASECONTROL_H

// End of File
