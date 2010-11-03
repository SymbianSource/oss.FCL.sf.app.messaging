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
* Description:   CUniEditorInsertOperation
*
*/



#ifndef __UNIEDITORINSERTOPERATION_H
#define __UNIEDITORINSERTOPERATION_H

// INCLUDES

#include <e32base.h>
#include <msvapi.h>

#include "uniobject.h"
#include "unislideloader.h"

#include "UniEditorOperation.h"


// FORWARD DECLARATIONS

class CUniEditorDocument;
class CMsgEditorView;
class CMsgMediaInfo; 
class CUniSlideLoader;
class CUniEditorHeader;
class CUniEditorChangeSlideOperation;
class CUniEditorProcessImageOperation;
class CUniEditorConvertVideoOperation;

// DATA TYPES

enum TUniInsertState
    {
    EUniInsertIdle = 0,
    EUniInsertCheckScaling,
    EUniInsertRecheckScaling,
    EUniInsertCreateAttachment,
    EUniInsertCopyAttachment,
    EUniInsertProcessImage,
    EUniInsertConvertVideo,
    EUniInsertCheckMode,
    EUniInsertCheckSize,
    EUniInsertResolve,
    EUniInsertAddToModel,
    EUniInsertLoad,
    EUniInsertReady,
    EUniInsertError,
    EUniInsertErrorReload,
    EUniInsertErrorReport
    };
    
// CLASS DECLARATION

/**
* CUniEditorInsertOperation
*
* @since 3.2
*/
class CUniEditorInsertOperation : public CUniEditorOperation,
                                  public MUniObjectSaveObserver,
                                  public MUniSlideLoaderObserver
    {
    public: // new operations

        /**
        * Factory functions
        */
        static CUniEditorInsertOperation* NewL( 
            MUniEditorOperationObserver& aOperationObserver,
            CUniEditorDocument& aDocument,
            CUniEditorHeader& aHeader,
            CUniSlideLoader& aSlideLoader,
            CMsgEditorView& aView,
            RFs& aFs );

        /**
        * Starts insert operation
        */
        void Insert( CMsgMediaInfo* aInfo, TBool aAddSlide, TBool aAddAttachment );

        /**
        * Destructor
        */
        virtual ~CUniEditorInsertOperation();

        /**
        * Return inserted media type.
        */
        TMsgMediaType InsertedType();
        
        /**
        * Returns inserted object.
        */
        CUniObject* InsertedObject();
        
        /**
        * Return ETrue if object was inserted as attachment. EFalse otherwise.
        */
        TBool IsAddedAsAttachment() const;

    protected: 

        /**
        * From CActive
        */
        void RunL();

        /**
        * From MUniObjectSaveObserver
        */
        void ObjectSaveReady( TInt aError );

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

        
    private: // new operations

        /**
        * C++ constructor
        */
        CUniEditorInsertOperation( MUniEditorOperationObserver& aOperationObserver,
                                   CUniEditorDocument& aDocument,
                                   CUniEditorHeader& aHeader,
                                   CUniSlideLoader& aSlideLoader,
                                   CMsgEditorView& aView,
                                   RFs& aFs );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Starts image processing
        */
        void DoStartProcessImageL();

        /**
        * Starts video conversion
        */
        void DoStartConvertVideoL();

        /**
        * Performs size check
        */
        void DoCheckSizeL();

        /**
        * Performs mode check
        */
        void DoCheckModeL();

        /**
        * Starts attachment copying
        */
        void DoStartCopyAttachmentL();

        /**
        * Starts loading
        */
        void DoStartLoadL();

        /**
        * Reloads current (old) slide after error.
        */
        void DoStartReloadL();

        /**
        * Performs error handling without state change.
        */
        void DoErrorWithoutStateChangeL();

        /**
        * Checks insert size.
        */
        TBool CheckSizeForInsertL();

        /**
        * Checks mode.
        *
        * @param aBeforeProcessing indicates whether the function is called
        *                          before or after image processing
        * 
        */
        TBool CheckModeForInsert( TBool aBeforeProcessing );

    private: // data

        CUniEditorHeader& iHeader;
        CUniSlideLoader& iSlideLoader;
        CMsgEditorView& iView;

        CUniEditorProcessImageOperation* iImageOperation;
        CUniEditorConvertVideoOperation* iVideoOperation;
        CUniEditorChangeSlideOperation* iChangeSlideOperation;
    
        CMsgMediaInfo*  iOldInfo;
        CMsgMediaInfo*  iNewInfo;
        CUniObject*     iInsertedObject;
        TBool           iAddSlide;
        TInt            iInsertedSlide;
        TBool           iAddAsAttachment;
        TBool           iPreviousSlide;
    };

#endif // __UNIEDITORINSERTOPERATION_H

// End of file
