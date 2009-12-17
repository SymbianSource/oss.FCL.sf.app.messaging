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
* Description:   Provides CUniEditorProcessImageOperation methods.
*
*/



// ========== INCLUDE FILES ================================

#include <apmstd.h>

#include <icl/imagedata.h>

#include <centralrepository.h>

#include <messagingvariant.hrh>
#include <messaginginternalcrkeys.h> // for Central Repository keys

#include <MsgMediaResolver.h>
#include <MsgImageInfo.h>
#include <MmsConformance.h>
#include <MsgMimeTypes.h>

#include <uniimageprocessor.h>
#include <unimsventry.h>

#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <cmsvattachment.h>

#include <msgtextutils.h>

#include <mmssettingsdefs.h>

#include "UniClientMtm.h"
#include "UniEditorEnum.h"
#include "UniEditorDocument.h"
#include "UniEditorProcessImageOperation.h"

// ========== CONSTANTS ====================================

// Leave some space after compression so that text can be inserted to the 
// message.
const TInt KUniCompressionMargin = 10 * 1024; // 10 kB

const TInt KUniMmsUploadImageWidth = 1600;
const TInt KUniMmsUploadImageHeight = 1200;


_LIT8( KUniExtImageJpeg_8, ".jpg" );
_LIT8( KUniExtImageGif_8,  ".gif" );

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::NewL
//
// Factory method.
// ---------------------------------------------------------
//
CUniEditorProcessImageOperation* CUniEditorProcessImageOperation::NewL( 
        MUniEditorOperationObserver& aObserver,
        CUniEditorDocument& aDocument,
        RFs& aFs )
    {
    CUniEditorProcessImageOperation* self = new ( ELeave ) CUniEditorProcessImageOperation(
            aObserver,
            aDocument,
            aFs );
            
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::CUniEditorProcessImageOperation.
// ---------------------------------------------------------
//
CUniEditorProcessImageOperation::CUniEditorProcessImageOperation(
        MUniEditorOperationObserver& aObserver,
        CUniEditorDocument& aDocument,
        RFs& aFs ) :
    CUniEditorOperation( aObserver, aDocument, aFs, EUniEditorOperationProcessImage ),
    iNewAttaId( KMsvNullIndexEntryId )
    {
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::ConstructL()
    {
    BaseConstructL();

    TInt featureBitmask( 0 );
    
    CRepository* repository = CRepository::NewL( KCRUidMuiuVariation );
    repository->Get( KMuiuMmsFeatures, featureBitmask );
    
    iExactImageScaling = ( featureBitmask & KMmsFeatureIdExactImageScaling );
    
    delete repository;
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::~CUniEditorProcessImageOperation
// ---------------------------------------------------------
//
CUniEditorProcessImageOperation::~CUniEditorProcessImageOperation()
    {
    Cancel();
    
    delete iNewImageInfo;
    iNewImageFile.Close();
    delete iEditStore;
    delete iImageProcessor;
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::Process
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::Process( CMsgImageInfo* aImageInfo,
                                               TMsvAttachmentId aAttachmentId,
                                               TInt aMessageSize )
    {
    ResetErrors();
    
    iImageInfo = aImageInfo;
    iAttachmentId = aAttachmentId;
    iMessageSize = aMessageSize;
    iOperationState = EUniProcessImgCheck;
    
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::DetachAttachmentId
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::DetachAttachmentId( TMsvAttachmentId& aAttachmentId )
    {
    // iNewImageInfo may be already detached in DetachImageInfo
    aAttachmentId = iNewAttaId;
    iNewAttaId = KMsvNullIndexEntryId;
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::RunL
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::RunL()
    {
    PrintOperationAndState();
    if ( iStatus.Int() != KErrNone )
        {
        SetError( iStatus.Int() );
        iOperationState = EUniProcessImgError;
        }
    
    switch ( iOperationState )
        {
        case EUniProcessImgCheck:
            {
            DoStartCheck();
            break;
            }
        case EUniProcessImgProcess:
            {
            DoStartProcessL();
            break;
            }
        case EUniProcessImgResolve:
            {
            DoStartResolveL();
            break;
            }
        case EUniProcessImgReady:
            {
            iObserver.EditorOperationEvent( EUniEditorOperationProcessImage,
                                            EUniEditorOperationComplete );
            break;
            }
        case EUniProcessImgError:
            {
            DoErrorWithoutStateChange();
            iObserver.EditorOperationEvent( EUniEditorOperationProcessImage,
                                            EUniEditorOperationError );
            break;
            }
        default:
            {
            iObserver.EditorOperationEvent( EUniEditorOperationProcessImage,
                                            EUniEditorOperationError );
            break;
            }
        }    
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::DoCancelCleanup
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::DoCancelCleanup()
    {
    if ( iImageProcessor )
        {
        iImageProcessor->Cancel();
        }
        
    DoErrorWithoutStateChange();
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::DoStartCheck
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::DoStartCheck()
    {
    if ( !CheckNeedToProcess() )
        {
        iOperationState = EUniProcessImgError;
        }
    else if ( iProcessMethod == EUniProcessImgMethodNone )
        {
        iOperationState = EUniProcessImgReady;
        }
    else
        {
        iOperationState = EUniProcessImgProcess;
        }
    CompleteSelf( KErrNone );
    }
  
// ---------------------------------------------------------
// CUniEditorProcessImageOperation::DoStartProcessL
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::DoStartProcessL()
    {
    CreateEmptyAttachmentL();
    
    if ( !iImageProcessor )
        {
        iImageProcessor = new( ELeave )CUniImageProcessor( this );
        }
        
    RFile sourceFile = OpenFileForReadingL();
    
    CleanupClosePushL( sourceFile );
    
    iImageProcessor->ProcessImageL( sourceFile,
                                    iNewImageFile,
                                    iScaleSize,
                                    iTargetType.Des8(),
                                    ETrue, // Always maintain aspect ratio
                                    iDocument.MaxMessageSize() - KUniCompressionMargin );
    SetPending();
                   
    CleanupStack::PopAndDestroy(); // sourceFile;
    }
  
// ---------------------------------------------------------
// CUniEditorProcessImageOperation::DoStartResolveL
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::DoStartResolveL()
    {
    iNewImageInfo = static_cast<CMsgImageInfo*>(iDocument.DataModel()->MediaResolver().CreateMediaInfoL( iNewImageFile ) );
    iDocument.DataModel()->MediaResolver().ParseInfoDetailsL( iNewImageInfo, iNewImageFile );

    iOperationState = EUniProcessImgReady;

    iNewImageFile.Close();
    
    __ASSERT_DEBUG( iEditStore, Panic( EUniNullPointer ) );
    
    iEditStore->CommitL();
    delete iEditStore;
    iEditStore = NULL;

    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::DoErrorWithoutStateChange
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::DoErrorWithoutStateChange()
    {
    iNewImageFile.Close();
    delete iNewImageInfo;
    iNewImageInfo = NULL;
    
    delete iEditStore;
    iEditStore = NULL;
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::ImageProcessingReady
//
// Image Compressor callback implementation.
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::ImageProcessingReady( TSize aBitmapSize, TInt aFileSize, TBool aCompressed )
    {
    TInt err = iImageProcessor->Error();
    
    if ( err == KErrNone &&
         ( aBitmapSize.iWidth == 0 || aBitmapSize.iHeight == 0 ||
           ( aCompressed &&
            ( aFileSize == 0 ||
              aFileSize > ( iDocument.MaxMessageSize() - KUniCompressionMargin ) ) ) ) )
        {
        err = KErrGeneral;
        }
        
    switch ( err )
        {
        case KErrNone:
            {
            iOperationState = EUniProcessImgResolve;
            if ( aCompressed )
                {
                SetError( EUniProcessImgCompressSuccessful );
                }
            break;
            }
        case KErrNoMemory:
            {
            iOperationState = EUniProcessImgError;
            SetError( EUniProcessImgOutOfMemory );
            break;
            }
        case KErrDiskFull:
            {
            iOperationState = EUniProcessImgError;
            SetError( EUniProcessImgOutOfDisk );
            break;
            }
        case KErrNotFound:
            {
            iOperationState = EUniProcessImgError;
            SetError( EUniProcessImgNotFound );
            break;
            }
        default:
            {
            iOperationState = EUniProcessImgError;
            if ( aCompressed )
                {
                SetError( EUniProcessImgCompressFailed );
                }
            else
                {
                SetError( EUniProcessImgScalingFailed );
                }
            break;
            }
        }
    
    if ( err == KErrCancel )
        {
        CompleteOperation( KErrCancel );
        }
    else
        {
        CompleteOperation( KErrNone );
        }
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::CheckNeedToProcess
// 
// Checks if scaling/converting/compression is needed.
// ---------------------------------------------------------
//
TBool CUniEditorProcessImageOperation::CheckNeedToProcess()
    {
    iProcessMethod = EUniProcessImgMethodNone;
    
    TMmsConformance conformance = 
        iDocument.DataModel()->MmsConformance().MediaConformance( *iImageInfo );
    
    if ( conformance.iCanAdapt == EFalse )
        {
        return ETrue;
        }
        
    TSize origSize = iImageInfo->Dimensions();
    
    if ( origSize.iWidth == 0 || origSize.iHeight == 0 )
        {    
        // Cannot get size -> corrupted
        SetError( EUniProcessImgCorrupted );
        return ETrue;
        }
    
    TSize scaleSize = ImageSendSize();
    TSize optimizedSize = origSize;
    
    while ( optimizedSize.iWidth > scaleSize.iWidth ||
            optimizedSize.iHeight > scaleSize.iHeight )
        {
        // Largest possible (1/2)^n size
        optimizedSize.iWidth >>= 1;
        optimizedSize.iHeight >>= 1;
        }
        
    if ( scaleSize.iWidth < origSize.iWidth ||
         scaleSize.iHeight < origSize.iHeight )
        {
        if ( !iExactImageScaling &&
            ( scaleSize.iWidth > KMmsUniImageSmallWidth ||
              scaleSize.iHeight > KMmsUniImageSmallHeight ) )
            {
            // Use optimized (1/2^n) size when scaling
            // to larger than "Small"
            scaleSize = optimizedSize;
            }
        // else -> scale to exact (non-optimized) size
        
        iProcessMethod |= EUniProcessImgMethodScale;
        }
    else
        {
        // Scaling not needed. Check possible conversion need.
        scaleSize = origSize;
        
        if ( conformance.iConfStatus & EMmsConfNokConversionNeeded )
            {
            // Conversion needed.
            iProcessMethod |= EUniProcessImgMethodConvert;
            }
        }

    if ( !( iProcessMethod & EUniProcessImgMethodScale ) &&
         ( iImageInfo->FileSize() + iMessageSize ) > iDocument.MaxMessageSize() &&
         iImageInfo->MimeType().CompareF( KMsgMimeImageJpeg ) == 0 &&
         iMessageSize < KUniCompressionMargin )
        {
        // Only compression needed as image is JPEG that is larger than can be fitted
        // into the message and scaling is not performed. Also current message size
        // is under comression margin.
        iProcessMethod |= EUniProcessImgMethodCompress;
        }
    
    TBool largeImageQuery = EFalse;
    
    if ( iProcessMethod == EUniProcessImgMethodNone )
        {
        // Image won't be processed
        if ( ( origSize.iWidth > KImageRichWidth ||
               origSize.iHeight > KImageRichHeight ) &&
             ( iImageInfo->FileSize() + iMessageSize ) < iDocument.MaxMessageSize() )
            {
            // Original image width or height is "non-conformant" and original image would 
            // fit to into the message without any processing.
            largeImageQuery = ETrue;
            }
        }
    else
        {
        // Image will be processed
        if ( scaleSize.iWidth > KImageRichWidth ||
             scaleSize.iHeight > KImageRichHeight )
            {
            // Processed image is "non-conformant" after processing.
            largeImageQuery = ETrue;
            }
        }

    if ( largeImageQuery &&  iDocument.CreationMode() == EMmsCreationModeWarning )
        {
        // Observer will make the final decision whether it will show the query or not.
        if ( !iObserver.EditorOperationQuery( EUniEditorOperationProcessImage,
                                              EMEOQueryGuidedInsertLarge ) )
            {
            SetError( EUniProcessImgUserAbort );
            return EFalse; // Abort
            }
        }
        
    iScaleSize = scaleSize;
    return ETrue;
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::CreateEmptyAttachmentL
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::CreateEmptyAttachmentL()
    {
    iNewAttaId = KMsvNullIndexEntryId;

    // Get the file name from original full path name.
    TParsePtrC parser( iImageInfo->FullFilePath() );

    TFileName ext( parser.Ext() );
    iTargetType = iImageInfo->MimeType();

    if ( iTargetType.Des8().CompareF( KMsgMimeImagePng ) == 0 )
        {
        //png is non-conformant image type
        //->convert to jpeg
        iTargetType = TDataType( KMsgMimeImageJpeg );
        ext.Zero();
        ext.Copy( KUniExtImageJpeg_8 );
        }
    else if ( iTargetType.Des8().CompareF( KMsgMimeImageWbmp ) == 0 )
        {
        //no wbmp encoder
        //->convert to gif if scaling is needed
        iTargetType = TDataType( KMsgMimeImageGif );
        ext.Zero();
        ext.Copy( KUniExtImageGif_8 );
        }

    TFileName newFileName( parser.Name() );
    newFileName.Append( ext );

    iEditStore = iDocument.Mtm().Entry().EditStoreL();
    MMsvAttachmentManagerSync& managerSync = iEditStore->AttachmentManagerExtensionsL();
    CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( attachment );
    
    attachment->SetMimeTypeL( iTargetType.Des8() );

    managerSync.CreateAttachmentL( newFileName, iNewImageFile, attachment );
    CleanupStack::Pop( attachment ); // ownership transferred
    
    iNewAttaId = attachment->Id();
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::ImageSendSize
// ---------------------------------------------------------
//
TSize CUniEditorProcessImageOperation::ImageSendSize() const
    {
    TSize size;
    if( TUniMsvEntry::IsMmsUpload( iDocument.Entry() ) )
        {
        size.iWidth = KUniMmsUploadImageWidth;
        size.iHeight = KUniMmsUploadImageHeight;
        }
    else
        {
        size =  iDocument.MaxImageSize();
        }
    return size;
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::OpenFileForReadingL
// ---------------------------------------------------------
//
RFile CUniEditorProcessImageOperation::OpenFileForReadingL()
    {
    RFile sourceFile;
    if ( iAttachmentId != KMsvNullIndexEntryId )
        {
        __ASSERT_DEBUG( iEditStore, Panic( EUniNullPointer ) );
        sourceFile = iEditStore->AttachmentManagerL().GetAttachmentFileL( iAttachmentId );
        }
    else
        {
        TInt err = sourceFile.Open(
            iFs, iImageInfo->FullFilePath(), EFileRead | EFileShareAny );
        if ( err )
            {
            err = sourceFile.Open(
                iFs, iImageInfo->FullFilePath(), EFileRead | EFileShareReadersOnly );
                
            User::LeaveIfError( err );
            }
        }
    return sourceFile;
    }
    
// ---------------------------------------------------------
// CUniEditorProcessImageOperation::RunError
// ---------------------------------------------------------
//
TInt CUniEditorProcessImageOperation::RunError( TInt aError )
    {
    delete iImageProcessor;
    iImageProcessor = NULL;
    
    if ( aError == KErrNotFound )
        {
        aError = EUniProcessImgNotFound;
        }
    else if ( aError == KErrNoMemory )
        {
        aError = EUniProcessImgOutOfMemory;
        }
    else if ( aError == KErrDiskFull )
        {
        aError = EUniProcessImgOutOfDisk;
        }
    else if ( aError == KErrCorrupt )
        {
        aError = EUniProcessImgCorrupted;
        }
        
    return CUniEditorOperation::RunError( aError );
    }
    
// ---------------------------------------------------------
// CUniEditorProcessImageOperation::DetachImageInfo
// ---------------------------------------------------------
//
CMsgImageInfo* CUniEditorProcessImageOperation::DetachImageInfo()
    {
    // ownership transferred
    CMsgImageInfo* tempInfo = iNewImageInfo;
    iNewImageInfo = NULL;
    return tempInfo;
    }
    
// End of file
