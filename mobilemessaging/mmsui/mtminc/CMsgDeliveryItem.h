/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     Encapsulates the data which populates the fields in the message 
*     info dialogs (email, sms, bios, mms), also holds the text for the 
*     labels and fields in the message info dialog and displays a message 
*     info dialog for particular message types
*
*/



#if !defined(__MUIU_DELIVERYITEM_H__)
#define __MUIU_DELIVERYITEM_H__


//  INCLUDES
#include <e32base.h>


// DATA TYPES


/**
* Class which contains the needed data for each message entry. Data is read in delivery status dialog.
*/
class CMsgDeliveryItem : public CBase
    {
    public:
         
        /**
        * Enumeration used to indicate entrys status
        */    
    enum TMmsStatus
        {
        ENone = 0,  // 0 
        EPending,  
        EFailed,   
        EDelivered, 
        ERead       // 4    
        };
    public:

        /**
        * Two-phased constructor.
        */  
        static CMsgDeliveryItem* NewL();

        /**
        * Destructor.
        */ 
        ~CMsgDeliveryItem();


        TTime GetTime();

        void SetTime(const TTime aTime);

        TDesC& GetNumber();

        void SetNumberL(const TDesC& aText);

        TDesC& GetRecipient();

        void SetRecipientL(const TDesC& aText);

        TInt GetStatus();

        void SetStatus(const TMmsStatus aStatus);


    private:

        /**
        * Default constructor.
        */
        CMsgDeliveryItem();

        /**
        * Symbian OS constructor.
        */
        void ConstructL();

    private:
    	TTime iTime;
    	HBufC* iRecipient;
    	HBufC* iNumber;
    	TMmsStatus iStatus; 
    };

#endif // __MUIU_LOGDATA_H__
// End of file