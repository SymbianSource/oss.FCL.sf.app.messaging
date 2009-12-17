/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*       UniEditor Plugin interface definition.
*
*/



#ifndef __UNIPLUGINAPI_H
#define __UNIPLUGINAPI_H

// INCLUDES
#include <e32base.h>
#include <msvstd.h>
#include <mtclbase.h>
#include <gsmuelem.h>

// DATA TYPES

// FORWARD DECLARATIONS
class CMsvSession;
class CMsvStore;
class CMsvOperation;
class CUniClientMtm;
class TUniSendingSettings;

// CLASS DECLARATION

struct TUniPluginParams
	{
	// Constructor
	TUniPluginParams( CMsvSession& aSession, CUniClientMtm& aUniMtm )
	    : iSession( aSession ), iUniMtm( aUniMtm ) {}
	// Data
	CMsvSession& iSession;
	CUniClientMtm& iUniMtm;
	};

/**
* CUniEditorPlugin
*/ 
class CUniEditorPlugin : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aImplementationUid The implementation UID of the
        *        MTM plugin to be created.
        * @return Pointer to object of CUniEditorPlugin
        */
        IMPORT_C static CUniEditorPlugin* NewL(
            const TUid& aImplementationUid,
            CMsvSession& aSession,
            CUniClientMtm& aUniMtm );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CUniEditorPlugin();

    public: // New functions

        /**
        * Loads MTM Specific headers for a message
        * @param aStore Read only store
        */
        virtual void LoadHeadersL( CMsvStore* aStore ) = 0;
        
        /**
        * Saves MTM Specific headers for message
        * @param aStore Read/write store
        */
        virtual void SaveHeadersL( CMsvStore& aStore ) = 0;
        
        /**
        * Converts MTM specific message into unified message type
        * @param aId TMsvId of the message to be converted
        * @return TmsvId of the converted message
        */
        virtual TMsvId ConvertFromL( TMsvId aId ) = 0;

        /**
        * Converts unified message into MTM specific message 
        * @param aId TMsvId of the message to be converted
        * @return TmsvId of the converted message
        */
        virtual TMsvId ConvertToL( TMsvId aId ) = 0;
        
        /**
        * Creates unified reply message 
        * @param aSrc TMsvId of MTM specific message to be replied
        * @param aDest The folder entry to which to assign the reply message
        * @param aParts The parts that are to be copied from the original message 
        *               into the reply 
        * @return TmsvId of the new unified message message
        */
        virtual TMsvId CreateReplyL( TMsvId aSrc, TMsvId aDest, TMsvPartList aParts ) = 0;

        /**
        * Creates unified forward message based on MTM specific message 
        * @param aSrc TMsvId of the message to be converted
        * @param aDest The folder entry to which to assign the forwarded message 
        * @param aParts The parts that are to be copied from the original message 
        *               into the forward message 
        * @return TmsvId of the new unified message
        */
        virtual TMsvId CreateForwardL( TMsvId aSrc, TMsvId aDest, TMsvPartList aParts ) = 0;
                
        /**
        * Sends a message which has already been converted into MTM specifc message
        * @param aId, TMsvId of the message to be sent
        */
        virtual void SendL( TMsvId aId ) = 0;

        /**
        * Validates settings. Parameter only needed for SMS messages.
        * @param aEmailOverSms, boolean informing if this message is a E-Mail over SMS message
        * @return ETrue/EFalse , valid or invalid
        */
        virtual TBool ValidateServiceL( TBool aEmailOverSms = EFalse ) = 0;

        /**
        * Copies MTM specific settings into parameter
        * @param aSettings, on return contains MTM specific settings
        */
        virtual void GetSendingSettingsL( TUniSendingSettings& aSettings ) = 0;

        /**
        * Sets MTM specific sending settings based on TUniSendingSettings
        * @param aSettings Unified sending settings
        */
        virtual void SetSendingSettingsL( TUniSendingSettings& aSettings ) = 0;

        /**
        * Checks whether service settings are valid.
        * @return ETrue/EFalse, valid or invalid
        */
        virtual TBool IsServiceValidL() = 0;

        /**
         * To Set encoding settings like encoding type, character support
         * and alternative encoding if any
         * Turkish SMS-PREQ2265 Specific 
         */
        virtual void SetEncodingSettings (TBool aUnicodeMode, TSmsEncoding aAlternativeEncodingType, TInt aCharSupportType) = 0;
        
        /**
         * To get PDU Info: extracts details of number of PDUs, number of remaining chars in last PDU
         * and encoding types used.
         * Turkish SMS-PREQ2265 Specific
         */
        virtual void GetNumPDUsL (
                TDesC& aBuf,
                TInt& aNumOfRemainingChars,
                TInt& aNumOfPDUs,
                TBool& aUnicodeMode, 
                TSmsEncoding& aAlternativeEncodingType) = 0;
        
    private:  // Data
        // Implementation UID of the sending service
        TUid iDtor_ID_Key;

    };

#endif      // __UNIPLUGINAPI_H

// End of File
