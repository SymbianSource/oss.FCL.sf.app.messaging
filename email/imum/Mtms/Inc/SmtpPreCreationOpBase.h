/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       Class declaration file
*
*/

#ifndef __SMTPPRECREATIONOPBASE_H__
#define __SMTPPRECREATIONOPBASE_H__

#include "ImumOnlineOperation.h"
#include "SelectionOperationBase.h"
#include <smtpset.h>

//
class CMsvEmailConnectionProgressProvider;
class CMtmStore;
class CEmailPreCreation;


/**
* CSmtpPreCreationOpBase
*/
class CSmtpPreCreationOpBase : public CMsvOperation
    {
    public:
        /**
        *
        */
        virtual ~CSmtpPreCreationOpBase();

        /**
        *
        */
        virtual const TDesC8& ProgressL();

    protected:

        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        *
        */
        void CompleteObserver();

        /**
        *
        */
        void CompleteThis();

    protected:

        /**
        *
        */
        CSmtpPreCreationOpBase(
            CMsvSession& aMsvSession, 
            TInt aPriority, 
            TRequestStatus& aObserverRequestStatus);

        /**
        *
        */
        void ConstructL();

    protected: // Data
        CEmailPreCreation* iPreCreate;
        CMtmStore* iMtmStore;
        CMsvOperation* iOperation;
        TPckgBuf<TMsvLocalOperationProgress> iProgress;
        // Owned: Pointer to Email API. The operation can exist, even if
        //        the smtp mtm ui doesn't, so own instance of API is needed
        CImumInternalApi*   iMailboxApi;
    };


#endif
