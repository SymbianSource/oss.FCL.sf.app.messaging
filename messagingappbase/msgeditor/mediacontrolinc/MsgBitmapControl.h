/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       MsgEditor bitmap container control
*
*/



#ifndef MSGBITMAPCONTROL_H
#define MSGBITMAPCONTROL_H

// INCLUDES
#include <coecntrl.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MIHLBitmap;

// CLASS DECLARATION

// ==========================================================

/**
* UI control handling the bitmap & mask drawing.
*/
NONSHARABLE_CLASS( CMsgBitmapControl ) : public CCoeControl
    {
    public:  // Constructor and destructor
        
        /**
        * C++ default constructor
        */
        CMsgBitmapControl();
                          
        /**
        * Destructor.
        */
        virtual ~CMsgBitmapControl();

    public:
        
        /**
        * Sets bitmap for control.
        *
        * @param aBitmap Bitmap. Not owned.
        */
        void SetBitmap( MIHLBitmap* aBitmap );

    public: // from CCoeControl
        
        /**
        * From CCoeControl,Draw.
        * @param aRect draw rect
        */
        void Draw( const TRect& aRect ) const;
    
    private: // Data

        // Destination bitmap
        MIHLBitmap*         iBitmap;
    };
    

#endif // MSGBITMAPCONTROL_H
