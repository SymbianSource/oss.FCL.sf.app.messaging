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
* Description:  
*     Sms Mtm Ui.
*
*/



#if !defined(__SMSUCDMA_H__)
#define __SMSUCDMA_H__

#if !defined(__MTMUIBAS_H__)
#include <MTMUIBAS.H>                    // CBaseMtmUi
#endif

#include "smsu.h"
// CLASS DECLARATION

/**
*  CSmsMtmUiCDMA
*  Inherited from CSmsMtmUi
*  MTM implementation for CDMA SMS 
*  Overloads the GSM method for lauching CDMA specific settings dialog and RFS functionalities
*/
NONSHARABLE_CLASS(CSmsMtmUiCDMA) : public CSmsMtmUi
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSmsMtmUiCDMA* NewL(CBaseMtm& aBaseMtm,CRegisteredMtmDll& aRegisteredMtmDll);
        
        /**
        * Destructor.
        */
        virtual ~CSmsMtmUiCDMA();

    public: // Functions from base classes

        /**
        * From CSmsMtmUi
        * Context-specific
        */
        CMsvOperation* OpenL(TRequestStatus& aStatus); 

        /**
        * From CSmsMtmUi
        */
        void InvokeSyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, TDes8& aParameter);

    private: // New functions
        /**
        * Launch the service settings dialog on the current entry
        * @param aStatus follows the operation progress
        * @return pointer to started operation
        */
        CMsvOperation* LaunchSettingsDialogL( TRequestStatus& aStatus );

        /**
        * Creates Sms Service
        * @param aNoServiceNoCheckNeeded is check needed or not
        * @return id of service
        */
        TMsvId CreateSmsServiceL( TBool aNoServiceNoCheckNeeded );
        
        /**
        * Actual Sms service default creator 
        * @param aServiceId of service
        */
        void CreateDefaultSettingsL( TMsvId aServiceId );

        /**
        * Reads default / variated settings from shared data 
        * @param aServiceSettings pointer to service
        */
        void ReadDefaultSettingsFromSharedDataL( CSmsSettings* aServiceSettings );

        /**
        * Take the name and number fields from the recipients array and concaternate them
        * together, putting <> around the numbers and separating recipients with a comma
        * @return pointer to descriptor
        */
        HBufC* TurnRecipientsArrayIntoStringLC( 
            const CArrayPtrFlat<CSmsNumber>& aRecip, 
            const TBool aEmailMessage = EFalse ) const;

        /**
        * Returns a pointer to Sms Client Mtm
        * @return reference to Sms Client Mtm
        */
        inline CSmsClientMtm& SmsClientMtm() const { return (CSmsClientMtm&)BaseMtm();};

        /**
        * Checks do we have sms service and store for it, if not creates (if wanted)
        * one with default settings
        * @param aCreateService if missing create one
        * @param aCreateStore if missing create one
        * @param aFetchSMSCs if something recreated fetch SMSCs from SIM
        * @return TInt enum TSmumSmsReCreation tells what was done 
        */
        TInt CheckAndEnsureSmsServiceOkL( 
            TBool aCreateService, 
            TBool aCreateStore, 
            TBool aFetchSMSCs );
        
        /**
        * Determines the current MO state of a SMS message
        * @param aContext SMS message Context
        * @return HBufC* Pointer to the string describing the current status
        */ 
        HBufC* StatusTextL( const TMsvEntry& aContext ) const;

        /**
        * Extracts all the required data from the mtm, populates a message info array
        * and passes this array to the message info dialog class which displays the
        * dialog.
        * This is used for sms's and also created bio messages (which use sms as the
        * bearer).
        * @return
        */
        void ConstructAndShowInfoDialogL() const;

    private:

        /**
        * By default Symbian OS constructor is private.
        */        
	    void ConstructL();

        /**
        * Constructor (accessible only through factory function)
        */
        CSmsMtmUiCDMA(CBaseMtm& aBaseMtm,CRegisteredMtmDll& aRegisteredMtmDll);

    private: // Data 
        // iNoCallbackNumber is a string, read from loc file, to be displayed
        // when there is no callback number set
        HBufC*              iNoCallbackNumber;
        // Message Queue
        TBool				iMsgQueueEnabled;
        TInt                iMsgQueueTimeout;
  };

#endif // __SMSUCDMA_H__



//  End of File
