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
* Description:  MsgHeader  declaration
*
*/



#ifndef CMSGHEADER_H
#define CMSGHEADER_H

// ========== INCLUDE FILES ================================

#include "MsgFormComponent.h"              // for CMsgFormComponent

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgCaptionedHighlight;
class CMsgDummyHeaderCleaner;

// ========== CLASS DECLARATION ============================

/**
* Defines header control container for message editors/viewers.
*
*/
class CMsgHeader : public CMsgFormComponent,
                   public MCoeControlHitTest
    {
    public:

        /**
        * Factory method that creates this control.
        * @param aParent
        * @param aMargins
        * @return
        */
        static CMsgHeader* NewL( const CCoeControl& aParent, 
                                 const TMargins& aMargins);

        /**
        * Destructor.
        */
        virtual ~CMsgHeader();

    public:  // Methods from base classes

        /**
        * Calculates and sets the size for the header and returns new size as
        * reference aSize. If aInit == ETrue, sets also size for the controls
        * by calling their SetAndGetSizeL functions.
        *
        * @param aSize
        * @param aInit
        */
        void SetAndGetSizeL( TSize& aSize, TBool aInit );    

        /**
        * Draws the header.
        * @param aRect
        */
        void Draw( const TRect& aRect ) const;

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
        * Evaluates whether control is "hit" by touch event.
        */
        TBool HitRegionContains( const TPoint& aPoint, const CCoeControl& aControl ) const;

    protected:  // from CCoeControl

        /**
        * Sets new position for all the controls.
        */
        void SizeChanged();

        /**
        * This is called when the focus of the control is changed.
        * @param aDrawNow
        */
        void FocusChanged( TDrawNow aDrawNow );

    private:
        
        /**
        * Constructor (not available).
        */
        CMsgHeader();
        
        /**
        * Constructor.
        * @param aMargins
        */
        CMsgHeader( const TMargins& aMargins );

        /**
        * 2nd phase constructor.
        * @param aParent
        */
        void ConstructL( const CCoeControl& aParent );

    private:

        CMsgCaptionedHighlight* iFrame;
        CMsgDummyHeaderCleaner* iHeaderCleaner;
    };

#endif // CMSGHEADER_H

// End of File
