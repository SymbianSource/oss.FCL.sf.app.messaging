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
*       CMsgGarbageCollection declaration file
*
*/



#ifndef MSGGARBAGECOLLECTION_H
#define MSGGARBAGECOLLECTION_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <msvapi.h>

#include "MsgErrorWatcher.h"

// FORWARD DECLARATIONS

class CClientMtmRegistry;
class CBaseMtm;

// CLASS DECLARATION

/**
* CMsgGarbageCollection
*
* @lib msgerrorwatcher.dll
* @since 3.2
*/
class CMsgGarbageCollection : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aSession A pointer to CMsvSession
        */
        static CMsgGarbageCollection* NewL( CClientMtmRegistry& aMtmRegistry );
        
        /**
        * Destructor.
        */
        ~CMsgGarbageCollection();
    
    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Private C++ constructor.
        *
        * @param aSession A pointer to CMsvSession
        */
        CMsgGarbageCollection( CClientMtmRegistry& aMtmRegistry );

        /**
        *
        */
        void StartWaiting();

        /**
        *
        */
        void WapPushGarbageCollectionL();
        
    private: //from CActive

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

    private:    // Data
    
        enum TGarbageCollectionState
            {
            EGarbageCollectionWaiting,
            EGarbageCollectionWapPush
            };

        CClientMtmRegistry& iMtmRegistry;
        CBaseMtm* iWapMtm;
        CMsvOperation* iOp;

        TGarbageCollectionState iState;
        RTimer iTimer;

    };

#endif      // MSGSENTITEMSOBSERVER_H
            
// End of File
