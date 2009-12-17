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


#ifndef __IMUMSETTINGSDIALOG_H__
#define __IMUMSETTINGSDIALOG_H__

//  INCLUDES
#include <bldvariant.hrh>
#include <AknForm.h>                // CAknDialog
#include <e32base.h>                // CArrayFixFlat
#include <muiuflagger.h>            // CMuiuFlags
#include <ConeResLoader.h>          // RConeResourceLoader

#include "ImumMboxData.h"

// DATA TYPES
enum TIMSFlags
    {
    // Indicates the settings state between wizard and normal setitngs
    EImumSettingsWizard = 0,
    // Indicates that the settings should be closed
    EImumSettingShouldClose,
    // Indicates if the settings and mce should be exitted
    EImumSettingShouldExit,
    // Indicates the need of query in exit
    EImumSettingQueryClose,
    // Indicates the need of opening settings menu
    EImumSettingOpenOptionsMenu,
    // Indicates if the submenu item is currently selected
    EImumSettingPositionSubMenu,
    // Indicates if the settings should be created/saved
    EImumSettingSave,
    // Keep this flag last
    EImumSettingLastFlag
    };

enum TImumUiExitCodes
    {
    // Nothing has changed, no need for save
    EImumUiNoChanges = 0,
    // After back key has been pressed, exit settings
    EImumUiClose,
    // After back key has been pressed, save and exit settings
    EImumUiSaveAndClose,
    // After options->exit has been chose, exit settings
    EImumUiExit,
    // After options->exit has been chose, save and exit settings
    EImumUiSaveAndExit,
    // External wizard created new mailbox
    EImumUiExtWizardCreated
    };

// FORWARD DECLARATIONS
class CIMSSettingsBaseUI;
class CEikFormattedCellListBox;
class CImumMboxManager;
class CAknTitlePane;
class CEikEdwin;
class CEikRichTextEditor;
class CImumInternalApiImpl;
class CImumInSettingsData;
class CAiwGenericParamList;
class CIMSWizardControl;

// CLASS DECLARATION

/**
*  CIMSSettingsUi
*/
class CIMSSettingsUi :
    public CAknDialog
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~CIMSSettingsUi();

    public: // New functions

        /**
        * Launches settings dialog for creating new account
        * @since S60 3.0
        * @param aSession: reference to the session object
        * @param aSettings: reference to the settings object
        * @return EFalse if no saving, ETrue otherwise
        */
        IMPORT_C static TMsvId NewAccountL(
            TImumUiExitCodes& aExitCode,
            CImumInSettingsData& aMailboxSettings,
            CMsvSession& aMsvSession );

        /**
        * Launches settings dialog for creating new account
        * @since S60 3.2
        * @param aSession: reference to the session object
        * @param aSettings: reference to the settings object
        * @param aDefaultWizOnly: if ETrue, no external wizard is
        * 					launched, even if one exists
        * @return EFalse if no saving, ETrue otherwise
        */
        IMPORT_C static TMsvId NewAccountL(
            TImumUiExitCodes& aExitCode,
            CImumInSettingsData& aMailboxSettings,
            CMsvSession& aMsvSession,
            TBool aDefaultWizOnly );

        /**
        * Launches settings dialog for editing existing account
        * @since S60 3.0
        * @param aSession: reference to the session object
        * @param aSettings: reference to the settings object
        * @param aId: TMsvId of the edited settings
        * @return EFalse if no saving, ETrue otherwise
        */
        IMPORT_C static void EditAccountL(
            TImumUiExitCodes& aExitCode,
            CImumInSettingsData& aMailboxSettings,
            CMsvSession& aMsvSession );

        /**
        *
        * @since S60 3.0
        */
        void LaunchHelp();

        /**
        *
        * @since S60 3.0
        */
        void DoQuitL();

        /**
        * Execute commands as requested by a flags
        * @since S60 3.2
        */
        void ExecFlags();

    public: // Functions from base classes

        //From CAknDialog
        /**
        * ProcessCommandL
        * @since S60 3.0
        * @param aCommandId, command id
        */
        void ProcessCommandL( TInt aCommandId );

        /**
        * OfferKeyEventL
        * Used here to handle exit event from subsettings
        * @since S60 3.0
        * @param aKeyEvent, event id
        * @param aType, event type
        * @return Response
        */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

        void HandleCommandL( TInt aCommand );

        /**
        *
        * @since S60 3.1
        */
    	virtual void HandleResourceChange( TInt aType );

    protected:  // New functions
    protected: // Functions from base classes

        //From CEikDialog

        /**
        * OkToExitL
        * @since S60 3.0
        * @param aButtonId, button id
        * @return if exit
        */
        virtual TBool OkToExitL( TInt aButtonId );

        /**
        * PreLayoutDynInitL
        */
        virtual void PreLayoutDynInitL();

        /**
        * DynInitMenuPaneL
        * @since S60 3.0
        * @param aResourceId
        * @param aMenuPane
        * @return command id
        */
        virtual void DynInitMenuPaneL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane );

        /**
        *
        * @since S60 3.0
        */
        virtual void GetHelpContext(
            TCoeHelpContext& aContext ) const;


         /**
         * Sets Settings ui's size and position
         * From CAknDialog
         * @since S60 3.2
         */
        virtual void SetSizeAndPosition( const TSize& aSize );


         /**
         * Method to create help screen control
         * From CEikDialog
         * @since S60 3.2
         */
        virtual SEikControlInfo CreateCustomControlL( TInt aControlType );

    private: // Constructors

        /**
        * C++ default constructor.
        * @since S60 3.0
        * @param aSession, session reference
        * @param aSettings, mail settings reference
        * @param aId, entry id
        * @param aEmailAddress, emailaddress from plugin
        * when user browses in settings
        */
        CIMSSettingsUi(
            TImumUiExitCodes& aExitCode,
            CImumInSettingsData& aMailboxSettings );

        /**
        *
        * @since S60 3.0
        */
        void SettingsConstructL(
            const TInt aResource,
            CMsvSession& aMsvSession,
            const TDesC& aEmailAddress );

    private: // New functions

        /**
        * Check to see if we are editing an existing
        * or a new account
        * @since S60 3.0
        * @return ETrue if editing, EFalse if creating new
        */
        inline TBool EditingAnAccount() const;

        /**
        * Change font color in startup and when skin is changed.
        * @since S60 3.1
        */
        void SetFontAndSkin();

        /**
         * Determines the exitcode
         *
         * @since S60 v3.2
         * @return Exit code based on the current status.
         */
        TImumUiExitCodes ExitCode();

        /**
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        static void StartL(
            TInt aResource,
            TImumUiExitCodes& aExitCode,
            CImumInSettingsData& aMailboxSettings,
            CMsvSession& aMsvSession,
            const TDesC& aEmailAddress );

        /**
         * Tries to launch external wizard via AIW
         *
         * @since S60 v3.2
         * @param aEmailAddress in which the email address is set if get
         * @param aMailboxId is set if external wizard creates mailbox
         * @leave KErrNotFound if not found approrpiate email wizard -plugin
         * @leave KErrCancel if operation was cancelled in external wizard
         */
        static void LaunchExtWizardL(
            TDes& aEmailAddress,
            TMsvId& aMailboxId );

        /**
         * Loads icon array to settings listbox object.
         *
         * @since S60 v3.2
         */
        void LoadListboxIconArrayL();

    private:    // Data

        // Flags of settings
        CMuiuFlags*             iFlags;
        // Exitcode to be returned to MTM
        TImumUiExitCodes&       iExitCode;
        // The settings manager that handles account creation
        CImumInSettingsData&    iMailboxSettings;
        //
        CImumInternalApiImpl*   iMailboxApi;
        // The listbox with skin support
        CEikFormattedCellListBox* iListBox;
        //
        CIMSSettingsBaseUI*     iSettings;
        //
        TInt                    iResourceMenuBar;
        // Object to load the resources
        RConeResourceLoader     iResourceLoader;
        // Descriptor to hold email address received from external plugin
        HBufC*                  iEmailAddress;

        // Help screen container, NOT OWNED
        CIMSWizardControl*      iWizardContainer;
    };

#endif      // __IMUMSETTINGSDIALOG_H__

// End of File
