/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*     Sms sim dialog header.
*
*/



#ifndef __SMSSIMDLG_H__
#define __SMSSIMDLG_H__

#include <simdlgplugin.h> //CSimDlgPlugin
#include <eikdialg.h>// CEikDialog
#include <msvstd.h>  // TMsvId
#include <AknForm.h> // CAknDialog
#include <eiklbo.h>  // MEikListBoxObserver
#include <gulicon.h> // CGulIcon
#include <MuiuMsvSingleOpWatcher.h> // MMsvSingleOpWatcher
#include <AknWaitDialog.h>          // CAknWaitDialog

#include <bldvariant.hrh>

// FORWARD DECLARATIONS
class CEikColumnListBox;
class CSmsMtmUi;
class CMsvSession;
class CCoeEnv;
class CAknWaitDialog;
class CAknProgressDialog;


const TInt KSmumMaxDisplayTextLength = 70;
const TInt KSmumMaxVisibleChars = 30;
const TInt KSmumMarkReplacementStringLength = 2;


//  CLASS DEFINITIONS

/**
* CSimMessageArray
* Helper array class used by CSmsSimDialog
*/
NONSHARABLE_CLASS( CSimMessageArray ) : 
    public CBase, 
    public MDesCArray
    {
    public:
        /**
        * Two phase constructor
        */
        static CSimMessageArray* NewL( CMsvSession& aSession );

        /**
        * Destructor
        */
        ~CSimMessageArray();

        /**
        * Update sim messages array
        * @param aFolderId: id of the folder where sim messages are
        */
        void RefreshArrayL( TMsvId aFolderId );

        /**
        * Returns entry id 
        * @param aIndex: index of the entry
        * @return TMsvId: id of the entry
        */
        TMsvId EntryId( TInt aIndex ) const;

    public: // Functions from base classes

        /**
        * from MDesCArray
        */
        virtual TInt MdcaCount() const;

        /**
        * from MDesCArray
        */
        virtual TPtrC MdcaPoint(TInt aIndex) const;

    private:
        /**
        * C++ constructor
        */
        CSimMessageArray( CMsvSession& aSession );

        /**
        * EPOC constructor
        */
        void ConstructL();

    private:
        CMsvSession&    iSession;
        CMsvEntry*      iSmsServiceEntry;
        HBufC*          iListBoxText;        
    };


/**
* CSmsSimDialog
* Dialog class to display sim messages
*/
NONSHARABLE_CLASS( CSmsSimDialog ) : 
    public CAknDialog,
    public MEikListBoxObserver,
    public MMsvSingleOpWatcher,
    public MProgressDialogCallback
    {
    public:
	    /**
        * Two phase constructor
        * @param aSmsMtmUi: reference to sms mtm ui object
        * @param aSession: reference to message server session
        * @return object of CSmsSimDialog
        */
        static CSmsSimDialog* NewL(CSmsMtmUi&  aSmsMtmUi, CMsvSession& aSession, TInt& aExitCode);
	   
        /**
        * Destructor
        */
        virtual ~CSmsSimDialog();
     
    
public:   
        /**
        * from CEikDialog
        */
        void PreLayoutDynInitL();

        /**
        * from CEikDialog
        */
        TBool OkToExitL(TInt aButtonId);

        /**
        * From CAknDialog
        */
        void ProcessCommandL( TInt aCommandId );

        /**
        * From MEikMenuObserver
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * from CCoeControl
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType) ;

        /**
        * From MMsvSingleOpWatcher
        */
        void OpCompleted(
            CMsvSingleOpWatcher& aOpWatcher, 
            TInt aCompletionCode );

        /**
        * from MProgressDialogCallback
        */
        void DialogDismissedL( TInt aButtonId );

        /**
        * From CCoeControl, returns help context
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;

    protected:

        /**
        * From MEikListBoxObserver
        */
        virtual void HandleListBoxEventL(
            CEikListBox* aListBox, 
            TListBoxEvent aEventType );

    private:

		/**
        * C++ constructor
        */
        CSmsSimDialog(CSmsMtmUi&  aSmsMtmUi, CMsvSession& aSession, TInt& aExitCode);

        /**
        * Symbian OS constructor
        */
        void ConstructL();

        /**
        * Copies sim messages to local folder
        */
        void HandleCopyFromSimToLocalFolderL();

        /**
        * Deletes sim messages from sim card
        */
        void HandleDeleteFromSimL();

        /**
        * Returns object of list box
        * @return pointer to list box object.
        */
        CEikColumnListBox* ListBox() const;

        /**
        * Reads sim messages from sim card.
        */
        void EnumerateSimMessagesL();

        /**
        * Deletes temporary sim messages folder.
        */
        void DeleteSimFolderL();

        /**
         * Handles Edit submenu. Checks what items are selected and deletes 
         * Mark, Unmark, Mark All, Unmark All menu items accordingly.
         */
        void DynInitEditMenuL( CEikMenuPane* aMenuPane );

        /**
         * Creates CMsvEntrySelection array from the selected items.
         * If no items marked adds item under 'cursor' to the array.
         * @return pointer to CMsvEntrySelection. Caller must pop and delete array.
         */
        CMsvEntrySelection* CreateSelectionArrayLC();

        /**
         * Updates iMessages array
         */
        void UpdateSimMessagesArrayL();

        /**
         * Stores current title pane text and replaces it with new one (qtn_mce_title_sim)
         */
        void StoreTitleTextAndSetNewL();

        /**
         * Restores title pane text with the text stored in 
         * StoreTitleTextAndSetNewL.
         */
        void RestoreTitleTextL();

        /**
        * Called by OpCompleted, does work after operation is completed.
        * Gets progress of the completed operation, calls sms mtm ui's DisplayProgressSummary.
        * If completed enumeration operation, updates dialog's array if no errors.
        *    if errors, dismisses whole dialog.
        * If completed copy or delete operation, starts enumeration.
        * @param aOperation: pointer to operation which is completed.
        * @param aCompletionCode: Same as in OpCompleted.
        * @return ETrue if TryToExitL is called. That can happen
        *    if there is errors with enumeration operation and dialog is dismissed.
        *    OpCompleted should do nothing.
        */ 
        TBool DoOperationCompletedL(
            CMsvOperation* aOperation, 
            TInt aCompletionCode );

        /**
        * TCleanupItem to clean wait or progress dialogs if leave happens after
        * dialog creation.
        * @param aAny: pointer to dialog pointer.
        */
        static void CleanupWaitDialog(TAny* aAny);

        /** 
        * Launches help application
        */
        void LaunchHelpL();
    
        /** 
        * Launches note and starts copy/delete operation
        * @param aDestinationId to separate copy/delete operation
        */
        void LaunchNoteAndOperationL( TMsvId aDestinationId = NULL );

		/** 
        * Updates Middle softkey
        */
		void UpdateMskL();
    private:
        CSmsMtmUi&          iSmsMtmUi;
        CMsvSession&        iSession;
        CSimMessageArray*   iMessages;
        TMsvId              iSimFolderId;
        HBufC*              iOldTitleText;
        TBuf<KSmumMarkReplacementStringLength>  iMarkReplacementText;
        CMsvSingleOpWatcher* iEnumerationOperation;
        CMsvSingleOpWatcher* iCopyOrDeleteOperation;
        CAknWaitDialog*     iWaitDialog;
        CAknProgressDialog* iProgressDialog;
        TInt&               iExitCode;
        TBool               iSelectionKeyPressed;
        TBool               iDialogDismissed;
        TBool               iHelpFeatureSupported;
        TInt                iMsgCountBeforeOperation;
        TInt                iResourceFileOffset;
 	    TBool               iContextCommand;
    };

#endif // __SMSSIMDLG_H__

//  End of File
