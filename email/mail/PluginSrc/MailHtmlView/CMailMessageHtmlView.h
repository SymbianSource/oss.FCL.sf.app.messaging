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
* Description:  Message Html view
*
*/



#ifndef CMailMessageHtmlView_H
#define CMailMessageHtmlView_H

//  INCLUDES
#include <CMailMessageView.h>
#include <MSVSTD.H>
#include <e32base.h>

// FORWARD DECLARATIONS
class CEikMenuPane;
class MMailAppUiInterface;
class CHtmlLoadStateMachine;
class CImHeader;
class CMailHtmlBodyControl;


// CLASS DECLARATION

/**
*  Mail message plain view.
*  Plain text message viewer plug-in.
*  @since Series 60 3.0
*/
class CMailMessageHtmlView : public CMailMessageView
    {
    public:  // Constructors and destructor
        
        /**
        * NewL
        * Called from the ECOM plug-in framework.
        */
        static CMailMessageHtmlView* NewL();

        /**
        * Destructor.
        */
        ~CMailMessageHtmlView();


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
            CMailMessage& aMessge,
            MMailAppUiInterface& aUICallBack,
            TUint& aParam);
        		                
    protected:  // Functions from base classes

        
    private: // Implementation
        void CreateBrControlL();
        void SetHeaderControlTextL(
    		CMailMessage& aMessage, 
    		CImHeader& aHeader);
    	TBool HtmlSettingValueL( CMailMessage& aMessage );
        
        /**
        * C++ default constructor.
        */
        CMailMessageHtmlView();
    
    private:    // Data               
        /// Ref: app UI call back
        MMailAppUiInterface* iAppUI;
        /// Ref: Mail message
        CMailMessage* iMessage;
        /// Ref: HTML body control
        CMailHtmlBodyControl* iHtmlBodyControl;
        /// Own: Load state machine
        CHtmlLoadStateMachine* iLoadStateMachine;
        

    };

#endif      // CMailMessageHtmlView_H   
            
// End of File
