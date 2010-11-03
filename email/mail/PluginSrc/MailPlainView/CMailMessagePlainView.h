/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Message plain view
*
*/



#ifndef CMailMessagePlainView_H
#define CMailMessagePlainView_H

//  INCLUDES
#include "MailLog.h"
#include <MMailMessageView.h>
#include <CMailMessageView.h>
#include <msvstd.h>


// FORWARD DECLARATIONS
class CEikMenuPane;
class MMailAppUiInterface;
class CMailMessage;
class CImHeader;
class CLoadStateMachine;
	
/**
*  Mail message plain view.
*  Plain text message viewer plug-in.
*  @since Series 60 3.0
*/
class CMailMessagePlainView : public CMailMessageView
    {
    public:  // Constructors and destructor
        
        /**
        * NewL
        * Called from the ECOM plug-in framework.
        */
        static CMailMessagePlainView* NewL();

        /**
        * Destructor.
        */
        ~CMailMessagePlainView();


    private: // Functions from base classes

        /**
        * From MMailMessageView
        */
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
        TBool HandleCommandL(TInt aCommand);
        TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType);
        void MessageLoadL(TInt aState, CMailMessage& aMessage);
        void ViewMessageL(
            CMailMessage& aMessage, 
            MMailAppUiInterface& aUICallBack,
            TUint& aParams);
        
    protected:  // Functions from base classes
    	// From MMailMessageView
    	const TDesC& ResourceFile();
        
        
    private: // Implementation
        void AddCcAndBccConstrolsL( CImHeader& aHeader );
        void SetBodyControlTextL( CMailMessage& aMessage );
        void SetHeaderControlTextL( 
        	CMailMessage& aMessage, CImHeader& aHeader );
        
        /**
        * C++ default constructor.
        */
        CMailMessagePlainView();
    
    private:    // Data        
        /// Ref: app UI call back
        MMailAppUiInterface* iAppUI;
        /// Own: load state machine
        CLoadStateMachine* iLoadStateMachine;
        // For Unit Test
        UNIT_TEST(T_Plugin_Src)
    };

#endif      // CMailMessagePlainView_H   
            
// End of File
