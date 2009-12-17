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
* Description:  IMSSettingsBaseUI.h
*
*/



#ifndef IMSSETTINGSBASEUI_H
#define IMSSETTINGSBASEUI_H

// INCLUDES
#include <e32base.h>
#include <coedef.h>                 // TKeyResponse
#include <w32std.h>                 // TKeyEvent
#include <msvapi.h>                 // CMsvSession
#include <StringLoader.h>           // StringLoader
#include "MuiuDynamicSettingsDialog.h"

#include "IMSSettingsItemAccessPoint.h" // CIMSSettingsAccessPointItem
#include "ImumMboxManager.h"     // CImumMboxManager
#include "IMASPageIds.hrh"           // TIMASettigsId
#include "IMSSettingsUi.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CIMSSettingsItemApprover;
class CIMSSettingsNoteUi;
class CImumMboxDefaultData;

// CLASS DECLARATION

/**
*
*  @lib
*  @since S60 3.0
*/
class CIMSSettingsBaseUI :
    public CMuiuDynamicSettingsDialog
    {
    public:  // Constructors and destructor

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CIMSSettingsBaseUI();

    public:  // New virtual functions

        /**
        *
        * @since S60 3.0
        */
        virtual CMuiuSettingBase* GetUninitializedItem(
            const TUid& aId );

        /**
        *
        * @since S60 3.0
        */
        virtual void GetHelpContext( TCoeHelpContext& aContext ) const;

    public: // New functions

        /**
        *
        * @since S60 3.0
        */
        void DoQuitL();

        /**
        *
        * @since S60 3.0
        */
        TBool Flag( TUint aFlag );

        /**
        *
        * @since S60 3.0
        */
        void SetFlag( TUint aFlag );

        /**
        *
        * @since S60 3.0
        */
        void ClearFlag( TUint aFlag );

        /**
        *
        * @since S60 3.0
        */
        void ChangeFlag(
            TUint aFlag,
            TBool aNewState );

        /**
        *
        * @since S60 3.0
        */
        const CMuiuFlags& Flags();

        /**
        *
        * @since S60 3.0
        */
        TInt FindItem(
            CIMSSettingsAccessPointItem*& aItem,
            const TUid& aId );

        /**
        *
        * @since S60 3.0
        */
        TInt FindItem(
            CMuiuSettingsLinkExtended*& aItem,
            const TUid& aId );

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingBase* CreateItemForBackupL(
            CMuiuSettingBase& aBaseItem );

        /**
         * Returns the id of the editable mailbox
         *
         * @since S60 v3.2
         * @return Mailbox Id
         */
        virtual TMsvId CurrentMailboxId();

    public: // Functions from base classes

        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

        void ProcessCommandL( TInt aCommandId );

        virtual TBool OkToExitL( const TInt aButtonId ) = 0;

        /**
        * Moves data from the UI to settings data object
        * @since S60 3.0
        */
        virtual void StoreSettingsToAccountL(
            CImumInSettingsData& aSettings ) = 0;

    protected:  // Constructors

        /**
        * Default constructor for classCIMSSettingsBaseUI
        * @since S60 3.0
        * @return, Constructed object
        */
        CIMSSettingsBaseUI(
            CIMSSettingsUi& aDialog,
            CImumInternalApiImpl& aEmailApi,
            CEikFormattedCellListBox& aListBox,
            CAknTitlePane& aTitlePane,
            CMuiuFlags& aFlags );

        /**
        * Symbian 2-phase constructor
        * @since S60 3.0
        */
        void BaseUiConstructL(
            CEikButtonGroupContainer& aSoftkeys,
            const TInt aSettingResourceId = NULL,
            const TBool aUpdateMSK = EFalse );

    protected:  // New virtual functions

        virtual TBool KeyPressSoftkey( const TInt aButton ) = 0;
        virtual TBool KeyPressOKButtonL( const TInt aButton ) = 0;

        /**
        *
        * @since S60 3.0
        */
        virtual CMuiuSettingBase* CreateCustomItemToArrayLC(
            const TUid& aId );

    protected:  // New functions

        /**
        *
        * @since S60 3.0
        */
        inline TUid ToUid( const TUint32 aId ) const;


        /**
        *
        * @since S60 3.0
        */
        inline TInt SafeStringLoad( const TInt aResource,
            					    HBufC*& aString ) const;

        /**
        *
        * @since S60 3.0
        */
        void HidePermanently( CMuiuSettingBase& aBase,
            				  const TBool aNewState );

        /**
        *
        * @since S60 3.0
        */
        void HidePermanently( const TUid& aId,
            				  const TBool aNewState );

        /**
        *
        * @since S60 3.0
        */
        void SetItemIapItemL(
            CIMSSettingsAccessPointItem& aIapItem );

        /**
        *
        * @since S60 3.0
        */
        void UpdateAccessPointItem( CMuiuSettingBase& aBaseItem );

        /**
        *
        * @since S60 3.0
        */
        void SetReadOnly(
            const TUid& aId,
            const TBool aNewState );

    protected:  // Functions from base classes

    private:  // New virtual functions

        virtual TBool EventHandlerL( const TInt aCommandId );

    private:  // New functions

        /**
        * Creates default data object and load the data from CenRep
        * @since S60 3.0
        * @return Default data -object
        */
        CImumMboxDefaultData* RestoreDefaultSettingsDataL();

    private:  // Functions from base classes

        // CMuiuDynamicSettingsDialog
        void HandleSettingPageEventL(
            CAknSettingPage* aSettingPage,
            TAknSettingPageEvent aEventType );

    public:     // Data
    protected:  // Data

        // Flags for the wizard
        CMuiuFlags&             iBaseUiFlags;
        // Dialog that owns the settings
        CIMSSettingsUi&         iDialog;
        // Reference to mailbox api
        CImumInternalApiImpl&       iMailboxApi;
        // Pointer to class that validates given item
        CIMSSettingsItemApprover* iItemApprover;
        // Class to handle notes in UI
        CIMSSettingsNoteUi*     iNoteUi;
        // Class to contain default settings data
        CImumMboxDefaultData*       iDefaultData;

    private:    // Data
    };

#include "IMSSettingsBaseUi.inl"

#endif      // IMSSETTINGSBASEUI_H

// End of File
