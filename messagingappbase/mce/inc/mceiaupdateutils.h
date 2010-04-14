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
*     Application class for Mce.
*
*/

#ifndef C_CMCEIAUPDATEUTILS_H
#define C_CMCEIAUPDATEUTILS_H

// system include files go here:
#include <e32base.h>
#include <iaupdateobserver.h>

// forward declarations go here:
class CMceUi;
class CIAUpdate;
class CIAUpdateParameters;

/**
 *  Has utilities to help IAD aware applications to
 *  update themselves (discover, download and install updates).
 *
 *  @code
 *   const TUid KYourAppUid ={0x10005ABC};
 *   CMceIAUpdateUtils *MsgIadUpdate = CMceIAUpdateUtils::NewL();
 *   iMsgIadUpdate->StartL(KYourAppUid);
 *  @endcode
 *
 */
class CMceIAUpdateUtils : public CActive,
public MIAUpdateObserver
    {
public:

    /**
     * Two-phased constructor.
     * @param aAppUid Uid of the app for which update needs to be checked.
     */
    static CMceIAUpdateUtils* NewL(CMceUi& aMceUi);


    /**
    * Destructor.
    */
    virtual ~CMceIAUpdateUtils();

    void StartL( const TUid aAppUid );
    
    /**
     * Is IAD Update requried to do now
     * @return TBool
     * ETrue, If the KMceIADUpdateCheckRetryInterval is over after the previous try
     * EFalse, If the KMceIADUpdateCheckRetryInterval is not over after the previous retry 
     */
    TBool IsUpdateRequired();
    
protected: 

     /**
     * From CActive
     */
     void RunL();
     
     void DoCancel();  

private:
	
    /**
     * C++ default constructor.
     */
    CMceIAUpdateUtils(CMceUi& aMceUi);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Cleanup function.
     */
    void Delete();
	
	/**
     * For Setting the active object active immediately.
     */	
	void CompleteSelf();
	
	/**
     * Start IA update process.
     * @param aAppUid Uid of the app for which update needs to be checked.
     */
    void DoStartL(TUid aAppUid);

private: // From MIAUpdateObserver

    /**
     * From MIAUpdateObserver.
     * This callback function is called when the update checking operation has
     * completed.
     *
     * @param aErrorCode The error code of the observed update operation.
     *                   KErrNone for successful completion,
     *                   otherwise a system wide error code.
     * @param aAvailableUpdates Number of the updates that were found available.
     */
    void CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates );


    /**
     * From MIAUpdateObserver.
     * This callback function is called when an update operation has completed.
     * Even if multiple functions are provided to start different update
     * operations, this callback function is always called after an update
     * operation has completed.
     *
     * @param aErrorCode The error code of the completed update operation.
     *                   KErrNone for successful completion,
     *                   otherwise a system wide error code.
     * @param aResult Details about the completed update operation.
     *                Ownership is transferred.
     */
    void UpdateComplete( TInt aErrorCode, CIAUpdateResult* aResultDetails );


    /**
     * From MIAUpdateObserver.
     * This callback function is called when an update query operation has
     * completed.
     *
     * @param aErrorCode The error code of the observed query operation.
     *                   KErrNone for successful completion,
     *                   otherwise a system wide error code.
     * @param aUpdateNow ETrue informs that an update operation should be started.
     *                   EFalse informs that there is no need to start an update
     *                   operation.
     */
    void UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow );

private: // data

    /**
     * IAD update API.
     * Own.
     */
    CIAUpdate* iUpdate;

    /**
     * IAD update parameters.
     * Own.
     */
    CIAUpdateParameters* iParameters;
    
    TUid 				 iAppUid;
    
    CMceUi&              iMceUi;
	
    /**
	 * Previous IAD update check retry time
	 */
    TTime                iPrevIADUpdateCheckTry;
    };

#endif // C_CMCEIAUPDATEUTILS_H

//EOF
