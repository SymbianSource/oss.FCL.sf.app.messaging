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
* Description:   CUniEditorSendOperation, asynchronous object for message sending
*
*/



#ifndef __UNIEDITORSENDOPERATION_H
#define __UNIEDITORSENDOPERATION_H

// INCLUDES
#include "UniEditorOperation.h"
#include "UniEditorDocument.h"
#include "UniEditorSaveOperation.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

class CMDXMLDocument;
class CUniEditorHeader;
class CMsgEditorView;
class CMsgEditorAppUi;
class CMsvOperation;
class CUniEditorPlugin;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CUniEditorSendOperation, Storage for single attachment in presentation. 
*
* @since 3.2
*/
class CUniEditorSendOperation : public CUniEditorOperation
    {
    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @since    3.2
        * @param    aObserver
        * @return   Pointer to instance in cleanup stack
        */
        static CUniEditorSendOperation* NewL(
            MUniEditorOperationObserver& aObserver,
            CUniEditorDocument& aDocument,
            CUniEditorHeader& aHeader,
            CUniEditorPlugin& aPlugin,
            CMsgEditorView& aView,
            RFs& aFs );

        /**
        * Destructor
        */
        virtual ~CUniEditorSendOperation();

        /**
        * Starts send operation
        */
        void Send();

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
        CUniEditorSendOperation(
            MUniEditorOperationObserver& aObserver,
            CUniEditorDocument& aDocument,
            CUniEditorHeader& aHeader,
            CUniEditorPlugin& aPlugin,
            CMsgEditorView& aView,
            RFs& aFs );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Performs send step
        */
        void DoSendStepL();

        /**
        * Removes duplicate addresses
        */
        void DoRemoveDuplicateAddressesL();

        /**
        * Saves the message
        */
        void DoSave();

        /**
        * Converts the message
        */
        void DoConvertL();

        /**
        * Sends the messsage
        */
        void DoSendL();
        
    protected: // data

        // assigned to iOperationState in base class
        enum TUniEditorSendState
            {
            EUniEditorSendRemoveDuplicateAddresses,
            EUniEditorSendSave,
            EUniEditorSendConvert,
            EUniEditorSendPluginSend,
            EUniEditorSendEnd
            };

        CUniEditorHeader& iHeader;
        CUniEditorPlugin& iPlugin;
        CMsgEditorView& iView;

        CUniEditorSaveOperation* iSaveOperation;

        TMsvId iConvertedEntry;
    };

#endif // __UNIEDITORSENDOPERATION_H
