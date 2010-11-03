/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Defines class CNcnCRRepositoryHandler.
*
*/



#ifndef NCNCRREPOSITORYHANDLER_H
#define NCNCRREPOSITORYHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <cenrepnotifyhandler.h>

// FORWARD DECLARATIONS
class MNcnCRRepositoryHandlerCallback;

// CLASS DECLARATION

/**
*  Observes the value of the CenRep key changes withing a repository.
*/
class CNcnCRRepositoryHandler :
    public CBase,
    public MCenRepNotifyHandlerCallback
    {
    private:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @param aRepository The repository to handle.
        * @param aKeyType Key type.
        * @param aId Id of the key.
        */
        CNcnCRRepositoryHandler(
            MNcnCRRepositoryHandlerCallback& aCallback,
            CRepository& aRepository,
            CCenRepNotifyHandler::TCenRepKeyType aKeyType,
            TUint32 aId );

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CNcnCRRepositoryHandler* NewL(
            MNcnCRRepositoryHandlerCallback& aCallback,
            CRepository& aRepository,
            CCenRepNotifyHandler::TCenRepKeyType aKeyType,
            TUint32 aId );

        /**
        * Destructor.
        */
        virtual ~CNcnCRRepositoryHandler();

    public: // From MCenRepNotifyHandlerCallback
        void HandleNotifyInt(TUint32 aId, TInt aNewValue);
        void HandleNotifyReal(TUint32 aId, TReal aNewValue);    
        void HandleNotifyString(TUint32 aId, const TDesC16& aNewValue);
        void HandleNotifyBinary(TUint32 aId, const TDesC8& aNewValue);
        void HandleNotifyGeneric(TUint32 aId);
        void HandleNotifyError(TUint32 aId, TInt aError, CCenRepNotifyHandler* aHandler);

    public:  // Other public methods
        /**
         * Starts listening the repository for specified key changes.
         */
        void StartListeningL();

        /**
         * Stops the listening.
         */
        void StopListening();

    protected: // Data

        // Callback reference
        MNcnCRRepositoryHandlerCallback& iCallback;

        // CenRep repository reference
        CRepository& iRepository;
        
        // Notify handler for repository key, owned
        CCenRepNotifyHandler* iNotifyHandler;
        
        // Key type
        CCenRepNotifyHandler::TCenRepKeyType iKeyType;
        
        // Id of the key
        TUint32 iId;
    };

#endif    // NCNCRREPOSITORYHANDLER_H

// End of File
