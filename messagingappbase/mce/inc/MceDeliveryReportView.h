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
*     Messaging Centre application "Delivery Reports" view class
*     implementation.
*
*/



#ifndef     __Mce_CMceDeliveryReportView_H__
#define     __Mce_CMceDeliveryReportView_H__

//  INCLUDES
#include <coemain.h>
#include <eikfrlb.h>
#include <eikmenub.h>
#include <aknview.h>
#include <e32std.h>

#include <msvapi.h>

// CONSTANTS

_LIT( KHelpContext, "MCE_HLP_DELIVERY_REPORTS" );

// FORWARD DECLARATIONS
class CMceUi;
class CEikMenuBar;
class CMceSessionHolder;


class CMceDeliveryReportControlContainer;

// CLASS DECLARATION

/**
 * Mce application "Delivery Report" view class implementation.
 */
class   CMceDeliveryReportView :
        public CAknView,
        public MEikListBoxObserver,
        public MCoeForegroundObserver
        
    {
    public:  // interface
        /**
         * Standard creation function. Creates and returns a new object of this
         * class.
         *
         * @return pointer to CMceDeliveryReportView instance.
         */
        static CMceDeliveryReportView* NewL( CMceSessionHolder& aSessionHolder );
        /**
         *  Destructor.
         */
        ~CMceDeliveryReportView();

        /**
         * Returns the Mce Application UI object.
         *
         * @return Pointer to Application UI
         */
        CMceUi* MceAppUi();

        /**
         * HandleClientRectChange. Called by the framework the view rectangle
         * changes
         */
        void HandleClientRectChange();
        
    public:  // from CAknView
        /**
         * Framework calls to init menu items.
         *
         * @param aResourceId   resource id
         * @param aMenuPane     menu pane
         */
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

    protected: // from MEikListBoxObserver
        /**
         * Handles list boxes key events.
         *
         * @param aListBox   listbox. Not used.
         * @param aEventType event type
         *                   This event type is specified by
         *                   MEikListBoxObserver class.
         */
        void HandleListBoxEventL(CEikListBox* aListBox,
                                 TListBoxEvent aEventType);

    private:  // from CAknView
        /**
         * Returns the ID of the view.
         *
         * @return id of this view.
         */
        TUid Id() const;

        /**
         * Called by the framework when this view is activated.
         *
         * @param aPrevViewId      This is ID for previous view.
         * @param aCustomMessageId ID of the custom message. Not used.
         * @param aCustomMessage   custom message. Not used.
         */
        void DoActivateL(const TVwsViewId& aPrevViewId,
                         TUid aCustomMessageId,
                         const TDesC8& aCustomMessage);
        /**
         * Called by the framework when this view is deactivated.
         */
        void DoDeactivate();


    public: // from MEikCommandObserver
        /**
         * HandleCommandL. Handles the view specific commands.
         *
         * @param aCommandId Command's id
         */
        void HandleCommandL(TInt aCommand);

    public: // MCoeForegroundObserver

        /**
         *  It is called whenever observer has gained foreground
         */
        void HandleGainingForeground();

        /**
         *  It is called whenever observer has lost foreground
         */
        void HandleLosingForeground();

    private:  // operations
        /**
         * C++ constructor
         *
         */
        CMceDeliveryReportView( CMceSessionHolder& aSessionHolder );
        /**
         * Constructor, second phase.
         */
        void ConstructL();

    public:
        /**
         *  Creates a pop up menubar of aResourceId. Runs DoLaunchPopupL
         *  inside a TRAP.
         *
         *  @param aResourceId Resource ID.
         */
        void LaunchPopupMenuL( TInt aResourceId );

        /**
         * Removes the popup from the view stack and deletes it.
         */
        void ClosePopup();

        /**
         * From MEikCommandObserver
         */
        virtual void ProcessCommandL( TInt aCommand );

        /**
         * Set the middle soft key.
         *
         */
        void SetMSKButtonL();

    private:  // functions
        /**
         * Launch the popup.
         *
         * @param aResourceId Resource ID.
         */
        void DoLaunchPopupL( TInt aResourceId );

    private:  // data
        /// Own: This view's control container.
        CMceDeliveryReportControlContainer* iContainer;

        /// Own: popup menu
        CEikMenuBar* iPopup;

        
        CMceSessionHolder& iSessionHolder;
    };


#endif  // __Mce_CMceDeliveryReportView_H__


// End of File
