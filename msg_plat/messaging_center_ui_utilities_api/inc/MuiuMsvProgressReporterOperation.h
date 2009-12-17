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
*     Operation progress reporting class.
*
*/



#ifndef __MSVPROGRESSREPORTEROPERATION_H__
#define __MSVPROGRESSREPORTEROPERATION_H__


//  INCLUDES
#include <msvapi.h>
#include <msvprgreporter.h>


// FORWARD DECLARATIONS
class CBaseMtmUi;
class CMsvWrappableProgressDialog;



// CLASS DECLARATION

/**
* Progress reporter operation.
*
* A 'super-operation' implementing the abstract progress reporter mixin
* interface. This class encapsulates the operation; which provides the progress;
* and the dialog that displays it.
*
*/
class CMsvProgressReporterOperation :
    public CMsvOperation, 
    public MMsvProgressReporter
    {
    public:
        /**
        * Creates new CMsvProgressReporterOperation operation
        * @param aMsvSession: reference to object of CMsvSession
        * @param aObserverRequestStatus: reference to TRequestStatus which is used when operation is completed
        * @param aImageIndex: index of the icon from avkon.mbm to be displayed in the progress note
        * @return: Object of class CMsvProgressReporterOperation.
        */
        IMPORT_C static CMsvProgressReporterOperation* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TInt aImageIndex );

        /**
        * @param aMsvSession: reference to object of CMsvSession
        * @param aObserverRequestStatus: reference to TRequestStatus which is used when operation is completed
        * @return: Object of class CMsvProgressReporterOperation.
        */
        IMPORT_C static CMsvProgressReporterOperation* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus );

        /**
        * Destructor
        */
        IMPORT_C virtual ~CMsvProgressReporterOperation();

        /**
        * @return: progress
        */
        IMPORT_C virtual const TDesC8& ProgressL();

        /**
        * Make progress visible
        */
        IMPORT_C virtual void MakeProgressVisibleL( TBool aVisible );

        /**
        * Sets text to the dialog
        */
        IMPORT_C virtual void SetTitleL( const TDesC& aTitle );

        /**
        * Sets text to the dialog
        */
        IMPORT_C virtual void SetTitleL( TInt aTitleResourceId );

        /**
        * @return mtm store
        */
        IMPORT_C virtual CMtmStore& MtmStore();

        /**
        * Sets if operation can be cancelled by user or not
        * @param aCanCancel: EFalse if operation cannot be cancelled by user,
        *    ETrue otherwise
        */
        IMPORT_C virtual void SetCanCancelL( TBool aCanCancel );

        /**
        * Sets progress decoder.
        */
        IMPORT_C virtual void SetProgressDecoder( MMsvProgressDecoder& aDecoder );

        /**
        * Removes progress decoder.
        */
        IMPORT_C virtual void UnsetProgressDecoder();

        /**
        * Sets help context
        */
        IMPORT_C virtual void SetHelpContext( const TCoeHelpContext& aContext );

        /**
        * Sets animation
        */
        IMPORT_C virtual void SetAnimationL( TInt aResourceId );

        /**
        * Sets operations
        * Takes ownership of the operation so this class will delete
        * the operation.
        */
        IMPORT_C void SetOperationL( CMsvOperation* aOp );

        /**
        * Sets seeding
        */
        IMPORT_C void SetSeeding( TBool aSeed );

        /**
        * @return TRequestStatus
        */
        IMPORT_C TRequestStatus& RequestStatus();

        /**
        * @param aDelay, a boolean to set the delay on or off
        */
        IMPORT_C void SetProgressVisibilityDelay( TBool aDelay );

        /**
        * Creates new CMsvProgressReporterOperation operation
        * @param aProgressVisibilityDelayOff: constructs progress dialog without visibility delay.
        * @param aMsvSession: reference to object of CMsvSession
        * @param aObserverRequestStatus: reference to TRequestStatus which is used when operation is completed
        * @param aImageIndex: index of the icon from avkon.mbm to be displayed in the progress note
        * @return: Object of class CMsvProgressReporterOperation.
        */
        IMPORT_C static CMsvProgressReporterOperation* NewL(
            TBool aProgressVisibilityDelayOff,
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TInt aImageIndex );

        /**
        * @param aProgressVisibilityDelayOff: constructs progress dialog without visibility delay.
        * @param aMsvSession: reference to object of CMsvSession
        * @param aObserverRequestStatus: reference to TRequestStatus which is used when operation is completed
        * @return: Object of class CMsvProgressReporterOperation.
        */
        IMPORT_C static CMsvProgressReporterOperation* NewL(
            TBool aProgressVisibilityDelayOff,
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus );

        /**
        * SetCompleteWithExit
        * This function sets complete code
        * @param aCompleteWithExit, boolean to set if EEikCmdExit
        */
        void SetCompleteWithExit( TBool aCompleteWithExit );

        /**
        * Creates new CMsvProgressReporterOperation operation. With this method it is possible
        * to show note dialog with EWaitLayout(With wait note bar).
        * @param aProgressVisibilityDelayOff: constructs progress dialog without visibility delay.
        * @param aDisplayWaitAnimation if ETrue then shows note dialog with wait note bar
        * @param aMsvSession: reference to object of CMsvSession
        * @param aObserverRequestStatus: reference to TRequestStatus which is used when operation is completed
        * @param aImageIndex: index of the icon from avkon.mbm to be displayed in the progress note
        * @return: Object of class CMsvProgressReporterOperation.
        */
        IMPORT_C static CMsvProgressReporterOperation* NewL(
            TBool aProgressVisibilityDelayOff,
            TBool aDisplayWaitAnimation,
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TInt aImageIndex );
            
        /**
        * Creates new CMsvProgressReporterOperation operation.
        * This method is used only when progress dialog is shown also in cover side.
        * @param aMsvSession: reference to object of CMsvSession
        * @param aObserverRequestStatus: reference to TRequestStatus which is used when operation is completed
        * @param aDialogIndex unique index to identify a dialog within the category.
        * @param aCategoryUid category uid
        * @return: Object of class CMsvProgressReporterOperation.
        */
        IMPORT_C static CMsvProgressReporterOperation* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TInt aDialogIndex,
            TUid aCategoryUid );

        /**
        * Publish new dialog on secondary display.
        * This method first dismisses the original dialog both from primary and 
        * secondary displays. After that a new dialog is created and it is 
        * published on secondary display with given parameters.
        * @param aDialogIndex unique index to identify a dialog within the category.
        * @param aCategoryUid category uid
        */
        IMPORT_C void PublishNewDialogL( TInt aDialogIndex, TUid aCategoryUid );

    protected:
        /**
        * from CActive
        */
        virtual void DoCancel();

        /**
        * from CActive
        */
        virtual void RunL();

    private:
        /**
        * internal
        * Completes observer with the aCode
        */
        void CompleteObserver( TInt aCode );

    private:
        /**
        * C++ constructor
        */
        CMsvProgressReporterOperation(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TInt aImageIndex );
        /**
        * Symbian OS constructor
        */
        void ConstructL( 
        TBool aProgressVisibilityDelayOff = EFalse,
        TBool aDisplayWaitAnimation = ETrue );
        
        /**
        * Symbian OS constructor
        * This method is used only when progress dialog 
        * is shown also in cover side.
        */
        void ConstructL( 
        TInt aDialogIndex,
        TUid aCategoryUid,
        TBool aProgressVisibilityDelayOff = EFalse,
        TBool aDisplayWaitAnimation = ETrue );


    private:
        CMtmStore* iMtmStore;
        CMsvWrappableProgressDialog* iDialog;
        CMsvOperation* iOperation;
        TBufC8<1> iDummyProgress;
        TInt iImageIndex;
        TBool iCompleteWithExit;
        TBool iDisplayWaitAnimation;
        HBufC* iProgressString;
    };

#endif // __MSVPROGRESSREPORTEROPERATION_H__

// End of file
