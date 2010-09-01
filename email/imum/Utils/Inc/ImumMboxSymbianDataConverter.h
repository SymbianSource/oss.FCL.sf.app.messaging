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
* Description:  ImumMboxSymbianDataConverter.h
*
*/


#ifndef CIMUMMBOXSYMBIANDATACONVERTER_H
#define CIMUMMBOXSYMBIANDATACONVERTER_H

// INCLUDES
#include <e32base.h>
#include <imapset.h>

#include "ImumInMailboxUtilities.h"
#include "cdbcols.h"
#include "ImumMboxDataExtension.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CImumInternalApiImpl;
class CImumInternalApiImpl;
class CImumInSettingsData;
class CImumMboxData;
class MImumInSettingsDataCollection;
class CImImap4Settings;
class CImPop3Settings;
class CImSmtpSettings;
class CImumMboxDataExtension;
class CMsvCommDbUtilities;
class TMuiuFlags;

// CLASS DECLARATION

/**
*
*  @lib
*  @since Series60 3.0
*/
NONSHARABLE_CLASS( CImumMboxSymbianDataConverter ) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Creates object from CImumMboxSymbianDataConverter and leaves it to cleanup stack
        * @since Series60 3.0
        * @return, Constructed object
        */
        static CImumMboxSymbianDataConverter* NewLC(
            CImumInternalApiImpl& aMailboxApi,
            const CImumInSettingsData& aSourceData,
            CImumMboxData& aTargetData );

        /**
        * Create object from CImumMboxSymbianDataConverter
        * @since Series60 3.0
        * @return, Constructed object
        */
        static CImumMboxSymbianDataConverter* NewL(
            CImumInternalApiImpl& aMailboxApi,
            const CImumInSettingsData& aSourceData,
            CImumMboxData& aTargetData );

        /**
        * Destructor
        * @since Series60 3.0
        */
        virtual ~CImumMboxSymbianDataConverter();

    public: // New functions

        /**
         *
         *
         * @since S60 v3.2
         */
        static CImumMboxData* ConvertToSymbianMboxDataLC(
            CImumInternalApiImpl& aMailboxApi,
            const CImumInSettingsData& aSourceData );

    public: // Functions from base classes

    protected:  // Constructors

        /**
        * Default constructor for classCImumMboxSymbianDataConverter
        * @since Series60 3.0
        * @return, Constructed object
        */
        CImumMboxSymbianDataConverter(
            CImumInternalApiImpl& aMailboxApi,
            const CImumInSettingsData& aSourceData,
            CImumMboxData& aTargetData );

        /**
        * Symbian 2-phase constructor
        * @since Series60 3.0
        */
        void ConstructL();

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions

        /**
         *
         *
         * @since S60 v3.2
         */
        void DoConvertToSymbianMboxDataL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         */
        void ConvToSymbianImap4DataL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         */
        void ConvToSymbianPop3DataL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         */
        void ConvToSymbianSmtpDataL();


        /**
         *
         *
         * @since S60 v3.2
         * @param
         */
        void DoPopulateSymbianImap4SettingsL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         */
        void DoPopulateSymbianPop3SettingsL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         */
        void DoPopulateSymbianSmtpSettingsL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         */
        void DoPopulateExtendedSettingsL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        void SetSymbianAccessPointL( const TInt aIndex );

        /**
         * Collects relevant data for storing the settings from the entry.
         *
         * @since S60 v3.2
         */
        void CollectEntryDataL();

        /**
         * Sets partial fetching status for Imap4 protocol.
         *
         * @since S60 v3.2
         * @param aFlags Download flags
         * @return Download flags in a form that can be stored to database.
         */
        TImImap4PartialMailOptions GetSymbianImapPartial(
            const TMuiuFlags& aFlags );

        /**
         * Converts the always online setting to form it can be stored.
         *
         * @since S60 v3.2
         * @param aAoState Internal API's Always Online setting
         * @return Always online state in a form that can be stored to database.
         */
        TMailAoStates SymbianAoState( const TInt aAoState );

        /**
         * Converts the always online interval setting to form it can be stored.
         *
         * @since S60 v3.2
         * @param aInterval Internal API's Always Online retrieval interval
         * @return Retrieval interval in a form that can be stored to database.
         */
        TInt SymbianAoRetrievalInterval( TInt aInterval );

        /**
         * Converts the EMN setting to form it can be stored.
         *
         * @since S60 v3.2
         * @param aEmnState Internal API's EMN setting
         * @return Emn state in a form that can be stored to database.
         */
        TMailEmnStates SymbianEmnState( const TInt aEmnState );

        /**
         * Set synchronization state.
         *
         * @since S60 v3.2
         * @param aFlags Flags to control synchronization setting.
         * @return Setting type.
         */
        TPop3GetMailOptions GetSymbianPopGetMail(
            const TUint aFlags );

        /**
         * Sets the correct download mode.
         *
         * @since S60 v3.2
         * @param aFlags Flags to control the download settings.
         * @param aInternalDataCollection Reference to connection data object.
         * @return Setting type.
         * @leave Any settigs leave code
         * @see ImumDaErrorCodes.h
         */
        TInt32 GetSymbianPopDownloadL(
            const TUint aFlags,
            const MImumInSettingsDataCollection& aInternalDataCollection );

        /**
         * Throws an exception.
         * Debug mode: Panic
         * Release mode: Leave
         *
         * @since S60 v3.2
         * @param aErrorCode The code of the exception.
         * @leave Provided error code
         */
        static void ConverterExceptionL( const TInt aErrorCode );

    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        // Reference to mailbox Api object
        CImumInternalApiImpl&   iMailboxApi;
        // Not owned: Reference to symbian data
        CImumMboxData&          iTargetData;
        // Not owned: Reference to internal data
        const CImumInSettingsData& iSourceData;
        // Protocol of the mailbox
        TUid                    iMailboxProtocol;
        // Owned: Entries of the mailbox
        MImumInMailboxUtilities::RMsvEntryArray iMailboxEntries;
        // Flag to indicate if the settings is in construction mode
        TBool                   iCreationPhase;
    };

#endif //  CIMUMMBOXSYMBIANDATACONVERTER_H
