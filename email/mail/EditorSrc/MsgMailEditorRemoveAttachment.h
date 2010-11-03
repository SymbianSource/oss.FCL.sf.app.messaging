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
*     E-mail attachment remove operation.
*
*/


#ifndef MSGMAILEDITORREMOVEATTACMENT_H
#define MSGMAILEDITORREMOVEATTACMENT_H

//  INCLUDES
#include <msvapi.h> // CMsvOperation

// CLASS DECLARATION
class CImEmailMessage;
class CMsvEntry;

/**
*  E-mail attachment remove operation.
*/
class CMsgMailEditorRemoveAttachmentOp :public CMsvOperation
    {
    public:  // Constructors and destructor
        /**
        * CMsgMailEditorRemoveAttachmentOp::NewLC()
        * @param aMsvSession reference to the Session object
        * @param aObserverRequestStatus status of the operation
        * @param aMsgId message that contains attachment to be removed
        * @param aAttachmentId attachment to be removed.
        */
        static CMsgMailEditorRemoveAttachmentOp* NewLC(
            CMsvSession& aMsvSession, 
            TRequestStatus& aObserverRequestStatus, const TMsvId aMsgId, 
            const TMsvId aAttachmentId);

        /**
        * Destructor.
        */
        virtual ~CMsgMailEditorRemoveAttachmentOp();
        
    public: // Functions from base classes
        /// From CMsvOperation
        virtual const TDesC8& ProgressL();        
        
    protected:  // Functions from base classes
        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void RunL();
    private:
        /**
        * C++ default constructor.
        */
        CMsgMailEditorRemoveAttachmentOp(CMsvSession& aMsvSession, 
            TRequestStatus& aObserverRequestStatus);

        /**
        * Symbian constructor
        * Sets operation active.
        */
        void ConstructL(const TMsvId aMsgId, const TMsvId aAttachmentId);

    private:    // Data
        /**
        * Own: Entry which has attachment
        */
        CMsvEntry* iMsgEntry;

        /**
        * Own: Mail message which has attachment
        */
        CImEmailMessage* iEmailMsg;

        /**
        * Package buffer
        */
        TPckgBuf<TMsvLocalOperationProgress> iProgress;

    };

#endif      // MSGMAILEDITORREMOVEATTACMENT_H
            
// End of File
