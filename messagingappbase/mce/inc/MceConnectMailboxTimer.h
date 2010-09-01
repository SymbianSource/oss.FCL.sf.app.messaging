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
*     Active object for timing "Connect" query for Remote mailbox.
*
*/



#ifndef MCECONNECTMAILBOXTIMER_H
#define MCECONNECTMAILBOXTIMER_H

//  INCLUDES

// FORWARD DECLARATIONS
class CMceUi;

// CLASS DECLARATION

/**
*  Active object to wait three seconds when remote mailbox is opened before
*  asking from the user to connect mailbox.
*/
class CMceConnectMailboxTimer : public CActive
    {
    public:
       /**
        * 2-phase constructor.
        */
        static CMceConnectMailboxTimer* NewL(
            CMceUi& aMceUi );

        /**
        * Destructor.
        */
        virtual ~CMceConnectMailboxTimer();

        /**
        * Start timer
        * @param aAccountId: id of the mailbox to be opened
        */
        void SetTimer( TMsvId aAccountId );

    private:
        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        * Calls MceUi's GoOnlineL function.
        */
        virtual void RunL();

       /**
        * C++ constructor.
        */
        CMceConnectMailboxTimer( CMceUi& aMceUi );

        /**
        * Symbian OS constructor.
        */
        void ConstructL();



    private:
        RTimer      iTimer;
        CMceUi&     iMceUi;
        TMsvId      iAccountId;
    };



#endif      // MCECONNECTMAILBOXTIMER_H

// End of File
