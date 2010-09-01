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
*     CMsgErrorStartupObserver declaration file
*
*/



#ifndef __MSGERRORSTARTUPOBSERVER_H
#define __MSGERRORSTARTUPOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class CMsgErrorWatcher;

// CLASS DECLARATION

/**
* CMsgErrorStartupObserver
*
* @lib msgerrorwatcher.dll
* @since 3.0
*/
class CMsgErrorStartupObserver : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcher
        */
        static CMsgErrorStartupObserver* NewL( CMsgErrorWatcher* aWatcher );
        
        /**
        * Destructor.
        */
        virtual ~CMsgErrorStartupObserver();

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Private C++ constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcher
        */
        CMsgErrorStartupObserver( CMsgErrorWatcher* aWatcher );

    private:

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();

    private:    // Data

        CMsgErrorWatcher*   iWatcher;
        RProperty iStartupProperty;

    };

#endif      // __MSGERRORSTARTUPOBSERVER_H
            
// End of File
