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
*       Static functions for imum
*
*/


#ifndef __EMAILUTILS_H__
#define __EMAILUTILS_H__

// INCLUDES
#include <eikdialg.h>
#include <msvapi.h> // CMsvOperation
#include <miutmsg.h> // CImEmailMessage
#include <Muiumsginfo.h>
#include <AlwaysOnlineManagerClient.h>
#include <ConeResLoader.h>  // RConeResourceLoader
#include <mtmuibas.h>
#include "MuiuDynamicHeader.h"
#include "ImumMboxSettingsCtrl.h"
#include "ImumMboxData.h"
#include "ImumDaSettingsKeys.h"

// CONSTANTS
enum TIMASMailboxType
    {
    EMailboxPOP3,
    EMailboxIMAP4
    };

enum TIMASMailSecurity
    {
    EImumSecurityOn = 0,
    EImumSecurityMs,
    EImumSecurityOff
    };

enum TIMASMailPort
    {
    EIMASPortDefault = 0,
    EIMASPortUserDefined
    };

enum TFetchAttachments
    {
    EFetchAttachments = 0,
    EFetchBodyOnly
    };

enum TSendOption
    {
    EImmediately = 0,
    EOnNextConnection
    };

enum TIMASOnOff
    {
    EIMASOn = 0,
    EIMASOff
    };

enum TIMASYesNo
    {
    EIMASYes = 0, // yes is 0 because it is ui specified so that 'yes' options is first in the list
    EIMASNo
    };

enum TIMASFetchHeaders
    {
    EIMASFetchAll = 0,
    EIMASFetchUserDefined,
    EIMASFetchPastNumberOfDays
    };

/**
* User defined KB if higher that 0, so if some KB has been
* set, the value being bigger that 0 already tells us that
* the setting is userdefined
*/
enum TIMASFetchedPartsMapping
    {
    EIMASMapHeadersOnly = -2,
    EIMASMapFullBody = -1,
    EIMASMapUserDefinedKB = 0
    };

enum TIMASFetchedPartsButtons
    {
    EIMASHeadersOnly = 0,
    EIMASLessThan,
    EIMASFullBody, // only for IMAP
    EIMASBodyAndAttachments
    };

enum TIMSAoInterval
    {
    EIMASAoInterval5Min = 0,
    EIMASAoInterval15Min,
    EIMASAoInterval30Min,
    EIMASAoInterval1Hour,
    EIMASAoInterval2Hours,
    EIMASAoInterval4Hours,
    EIMASAoInterval6Hours
    };

enum TIMASAoHoursButtons
    {
    EIMASAoHoursAll = 0,
    EIMASAoHoursUserDefined
    };

const TInt KIMASDefaultPortImap4        = 143;
const TInt KIMASDefaultSecurityPortImap4 = 993;
const TInt KIMASDefaultPortPop3         = 110;
const TInt KIMASDefaultSecurityPortPop3 = 995;
const TInt KIMASDefaultPortSmtp         = 25;
const TInt KIMASDefaultSecurityPortSmtp = 465;
const TInt KImumMaxMailboxes            = 6;
const TInt KIMASMaxAccounts             = 255;

const TInt KIMASAoInterval5Min   = 5;
const TInt KIMASAoInterval15Min  = 15;
const TInt KIMASAoInterval30Min  = 30;
const TInt KIMASAoInterval1Hour  = 60;
const TInt KIMASAoInterval2Hours = 120;
const TInt KIMASAoInterval4Hours = 240;
const TInt KIMASAoInterval6Hours = 360;

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CImEmailMessage;
class CEikonEnv;
class CImBaseEmailSettings;
class CContactItemFieldSet;
class CImIAPPreferences;
class CRepository;
class CImImap4Settings;
class CImPop3Settings;
class CImSmtpSettings;
class CImumInternalApi;

// CLASS DECLARATION

/**
*  Static functions for imum
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.0
*/
class MsvEmailMtmUiUtils
    {
    public:

        /**
        * Check there is enough disk space on the specified drive to
        * fetch a message of aMsgSize bytes.
        * If not, display a message and return EFalse.
        */
        IMPORT_C static TBool CheckAvailableDiskSpaceForDownloadL(
            TInt aMsgSize,
            CEikonEnv& aEikonEnv,
            CMsvSession& aSession );

        /**
        * Do a message server entry copy or move.
        */
        IMPORT_C static CMsvOperation* DoCEntryCopyMoveL(
            CMsvEntry& aEntry,
            const CMsvEntrySelection& aSel,
            TMsvId aTargetId,
            TRequestStatus& aCompletionStatus,
            TBool aCopy);

        /**
        * Strip message entries that have their
        * 'Complete' flag set from a CMsvEntrySelection.
        */
        IMPORT_C static CMsvEntrySelection* StripCompleteEntriesLC(
            const CMsvEntry& aParent,
            const CMsvEntrySelection& aSelection);

        /**
        * Strip message entries that have their 'Deleted'
        * flag set from a CMsvEntrySelection.
        */
        IMPORT_C static CMsvEntrySelection* StripDeletedEntriesLC(
            const CMsvEntry& aParent,
            const CMsvEntrySelection& aSelection);

        /**
        * Sets fields in the message info data array using
        * TMsvEntry and CImHeader information;
        * the TDes parameters are needed so that the strings
        * have persisted memory to
        * live in since the info data elements are all TPtrs
        */
        IMPORT_C static void SetMessageInfoDataLCCCCC(
            TMsgInfoMessageInfoData& aInfoData,
            CBaseMtm& aMtm,
            CCoeEnv& aEnv,
            TDes& dateBuf,
            TDes& timeBuf,
            TDes& sizeBuf,
            TDes& priorityBuf );

        /**
        * Sets fields in the message info data array using
        * TMsvEntry and CImHeader information;
        * the TDes parameters are needed so that the strings have
        * persisted memory to live in since the info data elements
        * are all TPtrs.
        * If entry has ReadStoreL this pushes 4 items to cleanup stack.
        * If ReadStoreL does not exist then only 2 items are pushed
        * to cleanup stack
        */
        IMPORT_C static TBool SetMessageInfoDataLCC(
            TMsgInfoMessageInfoData& aInfoData,
            CBaseMtm& aMtm,
            CCoeEnv& aEnv,
            TDes& dateBuf,
            TDes& timeBuf,
            TDes& sizeBuf,
            TDes& priorityBuf );

        /**
        * Replaces tab and enter characters by spaces using
        * AknTextUtils::ReplaceCharacters
        */
        static void ReplaceCharacters( TDes& aText );
        
        /**
        * Disconnect mailbox if it is tried to be
        * edited when connection is on.
        * aService is Imap4 or Pop3 service
        */
        IMPORT_C static TBool DisconnectMailboxL(
            const TMsvEntry& aService,
            CMsvSession& aSession );

        /**
        * Checks offline mode flag
        */
        IMPORT_C static TBool DoOfflineChecksL( TMsvId aService );

        /**
        * Removes 'Deleted' flag synchronoushly from single entry
        * @since S60 2.6
        * @param aBaseMtm:    client mtm (Imap4 or Pop3 in this case)
        * @param aFunctionId: 'remove delete flag' function id
        * @param aEntryId:    id of the entry
        * @return standard error code
        */
        IMPORT_C static TInt RemoveOfflineDeleteL(
            CBaseMtm& aBaseMtm,
            TInt aFunctionId,
            TMsvId aEntryId );

        /**
        * Function reserve disk space for deletion
        * @since S60 3.0
        * @param aFileSession, File session to be used for operation
        * @param aDriveNo, Drive where the space is needed
        * @param aSize, Amount of space needed
        * @return KErrNone, when reservation succesful
        */
        IMPORT_C static TInt AcquireDiskSpace(
            RFs& aFileSession,
            const TInt aDriveNo,
            const TInt aSize );

        /**
        * Sends specified message to mailbox
        * @since S60 3.0
        * @param aCommand, Command to be send to mailbox
        * @param aMailboxId, Mailbox the command is directed to
        */
        IMPORT_C static void SendAOCommandL(
            const TAlwaysOnlineServerAPICommands aCommand,
            TMsvId aMailboxId );

        /**
        *
        * @since S60 3.0
        */
        static TUint32 MsvId2AccountIdL(
            CMsvSession& aMsvSession,
            const TMsvId aMailboxId );

        /**
        * Creates central repository connection
        * @since S60 3.0
        * @param aRepository, Repository object to be created
        * @param aRepositoryUid, Uid to be used to create the session
        */
        static void CreateCenRepConnectionL(
            CRepository*& aRepository,
            TUid aRepositoryUid );

        /**
        *
        * @since S60 3.0
        */
        static void StoreSecuritySettings(
            CImBaseEmailSettings& aSettings,
            const TIMASMailSecurity& aSecurity );

        /**
        *
        * @since S60 3.0
        */
        static void StoreSecuritySettings(
            CImBaseEmailSettings& aSettings,
            const TImumDaSettings::TSecurityValues& aSecurity );
            
        /**
         * 
         *
         * @since S60 v3.2
         * @param 
         * @return 
         */
        static TInt RestoreSecuritySettings(
            const CImBaseEmailSettings& aSettings );
            
        /**
        *
        * @since S60 3.0
        */
        static TUint32 GetDefaultSecurityPort(
            const TIMASMailSecurity aSecurity,
            const TBool aIncoming,
            const TBool aIsImap4 = EFalse );


        /**
        * Tell to Ncn that there might be new messages to show
        * @since S60 3.0
        */
        IMPORT_C static void CallNewMessagesL( const TMsvId aMailboxId );

        /**
        * Checks the number of always online boxes
        * @since S60 3.0
        */
        static TBool CheckActiveAOLimitsL(
            CImumInternalApi& aEmailApi,            
            const TMsvId aMailboxId );

        /**
        *
        * @since S60 3.0
        */
        static TUint32 GetAccountId(
            const CImumMboxData& aAccountSettings );

        /**
        *
        * @since S60 3.0
        */
        IMPORT_C static void LoadResourceFileL(
            RConeResourceLoader& aResourceLoader );

        /**
        *
        * @since S60 3.0
        */
        static void GetMailboxName(
            TDes& aDest,
            CMsvSession& aMsvSession,
            TMsvId aMailboxId );

        /**
        *
        * @since S60 3.0
        */
        IMPORT_C static void CreateConnectingToText(
            TDes& aDest,
            CMsvSession& aMsvSession,
            TMsvId aMailboxId );

        
        /**
        *
        * @since S60 3.1
        */
        static void CheckAccountNameL(     
            CImumInternalApi& aMailboxApi,       
            TDes& aAccountName,
            const TMsvId aOwningMailbox,   
            const TBool& aGenerate = EFalse );       

        /**
        * @since S60 5.0
        * Strips tab and enter characters from text
        * @param aText, descriptor to operate on
        */
        static void StripCharacters( TDes& aText );
            
            
    private:

        /**
        *
        */
        static HBufC* TurnArrayIntoCommaSeparatedStringLC(
            CDesCArray& aTextArray );
                    
        /**
        *
        * @since S60 3.1
        */
        static TInt ContinueRenamingL( 
            CImumInternalApi& aMailboxApi,
            TInt& aResult,
            TDes& aAccountName, 
            const TMsvId aOwningMailbox,
            const TInt aNumber,
            const CMsvEntry& aAccount,
            const TBool aGenerate );            

        /**
        *
        * @since S60 3.1
        */
        static TInt ChangeAccountNameL(
            const TDesC& aOriginalAccountName,
            TDes& aAccountName,
            const TInt aNumber );
            
        /**
        *
        * @since S60 3.1
        */
        static TBool IsAccountNameUsedL(
            CImumInternalApi& aMailboxApi,
            TInt& aResult,
            const TDesC& aAccountName,
            const TMsvId aOwningMailbox,
            const CMsvEntry& aAccount );            
        
        /**
        * 
        * @since S60 3.2
        */    
        static void GetPriorityString(
            const TMsvEntry& aEntry,
            CCoeEnv& aEnv,
            TDes& aPriorityBuf );                      
    };

#endif

