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
*     Main settings dialog for Sms.
*
*/



#ifndef SMUMSETTINGSDIALOGGSM_H
#define SMUMSETTINGSDIALOGGSM_H

//  INCLUDES
#include <smumsettingsdialog.h> // CSmumSettingsDialog

// CONSTANTS
const TInt KSmumStringLength = 50;

// FORWARD DECLARATIONS
class CAknNavigationControlContainer;
class CRepository;

// MACROS

// DATA TYPES
enum TSmumSettingsValidityPeriod
    {
    ESmumValidityPeriodHour     = 0,
    ESmumValidityPeriodSixHours = 1,
    ESmumValidityPeriod24Hours  = 2,
    ESmumValidityPeriod3Days    = 3,
    ESmumValidityPeriodWeek     = 4,
    ESmumValidityPeriodMaximum  = 5
    };

enum TSmumSettingsConversion
    {
    ESmumConversionNone = 0,
    ESmumConversionFax = 1,
    ESmumConversionPaging = 2
    };

enum TSmumSettingsDelivery
    {
    ESmumDeliveryImmediately = 0,
    ESmumDeliveryUponRequest = 1
    };

enum TSmumConnectionType
    {
    ESmumPreferGSMConnection = 0, 
    ESmumPreferGPRSConnection = 1
    };

// LBI = ListBoxIndex
enum TSmumMainSmsSettingsSettingLBIndex
    {
    ESmumServiceCentresLBI,      //= 0
    ESmumServiceCentreInUseLBI,  //= 1
	ESmumEmailServiceCentreLBI,  //= 2,
    ESmumCharSupportInUseLBI,    //= 3
    ESmumDeliveryReportLBI,      //= 4
    ESmumValidityPeriodLBI,      //= 5
    ESmumPreferredConnectionLBI, //= 6
    ESmumReplyViaSameCentreLBI   //= 7
    };

enum TSmumMainSmsSendingOptSettingLBIndex
    {
    ESmumSendOptServiceCentreInUseLBI,  //= 0,
    ESmumSendOptCharSupportInUseLBI,    //= 1,
    ESmumSendOptDeliveryReportLBI,      //= 2,
    ESmumSendOptValidityPeriodLBI,      //= 3,
    ESmumSendOptReplyViaSameCentreLBI   //= 4
    };


// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CSmumSettingsArray;
class CEikTextListBox;
class CAknTitlePane;

// CLASS DECLARATION

/**
*  SMUM's main sms settings dialog.
*/
NONSHARABLE_CLASS( CSmumMainSettingsDialogGSM ) : public CSmumMainSettingsDialog
    {
    public:  // Constructors and destructor
      
        /**
        * Two-phased constructor.
        */
        static CSmumMainSettingsDialog* NewL(
            CSmsSettings& aSettings,
            TInt aTypeOfSettings,
            TInt& aExitCode,
            TInt& aCharSetSupportForSendingOptions,
            CSmsHeader* aSmsHeader = NULL );
    
        /**
        * Destructor.
        */
        ~CSmumMainSettingsDialogGSM();

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

        /**
        * From CEikDialog
        */ 
	    void HandleResourceChange( TInt aType );

    private: // New functions
        enum TCSPSupportedSettings
            {
            // Support for validity period
            EValidityPeriodSupport  = 0x0001,
            // Support for Protocol ID "Message conversion"
            EProtocolIDSupport      = 0x0002,
            // Support for delivery repot
            EDeliveryReportSupport  = 0x0004,
            // Support for Reply Path "Reply via same centre"
            EReplyPathSupport      = 0x0008,
            };

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
        void ClosingDialogL();

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

        /** 
        * Returns the index of variated listbox selection
        * @since 2.6
        * @param aListSelection, listbox selection
        * @return TInt, variated index of selection
        */
        TInt GetVariatedSelectionIndex( 
            const TInt aListSelection ) const;
        
        /** 
        * Delete not supported settings according to CSP bits 
        * and Email over sms feature
        * @since 2.6
        */
        void DeleteVariatedSettings();

        /** 
        * Deletes one single setting item from settings arrays
        * @since 2.6
        * @param aIndex index of the setting in array that will be removed
        */
        void DeleteSettingItemFromArrays( TInt aIndex );
        
        /** 
        * Read CSP bits from sim
        * @since 2.6
        */
        void ReadCspSupportBitsL();
    
        /** 
        * Reads the setting value and sets it to member variable
        * @since 3.0
        * @return TInt, 0 for Full charset support, 1 for Reduced
        */
        TInt ReadCharSetSupport();
        
		/** 
        * Displays the configuration page for 
        * Email Over Sms settings(SMSC and Email Gateway)
        * @since 2.6
        */
        void DisplayEmailOverSmsSettingsL();
        
        /** 
        * Updates the Middle Softkey 
        * @since 3.1
        */        
        void UpdateMskL( );
       
        /** 
        * Does delayed exit after specified delay
        * @since S60 v5.0
        * @param aDelayTime Requested delay in microseconds
        */ 
        void DoDelayedExitL( const TInt aDelay );
        
        /** 
        * Used for exiting application properly
        * @since S60 v5.0
        * param aThis Pointer to this AppUi
        */         
        static TInt DelayedExit( TAny* aThis );

    private :

        /**
        * C++ default constructor.
        */
        CSmumMainSettingsDialogGSM(
            CSmsSettings& aSettings,
            TInt aTypeOfSettings,
            TInt& aExitCode,
            TInt& aCharSetSupportForSendingOptions,
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
        TBool                   iHelpFeatureSupported;
        CAknNavigationControlContainer* iSmumNaviPane; //not owned
        TUint                   iCSPFeatures;
        CArrayFixFlat<TInt>*    iSettingsArrayIndex;
        TInt&                   iCharacterSupport;
        CRepository*            iCentralRapository;
        CPeriodic*              iIdle; 
        TBool                   iUseSimSCsOnly;
        TBool                   iEmailFeatureSupported;
        TInt 					iRemoveReplyScSetting;
        TInt					iStoreSettingsIndex;
        TInt                    iMskId;
    };

#endif      // SMUMSETTINGSDIALOGGSM_H   

// End of File
