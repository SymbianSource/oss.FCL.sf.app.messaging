/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CUniSendingSettingsDialog class definition.      
*
*/



#ifndef UNISENDINGSETTINGSDIALOG_H
#define UNISENDINGSETTINGSDIALOG_H

//  INCLUDES

#include <AknForm.h>
#include <msvapi.h> //for MMsvSessionObserver
#include <eiklbo.h> // MEikListBoxObserver
#include <ConeResLoader.h>              // ResLoader

#include <mmsconst.h> // MmsMessageDeliveryReport
#include <muiusettingsarray.h>

#include "UniSendingSettings.h"

// CONSTANTS

const TUint KColumnListSpace = ' ';//For getting the line on the listbox
const TInt KUniSettingsSendRetryCount = 3;
const TInt KUniSettingsSendRetryInterval = 600;
const TInt KSettingsGranularity = 4;
const TInt KUniUiLongestSettingLength = 100;

// MACROS

// DATA TYPES
typedef TBuf<KUniUiLongestSettingLength> TUniSettingsText;

// Enumerations for sending (=message) settings listbox
enum
    {
    EUniSendingSettingsMessageType = 0,
    EUniSendingSettingsCharSupport
    };

enum
    {
    EUniSettingsPriorityHigh = 0,
    EUniSettingsPriorityNormal,
    EUniSettingsPriorityLow
    };

enum
    {
    EUniSettingsYes = 0,
    EUniSettingsNo
    };

enum
    {
    EUniSettingsCharSupportFull = 0,
    EUniSettingsCharSupportReduced
    };

enum
    {
    EUniSettingsMessageTypeAutomatic = 0,
    EUniSettingsMessageTypeText,
    EUniSettingsMessageTypeMultimedia,
    EUniSettingsMessageTypeFax,
    EUniSettingsMessageTypePaging
    };

enum
    {
    EUniSettingsValidityPeriod1h = 0,
    EUniSettingsValidityPeriod6h,
    EUniSettingsValidityPeriod24h,
    EUniSettingsValidityPeriod3d,
    EUniSettingsValidityPeriodWeek,
    EUniSettingsValidityPeriodMaximum
    };

enum
    {
    EUniSettingsContentNeedsMms       = 0x01, // Show only MMS + Auto
    EUniSettingsPermanentSms          = 0x02, // Show only SMS + conversions
    EUniSettingsPermanentMms          = 0x04, // Show only MMS
    EUniSettingsCSPBitsOn             = 0x08, // Hide also conversions
    EUniSettingsHideCharSet           = 0x10, // Hide whole char set support
	EUniSettingsHideMessageTypeOption = 0x20  // Hide message type option 
    };

// FORWARD DECLARATIONS
class CAknNavigationControlContainer;
class CAknTitlePane;
class CEikTextListBox;

// CLASS DECLARATIONS

/**
 * Class CUniSettingsDialog
 * Creates a dialog for editing uni sending settings. Values are from loc
 * files. The actual setting values come from TUniSendingSettings.
 * @since 3.1
 */

class CUniSendingSettingsDialog : public CAknDialog, 
                                  public MEikListBoxObserver
    {
    public:
        enum TUniExitCode
            {
            EUniExit = 0,
            EUniBack,
            EUniExternalInterrupt,
            EUniExitWithSave
            };

    public:
        /**
         * Constructor
         * @param aClientMtm - client mtm instance. Note: context must be set 
         *                     to Uni service & LoadMessageL() called!
         * @param aExitCode out - which command closed the dialog
         */
        CUniSendingSettingsDialog(
            TInt aSettingsFlags,
            TUniSendingSettings& aSendingSettings,
            TUniExitCode& aExitCode,
            CAknTitlePane& aTitlePane );
            
        /**
         * Destructor
         */
        ~CUniSendingSettingsDialog();

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
        
        /**
        * From MEikMenuObserver Initializes items on the menu
        *
        * @param aMenuId    Current menu's resource Id
        * @param aMenuPane  Pointer to menupane
        */
        void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);
        
    private:
        
        /** 
         * From CEikDialog
         * Sets the layout for the dialog. Called just before the dialog
         * is activated
         */
        void PreLayoutDynInitL();
        
        /**
         * Gets the values for the uni message specific settings and store
         * them to listbox.
         */
        void GetSendingSettingsValues();

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
         * Edits the values of the 
         */
        void EditMessagePriorityL();

        /**
         * Sets the values of the 
         */
        void SetMessagePriorityL( TInt aPriority );

        /**
         * Edits the values of the 
         */
        void EditDeliveryReportL( TBool aEnterPressed );

        /**
         * Edits the values of the 
         */
        void EditMessageTypeL();

        /**
         * Sets the values of the 
         */
        void SetMessageTypeL( TInt aMessageType );

        /**
         * Edits the values of the 
         */
        void EditCharacterSupportL( TBool aEnterPressed );

        /**
         * Sets the values of the 
         */
        void SetCharacterSupportL( TInt aCharSetSupport );

        /**
         * Edits the values of the 
         */
        void EditValidityPeriodL();

        /**
         * Edits the values of the 
         */
        void SetValidityPeriodL( TInt aMessageValidity );

        /**
         * Edits the values of the 
         */
        void EditReplyViaSameCentreL( TBool aEnterPressed );

        /**
         * Sets the title text
         */
        void SetTitleTextL();
        
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
        
        /** 
         * Checks which items are currently invisible and
         * recalculates the index according to that
         */
        TInt OriginalIndexToVisibleIndex( TInt aOriginalIndex );

        /** 
         * Checks which items are currently invisible and
         * recalculates the index according to that
         */
        TInt VisibleIndexToOriginalIndex( TInt aVisibleIndex );

        TInt InvisibleItems( TInt aIndex );

        void UpdateMskL();
        
        /**
         * Executes settings page dialog 
         */
        TBool ExecuteSettingsPageDialogL( CDesCArrayFlat* items, 
                                          TInt& aCurrentItem,
                                          const TDesC& aLabelText );

    private:

        enum TSettingsDialogFlags
            {
            EShouldExit                    = 0x0001,
            EGoingToExit                   = 0x0002,
            EExitSelected                  = 0x0004,
            ESwitchToExit                  = 0x0008,
            EHelpFeatureSupported          = 0x0010,
            EDefaultNaviPane               = 0x0040
            };

        CAknNavigationControlContainer* iNaviPane; //not owned
        CMuiuSettingsArray*             iItems;
        HBufC*                          iPreviousTitleText;
        CAknTitlePane&                  iTitlePane;
        TUniExitCode&                   iExitCode;  // pass exit code to caller
        TUniSendingSettings&            iSendingSettings;
        CEikTextListBox*                iListbox;
        TInt                            iSettingsDialogFlags;
        TInt                            iSettingsFlags;
        TMuiuSettingsItem               iCharacterSettings;
        TMuiuSettingsItem               iMessageTypeSettings;
    };

#endif    //UNISENDINGSETTINGSDIALOG_H
            
// End of File
