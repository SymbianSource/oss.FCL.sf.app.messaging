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
*    CMsgErrorGlobalQuery declaration file 
*
*/



#ifndef MSGERRORGLOBALQUERYOBSERVER_H
#define MSGERRORGLOBALQUERYOBSERVER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CMsgErrorWatcher;
class CAknGlobalConfirmationQuery;

// CLASS DECLARATION

/**
* CMsgErrorGlobalQuery
*
* @lib msgerrorwatcher.dll
* @since 2.0
*/
class CMsgErrorGlobalQuery : public CActive
    {
    public:     // construct / destruct

        /**
        * Two-phased constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcher
        */
        static CMsgErrorGlobalQuery* NewL(
            CMsgErrorWatcher* aWatcher );

        /**
        * Destructor.
        */
        virtual ~CMsgErrorGlobalQuery();

        /**
        * Shows the global query
        *
        * @param aPrompt
        * @param aSoftkeys
        * @param aQueryId
        */
        void ShowQueryL(
            const TDesC& aPrompt,
            TInt aSoftkeys,
            TInt aQueryId );

    private:
    
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcher
        */
        CMsgErrorGlobalQuery( CMsgErrorWatcher* aWatcher );

        /**
        * From CActive
        */
        void RunL() ;

        /**
        * From CActive
        */
        void DoCancel();

    private:    // data

        CAknGlobalConfirmationQuery*    iGlobalQuery;
        CMsgErrorWatcher*               iWatcher;
        HBufC*                          iPrompt;
        TInt                            iQueryId;
    };

#endif

// End of File
