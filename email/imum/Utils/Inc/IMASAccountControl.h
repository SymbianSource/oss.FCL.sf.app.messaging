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
* Description:  This class handles the complex account id shifting operations.
*
*/


#ifndef IMASACCOUNTCONTROL_H
#define IMASACCOUNTCONTROL_H

// INCLUDES
#include <e32base.h>
#include <msvstd.h>                     // TMsvId
#include <cemailaccounts.h>             // CEmailAccounts

// CONSTANTS

// NOTE: Do not change the order of the enumerations, as it will 
//       break the data compatibility. Add new keys just before
//       EImaKeyLast -key.
enum TImasCommonSettings
    {
    // 3.0
    EImaCommonSetStatusFlagsH = 0x00,
    EImaCommonSetStatusFlagsL,
    EImaCommonSetMailboxId,
    EImaCommonSetAccountId,
    EImaCommonSetExtensionId,
    EImaCommonSetProtocol,
    EImaCommonSetEmailAddress,
    EImaExtSetEmailNotif,
    EImaExtSetIndicators,
    EImaExtSetHideMsgs,
    EImaExtSetOpenHtmlMail,
    EImaAoVersion,
    EImaAoOnlineState,
    EImaAoWeekDays,
    EImaAoTimeStartH,
    EImaAoTimeStartL,
    EImaAoTimeStopH,
    EImaAoTimeStopL,
    EImaAoInboxRefreshTime,
    EImaAoUpdateMode,
    EImaAoLastSuccessfulUpdateH,
    EImaAoLastSuccessfulUpdateL,
    EImaAoLastUpdateFailed,
    EImaAoUpdateSuccessfulWithCurSettings,
    // 3.2
    EImaExtSetMailDeletion,
    EImaExtIsPredefined,
    EImaKeyLast                 // Keep this last!
    };
    
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  Definitions: 
*   - Mailbox Id :  TMsvId, TEntry.Id()
*                   Entry Id of mail service entry. 
*   - AccountId  :  TUint32, TEntry.iMtmData2
*                   Index of the Symbian account in the Central Repository 
*                   for the specific protocol. Each protocol can have 
*                   account id of 1. Can get values from 1 to 6.
*   - ExtensionId : TUint32 (a.k.a ExtendedAccountId)
*                   Index of the S60 account in the Central Repository for
*                   the specific protocol. Extension Id is always unique.
*                   Can get any value except 0.
*                       Imap4 keys  : 0x00000001 - 0x000000ff
*                       Pop3 keys   : 0x00000100 - 0x0000ff00
*   - Base Key   :  Extension id shifted left by 8 (1 byte). This basekey 
*                   is then used to form the unique key id to refer the 
*                   setting item.
*                       Imap4 keys  : 0x000001xx - 0x0000ffxx
*                       Pop3 keys   : 0x000100xx - 0x00ff00xx
*
*  @lib ImumUtils.lib
*  @since S60 3.0
*/
class IMASAccountControl
    {
    public:  // Constructors and destructor
    public: // New functions

        /**
        *
        * @since S60 3.0
        */
        static TUint32 CreateBaseKeyFromAccountId(
            const TUint32 aAccountId,
            const TUid& aProtocol );

        /**
        *
        * @since S60 3.0
        */
        static TUint32 CreateBaseKeyFromExtendedAccountId(
            const TUint32 aExtendedAccountId );

        /**
        *
        * @since S60 3.0
        */
        static TUint32 AccountIdToExtendedAccountId(
            const TUint32 aAccountId,
            const TUid& aMtmId );

        /**
        *
        * @since S60 3.0
        */
        static TUint32 ExtendedAccountIdToAccountId(
            const TUint32 aExtension,
            const TUid& aMtmId );

        /**
        *
        * @since S60 3.0
        */
        static TUint32 MailboxIdToAccountIdL(
            const TMsvId aMailboxId,
            const TUid& aMtmId );

        /**
        *
        * @since S60 3.0
        */
        static TMsvId AccountIdToMailboxId(
            CEmailAccounts& aAccounts,
            const TUint32 aAccountId,
            const TUid& aMtmId );
                    
        /**
        *
        * @since Series60 3.0
        */
        static TUint32 CreateSettingKey(
            const TUint32 aAccountId,
            const TUint32 aSetting,
            const TUid& aMtmId );            

    public: // Functions from base classes

    protected:  // Constructors
    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions
        
        /**
        * Searches the correct account id to be used to load settings
        * @since S60 3.2
        * @param aEntry aPopMailboxId Id of the Pop3 mailbox
        * @return Account id of the mailbox
        */
        static TPopAccount GetPopAccountIdL( 
            const TMsvId aPopMailboxId );

        /**
        * Searches the correct account id to be used to load settings
        * @since S60 3.2
        * @param aEntry aImapMailboxId Id of the Imap4 mailbox
        * @return Account id of the mailbox
        */
        static TImapAccount GetImapAccountIdL( 
            const TMsvId aImapMailboxId );
            
        /**
        * Searches the correct account id to be used to load settings
        * @since S60 3.2
        * @param aEntry aSmtpMailboxId Id of the Smtp mailbox
        * @return Account id of the mailbox
        */
        static TSmtpAccount GetSmtpAccountIdL( 
            const TMsvId aSmtpMailboxId );             
            
    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data
    };

#endif //  IMASACCOUNTCONTROL_H

// End of File
