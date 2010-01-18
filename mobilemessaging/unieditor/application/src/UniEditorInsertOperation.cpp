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
* Description:   CUniEditorInsertOperation.
*
*/



// ========== INCLUDE FILES ================================

#include <apmstd.h>
#include <mmsconst.h>

#include <MmsConformance.h>

#include <MsgMediaInfo.h>
#include <MsgImageInfo.h>
#include <MsgVideoInfo.h>
#include <MsgMimeTypes.h>
#include <msgimagecontrol.h>
#include <msgvideocontrol.h>

// DRM
#include <fileprotectionresolver.h>

#include "unislideloader.h"
#include "UniClientMtm.h"
#include "UniEditorEnum.h"
#include "UniEditorDocument.h"
#include "UniEditorProcessImageOperation.h"
#include "UniEditorChangeSlideOperation.h"
#include "UniEditorConvertVideoOperation.h"
#include "UniEditorInsertOperation.h"

// ========== CONSTANTS ====================================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniEditorInsertOperation::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CUniEditorInsertOperation* CUniEditorInsertOperation::NewL( 
        MUniEditorOperationObserver& aOperationObserver,
        CUniEditorDocument& aDocument,
        CUniEditorHeader& aHeader,
        CUniSlideLoader& aSlideLoader,
        CMsgEditorView& aView,
        RFs& aFs )
    {
    CUniEditorInsertOperation* self = new ( ELeave ) CUniEditorInsertOperation(
            aOperationObserver,
            aDocument,
            aHeader,
            aSlideLoader,
            aView,
            aFs );
            
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::CUniEditorInsertOperation
// ---------------------------------------------------------
//
CUniEditorInsertOperation::CUniEditorInsertOperation(
        MUniEditorOperationObserver& aOperationObserver,
        CUniEditorDocument& aDocument,
        CUniEditorHeader& aHeader,
        CUniSlideLoader& aSlideLoader,
        CMsgEditorView& aView,
        RFs& aFs ) :
    CUniEditorOperation( aOperationObserver, aDocument, aFs, EUniEditorOperationInsert ),
    iHeader( aHeader ),
    iSlideLoader( aSlideLoader ),
    iView( aView ),
    iInsertedSlide( -1 ),
    iPreviousSlide( -1 )
    {
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::ConstructL
// 2nd phase constructor
// ---------------------------------------------------------
//
void CUniEditorInsertOperation::ConstructL()
    {
    BaseConstructL();
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::~CUniEditorInsertOperation
// ---------------------------------------------------------
//
CUniEditorInsertOperation::~CUniEditorInsertOperation()
    {
    Cancel();
    delete iOldInfo;
    delete iNewInfo;
    
    delete iImageOperation;
    delete iVideoOperation;
    delete iChangeSlideOperation;
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::Insert
// ---------------------------------------------------------
//
void CUniEditorInsertOperation::Insert( CMsgMediaInfo* aInfo, 
                                        TBool aAddSlide, 
                                        TBool aAddAsAttachment )
    {
    ResetErrors();
    
    iInsertedSlide = -1;
    iInsertedObject = NULL;
    iOldInfo = aInfo;
    iAddSlide = aAddSlide;
    
    if ( aInfo &&
         aInfo->Protection() & EFileProtDrm2 )
        {
        // DRM 2 content is always added as an attachment.
        iAddAsAttachment = ETrue;
        }
    else
        {
        iAddAsAttachment = aAddAsAttachment;
        }   
      
    //Coverty fix : Forward Null http://ousrv057/cov.cgi?cid=35696
    if ( aInfo && aInfo->MediaType() == EMsgMediaImage )
        {
        iOperationState = EUniInsertCheckMode;
        }
    else if ( aInfo && aInfo->MimeType() == KMsgMimeVideoMp4 )
        {
        iOperationState = EUniInsertConvertVideo;
        }
    else    
        {
        iOperationState = EUniInsertCopyAttachment;
        }
        
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::RunL
// ---------------------------------------------------------
//
void CUniEditorInsertOperation::RunL()
    {
    PrintOperationAndState();
    
    if ( iStatus.Int() != KErrNone )
        {
        SetError( iStatus.Int() );
        iOperationState = EUniInsertError;
        }
        
    switch ( iOperationState )
        {
        case EUniInsertCheckMode:
            {
            DoCheckModeL();
            break;
            }
        case EUniInsertProcessImage:
            {
            DoStartProcessImageL();
            break;
            }
       case EUniInsertConvertVideo:
            {
            DoStartConvertVideoL();
            break;    
            }
        case EUniInsertCheckSize:
            {
            DoCheckSizeL();
            break;
            }
        case EUniInsertCopyAttachment:
            {
            iObserver.EditorOperationEvent( EUniEditorOperationInsert,
                                            EUniEditorOperationProcessing );
            DoStartCopyAttachmentL();
            break;
            }
        case EUniInsertLoad:
            {
            DoStartLoadL();
            break;
            }
        case EUniInsertReady:
            {
            iObserver.EditorOperationEvent( EUniEditorOperationInsert,
                                            EUniEditorOperationComplete );
            break;
            }
        case EUniInsertError:
            {
            DoErrorWithoutStateChangeL();
            break;
            }
        case EUniInsertErrorReload:
            {
            DoStartReloadL();
            break;
            }
        case EUniInsertErrorReport:
            {
            iObserver.EditorOperationEvent( EUniEditorOperationInsert,
                                            EUniEditorOperationError );
            break;
            }
        default:
            {
            SetError( KErrUnknown );
            iObserver.EditorOperationEvent( EUniEditorOperationInsert,
                                            EUniEditorOperationError );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::DoCancelCleanup
// ---------------------------------------------------------
//
void CUniEditorInsertOperation::DoCancelCleanup()
    {
    if ( iImageOperation )
        {
        iImageOperation->Cancel();
        }
        
    if ( iVideoOperation )
        {
        iVideoOperation->Cancel();
        }    
    
    if ( iInsertedObject )
        {
        iInsertedObject->Cancel();
        }
    
    if ( iChangeSlideOperation )
        {
        iChangeSlideOperation->Cancel();
        }
    
    iSlideLoader.Cancel();
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::DoStartProcessImageL
// ---------------------------------------------------------
//
void CUniEditorInsertOperation::DoStartProcessImageL()
    {
    if ( iAddSlide )
        {
        // If there is an image with same name added on the current slide, it must be closed that
        // inserting would succeed. Insert attachment fails in Symbian messaging otherwise.
        // If error occurs after this, current slide must be realoded again
        CMsgImageControl* imageControl= static_cast<CMsgImageControl*> ( iView.ControlById( EMsgComponentIdImage ) );
        if ( imageControl )
            {
            imageControl->SetImageFileClosed();
            }
        }

    if ( !iImageOperation )
        {
        iImageOperation = CUniEditorProcessImageOperation::NewL( *this, iDocument, iFs );
        }
        
    iImageOperation->Process( static_cast<CMsgImageInfo*>( iOldInfo ), KMsvNullIndexEntryId, iDocument.MessageSize( ETrue ) );
    SetPending();
    }


// ---------------------------------------------------------
// CUniEditorInsertOperation::DoStartConvertVideoL
// ---------------------------------------------------------
//
void CUniEditorInsertOperation::DoStartConvertVideoL()
    {
    if ( CUniEditorConvertVideoOperation::IsImplementedL() )
        {
        if ( !iVideoOperation )
            {
            iVideoOperation = CUniEditorConvertVideoOperation::NewL( *this, iDocument, iFs );
            }
                        
        iVideoOperation->Process( static_cast<CMsgVideoInfo*>( iOldInfo ), KMsvNullIndexEntryId, iDocument.MaxMessageSize() );   
        SetPending();
        }
    else
        {
        iOperationState = EUniInsertCopyAttachment;
        CompleteSelf( KErrNone );
        }
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::DoStartCopyAttachmentL
// ---------------------------------------------------------
//
void CUniEditorInsertOperation::DoStartCopyAttachmentL()
    {
    TInt slide = iDocument.CurrentSlide();
    
    if ( iAddSlide )
        {
        iPreviousSlide = slide;
        
        slide++;
        iDocument.DataModel()->SmilModel().AddSlideL( slide );
        
        iInsertedSlide = slide;
        }   
        
    if ( iNewInfo )
        {
        TMsvAttachmentId attaId;
        
        if ( iImageOperation )
            {
            iImageOperation->DetachAttachmentId( attaId );
            }
        else
            {
            iVideoOperation->DetachAttachmentId( attaId );
            }
            
        if ( iAddAsAttachment )
            {
            iInsertedObject = iDocument.DataModel()->AddStoredAttachmentL( attaId, iNewInfo );
            }
        else
            {
            iInsertedObject = iDocument.DataModel()->SmilModel().AddStoredObjectL( slide, attaId, iNewInfo );
            }
            
        iNewInfo = NULL;
        CompleteSelf( KErrNone );        
        }
    else
        {
        if ( iAddAsAttachment )
            {
            iInsertedObject = iDocument.DataModel()->AddAttachmentL( *this, iOldInfo );            
            }
        else
            {
            iInsertedObject = iDocument.DataModel()->SmilModel().AddObjectL( *this, slide, iOldInfo );
            }
        
        iOldInfo = NULL; // Ownership transferred
        SetPending();
        }
    
    if ( iAddAsAttachment )
        {
        iOperationState = EUniInsertReady;
        }
    else
        {
        iDocument.SetBodyModified( ETrue );
        iOperationState = EUniInsertLoad;
        }
    }
    
// ---------------------------------------------------------
// CUniEditorInsertOperation::DoStartLoadL
// ---------------------------------------------------------
//
void CUniEditorInsertOperation::DoStartLoadL()
    {
    if ( iAddSlide )
        {
        if ( !iChangeSlideOperation )
            {
            iChangeSlideOperation = CUniEditorChangeSlideOperation::NewL( *this,
                                                                          iDocument,
                                                                          iHeader,
                                                                          iSlideLoader,
                                                                          iView,
                                                                          iFs );            
            }
            
        iChangeSlideOperation->ChangeSlide( iInsertedSlide );
        }
    else
        {        
        iSlideLoader.LoadObject( *this, iInsertedObject );
        }
    
    SetPending();
    iOperationState = EUniInsertReady;
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::DoStartReloadL
// ---------------------------------------------------------
//
void CUniEditorInsertOperation::DoStartReloadL()
    {
    TInt CurrSlide = iDocument.CurrentSlide();
    TInt slide = (iPreviousSlide!= -1)?iPreviousSlide:CurrSlide;
    if ( iPreviousSlide != ( -1 ) || CurrSlide == 0 )
        {
        if ( !iChangeSlideOperation )
            {
            iChangeSlideOperation = CUniEditorChangeSlideOperation::NewL( *this,
                                                                          iDocument,
                                                                          iHeader,
                                                                          iSlideLoader,
                                                                          iView,
                                                                          iFs );            
            }
            
        iChangeSlideOperation->ChangeSlide( slide );
        SetPending();
        }
        
    iOperationState = EUniInsertErrorReport;
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::DoCheckSizeL
// ---------------------------------------------------------
//
void CUniEditorInsertOperation::DoCheckSizeL()
    {
    if ( CheckSizeForInsertL() )
        {
        iOperationState = EUniInsertCopyAttachment;
        }
    else
        {
        iOperationState = EUniInsertError;
        }
        
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::DoCheckModeL
// ---------------------------------------------------------
//
void CUniEditorInsertOperation::DoCheckModeL()
    {
    if ( CheckModeForInsert( ETrue ) )
        {
        iOperationState = EUniInsertProcessImage;
        }
    else
        {
        iOperationState = EUniInsertError;
        }
        
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::CheckModeForInsertL
// ---------------------------------------------------------
//
TBool CUniEditorInsertOperation::CheckModeForInsert( TBool aBeforeProcessing )
    {
    CMsgMediaInfo* info;
    if ( iNewInfo )
        {
        info = iNewInfo;
        }
    else
        {
        info = iOldInfo;
        }    
        
    TMmsConformance conformance = 
        iDocument.DataModel()->MmsConformance().MediaConformance( *info );
        
    TUint32 confStatus = conformance.iConfStatus;
    
    if ( aBeforeProcessing &&  
         iDocument.CreationMode() != EMmsCreationModeRestricted )
        {
        return ETrue;
        }

    // In "free" mode user can insert images that are larger by dimensions than allowed by conformance
    if ( iDocument.CreationMode() != EMmsCreationModeRestricted )
        {
        TInt i = EMmsConfNokFreeModeOnly | EMmsConfNokScalingNeeded | EMmsConfNokTooBig;
        TInt j = ~( EMmsConfNokFreeModeOnly | EMmsConfNokScalingNeeded | EMmsConfNokTooBig );
        
        // If user answers yes to Guided mode confirmation query he/she moves to free mode
        if ( ( confStatus & i ) &&
             !( confStatus & j ) )
            {
            if ( info->Protection() & EFileProtSuperDistributable )
                {
                // SuperDistribution not checked here
                // Mask "FreeModeOnly" and "ScalingNeeded" away in free mode
                confStatus &= ~EMmsConfNokFreeModeOnly;
                confStatus &= ~EMmsConfNokScalingNeeded;
                }
            else if ( iObserver.EditorOperationQuery( EUniEditorOperationInsert, EMEOQueryGuidedObject ) )
                {
                // Guided query accepted.
                // Mask "FreeModeOnly" and "ScalingNeeded" away in free mode
                confStatus &= ~EMmsConfNokFreeModeOnly;
                confStatus &= ~EMmsConfNokScalingNeeded;
                }
            else
                {
                // Guided query not accepted. Stop insertion.
                SetError( EUniInsertUserGuidedAbort );
                return EFalse;
                }
            }
        }
    else if ( confStatus & EMmsConfNokDRM ||
              confStatus & EMmsConfNokNotEnoughInfo ||
              confStatus & EMmsConfNokNotSupported ||
              confStatus & EMmsConfNokFreeModeOnly ||
              confStatus & EMmsConfNokCorrupt )
        {
        // Sanity check
        // "Not conformant" assumed if check fails.
        SetError( EUniInsertNotSupported );
        return EFalse;
        }
    
    return ETrue;
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::CheckSizeForInsertL
// ---------------------------------------------------------
//
TBool CUniEditorInsertOperation::CheckSizeForInsertL()
    {
    CMsgMediaInfo* info;
    if ( iNewInfo )
        {
        info = iNewInfo;
        }
    else
        {
        info = iOldInfo;
        } 
        
    TMmsConformance conformance = 
        iDocument.DataModel()->MmsConformance().MediaConformance( *info );
        
    TUint32 confStatus = conformance.iConfStatus;

    if ( confStatus & EMmsConfNokTooBig ||
         TUint( iDocument.MessageSize( ETrue ) + info->FileSize() ) > iDocument.MaxMessageSize() )
        {
        SetError( EUniInsertTooBig );
        return EFalse;
        }

    return ETrue;
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::DoErrorWithoutStateChange
// ---------------------------------------------------------
//
void CUniEditorInsertOperation::DoErrorWithoutStateChangeL()
    {
    /*
    Object need to be removed before possible new slide.
    RemoveObjectL removes also attachment.

    Only possible reason for leave in RemoveObjectL is failure of attachment deletion.
    Object will be left in Objectlist and in SmilModel.
    -> Remove only from model
    */
    if ( iInsertedObject )
        {
        if ( iInsertedSlide != -1 )
            {
            iDocument.DataModel()->SmilModel().RemoveObjectL( iInsertedSlide, iInsertedObject, ETrue );
            
            iDocument.DataModel()->SmilModel().RemoveSlide( iInsertedSlide );
            iInsertedSlide = -1;
            }
        else
            {
            iDocument.DataModel()->SmilModel().RemoveObjectL( iDocument.CurrentSlide(), iInsertedObject, ETrue );
            }
        
        iInsertedObject = NULL;
        }
    else
        {
        // attachment may still be there after compress or scaling operation, if it didn't fit
        // into message
        TMsvAttachmentId attaId( KMsvNullIndexEntryId );
        if ( iNewInfo )
            {
            if ( iImageOperation )
                {
                iImageOperation->DetachAttachmentId( attaId );   
                }
            else
                {
                iVideoOperation->DetachAttachmentId( attaId );   
                }
            }
            
        if ( attaId != KMsvNullIndexEntryId )
            {
            CMsvStore* store = iDocument.Mtm().Entry().EditStoreL();
            CleanupStack::PushL( store );
            MMsvAttachmentManager& manager = store->AttachmentManagerL();
            MMsvAttachmentManagerSync& managerSync = store->AttachmentManagerExtensionsL();    
            managerSync.RemoveAttachmentL( CUniDataUtils::IndexPositionOfAttachmentL( manager, attaId ) );
            store->CommitL();
            CleanupStack::PopAndDestroy( store );
            attaId = KMsvNullIndexEntryId;
            }    
        }
    
    delete iNewInfo;
    iNewInfo = NULL;
    //If previous slide is -1 then reload first slide 
    if ( iPreviousSlide != -1 || (iDocument.CurrentSlide()) == 0)
        {
        iOperationState = EUniInsertErrorReload;
        }
    else
        {
        iOperationState = EUniInsertErrorReport;
        }
        
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::InsertedType
// ---------------------------------------------------------
//
TMsgMediaType CUniEditorInsertOperation::InsertedType()
    {
    TMsgMediaType type = EMsgMediaUnknown;
    if ( iInsertedObject )
        {
        type = iInsertedObject->MediaType();
        }
    return type;
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::InsertedObject
// ---------------------------------------------------------
//
CUniObject* CUniEditorInsertOperation::InsertedObject()
    {
    return iInsertedObject;
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::IsAddedAsAttachment
// ---------------------------------------------------------
//
TBool CUniEditorInsertOperation::IsAddedAsAttachment() const
    {
    return iAddAsAttachment;
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::ObjectSaveReady
// ---------------------------------------------------------
//
void CUniEditorInsertOperation::ObjectSaveReady( TInt aError )
    {
    CompleteOperation( aError );
    }

// ---------------------------------------------------------
// CUniEditorInsertOperation::SlideLoadReady
// ---------------------------------------------------------
//
void CUniEditorInsertOperation::SlideLoadReady( TInt aError )
    {
    CompleteOperation( aError );
    }


// ---------------------------------------------------------
// CUniEditorInsertOperation::HandleOperationEvent
// ---------------------------------------------------------
//
void CUniEditorInsertOperation::HandleOperationEvent( TUniEditorOperationType aOperation,
                                                      TUniEditorOperationEvent /*aEvent*/ )
    {
    if ( aOperation == EUniEditorOperationChangeSlide )
        {
        if ( iChangeSlideOperation->GetErrors()->Count() )
            {
            SetError( EUniInsertSlideChangeFailed );
            iOperationState = EUniInsertErrorReport;
            }
        }
    else 
        {
        TBool toErrorState = EFalse;
        
        if ( aOperation == EUniEditorOperationProcessImage )
            {
            CArrayFixFlat<TInt>* errors = iImageOperation->GetErrors();
            
            for ( TInt i = 0; i < errors->Count(); i++ )
                {
                if ( errors->At( i ) != EUniProcessImgCompressSuccessful &&
                     !( errors->At( i ) == EUniProcessImgCouldNotScale &&
                        iDocument.CreationMode() != EMmsCreationModeRestricted ) &&
                     errors->At( i ) != EUniProcessImgCorrupted )
                    {
                    toErrorState = ETrue;
                    }
                SetError( errors->At( i ) );
                }
            
            // Take ownership (even in case of possible errors)
            iNewInfo = iImageOperation->DetachImageInfo();
            }
        else  // EUniEditorOperationConvertVideo
            {
            CArrayFixFlat<TInt>* errors = iVideoOperation->GetErrors();
            
            for ( TInt i = 0; i < errors->Count(); i++ )
                {
                if ( errors->At( i ) != EUniConvertVidSuccessful )
                    {
                    toErrorState = ETrue;
                    }
                SetError( errors->At( i ) );
                }
     
            // Take ownership (even in case of possible errors)
            iNewInfo = iVideoOperation->DetachVideoInfo();
            }   
        
        if ( iNewInfo )
            {
            delete iOldInfo;
            iOldInfo = NULL;
            }
        
        if ( toErrorState )
            {
            // Process image / video error
            iOperationState = EUniInsertError;
            
            delete iOldInfo;
            iOldInfo = NULL;
            }
        else
            {
            if ( !CheckModeForInsert( EFalse ) )
                {
                iOperationState = EUniInsertError;
                
                delete iOldInfo;
                iOldInfo = NULL;
                }
            else
                {
                iOperationState = EUniInsertCheckSize;
                }
            }        
        }
            
    CompleteOperation( KErrNone );
    }


