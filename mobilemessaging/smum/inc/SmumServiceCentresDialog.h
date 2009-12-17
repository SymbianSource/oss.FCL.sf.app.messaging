/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*     Service Centres dialog.
*
*/



#ifndef SMUMSERVICECENTRESDIALOG_H
#define SMUMSERVICECENTRESDIALOG_H

//  INCLUDES
#include <AknForm.h>    // CAknDialog
#include <eiklbo.h>     // MEikListBoxObserver
#include "SMSETDLG.H"   // TSmumSCNameInfo, TSmsSettingSCNames, CSmumSCArray

#include <bldvariant.hrh>

// CONSTANTS

// MACROS

// DATA TYPES

enum TSmumScDeletionValues
    {
    ESmumScNormalDelete = 0,
    ESmumScLastOne,
    ESmumScInUse
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMuiuSettingsArray;
class CEikTextListBox;
class CSmsSettings;
class CAknTitlePane;

// CLASS DECLARATION

/**
*  Message centre's general settings dialog.
*/
NONSHARABLE_CLASS( CSmumServiceCentresDialog ) :
    public CAknDialog,
    public MEikListBoxObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Constructor.
        */
        static CSmumServiceCentresDialog* NewL( 
            CSmsSettings& aSettings,
            TInt& aExitCode,
            TInt aMenuTitleResourceId,
            TBool aHelpFeatureSupported,
            TBool aSimSCsOnly = EFalse );

        /**
        * Destructor.
        */
        ~CSmumServiceCentresDialog();

    protected:  // Functions from base classes
        
        /**
        * From CAknDialog
        */
        void ProcessCommandL(TInt aCommandId);

        /**
        * From CAknDialog
        */
        void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);
        
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
        * From CCoeControl, returns help context
        */
        void GetHelpContext(TCoeHelpContext& aContext) const; 

    private: // new functions
        
        /**
        * Launch add SC dialog.
        * @param none
        * @return nothing
        */
        void AddServiceCentreL();

        /**
        * Launch edit SC dialog.
        * @param none
        * @return nothing
        */
        void EditServiceCentreL();

        /**
        * Delete service centre.
        * @param none
        * @return nothing
        */
        void DeleteServiceCentreL();

        /**
        * Launch list query for the user to choose new SC in use.
        * @param none
        * @return nothing
        */
        void LaunchScInUseListQueryL();

        /**
        * Updates the service centre array to Sms Settings.
        * @param none
        * @return nothing
        */
        void ClosingServiceCentresDialogL() const;
         
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL(TInt aMenuTitleResourceId);
        CSmumServiceCentresDialog( 
        	CSmsSettings& aSettings, 
        	TInt& aExitCode, 
        	TBool aHelpFeatureSupported, 
        	TBool aSimSCsOnly /*= EFalse*/);

        /** 
        * Launches help application
        */
        void LaunchHelpL();
  
        /** 
        * Updates Middle softkey visibility
        */                        
        void UpdateButtonVisibility();
        
        /** 
        * Does delayed exit after specified delay
        * @since S60 v5.0
        * @param aDelayTime Requested delay in microseconds
        */ 
        void DoDelayedExitL( const TInt aDelay );
        
        /** 
        * Used for exiting application properly
        * @since S60 v5.0
        * param aThis Pointer to this AppUi
        */         
        static TInt DelayedExit( TAny* aThis );
        
    private:  // Data
        CEikTextListBox*    iListBox;
        CSmumSCArray*       iSCAddressesList;
        CSmsSettings&       iSettings;
        CAknTitlePane*      iTitlePane;
        HBufC*              iPreviousTitleText;
        TInt&               iExitCode;
        TBool               iHelpFeatureSupported;
        TBool				iSimSCsOnly;
        TBool				iMSKEnabledInPlatform;
        CPeriodic*          iIdle; 
        };
#endif // SMUMSERVICECENTRESDIALOG_H
            
// End of File
