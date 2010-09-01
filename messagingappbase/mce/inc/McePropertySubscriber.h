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
*     CallBack class for notification of RProperty changes 
*
*/



#ifndef MCEPROPERTYSUBSCRIBER_H
#define MCEPROPERTYSUBSCRIBER_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>

// CLASS DECLARATION

class CMcePropertySubscriber : public CActive
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMcePropertySubscriber* NewL( TCallBack aCallBack, RProperty& aProperty );
        ~CMcePropertySubscriber();

    public: // New functions
        
        /**
        * Subscribes notification if input mode has changed
        */
        void SubscribeL();
        
        /**
        * Stop Subscribe notificatios
        */
        void StopSubscribe();

    private:

        /**
        * C++ constructor.
        */
        CMcePropertySubscriber( TCallBack aCallBack, RProperty& aProperty );

        /**
        * Symbian OS constructor.
        */
        void ConstructL();
    
    private: // from CActive
        void RunL();
        void DoCancel();

    private:
        TCallBack   iCallBack;
        RProperty&  iProperty;
        };

#endif //MCEPROPERTYSUBSCRIBER_H

// End of file

