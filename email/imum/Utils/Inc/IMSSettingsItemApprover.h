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
* Description:  IMSSettingsItemApprover.h
*
*/


#ifndef IMSSETTINGSITEMAPPROVER_H
#define IMSSETTINGSITEMAPPROVER_H

// INCLUDES
#include <e32base.h>
#include "MuiuDynamicSettingItemBase.h" // CMuiuSettingBase
#include <msvapi.h>                     // CMsvSession
#include <muiuflagger.h>                // CMuiuFlags
#include "MuiuDynamicSettingsDialog.h"  // TMuiuPageEventResult

#include "ComDbUtl.h"                   // CMsvCommDbUtilities
#include "IMSSettingsNoteUi.h"          // CIMSSettingsNoteUi
#include "IMSSettingsItemApprover.h"    // CIMSSettingsItemApprover
#include "IMSSettingsBaseUi.h"          // CIMSSettingsBaseUi

// CONSTANTS
enum TIMSApproverFlags
    {
    // Indicates that the item contains unprepared value
    EIMSApproverGenerate = 15
    };

enum TIMSApproverEvent
    {
    EIMSApproverNoEvent = 0,
    EIMSApproverRemoveAlwaysOnline,
    EIMSApproverRemoveOmaEmn
    };

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CImumInternalApi;
class CImumInSettingsData;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.0
*/
class CIMSSettingsItemApprover : public CBase
    {
    public: // Constructors and destructor

        /**
        * Destructor
        * @since S60
        */
        ~CIMSSettingsItemApprover();

        /**
        *
        * @since S60
        */
        static CIMSSettingsItemApprover* NewL(
            CImumInternalApi& aMailboxApi,
            CMsvCommDbUtilities& aCommsDb,
            CIMSSettingsNoteUi& aNoteUi,
            CIMSSettingsBaseUI& aDialog );

        /**
        *
        * @since S60
        */
        static CIMSSettingsItemApprover* NewLC(
            CImumInternalApi& aMailboxApi,
            CMsvCommDbUtilities& aCommsDb,
            CIMSSettingsNoteUi& aNoteUi,
            CIMSSettingsBaseUI& aDialog );

    public: // New functions

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult EvaluateText(
            TIMSApproverEvent& aEvent,
            const CMuiuSettingBase& aBaseItem,
            TDes& aNewText );

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult EvaluateValue(
            TIMSApproverEvent& aEvent,
            const CMuiuSettingBase& aBaseItem,
            TInt& aNewValue );

    public: // Functions from base classes

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private: // Constructors

        /**
        *
        * @since S60
        */
        CIMSSettingsItemApprover(
            CImumInternalApi& aMailboxApi,
            CMsvCommDbUtilities& aCommsDb,
            CIMSSettingsNoteUi& aNoteUi,
            CIMSSettingsBaseUI& aDialog );

        /**
        *
        * @since S60
        */
        void ConstructL();

    private:  // New virtual functions
    private:  // New functions

        // COMMON ITEM VALIDATION

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors ValidateType(
            const CMuiuSettingBase& aBaseItem,
            TDes& aNewText,
            TInt& aNewValue );

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors EvaluateWizardItems(
            TIMSApproverEvent& aEvent,
            const CMuiuSettingBase& aBaseItem,
            TDes& aNewText,
            TInt& aNewValue );

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors EvaluateSettingItems(
            TIMSApproverEvent& aEvent,
            const CMuiuSettingBase& aBaseItem,
            TDes& aNewText,
            TInt& aNewValue );

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult EvaluateItem(
            TIMSApproverEvent& aEvent,
            const CMuiuSettingBase& aBaseItem,
            TDes& aNewText,
            TInt& aNewValue );

        /**
        *
        * @since S60 3.0
        */
        TBool IsItemFilled(
            const CMuiuSettingBase& aBaseItem,
            const TInt aLength );

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors ValidateText(
            const CMuiuSettingBase& aBaseItem,
            const TDesC& aNewText );

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors ValidateValue(
            const CMuiuSettingBase& aBaseItem,
            TInt& aNewValue );

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors ValidateRadioButtons(
            const TInt aNewValue );

        // SPESIFIC ITEM VALIDATION

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors EvaluateAccountName(
            const CMuiuSettingBase& aBaseItem,
            TDes& aNewText );

        /**
        *
        * @since Series60 3.0
        */
        TIMSErrors EvaluateServerAddress(
            const CMuiuSettingBase& aBaseItem,
            const TDesC& aNewText );

        /**
        *
        * @since Series60 3.0
        */
        TIMSErrors EvaluateEmailAddress(
            const CMuiuSettingBase& aBaseItem,
            const TDesC& aNewText );

        /**
        *
        * @since Series60 3.0
        */
        TIMSErrors EvaluateInternetAccessPoint(
            const CMuiuSettingBase& aBaseItem,
            TInt& aNewValue );

        // ITEM VALIDATION UTILITIES

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors EvaluateUsernameOrPassword(
            TIMSApproverEvent& aEvent,
            TDes& aNewText );

        /**
         * Evaluates free text fields that have no special
         * restrictions.
         *
         * @since S60 5.0
         */
        TIMSErrors EvaluateFreeText( TDes& aNewText );

        // Always Online and OMA EMN checks

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors CheckItemOnlineSettingsCheckTurnOn(
            TIMSApproverEvent& aEvent,
            const TInt aNewValue,
            TBool aIsEmn );

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors CheckItemOnlineSettingsCheckTurnOnL(
            TIMSApproverEvent& aEvent,
            TBool aIsEmn );

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors CheckItemAoCountOkL();

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors CheckItemAoAccessPointOkL();

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors CheckItemAoOmaEmnOffL( TBool aIsEmn );

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors CheckItemAoUsernameAndPasswordL();

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors CheckItemAoConnectionTypeL();

        /**
        *
        * @since S60 3.0
        */
        TIMSErrors CheckItemAOFixFetchL();

    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        // Internal mail API implementation
        CImumInternalApi&   iMailboxApi;
        // CommsDb utils
        CMsvCommDbUtilities& iCommsDbUtils;
        // Class to handle notes in UI
        CIMSSettingsNoteUi& iNoteUi;
        // The ownerdialog item
        CIMSSettingsBaseUI& iDialog;
        //
        TMuiuSettingsText*  iTempText;
    };

#endif // IMSSETTINGSITEMAPPROVER_H

// End of File
