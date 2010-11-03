/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Defines the CMsgIndicatorPluginImplementation class
*
*/




#ifndef CMSGINDICATORPLUGINIMPLEMENTATION_H
#define CMSGINDICATORPLUGINIMPLEMENTATION_H

// Until aknstylusactivatedpopupcontent.h is fixed

//  INCLUDES
#include <AknLayout2ScalableDef.h>
#include <AknIndicatorPlugin.h> // CAknIndicatorPlugin
#include <AknPreviewPopUpController.h> // Controlling the preview pop-up component
#include <aknstylusactivatedpopupcontent.h> // Stylus activated pop-up conten
#include <vmnumber.h> // TPhCltTelephoneNumber, RVmbxNumber
#include <etelmm.h> // RMobilePhone
#include "MsgWaitingObserver.h" // Observer

class TAknWindowComponentLayout; 
class CAknIndicatorPlugin;
class CAknIndicatorPopupContent;
class TAknWindowComponentLayout;
class CMsgIndicatorPluginVoiceMail;
/**
*  MSGIndicator Plug-in implementation
*
*  @lib MsgIndicatorPlugin
*  @since 5.0
*/
class CMsgIndicatorPluginImplementation : public CAknIndicatorPlugin,
                                         public MAknPreviewPopUpObserver,
                                         public MEikCommandObserver
                                         
    {
    private:
    
        // Voicemail lines
        enum TMsgLine
            {
            EMsgIndicatorGeneral = 0,
            EMsgIndicatorLine1 = 1,
            EMsgIndicatorLine2 = 2,
            EMsgIndicatorLineBoth = 3
            };
        // PopUpStrings
        enum TMsgStrings
            {
            ENewItemInInbox = 0, //"1 New message"
            ENewItemsInInbox, //"%N New messages"
            ENewItemInOutbox, //"1 message in outbox"
            ENewItemsInOutbox, //"%N msgs in outbox"
            ENewOtherMessages, //"New msgs available"
            
            ENewEmail, //"New e-mail"
            ENewVoiceMail, //"New voice msg"
            ENewVoiceMails, //"%N new voice msgs"
            ENewUnkVoiceMails, //"New voice msgs"
            ENewVoiceMailOnLine1, //"New msg, line 1"

            ENewVoiceMailsOnLine1, //"%N new msgs, line 1"
            ENewUnkVoiceMailsOnLine1, //"New msgs, line 1"
            ENewVoiceMailOnLine2, //"New msg, line 2"
            ENewVoiceMailsOnLine2, //"%N new msgs, line 2"
            ENewUnkVoiceMailsOnLine2, //"New msgs, line 2"

            ENewFax, //"1 New fax"
            ENewFaxes, //"%N New faxes"
            ENewUknFaxes,//"New Faxes"
            
            ESimMemoryFull //"Sim memory full"
            };

        
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMsgIndicatorPluginImplementation* NewL();

        /**
        * Destructor.
        */
        ~CMsgIndicatorPluginImplementation();

    private: // From CAknIndicatorPlugin
        
        /**
         * @see CAknIndicatorPlugin
         */
        void HandleIndicatorTapL( const TInt aUid );

        /**
         * @see CAknIndicatorPlugin
         */
        HBufC* TextL( const TInt aUid, TInt& aTextType );
        
    public: // From MAknPreviewPopUpObserver
   
        /**
         * @see MAknPreviewPopUpObserver
         */
        void HandlePreviewPopUpEventL(
            CAknPreviewPopUpController* aController,
            TPreviewPopUpEvent aEvent );

    public: // From MEikCommandObserver
    
        /**
         * @see MEikCommandObserver
         */
    	void ProcessCommandL( TInt aCommandId );
    	
    public:    	

    	/**
         * The current state of the alternative line subscription 
         * support
         * @return ALS status
         */
        TBool IsALSSupported();
    	
    private:

        /**
        * C++ default constructor.
        */
        CMsgIndicatorPluginImplementation();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // new functions

        /**
         * Create singular or plural content message for the popup
         * @return constructed text
         * 
         * NOTE! Ownership of the text is transferred to caller.
         */
        HBufC* CreatePopupContentEnvelopeMessageL(TBool& isMsgCountZero);

        /**
         * Create singular or plural content message for the popup
         * @return constructed text
         * 
         * NOTE! Ownership of the text is transferred to caller.
         */
        HBufC* CreatePopupContentFaxMessageL();

        /**
         * Create singular or plural content message for the popup
         * @return constructed text
         * 
         * NOTE! Ownership of the text is transferred to caller.
         */
        HBufC* CreatePopupContentOutboxMessageL(); 

        /**
         * Create singular or plural content message for the popup
         * @return constructed text
         * 
         * NOTE! Ownership of the text is transferred to caller.
         */
        HBufC* CreatePopupContentOtherMessageL();

        /**
         * Create singular or plural content message for the popup
         * @return constructed text
         * 
         * NOTE! Ownership of the text is transferred to caller.
         */
        HBufC* CreatePopupContentEmailMessageL();

        /**
         * Create singular or plural content message for the popup
         * @return constructed text
         * 
         * NOTE! Ownership of the text is transferred to caller.
         */
        HBufC* CreatePopupContentVoiceMailGeneralL();

        /**
         * Create singular or plural content message for the popup
         * @return constructed text
         * 
         * NOTE! Ownership of the text is transferred to caller.
         */
        HBufC* CreatePopupContentVoiceMailLineOneL();

        /**
         * Create singular or plural content message for the popup
         * @return constructed text
         * 
         * NOTE! Ownership of the text is transferred to caller.
         */
        HBufC* CreatePopupContentVoiceMailLineTwoL();
        
        /**
         * Check the voicemailbox number, if number is not defined
         * it is asked.
         * @param aLine: Linenumber of voicemail.
         */
        void CheckVMBNumberAndCallL( TMsgLine aLine );

		void LaunchNewVoiceMailAppL(TInt aType) ;
		void LaunchViewL(const TVwsViewId& aViewId, TUid aCustomMessageId, 
        const TDesC8& aCustomMessage);

        /**
         * Make a call to the number. Must be a valid phonenumber.
         * Otherwise does not do nothing
         * @param aTelNumber: A telephone number to be called.
         */
        void CallL( TPhCltTelephoneNumber aTelNumber );

		/**
        * Find out is alternative line subscription active in the terminal
		*/
        TBool UpdateALSStatus();
        
        /**
        * Launches voice mailbox application
        */
        void LaunchVmBxAppL ();

    private: // data

        // The current state of the ALS support
        TBool iIsALSSupported;

        RMobilePhone iMobilePhone;

        CWaitingObserver *iObserver;

        // String array        
        CDesCArrayFlat*  iMessagesStrings;
        
        //CRepository
        CRepository* iStorage;
        
        //RVmbxNumber
        RVmbxNumber iVmbx;
        
        //is vmbx Open allready
        TBool iSVmbxOpen;

        // central repository
        CRepository*   iRepository;
    };

#endif      // CMSGINDICATORPLUGINIMPLEMENTATION_H

// End of File
