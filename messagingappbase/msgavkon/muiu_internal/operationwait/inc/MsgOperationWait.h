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
*     Allows a synchronous wait on a operation
*
*/




#ifndef __MSGOPERATIONWAIT_H__
#define __MSGOPERATIONWAIT_H__


//  INCLUDES
#include <e32base.h>



// CLASS DECLARATION
/**
* Allows a synchronous wait on a operation.
*/
class CMsgOperationWait: public CActive
    {
    public:
        IMPORT_C static CMsgOperationWait* NewLC( TInt aPriority = EPriorityStandard );
        IMPORT_C ~CMsgOperationWait();
        IMPORT_C void Start();
    protected:
        CMsgOperationWait( TInt aPriority );
        void RunL();
        void DoCancel();
    protected:
        CActiveSchedulerWait iWait;
    };

#endif //__MSGOPERATIONWAIT_H__

// End of file
