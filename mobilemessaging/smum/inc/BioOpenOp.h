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
*     Bio Message opening operation
*
*/



#ifndef __BIOOPENOP_H__
#define __BIOOPENOP_H__

//  INCLUDES
#include <msvapi.h>     // for CMsvOperation & TMsvId

// FORWARD DECLARATIONS
class CMtmStore;

// CLASS DECLARATION

/**
 * Bio Message opening operation.
 */
NONSHARABLE_CLASS( CBioOpenOp ) : public CMsvOperation
    {
public:
    /**
     * Two-phased constructor.
     * @param aSession Session.
     * @param aObserverStatus Observer status.
     * @param aId Message ID.
     * @param aFlags Flags.
     * @return The object.
     */
    static CBioOpenOp* NewL(CMsvSession& aSession, TRequestStatus& aObserverStatus, TMsvId aId, TUint aFlags);

    /**
     * Destructor.
     */
    virtual ~CBioOpenOp();

public: // from CMsvOperation
    /**
     * The progress.
     */
    const TDesC8& ProgressL();

protected: // from CActive

    /**
     * Cancelling.
     */
    void DoCancel();

    /**
     * RunL.
     */
    void RunL();

    /**
     * Error handling.
     */
    TInt RunError(TInt aError);

private:

    /**
     * Constructor
     */
    CBioOpenOp(CMsvSession& aSession, TRequestStatus& aObserverStatus, TMsvId aId);

    /**
     *  Second phase constructor.
     * @param aFlags Flags.
     */
    void ConstructL(TUint aFlags);

    /**
     * Parse the message.
     */
    void DoParseL();

    /**
     * Launch the message editor.
     */
    void DoEditL();

    /**
     * Complete the observer.
     */
    void CompleteObserver();

private: // prohibited

    /// Default constructor prohibited
    CBioOpenOp();
    
    /// Copy contructor prohibited.
    CBioOpenOp(const CBioOpenOp& aSource);
    
    /// Assignment operator prohibited.
    const CBioOpenOp& operator=(const CBioOpenOp& aSource);

private:

    TMsvId      iMsvId;
    enum TState {EUnknown,EParsing,EEditing};
    TState iState;
    CMsvOperation* iOperation;
    CMtmStore* iMtmStore;
    CBaseMtmUi* iBioMtmUi;
    CBaseMtmUi* iSmsMtmUi;
    TBool       iBioMtmUiClaimed;
    TBool       iSmsMtmUiClaimed;
    };


#endif // __BIOOPENOP_H__

// end of file
