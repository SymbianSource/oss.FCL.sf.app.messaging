/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       MsgEditor Frame control
*
*/



#ifndef MSGFRAMECONTROL_H
#define MSGFRAMECONTROL_H

// INCLUDES
#include <coecntrl.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

// ==========================================================

/**
*  CMsgFrameControl is a class for drawing a frame
*  around the bitmap of CMsgMediaControl
*/
NONSHARABLE_CLASS( CMsgFrameControl ) : public CCoeControl
    {
    public:  // Constructor and destructor

        /**
        * Symbian constructor
        * @param aParent parent control
        */
        static CMsgFrameControl* NewL( const CCoeControl& aParent );

        /**
        * Destructor.
        */
        virtual ~CMsgFrameControl();

    public:

        /**
        * Called when size is changed.
        * @param aSize New size of the "image" (e.g. icon).
        */
        void SetImageSize( const TSize& aSize );

        /**
        * Size of the frame
        * @return size
        */
        TSize FrameSize() const;
        
        /**
        * Size of the frame borders i.e. thickness of frame.
        * The size includes both borders.
        * @return size
        */
        inline TSize FrameBorderSize() const;

        /**
        * To support media control reloading without creating new control
        */
        inline void ResetFrameSize();

    protected: // from CCoeControl

        /**
        * Draw Control
        * From CCoeControl
        */
        void Draw( const TRect& aRect ) const;
        
        /**
         * Handles resource change events.
         * From CCoeControl
         */
         void HandleResourceChange( TInt aType );
        
    private: // Constructors

        /**
        * Hidden C++ default constructor.
        */
        CMsgFrameControl();

        /**
        * By default Symbian constructor is private.
        * @param aParent parent control
        */
        void ConstructL( const CCoeControl& aParent );
        
        /**
        * Set correct frame size
        */
        void SetFrameSize();
        
    private: //Data

        TSize iFrameBorderSize;
        TSize iFrameSize;
        TBool iImageSizeSet;
    };


// ---------------------------------------------------------
// FrameBorderSize
//
// ---------------------------------------------------------
//
inline TSize CMsgFrameControl::FrameBorderSize() const
    {
    return iFrameBorderSize;
    }


// ---------------------------------------------------------
// ResetFrameSize
//
// ---------------------------------------------------------
//
inline void CMsgFrameControl::ResetFrameSize()
    {
    iFrameSize.iWidth = 0;
    iFrameSize.iHeight = 0;
    }

#endif // MSGFRAMECONTROL_H
