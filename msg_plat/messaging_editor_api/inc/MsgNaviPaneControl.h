/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgNaviPaneControl  declaration
*
*/



#ifndef CMSGNAVIPANECONTROL_H
#define CMSGNAVIPANECONTROL_H

// ========== INCLUDE FILES ================================

#include <coecntrl.h>
#include <MuiuMessageIterator.h>    // for CMessageIterator
#include <MsgEditorCommon.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================
class CEikImage;
class TAknsItemID;
class TAknLayoutRect;
class CEikLabel;
class CMessageIterator;
class CMsvSession;
class TMsvEntry;
class CCoeControlArray;
class MMsgNaviPaneControlObserver;

// ========== CLASS DECLARATION ============================
    
/**
* Navigation pane control for message viewers.
*
* @since 5.0
*
* @library CMsgEditorAppUi
*/
NONSHARABLE_CLASS( CMsgNaviPaneControl ) : public CCoeControl,
                                           public MMessageIteratorObserver
    {
    public: // Enumerations
    
        enum TMsgNaviControlId
            {
            EMsgNaviTimeControlId,
            EMsgNaviPriorityControlId,
            EMsgNaviLeftArrowControlId,
            EMsgNaviRightArrowControlId,
            EMsgNaviNavigationControlId
            };
            
    public: // Constructors and destructor
        
        /**
        * Factory method.
        *
        * @param aParent            Parent control.
        *
        * @return pointer to new CMsgNaviPaneControl object
        */
        IMPORT_C static CMsgNaviPaneControl* NewL( const CCoeControl* aParent );

        /**
        * Destructor.
        */
        virtual ~CMsgNaviPaneControl();
    
    public: // New functions
    
        /**
        * Sets time indicator to the control. 
        *
        * @since 5.0
        *
        * @param aTime New time
        * @param aUtcTime ETrue if given time is UTC time,
        *                 EFalse if given time is local time. 
        */
        IMPORT_C void SetTimeIndicatorL( TTime aTime, TBool aUtcTime = EFalse );
        
        /**
        * Sets priority indicator to the control
        *
        * @since 5.0
        *
        * @param aPriority New priority
        */
        IMPORT_C void SetPriorityIndicatorL( TMsgEditorMsgPriority aPriority );
        
        /**
        * Sets navigation indicator to the control. 
        *
        * @since 5.0
        *
        * @param aSession Message server session.
        * @param aCurrentMessage Current message entry.
        */
        IMPORT_C void SetNavigationIndicatorL( CMsvSession& aSession,
                                               const TMsvEntry& aCurrentMessage );
        
        /**
        * Sets navigation control observer. 
        *
        * @since 5.0
        *
        * @param aObserver Pointer to new observer. Setting observer to null
        *                  removes the observer.
        */
        IMPORT_C void SetNavigationControlObserver( MMsgNaviPaneControlObserver* aObserver );
        
        /**
         * Get Message Iterator Instance  
         *
         * @since 9.2
         *
         * @return aMessageIterator Message iterator 
         */
        IMPORT_C CMessageIterator* GetMessageIterator();

    public: // Functions from base classes

        /**
        * From CoeControl
        * See the documentation from the coecntrl.h
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl
        * See the documentation from the coecntrl.h
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;        
        
        /**
        * From CCoeControl
        * See the documentation from the coecntrl.h
        */
        void SizeChanged();
        
        /**
        * From CCoeControl
        * See the documentation from the coecntrl.h
        */
	    void PositionChanged();
        
        /**
        * From CCoeControl
        * See the documentation from the coecntrl.h
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );
        
        /**
         * From CCoeControl
         * For handling resource change events.
         */
         void HandleResourceChange( TInt aType );
        
        /**
        * From MMessageIteratorObserver
        * See the documentation from the MuiuMessageIterator.h
        */
        void HandleIteratorEventL( TMessageIteratorEvent aEvent );
        
    private:

        /**
        * Default C++ constructor.
        */
        CMsgNaviPaneControl();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const CCoeControl* aParent );
        
        /**
        * Initializes indicator.
        */
        void InitializeIconIndicatorL( CEikImage*& aIndicator, 
                                       const TAknsItemID& aItem, 
                                       const TInt aBitmapIndex,
                                       const TInt aMaskIndex ) const;
        
        /**
        * Sets specified icons extent to given one.
        */
        void SetIconIndicatorExtent( CEikImage* aIconIndicator,
                                     const TAknLayoutRect& aExtent) const;
        
        /**
        * Sets correct layout for all indicators.
        */
        void LayoutIndicators();
        
        /**
        * Sets correct layout for navigation indicator.
        */
        void LayoutNavigationIndicators();
        
        /**
        * Sets correct layout for priority indicator.
        */
        void LayoutPriorityIndicator();
        
        /**
        * Sets correct layout for time indicator.
        */
        void LayoutTimeIndicator();
        
        /**
        * Updates navigation indicators (left and right arrows and message icon)
        * according to current message count.
        */
        void UpdateNavigationIndicatorsL();
        
        /**
        * Updates priority indicator according to current priority.
        */
        void UpdatePriorityIndicatorL();
        
        /**
        * Updates time indicator according to current time.
        */
        void UpdateTimeIndicatorL();
        
        /**
        * Adds control to controls array
        */
        void AddToControlArrayL( CCoeControl* aControl, TMsgNaviControlId aControlId );
        
        /**
        * Adds control to controls array
        */
        void RemoveFromControlArray( TMsgNaviControlId aControlId );
        
        /**
         * Handles skin change event.
         */
         void  HandleSkinChangeL();
         /**
         * Updates the vaient type(between 1..4) to be used for Right left arrows and touch layouts
         */
         void UpdateVarientId();
        
    private: // data
        
        /** Message time */
        TTime iTime;
        
        /** Message priority */
        TMsgEditorMsgPriority iPriority;
        
        /** Message iterator */
        CMessageIterator* iMessageIterator;
        
        /** Control array */
        CCoeControlArray* iControlArray;
        
        /** Used to keep track of focused control between pointer events. */
        CCoeControl* iPreviouslyFocusedControl;
        
        /** Control observer. */
        MMsgNaviPaneControlObserver* iObserver;
        
        /** Touch areas from LAF for arrow icons */
        TRect iLeftArrowTouchRect;
        TRect iRightArrowTouchRect;
        TBool iPriorityIndicator;
        TInt iVarientId;
    };
    
#endif // CMSGNAVIPANECONTROL_H

// End of File
