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
* Description:   The global lock class for messaging components
*
*/



#ifndef MUIULOCK_H
#define MUIULOCK_H

class CRepository;

#include "messaginginternalcrkeys.h"


// Lock identifier for messaging settings access
//
const TUid KMuiuLockSettings = { KCRUidMuiuVariationValue };

/**
 *  The global lock class for messaging components
 *
 *  The service can be used for creating runtime "locks" for protecting
 *  the shared resources over the process boundaries; i.e.the locks can be 
 *  used for synchronisation between the processes.
 *
 *  The current implementation is based on the transaction mechanism of
 *  Central Repository. The lock ID is the identifier of the repository
 *  file. The "lock" is created by opening a write transaction to the
 *  repository. Thus, the repository cannot be modified while the lock is on.
 *
 *  @lib muiu_internal.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CMuiuLock ) : public CBase
    {
public:
    /**
     * Creates a new instance of the class
     *
     * @since S60 5.0
     * @param aLockId    The lock identifier. Currently, this has to be the ID
     *                   of the existing central repository file that can be reserved
     *                   for a write transaction while the lock is used.
     * @return           New instance of the class
     */
     IMPORT_C static CMuiuLock* NewL( TUid aLockId );

    /**
     * Destructor. Releases the lock if not done explicitly.
     */
    virtual ~CMuiuLock();

    /**
     * Reserves the lock for the client.
     *
     * @since S60 5.0
     * @return           Operation status. KErrNone if successful. KErrGeneral, if the lock
     *                   is already reserved by the client. The error codes of the underlying
     *                   services forwarded as such.
     */
    IMPORT_C TInt Reserve();

    /**
     * Releases the lock
     *
     * @since S60 5.0
     */
    IMPORT_C void Release();

private:
    /**
     * Default constructor
     *
     * @since S60 5.0
     */
    CMuiuLock();
    
    /**
     *  Second phase constructor
     *
     * @since S60 5.0
     * @param aLockId    See description of NewL() method
     */
    void ConstructL( TUid aLockId );
private: // data
    /**
     * Central repository handle
     */
    CRepository* iRepository;

    /**
     * State flag indicating whether the lock is reserved or not
     */
    TBool iLocked;
    };

#endif // MUIULOCK_H
