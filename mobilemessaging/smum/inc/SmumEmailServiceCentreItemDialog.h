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
*     A dialog for adding new and editing old email over sms centre.
*
*/



#ifndef SMUMEMAILSERVICECENTREITEMDIALOG_H
#define SMUMEMAILSERVICECENTREITEMDIALOG_H

//  INCLUDES
#include <AknForm.h> // CAknDialog
#include <eiklbo.h>  // MEikListBoxObserver

#include <bldvariant.hrh>

// CONSTANTS

// MACROS

// DATA TYPES
enum TSmumEmailServiceCentreItemIndex
    {
    ESmumEmailGateway,
    ESmumEmailServiceCentreNumber
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMuiuSettingsArray;
class CEikTextListBox;
class CAknTitlePane;

// CLASS DECLARATION

/**
* SMUM's main sms settings dialog.
* @since 2.6
*/
NONSHARABLE_CLASS( CSmumEmailServiceCentreDialog ):
    public CAknDialog,
    public MEikListBoxObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Constructor.
        */
        CSmumEmailServiceCentreDialog(
            TDes& aGateway, 
            TDes& aSC,
            const TBool& aModifiable,
            TInt& aExitCode,
            TBool aHelpFeatureSupported );

        /**
        * Destructor.
        */
        ~CSmumEmailServiceCentreDialog();

    protected:  // Functions from base classes

        /**
        * From CAknDialog
        */
        void ProcessCommandL( TInt aCommandId );

        /**
        * From CEikDialog
        */
        void PreLayoutDynInitL();

        /**
        * From CEikDialog
        */
        TBool OkToExitL( TInt aButtonId );

        /**
        * From MEikListBoxObserver
        */
        void HandleListBoxEventL(
            CEikListBox* aListBox, 
            TListBoxEvent aEventType);

        /**
        * From CAknDialog
        */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

        /**
        * From CAknDialog
        */
        void DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane );
        
        /**
        * From CCoeControl, returns help context       
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;  

    private: // New functions
    
        /**
        * Updating the array with new values from dialog's list boxes.
        * @since 2.6
        * @return ETrue if closing is ok
        */
        TBool ClosingEmailSettingsItemDialogL();

        /**
        * Prepare to open needed setting page
        * @since 2.6
        * @param aEnterPressed
        */
        void OpenSettingPageL( TBool aEnterPressed );

        /**
        * Open settingpage for editing selected setting
        * @since 2.6
        * @param aEnterPressed
        * @param aIndex position of selected setting
        */
        void ModifySettingNumberL( TBool aEnterPressed, TInt aIndex );

        /** 
        * Launches help application
        * @since 2.6
        */
        void LaunchHelpL() const;
        
        /**
        * Shows an information note
        * @since 2.6
        * @param aResource, resourse id used in note text
        */
        void ShowInformationNoteL( TInt aResource );               

    private:  // Data
        CMuiuSettingsArray* iSettingsArray;
        CEikTextListBox*    iListBox;
        CAknTitlePane*      iTitlePane;
        HBufC*              iPreviousTitleText;
        TDes&               iGateway;
        TDes&               iSC;
        TBool               iModifiable;        
        TInt&               iExitCode;
        TBool               iHelpFeatureSupported;
        TBool               iLaunchDialog;        
    };
#endif      // SMUMEMAILSERVICECENTREITEMDIALOG_H
            
// End of File
