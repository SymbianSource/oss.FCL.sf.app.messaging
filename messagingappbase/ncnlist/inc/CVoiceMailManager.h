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
* Description:   Handles the notifications for voice mail messages
*			   : Keeps track about the number of voice mail messages
*			   : Has support for alternative line (ALS)
*
*/



#ifndef NCNVOICEMAILMANAGER_H
#define NCNVOICEMAILMANAGER_H

// INCLUDES
#include "NcnSubscribeHandler.h"
#include "MNcnMsgWaitingManager.h"

// FORWARD DECLARATIONS
class CNcnSubscriber;
class CNcnCRHandler;
class MNcnNotifier;

// CLASS DECLARATION

/**
* Class implementing a voice mail management responsibility
* All the methods since 3.1 if other not specified 
* @since Series60 3.1
*/
class CVoiceMailManager : public CBase, public MNcnSubscribeHandler
{
    public:
    
    /**
     * Voice mail count can be a number telling the exact count of the messages
     * if the exact count is not known ECountNotKnown is used.
     * ENoVoiceMails is used to signify that no voice mails exists.
     */
    enum TVoiceMailCount
        {
        /* No voice mails exist */
    	ENoVoiceMails = 0,
    	
        /* Maximum number of voice mails */
    	EVMMaximumNumber = 0xfe,
    	
    	/* Voice mails exist but the exact count is not known */
    	EVMExistsButCountNotKnown = 0xff
        };
            
        /**
         * Two-phased constructor.
         */
        static CVoiceMailManager* NewL( CNcnModelBase& aModel );

        /**
         * Destructor.
         */
        ~CVoiceMailManager();

        /**
         * Received a voice mail message to line 1
         * Messages are SET(default) and CLEAR message. There is three ways
         * how voice mail messages can arrive. CPHS, DCS and "special SMS"
         * these messages are handled by NCN session observer. 
         * Use TVoiceMailCount values in case the message is a clear message or
         * the amount is not exactly know. Otherwise give the exact count.
		 * @param aAmount How many voice mails we received to line 1
         */
        void VMMessageReceived( MNcnMsgWaitingManager::TNcnMessageType aLineType, TUint aAmount );
 
        /**
         * Check the SIM status at the boot up phase. If the SIM contains
         * voice mails the PS key has been updated by some other component.
         * The internal number that we maintain about the voice message count
         * is now over written by a general notification about "New voice mail(s)"
         * Reason for this is that SIM card does not apparantly hold the exact amount
         * of received voice mails so only a general notification is desired.
         * @return True if SIM contains voice mail messages
         */
        TBool CheckForSIMVoiceMailMessages();
        
        /**
         * SIM has changed so we will need to erase old notes.
         * Model uses this method to notify voice mail manager
         * We also use this in the constructL if we notice that
         * the SIM has changed in boot-up
         */
		void NotifyAboutSIMChange();
		
        /**
         * Check the KMuiuSupressAllNotificationConfiguration value
         */		
		TBool CheckSupressNotificationSettingL();
		
    private:

        /**
         * C++ default constructor.
         */
        CVoiceMailManager( CNcnModelBase& aModel );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

	public: //from MNcnSubscribeHandler

		/**
        * Through this method the Property subscribers notify of Property changes
        * @param aCategory The category of the property to be observed
		* @param aKey The subkey of the property to be observed
		*/
        void HandlePropertyChangedL( const TUid& aCategory, TInt aKey );
        
        /**
         * ALS indication and note update
         * 
         * Indication and note must be updated for line 1 and line 2
         * whenever state for either of the line changes.
         * 
         */
        void UpdateNoteAndIndicationWithALS( TInt aMsgOnLine1,  TInt aMsgOnLine2 );
        
		/**
        * Update soft notification and trigger icon drawing to
        * reflect the current state of the lines
		*/
        void UpdateVMNotifications();


    private: // Own methods
    
        /**
        * Clear existing voice mail notes
        */
        void ClearVoiceMailSoftNotes();     

		/**
        * Find out is alternative line subscription active in the terminal
		*/
        TBool UpdateALSStatus();
        
    private: // data

    	/**
         * The current state of the ALS support
         */
        TBool iIsALSSupported;
        
        
		/* 107-18696: Handling of EF-CFIS and EF-MWIS in telephony area
		 * No need to keep voice mail count anymore. ETEL MM will take 
		 * care of that.
		 */
    	/**
         * Number of voice mails we have active in line 1
         */
        //TUint iVoiceMailCountInLine1;

    	/**
         * Number of voice mails we have active in line 2
         */
        //TUint iVoiceMailCountInLine2;
              
    	/**
         * Value of the PS key KNcnVoiceMailStatusValue, which
         * is currently mapped to KPSUidVoiceMailStatusValue
         */
        //TInt iPSKeyForVoiceMailStatus;

     	/**
         * CR keys are updated through the ncn CR handler
         * Not owned only referenced.
         */       
        //CNcnCRHandler& iNcnCRHandler;
        
     	/**
         * Used to display soft notification
         * Not owned only referenced.
         */                
        //MNcnNotifier& iNcnNotifier;
        
     	/**
         * Reference to model which is used to display
         * soft notes and manage indicators and message counts. 
         */                
        CNcnModelBase& iModel;
};


#endif // NCNVOICEMAILMANAGER_H

// End of File
