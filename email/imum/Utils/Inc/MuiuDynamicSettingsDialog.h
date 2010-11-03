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
* Description:  This file contains declearation of settings handler class
*
*/



#ifndef MUIUDYNAMICSETTINGSDIALOG_H
#define MUIUDYNAMICSETTINGSDIALOG_H

// INCLUDES
#include <e32base.h>                // CArrayPtrFlat
#include <eiklbo.h>                 // MEikListBoxObserver
#include <coedef.h>                 // TKeyResponse
#include <w32std.h>                 // TKeyEvent
#include <eikcmobs.h>               // MEikCommandObserver
#include <badesca.h>                // CDesCArrayFlat
#include <aknsettingpage.h>         // MAknSettingPageObserver
#include <eikbtgpc.h>               // CEikButtonGroupContainer
#include "MuiuDynamicSettingsArray.h"
#include "IMSSoftkeyControl.h"
#include "IMSPageResourceControl.h"
#include <muiuflags.h>

// CONSTANTS
enum TMuiuPageResult
    {
    EMuiuPageResultOk = 0,
    EMuiuPageResultSubMenuOpen,
    EMuiuPageResultInvalidResult,
    EMuiuPageResultReadOnly,
    EMuiuPageResultCancelled,
    EMuiuPageResultEditing,
    EMuiuPageResultEventCancel,
    EMuiuPageResultPageLocked
    };

enum TMuiuPageEventResult
    {
    // Given value is approved
    EMuiuPageEventResultApproved = 0,
    // User has entered an invalid value, editor page should be reopened
    EMuiuPageEventResultDisapproved,
    // The editor has been cancelled and should be closed
    EMuiuPageEventResultCancel
    };

// MACROS
// DATA TYPES
typedef CArrayFixFlat<TInt> CMuiuDynSetIntArray;
typedef CArrayPtrFlat<CAknSettingPage> CMuiuDynSetPages;
typedef CAknSettingPage::TAknSettingPageUpdateMode TMuiuDynSetUpdateMode;

enum TMuiuDynInitInfo
    {
    // Indicates the next type needs value to be initialized
    EMuiuDynInitInfoValue = 0,
    // Indicates the next type needs text to be initialized
    EMuiuDynInitInfoText
    };

// Arrays for initializing items
typedef CArrayFixFlat<TUid> CMuiuDynInitItemIdArray;
typedef CArrayFixFlat<TInt> CMuiuDynInitItemValueArray;
typedef CArrayPtrFlat<TMuiuSettingsText> CMuiuDynInitItemTextArray;
typedef CArrayFixFlat<TMuiuDynInitInfo> CMuiuDynInitInfoArray;

// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CEikFormattedCellListBox;
class CEikScrollBarFrame;
class CAknSettingPage;
class CAknTitlePane;
class CAknQueryDialog;
class CEikButtonGroupContainer;
class CSelectionItemList;
class CMuiuFlags;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.0
*/
class CMuiuDynamicSettingsDialog :
    public CMuiuDynamicSettingsArray,
    public MEikListBoxObserver,
    public MEikCommandObserver,
    public MAknSettingPageObserver
    {
    public: // Constructors and destructor
        virtual ~CMuiuDynamicSettingsDialog();

    public: // New virtual functions

        virtual TBool SettingsStartedL();

    public: // New functions

        TInt CurrentItemIndex() const;

        CMuiuSettingBase* CurrentItem() const;

    public: // Functions from base classes

    protected:  // Constructors

        CMuiuDynamicSettingsDialog(
            CEikFormattedCellListBox& aListBox,
            CAknTitlePane& aTitlePane );
        void SettingsConstructL(            
            CEikButtonGroupContainer& aButtons,            
            const TInt aResource = NULL,
            const TBool aUpdateMSK = EFalse,
            const TBool aLockedSettings = EFalse );

    protected:  // New virtual functions

        // From MAknSettingPageObserver
        virtual void HandleSettingPageEventL(
            CAknSettingPage* aSettingPage,
            TAknSettingPageEvent aEventType ) = 0;

        virtual TBool OkToExitL(
            const TInt aButtonId ) = 0;

        virtual TInt InitItemUnidentified(
            CMuiuSettingBase& aBaseItem,
            const TInt aValue,
            const TMuiuSettingsText* aText );

        /**
        *
        * @since S60 3.0
        */
        virtual TMuiuPageEventResult SettingLaunchMultilineEditorL(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );

        /**
        *
        * @since S60 3.0
        */
        virtual TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

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
        virtual TInt EventSubArrayChangeL(
            CMuiuSettingBase& aBaseItem );
        virtual TMuiuPageEventResult EventItemEditStartsL(
            CMuiuSettingBase& aBaseItem );
        virtual TInt EventItemEditEndsL(
            CMuiuSettingBase& aBaseItem );
        virtual TMuiuPageEventResult EventItemEvaluateValue(
            const CMuiuSettingBase& aBaseItem,
            TInt& aNewValue );
        virtual TMuiuPageEventResult EventItemEvaluateRadioButton(
            const CMuiuSettingBase& aBaseItem,
            TInt& aNewValue );
        virtual TMuiuPageEventResult EventItemEvaluateText(
            const CMuiuSettingBase& aBaseItem,
            TDes& aNewText );

    protected:  // New functions

        // Setting page opening

        TMuiuPageResult OpenSettingPageL(
            const TInt aIndex = KErrNotFound );
        TMuiuPageResult OpenSettingPageL(
            CMuiuSettingBase& aBaseItem );

        void SetTitlePaneTextL(
            const TDesC& aText,
            const TBool aSaveCurrent = ETrue );
        void SetPreviousTitlePaneText(
            const TBool aSaveCurrent = ETrue );

        /**
        * Find the editor from the index in the array under the given item
        * @since S60 3.0
        */
        CMuiuSettingBase* SettingFindEditor(
            const CMuiuSettingBase& aBaseItem,
            const TInt aIndex,
            const TBool aExcludeHidden = ETrue );

        /**
        * Find the editor based on the id under the given item
        * @since S60 3.0
        */
        CMuiuSettingBase* SettingFindEditor(
            const CMuiuSettingBase& aBaseItem,
            const TUid& aId,
            const TBool aExcludeHidden = ETrue );

        /**
        *
        * @since S60 3.0
        */
        TInt InitAnyItem(
            CMuiuSettingBase& aBaseItem,
            const TInt aValue,
            const TMuiuSettingsText* aText );

        /**
        *
        * @since S60 3.0
        */
        TInt InitAnyMultiItem(
            const CMuiuDynInitItemIdArray* aStartTree,
            const CMuiuDynInitItemIdArray& aIdArray,
            const CMuiuDynInitItemValueArray* aValueArray = NULL,
            const CMuiuDynInitItemTextArray* aDescArray = NULL,
            const CMuiuDynInitInfoArray* aInitInfoArray = NULL );

        /**
        *
        * @since S60
        * @return
        */
        TInt ShowCustomQueryL(
            const TInt aResourceStringId );

        TInt GetTextEditorFlags( const CMuiuSettingBase& aBaseItem );
        TInt GetValueEditorFlags( const CMuiuSettingBase& aBaseItem );

        /**
        *
        * @since S60 3.0
        */
        void SetListboxEmptyTextL( const TDesC& aText );


        /**
        * Gets the setting page resource of specific item
        * @since S60 3.0
        * @return resource id of the page
        */
        TInt SettingPageResource(
            const TIMSPageResource aSettingPage ) const;

        /**
        * Sets the new setting page resource for specific item
        * @since S60 3.0
        * @param aSettingPage, Inserts the resource for the page
        */
        void SetSettingPageResource(
            const TIMSPageResource aSettingPage,
            const TInt aResourceId );

        /**
        *
        * @since S60 3.0
        */
        void CheckRadioButton(
            CMuiuSettingsLink& aRadioButtonArray,
            const TInt aRadioButtonIndex );

        void CheckRadioButton(
            CMuiuSettingsLink& aRadioButtonArray,
            const TUid& aId );

        /**
        *
        * @since S60 3.0
        */
         TInt FindCheckedRadiobutton(
            const CMuiuSettingsLinkExtended& aArray,
            TUid& aId,
            TInt& aIndex );

        /**
        *
        * @since S60 3.0
        */
        void AddBackupItemL(
            const CMuiuSettingBase& aBaseItem );

        /**
        *
        * @since S60 3.0
        */
        void RemoveLastBackupItem();

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingBase* GetLastBackupItem();

        /**
        *
        * @since S60 3.0
        */
        void RestoreFromBackup();

        /**
        *
        * @since S60 3.0
        */
        void CleanBackupArray();      

        /**
        *
        * @since S60 3.2
        */
        void UpdateMskL();

        /**
        *
        * @since S60 3.2
        */
        void SetMskL();

        /**
        *
        * @since S60 3.2
        */
        TInt MskCommand();

    protected:  // Functions from base classes

        // MEikCommandObserver
        virtual void ProcessCommandL( TInt aCommandId ) = 0;

        // MEikListBoxObserver
        virtual void HandleListBoxEventL(
            CEikListBox* aListBox,
            TListBoxEvent aEventType );

    private:  // New virtual functions
    private:  // New functions

        // Setting page opening

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageResult SettingsPageOpenCheckL();

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageResult SettingPageOpenEditorL();

        /**
        *
        * @since S60 3.0
        */
        void SettingPageOpenFinishL();

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageResult SettingPageOpenL(
            CMuiuSettingBase& aBaseItem );
        TMuiuPageResult SettingLaunchPageLD(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );
        TMuiuPageResult SettingLaunchRadioButtonPageLD(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );
        TMuiuPageResult SettingLaunchCheckboxPageLD(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );
        TMuiuPageResult SettingLaunchTextEditorPageLD(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );
        TMuiuPageResult SettingLaunchNumberEditorPageLD(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );
        TMuiuPageResult SettingLaunchScNumberEditorPageLD(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );
        TMuiuPageResult SettingLaunchMultilinePageL(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );

        /**
        *
        * @since S60 3.0
        */
        void SettingsCheckNextEditorOpen(
            TMuiuPageResult& aResult );

        // Setting page creation

        /**
        * Creates editor of spesific type
        * @since S60 3.0
        */
        void SettingCreateEditorL(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );
        void SettingCreateRadioButtonPageL(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );
        void SettingCreateCheckboxPageL(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );
        void SettingCreatePlainTextEditorPageL(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );
        void SettingCreatePasswordTextEditorPageL(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );
        void SettingCreatePlainNumberEditorPageL(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );
        void SettingCreateScNumberEditorPageL(
            CMuiuSettingBase& aBaseItem,
            TMuiuDynSetUpdateMode& aUpdateMode );

        TMuiuDynSetUpdateMode DefineDefaultSettingPageL(
            const CMuiuSettingBase& aBase );
        void SettingCreateButtonArrayL(
            CMuiuDynSetItemArray& aButtonArray );
        void SettingCreateCheckboxArrayL(
            CMuiuDynSetItemArray& aButtonArray );

        // Setting page acquiring

        /**
        *
        * @since S60 3.0
        */
        void SettingPrepareAcquireL(
            TMuiuDynSetUpdateMode& aUpdateMode,
            CMuiuSettingBase& aBaseItem );

        /**
        *
        * @since S60 3.0
        */
        void SettingPrepareAcquireRadioButtonPageL(
            CMuiuSettingBase& aBaseItem );

        /**
        *
        * @since S60 3.0
        */
        void SettingPrepareAcquireCheckboxPageL(
            CMuiuSettingBase& aBaseItem );

        /**
        *
        * @since S60 3.0
        */
        void SettingPrepareAcquireTextEditorPageL(
            CMuiuSettingBase& aBaseItem );

        /**
        *
        * @since S60 3.0
        */
        void SettingPrepareAcquireValueEditorPageL(
            CMuiuSettingBase& aBaseItem );


        // Setting initialization

        /**
        *
        * @since S60 3.0
        */
        TInt InitArrayLink(
            CMuiuSettingBase& aBaseItem,
            const TMuiuSettingsText* aText );
        TInt InitArrayRadioButton(
            CMuiuSettingBase& aBaseItem,
            const TInt aValue,
            const TMuiuSettingsText* aText );
        TInt InitArrayCheckBox(
            CMuiuSettingBase& aBaseItem,
            const TInt aValue,
            const TMuiuSettingsText* aText );
        TInt InitItemTextEdit(
            CMuiuSettingBase& aBaseItem,
            const TMuiuSettingsText* aText );
        TInt InitItemNumberEdit(
            CMuiuSettingBase& aBaseItem,
            const TInt aValue,
            const TMuiuSettingsText* aText );

        /**
        *
        * @since S60 3.0
        */
        void PrepareInit(
            CMuiuSettingBase& aBaseItem,
            const CMuiuDynInitItemValueArray* aValueArray,
            const CMuiuDynInitItemTextArray* aDescArray,
            const CMuiuDynInitInfoArray* aInitInfoArray,
            TInt& aValueIndex,
            TInt& aTextIndex,
            TInt aIndex );

        /**
        *
        * @since S60 3.0
        */
        TInt EventPrepareSubArrayL(
            CMuiuSettingBase& aBaseItem,
            const TBool aForward = ETrue );

        /**
        * Gets the previous stored value and removes it
        * @since S60 3.0
        * @return Last index
        */
        TInt SettingsLastItemIndex( const TBool aRemove = ETrue ) const;

        /**
        * Adds current itemindex to stack
        * @since S60 3.0
        * @param aSettingsLastItemIndex, new index
        */
        void SetSettingsLastItemIndexL( const TInt aLastItemIndex );

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingBase* CurrentSettingsPage(
            const TBool aRemove = EFalse );

        /**
        *
        * @since S60 3.0
        */
        void SetCurrentSettingsPage(
            CMuiuSettingBase& aBaseItem );

        // Backing up

        /**
        *
        * @since S60 3.0
        */
        void BackupItemCreateLinkL(
            const CMuiuSettingBase& aBaseItem );
        void BackupItemCreateRadioL(
            const CMuiuSettingBase& aBaseItem );
        void BackupItemCreateValueL(
            const CMuiuSettingBase& aBaseItem );
        void BackupItemCreateTextL(
            const CMuiuSettingBase& aBaseItem );
        void BackupItemCreateUndefinedL(
            const CMuiuSettingBase& aBaseItem );

        /**
        *
        * @since S60 3.0
        */
        virtual CMuiuSettingBase* CreateItemForBackupL(
            const CMuiuSettingBase& aBaseItem );

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageResult HandleEditorCancel();

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageResult HandleEditorOk();

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageResult HandleEditorDisapproved();

    private:  // Functions from base classes

        // CMuiuDynamicSettingsArray
        TInt EventArrayChangedL(
            const TMuiuArrayEvents aEvent );

        TBool EventCustomMdcaPoint(
            const TUid& aId,
            TPtr& aString ) const;

    public:     // Data
    protected:  // Data

        // DIALOG REFERENCES
        // The listbox that is shown in view for user
        CEikFormattedCellListBox& iListBox;
        // Used titlepane in settings dialog
        CAknTitlePane&      iTitlePane;
        // Buttons used to show in the dialog
        CIMSSoftkeyControl* iSoftkeys;
        // Temporary storage for settings page
        CAknSettingPage*        iSettingPage;        

    private:    // Data

        enum TDynSettingsFlags
            {
            EMuiuDynSetStartIssued = 0,
            EMuiuDynSetRunningCustomPage,
            };

        // RESOURCES
        // Status flags
        TMuiuFlags          iDynSettingsFlags;
        // Container for resource page ids
        CIMSPageResourceControl* iSettingPageResources;
        // Container array for items to be edited
        CMuiuDynSetItemArray* iSettingsPageArray;

        // DIALOG
        // Pointer to scrollbar used in settings page
        CEikScrollBarFrame* iScrollBar;
        // Temporary place to store the previous titlepane text
        HBufC*              iOldTitleText;
        // Object for query dialog
        CAknQueryDialog*    iQueryDialog;

        // SETTING PAGE
        // Previous location of cursor
        CMuiuDynSetIntArray*    iSettingsLastItemIndex;
        // Temporary value for setting page
        TInt                    iSettingValue;
        // Temporary text for setting page
        TMuiuSettingsText*      iSettingText;
        // Temporary text for password setting page
        TMuiuPasswordText*      iNewPassword;
        // Temporary text for password setting page
        TMuiuPasswordText*      iOldPassword;
        // Temporary array for buttons
        CDesCArrayFlat*         iButtonArray;
        // Temporary array for checkboxes
        CSelectionItemList*     iCheckboxArray;
        // Array to temporary store the items to be edited
        CMuiuDynSetItemArray*   iBackupArray;
        // Index to focused list box item
        TInt                    iCurrentItemIndex; 
        CMuiuFlags*             iFeatureFlags; 
    };

#endif // MUIUDYNAMICSETTINGSDIALOG_H

// End of File
