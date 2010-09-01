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
* Description:  IMSSettingsWizard.h
*
*/



#ifndef IMUMMAILSETTINGSWIZARD_H
#define IMUMMAILSETTINGSWIZARD_H

// INCLUDES
#include <e32base.h>
#include <e32cmn.h>
#include <coedef.h>                 // TKeyResponse
#include <w32std.h>                 // TKeyEvent
#include <msvapi.h>                 // CMsvSession
#include <mtudreg.h>                // CMtmUiDataRegistry
#include <muiuflagger.h>
#include <aknnotedialog.h>
#include <AknInfoPopupNoteController.h>
#include "MuiuDynamicSettingsDialog.h"

#include "IMSSettingsUi.h"
#include "IMSSettingsBaseUi.h"
#include "IMSWizardLauncher.h"

// CONSTANTS
const TInt KIMSSettingsPopupTimerNoteTextLength = 256;
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CImumMboxManager;
class CImumMboxData;
class CImSmtpSettings;
class CImImap4Settings;
class CImPop3Settings;
class CAknMessageQueryDialog;
class CIMSSettingsWizardEditTextPage;
class CIMSSettingsWizardRadioButtonPage;
class CIMSSettingsItemApprover;
class CMsvCommDbUtilities;
class CEikRichTextEditor;
class CMtmStore;
class CIMSWizardLauncher;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.0
*/
class CIMSSettingsWizard :
    public CIMSSettingsBaseUI
    {
    public: // Constructors and destructor

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CIMSSettingsWizard();

        /**
        *
        * @since S60 3.0
        * @param aListBox, The listbox with skin support
        */
        static CIMSSettingsWizard* NewL(
            CIMSSettingsUi& aDialog,
            CImumInternalApiImpl& aMailboxApi,
            CEikFormattedCellListBox& aListBox,
            CIMSWizardControl& iHelpContainer,
            CAknTitlePane& aTitlePane,
            CEikButtonGroupContainer& aSoftkeys,
            CMuiuFlags& aFlags,
            const TDesC& aEmailAddress );

        /**
        *
        * @since S60 3.0
        * @param aListBox, The listbox with skin support
        */
        static CIMSSettingsWizard* NewLC(
            CIMSSettingsUi& aDialog,
            CImumInternalApiImpl& aMailboxApi,
            CEikFormattedCellListBox& aListBox,
            CIMSWizardControl& iHelpContainer,
            CAknTitlePane& aTitlePane,
            CEikButtonGroupContainer& aSoftkeys,
            CMuiuFlags& aFlags,
            const TDesC& aEmailAddress );

    public: // New functions

        /**
        *
        * @since S60 3.0
        */
        void StoreSettingsToAccountL(
            CImumInSettingsData& aSettings );

        /**
        *
        * @since S60 3.0
        */
        TBool SettingsStartedL( const TDesC& aEmailAddress );

        /**
        *
        * @since S60 3.0
        */
        void PostponePopup();

		/**
		 * Starts the settings wizard.
		 *
		 * @since S60 3.0
		 */
        void WizRunStart();

		/**
		 * Runs one step of the wizard procedure and checks
		 * the internal state.
		 *
		 * @return TBool ETrue if there are still steps to go
		 *				 	   through.
		 * @since S60 3.0
		 */
        TBool WizRunStepL();


    public: // Functions from base classes

        /**
        *
        * @since S60 3.0
        * @return ETrue, if possible to exit
        * @return EFalse, if not possible to exit
        */
        TBool OkToExitL( const TInt aButtonId );


    private: // Constructors and destructor

        /**
        *
        * @since S60 3.0
        */
        CIMSSettingsWizard(
            CIMSSettingsUi& aDialog,
            CImumInternalApiImpl& aMailboxApi,
            CEikFormattedCellListBox& aListBox,
            CIMSWizardControl& aHelpContainer,
            CAknTitlePane& aTitlePane,
            CMuiuFlags& aFlags );

        /**
        *
        * @since S60 3.0
        */
        void ConstructL( CEikButtonGroupContainer& aSoftkeys,
            const TDesC& aEmailAddress );

    private:  // New virtual functions
    private:  // New functions

        // WIZARD STATES
        void WizStateMachineL();
        void WizStateChangeStateL();
        void WizHandleStateInitializeL();
        void WizHandlePageForward();
        void WizHandlePageBackward();
        void WizHandleStateDeterminePage();
        void WizHandleStateEditL();
        void WizHandleStateFinishEditingL();
        void WizHandleStateExitL();

        // WIZARD ITEM HANDLING
        TBool ProcessValidatedItemL(
            CMuiuSettingBase& aBaseItem );
        void SettingsItemProcessItemL(
            CMuiuSettingBase& aBaseItem );
        void SettingsItemProcessProtocol(
            CMuiuSettingBase& aBaseItem );
        void SettingsItemProcessEmailL(
            CMuiuSettingBase& aBaseItem );
        void SettingsItemProcessIapL(
            CMuiuSettingBase& aBaseItem );
        /**
        * Creates server address string from verifed mail address
        * @since S60 3.0
        */
        void SettingsItemDefineServers(
            const TDesC& aEmailAddress );
        void SettingsItemDefineNormalWizardArray();

        /**
        * Creates mailbox name string from verified mail address
        * @since S60 3.0
        */
        void SettingsItemDefineMailboxName(
            const TDesC& aEmailAddress );

        // CUSTOM SETTINGS
        CAknSettingPage* AcquireCustomRadioButtonPageL(
            TInt& aReturnSettingValue,
            TMuiuDynSetUpdateMode& aReturnUpdateMode,
            MDesCArray& aReturnRadioButtonArray,
            CMuiuSettingBase& aParamBaseItem );
        CAknSettingPage* AcquireCustomTextEditorL(
            TMuiuSettingsText& aReturnSettingText,
            TMuiuDynSetUpdateMode& aReturnUpdateMode,
            CMuiuSettingBase& aParamBaseItem );
        CAknSettingPage* AcquireCustomIapPageL(
            TInt& aReturnSettingValue,
            TMuiuDynSetUpdateMode& aReturnUpdateMode,
            CDesCArrayFlat& aReturnRadioButtonArray,
            CMuiuSettingBase& aParamBaseItem );
        CAknSettingPage* AcquireProtocolPageL(
            TInt& aReturnSettingValue,
            TMuiuDynSetUpdateMode& aReturnUpdateMode,
            CDesCArrayFlat& aReturnRadioButtonArray,
            CMuiuSettingBase& aParamBaseItem );


        /**
        *
        * @since S60 3.0
        */
        void OpenHelpPageL();
        void DefinePageNumberStringL();
        TBool CheckEmailSkip();


        // KEY PRESSES

        /**
        *
        * @since S60 3.0
        */
        void KeyPressNextL();
        void KeyPressBackL();

        // MISC

        /**
        *
        * @since S60 3.0
        */
        TBool CheckCompleted();

        /**
        *
        * @since S60 3.0
        */
        CAknInfoPopupNoteController* ShowMailSettingsPopupAfterL(
            const TUid& aId,
            TInt aAfterMs );

         /**
         * Appends the MTM to the protocol selection list if the MTM supports
         * the wizard integration and if its technology type is email.
         *
         * @since S60 v3.2
         * @param aUid               ID of the MTM to be checked
         * @param aRadioButtons      List of pairs containg MTM ID and the title to be shown
         *                           protocol selection list
         * @param aUiDataMtmRegistry MTM UI Data registry
         */
        void CheckMtmWizardSupportL(
            CBaseMtmUiData& aMtmUiData,
            CDesCArrayFlat& aRadioButtons );

    private:  // Functions from base classes

        virtual TMuiuPageEventResult EventItemEditStartsL(
            CMuiuSettingBase& aBaseItem );

        /**
        *
        * @since S60 3.0
        */
        virtual void AcquireCustomSettingPageL(
            CAknSettingPage*& aReturnPage,
            TMuiuDynSetUpdateMode& aReturnUpdateMode,
            TInt& aReturnSettingValue,
            TMuiuSettingsText& aReturnSettingText,
            CDesCArrayFlat& aReturnRadioButtonArray,
            CMuiuSettingBase& aParamBaseItem );

        // CIMSSettingsBaseUI
        /**
        *
        * @since S60 3.0
        */
        TBool KeyPressSoftkey( const TInt aButton );

        /**
        *
        * @since S60 3.0
        */
        TBool KeyPressOKButtonL( const TInt aButton );

        TInt EventItemEditEndsL(
            CMuiuSettingBase& aBaseItem );
        TMuiuPageEventResult EventItemEvaluateRadioButton(
            const CMuiuSettingBase& aBaseItem,
            TInt& aNewValue );
        TMuiuPageEventResult EventItemEvaluateText(
            const CMuiuSettingBase& aBaseItem,
            TDes& aNewText );

        /**
         * Calls external mtm wizard
         *
         * @since S60 v3.2
         * @param aMtmUid for searching rigth wizard
         */
		void CallExternalMtmWizardL( const TUid& aMtmUid, TBool& aCompleted );


    private:    // Datad

        enum TWizState
            {
            EWizStateStarted = 0,
            EWizStateDeterminePage,
            EWizStateEditPage,
            EWizFinishEditing,
            EWizStateExit,
            EWizStateQuerying
            };

        enum TWizFlags
            {
            EWizFlagFirstFlag = EImumSettingLastFlag,
            EWizFlagStateRunning = EWizFlagFirstFlag,
            EWizFlagExiting,
            EWizFlagCompleted,
            EWizFlagNextKeyPressed,
            EWizFlagBackKeyPressed,
            EWizFlagOptionsKeyPressed,
            EWizFlagItemApproved,
            EWizFlagRadioButtonPage,
            EWizFlagTextEditPage,
            EWizFlagSkipEmail,
            EWizFlagPluginDetected,
            EWizFlagForcedCancel,
            EWizFlagShouldExit,
            EWizFlagShouldClose,
            EWizFlagLastFlag
            };

        // The current item in progress
        TUid                    iCurrentItem;
        //
        TWizState               iWizState;
        //
        CIMSSettingsWizardEditTextPage* iPageEdit;
        //
        CIMSSettingsWizardRadioButtonPage* iPageRadioBtn;
        //
        TInt                    iCurPage;
        //
        HBufC*                  iPageText;
        //
        CIMSWizardControl&      iHelpContainer;
        //
        CAknInfoPopupNoteController* iNoteDialog;
        //
        TMuiuSettingsText       iTemporaryText;
        //Owned: To launch wizard automatically
        CIMSWizardLauncher*        iWizardLauncher;

        // To keep track of the protocol
        TUid            		iProtocol;
        //Owned
        CMtmStore*              iMtmStore;
        RArray<TInt>            iProtocolArray;
    };

#endif      // IMUMMAILSETTINGSWIZARD_H

// End of File
