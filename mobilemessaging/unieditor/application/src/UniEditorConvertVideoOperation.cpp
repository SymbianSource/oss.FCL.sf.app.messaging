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
* Description:   CUniEditorConvertVideoOperation methods. 
*
*/



// ========== INCLUDE FILES ================================

#include <apmstd.h>

#include <centralrepository.h>

#include <messagingvariant.hrh>
#include <messaginginternalcrkeys.h> // for Central Repository keys

#include <mmsconst.h>

#include <MsgMediaResolver.h>
#include <MsgVideoInfo.h>
#include <MmsConformance.h>
#include <MsgMimeTypes.h>

#include <unimsventry.h>

#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <cmsvattachment.h>

#include <msgtextutils.h>

#include "UniClientMtm.h"
#include "UniEditorEnum.h"
#include "UniEditorDocument.h"
#include "UniEditorConvertVideoOperation.h"

// ========== CONSTANTS ====================================
_LIT8( KUniExtVideo3gp_8,  ".3gp" );
// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::NewL
//
// Factory method.
// ---------------------------------------------------------
//
CUniEditorConvertVideoOperation* CUniEditorConvertVideoOperation::NewL( 
        MUniEditorOperationObserver& aObserver,
        CUniEditorDocument& aDocument,
        RFs& aFs )
    {
    CUniEditorConvertVideoOperation* self = 
            new ( ELeave ) CUniEditorConvertVideoOperation( aObserver,
                                                            aDocument,
                                                            aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::IsImplementedL
// ---------------------------------------------------------
//
TBool CUniEditorConvertVideoOperation::IsImplementedL()
    {
    RImplInfoPtrArray implArray;
    CVideoConversionPlugin::ListImplementationsL( implArray );
    
    TBool result( EFalse );
    
    if ( implArray.Count() > 0)
        {
        result = ETrue;
        }    
        
    implArray.ResetAndDestroy();
    implArray.Close();
    
    return result;
    }

// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::CUniEditorConvertVideoOperation.
// ---------------------------------------------------------
//
CUniEditorConvertVideoOperation::CUniEditorConvertVideoOperation(
        MUniEditorOperationObserver& aObserver,
        CUniEditorDocument& aDocument,
        RFs& aFs ) :
    CUniEditorOperation( aObserver, aDocument, aFs, EUniEditorOperationConvertVideo ),
    iTargetSize( 0 ),
    iNewAttaId( KMsvNullIndexEntryId )
    {
    }

// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniEditorConvertVideoOperation::ConstructL()
    {
    BaseConstructL();
    
    RImplInfoPtrArray implArray;
    CVideoConversionPlugin::ListImplementationsL( implArray );
    
    if ( implArray.Count() > 0)
        {
        iConverter = CVideoConversionPlugin::NewL( implArray[0]->ImplementationUid() );    
        }
 
    implArray.ResetAndDestroy();
    implArray.Close();
    }

// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::~CUniEditorConvertVideoOperation
// ---------------------------------------------------------
//
CUniEditorConvertVideoOperation::~CUniEditorConvertVideoOperation()
    {
    Cancel();
    
    delete iNewVideoInfo;
    iNewVideoFile.Close();
    
    delete iEditStore;
    delete iConverter;
    }

// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::Process
// ---------------------------------------------------------
//
void CUniEditorConvertVideoOperation::Process(
        CMsgVideoInfo* aVideoInfo,
        TMsvAttachmentId aAttachmentId,
        TInt aMessageSize )
    {
    ResetErrors();
    
    iVideoInfo = aVideoInfo;
    iAttachmentId = aAttachmentId;
    iTargetSize = aMessageSize;
    iOperationState = EUniConvertVidProcess;
    
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::DetachAttachmentId
// ---------------------------------------------------------
//
void CUniEditorConvertVideoOperation::DetachAttachmentId( TMsvAttachmentId& aAttachmentId )
    {
    // iNewVideoInfo may be already detached in DetachVideoInfo
    aAttachmentId = iNewAttaId;
    iNewAttaId = KMsvNullIndexEntryId;
    }

// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::RunL
// ---------------------------------------------------------
//
void CUniEditorConvertVideoOperation::RunL()
    {
    PrintOperationAndState();
    if ( iStatus.Int() != KErrNone )
        {
        SetError( iStatus.Int() );
        iOperationState = EUniConvertVidError;
        }
    
    switch ( iOperationState )
        {
        case EUniConvertVidProcess:
            {
            DoStartProcessL();
            break;
            }
        case EUniConvertVidResolve:
            {
            DoStartResolveL();
            break;
            }
        case EUniConvertVidReady:
            {
            iObserver.EditorOperationEvent( EUniEditorOperationConvertVideo,
                                            EUniEditorOperationComplete );
            break;
            }
        case EUniConvertVidError:
            {
            DoErrorWithoutStateChange();
            iObserver.EditorOperationEvent( EUniEditorOperationConvertVideo,
                                            EUniEditorOperationError );
            break;
            }
        default:
            {
            iObserver.EditorOperationEvent( EUniEditorOperationConvertVideo,
                                            EUniEditorOperationError );
            break;
            }
        }    
    }

// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::DoCancelCleanup
// ---------------------------------------------------------
//
void CUniEditorConvertVideoOperation::DoCancelCleanup()
    {
    if ( iConverter )
        {
        iConverter->CancelConvert();
        }
        
    DoErrorWithoutStateChange();
    }

  
// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::DoStartProcessL
// ---------------------------------------------------------
//
void CUniEditorConvertVideoOperation::DoStartProcessL()
    {
    //Create new attachment with .3rp extension
    CreateEmptyAttachmentL();
    
    RFile sourceFile = OpenFileForReadingL();
    
    iConverter->ConvertL ( sourceFile, iNewVideoFile, iTargetSize, *this );  
    SetPending();
    
    //We are responsble for closeing file handles
    sourceFile.Close();
    }
  
// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::DoStartResolveL
// ---------------------------------------------------------
//
void CUniEditorConvertVideoOperation::DoStartResolveL()
    {
    iNewVideoInfo = static_cast<CMsgVideoInfo*>(iDocument.DataModel()->MediaResolver().CreateMediaInfoL( iNewVideoFile ) );
    iDocument.DataModel()->MediaResolver().ParseInfoDetailsL( iNewVideoInfo, iNewVideoFile );

    iOperationState = EUniConvertVidReady;

    iNewVideoFile.Close();
    
    __ASSERT_DEBUG( iEditStore, Panic( EUniNullPointer ) );
    
    iEditStore->CommitL();
    delete iEditStore;
    iEditStore = NULL;

    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::DoErrorWithoutStateChange
// ---------------------------------------------------------
//
void CUniEditorConvertVideoOperation::DoErrorWithoutStateChange()
    {
    iNewVideoFile.Close();
    
    delete iNewVideoInfo;
    iNewVideoInfo = NULL;
    
    delete iEditStore;
    iEditStore = NULL;
    }

// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::VideoConversionDone
//
// Video conversion callback implementation.
// ---------------------------------------------------------
//
void CUniEditorConvertVideoOperation::VideoConversionDone( TInt aStatus )
    {
    switch ( aStatus )
        {
        case KErrNone:
            {
            iOperationState = EUniConvertVidResolve;
            SetError( EUniConvertVidSuccessful );
            break;
            }
        case KErrNoMemory:
            {
            iOperationState = EUniConvertVidError;
            SetError( EUniConvertVidOutOfMemory );
            break;
            }
        case KErrDiskFull:
            {
            iOperationState = EUniConvertVidError;
            SetError( EUniConvertVidOutOfDisk );
            break;
            }
        case KErrNotFound:
            {
            iOperationState = EUniConvertVidError;
            SetError( EUniConvertVidNotFound );
            break;
            }
        default:
            {
            iOperationState = EUniConvertVidError;
            SetError( EUniConvertVidFailed );
            break;
            }
        }
    
    if ( aStatus == KErrCancel )
        {
        CompleteOperation( KErrCancel );
        }
    else
        {
        CompleteOperation( KErrNone );
        }
    }

// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::CreateEmptyAttachmentL
// ---------------------------------------------------------
//
void CUniEditorConvertVideoOperation::CreateEmptyAttachmentL()
    {
    iNewAttaId = KMsvNullIndexEntryId;

    // Get the file name from original full path name.
    TParsePtrC parser( iVideoInfo->FullFilePath() );

    TFileName ext( parser.Ext() );
    ext.Zero();
    ext.Copy( KUniExtVideo3gp_8 );

    TFileName newFileName( parser.Name() );
    newFileName.Append( ext );

    iEditStore = iDocument.Mtm().Entry().EditStoreL();
    MMsvAttachmentManagerSync& managerSync = iEditStore->AttachmentManagerExtensionsL();
    CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( attachment );

    attachment->SetMimeTypeL( KMsgMimeVideo3gpp );

    managerSync.CreateAttachmentL( newFileName, iNewVideoFile, attachment );
    
    CleanupStack::Pop( attachment ); // ownership transferred
    
    iNewAttaId = attachment->Id();
    }

// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::OpenFileForReadingL
// ---------------------------------------------------------
//
RFile CUniEditorConvertVideoOperation::OpenFileForReadingL()
    {
    RFile sourceFile;
    if ( iAttachmentId != KMsvNullIndexEntryId )
        {
        __ASSERT_DEBUG( iEditStore, Panic( EUniNullPointer ) );
        sourceFile = iEditStore->AttachmentManagerL().GetAttachmentFileL( iAttachmentId );
        }
    else
        {
        TInt err = sourceFile.Open( iFs, 
                                    iVideoInfo->FullFilePath(), 
                                    EFileRead | EFileShareAny );
        if ( err )
            {
            err = sourceFile.Open( iFs, 
                                   iVideoInfo->FullFilePath(), 
                                   EFileRead | EFileShareReadersOnly );
            User::LeaveIfError( err );
            }
        }
    return sourceFile;
    }
       

// ---------------------------------------------------------
// CUniEditorConvertVideoOperation::DetachVideoInfo
// ---------------------------------------------------------
//
CMsgVideoInfo* CUniEditorConvertVideoOperation::DetachVideoInfo()
    {
    // ownership transferred
    CMsgVideoInfo* tempInfo = iNewVideoInfo;
    iNewVideoInfo = NULL;
    return tempInfo;
    }

// End of file
