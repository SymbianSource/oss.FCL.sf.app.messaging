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
*     Operation to delete mail messages locally i.e. just remove body and
*     attachments from the flash but leave header. So don't delete message
*     from mail server. After operation user is able to fetch message again.
*
*
*/


#ifndef IMUMDELETEMESSAGESLOCALLY_H
#define IMUMDELETEMESSAGESLOCALLY_H

//  INCLUDES
#include <e32base.h>                    // Basic Symbian Includes
#include <cacheman.h>                   // CImCacheManager
#include <MsvPrgReporter.h>             // MMsvProgressDecoder
#include <centralrepository.h>          // CRepository
#include <ImumInternalApi.h>            // CImumInternalApi

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  Operation to delete mail messages locally i.e. just remove body and
*  attachments from the flash but leave header. So don't delete message
*  from mail server. After operation user is able to fetch message again.
*/
class CImumDeleteMessagesLocally :
    public CImCacheManager,
    public MMsvProgressDecoder
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aMessageSelection array of message ids to be purged
        * @param aMsvSession
        * @param aObserverRequestStatus
        * @param aDiskSpaceRequest: If ETrue, request disk space
        * @return pointer to created CImumDeleteMessagesLocally object.
        */
        static CImumDeleteMessagesLocally* NewL(
            CMsvEntrySelection& aMessageSelection,
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            TBool aDiskSpaceRequest = ETrue);

        /**
        * destructor
        */
        virtual ~CImumDeleteMessagesLocally();

    public: // Functions from base classes
        /**
        * From MMsvProgressDecoder
        */
        virtual TInt DecodeProgress (
            const TDesC8& aProgress,
            TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans,
            TBool aInternal);


    protected: // Constructors
        /**
        * Symbian OS constructor.
        * @param aMessageSelection array of message ids to be purged
        */
        void ConstructL(CMsvEntrySelection& aMessageSelection );

    private:
        /**
        * from CImCacheManager
        */
        TBool Filter() const;

    private: // Constructors
        /**
        * C++ constructor
        */
        CImumDeleteMessagesLocally(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            TBool aDiskSpaceRequest);

    private: // data

        CMsvEntrySelection*                 iMessageSelection;
        // File session
        RFs                                 iFileSession;
        // Flag to inform of reserved disk space
        TBool                               iDiskSpaceRequest;
        // Currently used drive for deletion
        TInt                                iDriveNo;
        // Text buffer for progress note
        HBufC*                              iProgressText;
    };

#endif      // IMUMDELETEMESSAGESLOCALLY_H

// End of File
