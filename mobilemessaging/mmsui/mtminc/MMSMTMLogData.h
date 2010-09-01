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



#if !defined(__MUIU_LOGDATA_H__)
#define __MUIU_LOGDATA_H__


//  INCLUDES

// DATA TYPES


/////////////////////////////////////////////////////////////

#include <e32base.h>
#include "MMmsMtmDeliveryDataContainer.h"
#include <mmsmsventry.h>
#include <logwrap.h>
/**
* Class which displays a message info dialog for particular message types
*/
class CLogViewEvent;
class CLogClient;
class CLogFilterList;
class CMsgDeliveryItem;
class CLogEvent;
class TMmsMsvEntry;
class CMmsClientMtm;


class CMmsMTMLogData : public CActive
    {
    public:
    
    enum TExecutionState 
        {
        ENone,
        EInit,
        EReading,
        EFinalize,
        EFinished
        };

        /**
        * Starts the active object. Creates the necessary log filters and starts the query
        * @param aMmsClient Constains information about the message, is used to ask message recipients
         * @return ETrue if log events found
         */
        TBool DeliveryDataL(const CMmsClientMtm* aMmsClient);

    public:

        /**
        * Two-phased constructor.
        */  
        static CMmsMTMLogData* NewL(MMmsMtmDeliveryDataContainer* aContainer, const TMmsMsvEntry& aEntry);

        /**
        * Destructor.
        */ 
        ~CMmsMTMLogData();

    private:

        /**
        * 
        */
        CMmsMTMLogData();

        /**
        * Symbian OS constructor.
        */
        void ConstructL(MMmsMtmDeliveryDataContainer* aContainer, const TMmsMsvEntry& aEntry );


          /**
         * Starts to fetch the next CLogEvent from the CLogViewEvent
         * @param none
         * @return void.
         */
        void FetchItemL();

          /**
         * Copies the next CLogEvent from the CLogViewEvent. Calls AddToArray
         * @param none
         * @return void.
         */
        void CopyItemL();

         /**
         *  Callback method. After finished with filling the data array,
         *  sends it back to the MMmsMtmDeliveryDataContainer (implemented in laucher class)
         * @param none
         * @return void.
         */
        void SendDataL();

         /**
         * If log hasn't every recipient, this function copies them to the resulting 
         * iDataArray
         */
        void MtmRecipientsToArrayL( const CMmsClientMtm& aMmsClient );

         /**
         * Checks whether recipient is already in the iDataArray and removes it
         * @param aRecipient recipient's email address or phone number to check
         * @param aType type of recipient address: ETrue phone number
         *                                   EFalse email address
         * @param aInitialIndex Log engine returns recipients, which are added into
         *        iDataArray. Items returned from log engine nooed not to be checked.
         */
        void DeleteRecipent(TDesC&  aRecipient,
                            TBool   aType,
                            TInt    aInitialIndex );

          /**
         *  Converts the CLogEvent to CMsgDeliveryItem and adds it to the data array
         * @param aEvent: The event to be stored and forwarded.
         * @return void.
         */
        void AddToArrayL(CLogEvent* aEvent);
        

    private: /// from CActive
        void DoCancel();
    protected: /// from CActive
        void RunL();
        TInt RunError(TInt aError);
    private: 
        // For log db handling
        CLogClient*             iLogClient;
        CLogViewEvent*          iEventView;
        CLogFilterList*         iFilterList;
        // callback
        MMmsMtmDeliveryDataContainer* iParent;
        // data array to be used
        CArrayPtrFlat< CMsgDeliveryItem >* iDataArray;
        RFs*                    iFs;
        // used to detect the first event on the list
        TInt                    iLoopCounter;
        // active objects state
        TExecutionState         iState;
        TMmsMsvEntry            iEntry;
        // localised strings to be used when comparing the entries delivery statuses
        TLogString              iEventStatusPendingTxt;
        TLogString              iEventStatusSentTxt;
        TLogString              iEventStatusFailedTxt;
        TLogString              iEventStatusNoDeliveryTxt;
        TLogString              iEventStatusDeliveredTxt;
        TLogString              iEventStatusNotSentTxt;
        TLogString              iEventStatusRead;
        // count of items, which log returns and have been appended into iDataArray
        TBool                   iInsertedCount;
    };
    
#endif // __MUIU_LOGDATA_H__
// End of file