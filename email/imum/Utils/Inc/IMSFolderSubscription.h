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
* Description: 
*       folder subscription dialog
*
*/


#if !defined(__IMSFOLDERSUBSCRIPTION_H__)
#define __IMSFOLDERSUBSCRIPTION_H__

// INCLUDES
#include <msvstd.h>
#include <msvapi.h>
#include <AknForm.h> // CAknDialog
#include <eiklbo.h>
#include <gulicon.h> // CGulIcon
#include <MuiuMsvSingleOpWatcher.h>
#include "IMSSoftkeyControl.h"
#include "IMSFolderListArray.h"

//enumeration for folder icon indexes.
enum TImumFolderIconIndexes
    {
    EImumFolderSingleUnsubbed = 0,
    EImumFolderSingleSubbed,
    EImumFolderSingleSubbedNew,
    EImumFolderSubfoldersUnsubbed,
    EImumFolderSubfoldersSubbed,
    EImumFolderSubfoldersSubbedNew,
    EImumFolderUnsubbedNew,
    EImumFolderSubfoldersUnsubbedNew,
    };

class CMtmStore;
class CBaseMtmUiData;
class CBaseMtmUi;
class CAknWaitDialog;
class CMuiuFlags;
class CImumInternalApiImpl;
class MImumInMailboxUtilities;

class CAknNavigationControlContainer;
class CAknNavigationDecorator;

/**
* CImapSubscriptionDialog
* Folder subscription dialog class
*/
class CImapSubscriptionDialog :
    public CAknDialog,
    public MEikListBoxObserver,
    public MMsvSessionObserver
    {
    public:

        /**
        * NewL()
        * @param CMuiuFlags&, flags
        * @param TMsvId, folder id
        * @param TMsvId, mailbox id
        * @param TBool&, login info changed (in/out)
        * @param const TDesC8&, user name
        * @param const TDesC8&, password
        * @param CMsvSession& session reference
        * @return CImapSubscriptionDialog*
        */
        static CImapSubscriptionDialog* NewL(
            CMuiuFlags& aFlags,
            TMsvId aServiceId,
            TMsvId aMailboxId,
            TBool& aLoginChanged,
            const TDesC8& aUserName,
            const TDesC8& aPassword,
            CImumInternalApiImpl& aMailboxApi );

            ~CImapSubscriptionDialog();


        /**
        * From MMsvSessionObserver
        * HandleSessionEventL
        * @param TMsvSessionEvent, event id
        * @param TAny*
        * @param TAny*
        * @param TAny*
        */
        virtual void HandleSessionEventL(
            TMsvSessionEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 );

        /**
        * SynchroniseFoldersL()
        */
        void SynchroniseFoldersL();

        /**
        * DisplayFinalProgressDialog()
        * To deal with final progress from suboperations
        * @param CMsvOperation&
        */
        void DisplayFinalProgressDialog(
            CMsvOperation& aCompletedOperation) const;

        /**
        * from CCoeControl
        * OfferKeyEventL()
        * @param TKeyEvent&, key event
        * @param TEventCode
        * @return TKeyResponse
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);


        /**
        * From CAknDialog
        * DynInitMenuPaneL()
        * @param TInt, resource id
        * @param CEikMenuPane*
        */
        virtual void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane ) ;

        /**
        * From CAknDialog
        * ProcessCommandL()
        * @param TInt, command id
        */
        virtual void ProcessCommandL( TInt aCommandId );

        /**
        * from CCoeControl
        * ActivateL()
        */
        virtual void ActivateL();

    protected:

        /**
        * From MEikListBoxObserver
        * @param CEikListBox*
        * @param TListBoxEvent
        */
        virtual void HandleListBoxEventL(
            CEikListBox* aListBox,
            TListBoxEvent aEventType );

        /**
        * HandleSubscriptionL()
        * @param TInt type of subscription, either subscribed or unsubscribed
        * @param CMsvEntrySelection&, selection to subscribe
        */
        void HandleSubscriptionL( TInt aSubType, CMsvEntrySelection& aSelection );

        /**
        * CheckAreAllFoldersIncludedL()
        * Checks if all subfolders are either subscribed of unsubscribed
        * @param TInt, type of subscription, either subscribed or unsubscribed
        * @param CMsvEntrySelection*, selection to check
        * @return TBool, ETrue if all subfolders have same subscription state
        */
        TBool CheckAreAllFoldersIncludedL( TInt aSubType, CMsvEntrySelection* sel );

    private:

        /**
        * PreLayoutDynInitL()
        */
        virtual void PreLayoutDynInitL();

        /**
        * PostLayoutDynInitL()
        */
        virtual void PostLayoutDynInitL();

        /**
        * OkToExitL()
        * @param TInt
        * @return TBool
        */
        virtual TBool OkToExitL(TInt aButtonId);

        /**
        * C++ constructor
        * @param CMuiuFlags&, flags
        * @param TMsvId, folder id
        * @param TMsvId, mailbox id
        * @param TBool&, login info changed (in/out)
        * @param const TDesC8&, user name
        * @param const TDesC8&, password
        * @param CMsvSession& session reference
        */
        CImapSubscriptionDialog(
            CMuiuFlags& aFlags,
            TMsvId aServiceId,
            TMsvId aMailboxId,
            TBool& aLoginChanged,
            const TDesC8& aUserName,
            const TDesC8& aPassword,
            CImumInternalApiImpl& aMailboxApi );

        /**
        * ConstructL()
        */
        void ConstructL();

        /**
        * CreateOperationL()
        * @param TInt, button id
        */
        void CreateOperationL(TInt aButtonId);

        /**
        * ContextHasChildFolders()
        * @return TBool, ETrue if has children
        */
        TBool ContextHasChildFolders() const;

        /**
        * GetMtmUiL()
        */
        void GetMtmUiL();


        /**
        * AppendIconsL()
        * Appends icons to icon array
        * @param TFileName&: Bitmap filename with path
        * @param CArrayPtrFlat<CGulIcon>*: icon array where bitmap is append
        */
        void AppendIconsL(
            TFileName& aFileName,
            CArrayPtrFlat<CGulIcon>* aIcons);

        /**
        * CurrentFolderL()
        * @return TMsvId, id of current folder
        */
        TMsvId CurrentFolderL();

        /**
        * HandleRefreshL
        */
        void HandleRefreshL();

        /**
        * LaunchHelpL()
        * Launches help
        */
        void LaunchHelpL();

        /**
        * Gets the text resource for MSK, based on the selected item
        * @since S60 3.1
        * @return Text resource
        */
        TInt GetResourceForMiddlekeyL();

        /**
        * Refresh folder list array and change listbox to root view
        * @since S60 3.2
        */
        void RefreshFolderListL();

        /**
        * Set Folder Back to Folder where user was before refresh
        * @since S60 3.2
        */
        void SetBackToOldFolderL();

        /**
        * Save login information into database
        * @since S60 3.2
        */
        void SetLoginInformationL();

        /**
        * Set middle softkey label
        * @since S60 3.2
        */
        void SetMskL();


        /**
        * Set folder depth for the navi pane icons
        * @since S60 5.0
        */
        void SetFolderDepthL( TInt aDepth );

    private:

        CImumInternalApiImpl&           iMailboxApi;
        CMuiuFlags&                     iFlags;
        TMsvId                          iServiceId;
        TMsvId                          iMailboxId;
        TBool&                          iLoginChanged;
        const TDesC8&                   iUserName;
        const TDesC8&                   iPassword;

        TBool                           iLoginSaved;
        CMtmStore*                      iMtmStore;
        CBaseMtmUiData*                 iMtmUiData;
        CBaseMtmUi*                     iMtmUi;
        CMsvEntry*                      iContextEntry;
        CImapFolderListArray*           iFolderListArray;
        CIMSSoftkeyControl*             iSoftkeys;
        TMsvId                          iOpenedFolderId;
        TInt                            iPreviousSubscriptionRID;
        TInt                            iPreviousFolderOpenRID;
        MImumInMailboxUtilities*        iUtils;
        RArray< TMsvId >                iFolderIdArray;

		// depth of the folder structure
		// used for drawing the folder icons to navipane
        TInt							iDepth;
        // pointer to navipane
        CAknNavigationControlContainer* iNaviPane;
        // pointer to navi pane decorator
		CAknNavigationDecorator*        iFolderIndicator;
    };

#endif // __IMSFOLDERSUBSCRIPTION_H__
