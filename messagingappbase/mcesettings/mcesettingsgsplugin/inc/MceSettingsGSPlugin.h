/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Defines class for Messaging Settings GS plugin
*
*/



#ifndef MCESETTINGSGSPLUGIN_H
#define MCESETTINGSGSPLUGIN_H

// Includes
#include <gsbaseview.h>         // CGSBaseView

// Classes referenced
class CMuiuLock;
class CGSBaseView;
class CAknLocalScreenClearer;

// Constants

/**
* This UID is used for both the view UID and the ECOM plugin implementation
* UID.
*/
const TUid KGSMceSettingsGSPluginUid = { 0x10282EE0 };

// CLASS DECLARATION

/**
* Header of CMceSettingsGSPlugin which implements the CGSPluginInterface.
*/
class CMceSettingsGSPlugin :
    public CGSBaseView,
    public MCoeViewDeactivationObserver
    {
    public:
        /**
        * NewL
        * Symbian OS two-phased constructor
        * @return CMceSettingsGSPlugin*, self pointer
        */
        static CMceSettingsGSPlugin* NewL( TAny* aAppUi );

        /**
        * Destructor
        */
        virtual ~CMceSettingsGSPlugin();

    private: // Functions from base classes

        /**
        * From CAknView, returns the views id.
        * See base class.
        * @since 5.0
        */
        TUid Id() const;

        /**
        * From CAknView
        * See base class.
        * @since 5.0
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
        * From CAknView Deactivate this view
        * See base class.
        * @since 5.0
        */
        void DoDeactivate();

         /**
        * From MEikCommandObserver, handles the menu based commands.
        * See base class.
        * @since 5.0
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CGSBaseView
        * See base class.
        * @since 5.0
        */
        void NewContainerL();

        /**
        * Handles OK key press.
        * @since 5.0
        */
        void HandleListBoxSelectionL();

        /**
        * From CGSPluginInterface
        * See base class.
        * @since 5.0
        */
        void GetCaptionL( TDes& aCaption ) const;

        /**
        * From CGSPluginInterface
        * See base class.
        */
        TInt PluginProviderCategory() const;

        /**
        * From CGSPluginInterface
        * See base class.
        * @since 5.0
        */
        CGulIcon* CreateIconL( const TUid aIconType );

        /**
        * From MEikMenuObserver
        * See base class.
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * From MCoeViewDeactivationObserver
        * See base class.
        */
        void HandleViewDeactivation (
            const TVwsViewId& aViewIdToBeDeactivated,
            const TVwsViewId& aNewlyActivatedViewId );

        /**
        * From MEikListBoxObserver
        * See basc class
        */
        virtual void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType );
        
    private: // Constructors

        /**
        * CMceSettingsGSPlugin
        */
        CMceSettingsGSPlugin();

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

    private: // Data
        CMuiuLock*                      iSettingsLock;
        CAknLocalScreenClearer*         iLocalScreenClearer;
    };

#endif    // MCESETTINGSGSPLUGIN_H
// End of File
