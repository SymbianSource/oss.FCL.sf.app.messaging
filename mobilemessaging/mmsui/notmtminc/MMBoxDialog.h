/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     MMS MMBox dialog defenitions.
*
*/



#ifndef __MMBOXDLG_H__
#define __MMBOXDLG_H__

#include <eikdialg.h>// CEikDialog
#include <msvstd.h>  // TMsvId
#include <AknForm.h> // CAknDialog
#include <eiklbo.h>  // MEikListBoxObserver
#include <gulicon.h> // CGulIcon
#include <MuiuMsvSingleOpWatcher.h> // MMsvSingleOpWatcher
#include <MsvPrgReporter.h> // MMsvProgressDecoder
#include <AknWaitDialog.h>          // CAknWaitDialog

#include <bldvariant.hrh>

#include "MMBoxArray.h"

// FORWARD DECLARATIONS
class CEikColumnListBox;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CNotMtmUi;
class CNotMtmUiData;
class CMmsNotificationClientMtm;
class CClientMtmRegistry;
class CMsvSession;
class CCoeEnv;
class CAknWaitDialog;
class CAknProgressDialog;
class TMsvSelectionOrdering;

const TInt KMMBoxMarkReplacementStringLength = 2;

enum TMMBoxDialogFlags
    {
    EHelpSupported = 1,     // Set when help menu option should be visible
    ESortTypeChanged = 2,   // Set if sort type has been changed during dialog lifetime
    EDoNotDeleteOpen = 4    // Set if user has pressed arrow keys in NotViewer
    };

//  CLASS DEFINITIONS

/**
* CMMBoxDialog
* Dialog class to display MMBox View
*/
class CMMBoxDialog : 
    public CAknDialog,
    public MEikListBoxObserver,
    public MMsvSingleOpWatcher,
    public MMsvProgressDecoder,
    public MMsvSessionObserver
    {
    public:
        /**
        * C++ constructor
        */
        CMMBoxDialog( CNotMtmUi&  aUi, 
            CNotMtmUiData& aUiData, 
            CMsvSession& aSession, 
            TInt& aExitCode );

        /**
        * Destructor
        */
        ~CMMBoxDialog();

        /**
        * Two phase constructor
        * @param aUi: reference to not mtm ui object
        * @param aUiData: reference to not mtm ui data object
        * @param aSession: reference to message server session
        * @return object of CMMBoxDialog
        */
        static CMMBoxDialog* NewL( CNotMtmUi& aUi, 
            CNotMtmUiData& aUiData, 
            CMsvSession& aSession, 
            TInt& aExitCode );

        /**
        * from CEikDialog
        * Creates scrollbarframe.
        * Sets the listbox.
        * Sets the navipane text.
        * Calls UpdateMMBoxArrayL.
        */
        void PreLayoutDynInitL();

        /**
        * from CEikDialog
        * Check if quota info is available.
        * Asks user if (s)he wants the info to be updated.
        */
        void PostLayoutDynInitL();

        /**
        * from CEikDialog
        * If CEikDialog thinks it's ok to exit, calls
        * RestoreTitleTextAndNavipaneL to set the title and returns ETrue.
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
        * From CCoeControl, returns help context
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;

        /**
        * from MProgressDecoder
        */
        virtual TInt DecodeProgress (
            const TDesC8& aProgress,
            TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans, TBool aInternal );

        /**
        * from MMsvSessionObserver
        */
    	virtual void HandleSessionEventL(TMsvSessionEvent aEvent, 
            TAny* aArg1, TAny* aArg2, TAny* aArg3);

    protected:

        /**
        * From MEikListBoxObserver
        */
        virtual void HandleListBoxEventL(
            CEikListBox* aListBox, 
            TListBoxEvent aEventType );

    private:

        /**
        * Symbian OS constructor
        * Creates a message array. 
        * Asks things from Feature Manager.
        * Gets a pointer to navipane.
        */
        void ConstructL();

        /**
        * Returns object of list box
        * @return pointer to list box object.
        */
        CEikColumnListBox* ListBox() const;

        /**
        * Appends icon to icon array
        * @param aFileName: Bitmap filename with path
        * @param aIconIndex: index of the bitmap to be read
        * @param aIcons: icon array where bitmap is append
        */
        void AppendSkinnedIconL(    
            TFileName& aFileName, 
            TInt aIconIndex, 
            CArrayPtrFlat<CGulIcon>* aIcons);

        /**
         * Updates iMessages array
         */
        void UpdateMMBoxArrayL();

        /**
         * Updates navipane with the latest update date
         */
        void UpdateNavipaneL();

        /**
         * Stores current title pane text and replaces it with new one (qtn_mce_title_mms_notifs_view)
         */
        void StoreTitleTextAndNavipaneAndSetNewL();

        /**
         * Restores title pane text with the text stored in 
         * StoreTitleTextAndSetNewL.
         */
        void RestoreTitleTextAndNavipaneL();

        /**
        * Called by OpCompleted after operation is completed.
        * If open op was completed and next/prev was chosen in Notviewer,
        * this function calls HandleOpenNextPreviousL().
        * If update or Delete operations were completed, calls
        * UpdateMMBoxArrayL.
        * Returns ETrue if we should try to exit from MMBoxDialog after this function.
        * @param aOperation: pointer to operation which is completed.
        * @param aCompletionCode: Same as in OpCompleted.
        * @return ETrue if TryToExitL is called. That can happen
        *    if exit is called during Open operation and dialog is dismissed.
        */ 
        TBool DoOperationCompletedL(
            CMsvOperation* aOperation, 
            TInt aCompletionCode );

        /** 
        * Launches help application
        */
        void LaunchHelpL();
    
        /**
        * Sorts the messages again. If aType is the same as the current
        * sorting, the sorting is reserved.
        * @param aType the type of the sorting
        */
        void SortMessagesByL( TMMBoxSortType aType );

        /**
        * Opens the currently focused item in Notification Viewer
        *
        */
        void OpenNotificationL( );

        /**
        * Starts fetching messages.
        * @param aAll - If ETrue, fetches all, if EFalse, fetches selection
        */
        void FetchL( TBool aAll=EFalse );

        /**
        * Deletes messages.
        * @param aAll - If ETrue, deletes all, if EFalse, deletes selection
        */
        void DeleteL( TBool aAll=EFalse );

        /**
        * Returns selection of entries
        * @param aAll - If ETrue, returns all, if EFalse, returns selected
        *               if none are selected -> returns just the current one
        */
        CMsvEntrySelection* SelectionL( TBool aAll=EFalse );

        /**
        * Creates and shows the status info dialog.
        */
        void ShowStatusL( );

        /**
        * Creates an instance of Progress Reporter and calls Notification 
        * Client MTM's UpdateMmBoxListL function to start the update process
        */
        void UpdateListL( );

        /**
        * Creates and shows an information note with text string aResourceId
        */
        void ShowInformationNoteL( TInt aResourceID, TBool aWaiting );

        /**
        * Creates and shows a confirmation query.
        * First loads the string aResourceId from the resource file.
        * Then calls the other ShowConfirmationQuery to show the query.
        */
        TInt ShowConfirmationQueryL( TInt aResourceId, TBool aOnlyOK=EFalse ) const;

        /**
        * Creates and shows a confirmation query.
        * First loads the string aResourceId adding numbers into place holders.
        * Then calls the other ShowConfirmationQuery to show the query.
        * @param aCount - count of objects for which something happend
        * @param aTotalOnes - count of total objects
        * @param aResource - string resource to be shown
        * @return Returns the value returned by the dialog.
        */
        TInt ShowConfirmationQueryL(    TInt    aCount,
                                        TInt    aTotalOnes,                          
                                        TInt    aResource );

        /**
        * Creates and shows a confirmation query.
        * Creates a query dialog with text aText. If aOnlyOK is true, the softkeys
        * are OK-Empty. Otherwise they are Yes-No.
        * Returns the value returned by the dialog.
        */
        TInt ShowConfirmationQueryL( const TDesC& aText, TBool aOnlyOK=EFalse ) const;

        /**
         * Opens next or previous message ie. sets the current context to next/prev
         * notification and calls OpenNotificationL function.
         */
        void HandleOpenNextPreviousL( TBool aOpenNext = ETrue );

        /**
        * Shows wait note using CAknWaitDialog.
        * @param aNoteTxtResourceId
        * @param aVisibilityDelayOff, ETrue show note immediately, 
        *                             EFalse after 1.5 sec
        */
        void ShowWaitNoteLC(
            TInt aNoteTxtResourceId, 
            TBool aVisibilityDelayOff);

        /**
        * Construct and prepares wait note using CAknWaitDialog.
        * This is called from ShowWaitNoteLC variants.
        * @param aVisibilityDelayOff, ETrue show note immediately, 
        *                             EFalse after 1.5 sec
        */
        void ConstructWaitNoteLC( TBool aVisibilityDelayOff );

        /**
        * TCleanupItem to clean wait or progress dialogs if leave happens after
        * dialog creation.
        * @param aAny: pointer to dialog pointer.
        */
        static void CleanupWaitDialog(TAny* aAny);

        void ShowWaitNoteL(
            TInt aNoteTxtResourceId, 
            TBool aVisibilityDelayOff);
            
        /**
        * Sets MSK if necessary.
        */
        void SetMSKL();

    private:
        CMMBoxArray*        iMessages;
        CNotMtmUi&          iUi;
        CNotMtmUiData&      iUiData;
        CMmsNotificationClientMtm* iClientMtm;
        CClientMtmRegistry* iMtmReg;
        CMsvSession&        iSession;

        CAknNavigationControlContainer* iNaviPane; //not owned
        CAknNavigationDecorator*    iNaviDecorator; //
        CAknNavigationDecorator*    iNaviOld;
        HBufC*              iOldTitleText;
        TMsvId              iMMBoxFolderId;
        TBuf<KMMBoxMarkReplacementStringLength>  iMarkReplacementText;
        
        CMsvSingleOpWatcher* iDeleteOperation;
        CMsvSingleOpWatcher* iOpenOperation;
        CMsvSingleOpWatcherArray    iFetchOperations;
        CMsvSingleOpWatcher* iUpdateOperation;

        CMsvEntrySelection* iDeleteSelection;

        CAknWaitDialog*     iWaitDialog;
        
        TInt&               iExitCode;
        TInt                iFlags;
        TMsvSelectionOrdering iOriginalOrder;
    };

#endif // __MMBOXDLG_H__

//  End of File
