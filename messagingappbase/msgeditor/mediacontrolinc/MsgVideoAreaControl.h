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
* Description:  
*       MsgEditor video area control. This control contains the real
* 	 	video playback area.
*
*/



#ifndef MSGVIDEOAREACONTROL_H
#define MSGVIDEOAREACONTROL_H

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
* UI control controlling the video playback area.
*/
NONSHARABLE_CLASS( CMsgVideoAreaControl ) : public CCoeControl
    {
    public:  // Constructor and destructor
        
	    /**
	     * Two-phased constructor.
	     * @param aParent parent control
	     * @return new object
	     */
	   static CMsgVideoAreaControl* NewL( CCoeControl* aParent );
                          
        /**
        * Destructor.
        */
        virtual ~CMsgVideoAreaControl();
    
    private: // Funtions from base classes
    	
    	/**
    	* From CCoeControl Draws video area background.
    	*/ 
    	void Draw( const TRect& aRect ) const;
    
    private:
    	
	    /**
	     * C++ default constructor
	     */
    	CMsgVideoAreaControl();
    	
    	/**
         * By default Symbian constructor is private.
         */
        void ConstructL( CCoeControl* aParent );
         
    private: // Data
    };
    

#endif // MSGVIDEOAREACONTROL_H
