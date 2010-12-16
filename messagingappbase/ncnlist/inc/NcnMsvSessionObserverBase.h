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
* Description:   Defines an abstract class CNcnMsvSessionObserverBase.
*
*/



#ifndef NCNMSVSESSIONOBSERVERBASE_H
#define NCNMSVSESSIONOBSERVERBASE_H

//  INCLUDES
#include    <msvapi.h>              // MsvSession class
#include    "MNcnMsvSessionHandlerObserver.h"

// CONSTANTS
const TUid KNcnSmsUid = { 0x1000102c }; 

// FORWARD DECLARATIONS
class CNcnModelBase;
class CMsvSession;
class CNcnHandlerAudio;
class TVwsViewId;
class CNcnNotifApiObserver;
class CNcnMsvSessionHandler;
class CVoiceMailManager;

// CLASS DECLARATION

/**
*  Receives and handles In-box events.
*/
class CNcnMsvSessionObserverBase : 
    public CBase, public MMsvEntryObserver,
    public MNcnMsvSessionHandlerObserver
    {
    public:  // Constructors and destructor

        /**
        * Class factory.
        */
        static CNcnMsvSessionObserverBase* NewL( 
            CNcnModelBase* aModel, CVoiceMailManager& aVMManager );

        /**
        * Destructor.
        */
        virtual ~CNcnMsvSessionObserverBase();


    public: // From MNcnMsvSessionHandlerObserver
    
        virtual void HandleMsvSessionReadyL( CMsvSession& aMsvSession );
        virtual void HandleMsvSessionClosedL();
		//If new msvsession starts
        void HandleMsvSessionReReadyL(CMsvSession& aMsvSession);
        
    public: // Functions from base classes

        /**
        * From MMsvEntryObserver. Callback function. Handles global In-box 
        * events. This is a pure virtual function must be implemented in 
        * protocol specific subclasses.
        * @param aEvent An entry event.
        */
        virtual void HandleEntryEventL( 
            TMsvEntryEvent aEvent, 
            TAny* aArg1, 
            TAny* aArg2, 
            TAny* aArg3 ) = 0;
        
    protected:
        
        /**
        * C++ default constructor.
        */
        CNcnMsvSessionObserverBase(
            CNcnModelBase* aModel );

        /**
        * By default Symbian OS constructor is private.
        */
        virtual void ConstructL();
        
        /**
        * Checks the amount of unread messages in the global In-box.
        * This is a pure virtual function must be implemented in protocol specific subclasses.
        */
        virtual void CheckAmountOfUnreadMessagesInInboxL( TInt& aMsgCount, TInt& aAudioMsgCount ) = 0;         

    protected: //data
        
        // A pointer to the event monitor object.
        CNcnModelBase*  iModel;
        
        // In-box folder entry. Note that the entry is not owned by this class.
        CMsvEntry* iInboxFolder;
        
};

#endif      // NCNMSVSESSIONOBSERVERBASE_H

// End of File
