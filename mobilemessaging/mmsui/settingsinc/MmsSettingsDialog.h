/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
*           Implements MMS service and sending settings dialogs
*
*/



#ifndef MMSSETTINGSDIALOG_H
#define MMSSETTINGSDIALOG_H

//  INCLUDES

#include <AknForm.h>
#include <msvapi.h> //for MMsvSessionObserver
#include <eiklbo.h> // MEikListBoxObserver
#include <ConeResLoader.h>              // ResLoader

#include <mmsconst.h> // MmsMessageDeliveryReport

// CONSTANTS

const TUint KColumnListSpace = ' ';//For getting the line on the listbox
const TInt KMmsSettingsSendRetryCount = 3;
const TInt KMmsSettingsSendRetryInterval = 600;
const TInt KSettingsGranularity = 4;
const TInt KMmsUiLongestSettingLength = 100;

// MACROS

// DATA TYPES
typedef TBuf<KMmsUiLongestSettingLength> TMmsSettingsText;

// Enumerations for service settings listbox
enum
    {
    EMmsSettingsImageSize = 0,
    EMmsSettingsCreationMode,
    EMmsSettingsAccessPoint,
    EMmsSettingsReceivingMode,
    EMmsSettingsReceivingAnonymous,
    EMmsSettingsReceivingAds,
    EMmsSettingsReceiveReport,
// This value is used for denying read report, too. 
// CMmsSettingsDialog handles that their values go hand in hand ( both have either EFalse or ETrue )
// in CMmsSettings object.
    EMmsSettingsDenyDeliveryReport,
    EMmsSettingsValidityPeriod
    };
    
// MMS receiving - When enabled
enum
    {
    EMmsSettingsWhenEnabledFetch = 0,
    EMmsSettingsWhenEnabledPostpone,
    EMmsSettingsWhenEnabledReject
    };

// MMS receiving settings. Used both for in & outside home network
enum
    {
    EMmsSettingsReceivingAutomaticAlways = 0,
    EMmsSettingsReceivingAutomaticHome,
    EMmsSettingsReceivingManualAlways,
    EMmsSettingsReceivingDisabledAlways
    };

enum
    {
    EMmsSettingsYes = 0,
    EMmsSettingsNo
    };

// MMS read reports settings values
enum TMmsSettingsReceiveReportValues
    {
    EMmsSettingsReceiveReportBoth = 0,
    EMmsSettingsReceiveReportDelivery,
    EMmsSettingsReceiveReportNone,
    };
    
enum
    {
    EMmsSettingsValidityPeriod1h = 0,
    EMmsSettingsValidityPeriod6h,
    EMmsSettingsValidityPeriod24h,
    EMmsSettingsValidityPeriod3d,
    EMmsSettingsValidityPeriodWeek,
    EMmsSettingsValidityPeriodMaximum
    };

// MMS priority settings per mms
enum
	{
    	EMmsSettingsPriorityHigh = 0,
    	EMmsSettingsPriorityNormal,
    	EMmsSettingsPriorityLow
	};
	
// Image size options. Normally 2 options:
//  First:  large  (0)
//  Second: small  (1)
//
//  If the image is small:
//  First:  small  (0)
//  Second: <not_available>

enum
    {
    EMmsSettingsImageSmall = 0,
    EMmsSettingsImageLarge
    };

// Type of creation mode setting
enum TMmsUiSettingsCreationMode
    {
    EMmsSettingsCreationModeRestricted = 0,
    EMmsSettingsCreationModeGuided,
    EMmsSettingsCreationModeFree
    };

// FORWARD DECLARATIONS
class CClientMtmRegistry;
class CAknNavigationControlContainer;
class CAknTitlePane;
class CMmsSettings;
class CCommsDatabase;
class CApUtils;
class CEikTextListBox;
class CMuiuSettingsArray;

// CLASS DECLARATIONS

/**
 * Class CMmsSettingsDialog
 * Creates a dialog for editing mms settings. Values are from loc
 * files. The actual setting values come from MmsClientMtm and then
 * a corresponding value from localisation is shown to the user.
 *
 * Note: It is expected that the context is set to MMS service
 * object before calling!
 *
 * @since 0.9
 */

class CMmsSettingsDialog : public CAknDialog, public MEikListBoxObserver
    {
    public:
        enum TMmsExitCode
            {
            EMmsExit = 0,
            EMmsBack,
            EMmsExternalInterrupt,
            EMmsExitWithSave
            };
            
    public:
        /**
         * Constructor
         * @param aClientMtm - client mtm instance. Note: context must be set 
         *                     to MMS service & LoadMessageL() called!
         * @param aExitCode out - which command closed the dialog
         */
        CMmsSettingsDialog(
            CMmsSettings* aMmsSettings,
            TMmsExitCode& aExitCode );
            
        /**
        *
        */
        //void ConstructL();
            
        /**
         * Destructor
         */
        ~CMmsSettingsDialog();

    protected:
        
        /**
         * From CAknDialog
         */
        void ProcessCommandL( TInt aCommandId ) ;   
        
        /** 
        * From CEikDialog
        */
        TBool OkToExitL( TInt aKeyCode );

        /** 
         * From CEikDialog
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
         * From MEikListBoxObserver 
         */
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

    private:
        
        /** 
         * From CEikDialog
         * Sets the layout for the dialog. Called just before the dialog
         * is activated
         */
        void PreLayoutDynInitL();

        /**
        * From CEikDialog
        * Initializes items on the menu
        */
        void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);

        /**
         * Gets the values for the mms service settings and store to listbox.
         */
        void GetServiceSettingsValuesL();

        /**
         * Maps creation mode value from engine to SettingsDialog values
         */
        TInt32 GetCreationMode();

        /**
         * Sets one listbox item to settings array.
         * @param aIndex - index of the item
         * @param aCurrentNumber - value of the item
         */
        void SetArrayItem( TInt aIndex, TInt aCurrentNumber );

        /**
         * Sets one listbox item to settings array.
         * @param aIndex - index of the item
         * @param aCurrentNumber - value of the item
         * @param aUserText - formatted UI text
         */
        void SetArrayItem( TInt aIndex, TInt aCurrentNumber, const TDesC& aUserText );


        /**
         * Handles which settings is selected
         * @param aIndex - index of the item
         * @param aEnterPressed
         */
        void HandleEditRequestL( TInt aIndex, TBool aEnterPressed );

        /**
         * Edits the values of the mms receiving mode
         */
        void EditReceivingModeL();

        /**
         * Edits the values of the receiving anonymous messages setting
         * @param aEnterPressed
         */
        void EditReceivingAnonymousL( TBool aEnterPressed );

        /**
         * Edits the values of the receiving adds setting
         * @param aEnterPressed
         */
        void EditReceivingAdsL( TBool aEnterPressed );
        
        /**
         * Edits the values of the receiving reports setting
         */ 
        void EditReceiveReportsL( );

        /**
         * Store the receive report setting.
         * @param aReceiveReport - receive report setting
         */ 
        void SetReceiveReportSetting( TMmsSettingsReceiveReportValues aReceiveReport );
        
        /**
         * Edits the values of the sending delivery reports setting
         * @param aEnterPressed
         */ 
        void EditDenySendingDeliveryReportsL( TBool aEnterPressed );
        
        /**
         * Edits the values of the validity period setting
         */ 
        void EditValidityPeriodL();

        /**
         * Store the validity period setting .
         * @param aValidityPeriod validity period time
         */ 
        void SetValidityPeriodSetting( TInt32 aValidityPeriod );
       
        /**
         * Edits the values of the Access Point
         * @return ETrue if exit was requested from Access point dialog
         *         EFalse otherwise
         */
        TBool EditAccessPointL();

        /**
         * Gets a name for the access point
         * @param aId ID of the access point
         * @param aName caller allocated buffer for access point,
         *              size KCommsDbSvrMaxFieldLength
         */
        void GetNameForApL( TUint32 aId, TDes& aName );

        /**
         * Edits the values of the image size setting
         * @param aEnterPressed
         */ 
        void EditImageSizeL( TBool aEnterPressed );

        /**
         * Edits the values of the creation mode setting
         * @param aEnterPressed
         */ 
        void EditCreationModeL( TBool aEnterPressed );

        /**
         * Sets the title text
         */
        void SetTitleTextL();
        
        /**
         * Checks the access point as a part of closing dialog (back / exit).
         * @return ETrue if closing with errors i.e. AP does not exist
         *               and exit was selected at some point.
         *         EFalse if AP is ok.
         */
        TBool CheckAndCorrectAccessPointL();

        /**
         * Checks whether or not the access point exists
         * @param aUtils utility class 
         * @param aAP access point id
         * @return ETrue access point exists
         *         EFalse access point does not exist (got "not found" leave)
         */
        TBool ApExistsL( CApUtils* aUtils, TInt32 aAP ) const;
        
        /**
         * Finishing operations when closing the dialog.
         * @return ETrue if save is not wanted
         *         EFalse if OK to save settings
         */
        TBool ClosingDialogL();

        /**
         * Does the exit operation for all
         */
        void ExitDialogL();
 
        /**
         * Sets the selection item's ui side and return true if selected value is accepted.
         * @param TInt aCurrentItem The value which is chosen by the user.
         * @param TInt aList What list is to be read from the resource file.        
         * @param TInt aItem Which setting item is to changed
         * @return TBool ETrue if selected value is accepted
         *               EFalse otherwise.
         */
        TBool SetAndGetSettingItemL( TInt& aCurrentItem, TInt aList, TInt aItem );

        /**
         * Shows query "settings not ok. exit anyway)
         * @return ETrue if exit wanted, EFalse otherwise
         */
        TBool ShowExitAnywayQueryL() const;

        /**
         * From CCoeControl, returns help context
         */
        void GetHelpContext( TCoeHelpContext& aContext ) const; 
        
        /** 
         * Launches help application
         */
        void LaunchHelpL();

        /** 
         * Show information note
         */
        void ShowInformationNoteL( TInt aResourceId );

    private:

        enum TSettingsDialogFlags
            {
            EShouldExit                    = 0x0001,
            EGoingToExit                   = 0x0002,
            EExitSelected                  = 0x0004,
            ESwitchToExit                  = 0x0008,
            EHelpFeatureSupported          = 0x0010,
            ECSDEnabled                    = 0x0020,
            EDefaultNaviPane               = 0x0040,
            ECreationModeChangeEnabled     = 0x0080,
            EManualFetchFeatureSupported   = 0x0100,
            EReceivingOffDisabled          = 0x0200,
            EReceivingAlwaysManualDisabled = 0x0400,
            EReceivingAutomaticHomeDisabled= 0x0800
            };

        CAknNavigationControlContainer* iNaviPane; //not owned
        CMuiuSettingsArray*             iItems;
        //CMmsClientMtm*                  iMmsClient;
        CMmsSettings*                   iMmsSettings;
        HBufC*                          iPreviousTitleText;
        CAknTitlePane*                  iTitlePane;
        TMmsExitCode&                   iExitCode;  // pass exit code to caller
        CCommsDatabase*                 iCommsDb;
        CEikTextListBox*                iListbox;
        TInt                            iSettingsFlags;
    };

#endif    //MMSSETTINGSDIALOG_H
            
// End of File
