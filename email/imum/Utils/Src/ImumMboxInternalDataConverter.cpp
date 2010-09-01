/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ImumMboxInternalDataConverter.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <miutset.h>
#include <smtpset.h>
#include <pop3set.h>
#include <imapset.h>
#include <SendUiConsts.h>

#include "ImumMboxInternalDataConverter.h"
#include "ImumInSettingsDataCollection.h"
#include "ImumDaSettingsKeys.h"
#include "ImumInSettingsKeys.h"
#include "ImumDaErrorCodes.h"
#include "ImumPanic.h"
#include "ImumMboxData.h"
#include "ImumInSettingsDataImpl.h"
#include "ImumInSettingsDataCollectionImpl.h"
#include "ImumInMailboxServicesImpl.h"
#include "ImumInternalApiImpl.h"
#include "ComDbUtl.h"
#include "EmailUtils.H"
#include "ImumUtilsLogging.h"

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
// CImumMboxInternalDataConverter::CImumMboxInternalDataConverter()
// ----------------------------------------------------------------------------
//
CImumMboxInternalDataConverter::CImumMboxInternalDataConverter(
    CImumInternalApiImpl& aMailboxApi,
    const CImumMboxData& aSourceData,
    CImumInSettingsData& aTargetData )
    :
    iMailboxApi( aMailboxApi ),
    iTargetData( aTargetData ),
    iSourceData( aSourceData )
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::CImumMboxInternalDataConverter, 0, KLogData );

    }

// ----------------------------------------------------------------------------
// CImumMboxInternalDataConverter::~CImumMboxInternalDataConverter()
// ----------------------------------------------------------------------------
//
CImumMboxInternalDataConverter::~CImumMboxInternalDataConverter()
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::~CImumMboxInternalDataConverter, 0, KLogData );
    }

// ----------------------------------------------------------------------------
// CImumMboxInternalDataConverter::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumMboxInternalDataConverter::ConstructL()
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::ConstructL, 0, KLogData );
    }

// ----------------------------------------------------------------------------
// CImumMboxInternalDataConverter::NewL()
// ----------------------------------------------------------------------------
//
CImumMboxInternalDataConverter* CImumMboxInternalDataConverter::NewL(
    CImumInternalApiImpl& aMailboxApi,
    const CImumMboxData& aSourceData,
    CImumInSettingsData& aTargetData )
    {
    IMUM_STATIC_CONTEXT( CImumMboxInternalDataConverter::NewL, 0, utils, KLogData );

    CImumMboxInternalDataConverter* self = NewLC(
        aMailboxApi, aSourceData, aTargetData );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CImumMboxInternalDataConverter::NewLC()
// ----------------------------------------------------------------------------
//
CImumMboxInternalDataConverter* CImumMboxInternalDataConverter::NewLC(
    CImumInternalApiImpl& aMailboxApi,
    const CImumMboxData& aSourceData,
    CImumInSettingsData& aTargetData )
    {
    IMUM_STATIC_CONTEXT( CImumMboxInternalDataConverter::NewLC, 0, utils, KLogData );

    CImumMboxInternalDataConverter* self =
        new ( ELeave ) CImumMboxInternalDataConverter(
        aMailboxApi, aSourceData, aTargetData );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::ConvertToInternalMboxDataLC()
// ---------------------------------------------------------------------------
//
CImumInSettingsData* CImumMboxInternalDataConverter::ConvertToInternalMboxDataLC(
    CImumInternalApiImpl& aMailboxApi,
    const CImumMboxData& aSourceData )
    {
    IMUM_STATIC_CONTEXT( CImumMboxInternalDataConverter::ConvertToInternalMboxDataLC, 0, utils, KLogData );

    TUid protocol = { 0 };

    // Imap4 settings?
    if ( aSourceData.iIsImap4 )
        {
        protocol = KSenduiMtmImap4Uid;
        }
    // Pop3 settings!
    else
        {
        protocol = KSenduiMtmPop3Uid;
        }

    CImumInSettingsData* targetData = CreateInternalDataLC( aMailboxApi, protocol );

    ConvertToInternalMboxDataL( aMailboxApi, aSourceData, *targetData );

    return targetData;
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::ConvertToInternalMboxDataL()
// ---------------------------------------------------------------------------
//
void CImumMboxInternalDataConverter::ConvertToInternalMboxDataL(
    CImumInternalApiImpl& aMailboxApi,
    const CImumMboxData& aSourceData,
    CImumInSettingsData& aTargetData )
    {
    IMUM_STATIC_CONTEXT( CImumMboxInternalDataConverter::ConvertToInternalMboxDataL, 0, utils, KLogData );

    CImumMboxInternalDataConverter* converter = NewLC(
        aMailboxApi, aSourceData, aTargetData );

    converter->DoConvertToInternalMboxDataL();

    CleanupStack::PopAndDestroy( converter );
    converter = NULL;
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::DoConvertToInternalMboxDataL()
// ---------------------------------------------------------------------------
//
void CImumMboxInternalDataConverter::DoConvertToInternalMboxDataL()
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::DoConvertToInternalMboxDataL, 0, KLogData );

    SetIdsToInternalDataL();

    // Imap4 settings?
    if ( iSourceData.iIsImap4 )
        {
        ConvToInternalImap4DataL();
        }
    // Pop3 settings!
    else
        {
        ConvToInternalPop3DataL();
        }
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::CreateInternalDataLC()
// ---------------------------------------------------------------------------
//
CImumInSettingsData* CImumMboxInternalDataConverter::CreateInternalDataLC(
    CImumInternalApiImpl& aMailboxApi,
    const TUid& aProtocol )
    {
    IMUM_STATIC_CONTEXT( CImumMboxInternalDataConverter::CreateInternalDataLC, 0, utils, KLogData );

    CImumInSettingsData* data =
        aMailboxApi.MailboxServicesL().CreateSettingsDataL( aProtocol );
    CleanupStack::PushL( data );
    return data;
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::ConvToInternalImap4DataL()
// ---------------------------------------------------------------------------
//
void CImumMboxInternalDataConverter::ConvToInternalImap4DataL()
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::ConvToInternalImap4DataL, 0, KLogData );

    DoPopulateInternalImap4SettingsL();
    DoPopulateInternalSmtpSettingsL();
    DoPopulateInternalGeneralSettingsL();
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::ConvToInternalPop3DataL()
// ---------------------------------------------------------------------------
//
void CImumMboxInternalDataConverter::ConvToInternalPop3DataL()
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::ConvToInternalPop3DataL, 0, KLogData );

    DoPopulateInternalPop3SettingsL();
    DoPopulateInternalSmtpSettingsL();
    DoPopulateInternalGeneralSettingsL();
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::DoPopulateInternalImap4SettingsL()
// ---------------------------------------------------------------------------
//
void CImumMboxInternalDataConverter::DoPopulateInternalImap4SettingsL()
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::DoPopulateInternalImap4SettingsL, 0, KLogData );

    // CONNECTION SETTINGS
    MImumInSettingsDataCollection& connSett = iTargetData.GetInSetL(0);
    const CImImap4Settings& imap4 = *iSourceData.iImap4Settings;

    //
    FillTextInternalL<KImumDaServerSettingLength>(
        connSett, TImumDaSettings::EKeyServer,
        imap4.ServerAddress() );

    //
    FillText8InternalL<KImumDaUserNameLength>(
        connSett, TImumDaSettings::EKeyUsername,
        imap4.LoginName() );

    //
    FillText8InternalL<KImumDaPasswordLength>(
        connSett, TImumDaSettings::EKeyPassword,
        imap4.Password() );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumDaSettings::EKeySecurity,
        MsvEmailMtmUiUtils::RestoreSecuritySettings( imap4 ) ) );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumDaSettings::EKeyPort,
        static_cast<TInt>( imap4.Port() ) ) );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumInSettings::EKeySyncFlags,
        GetInternalImapDownload( imap4.GetMailOptions() ) ) );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumInSettings::EKeyDownloadFlags,
        GetInternalImapPartial( imap4.PartialMailOptions() ) ) );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumInSettings::EKeyDownloadBodySize,
        imap4.BodyTextSizeLimit() ) );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumInSettings::EKeyDownloadAttachmentSize,
        imap4.AttachmentSizeLimit() ) );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumInSettings::EKeyDownloadMaxEmailSize,
        static_cast<TInt>( imap4.MaxEmailSize() ) ) );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumInSettings::EKeyImapIdle,
        imap4.ImapIdle() ) );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumInSettings::EKeyImapIdleTimeout,
        imap4.ImapIdleTimeout() ) );

    //
    FillText8InternalL<KImumDaFolderPathLength>(
        connSett, TImumDaSettings::EImapFolderPath,
        imap4.FolderPath() );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumInSettings::EKeySyncRate,
        imap4.SyncRate() ) );

    // MAILBOX SETTINGS

    //
    TBuf8<KImumInSeparatorLength> buffer;
    buffer.Append(imap4.PathSeparator());
    FillText8InternalL<KImumInSeparatorLength>(
        iTargetData, TImumInSettings::EKeyPathSeparator,
        buffer );

    //
    FillText8InternalL<KImumInSearchStringLength>(
        iTargetData, TImumInSettings::EKeySearchString,
        imap4.SearchString() );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyDisconnectedMode,
        imap4.DisconnectedUserMode() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeySynchronise,
        imap4.Synchronise() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeySubscribe,
        imap4.Subscribe() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyAutoSendOnConnect,
        imap4.AutoSendOnConnect() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyDeleteEmailsWhenDisconnecting,
        imap4.DeleteEmailsWhenDisconnecting() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyAcknowledgeReceipts,
        imap4.AcknowledgeReceipts() ) );

    //
    TInt32 val = imap4.InboxSynchronisationLimit();
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeyInboxUpdateLimit,
        ( val > 0 ) ? val : KErrNotFound ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeyFolderUpdateLimit,
        imap4.MailboxSynchronisationLimit() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyUpdatingSeenFlag,
        imap4.UpdatingSeenFlags() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyFetchSize,
        static_cast<TInt>( imap4.FetchSize() ) ) );
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::DoPopulateInternalPop3SettingsL()
// ---------------------------------------------------------------------------
//
void CImumMboxInternalDataConverter::DoPopulateInternalPop3SettingsL()
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::DoPopulateInternalPop3SettingsL, 0, KLogData );

    // CONNECTION SETTINGS
    MImumInSettingsDataCollection& connSett = iTargetData.GetInSetL( 0 );
    const CImPop3Settings& pop3 = *iSourceData.iPop3Settings;

    //
    FillTextInternalL<KImumDaServerSettingLength>(
        connSett,
        TImumDaSettings::EKeyServer,
        pop3.ServerAddress() );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumDaSettings::EKeyPort,
        static_cast<TInt>( pop3.Port() ) ) );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumDaSettings::EKeySecurity,
        MsvEmailMtmUiUtils::RestoreSecuritySettings( pop3 ) ) );

    //
    FillText8InternalL<KImumDaUserNameLength>(
        connSett, TImumDaSettings::EKeyUsername,
        pop3.LoginName() );

    //
    FillText8InternalL<KImumDaPasswordLength>(
        connSett, TImumDaSettings::EKeyPassword,
        pop3.Password() );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumDaSettings::EKeyAPop, pop3.Apop() ) );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumInSettings::EKeySyncFlags,
        GetInternalPopSyncL(
            pop3.GetMailOptions() ) ) );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumInSettings::EKeySyncFlags,
        GetInternalPopDownloadL(
            pop3.PopulationLimit(), connSett ) ) );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumInSettings::EKeyDownloadMaxEmailSize,
        pop3.MaxEmailSize() ) );

    // MAILBOX SETTINGS

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyAutoSendOnConnect,
        pop3.AutoSendOnConnect() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyDisconnectedMode,
        pop3.DisconnectedUserMode() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyDeleteEmailsWhenDisconnecting,
        pop3.DeleteEmailsWhenDisconnecting() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyAcknowledgeReceipts,
        pop3.AcknowledgeReceipts() ) );

    //
    TInt32 val = pop3.InboxSynchronisationLimit();
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeyInboxUpdateLimit,
        ( val > 0 ) ? val : KErrNotFound ) );
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::DoPopulateInternalSmtpSettingsL()
// ---------------------------------------------------------------------------
//
void CImumMboxInternalDataConverter::DoPopulateInternalSmtpSettingsL()
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::DoPopulateInternalSmtpSettingsL, 0, KLogData );

    // CONNECTION SETTINGS
    MImumInSettingsDataCollection& connSett = iTargetData.GetInSetL( 1 );
    const CImSmtpSettings& smtp = *iSourceData.iSmtpSettings;

    //
    FillTextInternalL<KImumDaServerSettingLength>(
        connSett, TImumDaSettings::EKeyServer,
        smtp.ServerAddress() );

    //
    FillText8InternalL<KImumDaUserNameLength>(
        connSett, TImumDaSettings::EKeyUsername,
        smtp.LoginName() );

    //
    FillText8InternalL<KImumDaPasswordLength>(
        connSett, TImumDaSettings::EKeyPassword,
        smtp.Password() );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumDaSettings::EKeyPort,
        static_cast<TInt>( smtp.Port() ) ) );

    //
    User::LeaveIfError( connSett.SetAttr(
        TImumDaSettings::EKeySecurity,
        MsvEmailMtmUiUtils::RestoreSecuritySettings( smtp ) ) );


    // MAILBOX SETTINGS

    //
    FillTextInternalL<KImumDaEmailSettingLength>(
        iTargetData, TImumDaSettings::EKeyEmailAddress,
        smtp.EmailAddress() );

    //
    FillTextInternalL<KImumDaUserAliasLength>(
        iTargetData, TImumDaSettings::EKeyUserAlias,
        smtp.EmailAlias() );

    //
    FillTextInternalL<KImumDaEmailSettingLength>(
        iTargetData, TImumDaSettings::EKeyReplyToAddress,
        smtp.ReplyToAddress() );

    //
    FillTextInternalL<KImumDaEmailSettingLength>(
        iTargetData, TImumInSettings::EKeyReceiptAddress,
        smtp.ReceiptAddress() );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyDefaultMsgCharSet,
        smtp.DefaultMsgCharSet().iUid ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyBodyEncoding,
        smtp.BodyEncoding() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyAddVCardToEmail,
        smtp.AddVCardToEmail() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeyIncludeSignature,
        smtp.AddSignatureToEmail() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyRequestReceipts,
        smtp.RequestReceipts() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeySendCopyToSelf,
        smtp.SendCopyToSelf() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeySendDelay,
        smtp.SendMessageOption() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeySMTPAuth,
        smtp.SMTPAuth() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyInboxLoginDetails,
        smtp.InboxLoginDetails() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyToCcIncludeLimit,
        smtp.ToCcIncludeLimit() ) );
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::DoPopulateInternalGeneralSettingsL()
// ---------------------------------------------------------------------------
//
void CImumMboxInternalDataConverter::DoPopulateInternalGeneralSettingsL()
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::DoPopulateInternalGeneralSettingsL, 0, KLogData );

    const CImumMboxDataExtension& ext = *iSourceData.iExtendedSettings;

    // Set access point data here
    SetInternalAccessPointL( 0 );
    SetInternalAccessPointL( 1 );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeyMailboxName,
        iSourceData.iName ) );

    TImumDaSettings::TTextSignature signature;
    iSourceData.iSignature->iRichText->Extract(
        signature, 0, KImumDaSignatureLength );
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeySignature,
        signature ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeyMailDeletion,
        ext.MailDeletion() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeyAutoNotifications,
        InternalEmnState( ext.EmailNotificationState() ) ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeyEmailAlert,
        ext.NewMailIndicators() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyHideMsgs,
        ext.HideMsgs() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyOpenHtmlMail,
        ext.OpenHtmlMail() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeySettingsVersion,
        ext.Version() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeyAutoRetrieval,
        InternalAoState( ext.AlwaysOnlineState() ) ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeyAutoRetrievalDays,
        static_cast<TInt>( ext.SelectedWeekDays() ) ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeyAutoRetrievalStartHour,
        ext.SelectedTimeStart().Int64() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeyAutoRetrievalEndHour,
        ext.SelectedTimeStop().Int64() ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumDaSettings::EKeyAutoRetrievalInterval,
        InternalRetrivalInterval( ext.InboxRefreshTime() ) ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyInfoLastUpdateFailed,
        ext.LastUpdateInfo().iLastUpdateFailed ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyInfoLastSuccessfulUpdate,
        ext.LastUpdateInfo().iLastSuccessfulUpdate.Int64() ) );

    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyAoUpdateSuccessfulWithCurSettings,
        ext.LastUpdateInfo().iUpdateSuccessfulWithCurSettings ) );

    //
    User::LeaveIfError( iTargetData.SetAttr(
        TImumInSettings::EKeyIsPredefinedMailbox,
        ext.IsPredefined() ) );

    // Temporary password status
    const TMuiuFlags& statusFlags =  ext.StatusFlags();
    // Receive temp password flag
    User::LeaveIfError( iTargetData[0].SetAttr(
        TImumInSettings::EKeyTemporaryPassword,
        statusFlags.Flag( EIMASStatusTempRcvPassword ) ) );
    // Transmit temp password flag
    User::LeaveIfError( iTargetData[1].SetAttr(
        TImumInSettings::EKeyTemporaryPassword,
        statusFlags.Flag( EIMASStatusTempSndPassword ) ) );
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::SetIdsToInternalDataL()
// ---------------------------------------------------------------------------
//
void CImumMboxInternalDataConverter::SetIdsToInternalDataL()
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::SetIdsToInternalDataL, 0, KLogData );

	// Get smtp id
    TMsvId id = iSourceData.iExtendedSettings->MailboxId();

    // Set smtp id
    iTargetData[1].SetAttr( TImumDaSettings::EKeyMailboxId, id );

    // Set Receiving id
    TMsvEntry entry = iMailboxApi.MailboxUtilitiesL().GetMailboxEntryL( id );
    iTargetData[0].SetAttr( TImumDaSettings::EKeyMailboxId, entry.iRelatedId );
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::GetInternalImapPartial()
// ---------------------------------------------------------------------------
//
TInt32 CImumMboxInternalDataConverter::GetInternalImapPartial(
    const TImImap4PartialMailOptions& /* aPartialOptions */ )
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::GetInternalImapPartial, 0, KLogData );

    return TImumInSettings::EFlagSetDownloadAll;
    }


// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::GetInternalImapDownload()
// ---------------------------------------------------------------------------
//
TInt32 CImumMboxInternalDataConverter::GetInternalImapDownload(
    const TImap4GetMailOptions& /* aPartialOptions */ )
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::GetInternalImapDownload, 0, KLogData );

    return TImumInSettings::EFlagDownloadHeader;
    }


// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::SetInternalAccessPointL()
// ---------------------------------------------------------------------------
//
void CImumMboxInternalDataConverter::SetInternalAccessPointL(
    const TInt aIndex )
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::SetInternalAccessPointL, 0, KLogData );

    // Initialize access point type to Iap Id
    iTargetData[aIndex].SetAttr( TImumInSettings::EKeyAccessPointType,
    							 TImumInSettings::EValueApIsIapId );

    // aIndex refers to incoming or outgoing settings
    TInt ap = KErrNotFound;
    if ( !aIndex )
        {
        ap = iSourceData.iIncomingIap;
        }
    else
        {
        ap = iSourceData.iOutgoingIap;
        }

    // And access point to always ask, which means that default connection
    // is used
    iTargetData[aIndex].SetAttr( TImumDaSettings::EKeyAccessPoint, ap );
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::InternalAoState()
// ---------------------------------------------------------------------------
//
TInt CImumMboxInternalDataConverter::InternalAoState(
    const TMailAoStates& aAoState )
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::InternalAoState, 0, KLogData );

    switch ( aAoState )
        {
        case EMailAoAlways:
            return TImumDaSettings::EValueAutoOn;
        case EMailAoHomeOnly:
            return TImumDaSettings::EValueAutoHomeNetwork;
        case EMailAoOff:
            return TImumDaSettings::EValueAutoOff;
        }

    return TImumDaSettings::EValueAutoOff;
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::InternalEmnState()
// ---------------------------------------------------------------------------
//
TInt CImumMboxInternalDataConverter::InternalEmnState(
    const TMailEmnStates& aEmnState )
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::InternalEmnState, 0, KLogData );

    switch ( aEmnState )
        {
        case EMailEmnAutomatic:
            return TImumDaSettings::EValueNotificationsOn;
        case EMailEmnHomeOnly:
            return TImumDaSettings::EValueNotificationsHome;
        case EMailEmnOff:
            return TImumDaSettings::EValueNotificationsOff;
        }

    return TImumDaSettings::EValueNotificationsOff;
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::InternalRetrivalInterval()
// ---------------------------------------------------------------------------
//
TInt CImumMboxInternalDataConverter::InternalRetrivalInterval(
    TInt aInterval )
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::InternalRetrivalInterval, 0, KLogData );

    switch ( aInterval )
        {
        case KIMASAoInterval5Min:
            return TImumDaSettings::EValue5Minutes;
        case KIMASAoInterval15Min:
            return TImumDaSettings::EValue15Minutes;
        case KIMASAoInterval30Min:
            return TImumDaSettings::EValue30Minutes;
        case KIMASAoInterval1Hour:
            return TImumDaSettings::EValue1Hour;
        case KIMASAoInterval2Hours:
            return TImumDaSettings::EValue2Hours;
        case KIMASAoInterval4Hours:
            return TImumDaSettings::EValue4Hours;
        case KIMASAoInterval6Hours:
            return TImumDaSettings::EValue6Hours;
        }

    return TImumDaSettings::EValue1Hour;
    }


// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::GetInternalPopDownloadL()
// ---------------------------------------------------------------------------
//
TInt32 CImumMboxInternalDataConverter::GetInternalPopDownloadL(
    const TInt32& aSetting,
    MImumInSettingsDataCollection& aInternalDataCollection )
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::GetInternalPopDownloadL, 0, KLogData );

    TInt partial = 0;
    if ( aSetting == EIMASMapFullBody )
        {
        partial = TImumInSettings::EFlagSetDownloadAll;
        }
    else if ( aSetting == EIMASMapHeadersOnly )
        {
        partial = TImumInSettings::EFlagDownloadHeader;
        }
    else
        {
        partial = TImumInSettings::EFlagDownloadPartialCumulative;
        aInternalDataCollection.SetAttr(
            TImumInSettings::EKeyDownloadBodySize, aSetting );
        }

    return partial;
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::GetInternalPopSyncL()
// ---------------------------------------------------------------------------
//
TInt32 CImumMboxInternalDataConverter::GetInternalPopSyncL(
    const TInt32& aSetting )
    {
    IMUM_CONTEXT( CImumMboxInternalDataConverter::GetInternalPopSyncL, 0, KLogData );

    return ( aSetting == EGetPop3EmailMessages ) ?
        TImumInSettings::EFlagSetDownloadAll :
        TImumInSettings::EFlagDownloadHeader;
    }

// End of File
