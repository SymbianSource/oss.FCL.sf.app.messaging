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
*     Timer implementation designed for always online timed operations
*
*/


#ifndef __ALWAYSONLINEEMAILPLUGINTIMER_H__
#define __ALWAYSONLINEEMAILPLUGINTIMER_H__

//INCLUDES
#include <msvapi.h>

/**
* CAlwaysOnlineEmailPluginTimer
*
*/
class CAlwaysOnlineEmailPluginTimer : public CMsvOperation
    {
    public:
        /**
        * NewL
        * Symbian OS constructor.
        * @param CMsvSession&, session reference
        * @param TRequestStatus&, request status
        * @return CAlwaysOnlineEmailPluginTimer*, self pointer
        */
        static CAlwaysOnlineEmailPluginTimer* NewL( 
            CMsvSession& aMsvSession, TRequestStatus& aStatus );


        /**
        * ~CAlwaysOnlineEmailPluginTimer
        * destructor
        */
        virtual ~CAlwaysOnlineEmailPluginTimer();

    private:
        /**
        * CAlwaysOnlineEmailPluginTimer
        * @param CMsvSession&, session reference
        * @param TInt, priority
        * @param TRequestStatus&, request status
        */
        CAlwaysOnlineEmailPluginTimer(
            CMsvSession& aMsvSession, 
            TInt aPriority, 
            TRequestStatus& aObserverRequestStatus );

        /**
        * ConstructL
        * Symbian OS second phase constructor.
        */
		void ConstructL();

    public:

        /**
        * ProgressL
        * @return TDesC8&, progress
        */
		virtual const TDesC8& ProgressL();

        /**
        * At
        * @param const TTime&, time when to complete 
        */
		void At(const TTime& aTime);


	protected:

        /**
        * DoCancel
        * From CActive
        */
		virtual void DoCancel();

        /**
        * RunL
        * From CActive
        */
		virtual void RunL();
		
		/**
		* Function to handle error during running the operation
		* @since Series60 2.6
		* @return KErrNone, when error has been handled
		*/
		virtual TInt RunError( TInt aError );

	protected: // Data
		RTimer iTimer;
		TBuf8<1> iProgress;
        TTime iCompletionTime;
    };


#endif
//EOF
