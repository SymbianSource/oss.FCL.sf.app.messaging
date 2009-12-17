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
*     Starts copy/move message store progress dialog and transfer handler
*
*/



#ifndef MCESETTINGSMOVEPROGRESS_H
#define MCESETTINGSMOVEPROGRESS_H

//  INCLUDES

#include <MuiuMsvProgressReporterOperation.h> // CMsvProgressReporterOperation
#include <MuiuMsvRemoteOperationProgress.h> // MMsvRemoteOperationProgressObserver
#include "MceSettingsMovemsgs.h"            // MMceMsgTransHandlerObserver
#include <AknProgressDialog.h>              // CAknProgressDialog
#include <babackup.h>                       // CBaBackupSessionWrapper


// FORWARD DECLARATIONS
class CMceGeneralSettingsDialog;

// CLASS DECLARATION



class CMceMoveProgress: public CBase, 
                                       public MMsvRemoteOperationProgressObserver, 
                                       public MMceMsgTransHandlerObserver, 
                                       public MProgressDialogCallback
    {
public:

    /**
    * Two-phased constructor.
    */
    static CMceMoveProgress* NewL(
            CMsvSession*& aSession, MMsvSessionObserver& aObserver,
            TInt& aSource, const TInt& aTarget, TBool aDeleteCopiedStore,
            CMceGeneralSettingsDialog& aGeneralSettings, TBool aDeleteOnly);
    

    /**
    * Destructor
    */
    ~CMceMoveProgress();

    /**
    * Constructor
    */
    void ConstructL();

    /**
    * Starts transferring message store
    */
    void InitializingL();

    /**
    * Starts message store transfer thread
    */
    void TransferingL();

    /**
    * Starts copy/move progress dialog
    */
    void   ExecuteLD();

    // MMsvRemoteOperationProgressObserver
    void UpdateRemoteOpProgressL();

      // from MMceMsgTransHandlerObserver
    void CopyCompleteL(TInt aErr);
    void TransferCompleteL(TInt aErr);
    void DisableCancelL();

    // from MProgressDialogCallback
    void DialogDismissedL( TInt aButtonId );

private:
    /**
    * Constructor
    */
    CMceMoveProgress::CMceMoveProgress(
    	CMsvSession*& aSession, MMsvSessionObserver& aObserver,
    	TInt& aSource, const TInt& aTarget, TBool aDeleteCopiedStore,
    	CMceGeneralSettingsDialog& aGeneralSettings, TBool aDeleteOnly);

private:
    CMsvSession*&               iSession;
    MMsvSessionObserver&        iObserver;
    TInt&                       iSource;
    const TInt&                iTarget;
    TBool                       iDeleteCopiedStore;
    TBool						iCopyOperation;
    CMceGeneralSettingsDialog&  iGeneralSettings;
    TBool                       iDeleteOnly;

    CMceMessageTransHandler*    iHandler;
    CAknProgressDialog*         iProgressDialog;
    CEikProgressInfo*           iProgressInfo;
    CMsvRemoteOperationProgress* iProgressTimer;
    };


class CMceMoveProgressDialog : public CAknProgressDialog
    {
public:

    /**
    * Constructor
    */
    CMceMoveProgressDialog( TInt aFinalValue,
                                            TInt anIncrement,
                                            TInt anInterval, 
                                            CEikDialog** aSelfPtr ) :
                            CAknProgressDialog(aFinalValue,anIncrement, anInterval, aSelfPtr ) {}
        

    /**
    * Constructor
    */
    CMceMoveProgressDialog( CEikDialog** aSelfPtr ) :
          CAknProgressDialog( aSelfPtr) {}

    /**
    * Constructor
    */
    CMceMoveProgressDialog( CEikDialog** aSelfPtr, TBool aVisibilityDelayOff ) :
         CAknProgressDialog( aSelfPtr,aVisibilityDelayOff ) {}

public:

    /**
    * Disables Cancel button in copy/move progress dialog
    */
    void DisableCancelL();
    };



#endif      // MCESETTINGSMOVEPROGRESS_H

// End of File
