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
* Description:  ImumCreatorPredefinedMailbox.h
*
*/



#ifndef IMUMCREATORPREDEFINEDMAILBOX_H
#define IMUMCREATORPREDEFINEDMAILBOX_H

// INCLUDES
#include <e32base.h>
#include <centralrepository.h>      // CRepository
#include <ImumInternalApi.h>        // CImumInternalApi

#include "Imas.hrh"                 // KImas -constants

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CImumInternalApiImpl;
class CImumInSettingsData;
class MImumInSettingsDataCollection;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.0
*/
NONSHARABLE_CLASS( CImumCreatorPredefinedMailbox ) : public CBase
    {
    public: // Constructors and destructor
        static CImumCreatorPredefinedMailbox* NewL(
            CImumInternalApi& aMailboxApi );
        static CImumCreatorPredefinedMailbox* NewLC(
            CImumInternalApi& aMailboxApi );
        virtual ~CImumCreatorPredefinedMailbox();

    public: // New functions
        TBool DefinePredefinedMailboxL(
            const TUint aMailbox,
            CImumInSettingsData& aSettings );
    public: // Functions from base classes

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // Constructors
        CImumCreatorPredefinedMailbox( CImumInternalApi& aMailboxApi );
        void ConstructL();

    private:  // New virtual functions
    private:  // New functions
        void ReadSMTPParameters();
        void ReadPOP3Parameters();
        void ReadIMAP4Parameters();

        void CreateIMAP4MailboxL( CImumInSettingsData& aSettings );
        void CreatePOP3MailboxL( CImumInSettingsData& aSettings );
        void CreateGeneralSettingsL( CImumInSettingsData& aSettings );

        void CreateSMTPSettingsL( CImumInSettingsData& aSettings );
        void CreateIMAP4SettingsL( CImumInSettingsData& aSettings );
        void CreatePOP3SettingsL( CImumInSettingsData& aSettings );

        void ClearFields( const TUint aMailbox = 0 );

        /**
        *
        * @since S60 3.1
        */
        TBool ReadAndCreate();

        /**
        * Forms the key based on the id and fetches the string
        * @since S60 3.1
        * @param aId, Id of the key
        * @param aString, String to be updated
        * @return KErrNone, when the key can be used
        */
        TInt GetKey(
            TUint32 aId,
            TDes& aString );

        /**
        * Forms the key based on the id and fetches the integer
        * @since S60 3.1
        * @param aId, Id of the key
        * @param aString, String to be updated
        * @return KErrNone, when the key can be used
        */
        TInt GetKey(
            TUint32 aId,
            TInt& aInt );

        /**
         *
         *
         * @since S60 v3.2
         * @return
         */
        TBool ProtocolOk();

        /**
         *
         *
         * @since S60 v3.2
         * @return
         */
        TBool CompulsoryFilled();

        /**
         *
         *
         * @since S60 v3.2
         */
        void FixSettings();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         * @leave
         */
        void FillInLoginInfoL(
            const TDesC& aUsername,
            const TDesC& aPassword,
            MImumInSettingsDataCollection& aConnectionSettings ) const;

        /**
         * Convert Security values from keys_imum.xls format to match values in
         * ImumDaSettingsKeys.h. "Off" to "SSL" and vice versa.
         *
         * @since S60 v3.2
         * @param aSecurity, conversion result is returned via aSecurity.
         */
        void DoSecurityConversion( TInt& aSecurity ) const;

        /**
         * Determines the default port number based on security and
         * mail transfer protocol settings
         *
         * @since S60 v3.2
         * @param aSecurity, Security protocol used
         * @param aProtocol, Mail transfer protocol used
         * @return Port number
         */
        TInt GetDefaultSecurityPort(
            const TInt aSecurity,
            const TInt aProtocol );


    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        // Message Server Session
        CImumInternalApi& iMailboxApi;
        // Central Repository Session
        CRepository* iCenRepSession;
        // The current mailbox index
        TUint        iMailbox;
        // Compulsory field: Mailbox name
        RBuf         iMailboxName;
        // Compulsory field: Sending server
        RBuf         iMailboxSndServer;
        // Compulsory field: Receiving server
        RBuf         iMailboxRcvServer;
        // Compulsory field: Access point
        TInt         iMailboxAccessPoint;
        // Compulsory field: Protocol
        TInt         iMailboxProtocol;
        // Compulsory field: Receiving port number
        TInt         iMailboxRcvPort;
        // Compulsory field: Sending port number
        TInt         iMailboxSndPort;
        // Optional field: Email address
        RBuf         iMailboxEmailAddress;
        // Optional field: Fill CC field with own email address
        TInt         iMailboxFillCC;
        // Optional field: Send message delay (immediately/next connection)
        TInt         iMailboxMessageSend;
        // Optional field: Sending server security option
        TInt         iMailboxSndSecurity;
        // Optional field: APop setting
        TInt         iMailboxAPop;
        // Optional field: Receiving server security option
        TInt         iMailboxRcvSecurity;
        // Optional field: Download settings
        TInt         iMailboxFetchStates;
        // Optional field: Number of headers to be fetched
        TInt         iMailboxHeaderCount;
        // Optional field: Username to receiving mailbox server
        RBuf         iMailboxRcvUsername;
        // Optional field: Password to receiving mailbox server
        RBuf         iMailboxRcvPassword;
        // Optional field: Username to sending mailbox server
        RBuf         iMailboxSndUsername;
        // Optional field: Password to sending mailbox server
        RBuf         iMailboxSndPassword;
        // Optional field: OMA Email Notifications
        TInt         iMailboxOmaEmnEnabled;
    };

#endif      // IMUMCREATORPREDEFINEDMAILBOX_H

// End of File
