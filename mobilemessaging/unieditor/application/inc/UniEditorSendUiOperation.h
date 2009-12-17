/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CUniEditorSendUiOperation, operation for opening messages created by SendUI   
*
*/



#ifndef __UNIEDITORSENDUIOPERATION_H
#define __UNIEDITORSENDUIOPERATION_H

// INCLUDES

#include <e32base.h>
#include <e32std.h>

#include "UniEditorDocument.h"
#include "UniEditorOperation.h"
#include "UniEditorProcessImageOperation.h"
#include "UniEditorConvertVideoOperation.h"
// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

class CMsgEditorView;
class CUniEditorProcessTextOperation;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CUniEditorSendUiOperation, operation for opening messages created by SendUI
*
* @since 3.2
*/
class CUniEditorSendUiOperation : public CUniEditorOperation
    {
    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @param    aObserver
        * @return   Pointer to instance in cleanup stack
        */
        static CUniEditorSendUiOperation* NewL(
            MUniEditorOperationObserver& aObserver,
            CUniEditorDocument& aDocument,
            RFs& aFs );

        /**
        * Destructor
        */
        virtual ~CUniEditorSendUiOperation();

        /**
        * Starts send ui operation
        */
        void Start();

    protected:

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CUniEditorOperation
        */
        void DoCancelCleanup();

        /**
        * From CUniEditorOperation
        */
        void HandleOperationEvent( TUniEditorOperationType aOperation,
                                   TUniEditorOperationEvent aEvent );

    private:

        /**
        * C++ constructor.
        */
        CUniEditorSendUiOperation(
            MUniEditorOperationObserver& aObserver,
            CUniEditorDocument& aDocument,
            RFs& aFs );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Performs single send ui step.
        */
        void DoSendUiStepL();

        /**
        * Performs send ui checks.
        */
        void DoSendUiCheckL();

        /**
        * Check files.
        */
        void DoCheckFilesL();

        /**
        * Prepares objects
        */
        void DoSendUiPrepareObjectL();

        /**
        * Prepased single object.
        */
        void DoPrepareObjectL( CUniObject* aObject );

        /**
        * Verifies prepared objects.
        */
        void VerifyPreparedObjectL();

        /**
        * Prepares attachments.
        */
        void DoSendUiPrepareAttachmentsL();

    protected: // data

        // assigned to iOperationState in base class
        enum TUniEditorSendUiState
            {
            EUniEditorSendUiCheck,
            EUniEditorSendUiPrepareObject,
            EUniEditorSendUiPrepareAttachments,
            EUniEditorSendUiEnd
            };

        TUint32 iDeleteAllLimit;

        CUniEditorProcessImageOperation* iImageOperation;
        CUniEditorConvertVideoOperation*  iVideoOperation;

        TInt iObjectsSize;
        TInt iPreviousSize;
        TInt iObjectNum;
        TInt iSlideNum;
        CUniObject* iPreparedObject;
        TBool iDeleteAll;
        CUniEditorProcessTextOperation* iTextOperation;

    };

#endif // __UNIEDITORSENDUIOPERATION_H
