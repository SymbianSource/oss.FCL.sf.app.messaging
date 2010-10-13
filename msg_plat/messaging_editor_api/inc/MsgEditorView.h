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
* Description:  MsgEditorView  declaration
*
*/



#ifndef CMSGEDITORVIEW_H
#define CMSGEDITORVIEW_H

// ========== INCLUDE FILES ================================

#include <coecntrl.h>      // for CCoeControl
#include <aknenv.h>        // for CAknEnv
#include <ItemFinder.h>
#include <ConeResLoader.h>

#include "MsgBaseControlObserver.h"        // for MMsgBaseControlObserver
#include "MsgEditorObserver.h"             // for MMsgEditorObserver
#include "MsgEditor.hrh"                   // for typedefs

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class MMsgEditorObserver;
class CMsgBaseControl;
class CMsgHeader;
class CMsgBody;
class CMsgFormComponent;
class CEikScrollBarFrame;
class CAknsBasicBackgroundControlContext;

// ========== CLASS DECLARATION ============================

/**
* Defines an API for message editor/viewer view.
*
* @lib MsgEditor.lib
* @since S60 v2.0
*/
class CMsgEditorView : public CCoeControl,
                       public MMsgBaseControlObserver,
                       public MCoeControlObserver,
                       public MEikScrollBarObserver
    {   
    public:

        enum TMsgEditorMode
            {
            EMsgEditorModeNone             = 0x00,
            EMsgEditorModeNormal           = 0x01,
            EMsgReadOnly                   = 0x02,
            EMsgDoNotUseDefaultBodyControl = 0x04
            };

    public:

        /**
        * Factory method.
        * @param aObserver
        * @param aEditorModeFlags
        * @return
        */
        IMPORT_C static CMsgEditorView* NewL( MMsgEditorObserver& aObserver,
                                              TUint32 aEditorModeFlags );

        /**
        * from MObjectProvider
        */
        IMPORT_C MObjectProvider* MopNext();

        /**
        * Destructor.
        */
        IMPORT_C ~CMsgEditorView();

        /**
        * Prepares the editor view for showing it on the screen.
        * @param aRect
        * @param aControlIdForFocus
        */
        IMPORT_C void ExecuteL( const TRect& aRect, TInt aControlIdForFocus );

        /**
        * Finds a control from the header and the body by id and returns a
        * pointer to it. If a control cannot be found, returns NULL.
        * @param aControlId
        * @return
        */
        IMPORT_C CMsgBaseControl* ControlById( TInt aControlId ) const;

        /**
        * Returns a pointer to a focused control. If no control is focused,
        * returns NULL.
        * @return
        */
        IMPORT_C CMsgBaseControl* FocusedControl() const;

        /**
        * Sets focus to a control aControlId.
        * @param aControlId
        */
        IMPORT_C void SetFocus( TInt aControlId );

        /**
        * Constructs a control of type aControlType from resource and adds it
        * to a form component aFormComponent. The control is added to a
        * position aIndex.
        *
        * Leaves with KErrNotFound if aControlType is incorrect.
        *
        * @param aResourceId
        * @param aControlType
        * @param aIndex
        * @param aFormComponent
        * @return
        */
        IMPORT_C TInt AddControlFromResourceL( TInt aResourceId,
                                               TInt aControlType,
                                               TInt aIndex,
                                               TMsgFormComponent aFormComponent );

        /**
        * Adds a control given by aControl to a form component aFormComponent.
        * The control is added to a position aIndex with control id aControlId.
        *
        * @param aControl
        * @param aControlId
        * @param aIndex
        * @param aFormComponent
        */
        IMPORT_C void AddControlL( CMsgBaseControl* aControl,
                                   TInt aControlId,
                                   TInt aIndex,
                                   TMsgFormComponent aFormComponent );

        /**
        * Removes the control aControlId from the header or the body and returns
        * pointer to it. If a control cannot be found, returns NULL.
        *
        * @param aControlId
        * @return
        */
        IMPORT_C CMsgBaseControl* RemoveControlL( TInt aControlId );
        
        /**
        * Removes and deletes the control aControlId from the header or the body if found.
        *
        * @param aControlId Control ID of the deleted control.
        */
        IMPORT_C void DeleteControlL( TInt aControlId );
        
        /**
        * Returns a reference to a form component.
        * @param aFormComponent
        * @return
        */
        IMPORT_C CCoeControl& FormComponent( TMsgFormComponent aFormComponent ) const;

        /**
        * Prepares the editor view for viewing it on the screen after screen
        * size change.
        * @param aRect
        */
        IMPORT_C void HandleScreenSizeChangeL( const TRect& aRect );

        /**
        * Resets (= clears) all controls.
        */
        IMPORT_C void ResetControls();

        /**
        * Resets (= clears) controls from header or body.
        * @param aFormComponent EMsgHeader or EMsgBody
        */
        IMPORT_C void ResetControls( TMsgFormComponent aFormComponent );

        /**
        * Checks if any control owned by view is modified.
        * @return ETrue if modified, EFalse if not.
        */
        IMPORT_C TBool IsAnyControlModified() const;

        /**
        * Updates modified flag of controls owned by view.
        * Call this with EFalse when you have populated controls to reset the
        * modifed flag.
        * @param aFlag ETrue or EFalse
        */
        IMPORT_C void SetControlsModified( TBool aFlag );

        inline void SetScrollParts( TInt aScrollParts );
        inline void SetCurrentPart( TInt aCurrentPart );
        
        /**
        * Returns used item finder pointer if any.
        *
        * @return CItemFinder pointer if item finder is used.
        *         NULL otherwise
        */        
        IMPORT_C CItemFinder* ItemFinder();
        
        /**
        * Sets edwin observer.
        * Edwin observer is added to every text editor control that currently exists
        * or is added after this call to the view.
        * 
        * aObserver IN Edwin observer. 
        *              Setting observer to NULL will remove the edwin observer.
        */
        IMPORT_C void SetEdwinObserverL( MEikEdwinObserver* aObserver );
        
    public:   // from CCoeControl

        /**
        * Handles key events.
        * @param aKeyEvent
        * @param aType
        * @return
        */
        IMPORT_C TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode       aType);

        /**
        * For handling dynamic layout switch.
        */
        void HandleResourceChange( TInt aType );
        
        /**
        * From CCoeControl. See coecntrl.h
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    public:  // from MMsgBaseControlObserver
        
        /**
        * Returns whether view is intialized.
        */
        TBool ViewInitialized() const;
        
        /**
        * Returns the current view rectangle.
        * @return
        */
        TRect ViewRect() const;
        
        /**
        * Handles an event from a control.
        * @param aControl
        * @param aRequest
        * @param aDelta
        * @return
        */
        TBool HandleBaseControlEventRequestL( CMsgBaseControl* aControl,
                                              TMsgControlEventRequest aRequest,
                                              TInt aDelta );

        /**
        * Handles an event from a control.
        * @param aControl
        * @param aRequest
        * @return
        */
        TBool HandleBaseControlEventRequestL( CMsgBaseControl* aControl,
                                              TMsgControlEventRequest aRequest );

        /**
        * Handles an event from a control.
        * @param aControl
        * @param aRequest
        * @param aArg1
        * @param aArg2
        * @param aArg3
        * @return
        */
        TInt HandleEditObserverEventRequestL( const CCoeControl* aControl,
                                              TMsgControlEventRequest aRequest,
                                              TAny* aArg1 = 0,
                                              TAny* aArg2 = 0,
                                              TAny* aArg3 = 0 );
        
     public: // New methods, from MObjectProvider
    	
    	IMPORT_C TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
    
    public: // from MEikScrollBarObserver
	    
	    /**
	    * Handles scroll events
	    * 
	    * @param aScrollBar Pointer to the originating scroll bar object.
	    * @param aEventType A scroll event
	    */
	    void HandleScrollEventL( CEikScrollBar* aScrollBar, TEikScrollEvent aEventType );
    	
    protected:   // from CCoeControl

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

        /**
        * Sets new position for all the controls.
        */
        void SizeChanged();

        /**
        * This is called when the focus of the control is changed.
        * @param aDrawNow
        */
        void FocusChanged( TDrawNow aDrawNow );

        /**
        * Draw
        */
    	void Draw( const TRect& aRect ) const;

    protected:  // from MCoeControlObserver

        /**
        * Handle control event.
        * @param aControl
        * @param aEventType
        */
        void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

    private:

        enum TMsgFocus
            {
            EMsgNoneFocused,
            EMsgHeaderFocused,
            EMsgBodyFocused
            };

    private:

        /**
        * Constructor.
        * @param aObserver
        * @param aEditorModeFlags
        */
        CMsgEditorView( MMsgEditorObserver& aObserver, TUint32 aEditorModeFlags );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Creates the header.
        */
        void CreateHeaderL();

        /**
        * Creates the body. Creates also the default body control if requested
        * in editor mode flags in constructor.
        */
        void CreateBodyL();

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
        * Sets sizes for the header and the body and returns their total size
        * aSize as a reference. The function does not set new sizes for the
        * controls if the aInit argument is EFalse. If aInit == ETrue, this
        * function sets the size so that only a total height can change.
        *
        * @param aSize
        * @param aInit
        */
        void SetAndGetSizeL( TSize& aSize, TBool aInit );

        /**
        * Sets the positions for the controls and re-draws the view.
        */
        void RefreshViewL();

        /**
        * Prepares controls for viewing.
        * @param aEvent
        * @param aParam
        */
        void NotifyControlsForEvent( TMsgViewEvent aEvent, TInt aParam );

        /**
        * Sets focus to a control aControlId.
        * @param aControlId
        */
        void SetFocusByControlIdL( TInt aControlId,  
                                   TBool aCorrectFormPosition = ETrue,
                                   TBool aSetCursorPos = ETrue );

        /**
        * Does an actual add operation for the control by setting all the
        * necessary observers etc.
        *
        * @param aControl
        * @param aControlId
        * @param aIndex
        * @param aFormComponent
        */
        void DoAddControlL( CMsgBaseControl* aControl,
                            TInt aControlId,
                            TInt aIndex,
                            TMsgFormComponent aFormComponent);

        /**
        * Rotates focus up or down depending aDirection argument.
        * Returns ETrue if operation can be done.
        *
        * @param aDirection
        * @param aFocusEvent
        * @return
        */
        TBool RotateFocusL( TMsgFocusDirection aDirection,
                            MMsgEditorObserver::TMsgFocusEvent& aFocusEvent );

        /**
        * Ensures that the cursor is visible on the view and that the form
        * position is correct.
        *
        */
        TBool EnsureCorrectFormPosition( TBool aScrollDown, TBool aScrollUp = EFalse );

        /**
        * Handles the form's height change. Gets pointer aControl to a control
        * which height is changing. If aDeltaHeight > 0 the height is increasing.
        *
        * @param aControl
        * @param aDeltaHeight
        * @return
        */
        TBool HandleHeightChangedL( CMsgBaseControl* aControl, TInt aDeltaHeight );

        /**
        * Scrolls the form up or down depending given amount of pixels. 
        *
        * @param aDelta              Delta of pixels to move. If aDelta > 0,
        *                            the form is scrolled downward.
        * @param aUpdateScrollBarPos Indicates whether scroll bar focus position 
        *                            should be updated inside this function.
        *
        * @return ETrue if requested pixels where scrolled. Otherwise EFalse.
        */
        TBool ScrollForm( TInt aDelta, TBool aUpdateScrollBarPos );

        /**
        * Sets view rect.
        * @param aRect
        */
        void SetViewRect( const TRect& aRect );

        /**
        * Sets proper distances between controls to obey LAF coordinates.
        */
        void AdjustComponentDistances();

        /**
        * Sets all controls initialized.
        */
        void SetComponentsInitialized();

        /**
        *
        */
        TBool SetAfterFocusL( MMsgEditorObserver::TMsgAfterFocusEventFunc aAfterFocus );

        /**
        * Determines whether given control is fully visible on the screen.
        *
        * @return ETrue if fully visible.
        *         EFalse if not fully visible.
        */
        TBool ControlFullyVisible( CMsgBaseControl* aControl ) const;

        /**
        * Resolves next focusable control when given form is navigated to 
        * certain direction. 
        *
        * @param aFormComponent Form component that is navigated 
        *                       (i.e. either header or body)
        * @param aStart         Control ID of control where search should be started.
        * @param aDirection     Navigation direction.
        *
        * @return               Control ID of next focusable control.
        *                       KErrNotFound if next control is not found.
        */
        TInt NextFocusableFormControl( CMsgFormComponent* aFormComponent, 
                                       TInt aStart, 
                                       TMsgFocusDirection aDirection );
        
        /**
        *
        */
    	void ReportFocusMovement( TInt aFocusEvent );

        /**
        * Constructor (not available).
        */
        CMsgEditorView();
        
        /**
        * Ensures that a Visible-part is completely scrolled. 
        * 
        * @param aFocusPosition .
        * @param aDirection Scrolling direction.
        * 
        * @return               ETrue/EFalse
        */
        TBool EnsureVisiblePartScrollComplete( TInt aFocusPosition,
                                               TMsgScrollDirection aDirection );
        
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
        * Returns control matching located at given screen position.
        *
        * @param aPosition          Screen coordinates where control should be returned.
        * @param aEvaluateHitText   If ETrue internal hit test evaluation is performed.
        *                           Otherwise hit text evaluation is not performed and control
        *                           location is solely based on it's physical coordinates.
        */
        CMsgBaseControl* ControlFromPosition( TPoint aPosition, TBool aEvaluateHitTest ) const;
        
        /**
        * Convenience function for casting the scroll bar model.
        */
        inline const TAknDoubleSpanScrollBarModel* AknScrollBarModel() const; 
        
        /**
        * Convenience function for checking whether view is on editor or viewer mode.
        */
        inline TBool IsReadOnly() const;
        
        /**
        * Ensures that cursor are located correctly relative to currently focused control.
        */
        void EnsureCorrectCursorPosition();
        
        /**
        * Ensures that correct scroll part is shown to the user.
        */
        void EnsureCorrectScrollPartL( TInt aFocusPosition );
        
        /**
        * Returns the number of currently scrolled scroll part.
        */
        TInt CurrentScrollPart( TInt aFocusPosition );
        
        /**
        * Determines if control is focusable (i.e. focus should
        * stop to the control).
        *
        * @since 3.2
        *
        * @param aControl   Control that is tested.
        * @param aDirection Navigation direction.
        *
        * @return ETrue if focus should stop to this control.
        *         EFalse if focus should not stop to this control.
        */
        TBool IsFocusable( CMsgBaseControl* aControl, TMsgFocusDirection aDirection ) const;
        
        /**
        * Shows scrollbar popup info text.
        */
        void ShowScrollPopupInfoTextL( CAknDoubleSpanScrollBar* aScrollBar, TInt aPartNumber );
        
        /**
        * Ensures that view position is valid and correctly positioned.  
        */
        void EnsureCorrectViewPosition();
        
        /**
         * Performs view scrolling to given direction. 
         * 
         * @param aPixelsToScroll Amount of pixels wanted to be scrolled. On return contains
         *                        the amount of pixels not scrolled.
         * @param aDirection Scrolling direction.
         */
         void DoScrollViewL( TInt& aPixelsToScroll, 
                             TMsgScrollDirection aDirection );
		/**
		* Handle text scrolling from pointer event.
		* 
		* @param aPointerEvent Current pointer event received from HandlePointerEventL.
		*/					 
		TBool HandleScrollEventL(const TPointerEvent& aPointerEvent);
		
		/**
		* Throw the pointer event (which is not consumed by text scrolling) to other handler. 
		* 
		* @param aPointerEvent Current pointer event received from HandlePointerEventL.
		*/
		
    	void ThrowOutPointerEventL(const TPointerEvent& aPointerEvent);
		
		/**
		* Move current view up or down with screen height. 
		* 
		* @param aOffset Offset positive/negative will indicate the direction.
		*/
		
    	void ScrollPartL(TInt aOffset);
		
		/**
		* Change to the previous or the next part(slide) if possible. 
		* 
		* @param aOffset Offset positive/negative will indicate the direction.
		*/
		
    	void ScrollPageL(TInt aOffset);
		
		/**
		* Scroll text in current part (slide).  This will not trigger part change.
		* 
		* @param aScrolledPixels Pixels wanted to be scrolled.
		*
		* @return Pixels scrolled.
		*/
		
    	TInt ScrollL(TInt aScrolledPixels);

        
    private:

        MMsgEditorObserver& iEditorObserver;
        CMsgHeader*         iHeader;
        CMsgBody*           iBody;
        TMsgFocus           iCurrentFocus;
        TInt                iFormOffset;         // distance from the main window's top y to form's top y
        TUint32             iEditorModeFlags;
        TRect               iViewRect;
        TUint32             iStateFlags;
        CEikScrollBarFrame* iScrollBar;
        TInt                iScrollParts;
        TInt                iVisiblePart;
    	CAknsBasicBackgroundControlContext* iBgContext; // Skin background control context
    	TInt                iUniqueHandlePool;
    	TInt                iLineHeight;
        TInt                iBaseLineOffset;    	
    	TInt                iViewFocusPosition;
    	
    	CArrayFixFlat<TInt>* iScrollPartArray;
    	HBufC*               iScrollPopText;   
    	TInt                 iPopUpPart; 
    	TInt                 iVisiblePartHeight;
    	
    	MEikEdwinObserver*   iEdwinObserver;
    	RConeResourceLoader  iResourceLoader;
    	TMsgFocus            iPrevFocus; 
    	TBool                iMoveUpDownEvent; 
    	TBool                iHaveScrolled;
    	TPoint               iScrollPos;
    	TPointerEvent        iFirstPointerDown;
    	TBool                iIsScrolling;
    };

#include <MsgEditorView.inl>

// =========================================================

#endif // CMSGEDITORVIEW_H

// End of File
