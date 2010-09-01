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
* Description:  ImumMboxSymbianDataConverter.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <SendUiConsts.h>
#include <miutset.h>
#include <smtpset.h>
#include <pop3set.h>
#include <imapset.h>

#include "ImumMboxSymbianDataConverter.h"
#include "ImumDaSettingsKeys.h"
#include "ImumInSettingsKeys.h"
#include "ImumDaErrorCodes.h"
#include "ImumPanic.h"
#include "ImumMboxSymbianDataConverter.h"
#include "ImumMboxInternalDataConverter.h"
#include "ImumMboxSymbianDataConverter.h"
#include "ImumMboxData.h"
#include "ImumInSettingsDataImpl.h"
#include "ImumInSettingsDataCollectionImpl.h"
#include "ImumInternalApiImpl.h"
#include "ComDbUtl.h"
#include "IMASAccountControl.h"
#include "EmailUtils.H"
#include "ImumUtilsLogging.h"
#include "EmailFeatureUtils.h"

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
// CImumMboxSymbianDataConverter::CImumMboxSymbianDataConverter()
// ----------------------------------------------------------------------------
//
CImumMboxSymbianDataConverter::CImumMboxSymbianDataConverter(
    CImumInternalApiImpl& aMailboxApi,
    const CImumInSettingsData& aSourceData,
    CImumMboxData& aTargetData )
    :
    iMailboxApi( aMailboxApi ),
    iTargetData( aTargetData ),
    iSourceData( aSourceData ),
    iMailboxProtocol( TUid::Uid( 0 ) ),
    iCreationPhase( ETrue )
    {
    IMUM_CONTEXT( CImumMboxSymbianDataConverter::CImumMboxSymbianDataConverter, 0, KLogData );
    IMUM_IN();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::~CImumMboxSymbianDataConverter()
// ----------------------------------------------------------------------------
//
CImumMboxSymbianDataConverter::~CImumMboxSymbianDataConverter()
    {
    IMUM_CONTEXT( CImumMboxSymbianDataConverter::~CImumMboxSymbianDataConverter, 0, KLogData );
    IMUM_IN();

    iMailboxEntries.Close();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumMboxSymbianDataConverter::ConstructL()
    {
    IMUM_CONTEXT( CImumMboxSymbianDataConverter::ConstructL, 0, KLogData );
    IMUM_IN();

    TMsvId mailboxId = 0;
    TInt error =
        iSourceData[0].GetAttr( TImumDaSettings::EKeyMailboxId, mailboxId );

    // Get the mailbox id from the settings. In case of wizard, the id does
    // not exist, so this can be skipped.
    if ( !error && mailboxId )
        {
        iMailboxProtocol = iMailboxApi.MailboxUtilitiesL().GetMailboxEntriesL(
            mailboxId, iMailboxEntries );
        iCreationPhase = EFalse;
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::NewL()
// ----------------------------------------------------------------------------
//
CImumMboxSymbianDataConverter* CImumMboxSymbianDataConverter::NewL(
    CImumInternalApiImpl& aMailboxApi,
    const CImumInSettingsData& aSourceData,
    CImumMboxData& aTargetData )
    {
    IMUM_STATIC_CONTEXT( CImumMboxSymbianDataConverter::NewL, 0, utils, KLogData );
    IMUM_IN();

    CImumMboxSymbianDataConverter* self = NewLC(
        aMailboxApi, aSourceData, aTargetData );
    CleanupStack::Pop( self );

    IMUM_OUT();
    return self;
    }

// ----------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::NewLC()
// ----------------------------------------------------------------------------
//
CImumMboxSymbianDataConverter* CImumMboxSymbianDataConverter::NewLC(
    CImumInternalApiImpl& aMailboxApi,
    const CImumInSettingsData& aSourceData,
    CImumMboxData& aTargetData )
    {
    IMUM_STATIC_CONTEXT( CImumMboxSymbianDataConverter::NewLC, 0, utils, KLogData );

    CImumMboxSymbianDataConverter* self =
        new ( ELeave ) CImumMboxSymbianDataConverter(
            aMailboxApi, aSourceData, aTargetData );
    CleanupStack::PushL( self );
    self->ConstructL();

    IMUM_OUT();
    return self;
    }


// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::ConvertToSymbianMboxDataLC()
// ---------------------------------------------------------------------------
//
CImumMboxData* CImumMboxSymbianDataConverter::ConvertToSymbianMboxDataLC(
    CImumInternalApiImpl& aMailboxApi,
    const CImumInSettingsData& aSourceData )
    {
    IMUM_STATIC_CONTEXT( CImumMboxSymbianDataConverter::ConvertToSymbianMboxDataLC, 0, utils, KLogData );
    IMUM_IN();

    CImumMboxData* targetData = CImumMboxData::NewLC();
    CImumMboxSymbianDataConverter* converter = NewLC(
        aMailboxApi, aSourceData, *targetData );

    TRAPD( error, converter->DoConvertToSymbianMboxDataL() );
    if ( error )
        {
        ConverterExceptionL( error );
        }

    CleanupStack::PopAndDestroy( converter );
    converter = NULL;

    IMUM_OUT();
    return targetData;
    }

// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::DoConvertToSymbianMboxDataL()
// ---------------------------------------------------------------------------
//
void CImumMboxSymbianDataConverter::DoConvertToSymbianMboxDataL()
    {
    IMUM_CONTEXT( CImumMboxSymbianDataConverter::DoConvertToSymbianMboxDataL, 0, KLogData );
    IMUM_IN();

    // Collect the data from the entry
    if ( !iCreationPhase )
        {
        CollectEntryDataL();
        }

    TInt protocol = 0;
    iSourceData[0].GetAttr( TImumDaSettings::EKeyProtocol, protocol );

    // Create settings data objects according to protocol
    switch ( protocol )
        {
        case TImumDaSettings::EValueProtocolImap4:
            ConvToSymbianImap4DataL();
            break;

        case TImumDaSettings::EValueProtocolPop3:
            ConvToSymbianPop3DataL();
            break;

        default:
            ConverterExceptionL(
                TImumDaErrorCode::EParamUnsupportedProtocol );
            break;
        }

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::ConvToSymbianImap4DataL()
// ---------------------------------------------------------------------------
//
void CImumMboxSymbianDataConverter::ConvToSymbianImap4DataL()
    {
    IMUM_CONTEXT( CImumMboxSymbianDataConverter::ConvToSymbianImap4DataL, 0, KLogData );
    IMUM_IN();

    iTargetData.iIsImap4 = ETrue;

    // For wizard, there is no account available
    if ( !iCreationPhase )
        {
        iMailboxApi.EmailAccountsL().GetImapAccountL(
            iMailboxEntries[0].Id(), iTargetData.iImap4AccountId );
        }

    DoPopulateSymbianImap4SettingsL();
    DoPopulateSymbianSmtpSettingsL();
    DoPopulateExtendedSettingsL();

    SetSymbianAccessPointL( 0 );
    SetSymbianAccessPointL( 1 );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::ConvToSymbianPop3DataL()
// ---------------------------------------------------------------------------
//
void CImumMboxSymbianDataConverter::ConvToSymbianPop3DataL()
    {
    IMUM_CONTEXT( CImumMboxSymbianDataConverter::ConvToSymbianPop3DataL, 0, KLogData );
    IMUM_IN();

    // For wizard, there is no account available
    if ( !iCreationPhase )
        {
        iMailboxApi.EmailAccountsL().GetPopAccountL(
            iMailboxEntries[0].Id(), iTargetData.iPop3AccountId );
        }

    DoPopulateSymbianPop3SettingsL();
    DoPopulateSymbianSmtpSettingsL();
    DoPopulateExtendedSettingsL();

    SetSymbianAccessPointL( 0 );
    SetSymbianAccessPointL( 1 );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::DoPopulateSymbianImap4SettingsL()
// ---------------------------------------------------------------------------
//
void CImumMboxSymbianDataConverter::DoPopulateSymbianImap4SettingsL()
    {
    IMUM_CONTEXT( CImumMboxSymbianDataConverter::DoPopulateSymbianImap4SettingsL, 0, KLogData );
    IMUM_IN();

    // CONNECTION SETTINGS
    MImumInSettingsDataCollection& rcvConnSet = iSourceData.GetInSetL(0);
    CImImap4Settings& imap4 = *iTargetData.iImap4Settings;
    TInt value = 0;
    TInt32 value32 = 0;
    TImumDaSettings::TTextServerAddress serverAddr;
    TImumDaSettings::TTextUserName username;
    TImumDaSettings::TTextPassword password;
    TImumDaSettings::TTextMailboxName name;

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumDaSettings::EKeyServer, serverAddr ) );
    imap4.SetServerAddressL( serverAddr.Left( KImumDaMailboxNameLength ) );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumDaSettings::EKeyPort, value ) );
    imap4.SetPort( value );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumDaSettings::EKeySecurity, value ) );
    MsvEmailMtmUiUtils::StoreSecuritySettings(
        imap4, static_cast<TImumDaSettings::TSecurityValues>( value ) );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumDaSettings::EKeyUsername, username ) );
    imap4.SetLoginNameL( username );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumDaSettings::EKeyPassword, password ) );
    imap4.SetPasswordL( password );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumInSettings::EKeyImapIdle, value ) );
    imap4.SetImapIdle( value );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumInSettings::EKeyImapIdleTimeout, value ) );
    imap4.SetImapIdleTimeoutL( value );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumInSettings::EKeyDownloadMaxEmailSize, value ) );
    imap4.SetMaxEmailSize( value );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumInSettings::EKeyDownloadFlags, value32 ) );
    imap4.SetPartialMailOptionsL( GetSymbianImapPartial(
        TMuiuFlags( value32 ) ) );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumInSettings::EKeyDownloadBodySize, value32 ) );
    imap4.SetBodyTextSizeLimitL( value32 );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumInSettings::EKeyDownloadAttachmentSize, value32 ) );
    imap4.SetAttachmentSizeLimitL( value32 );

    //
    TImumDaSettings::TTextFolderPath folderPath;
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumDaSettings::EImapFolderPath, folderPath ) );
    imap4.SetFolderPathL( folderPath );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumInSettings::EKeySyncRate, value ) );
    imap4.SetSyncRateL( value );

    // MAILBOX SETTINGS

    //
    TImumInSettings::TTextSearchString searchString;
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeySearchString, searchString ) );
    imap4.SetSearchStringL( searchString );

    //
    TBuf8<KImumInSeparatorLength> pathSeparator;
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyPathSeparator, pathSeparator ) );
    imap4.SetPathSeparator( *pathSeparator.Ptr() );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyDisconnectedMode, value ) );
    imap4.SetDisconnectedUserMode( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeySynchronise, value ) );
    imap4.SetSynchronise( static_cast<TFolderSyncType>( value ) );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeySubscribe, value ) );
    imap4.SetSubscribe( static_cast<TFolderSubscribeType>( value ) );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyAutoSendOnConnect, value ) );
    imap4.SetAutoSendOnConnect( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyDeleteEmailsWhenDisconnecting, value ) );
    imap4.SetDeleteEmailsWhenDisconnecting( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyAcknowledgeReceipts, value ) );
    imap4.SetAcknowledgeReceipts( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyInboxUpdateLimit, value32 ) );
    imap4.SetInboxSynchronisationLimit( value32 );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyFolderUpdateLimit, value32 ) );
    imap4.SetMailboxSynchronisationLimit( value32 );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyUpdatingSeenFlag, value ) );
    imap4.SetUpdatingSeenFlags( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyFetchSize, value ) );
    imap4.SetFetchSizeL( value );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::DoPopulateSymbianPop3SettingsL()
// ---------------------------------------------------------------------------
//
void CImumMboxSymbianDataConverter::DoPopulateSymbianPop3SettingsL()
    {
    IMUM_CONTEXT( CImumMboxSymbianDataConverter::DoPopulateSymbianPop3SettingsL, 0, KLogData );
    IMUM_IN();

    MImumInSettingsDataCollection& rcvConnSet = iSourceData.GetInSetL(0);
    CImPop3Settings& pop3 = *iTargetData.iPop3Settings;
    TInt value = 0;
    TInt32 value32 = 0;
    TImumDaSettings::TTextServerAddress serverAddr;
    TImumDaSettings::TTextUserName username;
    TImumDaSettings::TTextPassword password;

    // CONNECTION SETTINGS

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumDaSettings::EKeyServer, serverAddr ) );
    pop3.SetServerAddressL( serverAddr );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumDaSettings::EKeyPort, value ) );
    pop3.SetPort( value );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumDaSettings::EKeySecurity, value ) );
    MsvEmailMtmUiUtils::StoreSecuritySettings( pop3,
        static_cast<TImumDaSettings::TSecurityValues>( value ) );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumDaSettings::EKeyUsername, username ) );
    pop3.SetLoginNameL( username );
    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumDaSettings::EKeyPassword, password ) );
    pop3.SetPasswordL( password );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumInSettings::EKeyDownloadMaxEmailSize, value ) );
    pop3.SetMaxEmailSize( value );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumInSettings::EKeySyncFlags, value32 ) );
    pop3.SetGetMailOptions( GetSymbianPopGetMail( value32 ) );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumInSettings::EKeySyncFlags, value32 ) );
    pop3.SetPopulationLimitL(
        GetSymbianPopDownloadL( value32, rcvConnSet ) );

    //
    User::LeaveIfError( rcvConnSet.GetAttr(
        TImumDaSettings::EKeyAPop, value ) );
    pop3.SetApop( value );

    // MAILBOX SETTINGS

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyAutoSendOnConnect, value ) );
    pop3.SetAutoSendOnConnect( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyDisconnectedMode, value ) );
    pop3.SetDisconnectedUserMode( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyDeleteEmailsWhenDisconnecting, value ) );
    pop3.SetDeleteEmailsWhenDisconnecting( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyAcknowledgeReceipts, value ) );
    pop3.SetAcknowledgeReceipts( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyInboxUpdateLimit, value32 ) );
    pop3.SetInboxSynchronisationLimit( value32 );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::DoPopulateSymbianSmtpSettingsL()
// ---------------------------------------------------------------------------
//
void CImumMboxSymbianDataConverter::DoPopulateSymbianSmtpSettingsL()
    {
    IMUM_CONTEXT( CImumMboxSymbianDataConverter::DoPopulateSymbianSmtpSettingsL, 0, KLogData );
    IMUM_IN();

    MImumInSettingsDataCollection& sndConnSet = iSourceData.GetInSetL(1);
    CImSmtpSettings& smtp = *iTargetData.iSmtpSettings;
    TInt value = 0;
    TInt32 value32 = 0;
    TImumDaSettings::TTextServerAddress serverAddr;
    TImumDaSettings::TTextUserName username;
    TImumDaSettings::TTextPassword password;

    // CONNECTION SETTINGS

    //
    User::LeaveIfError( sndConnSet.GetAttr(
        TImumDaSettings::EKeyServer, serverAddr ) );
    smtp.SetServerAddressL( serverAddr );

    //
    User::LeaveIfError( sndConnSet.GetAttr(
        TImumDaSettings::EKeyPort, value ) );
    smtp.SetPort( value );

    //
    User::LeaveIfError( sndConnSet.GetAttr(
        TImumDaSettings::EKeySecurity, value ) );
    MsvEmailMtmUiUtils::StoreSecuritySettings( smtp,
        static_cast<TImumDaSettings::TSecurityValues>( value ) );

    //
    User::LeaveIfError( sndConnSet.GetAttr(
        TImumDaSettings::EKeyUsername, username ) );
    smtp.SetLoginNameL( username );

    //
    User::LeaveIfError( sndConnSet.GetAttr(
        TImumDaSettings::EKeyPassword, password ) );
    smtp.SetPasswordL( password );


    // MAILBOX SETTINGS

    //
    TImumDaSettings::TTextEmailAddress emailAddress;
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyEmailAddress, emailAddress ) );
    smtp.SetEmailAddressL( emailAddress );

    //
    TImumDaSettings::TTextUserAlias userAlias;
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyUserAlias, userAlias ) );
    smtp.SetEmailAliasL( userAlias );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyReplyToAddress, emailAddress ) );
    smtp.SetReplyToAddressL( emailAddress );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyReceiptAddress, emailAddress ) );
    smtp.SetReceiptAddressL( emailAddress );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyBodyEncoding, value ) );
    smtp.SetBodyEncoding( static_cast<TMsgOutboxBodyEncoding>( value ) );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyDefaultMsgCharSet, value32 ) );
    smtp.SetDefaultMsgCharSet( TUid::Uid( value32 ) );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyAddVCardToEmail, value ) );
    smtp.SetAddVCardToEmail( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyIncludeSignature, value ) );
    smtp.SetAddSignatureToEmail( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyRequestReceipts, value ) );
    smtp.SetRequestReceipts( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeySendCopyToSelf, value ) );
    smtp.SetSendCopyToSelf( static_cast<TImSMTPSendCopyToSelf>( value ) );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeySendDelay, value ) );
    smtp.SetSendMessageOption(
        static_cast<TImSMTPSendMessageOption>( value ) );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeySMTPAuth, value ) );
    smtp.SetSMTPAuth( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyInboxLoginDetails, value ) );
    smtp.SetInboxLoginDetails( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyToCcIncludeLimit, value ) );
    smtp.SetToCcIncludeLimitL( value );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::DoPopulateExtendedSettingsL()
// ---------------------------------------------------------------------------
//
void CImumMboxSymbianDataConverter::DoPopulateExtendedSettingsL()
    {
    IMUM_CONTEXT( CImumMboxSymbianDataConverter::DoPopulateExtendedSettingsL, 0, KLogData );
    IMUM_IN();

    CImumMboxDataExtension& ext = *iTargetData.iExtendedSettings;

    TInt value = 0;
    TInt64 value64 = 0;
    TImumDaSettings::TTextSignature signature;

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyMailboxName, iTargetData.iName ) );

    //
    TImumDaSettings::TTextEmailAddress emailAddress;
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyEmailAddress, emailAddress ) );
    ext.SetEmailAddress( emailAddress );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyEmailAlert, value ) );
    ext.SetNewMailIndicators( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyIsPredefinedMailbox, value ) );
    ext.SetIsPredefined( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeySignature, signature ) );
    iTargetData.iSignature->iRichText->Reset();
    iTargetData.iSignature->iRichText->InsertL( 0, signature );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyAutoNotifications, value ) );
    ext.SetEmailNotificationState( SymbianEmnState( value ) );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyMailDeletion, value ) );
    ext.SetMailDeletion( static_cast<TIMASMailDeletionMode>( value ) );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyHideMsgs, value ) );
    ext.SetHideMsgs( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyOpenHtmlMail, value ) );
    ext.SetOpenHtmlMail( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyAutoRetrieval, value ) );
    ext.SetAlwaysOnlineState( SymbianAoState( value ) );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyAutoRetrievalDays, value ) );
    ext.SetSelectedWeekDays( value );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyAutoRetrievalStartHour, value64 ) );
    ext.SetSelectedTimeStart( value64 );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyAutoRetrievalEndHour, value64 ) );
    ext.SetSelectedTimeStop( value64 );

    //
    User::LeaveIfError( iSourceData.GetAttr(
        TImumDaSettings::EKeyAutoRetrievalInterval, value ) );
    ext.SetInboxRefreshTime( SymbianAoRetrievalInterval( value ) );

    //
    TAOInfo aoInfo;
    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyInfoLastUpdateFailed, value ) );
    aoInfo.iLastUpdateFailed = value;

    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyInfoLastSuccessfulUpdate, value64 ) );
    aoInfo.iLastSuccessfulUpdate = value64;

    User::LeaveIfError( iSourceData.GetAttr(
        TImumInSettings::EKeyAoUpdateSuccessfulWithCurSettings, value ) );
    aoInfo.iUpdateSuccessfulWithCurSettings = value;

    ext.SetLastUpdateInfo( aoInfo );

    // Temporary password status
    TMuiuFlags statusFlags =  ext.StatusFlags();
    // Receive temp password flag
    User::LeaveIfError( iSourceData[0].GetAttr(
        TImumInSettings::EKeyTemporaryPassword, value ) );
    statusFlags.ChangeFlag( EIMASStatusTempRcvPassword , value );
    // Transmit temp password flag
    User::LeaveIfError( iSourceData[1].GetAttr(
        TImumInSettings::EKeyTemporaryPassword, value ) );
    statusFlags.ChangeFlag( EIMASStatusTempSndPassword, value );
    ext.SetStatusFlags( statusFlags );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::SetSymbianAccessPointL()
// ---------------------------------------------------------------------------
//
void CImumMboxSymbianDataConverter::SetSymbianAccessPointL(
    const TInt aIndex )
    {
    IMUM_CONTEXT( CImumMboxSymbianDataConverter::SetSymbianAccessPointL, 0, KLogData );
    IMUM_IN();

    MImumInSettingsDataCollection& source = iSourceData.GetInSetL( aIndex );

    TInt ap = 0;
    source.GetAttr( TImumDaSettings::EKeyAccessPoint, ap );

    // set ap id to 0 if always ask is set, meaning default connection
    // is used
    if( ap == TImumDaSettings::EValueApAlwaysAsk )
        {
        ap = 0;
        }

    if ( !aIndex )
        {
        iTargetData.iIncomingIap = ap;
        }
    else
        {
        iTargetData.iOutgoingIap = ap;
        }

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// ImumInSettingsDataConverter::CollectEntryDataL()
// ---------------------------------------------------------------------------
//
void CImumMboxSymbianDataConverter::CollectEntryDataL()
    {
    IMUM_CONTEXT( CImumMboxSymbianDataConverter::CollectEntryDataL, 0, KLogData );
    IMUM_IN();

    // Protocol
    iTargetData.iIsImap4 = ( iMailboxProtocol == KSenduiMtmImap4Uid );
    iTargetData.iExtendedSettings->SetProtocol( iMailboxProtocol );

    // Account Id
    const TMsvEntry& rcv = iMailboxEntries[0];
    iTargetData.iExtendedSettings->SetAccountId( rcv.iMtmData2 );

    // Extension Id
    TUint32 extId = IMASAccountControl::AccountIdToExtendedAccountId(
        rcv.iMtmData2,
        rcv.iMtm );
    iTargetData.iExtendedSettings->SetExtensionId( extId );

    // Mailbox Id
    TMsvEntry smtp = iMailboxEntries[1];
    iTargetData.iMailboxId = smtp.Id();
    iTargetData.iExtendedSettings->SetMailboxId( smtp.Id() );


    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::GetSymbianImapPartial()
// ---------------------------------------------------------------------------
//
TImImap4PartialMailOptions CImumMboxSymbianDataConverter::GetSymbianImapPartial(
    const TMuiuFlags& /* aFlags */ )
    {
    IMUM_CONTEXT( CImumMboxSymbianDataConverter::GetSymbianImapPartial, 0, KLogData );
    IMUM_IN();

    // Currently, no sizelimits supported in imap4
    IMUM_OUT();

    return ENoSizeLimits;
    }

// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::SymbianAoState()
// ---------------------------------------------------------------------------
//
TMailAoStates CImumMboxSymbianDataConverter::SymbianAoState(
    const TInt aAoState )
    {
    switch ( aAoState )
        {
        case TImumDaSettings::EValueAutoOff:
            return EMailAoOff;
        case TImumDaSettings::EValueAutoOn:
            return EMailAoAlways;
        case TImumDaSettings::EValueAutoHomeNetwork:
            return EMailAoHomeOnly;
        }

    return EMailAoOff;
    }

// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::SymbianAoRetrievalInterval()
// ---------------------------------------------------------------------------
//
TInt CImumMboxSymbianDataConverter::SymbianAoRetrievalInterval(
    TInt aInterval )
    {
    switch ( aInterval )
        {
        case TImumDaSettings::EValue5Minutes:
            return KIMASAoInterval5Min;
        case TImumDaSettings::EValue15Minutes:
            return KIMASAoInterval15Min;
        case TImumDaSettings::EValue30Minutes:
            return KIMASAoInterval30Min;
        case TImumDaSettings::EValue1Hour:
            return KIMASAoInterval1Hour;
        case TImumDaSettings::EValue2Hours:
            return KIMASAoInterval2Hours;
        case TImumDaSettings::EValue4Hours:
            return KIMASAoInterval4Hours;
        case TImumDaSettings::EValue6Hours:
            return KIMASAoInterval6Hours;
        }

    // default
    return KIMASAoInterval1Hour;
    }

// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::SymbianEmnState()
// ---------------------------------------------------------------------------
//
TMailEmnStates CImumMboxSymbianDataConverter::SymbianEmnState(
    const TInt aEmnState )
    {
    switch ( aEmnState )
        {
        case TImumDaSettings::EValueNotificationsOff:
            return EMailEmnOff;
        case TImumDaSettings::EValueNotificationsOn:
            return EMailEmnAutomatic;
        case TImumDaSettings::EValueNotificationsHome:
            return EMailEmnHomeOnly;
        }

    return EMailEmnOff;
    }

// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::GetSymbianPopGetMail()
// ---------------------------------------------------------------------------
//
TPop3GetMailOptions CImumMboxSymbianDataConverter::GetSymbianPopGetMail(
    const TUint aFlags )
    {
    // Get body and headers as default
    return ( aFlags == TImumInSettings::EFlagSetDownloadAll ) ?
        EGetPop3EmailMessages : EGetPop3EmailHeaders ;
    }

// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::GetSymbianPopDownloadL()
// ---------------------------------------------------------------------------
//
TInt32 CImumMboxSymbianDataConverter::GetSymbianPopDownloadL(
    const TUint aFlags,
    const MImumInSettingsDataCollection& aInternalDataCollection )
    {
    IMUM_CONTEXT( CImumMboxSymbianDataConverter::GetSymbianPopDownloadL, 0, KLogData );
    IMUM_IN();

    TInt32 partial = 0;

    // Check if Header & Body & Attachment is set
    if ( aFlags == TImumInSettings::EFlagSetDownloadAll )
        {
        partial = EIMASMapFullBody;
        }
    // Check if only headers is set
    else if ( aFlags == TImumInSettings::EFlagDownloadHeader |
        aFlags == TImumInSettings::EFlagDownloadBody )
        {
        partial = EIMASMapHeadersOnly;
        }
    // Set up the cumulative partial download
    else if ( aFlags == TImumInSettings::EFlagDownloadPartialCumulative )
        {
        // In cumulative retrieval, only bodycount matters
        User::LeaveIfError( aInternalDataCollection.GetAttr(
            TImumInSettings::EKeyDownloadBodySize, partial ) );
        }
    else
        {
        // Unsupported set... Force default set
        partial = EIMASMapHeadersOnly;
        }

    IMUM_OUT();

    return partial;
    }

// ---------------------------------------------------------------------------
// CImumMboxSymbianDataConverter::ConverterExceptionL()
// ---------------------------------------------------------------------------
//
void CImumMboxSymbianDataConverter::ConverterExceptionL(
    const TInt aErrorCode )
    {
    IMUM_STATIC_CONTEXT( CImumMboxSymbianDataConverter::ConverterExceptionL, 0, utils, KLogData );
    IMUM_IN();

    __ASSERT_DEBUG( EFalse,
        User::Panic( KImumMboxSymbianDataConverter, aErrorCode ) );
    User::Leave( aErrorCode );
    }


// End of File
