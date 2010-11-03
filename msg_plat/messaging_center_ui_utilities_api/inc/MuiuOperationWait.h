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




#ifndef __MUIUOPERATIONWAIT_H__
#define __MUIUOPERATIONWAIT_H__


//  INCLUDES
#include <e32base.h>



// CLASS DECLARATION
/**
*
* Deprecated! Use internal CMsgOperationWait instead!
*
* Allows a synchronous wait on a operation.
*/
class CMuiuOperationWait: public CActive
    {
    public:
        IMPORT_C static CMuiuOperationWait* NewLC( TInt aPriority = EPriorityStandard );
        IMPORT_C ~CMuiuOperationWait();
        IMPORT_C void Start();
    protected:
        CMuiuOperationWait( TInt aPriority );
        void RunL();
        void DoCancel();
    protected:
        CActiveSchedulerWait iWait;
    };

#endif //__MUIUOPERATIONWAIT_H__

// End of file
