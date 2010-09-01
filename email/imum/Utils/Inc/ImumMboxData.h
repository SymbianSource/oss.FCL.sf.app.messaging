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
*       Class declaration file
*
*/




#ifndef __ImumMboxData_H__
#define __ImumMboxData_H__

// INCLUDES
#include <e32std.h>
#include <msvstd.h>
#include <cemailaccounts.h>
#include <iapprefs.h>

#include "Imas.hrh"

// CONSTANTS
// MACROS
// DATA TYPES

/**
* Panics
*/
enum TImumImasPanic
    {
    EImasPanicInvalidMailbox = 0,
    EImasPanicNullDbTable,
    EImasPanicControlNotFound,
    KImasPanicNoServicesDefined,
    EImasPanicInvalidSignatureSource,
    EImasPanicServiceIdsNotCreated,
    EImasPanicAccountIdsNotCreated,
    EImasPanicDefaultAccountNotFound,
    EImasPanicNoSettings,
    EImasPanicBadEngineSetting,
    EImasPanicBadListIndex,
    EImasPanicIdArrayNotNull,
    EImasPanicUnknownSettings

    };

// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CRichText;
class CParaFormatLayer;
class CCharFormatLayer;
class CImSmtpSettings;
class CImPop3Settings;
class CImImap4Settings;
class CImIAPPreferences;
class CImBaseEmailSettings;
class CImumMboxDataExtension;
GLREF_C void ImasPanic( TImumImasPanic aCode );

// CLASS DECLARATION

///////////////////////////////////////////////////////////////////////////////
//
// ACCOUNT SETTINGS DATA
//
///////////////////////////////////////////////////////////////////////////////

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib Imum.lib
*  @since S60 3.0
*/
class CMsvImailSignature : public CBase
    {
    public:

        /**
        * CMsvImailSignature
        */
        CMsvImailSignature();

        /**
        * CMsvImailSignature
        * @param CRichText*
        * @param CParaFormatLayer*
        * @param CCharFormatLayer*
        */
        CMsvImailSignature(
            CRichText* aRichText,
            CParaFormatLayer* aParaFormat,
            CCharFormatLayer* aCharFormat );

        /**
        * ~CMsvImailSignature
        * Destructor
        */
        ~CMsvImailSignature();

        /**
        * CopyL
        * @param CMsvImailSignature&
        */
        void CopyL(const CMsvImailSignature& aSignature);

        /**
        * CreateEmptyRichTextL
        */
        void CreateEmptyRichTextL();

    public:
        CRichText* iRichText;
        CParaFormatLayer* iParaFormatLayer;
        CCharFormatLayer* iCharFormatLayer;
    };

///////////////////////////////////////////////////////////////////////////////

/**
* class CImumMboxData
*
*/
class CImumMboxData : public CBase
    {
    public:

        /**
        * NewL function to create settings
        * @since S60 2.0
        */
        static CImumMboxData* NewL();

        /**
        * NewLC function to create settings
        * @since S60 2.0
        */
        static CImumMboxData* NewLC();

        /**
        * NewL function to create POP3 settings
        * @since S60 3.0
        * @param aName, name
        * @param aSmtpSettings, pointer to smtp settings
        * @param aPop3Settings, pointer to pop3 settings
        * @return Pointer to settings
        */
        static CImumMboxData* NewL(
            const TDesC& aName,
            CImSmtpSettings* aSmtpSettings,
            CImPop3Settings* aPop3Settings,
            CImumMboxDataExtension* aExtendedSettings = NULL );

        /**
        * NewLC function to create POP3 settings
        * @since S60 3.0
        * @param aName, name
        * @param aSmtpSettings, pointer to smtp settings
        * @param aPop3Settings, pointer to pop3 settings
        * @return Pointer to settings
        */
        static CImumMboxData* NewLC(
            const TDesC& aName,
            CImSmtpSettings* aSmtpSettings,
            CImPop3Settings* aPop3Settings,
            CImumMboxDataExtension* aExtendedSettings = NULL );

        /**
        * NewL function to create IMAP4 settings
        * @since S60 3.0
        * @param aName, name
        * @param aSmtpSettings, pointer to smtp settings
        * @param aImap4Settings, pointer to pop3 settings
        * @return Pointer to settings
        */
        static CImumMboxData* NewL(
            const TDesC& aName,
            CImSmtpSettings* aSmtpSettings,
            CImImap4Settings* aImap4Settings,
            CImumMboxDataExtension* aExtendedSettings = NULL );

        /**
        * NewLC function to create IMAP4 settings
        * @since S60 3.0
        * @param aName, name
        * @param aSmtpSettings, pointer to smtp settings
        * @param aImap4Settings, pointer to pop3 settings
        * @return Pointer to settings
        */
        static CImumMboxData* NewLC(
            const TDesC& aName,
            CImSmtpSettings* aSmtpSettings,
            CImImap4Settings* aImap4Settings,
            CImumMboxDataExtension* aExtendedSettings = NULL );

        /**
        * ~CImumMboxData
        */
        ~CImumMboxData();

        /**
        * CopyL
        * @param CImumMboxData&, settings reference
        */
        void CopyL( const CImumMboxData& aSettings );

        /**
        * RelatedMtmSettings
        * @return settings reference
        */
        CImBaseEmailSettings& RelatedMtmSettings() const;

        /**
        *
        * @since S60 3.0
        */
        void InitializeL();

        /**
        *
        * @since S60
        * @return
        */
        void Clean();

        /**
        *
        * @since S60 3.0
        */
        TBool IsOk() const;

    protected:

        /**
        * CImumMboxData
        */
        CImumMboxData();

        /**
        * CImumMboxData
        * @since S60 3.0
        * @param TDesC&, name
        * @param CImSmtpSettings*, smtp settings
        * @param CImPop3Settings*, pop3 settings
        * @param CImImap4Settings*, imap4 settings
        */
        CImumMboxData(
            const TDesC& aName,
            CImSmtpSettings* aSmtpSettings,
            CImPop3Settings* aPop3Settings,
            CImImap4Settings* aImap4Settings,
            CImumMboxDataExtension* aExtendedSettings );

        /**
        * ConstructL
        * @since S60
        */
        void ConstructL();

    public: // Data

        // Common settings
        TBool                       iIsImap4;
        TBuf<KImasImailServiceNameLength> iName;
        CMsvImailSignature*         iSignature;
        TMsvId                      iMailboxId;
        TMsvId                      iDefaultAccountId;

        // IMAP4 settings
        CImImap4Settings*           iImap4Settings;
        TImapAccount                iImap4AccountId;

        // POP3 settings
        CImPop3Settings*            iPop3Settings;
        TPopAccount                 iPop3AccountId;

        // SMTP settings
        CImSmtpSettings*            iSmtpSettings;

        // Extra settings
        CImumMboxDataExtension*     iExtendedSettings;

        // Internet Access Point settings
        //  - Incoming server
        TUint32                     iIncomingIap;
        TCommDbDialogPref           iIncomingDialogPref;
        CImIAPPreferences*          iIncomingIapPref;
        //  - Incoming server
        TUint32                     iOutgoingIap;
        TCommDbDialogPref           iOutgoingDialogPref;
        CImIAPPreferences*          iOutgoingIapPref;

    };

#endif // __ImumMboxData_H__
