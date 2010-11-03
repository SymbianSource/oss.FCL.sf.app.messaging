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
*     Sends messages from Outbox when user selects "Start" in Outbox.
*
*/



#if !defined(__SENDOP_H__)
#define __SENDOP_H__


//  INCLUDES
#include <msvapi.h>

// FORWARD DECLARATIONS
class CMtmStore;

// CLASS DECLARATION

/**
*  Sends messages from Outbox when user selects "Start" in Outbox.
*/
class CMceSendOperation : public CMsvOperation
    {
public:
        /**
        * Constructor.
        */
        static CMceSendOperation* NewL(CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus, CMtmStore& aMtmStore, CMsvEntrySelection* aSel);
        /**
        * Destructor.
        */
        virtual ~CMceSendOperation();
        /**
        *
        */
        virtual const TDesC8& ProgressL();
private:
        /**
        * Constructor.
        */
        CMceSendOperation(CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus, CMtmStore& aMtmStore);
        /**
        * Constructor.
        */
        void ConstructL(CMsvEntrySelection* aSel);
        /**
        * From CActive
        */
        void DoCancel();
        /**
        * From CActive
        */
        void RunL();

        /**
        *
        */
        void StartNextOperation();

        /**
        *
        */
        void MakeNewOperationL();
private:
        CMtmStore&              iMtmStore;
        CArrayPtrFlat<CMsvEntrySelection> iSelections;
        CArrayFixFlat<TMsvId>   iServices;
        CMsvOperation*          iOperation;
        CMsvEntry*              iCEntry;
        TBuf8<1>                iBlank;
    };

#endif // __SENDOP_H__

// End of file
