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
*     A dialog for adding new and editing old service centres.
*
*/



#ifndef SMUMSERVICECENTREITEMDIALOG_H
#define SMUMSERVICECENTREITEMDIALOG_H

//  INCLUDES
#include <AknForm.h> // CAknDialog
#include <eiklbo.h>  // MEikListBoxObserver

#include <bldvariant.hrh>

// CONSTANTS

// MACROS

// DATA TYPES
enum TSmumServiceCentreItemIndex
    {
    ESmumServiceCentreName      = 0,
    ESmumServiceCentreNumber    = 1
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMuiuSettingsArray;
class CEikTextListBox;
class CSmsSettings;
class CAknTitlePane;
class CSmumSCArray;

// CLASS DECLARATION

/**
*  SMUM's main sms settings dialog.
*/
NONSHARABLE_CLASS( CSmumAddEditServiceCentreDialog ):
    public CAknDialog,
    public MEikListBoxObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Constructor.
        */
        CSmumAddEditServiceCentreDialog(
            TDes& aName, 
            TDes& aNumber,
            TInt& aExitCode,
            TBool aHelpFeatureSupported,
            CSmumSCArray* aSCDlgsAddressesList = NULL,
            TBool aSimSCsOnly = EFalse );

        /**
        * Destructor.
        */
        ~CSmumAddEditServiceCentreDialog();

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
        * From MEikListBoxObserver
        */
        void HandleListBoxEventL(
            CEikListBox* aListBox, 
            TListBoxEvent aEventType);

        /**
        * From CAknDialog
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

        /**
        * From CAknDialog
        */
        void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);
        
        /**
        * From CCoeControl, returns help context       
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;  

    private: // New functions
    
        /**
        * Updating the array with new values from dialog's list boxes.
        * @param none
        * @return TBool
        */
        TBool ClosingSCItemDialogL();

        /**
        * Prepare to open needed setting page
        * @param TBool aEnterPressed
        * @return nothing
        */
        void OpenSettingPageL(TBool aEnterPressed);

        /**
        * Open settingpage for editing name, update title if necessary
        * @param TBool aEnterPressed
        * @param TInt aIndex
        * @return nothing
        */
        void ModifyScNameL(TBool aEnterPressed, TInt aIndex);
        
        /**
        * Open settingpage for editing number
        * @param TBool aEnterPressed
        * @param TInt aIndex
        * @return nothing
        */
        void ModifyScNumberL(TBool aEnterPressed, TInt aIndex);

        /**
        * Replaces tab and enter characters by spaces using AknTextUtils::ReplaceCharacters
        */
        void ReplaceCharacters( TDes& aText );

        /** 
        * Launches help application
        */
        void LaunchHelpL();
               
    private:  // Data
        CMuiuSettingsArray* iSettingsArray;
        CEikTextListBox*    iListBox;
        TDes&               iName;
        TDes&               iNumber;
        TInt&               iExitCode;
        TBool               iHelpFeatureSupported;
        CSmumSCArray*       iSCDlgsAddressesList;
        CAknTitlePane*      iTitlePane;
        HBufC*              iPreviousTitleText;
        TBool 				iSimSCsOnly;
    };
#endif      // SMUMSERVICECENTREITEMDIALOG_H   
            
// End of File
