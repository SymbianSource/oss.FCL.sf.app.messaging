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
* Description: MmsMtmDeliveryPopup  declaration
*
*/



#if !defined(__MUIU_DELIVERY_POPUP_H__)
#define __MUIU_DELIVERY_POPUP_H__


//  INCLUDES
#include <bamdesca.h>               //MDesCArray
#include <aknPopup.h>               //CAknPopupList
#include <aknlists.h>               //CAknSingleHeadingPopupMenuStyleListBox
#include <ConeResLoader.h>
#include <Muiumsginfo.h> // TMsgInfoMessageInfoData

// DATA TYPES
class CMsgDeliveryItem;


/**
* Class which displays a message info dialog for particular message types
*/
class CMsgInfoDeliveryStatusPopup : public CBase
    {

    public:

        /**
        * Two-phased constructor.
        */  
        static CMsgInfoDeliveryStatusPopup* NewL();

        /**
        * Destructor.
        */ 
        ~CMsgInfoDeliveryStatusPopup();

        /**
        * Show the message info dialog.
        */
        TInt ExecuteL();

        /**
        * Receives the data array from where to read data for the dialog.
        */
        void SetDataArray(CArrayPtrFlat< CMsgDeliveryItem >* aArray);

    private:

        /**
        * 
        */
        CMsgInfoDeliveryStatusPopup();

        /**
        * Symbian OS constructor.
        */
        void ConstructL();

        /**
        * Reads data from the delivery item, parses it to a suitable form and
        * adds it into a string given in aDest. Calculates the length of the items 
        * data presented in one string.
        * 
        * @param aItem: Message item to be parsed.
        * @param aDest: Destination string. Give it as NULL and method only returns the lenght.
        */
        TInt ParseStringL(CMsgDeliveryItem* aItem, TPtr* aDest);
        
        /**
        * Reads data from the delivery item, parses it to a suitable form and
        * adds it into iFailedText string.
        * 
        * @param aItem: Message item to be parsed.
        */
        void ParseFailedL(CMsgDeliveryItem* aItem);

        /**
        * Reads data from the delivery item, parses it to a suitable form and
        * adds it into iDeliveredText string.
        * 
        * @param aItem: Message item to be parsed.
        */
        void ParseDeliveredL(CMsgDeliveryItem* aItem);

        /**
        * Reads data from the delivery item, parses it to a suitable form and
        * adds it into iPendingText string.
        * 
        * @param aItem: Message item to be parsed.
        */
        void ParsePendingL(CMsgDeliveryItem* aItem);	

        /**
        * Loops through the delivery items in the data array and
        * parses their data into the correct strings with a correct style
        */
        void ParseTextL();
        
        /**
        * Adds localised "Delivered:", "Failed:" and "Pending" strings as labels into the text
        * strings
        */
        void AddLabelsL(TInt aFailed, 
                        TInt aPending, 
                        TInt aDelivered, 
                        TInt aRead, 
                        TInt aUnavailable );

        /**
        * Calculates the length of the labels and adds the lengths in to the message length attributes
        */
        void CalculateLabelSizesL(   TInt aFailed, 
                                    TInt aPending, 
                                    TInt aDelivered, 
                                    TInt aRead, 
                                    TInt aUnavailable );

        /**
        * Checks whether message was sent today. Information used to determine
        * on how to display the time / date in delivery status dialog
        */
        TBool MessageSentToday(const TDateTime& messageTime);

    private:
        HBufC* iFailedText;
        HBufC* iPendingText;
        HBufC* iDeliveredText;
        HBufC* iReadText;
        HBufC* iUnavailableText;
        HBufC* iText;
        
        TInt iMessageSize;
        TInt iFailedSize;
        TInt iPendingSize;
        TInt iDeliveredSize;
        TInt iReadSize;
        TInt iUnavailableSize;

        CArrayPtrFlat< CMsgDeliveryItem >* iDataArray;	
    };

#endif  //__MUIU_MESSAGE_INFO_H__

// End of file
