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
*      Sms Viewer
*
*/



#ifndef MSGSMSVIEWERCDMAAPPUI_H
#define MSGSMSVIEWERCDMAAPPUI_H

//  INCLUDES
#include <msgeditorappui.h>             // CMsgEditorAppUi
#include <msgbiocontrolobserver.h>
#include <RPbkViewResourceFile.h>
#include <PhCltTypes.h>                 // PhCltTypes
#include "msgsmsviewerpan.h"            // panic-codes

#include <bldvariant.hrh>



// CLASS DECLARATION

/**
*  CMsgSmsViewerCDMAAppUi
*  Inherited from CMsgEditorAppUi
*/
class CMsgSmsViewerCDMAAppUi : public CMsgSmsViewerAppUi
    {
    public: 
        enum  TSMSViewerFieldLength
			{
            ESMSViewerCallbackNumberLength = 256,
            ESMSViewerEmailAddressLength = 60
			};  
        // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMsgSmsViewerCDMAAppUi();

        /**
        * Destructor.
        */
        virtual ~CMsgSmsViewerCDMAAppUi();

    private:

         /**
        * From MMsgEditorLauncher 
        */
        void LaunchViewL();

        /**
        * Separate the SmsBasedMsgHandlingL from LaunchViewL to have own function for Sms based msg handling. 
        * @param BioType of the TMsvEntry in question
        * @param aStore current message entry's msg store
        */
        void SmsBasedMsgHandlingL( TInt32 aBioType, CMsvStore* aStore );
        
        /**
        * From CCoeAppUi 
        * @param aKeyEvent look from CCoeAppUi
        * @param aType look from CCoeAppUi
        * @return TKeyResponse look from CCoeAppUi
        */
        TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

        /**
        * The non-bio dynamic handling of opening options-menu
        * @param aMenuPane pointer to CEikMenuPane
        */
        void DynInitNonBioOptionsMenuL( CEikMenuPane* aMenuPane );

        /**
        * The non-bio dynamic handling of opening context-menu
        * @param aMenuPane pointer to CEikMenuPane
        */
        void DynInitNonBioContextMenuL( CEikMenuPane* aMenuPane );
 
		/**
        * From CEikAppUi 
        * @param aCommand look from CEikAppUi
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From MEikMenuObserver 
        * @param aMenuId look from MEikMenuObserver
        * @param aMenuPane look from MEikMenuObserver
        */
        void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

         /**
        * Handles SEND-key
        */       
        void DoHandleSendKeyL();

        /**
        * Set the state of automatic highlighting
        * @param aSwitchON, ETrue if automatic highlighting is to used, otherwise EFalse
        */
        void SetAutomaticHighlightL( const TBool aSwitchON );

         /**
        * Extract all the calling information from message
        */       
        void ExtractCallingInfoL();

    private:
        TBuf< ESMSViewerEmailAddressLength > iEmailAddress;   // Sender' email address
        TBuf< ESMSViewerCallbackNumberLength >    iCallbackNumber;  // Callback number
        TBool                               iEmailFeatureSupported;
        TBool                               iValidCallbackNumber;
        TBool                               iValidEmailAddress;
        TBool                               iIsFromEmail;
    };

#endif // MSGSMSVIEWERAPPUICDMA_H

//  End of File
