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
* Description:   Defines class CMNcnCRRepositoryHandlerCallback.
*
*/



#ifndef NCNCRREPOSITORYHANDLERCALLBACK_H
#define NCNCRREPOSITORYHANDLERCALLBACK_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CRepository;
class CCenRepNotifyHandler;

// CLASS DECLARATION

/**
 * Callback interface for repository specific key changes.
 */
class MNcnCRRepositoryHandlerCallback
    {
    public:
        /**
         * Repository specific integer value notification callback.
         * @param aRepository The repository.
         * @param aId Id of the key that has changed.
         * @param aNewValue The new value of the key.
         */
        virtual void HandleRepositoryNotifyInt(
            CRepository& aRepository,
            TUint32 aId,
            TInt aNewValue );

        /**
         * Repository specific real value notification callback.
         * @param aRepository The repository.
         * @param aId Id of the key that has changed.
         * @param aNewValue The new value of the key.
         */
        virtual void HandleRepositoryNotifyReal(
            CRepository& aRepository,
            TUint32 aId,
            TReal aNewValue );


        /**
         * Repository specific string value notification callback.
         * @param aRepository The repository.
         * @param aId Id of the key that has changed.
         * @param aNewValue The new value of the key.
         */
        virtual void HandleRepositoryNotifyString(
            CRepository& aRepository,
            TUint32 aId,
            const TDesC16& aNewValue );

        /**
         * Repository specific binary value notification callback.
         * @param aRepository The repository.
         * @param aId Id of the key that has changed.
         * @param aNewValue The new value of the key.
         */
        virtual void HandleRepositoryNotifyBinary(
            CRepository& aRepository,
            TUint32 aId,
            const TDesC8& aNewValue );

        /**
         * Repository specific generic value notification callback.
         * @param aRepository The repository.
         * @param aId Id of the key that has changed.
         */
        virtual void HandleRepositoryNotifyGeneric(
            CRepository& aRepository,
            TUint32 aId );

        /**
         * Repository specific callback method for error notifications.
         * @param aRepository The repository.
         * @param aId Id of the key that has changed.
         * @param aError Error code.
         * @param aHandler Pointer to the handler instance.
         */
        virtual void HandleRepositoryNotifyError(
            CRepository& aRepository,
            TUint32 aId,
            TInt aError,
            CCenRepNotifyHandler* aHandler );
    };
    
#endif    // NCNCRREPOSITORYHANDLERCALLBACK_H

// End of File
