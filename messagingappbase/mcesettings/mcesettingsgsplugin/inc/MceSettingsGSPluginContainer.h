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
* Description:   Messaging settings plugin container 
*
*/



#ifndef MCESETTINGSGSPLUGINGCONTAINER_H
#define MCESETTINGSGSPLUGINGCONTAINER_H

// INCLUDES
// System includes
#include <gsbasecontainer.h>    // CGSBaseContainer
#include <msvapi.h>             // TMsvSessionEvent

// FORWARD DECLARATIONS
class CMceSettingsSessionObserver;
class CMtmStore;
class CMceUidNameArray;
class CMsvSession;
class CMtmUiDataRegistry;
class CUidNameArray;
class CGSBaseContainer;

// CLASS DECLARATION

/**
*  CMceSettingsGSPluginContainer class 
*  @since Series60_5.0
*/
class CMceSettingsGSPluginContainer : 
    public CGSBaseContainer,
    public MMsvSessionObserver
    {
    public: // Constructors and destructor
        
        /**
        * Default C++ constructor
        */        
        CMceSettingsGSPluginContainer();

        /**
        * Destructor.
        */
        ~CMceSettingsGSPluginContainer();
    
    private: // Functions from base classes


TBool IsSettingsInUse();
        
        /**
        * From CGSBaseContainer
        * See base class.        
        */    
        void ConstructListBoxL( TInt aResLbxId );

        /**
        * From CGSBaseContainer
        * See base class.        
        */    
        TKeyResponse OfferKeyEventL( 
            const TKeyEvent& aKeyEvent,
            TEventCode aType );        

        /**
        * From CCoeControl
        * See base class.        
        */ 
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        
    public:
        /**
        * From MMsvSessionObserver
        */
        virtual void HandleSessionEventL(
            TMsvSessionEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3);
                
   public: // new functions

        /**
        *
        */
        void EditSubSettingsL();
        
        
    private: // Constructor
    
        /**
        * Symbian OS constructor.
        *
        * @param aRect Listbox's rect.
        */
        void ConstructL( const TRect& aRect );
        
        
    private: // New functions
        /**
        *
        */
        void UpdateSettingItemsL();
        
        /**
        * Loads mtm
        * @param aMtmType: mtm type
        * @param aMtmStore: mtm store 
        */
        void LoadMtmL( const TUid aMtmType, CMtmStore& aMtmStore );

        /**
        * Changes array order: sms, mms, email, others...
        */
        void SortAndChangeSmsMmsEmailFirstL();

        /**
        * Places given mtm first in the array.
        * @param aArray: reference to array
        * @param aMsgType: mtm to changed to the first.
        */
        void ChangeMsgTypeTopL( CUidNameArray& aArray, TUid aMsgType ) const;
        
        /**
        * Checks CSP Cell Broadcast bit from SIM      
        */
        TBool CheckCspBitL() const;
        
    private: // Data
        CMsvSession*                    iMsvSession; // not owed by this class.        
        CMceUidNameArray*               iMsgTypesSettings;
        CMceSettingsSessionObserver*    iAccountManager;
        TBool														iEmailFramework;
        TBool                           iCbs;
        TBool							iDialogOpen; // To verify dialog is already open

    };

#endif // MCESETTINGSGSPLUGINCONTAINER_H

// End of File
