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
* Description:  ImumMboxDefaultData.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <imapset.h>
#include <SendUiConsts.h>
#include <centralrepository.h>

#include "ImumPanic.h"
#include "ImumMboxDefaultData.h"            // KImapDefaultFetchSizeBytes
#include "ImumInSettingsDataArray.h"
#include "ImumDaErrorCodes.h"
#include "ImumInSettingsKeys.h"
#include "ImumUtilsLogging.h"
#include "ImumConstants.h"
#include "ImumPrivateCRKeys.h"
#include "IMASCenRepControl.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CImumMboxDefaultData::CImumMboxDefaultData()
// ----------------------------------------------------------------------------
//
CImumMboxDefaultData::CImumMboxDefaultData(
    CImumInternalApiImpl& aMailboxApi )
    :
    CImumInSettingsDataImpl( aMailboxApi ),
    iCenRepControl( NULL ),
    iImumRepository( NULL )
    {
    IMUM_CONTEXT( CImumMboxDefaultData::CImumMboxDefaultData, 0, KLogData );
    IMUM_IN();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxDefaultData::~CImumMboxDefaultData()
// ----------------------------------------------------------------------------
//
CImumMboxDefaultData::~CImumMboxDefaultData()
    {
    IMUM_CONTEXT( CImumMboxDefaultData::~CImumMboxDefaultData, 0, KLogData );
    IMUM_IN();

    delete iCenRepControl;
    iCenRepControl = NULL;
    delete iImumRepository;
    iImumRepository = NULL;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// From class CImumInSettingsDataImpl.
// CImumMboxDefaultData::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumMboxDefaultData::ConstructL()
    {
    IMUM_CONTEXT( CImumMboxDefaultData::ConstructL, 0, KLogData );
    IMUM_IN();

    iSettings = CImumInSettingsDataArray::NewL();
    iConnectionSettings =
        new ( ELeave ) CImumInConnSettings( KImumInConnSettingsGranularity );

    iCenRepControl = CIMASCenRepControl::NewL();
    iImumRepository = CRepository::NewL( KCRUidImum );

    // Prepare default data reading
    iCenRepControl->SetActiveCentralRepository( *iImumRepository );
    iCenRepControl->SetActiveDefaultDataKey();

    AddInSetL( KSenduiMtmImap4Uid );
    AddInSetL( KSenduiMtmPop3Uid );
    AddInSetL( KSenduiMtmSmtpUid );

    ResetAll();

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxDefaultData::NewL()
// ----------------------------------------------------------------------------
//
CImumMboxDefaultData* CImumMboxDefaultData::NewL(
    CImumInternalApiImpl& aMailboxApi )
    {
    IMUM_STATIC_CONTEXT( CImumMboxDefaultData::NewL, 0, utils, KLogData );
    IMUM_IN();

    CImumMboxDefaultData* self = NewLC( aMailboxApi );
    CleanupStack::Pop( self );

    IMUM_OUT();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumMboxDefaultData::NewLC()
// ----------------------------------------------------------------------------
//
CImumMboxDefaultData* CImumMboxDefaultData::NewLC(
    CImumInternalApiImpl& aMailboxApi )
    {
    IMUM_STATIC_CONTEXT( CImumMboxDefaultData::NewLC, 0, utils, KLogData );
    IMUM_IN();

    CImumMboxDefaultData* self =
        new ( ELeave ) CImumMboxDefaultData( aMailboxApi );
    CleanupStack::PushL( self );
    self->ConstructL();

    IMUM_OUT();

    return self;
    }

// ---------------------------------------------------------------------------
// From class CImumInSettingsDataImpl.
// CImumMboxDefaultData::AddSetL()
// ---------------------------------------------------------------------------
//
MImumDaSettingsDataCollection& CImumMboxDefaultData::AddSetL(
    const TUid& aProtocol )
    {
    IMUM_CONTEXT( CImumMboxDefaultData::AddSetL, 0, KLogInApi );
    IMUM_IN();
    IMUM_OUT();
    return AddInSetL( aProtocol );
    }

// ---------------------------------------------------------------------------
// From class CImumInSettingsDataImpl.
// CImumMboxDefaultData::AddInSetL()
// ---------------------------------------------------------------------------
//
MImumInSettingsDataCollection& CImumMboxDefaultData::AddInSetL(
    const TUid& aProtocol )
    {
    IMUM_CONTEXT( CImumMboxDefaultData::AddInSetL, 0, KLogInApi );
    IMUM_IN();

    CImumInSettingsDataCollectionImpl* connSettings =
        CImumInSettingsDataCollectionImpl::NewLC(
            aProtocol,
            SelectDefaultData( aProtocol, iDefaultData ),
            *iValidator );
    iConnectionSettings->AppendL( connSettings );
    CleanupStack::Pop( connSettings );

    IMUM_OUT();

    return *connSettings;
    }

// ---------------------------------------------------------------------------
// From class CImumInSettingsDataImpl.
// CImumMboxDefaultData::Validate()
// ---------------------------------------------------------------------------
//
TInt CImumMboxDefaultData::Validate()
    {
    // Return KErrNotSupported to avoid possible usage of default data with
    // the mailbox creation function. This way, no mailboxes are attempted to
    // be created based on default settings. This prevents unexpected
    // crashes and also spots possible problems in the code in early
    // development.
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// From class CImumInSettingsDataImpl.
// CImumMboxDefaultData::Reset()
// ---------------------------------------------------------------------------
//
void CImumMboxDefaultData::Reset( const TUint /* aAttributeKey */ )
    {
    // Just reset whole ...thing
    ResetAll();
    }

// ---------------------------------------------------------------------------
// From class CImumInSettingsDataImpl.
// CImumMboxDefaultData::ResetAll()
// ---------------------------------------------------------------------------
//
void CImumMboxDefaultData::ResetAll()
    {
    ResetImap4Data();
    ResetPop3Data();
    ResetSmtpData();
    ResetGeneralData();
    }

// ---------------------------------------------------------------------------
// CImumMboxDefaultData::ResetImap4Data()
// ---------------------------------------------------------------------------
//
void CImumMboxDefaultData::ResetImap4Data()
    {
    MImumInSettingsDataCollection& imap4 =
        *iConnectionSettings->At( EImumMboxDefaultDataIndexImap4 );

    // Download control settings
    imap4.SetAttr(
        TImumInSettings::EKeySyncFlags,
        KImumMboxDefaultSyncFlags[EImumMboxDefaultDataIndexImap4] );
    imap4.SetAttr(
        TImumInSettings::EKeyDownloadFlags,
        KImumMboxDefaultDownloadFlags[EImumMboxDefaultDataIndexImap4] );
    imap4.SetAttr(
        TImumInSettings::EKeyDownloadBodySize,
        KImumMboxDefaultDownloadBodySize );
    imap4.SetAttr(
        TImumInSettings::EKeyDownloadAttachmentSize,
        KImumMboxDefaultDownloadAttachmentSize );
    imap4.SetAttr(
        TImumInSettings::EKeyDownloadMaxEmailSize,
        KImumMboxDefaultDownloadMaxEmailSize );
    imap4.SetAttr(
        TImumInSettings::EKeyImapIdle,
        KImumMboxDefaultImapIdle );
    imap4.SetAttr(
        TImumInSettings::EKeyImapIdleTimeout,
        KImumMboxDefaultImapIdleTimeout );
    imap4.SetAttr(
        TImumDaSettings::EImapFolderPath,
        KImumMboxDefaultpFolderPath );
    imap4.SetAttr(
        TImumInSettings::EKeySyncRate,
        KImumMboxDefaultSyncRate );
    }

// ---------------------------------------------------------------------------
// CImumMboxDefaultData::ResetPop3Data()
// ---------------------------------------------------------------------------
//
void CImumMboxDefaultData::ResetPop3Data()
    {
    MImumInSettingsDataCollection& pop3 =
        *iConnectionSettings->At( EImumMboxDefaultDataIndexPop3 );

    // Download control settings
    pop3.SetAttr(
        TImumInSettings::EKeySyncFlags,
        KImumMboxDefaultSyncFlags[EImumMboxDefaultDataIndexPop3] );
    pop3.SetAttr(
        TImumInSettings::EKeyDownloadFlags,
        KImumMboxDefaultDownloadFlags[EImumMboxDefaultDataIndexPop3] );
    pop3.SetAttr(
        TImumInSettings::EKeyDownloadBodySize,
        KImumMboxDefaultDownloadBodySize );
    pop3.SetAttr(
        TImumInSettings::EKeyDownloadMaxEmailSize,
        KImumMboxDefaultDownloadMaxEmailSize );
    pop3.SetAttr(
        TImumDaSettings::EKeyAPop,
        KImumMboxDefaultAPop );
    }

// ---------------------------------------------------------------------------
// CImumMboxDefaultData::ResetSmtpData()
// ---------------------------------------------------------------------------
//
void CImumMboxDefaultData::ResetSmtpData()
    {
    // Keep this here reserving for future use
    }

// ---------------------------------------------------------------------------
// CImumMboxDefaultData::ResetGeneralData()
// ---------------------------------------------------------------------------
//
void CImumMboxDefaultData::ResetGeneralData()
    {
    IMUM_CONTEXT( CImumMboxDefaultData::ResetDefaultData, 0, KLogData );
    IMUM_IN();

    // Reset connection settings
    for ( TInt index = iConnectionSettings->Count(); --index >= 0; )
        {
        CImumInSettingsDataCollectionImpl& connSet =
            *iConnectionSettings->At( index );

        connSet.SetAttr(
            TImumDaSettings::EKeyAccessPoint,
            KImumMboxDefaultAccessPoint );
        connSet.SetAttr(
            TImumInSettings::EKeyAccessPointType,
            KImumMboxDefaultAccessPointType );
        connSet.SetAttr(
            TImumInSettings::EKeyAccessPointName,
            KImumMboxDefaultAccessPointName );
        connSet.SetAttr(
            TImumDaSettings::EKeyUsername,
            KImumMboxDefaultUsername );
        connSet.SetAttr(
            TImumInSettings::EKeyTemporaryUsername,
            KImumMboxDefaultTemporaryUsername );
        connSet.SetAttr(
            TImumDaSettings::EKeyPassword,
            KImumMboxDefaultPassword );
        connSet.SetAttr(
            TImumInSettings::EKeyTemporaryPassword,
            KImumMboxDefaultTemporaryPassword );
        connSet.SetAttr(
            TImumDaSettings::EKeyPort,
            KImumMboxDefaultPort );
        connSet.SetAttr(
            TImumDaSettings::EKeySecurity,
            KImumMboxDefaultSecurity );
        }

    SetAttr(
        TImumDaSettings::EKeyEmailAddress,
        KImumMboxDefaultEmailAddress );
    SetAttr(
        TImumDaSettings::EKeyMailboxName,
        KImumMboxDefaultMailboxName );
    SetAttr(
        TImumDaSettings::EKeyUserAlias,
        KImumMboxDefaultUserAlias );
    SetAttr(
        TImumDaSettings::EKeyReplyToAddress,
        KImumMboxDefaultReplyToAddress );
    SetAttr(
        TImumDaSettings::EKeySendDelay,
        KImumMboxDefaultSendDelay );
    SetAttr(
        TImumDaSettings::EKeySendCopyToSelf,
        KImumMboxDefaultSendCopyToSelf );
    SetAttr(
        TImumDaSettings::EKeyIncludeSignature,
        KImumMboxDefaultIncludeSignature );
    SetAttr(
        TImumDaSettings::EKeySignature,
        KImumMboxDefaultSignature );
    SetAttr(
        TImumDaSettings::EKeyEmailAlert,
        KImumMboxDefaultEmailAlert );
    SetAttr(
        TImumDaSettings::EKeyInboxUpdateLimit,
        KImumMboxDefaultInboxUpdateLimit );
    SetAttr(
        TImumDaSettings::EKeyFolderUpdateLimit,
        KImumMboxDefaultFolderUpdateLimit );

    UpdateData( *this, TImumDaSettings::EKeyMailDeletion,
        KImumDefMailDeletionSettingMode, KImumMboxDefaultMailDeletion );

    SetAttr(
        TImumDaSettings::EKeyAutoNotifications,
        KImumMboxDefaultAutoNotifications );
    SetAttr(
        TImumDaSettings::EKeyAutoRetrieval,
        KImumMboxDefaultAutoRetrieval );
    SetAttr(
        TImumDaSettings::EKeyAutoRetrievalDays,
        KImumMboxDefaultAutoRetrievalDays );
    SetAttr(
        TImumDaSettings::EKeyAutoRetrievalStartHour,
        KImumMboxDefaultAutoRetrievalStartHour );
    SetAttr(
        TImumDaSettings::EKeyAutoRetrievalEndHour,
        KImumMboxDefaultAutoRetrievalEndHour );
    SetAttr(
        TImumDaSettings::EKeyAutoRetrievalInterval,
        KImumMboxDefaultAutoRetrievalInterval );

    // Internal setting keys
    SetAttr(
        TImumInSettings::EKeyHideMsgs,
        KImumMboxDefaultHideMsgs );
    SetAttr(
        TImumInSettings::EKeyOpenHtmlMail,
        KImumMboxDefaultOpenHtmlMail );
    SetAttr(
        TImumInSettings::EKeySearchString,
        KImumMboxDefaultSearchString );
    SetAttr(
        TImumInSettings::EKeyPathSeparator,
        KImumMboxDefaulthPathSeparator );
    SetAttr(
        TImumInSettings::EKeySynchronise,
        KImumMboxDefaultSynchronise );
    SetAttr(
        TImumInSettings::EKeySubscribe,
        KImumMboxDefaultSubscribe );
    SetAttr(
        TImumInSettings::EKeyUpdatingSeenFlag,
        KImumMboxDefaultUpdatingSeenFlag );
    SetAttr(
        TImumInSettings::EKeyFetchSize,
        KImumMboxDefaultFetchSize );
    SetAttr(
        TImumInSettings::EKeyReceiptAddress,
        KImumMboxDefaultReceiptAddress );
    SetAttr(
        TImumInSettings::EKeyBodyEncoding,
        KImumMboxDefaultBodyEncoding );
    SetAttr(
        TImumInSettings::EKeyDefaultMsgCharSet,
        KImumMboxDefaultDefaultMsgCharSet );
    SetAttr(
        TImumInSettings::EKeyAddVCardToEmail,
        KImumMboxDefaultAddVCardToEmail );
    SetAttr(
        TImumInSettings::EKeyRequestReceipts,
        KImumMboxDefaultRequestReceipts );
    SetAttr(
        TImumInSettings::EKeySMTPAuth,
        KImumMboxDefaultSMTPAuth );
    SetAttr(
        TImumInSettings::EKeyInboxLoginDetails,
        KImumMboxDefaultInboxLoginDetails );
    SetAttr(
        TImumInSettings::EKeyToCcIncludeLimit,
        KImumMboxDefaultToCcIncludeLimit );
    SetAttr(
        TImumInSettings::EKeyDisconnectedMode,
        KImumMboxDefaultDisconnectedMode );
    SetAttr(
        TImumInSettings::EKeyAutoSendOnConnect,
        KImumMboxDefaultAutoSendOnConnect );
    SetAttr(
        TImumInSettings::EKeyDeleteEmailsWhenDisconnecting,
        KImumMboxDefaultDeleteEmailsWhenDisconnecting );
    SetAttr(
        TImumInSettings::EKeyAcknowledgeReceipts,
        KImumMboxDefaultAcknowledgeReceipts );
    SetAttr(
        TImumInSettings::EKeySettingsVersion,
        KImumMboxDefaultSettingsVersion );
    SetAttr(
        TImumInSettings::EKeyAllowEmptyEmailAddress,
        KImumMboxDefaultAllowEmptyEmailAddress );
    SetAttr(
        TImumInSettings::EKeyGenerateMailboxName,
        KImumMboxDefaultGenerateMailboxName );
    SetAttr(
        TImumInSettings::EKeyInfoLastUpdateFailed,
        KImumMboxDefaultLastUpdateFailed );
    SetAttr(
        TImumInSettings::EKeyInfoLastSuccessfulUpdate,
        KImumMboxDefaultLastSuccessfulUpdate );
    SetAttr(
        TImumInSettings::EKeyAoUpdateSuccessfulWithCurSettings,
        KImumMboxUpdateSuccessfulWithCurSettings );
    SetAttr(
        TImumInSettings::EKeyDownloadBufferWlan,
        KImumMboxDefaultDownloadBufferWlan );
    SetAttr(
        TImumInSettings::EKeyDownloadBufferGprs,
        KImumMboxDefaultDownloadBufferGprs );
    SetAttr(
        TImumInSettings::EKeyIsPredefinedMailbox,
        KImumMboxDefaultIsPredefinedMailbox );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// From class CImumInSettingsDataImpl.
// CImumMboxDefaultData::Log()
// ---------------------------------------------------------------------------
//
void CImumMboxDefaultData::Log() const
    {
    // Do nothing here right now
    }

// ---------------------------------------------------------------------------
// CImumMboxDefaultData::DefaultConnectionData()
// ---------------------------------------------------------------------------
//
const CImumInSettingsDataCollectionImpl*
    CImumMboxDefaultData::DefaultConnectionData(
        const TInt aIndex ) const
    {
    return iConnectionSettings->At( aIndex );
    }

// ---------------------------------------------------------------------------
// CImumMboxDefaultData::CloneSettingsL()
// ---------------------------------------------------------------------------
//
CImumInSettingsDataArray* CImumMboxDefaultData::CloneSettingsL() const
    {
    return iSettings->CloneL();
    }

// ---------------------------------------------------------------------------
// CImumMboxDefaultData::CloneConnectionSettingsL()
// ---------------------------------------------------------------------------
//
CImumInSettingsDataCollectionImpl*
    CImumMboxDefaultData::CloneConnectionSettingsL(
        const TUid& aProtocol ) const
    {
    const CImumInSettingsDataCollectionImpl* defaultData =
        SelectDefaultData( aProtocol, this );
    CImumInSettingsDataCollectionImpl* clone = NULL;

    if ( defaultData )
        {
        clone = defaultData->DoCloneL();
        }
    else
        {
        SettingsDataExceptionL( TImumDaErrorCode::EConnectionSettingsNull );
        }

    return clone;
    }
