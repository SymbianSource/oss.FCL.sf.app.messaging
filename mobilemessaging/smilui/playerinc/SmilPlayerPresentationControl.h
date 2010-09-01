/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SmilPlayerPresentationControl  declaration
*
*/


#ifndef SMILPLAYERPRESENTATIONCONTROL_H
#define SMILPLAYERPRESENTATIONCONTROL_H

// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CSmilPlayerPresentationController;

// CLASS DECLARATION
/**
*  Control class of the presentation
*/
NONSHARABLE_CLASS(CSmilPlayerPresentationControl) : public CCoeControl
    {

    public: // Constructors and destructor

        /**
        * Constructor.
        *
        * @param aParent      Presentation controller
        *
        * @return pointer to the object
        */
        static CSmilPlayerPresentationControl* NewL( 
                        CSmilPlayerPresentationController* aParent );

        /**
        * Destructor.
        */
        virtual ~CSmilPlayerPresentationControl();

    public: // Functions from base classes

        /**
        * From CCoeControl,Draw.
        * See the documentation from the coecntrl.h
        */
        void Draw(const TRect& aRect) const;
        
    public: // new functions

        /**
        * Redraws given area of presentation to screen.
        *
        * @since 2.8
        * 
        * @param aArea Area to redraw.
        */
        void RedrawArea( const TRect& aArea ) const;

    private:

        /**
        * C++ constructor.
        */
        CSmilPlayerPresentationControl( 
            CSmilPlayerPresentationController* aParent );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Performs presentation drawing operation.
        */
        void DoDrawPresentation( const TPoint& aDestination,
                                 const TRect& aSourceRect,
                                 const TRect& aPresentationRect ) const;

    private: // data

        // Controller class for this class
        CSmilPlayerPresentationController*              iParent;
    };

#endif // SMILPLAYERPRESENTATIONCONTROL_H

// End of File
