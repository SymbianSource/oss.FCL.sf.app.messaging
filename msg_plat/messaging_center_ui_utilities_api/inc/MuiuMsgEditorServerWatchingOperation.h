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
*     Msv operation for server app watching
*
*/




#include <msvapi.h>        // CMsvOperation
#include <aknserverapp.h>  // MAknServerAppExitObserver
#include <muiumsgeditorlauncher.h>  // TEditorParameters 

class CMuiuMsgEditorService;

// CLASS DECLARATION

/**
* CMsgEditorServerWatchingOperation
* Simple operation class which watches a service (editor application),
* completing when that thread closes.
*
* The passed CMsvSession is not used (merely required to base-construct
* a CMsvOperation).
*/
class CMsgEditorServerWatchingOperation : public CMsvOperation, public MAknServerAppExitObserver			
    { 
    public:

	    IMPORT_C static CMsgEditorServerWatchingOperation* NewL( 
            CMsvSession& aMsvSession,
            TInt aPriority,
            TRequestStatus& aObserverRequestStatus,
            TUid aMtm,
            const TEditorParameters aParams );
	    
        /**
        * C++ constructor
        */
        IMPORT_C CMsgEditorServerWatchingOperation(
            CMsvSession& aMsvSession,
            TInt aPriority,
            TRequestStatus& aObserverRequestStatus,
            TUid aMtm );

        /**
        * Destructor.
        */
        IMPORT_C ~CMsgEditorServerWatchingOperation();

    private: // From MApaServerAppExitObserver
        void HandleServerAppExit( TInt aReason );
		
    protected:
        /**
        * From CMsvOperation
        */
        virtual const TDesC8& ProgressL();

        /**
        * From CMsvOperation
        */
        virtual const TDesC8& FinalProgress();

        /**
        * From CActive
        */
        virtual void RunL();

        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        *
        */
        void Start();

    private:
        void ConstructL( const TEditorParameters aParams, TUid aMtmType );

        /**
        * Completes observer with the completion code
        * @param aCode: Completion code
        */
        void CompleteObserver( TInt aCode );

    protected:// Data
        HBufC8*     iProgress;

    private:
    	CMuiuMsgEditorService* iService;

    };

// End of file