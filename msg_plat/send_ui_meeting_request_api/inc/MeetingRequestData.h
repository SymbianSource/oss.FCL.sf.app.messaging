/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Encapsulates the message data for MeetingRequestSender
*                sending services.
*
*/



#ifndef CMEETINGREQUESTDATA_H
#define CMEETINGREQUESTDATA_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <msvstd.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

const TInt KMethodTypeMaxLength = 50;

} // namespace


//  CLASS DEFINITION
/**
 * CMeetingRequestData is a container for meeting requests
 * CMeetingRequestData is used by CMeetingRequestSender.
 * Inherits CMessageData. 
 *
 * @lib MeetingRequestData.dll
 * @since Series 60 3.0
 * @see CMessageData
 */
class CMeetingRequestData : public CBase // base class for sendui messages
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMeetingRequestData* NewL();

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMeetingRequestData* NewLC();

        /**
        * Destructor.
        */
        virtual ~CMeetingRequestData();

    protected: 

        /**
        * C++ default constructor.
        */
        CMeetingRequestData();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    public:     // derived from CSendingService

        /**
        * Writes meeting request data to a buffer
        * @since Series 60 3.2
        * @return A buffer containing "externalized" CMeetingRequestData.
        */ 
        IMPORT_C HBufC8* WriteToBufferLC() const;
        
        /**
        * Reads meeting request data from a buffer
        * @since Series 60 3.2
        * @param aData A buffer containing "externalized" CMeetingRequestData.
        * @return None.
        */ 
        IMPORT_C void ReadFromBufferL( const TDesC8& aData );

    public:     // New methods

        /**
        * Set mailbox which should be used when sending an meeting request. 
        *
        * NOTE: if mailbox is not set, MeetingRequestSender leaves
        * with KErrArgument. 
        *
        * Mailbox id is the same as mail client service id. 
        * @since Series 60 3.0
        * @param aMailboxId Mailbox id. 
        * @return None. 
        */
        IMPORT_C void UseMailbox( const TMsvId aMailboxId );

        /**
        * Returns mailbox id.
        * Mailbox id is the same as mail client service id. 
        * @since Series 60 3.0
        * @return Mailbox id. 
        */
        IMPORT_C TMsvId Mailbox( ) const;

        /**
        * Set id of calendar database file.
        *
        * @since Series 60 3.0
        * @param aFileId Id of calendar database file.
        * @return None.
        */
        IMPORT_C void SetDatabaseFileId( const TUint64 aFileId );

        /**
        * Returns id of calendar database file.
        *
        * @since Series 60 3.0
        * @return Id of calendar database file.
        */
        IMPORT_C TUint64 DatabaseFileId() const;

        /**
        * Append local id of calendar entry.
        *
        * @since Series 60 3.0
        * @param aEntryLocalId Local id of calendar entry.
        * @return None.
        */
        IMPORT_C void AppendEntryLocalId( const TUint32 aEntryLocalId );

        /**
        * Returns array of local ids of calendar entries.
        *
        * @since Series 60 3.0
        * @return Array of local ids of calendar entries.
        */
        IMPORT_C const RArray<TUint32>& EntryLocalIdArray() const;

        /**
        * Set method type for mail header. Eg. REPLY
        *
        * @since Series 60 3.0.
        * @param aMethod Method type for mail header.
        * @return None.
        */
        IMPORT_C void SetMailHeaderMethodL( const TDesC8& aMethodType );

        /**
        * Returns method type for mail header. 
        *
        * @since Series 60 3.0
        * @return Method type for mail header.
        */
        IMPORT_C TPtrC8 MailHeaderMethod() const;
        

    private:    // Data

        // Mailbox id
        TMsvId                                  iMailboxId;

        // Mail header method type
        TBuf8<KMethodTypeMaxLength>             iMethodType;

        // Calendar database file id.
        TUint64                                 iDatabaseFileId;

        // Array of local ids for entries
        RArray<TUint32>                         iEntryLocalIdArray;
    };

#endif      //  CMEETINGREQUESTDATA_H

// End of File
