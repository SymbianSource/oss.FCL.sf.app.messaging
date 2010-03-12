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
*     Empty container for main view. This is displayed when mce is launched before
*     message server is ready.
*
*/



#ifndef MCEMAINVIEWEMPTYLISTCONTAINER_H
#define MCEMAINVIEWEMPTYLISTCONTAINER_H

//  INCLUDES
#include <coecntrl.h>
// CLASS DECLARATION
class CAknsBasicBackgroundControlContext;

/**
*  Displayed in mce launch before message server is ready.
*/
class CMceMainViewEmptyListContainer : public CCoeControl
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CMceMainViewEmptyListContainer();

        /**
        * Two-phased constructor.
        */
        static CMceMainViewEmptyListContainer* NewL(const TRect& aRect);

        /**
        * Destructor.
        */
        virtual ~CMceMainViewEmptyListContainer();

    public: // Functions from base classes

        /**
        * From CCoeControl
        */
        virtual void Draw(const TRect& aRect) const;

        /**
        * From CCoeControl
        */
        virtual TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );
        
        /**
        * From CCoeControl
        */
        TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
        /**
        * Called when the size has changed.
        */        
        void SizeChanged();

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL(const TRect& aRect);
        
        /**
        * Own: Background Skin
        */
        CAknsBasicBackgroundControlContext* iBgContext;

    };

#endif      // MCEMAINVIEWEMPTYLISTCONTAINER_H

// End of File
