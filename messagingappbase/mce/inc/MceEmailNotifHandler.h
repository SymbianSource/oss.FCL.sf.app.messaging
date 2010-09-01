/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Active object for marking the e-mails as read and resetting
*				 the count of the unread e-mails.
*
*/




#ifndef MCE_EMAIL_NOTIF_HANDLER_H
#define MCE_EMAIL_NOTIF_HANDLER_H

//  INCLUDES
#include <e32base.h>
#include <MNcnInternalNotification.h>
#include <NcnNotificationDefs.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  Active object for marking the e-mails as read and resetting
*  the count of the unread e-mails.
*  @lib Mce.exe
*  @since Series 60 3.0
*/
class CMceEmailNotifHandler : public CActive
	{
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
	    static CMceEmailNotifHandler* NewLC();
	    
	    /**
        * Two-phased constructor.
        */
	    static CMceEmailNotifHandler* NewL();
	    
        /**
        * Destructor.
        */
	    ~CMceEmailNotifHandler();
	    
    public: // New functions	    
    
        /**
        * Starts the operation for marking all the e-mails as read and
        * resetting the count of the unread e-mails. Operation runs in
        * the background
        * @since Series 60 3.0
        * @param  None.
        * @return None.
        */
        void HandleNotif();
        
        /**
         * Starts the operation for marking the e-mails of specific account as read and
         * resetting the count of the unread e-mails. Operation runs in
         * the background
         * @since Series 60 3.0
         * @param aMailBox The id for mail msg type identifying mailbox or mail
         *                 folder containing message(s) to be marked as unread.
         * @return None.
         */
        void HandleNotif(const TMsvId& aMailbox);

    public:  // Functions from base classes
    
        /**
        * From CActive, completion of the asyncronous request
        * @since Series 60 3.0
        * @return None.
        */ 
	    void RunL();
	    
	    /**
        * From CActive, cancellation of the asyncronous request
        * @since Series 60 3.0
        * @return None.
        */        
        void DoCancel();
        
        /**
        * From CActive, handles the leaves from RunL
        * @since Series 60 3.0
        * @param aError: Error code
        * @return None.
        */
        TInt RunError( TInt );

    private:

        /**
        * C++ default constructor.
        */
	    CMceEmailNotifHandler();
	
		/**
        * Second phase constructor.
        */
		void ConstructL();
	
    private:     // Data

        MNcnInternalNotification*	iNcnNotification;
	};

#endif      // MCE_EMAIL_NOTIF_HANDLER_H
            
// End of File
