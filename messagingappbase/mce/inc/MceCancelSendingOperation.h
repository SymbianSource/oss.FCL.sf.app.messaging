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
*     CMceCancelSendingOperation.
*     Cancels first sending of the message and then tries to delete or move it to drafts.
*
*/



#ifndef MCECANCELSENDINGOPERATION_H
#define MCECANCELSENDINGOPERATION_H

//  INCLUDES
#include <MsvPrgReporter.h> // MMsvProgressDecoder

// CLASS DECLARATION

/**
*  CMceCancelSendingOperation.
*  Cancels first sending of the message and then tries to delete or move it to drafts.
*/
class CMceCancelSendingOperation: public CMsvOperation
    {
    public:
        enum TCancelSendingType
            {
            ECancelOnly,
            ECancelAndMoveToDrafts,
            ECancelAndDelete
            };
        /**
        * Constructor.
        */
        static CMceCancelSendingOperation* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TMsvId aEntryId,
            TCancelSendingType aCancelType,
            CMtmStore& aMtmStore );

        /**
        * Destructor.
        */
        virtual ~CMceCancelSendingOperation();

        /**
        * From CMsvOperation
        */
        virtual const TDesC8& ProgressL();

    private:
        /**
        * Constructor.
        */
        CMceCancelSendingOperation(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TMsvId aEntryId,
            TCancelSendingType aCancelType,
            CMtmStore& aMtmStore );

        /**
        * Constructor.
        */
        void ConstructL();

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();

        /**
        * Cancels sending
        */
        void CancelSendingL();

        /**
        * Moves or deletes message after it has been cancelled
        */
        void MoveOrDeleteEntryL();

    private:
        enum TCancelState
            {
            ECancelling,
            EDeletingOrMoving
            };

    private:
        CMtmStore&              iMtmStore;
        TCancelSendingType      iCancelType;
        TCancelState            iCancelState;
        CMsvOperation*          iOperation;
        CMsvEntry*              iEntry;
        CMsvEntrySelection*     iEntrySelection;
        TMsvId                  iEntryId;
        TBuf8<1>                iBlank;
    };



#endif      // MCECANCELSENDINGOPERATION_H

// End of File
