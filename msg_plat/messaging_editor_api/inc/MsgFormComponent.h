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
* Description:  MsgFormComponent  declaration
*
*/



#ifndef CMSGFORMCOMPONENT_H
#define CMSGFORMCOMPONENT_H

// ========== INCLUDE FILES ================================

#include <eikbctrl.h>                      // for CEikBorderedControl

#include "MsgEditor.hrh"                   // for typedefs

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgControlArray;
class CMsgBaseControl;

// ========== CLASS DECLARATION ============================

/**
* Defines a base class for message editor/viewer form component
* (header or body).
*
*/
class CMsgFormComponent : public CEikBorderedControl
    {
    public:

        /**
        * Constructor.
        * @param aMargins
        */
        CMsgFormComponent( const TMargins& aMargins );

        /**
        * Destructor.
        */
        virtual ~CMsgFormComponent();

        /**
        *
        * @param aParent
        */
        void BaseConstructL( const CCoeControl& aParent );

        /**
        * Return a pointer to a control by given control id aControlId. If the
        * control cannot be found returns NULL.
        * @param aControlId
        * @return
        */
        CMsgBaseControl* Component( TInt aControlId ) const;

        /**
        * Returns control's array index by given control id aControlId.
        * @param aControlId
        * @return
        */
        TInt ComponentIndexFromId( TInt aControlId ) const;
        
        /**
        * Returns control which is located at given position.
        *
        * @since 3.2
        *
        * @param aPosition          Given position.
        * @param aEvaluateHitText   Indicates whether HitTest evaluation should be used or not.
        *
        * @return Control located at given position or NULL if no control was found.
        */
        CMsgBaseControl* ControlFromPosition( TPoint aPosition, TBool aEvaluateHitTest ) const;
        
        /**
        * Adds a control aControl to the control array to position aIndex
        * and sets control id for the control.
        * @param aControl
        * @param aControlId
        * @param aIndex
        */
        void AddControlL( CMsgBaseControl* aControl, TInt aControlId, TInt aIndex = 0);

        /**
        * Removes a control from array by given control id aControlId and returns
        * pointer to it. If the control cannot be found retuns NULL.
        * @param aControlId
        * @return
        */
        CMsgBaseControl* RemoveControlL( TInt aControlId );

        /**
        * Returns margins.
        * @return
        */
        TMargins Margins() const;

        /**
        * Returns the current control rect.
        * @return
        */
        TRect CurrentLineRect();

        /**
        * Changes focus to a control whose index is aNewFocus. Returns EFalse if
        * focus cannot be changed.
        * @param aNewFocus
        * @return
        */
        TBool ChangeFocusTo( TInt aNewFocus, TDrawNow aDrawNow = EDrawNow );

        /**
        * Returns a pointer to the focused control.
        * @return
        */
        CMsgBaseControl* FocusedControl() const;

        /**
        * Return index of first focusable control
        * @param aStart     start index to search.
        * @param aDirection direction to search.
        */
        TInt FirstFocusableControl( TInt aStart, TMsgFocusDirection aDirection );

        /**
        * Returns index of currently focused control.
        * @return
        */
        TInt CurrentFocus() const;

        /**
        * Resets (= clears) controls from this component.
        */
        void ResetControls();

        /**
        * Return virtual height of components.
        */
        TInt VirtualHeight();

        /**
        * Returns total of pixels above each control' band. This function is
        * needed for the scroll bar for estimating message form's position
        * relative to screen.
        */
        TInt VirtualExtension();

        /**
        *
        * @param aEvent
        * @param aParam
        */
        virtual void NotifyControlsForEvent( TMsgViewEvent aEvent, TInt aParam );

        /**
        * Returns a number of MsgBaseControl based controls
        * @return
        */
        TInt CountMsgControls() const;
        
        /**
        * Returns a MsgBaseControl based control of index aIndex.
        * @param aIndex
        * @return
        */
        CMsgBaseControl* MsgControl( TInt aIndex ) const;
        
        /**
        * Returns the height of text font in controls (editor etc.)
        * @return font height
        */
        TInt TextFontHeight() const { return iTextFontHeight; }
        
        /**
        * Record font height from current layout for editor. 
        */
        void GetTextFontHeight();

    public:  // pure virtuals

        /**
        * Calculates and sets the size for the control and returns new size as
        * reference aSize. If aInit == ETrue, sets also size for the controls
        * by calling their SetAndGetSizeL functions.
        *
        * @param aSize
        * @param aInit
        */
        virtual void SetAndGetSizeL( TSize& aSize, TBool aInit ) = 0;

    public:  // from CCoeControl

        /**
        * Handles key events.
        * @param aKeyEvent
        * @param aType
        * @return
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Returns a number of controls.
        * @return
        */
        TInt CountComponentControls() const;

        /**
        * Returns a control of index aIndex.
        * @param aIndex
        * @return
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

    protected:  // from CCoeControl

        /**
        * Sets new position for all the controls.
        */
        void SizeChanged();
        
        /**
        * From CCoeControl Handles a resource relative event
        */
        void HandleResourceChange( TInt aType );
        
        /**
        * This is called when the focus of the control is changed.
        * @param aDrawNow
        */
        void FocusChanged( TDrawNow aDrawNow );

    private:

        /**
        * Adds control aControl to the component array to position aIndex.
        * If aIndex = the number components in the array or EMsgAppendControl,
        * appends the control to end of the array. Panics is aIndex is incorrect.
        * @param aControl
        * @param aIndex
        * @return
        */
        TInt DoAddControlL( CMsgBaseControl* aControl, TInt aIndex );

    private:

        /**
        * Constructor (not available).
        */
        CMsgFormComponent();

    protected:

        CMsgControlArray* iControls;     // control arrays
        TInt              iCurrentFocus;
        TMargins          iMargins;
        TInt              iTextFontHeight;

    };

#endif // CMSGFORMCOMPONENT_H

// End of File
