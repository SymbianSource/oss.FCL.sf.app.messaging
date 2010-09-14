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
* Description:   CUniEditorLaunchOperation, asynchronous operation for object saving
*
*/



#ifndef __UNIEDITORLAUNCHOPERATION_H
#define __UNIEDITORLAUNCHOPERATION_H

// INCLUDES

#include <e32base.h>
#include <e32std.h>

#include <unislideloader.h>

#include "UniEditorOperation.h"
#include "UniEditorDocument.h"
#include "UniEditorSendUiOperation.h"
#include "UniEditorProcessTextOperation.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

class MMsgAsyncControlObserver;
class MUniDataModelObserver;
class CMsgEditorView;
class CMsgEditorAppUi;
class CUniEditorHeader;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CUniEditorLaunchOperation
*
* @since 3.2
*/
class CUniEditorLaunchOperation : public CUniEditorOperation,
                                  public MUniSlideLoaderObserver,
                                  public MUniDataModelObserver
    {
    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @param    aObserver
        * @return   Pointer to instance in cleanup stack
        */
        static CUniEditorLaunchOperation* NewL(
            MMsgAsyncControlObserver& aControlObserver,
            MUniEditorOperationObserver& aOperationObserver,
            CUniEditorDocument& aDocument,
            CMsgEditorView& aView,
            CMsgEditorAppUi& aAppUi,
            RFs& aFs );

        /**
        * Destructor
        */
        virtual ~CUniEditorLaunchOperation();

        /**
        * Start launch process
        */
        void Launch();

        /**
        * Detach header object
        */
        inline CUniEditorHeader* DetachHeader();

        /**
        * Detach slide loader object
        */
        inline CUniSlideLoader* DetachSlideLoader();

        /**
        * Return parse result
        */
        TInt ParseResult();

        /**
        * From MUniDataModelObserver
        */
        void RestoreReady( TInt aParseResult, TInt aError );
            
		/**
        * check Is optimized flow
        */  
        TBool IsOptimizedFlagSet();

    protected: 

        /**
        * From CActive
        */
        void RunL();
        
        /**
        * From CActive
        */
        TInt RunError( TInt aError );

        /**
        * From MUniSlideLoaderObserver
        */
        void SlideLoadReady( TInt aError );
        
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
        CUniEditorLaunchOperation( MMsgAsyncControlObserver& aControlObserver,
                                   MUniEditorOperationObserver& aOperationObserver,
                                   CUniEditorDocument& aDocument,
                                   CMsgEditorView& aView,
                                   CMsgEditorAppUi& aAppUi,
                                   RFs& aFs );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Performs lauch step
        */
        void DoLaunchStepL();

        /**
        * Initializes document.
        */
        void DoInitializeDocL();

        /**
        * Initializes data model
        */
        void DoInitializeModelL();

        /**
        * Processes message 
        */
        void DoHandleMessageL();

        /**
        * Prepares header
        */
        void DoPrepareHeaderL();

        /**
        * Prepares body
        */
        void DoPrepareBodyL();

        /**
        * Prepares 3GPP body
        */
        void DoPrepare3GPPBodyL();

        /**
        * Add subject prefix ("Re:" or "Fw:")
        * @param ETrue, for reply -> "Re:"
        *        EFalse, for forward -> "Fw:"
        */
        void AddSubjectPrefixL( TBool aReply );

    protected: // data

        // assigned to iOperationState in base class
        enum TUniLaunchState
            {
            EUniLaunchInitializeDoc = 0,
            EUniLaunchInitializeModel,
            EUniLaunchHandleMessage,
            EUniLaunchPrepareHeader,
            EUniLaunchPrepareBody,
            EUniLaunchEnd
            };

        MMsgAsyncControlObserver&       iControlObserver;
        CMsgEditorView&                 iView;
        CMsgEditorAppUi&                iAppUi;

        CUniEditorSendUiOperation*      iSendUiOperation;
        CUniEditorHeader*               iHeader;
        CUniSlideLoader*                iSlideLoader;
        
        CUniEditorProcessTextOperation* iTextOperation;

        TInt                            iError;
        TBool                           iSmilEditorSupported;
        // count of failed image operations
        TInt                            iParseResultTemp;
		// sendui+jepg optimization changes
        TBool 							iOptimizedFlow;

    };

#include "UniEditorLaunchOperation.inl"

#endif // __UNIEDITORLAUNCHOPERATION_H

// End of file

