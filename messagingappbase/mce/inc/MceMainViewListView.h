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
*     Main view of the Mce.
*
*/



#ifndef MCEMAINVIEWLISTVIEW_H
#define MCEMAINVIEWLISTVIEW_H

//  INCLUDES
#include <aknview.h>    // CAknView
#include <msvapi.h>

#include <AiwServiceHandler.h>

//#include "MceBaseListView.h"
#include "MceListItem.h"
#include "MceMainViewListItemArray.h"

#include "mcetemplate.h"

#include <bldvariant.hrh>

#include <ImumInHealthServices.h>

// FORWARD DECLARATIONS

class CMsvSession;
class CMceUi;
class CMceSessionHolder;
class CMceBitmapResolver;
class CMceMainViewEmptyListContainer;
class CMceMainViewListContainer;

// CLASS DECLARATION

/**
*  Main view of the Mce.
*/
class CMceMainViewListView :
    public CAknView,
    public MMceMainViewListItemArrayObserver,
    public MEikListBoxObserver,
    public MEikScrollBarObserver,
    public MMsvSessionObserver
    {
    public:
        
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMceMainViewListView* NewL(
            CMsvSessionPtr aSession,
            CMceSessionHolder& aSessionHolder,
            CMceBitmapResolver& aBitmapResolver );

        /**
        * Destructor.
        */
        virtual ~CMceMainViewListView();

    public: // new function

        /**
        * Sets item id (from msgs) to be selected when mce main view is activated.
        * Called by message list view when closing.
        * @param: aFolderId: id of the folder to be selected
        */
        void SetSelectedFolderId( TMsvId aFolderId );

        /**
        * Sets selection so that Delivery reports item is selected next time
        * main view is activated.
        */
        void SetDeliveryReportsSelected( );

        /**
        * Set special MSK button
        */
        void SetSpecialMSKButtonL(TInt aResourceId);        

        /**
        * Should be called by mce after message server startup.
        * This adds view to session observer list and creates needed CMsvEntries.
        */
        virtual void HandleMsgServerStartupL();

        /**
        * Returns pointer to list container.
        * @return Pointer to list container
        */
        CMceMainViewListContainer* ListContainer() const;
        
        /**
        * 
        */
        void HandleClientRectChange();        
        
        /**
        * Reconstructs main view folder list
        */
        void ConstructMainViewL();
        
    public:     // Functions from base classes
    
    
        /**
        * From CAknView
        */
        TUid Id() const;

        /**
        * From CAknView
        */
        void DoActivateL(
            const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );

        /**
        * From CAknView
        */
        void DoDeactivate();

        /**
        * From CAknView
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * From MEikCommandObserver
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From MEikCommandObserver
        */
        virtual void ProcessCommandL(TInt aCommand);

        /**
        * From MMsvSessionObserver
        */
        void HandleSessionEventL(
            TMsvSessionEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 );

        /**
        * From MMceMainViewListItemArrayObserver
        */
        virtual void MceListItemArrayChangedL( );
        
        /**
         * Called Main view refresh is happening 
         * and need to know flick is ON\OFF
         * @return ETrue if flick is ON 
         * otherwise OFF 
         */
        TBool MceListItemArrayFlickEvent();

    protected:  // Functions from base classes

        /**
        * From MEikListBoxObserver
        */
        void HandleListBoxEventL(
            CEikListBox* aListBox,
            TListBoxEvent aEventType);
			
    protected:  // Functions from base classes

        /**
        * From MEikScrollBarObserver
        */
        void HandleScrollEventL(
             CEikScrollBar* aScrollBar, 
             TEikScrollEvent aEventType);

    private:

        /**
        * Constructor is private.
        */
        CMceMainViewListView(
            CMsvSessionPtr aSession,
            CMceSessionHolder& aSessionHolder,
            CMceBitmapResolver& aBitmapResolver );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * When user wants to open some folder then this is called.
        * Handles folder opening and extra opens such as CBS launching, creating
        * remote mailbox if not mailboxes.
        */
        void ChangeViewL();

        /**
        * Adds extra items to the list array:
        * "Write message", "Delivery reports", "No remote mailboxes"
        */
        void AddExtraItemsL();

        /**
        * Creates containers etc. after msv session is ready.
        */
        void CreateContainerAndActivateL();

        /**
        * Finds changed folder's index in main view list.
        * For example, if documents\subfolder is change then this finds an
        * index for the documents and changes its bitmap index to KErrNotFound
        * @param aChangedFolderId: id of the changed folder.
        */
        void CheckMainViewBitmapL( TMsvId aChangedFolderId ) const;

        /**
        * Changes all the bitmap indexes in mainview to KErrNotFound.
        */
        void ResetBitmapsL() const;
        
        /**
        * Starts application when extra item is selected from the 
        * main view list.
        */
        void StartAppL( TInt aCommandId );

        /**
        * Counts how many mailboxes with unread messages there is
        * @param aFirstMailboxId: id of the first mailbox with unread messages, 
        *    0 if no mailboxes with unread messages
        * @return Number of mailboxes with unread messages
        * @since 3.1
        */
        TInt MailboxWithUnreadMessagesL( TMsvId& aFirstMailboxId );

		/**
		* Input parameter list for AIW plugin
		*/
        CAiwGenericParamList* AiwSyncParamListLC(TInt aId, const TDesC& aText);

        /**
         * Show a confirmation note to the user after successful 
         * setting of the default view
         */
        void ShowConfirmationNoteL( TInt aResourceID );
       
        /**
         *  Set the default view settings    
         */
        TBool SetMceDefaultViewL(TBool aVal);

        /**
         *  Get the current default view settings    
         */
        TInt GetMceDefaultViewL();

    private:    // Data
        CMceMainViewListContainer*      iMsgListContainer;

        CMsvSessionPtr          iSession;
        CMsvEntry*              iFolderEntry;

        CMceSessionHolder&      iSessionHolder;

        CMceBitmapResolver&     iBitmapResolver;

        TBool                   iMceViewActivated;
        TBool                   iMsvSessionReady;
        
        CAiwServiceHandler*		iAiwServiceHandler;
        TMceFlags               iListViewFlags;

        TMsvId 					iCurrentItemId;

        CMceUi*                 iMceUi;

        TInt    iSelectedIndex; // selection before deactivated
        TInt    iTopItemIndex; // top item before deactivated
        TMsvId  iSelectionFolderId;
        TBool   iDrawListbox;
        CMceMainViewEmptyListContainer* iEmptyListContainer;
        TInt    iPerformOperationAfterMsgStart; // 0 when do nothing, KMceShowMail or KMceShowAudioMessage if activated before msg started.
        // ETrue if MCE integration feature is ON. In practice, this means
        // that S60 mailboxes are hided from MCE main view
        TBool iEmailClientIntegration;
       
        // ETrue if Selectable Default Email feature is on
        TBool iSelectableEmail;

        // Id of the email application (other than platform email application)
        TInt iMtmPluginId;
        
        /**
         * CMail Changes, it will be True if 
         * KFeatureIdFfEmailFramework is enabled
         */
        TBool  iEmailFramework;        
        /**
         * Flag that shows Flicking status in Main View
         * ETrue if ON
         * otherwsie EFalse
         */
        TBool                           iFlick; 
    };

#endif

// End of file
