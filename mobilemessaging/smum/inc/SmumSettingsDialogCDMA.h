/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*     Main settings dialog for Sms.
*
*/



#ifndef SMUMSETTINGSDIALOGCDMA_H
#define SMUMSETTINGSDIALOGCDMA_H

//  INCLUDES
#include <SmumSettingsDialog.h> // CSmumSettingsDialog
#include <bldvariant.hrh>
#include <CPbkContactEngine.h>              // Phonebook Engine
#include <CPbkContactItem.h>                // Phonebook Contact
#include <RPbkViewResourceFile.h>
#include <CentralRepository.h>
#include <MessagingInternalCRKeys.h>

// DATA TYPES

// LBI = ListBoxIndex

enum TSmumMainSmsSettingsSettingLBIndexCDMA
    {
    ESmumPriorityLevelLBICDMA       = 0,
    ESmumCallbackNumberLBICDMA      = 1,
    ESmumMessageQueuingLBICDMA      = 2,
    ESmumDeliveryReportLBICDMA      = 3
    };

enum TSmumMainSmsSendingOptSettingLBIndexCDMA
    {
    ESmumSendOptPriorityLevelLBICDMA	   = 0,
	ESmumSendOptCallbackNumberLBICDMA	   = 1,
    ESmumCdmaSendOptDeliveryReportLBICDMA  = 2
    };

enum TSmumSettingsPriorityCDMA
    {
    ESmumPriorityNormal			= 0,
    ESmumPriorityUrgent			= 1
    };

enum TSmumCallbackNumberSettingsList
    {
    ESmumUseThisPhoneNumber		= 0,
    ESmumAddFromContact         = 1,
    ESmumEnterPhoneNumber       = 2,
    ESmumNoCallbackNumber       = 3
    };

enum TSmumMessageQueuing
	{
	ESmumQueuingOn   =0,
	ESmumQueuingOff  =1
	};

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
NONSHARABLE_CLASS(CSmumSettingsArray);
NONSHARABLE_CLASS(CEikTextListBox);
NONSHARABLE_CLASS(CAknTitlePane);
NONSHARABLE_CLASS(CSmsAccount);

// CLASS DECLARATION

/**
*  SMUM's main sms settings dialog.
*/
NONSHARABLE_CLASS(CSmumMainSettingsDialogCDMA) : public CSmumMainSettingsDialog
    {
    public:  // Constructors and destructor
      
        /**
        * Two-phased constructor.
        */
        static CSmumMainSettingsDialog* NewL(
            CSmsSettings& aSettings,
            TInt aTypeOfSettings,
            TInt& aExitCode,
            CSmsHeader* aSmsHeader = NULL );

        /**
        * Destructor.
        */
        ~CSmumMainSettingsDialogCDMA();

    protected:  // Functions from base classes
        
        /**
        * From CAknDialog 
        */
        void ProcessCommandL(TInt aCommandId);
        
        /**
        * From CEikDialog
        */
        void PreLayoutDynInitL();

        /**
        * From CEikDialog
        */
        TBool OkToExitL(TInt aButtonId);

        /**
        * From CEikDialog
        */
        void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);

        /**
        * From MEikListBoxObserver
        */
        void HandleListBoxEventL(
            CEikListBox* aListBox, 
            TListBoxEvent aEventType );

        /**
        * From CAknDialog
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

        /**
        * From CCoeControl, returns help context
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;        

    private: // New functions
        
        /**
        * Initializing the values for listboxes in Sms Settings from Sms settings.
        * @param none
        * @return nothing
        */
        void OpeningDialogL();

        /**
        * Updating the values from listboxes to Sms settings.
        * @param none
        * @return nothing
        */
        void ClosingDialogL() const;

        /**
        * Launch Service Centres dialog.
        * @param none
        * @return TInt button id
        */
        TInt LaunchServiceCentresDialogL();

        /**
        * Updating "SC in use" dynamic array. 
        * @param none
        * @return nothing
        */
        void UpdateSCInUseArrayL();

        /**
        * Check what to open depending the focus of listbox
        * @param TBool aEnterPressed
        * @return nothing
        */
        void CheckOpeningListBoxItemL(TBool aEnterPressed);

        
		/**
		* Reads the phone's won number from the NAM
		* @param TBuf<KSMSCallBackNumberLenght> ownNumber 
		* @return nothing
		*/
		void ReadOwnNumberL(TDes& aOwnNumber);

		/**
		* Selects a callback number from the phonebook engine
		* @param TBuf<KSMSCallbackNumberLength> number
		* @return nothing
		*/
        void DoAddressBookDialogL(TDes& aNumber);
        
		
		/**
        * Set item to listbox.
        * @param TInt aSettingLBIndex, TInt aValue
        * @return nothing
        */
        void SetItem(TInt aSettingLBIndex, TInt aValue);// new method

        /**
        * Get item value from listbox.
        * @param TInt TInt aSettingLBIndex
        * @return TInt
        */        
        TInt Item(TInt aSettingLBIndex) const;// new method

        /** 
        * Launches help application
        */
        void LaunchHelpL();
        
        /*
        * Checks if message queuing is on
        * @returns true if queuing is On
        */
        TBool IsMsgQueuingOnL();
		
		/*
		* Sets Message Queuing On/Off
		* @param TBool aSetting On/Off setting
		*/
		void SetMsgQueuingOnL(TBool aSetting) const;
		
    private :

        /**
        * C++ default constructor.
        */
        CSmumMainSettingsDialogCDMA(
            CSmsSettings& aSettings,
            TInt aTypeOfSettings,
            TInt& aExitCode,
            CSmsHeader* aSmsHeader = NULL );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
               
    private:  // Data
        CSmumSettingsArray*     iSettingsArray;
        CEikTextListBox*        iListBox;
        CSmsSettings&           iSettings;
        TInt                    iTypeOfSettings;    // for knowing the type 0 = sms settings, 1 = send options, 2 = bio
        TInt&                   iExitCode;
        CSmsHeader*             iSmsHeader;         // for replypath handling
        CAknTitlePane*          iTitlePane;         // for handling titlepanes
        HBufC*                  iPreviousTitleText; // - " -
        TBool                   iLaunchDialog;
        TBool                   iHelpFeatureSupported;
        HBufC*                  iNoCallbackNumber;  // read from loc, to be displayed when there is no callback number set
        CPbkContactEngine*      iPbkEngine;
        CSmsAccount*            iSmsAccount;         // For Queuing Settings
        RPbkViewResourceFile    iPhonebookResource;
        TInt                    iMaxCallbackLength;  // Maximum callback number length
        TBool                   iDeliveryAckSupport; // Support for delivery ack
    };

#endif      // SMUMSETTINGSDIALOGCDMA_H   

// End of File
