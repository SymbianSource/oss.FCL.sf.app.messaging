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
* Description:  IMSSettingsDialog.h
*
*/



#ifndef IMUMMAILSETTINGSDIALOG_H
#define IMUMMAILSETTINGSDIALOG_H

// INCLUDES
#include <e32base.h>
#include "MuiuDynamicSettingsDialog.h"
#include <msvapi.h>                    // CMsvSession
#include <imapset.h>                   // CImImap4Settings
#include "MuiuDynamicSettingItemBase.h"      // CMuiuSettingBase
#include "MuiuDynamicSettingItemEditValue.h" // CMuiuSettingEditValue
#include "MuiuDynamicSettingItemEditText.h"  // CMuiuSettingEditText
#include "MuiuDynamicSettingItemLink.h"      // CMuiuSettingLink
#include "MuiuDynamicSettingItemExtLink.h"   // CMuiuSettingExtLink

#include "IMSSettingsItemAccessPoint.h"
#include "IMASPageIds.hrh"              // TIMASettigsId
#include "IMSSettingsUi.h"
#include "IMSSettingsBaseUi.h"
#include "IMSSettingsItemApprover.h"
#include "EmailUtils.H"

// CONSTANTS
enum TDialogFlags
    {
    // Flags to keep track, which of the resources have initialized
    EDialogMainMenuInitialized = EImumSettingLastFlag,
    EDialogIncomingInitialized,
    EDialogOutgoingInitialized,
    EDialogUserPrefInitialized,
    EDialogRetLimitInitialized,
    EDialogDownloadInitialized,
    EDialogAlOnlineInitialized,
    // Flags to keep track, which of the resources have changed
    EDialogIncomingChanged,
    EDialogOutgoingChanged,
    EDialogUserPrefChanged,
    EDialogRetLimitChanged,
    EDialogDownloadChanged,
    EDialogAlOnlineChanged,
    EDialogMainMenu,
    EDialogAlwaysOnlineOn,
    EDialogOmaEmnOn,
    EDialogCsdAccessPoint,
    EDialogQuitNotAllowed,
    EDialogIncomingIapNotOk,
    EDialogOutgoingIapNotOk,
    EDialogLastFlag
    };

const TInt KIMASDialogAOArrayIndex = 3;

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CFlags;
class CIMSSettingsItemApprover;
class CMsvCommDbUtilities;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.0
*/
class CIMSSettingsDialog :
    public CIMSSettingsBaseUI
    {
    public: // Constructors and destructor

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CIMSSettingsDialog();

        /**
        *
        * @since S60 3.0
        * @param aListBox, The listbox to be used to show the array
        */
        static CIMSSettingsDialog* NewL(
            CIMSSettingsUi& aDialog,
            CImumInternalApiImpl& aMailboxApi,
            CEikFormattedCellListBox& aListBox,
            CAknTitlePane& aTitlePane,
            CEikButtonGroupContainer& aButtons,
            CMuiuFlags& aFlags,
            CImumInSettingsData& aMailboxSettings );

        /**
        *
        * @since S60 3.0
        * @param aListBox, The listbox to be used to show the array
        */
        static CIMSSettingsDialog* NewLC(
            CIMSSettingsUi& aDialog,
            CImumInternalApiImpl& aMailboxApi,
            CEikFormattedCellListBox& aListBox,
            CAknTitlePane& aTitlePane,
            CEikButtonGroupContainer& aButtons,
            CMuiuFlags& aFlags,
            CImumInSettingsData& aMailboxSettings );

    public: // New functions

        /**
        *
        * @since S60 3.0
        */
        void StoreSettingsToAccountL(
            CImumInSettingsData& aSettings );

    public: // Functions from base classes

        /**
        *
        * @since S60
        * @return ETrue, if possible to exit
        * @return EFalse, if not possible to exit
        */
        TBool OkToExitL( const TInt aButtonId );

        /**
        * Seeks the item from the array and initializes it.
        * Ownership is not transferred.
        * @since S60 3.0
        */
        CMuiuSettingBase* GetUninitializedItem(
            const TUid& aId );

        /**
        *
        * @since S60 3.0
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private: // Constructors and destructor

        /**
        *
        * @since S60 3.0
        */
        CIMSSettingsDialog(
            CIMSSettingsUi& aDialog,
            CImumInternalApiImpl& aMailboxApi,
            CEikFormattedCellListBox& aListBox,
            CAknTitlePane& aTitlePane,
            CMuiuFlags& aFlags );

        /**
        *
        * @since S60 3.0
        */
        void ConstructL(
            CEikButtonGroupContainer& aSoftkeys,
            CImumInSettingsData& aMailboxSettings );

    private:  // New virtual functions
    private:  // New functions

        // ARRAY INITIALIZATION

        /**
        *
        * @since S60 3.0
        */
        void InitializeArrayL( const TInt aResource );

        /**
        *
        * @since S60 3.0
        */
        void InitializeNewResourceL();
        void InitializeArrayMainMenu();
        void InitializeArrayIncomingL();
        void InitializeArrayIncomingImap4L();
        void InitializeArrayIncomingPop3L();
        void InitializeArrayOutgoingL();
        void InitializeArrayUserPrefL();
        void InitializeArrayRetLimitL();
        void InitializeArrayDownloadL();
        void InitializeArrayAlwaysOnlineL();

        /**
        *
        * @since S60 3.0
        */
        void InitializeRetrieveLimitL(
            const TInt aLimit,
            const TInt aId );

        /**
        *
        * @since S60 3.0
        */
        TInt InitializeIap( const TUid& aIapItemId );

        /**
        *
        * @since S60 3.0
        */
        void InitializeAlwaysOnlineState();


        /**
        *
        * @since S60 3.0
        */
        void InitializeUsername(
            const TUid& aItemId,
            const TPtrC8& aUsername );

        /**
        *
        * @since S60 3.0
        */
        void InitializePassword(
            const TUid& aItemId,
            const TPtrC8& aPassword,
            const TInt aFlag );

        /**
        *
        * @since S60 3.0
        */
        TInt InitializeSignature();

        /**
        *
        * @since S60 3.0
        */
        void InitializeSignatureL();

        /**
        *
        * @since S60 3.0
        */
        void InitializeSecuritySettings(
            const TUid& aSecurityItemId );

        /**
        *
        * @since S60 3.0
        */
        TIMASMailSecurity InitializeSecurity( const TUid& aSecurityItemId );

        /**
        *
        * @since S60 3.0
        */
        TInt InitializeRetrievedParts();
        void InitializeRetrievedPartsL();

        /**
        *
        * @since S60 3.0
        */
        TInt InitializePort(
            const TIMASMailSecurity aSecurity,
            const TBool aIncoming  );

        /**
        *
        * @since S60 3.0
        */
        void InitializePortL(
            const TIMASMailSecurity aSecurity,
            const TBool aIncoming  );

        /**
        *
        * @since S60 3.0
        */
        TInt InitializeAoDays();

        /**
        *
        * @since S60 3.0
        */
        void InitializeAoIntervalButtons(
            TInt& aInterval );

        /**
        *
        * @since S60 3.0
        */
        void InitializeAoIntervalEditor(
            TInt& aInterval,
            HBufC*& aText );

        /**
        *
        * @since S60 3.0
        */
        void InitializeAoInterval();

        /**
        *
        * @since S60 3.0
        */
        TInt InitializeFolderPath();

        /**
        *
        * @since S60 3.0
        */
        TInt InitializeAoHoursL();

        void InitializeNext(
            const TUid& aItemId,
            const TMuiuSettingsText& aText );
        void InitializeNext(
            const TUid& aItemId,
            const TPtrC8& aText );
        void InitializeNext(
            const TUid& aItemId,
            const TInt aValue );
        void InitializeNext(
            const TUid& aItemId,
            const TInt aValue,
            const TMuiuSettingsText& aText );

        /**
        *
        * @since S60 3.0
        */
        void InitializeResetArrays();

        void InitializeItem(
            CMuiuDynFinderItemArray& aArray,
            const TInt aIndex,
            const TInt aValue,
            const TMuiuSettingsText* aText );
        void InitializeItem(
            CMuiuSettingBase& aBaseItem,
            const TInt aValue,
            const TMuiuSettingsText* aText );

        // SETTING STORING

        /**
        *
        * @since S60 3.0
        */
        void StoreSettingsL();
        void StoreSettingsIncomingL();
        void StoreSettingsIncomingImap4L();
        void StoreSettingsIncomingPop3L();
        void StoreSettingsOutgoingL();
        void StoreSettingsUserPrefL();
        void StoreSettingsRetLimitL();
        void StoreSettingsDownloadL();
        void StoreSettingsAlwaysOnlineL();
        void StoreSettingsIapL( const TBool aIsIncoming );
        void StoreSettingsSecurityL( const TBool aIsIncoming );
        void StoreSettingsRetrievedPartsImap4L(
            const TImap4GetMailOptions aOptions,
            const TImImap4PartialMailOptions aPartial = ENoSizeLimits );
        /**
        *
        * @since S60 3.0
        */
        void StoreRetrievedValuesImap4L( const TInt32 aSize = KMaxTInt );

        /**
        *
        * @since S60 3.0
        */
        void StoreRetrievedPartsL();

        /**
        *
        * @since S60 3.0
        */
        void StoreSettingsSignature();

        /**
        *
        * @since S60 3.0
        */
        const TBuf8<KMuiuDynMaxSettingsTextLength> StoreUsername(
            const TDesC& aUsername );

        /**
        *
        * @since S60 3.0
        */
        const TBuf8<KMuiuDynMaxSettingsTextLength> StorePassword(
            const TDesC& aPassword,
            const TInt aFlag );


        /**
        *
        * @since S60 3.0
        */
        void StoreSignatureL();

        /**
        *
        * @since S60 3.0
        */
        void StorePortL( const TBool aIncoming );

        /**
        *
        * @since S60 3.0
        */
        void StoreAoHoursL();

        /**
        *
        * @since S60 3.0
        */
        void StoreAoInterval();

        /**
        *
        * @since S60 3.0
        */
        void StoreFolderPathL();

        const TMuiuSettingsText* StoreGetNextText(
            const TUid& aItemId );
        TInt StoreGetNextValue(
            const TUid& aItemId );
        TBool StoreGetNextBoolean(
            const TUid& aItemId );

        /**
        *
        * @since S60 3.2
        */
        void StoreGetLoginInformation( TDes8& aUserName,
            TDes8& aPassword );
        void StoreSetLoginInformation( const TDesC8& aUserName,
            const TDesC8& aPassword );

        /**
        *
        * @since S60 3.0
        */
        void HandleSubMenuOpenL(
            CMuiuSettingBase& aBaseItem );

        // ITEM CHECKS

        /**
        *
        * @since S60 3.0
        */
        void EventHandleApproverEvent(
            const TIMSApproverEvent aEvent );

        // MISC FUNCTIONS

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingBase* NextItemSearch(
            const TUid& aExpectedId );

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingBase* NextItemDoSearch(
            const TInt aLastItem,
            const TUid& aExpectedId );

        /**
        *
        * @since S60 3.0
        */
        void SetEmailSettingsState();

        /**
        *
        * @since S60 3.0
        */
        void SubscribeFoldersL();

        /**
        *
        * @since S60 3.0
        */
        void CheckSubscribedFolderStateL();

        /**
        *
        * @since S60 3.0
        */
        void LaunchIapPageL(
            CMuiuSettingBase& aBaseItem );

        /**
        *
        * @since S60 3.0
        */
        void SetStoreFlags(
            const TBool aState,
            const TInt aFlag );

        /**
        *
        * @since S60 3.0
        */
        TInt GetStorerFlag(
            const CMuiuSettingBase& aBaseItem );

        /**
        *
        * @since S60 3.0
        */
        void SetFlags(
            const TInt aError,
            const TInt aFlag );

        /**
        *
        * @since S60 3.0
        */
        void SetInitializeFlags( const TInt aError );

        /**
        * ?member_description
        * @since S60 3.0
        * @param aBaseItem Item containing the information
        */
        void UpdateItemDefaultSettingTextL(
            CMuiuSettingsLinkExtended& aLinkItem ) const;

        /**
        * ?member_description
        * @since S60 3.0
        * @param aBaseItem Item containing the information
        */
        void UpdateItemCustomSettingTextL(
            CMuiuSettingsLinkExtended& aLinkItem,
            CMuiuSettingsEditValue& aItemFrom,
            CMuiuSettingsEditValue& aItemTo ) const;

        /**
        * Checks whether incoming/outgoing port is user defined or not and
        * sets the port number according to it and security setting. If user
        * defined then does not edit it.
        * @since S60 3.0
        * @param aBaseItemSecurity Item containing the information
        * @param aIncoming Incoming or outgoing
        */
        void CheckPort(
            CMuiuSettingBase& aBaseItemSecurity,
            const TBool aIncoming );

        /**
        * Checks port, updates item text and sets setting dialog resource
        * back to normal.
        * @since S60 3.1
        * @param aBaseItem Item containing the information
        */
        void ClosePortDialog(
            CMuiuSettingBase& aBaseItem );

        /**
        * Checks and sets EDialogCsdAccessPoint flag according to IAP
        * @since S60 3.0
        */
        void UpdateCSDFlagL();

        /**
        * Updates AO intervals, based on EDialogCsdAccessPoint flag
        * @since S60 3.0
        */
        void UpdateAOIntervalsL();

        /**
        * Gets AO interval in minutes, based on selected radio button.
        * @since S60 3.0
        */
        TInt GetAoIntervalTimeInMinutes();

        /**
        *
        * @since S60 3.0
        */
        void HideItemsPermanently();

        /**
        *
        * @since S60 3.0
        */
        void UpdateItemText(
            const TUid& aItemId );

        /**
        *
        * @since S60 3.0
        */
        void UpdateFetchLimitSetting();

        /**
        *
        * @since S60 3.2
        */
        void GetLoginInformationL(
            TDes8& aUserName,
            TDes8& aPassword,
            TBool& aPasswordTemporary );

    private:  // Functions from base classes

        virtual TInt EventSubArrayChangeL(
            CMuiuSettingBase& aBaseItem );

        virtual TMuiuPageEventResult EventItemEditStartsL(
            CMuiuSettingBase& aBaseItem );

        virtual TInt EventItemEditEndsL(
            CMuiuSettingBase& aBaseItem );

        virtual TMuiuPageEventResult EventItemEvaluateText(
            const CMuiuSettingBase& aBaseItem,
            TDes& aNewText );

        virtual TMuiuPageEventResult EventItemEvaluateRadioButton(
            const CMuiuSettingBase& aBaseItem,
            TInt& aNewValue );

        /**
        *
        * @since S60 3.0
        */
        void FindMultilineEditorL(
            const TUid& aId,
            CMuiuSettingsEditValue*& aTopEditor,
            CMuiuSettingsEditValue*& aBottomEditor );

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult SettingLaunchMultilineEditorL(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );

        /**
        *
        * @since S60 3.0
        */
        void EventUpdateAlwaysOnline(
            const TInt aNewState );

        /**
        *
        * @since S60 3.0
        */
        void EventUpdateMailNotifications(
            const TInt aNewState );

        /**
        *
        * @since S60 3.0
        */
        void EventAOForceTurnOff();

        /**
        *
        * @since S60 3.0
        */
        void EventEMNForceTurnOff();

        /**
        *
        * @since S60 3.0
        */
        void EventUpdateAoDays( CMuiuSettingBase& aBaseItem );

        /**
        *
        * @since S60 3.0
        */
         void IncludePartialFetchString(
            CMuiuSettingBase& aRetrievedParts );

        /**
        * @since S60 3.0
        */
        virtual void AcquireCustomSettingPageL(
            CAknSettingPage*& aReturnPage,
            TMuiuDynSetUpdateMode& aReturnUpdateMode,
            TInt& aReturnSettingValue,
            TMuiuSettingsText& aReturnSettingText,
            CDesCArrayFlat& aReturnRadioButtonArray,
            CMuiuSettingBase& aParamBaseItem );

        /**
        *
        * @since S60 3.0
        */
        CAknSettingPage* AcquireCustomEncodingPageL(
            TMuiuDynSetUpdateMode& aReturnUpdateMode,
            TInt& aReturnSettingValue,
            CDesCArrayFlat& aReturnRadioButtonArray,
            CMuiuSettingBase& aParamBaseItem );

        /**
        *
        * @since S60 3.0
        */
        void AcquireCustomEncodingButtonArrayL(
            CDesCArrayFlat& aButtons );

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

        /**
         * Returns the id of the editable mailbox
         *
         * @since S60 v3.2
         * @return Mailbox Id
         */
        TMsvId CurrentMailboxId();

        // From CMuiuDynamicSettingsDialog
        virtual void HandleListBoxEventL(
            CEikListBox* aListBox,
            TListBoxEvent aEventType );

        /**
         * Method is used to execute flags with a delay. SubscribeFoldersL
         * method needs to use this when Exit is issued.
         *
         * @return Mailbox Id
         */
        static TInt DelayedExecFlags( TAny* aPtr );

    public:     // Data
    protected:  // Data
    private:    // Data

        // Temporary array for initializing phase
        CMuiuDynFinderItemArray*    iInitArray;
        // Owned: Pointer to symbian data object
        CImumMboxData*              iAccountSettings;
        //
        TDialogFlags                iInitSetFlag;
        //
        TInt                        iInitCurrentIndex;
        //
        TInt                        iInitResource;
        //
        CMuiuDynInitItemIdArray*    iInitTreeArray;
        //
        CMuiuDynInitItemIdArray*    iInitIdArray;
        //
        CMuiuDynInitItemValueArray* iInitValueArray;
        //
        CMuiuDynInitItemTextArray*  iInitTextArray;
        //
        CMuiuDynInitInfoArray*      iInitInfoArray;
        //
        CIdle*                      iIdle;
    };

#endif      // IMUMMAILSETTINGSDIALOG_H

// End of File
