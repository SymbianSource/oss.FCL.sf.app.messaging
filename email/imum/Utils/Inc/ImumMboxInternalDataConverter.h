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
* Description:  ImumMboxInternalDataConverter.h
*
*/


#ifndef CIMUMMBOXInternalDataCONVERTER_H
#define CIMUMMBOXInternalDataCONVERTER_H

// INCLUDES
#include <e32base.h>
#include <imapset.h>

#include "ImumMboxDataExtension.h"
#include "ImumInMailboxUtilities.h"

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

// CLASS DECLARATION


/**
*
*  @lib
*  @since Series60 3.0
*/
NONSHARABLE_CLASS( CImumMboxInternalDataConverter ) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Creates object from CImumMboxInternalDataConverter and leaves it to cleanup stack
        * @since Series60 3.0
        * @return, Constructed object
        */
        static CImumMboxInternalDataConverter* NewLC(
            CImumInternalApiImpl& aMailboxApi,
            const CImumMboxData& aSourceData,
            CImumInSettingsData& aTargetData );

        /**
        * Create object from CImumMboxInternalDataConverter
        * @since Series60 3.0
        * @return, Constructed object
        */
        static CImumMboxInternalDataConverter* NewL(
            CImumInternalApiImpl& aMailboxApi,
            const CImumMboxData& aSourceData,
            CImumInSettingsData& aTargetData );

        /**
        * Destructor
        * @since Series60 3.0
        */
        virtual ~CImumMboxInternalDataConverter();

    public: // New functions


        /**
         *
         *
         * @since S60 v3.2
         */
        static void ConvertToInternalMboxDataL(
            CImumInternalApiImpl& aMailboxApi,
            const CImumMboxData& aSourceData,
            CImumInSettingsData& aTargetData );

        /**
         *
         *
         * @since S60 v3.2
         */
        static CImumInSettingsData* ConvertToInternalMboxDataLC(
            CImumInternalApiImpl& aMailboxApi,
            const CImumMboxData& aSourceData );

    public: // Functions from base classes

    protected:  // Constructors

        /**
        * Default constructor for classCImumMboxInternalDataConverter
        * @since Series60 3.0
        * @return, Constructed object
        */
        CImumMboxInternalDataConverter(
            CImumInternalApiImpl& aMailboxApi,
            const CImumMboxData& aSourceData,
            CImumInSettingsData& aTargetData );

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
        void DoConvertToInternalMboxDataL();

        /**
         *
         *
         * @since S60 v3.2
         */
        static CImumInSettingsData* CreateInternalDataLC(
            CImumInternalApiImpl& aMailboxApi,
            const TUid& aProtocol );

        /**
         *
         *
         * @since S60 v3.2
         * @param
         */
        void ConvToInternalImap4DataL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         */
        void ConvToInternalPop3DataL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        void DoPopulateInternalImap4SettingsL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        void DoPopulateInternalPop3SettingsL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        void DoPopulateInternalSmtpSettingsL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        void DoPopulateInternalGeneralSettingsL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        template<TInt TAttrSize, class TAttrSettings>
        inline static void FillText8InternalL(
            TAttrSettings& aSettings,
            const TUint aAttributeKey,
            const TDesC8& aText );

        template<TInt TAttrSize, class TAttrSettings>
        inline static void FillTextInternalL(
            TAttrSettings& aSettings,
            const TUint aAttributeKey,
            const TDesC& aText );

        /**
         *
         *
         * @since S60 v3.2
         */
        void SetIdsToInternalDataL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        TInt32 GetInternalImapPartial(
            const TImImap4PartialMailOptions& aPartialOptions );

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        TInt32 GetInternalImapDownload(
            const TImap4GetMailOptions& aPartialOptions );

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        void SetInternalAccessPointL( const TInt aIndex );

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        TInt InternalAoState( const TMailAoStates& aAoState );

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        TInt InternalEmnState( const TMailEmnStates& aEmnState );

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        TInt InternalRetrivalInterval( TInt aInterval );

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        TInt32 GetInternalPopDownloadL(
            const TInt32& aSetting,
            MImumInSettingsDataCollection& aInternalDataCollection );

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        TInt32 GetInternalPopSyncL(
            const TInt32& aSetting );

    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        // Reference to mailbox Api object
        CImumInternalApiImpl&   iMailboxApi;
        // Reference to symbian data
        CImumInSettingsData&    iTargetData;
        // Reference to internal data
        const CImumMboxData&    iSourceData;

    };


#include "ImumMboxInternalDataConverter.inl"

#endif //  CIMUMMBOXInternalDataCONVERTER_H
