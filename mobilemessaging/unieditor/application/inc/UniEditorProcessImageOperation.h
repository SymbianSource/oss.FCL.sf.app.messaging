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
* Description:   CUniEditorProcessImageOperation class definition.      
*
*/



#ifndef __UNIEDITORPROCESSIMAGEOPERATION_H
#define __UNIEDITORPROCESSIMAGEOPERATION_H

// INCLUDES

#include <e32base.h>
#include <msvapi.h>

#include <uniobject.h>
#include <uniimageprocessor.h>
#include <cmsvattachment.h>

#include "UniEditorOperation.h"

// FORWARD DECLARATIONS

class CMsgImageInfo; 
class CMsgImageControl;
class CUniEditorDocument;
class MMsvAttachmentManager;
class CMsvStore;

// DATA TYPES

// CLASS DECLARATION

/**
* CUniEditorProcessImageOperation
*
* @since 3.2
*/
class CUniEditorProcessImageOperation : public CUniEditorOperation,
                                        public MUniImageProcessorCallback
    {
    public: // new operations

        /**
        * Factory method
        */
        static CUniEditorProcessImageOperation* NewL( 
            MUniEditorOperationObserver& aObserver,
            CUniEditorDocument& aDocument,
            RFs& aFs );

        /**
        * Start image process operation
        *
        * @param aImageInfo
        * @param aAttachmentId  Attachment id, if the image is already stored as an attachment
        *                       KMsvNullIndexEntryId, otherwise.
        * @param aMessageSize
        */
        void Process( CMsgImageInfo* aImageInfo, TMsvAttachmentId aAttachmentId, TInt aMessageSize );

        /**
        * Destructor
        */
        virtual ~CUniEditorProcessImageOperation();

        /**
        * Detaches image info object.
        */
        CMsgImageInfo* DetachImageInfo();

        /**
        * Detaches attachment id.
        */
        void DetachAttachmentId( TMsvAttachmentId& aAttachmentId );

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
        * From CUniEditorOperation
        */
        void DoCancelCleanup();
        
        /**
        * From MUniImageProcessorCallback
        */
        void ImageProcessingReady( TSize aBitmapSize, TInt aFileSize, TBool aCompressed );
        
    private: // new operations

        /**
        * C++ constructor
        */
        CUniEditorProcessImageOperation( MUniEditorOperationObserver& aObserver,
                                         CUniEditorDocument& aDocument,
                                         RFs& aFs );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Returns optimal send image size.
        */
        TSize ImageSendSize() const;

        /**
        * Checks whether some processing is needed for the image.
        * Stores the possible processing method to "iProcessMethod".
        *
        * @return EFalse if aborted by user,
        *         ETrue  otherwise
        */
        TBool CheckNeedToProcess();

        /**
        * Creates empty attachment.
        */
        void CreateEmptyAttachmentL();

        /**
        * Start check step.
        */
        void DoStartCheck();

        /**
        * Start process step.
        */
        void DoStartProcessL();

        /**
        * Start resolve step.
        */
        void DoStartResolveL();

        /**
        * Processing ready.
        */
        void DoReady();

        /**
        * Performs error handling without state change.
        */
        void DoErrorWithoutStateChange();
        
        /**
        * Opens file by file name. If it fails with KErrPermissionDenied,
        * file is searched from message store and opened from there        
        */
        RFile OpenFileForReadingL();
        
    private: // data

        enum TUniProcessStates
            {
            EUniProcessImgCheck,
            EUniProcessImgProcess,
            EUniProcessImgResolve,
            EUniProcessImgReady,
            EUniProcessImgError
            };

        enum TUniProcessMethods
            {
            EUniProcessImgMethodNone        = 0x0,
            EUniProcessImgMethodScale       = 0x1,
            EUniProcessImgMethodConvert     = 0x2,
            EUniProcessImgMethodCompress    = 0x4
            };

        CMsgImageInfo*              iImageInfo;
        TMsvAttachmentId            iAttachmentId;
        TInt                        iMessageSize;

        CMsgImageInfo*              iNewImageInfo;
        RFile                       iNewImageFile;
        TMsvAttachmentId            iNewAttaId;
        CMsvStore*                  iEditStore;
    
        CUniImageProcessor*         iImageProcessor;

        TInt                        iProcessMethod;

        TSize                       iScaleSize;
        TSize                       iScaleSizeResult;
        TDataType                   iTargetType;
        TBool                       iExactImageScaling;
    };

#endif //__UNIEDITORPROCESSIMAGEOPERATION_H
