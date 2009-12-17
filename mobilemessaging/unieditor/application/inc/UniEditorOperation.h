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
* Description:   CUniEditorOperation  
*
*/



#ifndef __UNIEDITOROPERATION_H
#define __UNIEDITOROPERATION_H

// INCLUDES

#include <e32base.h>
#include <f32file.h>

#include "UniEditorDocument.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

// DATA TYPES

enum TUniEditorOperationType
    {
    EUniEditorOperationChangeSlide,
    EUniEditorOperationInsert,
    EUniEditorOperationLaunch,
    EUniEditorOperationPreview,
    EUniEditorOperationSave,
    EUniEditorOperationProcessImage,
    EUniEditorOperationSend,
    EUniEditorOperationSendUi,
    EUniEditorOperationSetSender,
    EUniEditorOperationVCard,
    EUniEditorOperationConvertVideo,
    EUniEditorOperationProcessText
    };

enum TUniEditorOperationEvent
    {
    EUniEditorOperationProcessing,
    EUniEditorOperationComplete,
    EUniEditorOperationError,
    // Cancel is called for active operation
    EUniEditorOperationCancel
    };

enum TUniEditorOperationQuery
    {
    EMEOQueryGuidedInsertLarge,
    EMEOQueryGuidedObject
    };

enum TUniEditorOperationErrors
    {
    EUniInsertUserGuidedAbort = 1,
    EUniInsertTooBig,
    EUniInsertNotSupported,
    EUniInsertSlideChangeFailed,
    KUniLaunchAbortPresRestricted,
    KUniLaunchPresGuided,
    EUniProcessImgOutOfMemory,
    EUniProcessImgOutOfDisk,
    EUniProcessImgNotFound,
    EUniProcessImgNonConformant,
    EUniProcessImgCorrupted,
    EUniProcessImgUserAbort,
    EUniProcessImgCouldNotScale,
    EUniProcessImgScalingFailed,
    EUniProcessImgCompressFailed,
    EUniProcessImgCompressSuccessful, 
    EUniConvertVidOutOfMemory,
    EUniConvertVidOutOfDisk,
    EUniConvertVidNotFound,
    EUniConvertVidNonConformant,
    EUniConvertVidCorrupted,
    EUniConvertVidUserAbort,
    EUniConvertVidFailed,
    EUniConvertVidSuccessful, 
    EUniSendUiForbidden,
    EUniSendUiCorrupted,
    EUniSendUiUnsupported,
    EUniSendUiTooBig,
    EUniEditorExit  // when RunL in CMmsEditorLaunchOperation fails
    };



// FUNCTION PROTOTYPES

// CLASS DECLARATION

class MUniEditorOperationObserver
    {
    public:
    
        /**
        * Notifies operation event
        */
        virtual void EditorOperationEvent( TUniEditorOperationType aOperation,
                                           TUniEditorOperationEvent aEvent ) = 0;
        
        /**
        * Notifies operation query event.
        */                                 
        virtual TBool EditorOperationQuery( TUniEditorOperationType aOperation,
                                            TUniEditorOperationQuery aQuery ) = 0;
    };

/**
* CUniEditorOperation
*
* @since 3.2
*/
class CUniEditorOperation : public CActive,
                            public MUniEditorOperationObserver
    {
    public:  // New methods

        /**
        * Destructor
        */
        virtual ~CUniEditorOperation();

        /**
        * Returns error code array
        * @return Array of error codes
        */
        virtual CArrayFixFlat<TInt>* GetErrors();

    protected: // From base classes
    
        /**
        * From CActive
        */
        TInt RunError( TInt aError );

        /**
        * From CActive.
        */
        void DoCancel();
    
    protected: // New methods

        /**
        * Constructor.
        */
        CUniEditorOperation( MUniEditorOperationObserver& aObserver,
                             CUniEditorDocument& aDocument,
                             RFs& aFs,
                             TUniEditorOperationType aOperationType );

        /**
        * 2nd phase constructor.
        */
        void BaseConstructL();

        /**
        * CompleteSelf
        */
        void CompleteSelf( TInt aError );

        /**
        * Set error code to error code array
        *
        * @param TInt error code
        */
        void SetError( TInt aError );

        /**
        * Resets error code array
        */
        void ResetErrors();
        
        /**
        * 1) Saves the error status into error code array, if aError != KErrNone. 
        * 2) reports the observer of the error
        * 3) returns the error code. 
        * @return 0, operation can be continued
        *         otherwise, error status of CActive operation
        * 
        */
        TInt SetErrorAndReport( TInt aError );

        /**
        * Writes into trace log current operation and state
        */
        void PrintOperationAndState();
        
        /**
        * Sets operation pending.
        */
        void SetPending();
        
        /**
        * Completes operation
        */
        void CompleteOperation( TInt aError );
        
        /**
        * Implements cancellation of an outstanding operations.
	    *
	    * This function is called as part of the UniEditor operation cancelling.
	    *
	    * It must call the appropriate cancel function offered by the active object's 
	    * asynchronous service provider. Also open resource handles should be closed.
        *	
	    * DoCancelCleanup() must not wait for event completion; this is handled by Cancel().
	    *
	    * @see CActive::DoCancel
        */ 
        virtual void DoCancelCleanup() = 0;
        
        /**
        * Notifies operation event
        * Default implementation performs cancel operation handling
        * and forwards all other events to HandleOperationEvent function.
        *
        * @see MUniEditorOperationObserver::EditorOperationEvent
        */
        void EditorOperationEvent( TUniEditorOperationType aOperation,
                                   TUniEditorOperationEvent aEvent );
        
        /**
        * Notifies operation query event.
        * Default implementation forwards the request to operation observer.
        *
        * @note Derived class can overwrite this implementation if needed.
        *
        * @see MUniEditorOperationObserver::EditorOperationQuery
        */                                 
        TBool EditorOperationQuery( TUniEditorOperationType aOperation,
                                    TUniEditorOperationQuery aQuery );
        
        /**
        * Called from EditorOperationEvent to handle operation event.
        * Default implementation only completes the operation so operation with child operations
        * must overwrite this function to perform error handling.
        *
        * @param aOperation Completed operation.
        */    
        virtual void HandleOperationEvent( TUniEditorOperationType aOperation,
                                           TUniEditorOperationEvent aEvent );
        
    protected: // data

        MUniEditorOperationObserver&    iObserver;
        CUniEditorDocument&             iDocument;
        RFs&                            iFs;
        CArrayFixFlat<TInt>*            iErrors;
        TUniEditorOperationType         iOperationType;
        
        TInt                            iOperationState;
    };

#endif // __UNIEDITOROPERATION_H

// End of file

