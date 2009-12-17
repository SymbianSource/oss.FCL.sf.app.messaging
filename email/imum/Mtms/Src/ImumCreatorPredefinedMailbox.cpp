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
* Description:  ImumCreatorPredefinedMailbox.cpp
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <smtpset.h>                        // SMTP enumerations
#include <imapset.h>                        // IMAP4 mail settings
#include <pop3set.h>                        // POP3 mail settings
#include <iapprefs.h>
#include <SendUiConsts.h>

#include "ImumDaSettingsKeys.h"
#include "ImumInSettingsKeys.h"
#include "ImumCreatorPredefinedMailbox.h"
#include "ImumPrivateCRKeys.h"                  // Predefined mailbox keys
#include "EmailUtils.H"
#include "ImumInternalApi.h"                // CImumInternalApi
#include "ImumInSettingsData.h"
#include "ImumInSettingsDataCollection.h"
#include "ImumMtmLogging.h"
#include "ImumDaErrorCodes.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
const TInt KImumMailboxIndex = 0x100;
const TInt KImumMboxMaxTextLength = 500;
const TInt KImumConvertValue = 0x02;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::Newl()
// ----------------------------------------------------------------------------
//
CImumCreatorPredefinedMailbox* CImumCreatorPredefinedMailbox::NewL(
    CImumInternalApi& aMailboxApi )
    {
    IMUM_STATIC_CONTEXT( CImumCreatorPredefinedMailbox::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();

    // Create the object
    CImumCreatorPredefinedMailbox* self = NewLC( aMailboxApi );
    CleanupStack::Pop(); // self

    IMUM_OUT();

    return self;
    }

// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::NewlC()
// ----------------------------------------------------------------------------
//
CImumCreatorPredefinedMailbox* CImumCreatorPredefinedMailbox::NewLC(
    CImumInternalApi& aMailboxApi )
    {
    IMUM_STATIC_CONTEXT( CImumCreatorPredefinedMailbox::NewLC, 0, mtm, KImumMtmLog );
    IMUM_IN();

    // Create the object
    CImumCreatorPredefinedMailbox* self =
        new ( ELeave ) CImumCreatorPredefinedMailbox( aMailboxApi );
    CleanupStack::PushL( self );

    // Constuct the object
    self->ConstructL();

    IMUM_OUT();

    return self;
    }

// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::~CImumCreatorPredefinedMailbox()
// ----------------------------------------------------------------------------
//
CImumCreatorPredefinedMailbox::~CImumCreatorPredefinedMailbox()
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::~CImumCreatorPredefinedMailbox, 0, KImumMtmLog );
    IMUM_IN();

    delete iCenRepSession;
    iCenRepSession = NULL;

    iMailboxName.Close();
    iMailboxRcvServer.Close();
    iMailboxSndServer.Close();
    iMailboxEmailAddress.Close();
    iMailboxRcvUsername.Close();
    iMailboxRcvPassword.Close();
    iMailboxSndUsername.Close();
    iMailboxSndPassword.Close();

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::CImumCreatorPredefinedMailbox()
// ----------------------------------------------------------------------------
//
CImumCreatorPredefinedMailbox::CImumCreatorPredefinedMailbox(
    CImumInternalApi& aMailboxApi )
    :
    iMailboxApi( aMailboxApi ),
    iCenRepSession( NULL )
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::CImumCreatorPredefinedMailbox, 0, KImumMtmLog );
    IMUM_IN();

    // Clear all fields
    ClearFields();

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumCreatorPredefinedMailbox::ConstructL()
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::ConstructL, 0, KImumMtmLog );
    IMUM_IN();

    iCenRepSession = CRepository::NewL( KCRUidImum );

    iMailboxName.CreateMax( KImumMboxMaxTextLength );
    iMailboxRcvServer.CreateMax( KImumMboxMaxTextLength );
    iMailboxSndServer.CreateMax( KImumMboxMaxTextLength );
    iMailboxEmailAddress.CreateMax( KImumMboxMaxTextLength );
    iMailboxRcvUsername.CreateMax( KImumMboxMaxTextLength );
    iMailboxRcvPassword.CreateMax( KImumMboxMaxTextLength );
    iMailboxSndUsername.CreateMax( KImumMboxMaxTextLength );
    iMailboxSndPassword.CreateMax( KImumMboxMaxTextLength );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumCreatorPredefinedMailbox::ReadAndCreate()
// ----------------------------------------------------------------------------
//
TBool CImumCreatorPredefinedMailbox::ReadAndCreate()
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::ReadAndCreate, 0, KImumMtmLog );
    IMUM_IN();

    // Read SMTP settings and validate them
    ReadSMTPParameters();
    TBool create = EFalse;

    // Continue settings reading, if everything is fine
    if ( ProtocolOk() )
        {
        IMUM0( 0, "Protocol information ok, proceed with reading receiving settings" );
        // Read settings based on protocol
        if ( !iMailboxProtocol )
            {
            IMUM0( 0, "Read POP3 Parameters" );

            // Read POP3 settings and validate them
            ReadPOP3Parameters();
            }
        else
            {
            IMUM0( 0, "Read IMAP4 Parameters" );

            // Read IMAP4 settings and validate them
            ReadIMAP4Parameters();
            }

        create = CompulsoryFilled();
        }

    // Settings are ok
    if ( create )
        {
        IMUM0( 0, "Mailbox can be created, perform final finetuning" );
        FixSettings();
        }

    IMUM_OUT();
    return create;
    }

// ---------------------------------------------------------------------------
// From class .
// CImumCreatorPredefinedMailbox::ProtocolOk()
// ---------------------------------------------------------------------------
//
TBool CImumCreatorPredefinedMailbox::ProtocolOk()
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::ProtocolOk, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();

    return ( iMailboxProtocol == 0 || iMailboxProtocol == 1 );
    }


// ---------------------------------------------------------------------------
// CImumCreatorPredefinedMailbox::CompulsoryFilled()
// ---------------------------------------------------------------------------
//
TBool CImumCreatorPredefinedMailbox::CompulsoryFilled()
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::CompulsoryFilled, 0, KImumMtmLog );
    IMUM_IN();

    TBool sendServerOk = iMailboxSndServer.Length() > 0;
    TBool rcvServerOk = iMailboxRcvServer.Length() > 0;
    TBool nameOk = iMailboxName.Length() > 0;

    IMUM_OUT();

    return sendServerOk && rcvServerOk && nameOk;
    }

// ---------------------------------------------------------------------------
// CImumCreatorPredefinedMailbox::FixSettings()
// ---------------------------------------------------------------------------
//
void CImumCreatorPredefinedMailbox::FixSettings()
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::FixSettings, 0, KImumMtmLog );
    IMUM_IN();

    // Set correct flag for the attachment setting
    TBool set = iMailboxFetchStates > 0;
    iMailboxFetchStates = set ?
        TImumInSettings::EFlagDownloadHeader:
        TImumInSettings::EFlagSetDownloadAll;

    IMUM_OUT();
    }


// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::DefinePredefinedMailboxL()
// ----------------------------------------------------------------------------
//
TBool CImumCreatorPredefinedMailbox::DefinePredefinedMailboxL(
    const TUint aMailbox,
    CImumInSettingsData& aSettings )
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::DefinePredefinedMailboxL, 0, KImumMtmLog );
    IMUM_IN();

    // Clear all fields
    ClearFields( aMailbox );
    aSettings.ResetAll();

    // Create settings if things are fine
    if ( ReadAndCreate() )
        {
        // Check the mailbox type
        if ( !iMailboxProtocol )
            {
            CreatePOP3MailboxL( aSettings );
            }
        else
            {
            CreateIMAP4MailboxL( aSettings );
            }
        }

    IMUM_OUT();
    return !aSettings.Validate();
    }

// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::ReadSMTPParameters()
// ----------------------------------------------------------------------------
//
void CImumCreatorPredefinedMailbox::ReadSMTPParameters()
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::ReadSMTPParameters, 0, KImumMtmLog );
    IMUM_IN();

    // Get mailbox name
    GetKey( KImumMailName1, iMailboxName );

    // Get protocol
    GetKey( KImumMailProtocol1, iMailboxProtocol );

    // Get sending server address
    GetKey( KImumMailSendingHost1, iMailboxSndServer );

    // Get access point
    GetKey( KImumMailIAP1, iMailboxAccessPoint );

    // Get own email address
    GetKey( KImumMailOwnAddress1, iMailboxEmailAddress );

    // Get copy email address to CC setting
    GetKey( KImumMailCopyToOwn1, iMailboxFillCC );

    // Get sending delay setting
    GetKey( KImumMailSendMessage1, iMailboxMessageSend );

    // Get sending security setting
    GetKey( KImumSmtpSecurity1, iMailboxSndSecurity );
    DoSecurityConversion( iMailboxSndSecurity );

    // Get OMA Email Notifications setting
    GetKey( KImumOmaEmnEnabled1, iMailboxOmaEmnEnabled );

    // Get username setting
    GetKey( KImumMailOutgoingUsername1, iMailboxSndUsername );

    // Get password setting
    GetKey( KImumMailOutgoingPassword1, iMailboxSndPassword );

    // Get sending port number setting
    GetKey( KImumMailOutgoingPortNumber1, iMailboxSndPort );

    if(iMailboxSndPort <= 0)
        {
        iMailboxSndPort = GetDefaultSecurityPort( iMailboxSndSecurity,
        										  TImumDaSettings::EValueProtocolSmtp );
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::ReadPOP3Parameters()
// ----------------------------------------------------------------------------
//
void CImumCreatorPredefinedMailbox::ReadPOP3Parameters()
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::ReadPOP3Parameters, 0, KImumMtmLog );
    IMUM_IN();

    // Get receiving server address
    GetKey( KImumMailReceivingHost1, iMailboxRcvServer );

    // Get APop settings
    GetKey( KImumMailAPop1, iMailboxAPop );

     // Get header fetching setting
    GetKey( KImumMailFetchHeaders1, iMailboxHeaderCount );

    // Get username setting
    GetKey( KImumMailIncomingUsername1, iMailboxRcvUsername );

    // Get password setting
    GetKey( KImumMailIncomingPassword1, iMailboxRcvPassword );

    // Get receiving security setting
    GetKey( KImumMailSecurity1, iMailboxRcvSecurity );
    DoSecurityConversion( iMailboxRcvSecurity );

    // Get receiving port number
    GetKey( KImumMailIncomingPortNumber1, iMailboxRcvPort );

    if(iMailboxRcvPort <= 0)
        {
        iMailboxRcvPort = GetDefaultSecurityPort( iMailboxRcvSecurity,
        										  TImumDaSettings::EValueProtocolPop3 );
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::ReadIMAP4Parameters()
// ----------------------------------------------------------------------------
//
void CImumCreatorPredefinedMailbox::ReadIMAP4Parameters()
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::ReadIMAP4Parameters, 0, KImumMtmLog );
    IMUM_IN();

    // Get receiving host
    GetKey( KImumMailReceivingHost1, iMailboxRcvServer );

    // Get attachment fetching setting
    GetKey( KImumMailFetchAttachment1, iMailboxFetchStates );

    // Get header fetching setting
    GetKey( KImumMailFetchHeaders1, iMailboxHeaderCount );

    // Get username setting
    GetKey( KImumMailIncomingUsername1, iMailboxRcvUsername );

    // Get password setting
    GetKey( KImumMailIncomingPassword1, iMailboxRcvPassword );

    // Get receiving security setting
    GetKey( KImumMailSecurity1, iMailboxRcvSecurity );
    DoSecurityConversion( iMailboxRcvSecurity );

    // Get receiving port number
    GetKey( KImumMailIncomingPortNumber1, iMailboxRcvPort );

    if(iMailboxRcvPort <= 0)
        {
        iMailboxRcvPort = GetDefaultSecurityPort( iMailboxRcvSecurity,
        										  TImumDaSettings::EValueProtocolImap4 );
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::CreateIMAP4MailboxL()
// ----------------------------------------------------------------------------
//
void CImumCreatorPredefinedMailbox::CreateIMAP4MailboxL(
    CImumInSettingsData& aSettings )
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::CreateIMAP4MailboxL, 0, KImumMtmLog );
    IMUM_IN();

    CreateIMAP4SettingsL( aSettings );
    CreateSMTPSettingsL( aSettings );
    CreateGeneralSettingsL( aSettings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::CreatePOP3MailboxL()
// ----------------------------------------------------------------------------
//
void CImumCreatorPredefinedMailbox::CreatePOP3MailboxL(
    CImumInSettingsData& aSettings )
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::CreatePOP3MailboxL, 0, KImumMtmLog );
    IMUM_IN();

    CreatePOP3SettingsL( aSettings );
    CreateSMTPSettingsL( aSettings );
    CreateGeneralSettingsL( aSettings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::CreateGeneralSettingsL()
// ----------------------------------------------------------------------------
//
void CImumCreatorPredefinedMailbox::CreateGeneralSettingsL(
    CImumInSettingsData& aSettings )
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::CreateGeneralSettingsL, 0, KImumMtmLog );
    IMUM_IN();

    // Set OMA EMN state
    aSettings.SetAttr(
        TImumDaSettings::EKeyAutoNotifications, iMailboxOmaEmnEnabled );

    // Add email address to settings
    aSettings.SetAttr(
        TImumDaSettings::EKeyEmailAddress, iMailboxEmailAddress );

    // Set mail address can be empty
    aSettings.SetAttr(
        TImumInSettings::EKeyAllowEmptyEmailAddress, ETrue );

    // Set mailbox name
    aSettings.SetAttr(
        TImumDaSettings::EKeyMailboxName, iMailboxName );

    // Generate mailbox name when necessary
    aSettings.SetAttr(
        TImumInSettings::EKeyGenerateMailboxName, ETrue );

    // Set send message delay
    aSettings.SetAttr(
        TImumDaSettings::EKeySendDelay, iMailboxMessageSend );

    // Set send copy to self
    aSettings.SetAttr(
        TImumDaSettings::EKeySendCopyToSelf, iMailboxFillCC );

    // Update inbox update limit
    aSettings.SetAttr(
        TImumDaSettings::EKeyInboxUpdateLimit,
        static_cast<TInt32>( iMailboxHeaderCount ) );

    // Update folder update limit
    aSettings.GetInSetL( 0 ).SetAttr(
        TImumDaSettings::EKeyFolderUpdateLimit,
        static_cast<TInt32>( iMailboxHeaderCount ) );

    // Set download flags
    aSettings.GetInSetL( 0 ).SetAttr(
        TImumInSettings::EKeyDownloadFlags,
        static_cast<TInt32>( iMailboxFetchStates ) );

    // Set predefined mailbox flag
    aSettings.SetAttr(
        TImumInSettings::EKeyIsPredefinedMailbox, ETrue );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::CreateSMTPSettingsL()
// ----------------------------------------------------------------------------
//
void CImumCreatorPredefinedMailbox::CreateSMTPSettingsL(
    CImumInSettingsData& aSettings )
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::CreateSMTPSettingsL, 0, KImumMtmLog );
    IMUM_IN();

    // Create Imap4 settings
    MImumInSettingsDataCollection& smtpSettings =
        aSettings.AddInSetL( KSenduiMtmSmtpUid );

    // Add security setting to settings
    smtpSettings.SetAttr(
        TImumDaSettings::EKeyAccessPoint, iMailboxAccessPoint );

    // Add access point detail to settings
    smtpSettings.SetAttr(
        TImumInSettings::EKeyAccessPointType,
        TImumInSettings::EValueApIsIapId );

    // Add server address to settings
    smtpSettings.SetAttr(
        TImumDaSettings::EKeyServer, iMailboxSndServer );

    // Add security settings
    smtpSettings.SetAttr(
        TImumDaSettings::EKeySecurity, iMailboxSndSecurity );

    // Add sending port number settings
    smtpSettings.SetAttr(
        TImumDaSettings::EKeyPort, iMailboxSndPort );

    // Fill in the login info
    FillInLoginInfoL( iMailboxSndUsername, iMailboxSndPassword, smtpSettings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::CreateIMAP4SettingsL()
// ----------------------------------------------------------------------------
//
void CImumCreatorPredefinedMailbox::CreateIMAP4SettingsL(
    CImumInSettingsData& aSettings )
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::CreateIMAP4SettingsL, 0, KImumMtmLog );
    IMUM_IN();

    // Create Imap4 settings
    MImumInSettingsDataCollection& imap4Settings =
        aSettings.AddInSetL( KSenduiMtmImap4Uid );

    // Add security setting to settings
    imap4Settings.SetAttr(
        TImumDaSettings::EKeyAccessPoint, iMailboxAccessPoint );

    // Add access point detail to settings
    imap4Settings.SetAttr(
        TImumInSettings::EKeyAccessPointType,
        TImumInSettings::EValueApIsIapId );

    // Add server address to settings
    imap4Settings.SetAttr(
        TImumDaSettings::EKeyServer, iMailboxRcvServer );

    // Add security settings
    imap4Settings.SetAttr(
        TImumDaSettings::EKeySecurity, iMailboxRcvSecurity );

    // Add receiving port number settings
    imap4Settings.SetAttr(
        TImumDaSettings::EKeyPort, iMailboxRcvPort );

    // Fill in the login info
    FillInLoginInfoL( iMailboxRcvUsername, iMailboxRcvPassword, imap4Settings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::CreatePOP3SettingsL()
// ----------------------------------------------------------------------------
//
void CImumCreatorPredefinedMailbox::CreatePOP3SettingsL(
    CImumInSettingsData& aSettings )
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::CreatePOP3SettingsL, 0, KImumMtmLog );
    IMUM_IN();

    // Create Imap4 settings
    MImumInSettingsDataCollection& pop3Settings =
        aSettings.AddInSetL( KSenduiMtmPop3Uid );

    // Add access point to settings
    pop3Settings.SetAttr(
        TImumDaSettings::EKeyAccessPoint, iMailboxAccessPoint );

    // Add access point detail to settings
    pop3Settings.SetAttr(
        TImumInSettings::EKeyAccessPointType,
        TImumInSettings::EValueApIsIapId );

    // Add server address to settings
    pop3Settings.SetAttr(
        TImumDaSettings::EKeyServer, iMailboxRcvServer );

    // Add security settings
    pop3Settings.SetAttr(
        TImumDaSettings::EKeySecurity, iMailboxRcvSecurity );

    // Add APOP setting
    pop3Settings.SetAttr(
        TImumDaSettings::EKeyAPop, iMailboxAPop );

    // Add receiving port number settings
    pop3Settings.SetAttr(
        TImumDaSettings::EKeyPort, iMailboxRcvPort );


    // Fill in the login info
    FillInLoginInfoL( iMailboxRcvUsername, iMailboxRcvPassword, pop3Settings );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumCreatorPredefinedMailbox::FillInLoginInfoL()
// ---------------------------------------------------------------------------
//
void CImumCreatorPredefinedMailbox::FillInLoginInfoL(
    const TDesC& aUsername,
    const TDesC& aPassword,
    MImumInSettingsDataCollection& aConnectionSettings ) const
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::FillInLoginInfoL, 0, KImumMtmLog );
    IMUM_IN();

    // Convert to 8-bit format
    RBuf8 temp;
    temp.CreateMax( KImumMboxMaxTextLength );
    temp.Copy( aUsername );
    aConnectionSettings.SetAttr( TImumDaSettings::EKeyUsername, temp );

    temp.Copy( aPassword );
    aConnectionSettings.SetAttr( TImumDaSettings::EKeyPassword, temp );

    // Reset descriptors
    temp.Close();

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
//  CImumCreatorPredefinedMailbox::ClearFields()
// ----------------------------------------------------------------------------
//
void CImumCreatorPredefinedMailbox::ClearFields( const TUint aMailbox )
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::ClearFields, 0, KImumMtmLog );
    IMUM1_IN( "Resetting with mailbox: %d", aMailbox );

    iMailbox = aMailbox;
    iMailboxName.Zero();
    iMailboxSndServer.Zero();
    iMailboxRcvServer.Zero();
    iMailboxAccessPoint = KErrUnknown;
    iMailboxProtocol = KErrUnknown;
    iMailboxEmailAddress.Zero();
    iMailboxFillCC = KErrUnknown;
    iMailboxMessageSend = KErrUnknown;
    iMailboxSndSecurity = KErrUnknown;
    iMailboxAPop = KErrUnknown;
    iMailboxRcvSecurity = KErrUnknown;
    iMailboxHeaderCount = KErrUnknown;
    iMailboxRcvUsername.Zero();
    iMailboxRcvPassword.Zero();
    iMailboxSndUsername.Zero();
    iMailboxSndPassword.Zero();
    iMailboxOmaEmnEnabled = KErrUnknown;
    iMailboxRcvPort = KErrUnknown;
    iMailboxSndPort = KErrUnknown;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumCreatorPredefinedMailbox::GetKey()
// ----------------------------------------------------------------------------
//
TInt CImumCreatorPredefinedMailbox::GetKey(
    TUint32 aId,
    TDes& aString )
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::GetKey, 0, KImumMtmLog );
    IMUM2_IN( "aId = %d, iMailbox = %d", aId, iMailbox );

    // Get the mailbox by index
    aId += iMailbox * KImumMailboxIndex;
    TInt error = iCenRepSession->Get( aId, aString );

    IMUM3( 0, "Key (%d) loading result: error = %d, aString = %S", aId, error, &aString );
    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumCreatorPredefinedMailbox::GetKey()
// ----------------------------------------------------------------------------
//
TInt CImumCreatorPredefinedMailbox::GetKey(
    TUint32 aId,
    TInt& aInt)
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::GetKey, 0, KImumMtmLog );
    IMUM2_IN( "aId = %d, iMailbox = %d", aId, iMailbox );

    // Get the mailbox by index
    aId += iMailbox * KImumMailboxIndex;
    TInt error = iCenRepSession->Get( aId, aInt );

    IMUM3( 0, "Key (%d) loading result: error = %d, aInt = %d", aId, error, aInt );
    IMUM_OUT();
    return error;
    }

// ---------------------------------------------------------------------------
// CImumCreatorPredefinedMailbox::DoSecurityConversion()
// ---------------------------------------------------------------------------
//
void CImumCreatorPredefinedMailbox::DoSecurityConversion( TInt& aSecurity ) const
    {
    //Conversion concerns only even numbers
    if ( ( aSecurity % 2 ) == 0 )
        {
        //convert 0 -> 2 and 2 -> 0
        aSecurity ^= KImumConvertValue;
        }
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataValidator::GetDefaultSecurityPortL()
// ----------------------------------------------------------------------------
//
TInt CImumCreatorPredefinedMailbox::GetDefaultSecurityPort(
    const TInt aSecurity,
    const TInt aProtocol )
    {
    IMUM_CONTEXT( CImumCreatorPredefinedMailbox::GetDefaultSecurityPortL, 0, KImumMtmLog );
    IMUM_IN();

    TInt port( 0 );

    switch( aSecurity )
        {
        case KErrUnknown: //value is not set at all
        case TImumDaSettings::EValueSecurityTls:
        case TImumDaSettings::EValueSecurityOff:
            {
            switch ( aProtocol )
                {
                case TImumDaSettings::EValueProtocolImap4:
                    port = TImumDaSettings::EDefaultPortImap4;
                    break;

                case TImumDaSettings::EValueProtocolPop3:
                    port = TImumDaSettings::EDefaultPortPop3;
                    break;

                case TImumDaSettings::EValueProtocolSmtp:
                    port = TImumDaSettings::EDefaultPortSmtp;
                    break;
                }
            break;
            }
        case TImumDaSettings::EValueSecuritySsl:
            {
            switch ( aProtocol )
                {
                case TImumDaSettings::EValueProtocolImap4:
                    port = TImumDaSettings::EDefaultSecurityPortImap4;
                    break;

                case TImumDaSettings::EValueProtocolPop3:
                    port = TImumDaSettings::EDefaultSecurityPortPop3;
                    break;

                case TImumDaSettings::EValueProtocolSmtp:
                    port = TImumDaSettings::EDefaultSecurityPortSmtp;
                    break;
                }
            }
            break;
        }

    IMUM_OUT();

    return port;
    }


//  End of File

