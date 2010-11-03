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
* Description:   CUniEditorConvertVideoOperation class definition.      
*
*/



#ifndef __UNIEDITORCONVERTVIDEOOPERATION_H
#define __UNIEDITORCONVERTVIDEOOPERATION_H

// INCLUDES

#include <e32base.h>
#include <msvapi.h>

#include <uniobject.h>
#include <cmsvattachment.h>

#include "UniEditorOperation.h"
#include "VideoConversionApi.h"

// FORWARD DECLARATIONS

class CMsgVideoInfo; 
class CUniEditorDocument;
class MMsvAttachmentManager;
class CMsvStore;

// DATA TYPES

// CLASS DECLARATION

/**
* CUniEditorConvertVideoOperation
*
* @since 3.2
*/
class CUniEditorConvertVideoOperation : public CUniEditorOperation,
                                        public MVideoConversionCallback
    {
    public: // new operations

        /**
        * Factory method
        */
        static CUniEditorConvertVideoOperation* NewL( 
            MUniEditorOperationObserver& aObserver,
            CUniEditorDocument& aDocument,
            RFs& aFs );

        
        /**
        * Returns whether converter is implemented
        */
        static TBool IsImplementedL();
        
        /**
        * Start video convert operation
        *
        * @param aImageInfo
        * @param aAttachmentId  Attachment id, if the image is already stored as an attachment
        *                       KMsvNullIndexEntryId, otherwise.
        * @param aMessageSize
        */
        void Process(         CMsgVideoInfo* aVideoInfo,
        TMsvAttachmentId aAttachmentId,
        TInt aMessageSize );

        /**
        * Destructor
        */
        virtual ~CUniEditorConvertVideoOperation();
        
        /**
        * Detaches video info object.
        */
        CMsgVideoInfo* DetachVideoInfo();
        
        /**
        * Detaches attachment id
        */
        void DetachAttachmentId( TMsvAttachmentId& aAttachmentId );

    protected:

        /**
        * From CActive.
        */
        void RunL();

        /**
        * From CUniEditorOperation
        */
        void DoCancelCleanup();

        /**
        * From MVideoConversionCallback. Called when
        * video conversion has been completed by video
        * conversion plugin
        */
        void VideoConversionDone( TInt aStatus );

    private: // new operations

        /**
        * C++ constructor
        */
        CUniEditorConvertVideoOperation( MUniEditorOperationObserver& aObserver,
                                         CUniEditorDocument& aDocument,
                                         RFs& aFs );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();


        /**
        * Creates empty attachment
        */
        void CreateEmptyAttachmentL();

        /**
        * Performs processing start
        */
        void DoStartProcessL();

        /**
        * Start resolving
        */
        void DoStartResolveL();

        /**
        * Performs actions when conversion is ready
        */
        void DoReady();

        /**
        * Performs error handling without state change.
        */
        void DoErrorWithoutStateChange();
        
        /**
        * Opens file by file name. If it fails with KErrPermissionDenied,
        * file is searched from message store and opened from there.
        */
        RFile OpenFileForReadingL();
        
    private: // data

        // assigned to iOperationState in base class
        enum TUniProcessStates
            {
            EUniConvertVidCheck,
            EUniConvertVidProcess,
            EUniConvertVidResolve,
            EUniConvertVidReady,
            EUniConvertVidError
            };
            
        TInt iTargetSize;    
            
        CMsgVideoInfo*              iVideoInfo;
        TMsvAttachmentId            iAttachmentId;
        
        CMsgVideoInfo*              iNewVideoInfo;
        RFile                       iNewVideoFile;
        TMsvAttachmentId            iNewAttaId;
        CMsvStore*                  iEditStore;
    
        CVideoConversionPlugin*     iConverter;
    };

#endif //__UNIEDITORCONVERTVIDEOOPERATION_H
