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
* Description:   CUniEditorChangeSlideOperation, asynchronous operation for changing slide    
*
*/



#ifndef __UNIEDITORCHANGESLIDEOPERATION_H
#define __UNIEDITORCHANGESLIDEOPERATION_H

// INCLUDES
#include <unislideloader.h>

#include "UniEditorOperation.h"
#include "UniEditorDocument.h"
#include "UniEditorSaveOperation.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

class CUniEditorHeader;
class CUniSlideLoader;
class CMsgEditorView;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CUniEditorChangeSlideOperation, Storage for single attachment in presentation. 
*
* @since 3.2
*/
class CUniEditorChangeSlideOperation : public CUniEditorOperation,
                                       public MUniSlideLoaderObserver
    {
    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @since    3.2
        * @param    aObserver
        * @return   Pointer to instance in cleanup stack
        */
        static CUniEditorChangeSlideOperation* NewL( MUniEditorOperationObserver& aOperationObserver,
                                                     CUniEditorDocument& aDocument,
                                                     CUniEditorHeader& aHeader,
                                                     CUniSlideLoader& aSlideLoader,
                                                     CMsgEditorView& aView,
                                                     RFs& aFs );

        /**
        * Destructor
        */
        virtual ~CUniEditorChangeSlideOperation();

        /**
        * Start slide changing
        */
        void ChangeSlide( TInt aNextNum );

    private:

        /**
        * From MUniSlideLoaderObserver
        */
        void SlideLoadReady( TInt aError );

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
        
        /**
        * Performs change slide step
        */
        void DoChangeSlideStepL();

        /**
        * Performs save step
        */
        void DoSaveL();

        /**
        * Performs load step
        */
        void DoLoadL();

        /**
        * Performs finalization step
        */
        void DoFinalizeL();

    protected: 

        /**
        * Constructor.
        */
        CUniEditorChangeSlideOperation( MUniEditorOperationObserver& aOperationObserver,
                                        CUniEditorDocument& aDocument,
                                        CUniEditorHeader& aHeader,
                                        CUniSlideLoader& aSlideLoader,
                                        CMsgEditorView& aView,
                                        RFs& aFs );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    protected: // data

        enum TUniEditorChangeSlideState
            {
            EUniEditorChangeSlideSave,
            EUniEditorChangeSlideLoad,
            EUniEditorChangeSlideFinalize,
            EUniEditorChangeSlideEnd
            };

        CUniEditorHeader& iHeader;
        CUniSlideLoader& iSlideLoader;
        CMsgEditorView& iView;
        
        TInt iNextNum;

        CUniEditorSaveOperation* iSaveOperation;
    };

#endif // __UNIEDITORCHANGESLIDEOPERATION_H
