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
* Description:   CUniEditorSendUiOperation, operation for opening messages created by SendUI
*
*/



// ========== INCLUDE FILES ================================

#include <mmsconst.h>

#include <unidatautils.h>
#include <MsgMimeTypes.h>
#include <unismilmodel.h>
#include <MsgMediaInfo.h>
#include <MsgTextInfo.h>
#include <MsgImageInfo.h>
#include <MsgVideoInfo.h>

#include <MmsConformance.h>

#include "UniEditorSendUiOperation.h"
#include "UniEditorProcessTextOperation.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

const TUint KSendAsDeleteAllPercentage      = 80;   

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniEditorSendUiOperation::NewL
//
// Factory method.
// ---------------------------------------------------------
//
CUniEditorSendUiOperation* CUniEditorSendUiOperation::NewL(
        MUniEditorOperationObserver& aObserver,
        CUniEditorDocument& aDocument,
        RFs& aFs )
    {
    CUniEditorSendUiOperation* self = 
        new ( ELeave ) CUniEditorSendUiOperation( aObserver, aDocument, aFs );
        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CUniEditorSendUiOperation::CUniEditorSendUiOperation
//
// Constructor.
// ---------------------------------------------------------
//
CUniEditorSendUiOperation::CUniEditorSendUiOperation(
        MUniEditorOperationObserver& aObserver,
        CUniEditorDocument& aDocument,
        RFs& aFs ) :
    CUniEditorOperation( aObserver, aDocument, aFs, EUniEditorOperationSendUi )
    {
    }


// ---------------------------------------------------------
// CUniEditorSendUiOperation::CUniEditorSendUiOperation
//
// Destructor.
// ---------------------------------------------------------
//
CUniEditorSendUiOperation::~CUniEditorSendUiOperation()
    {
    Cancel();
    delete iImageOperation;
    delete iVideoOperation;
#ifdef RD_MSG_XHTML_SUPPORT 
    delete iTextOperation;
#endif        
    }

// ---------------------------------------------------------
// CUniEditorSendUiOperation::Start
// ---------------------------------------------------------
//
void CUniEditorSendUiOperation::Start()
    {
    ResetErrors();
    
    iOperationState = EUniEditorSendUiCheck;
    iObjectsSize = 0;
    iObjectNum = 0;
    iSlideNum = 0;
    iDeleteAll = EFalse;
    
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorSendUiOperation::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniEditorSendUiOperation::ConstructL()
    {
    BaseConstructL();
    
    iDeleteAllLimit = ( iDocument.MaxMessageSize() * KSendAsDeleteAllPercentage ) / 100;
    iOptimizedFlow = EFalse;
    }

// ---------------------------------------------------------
// CUniEditorSendUiOperation::DoCancelCleanup
//
// ---------------------------------------------------------
//
void CUniEditorSendUiOperation::DoCancelCleanup()
    {
    if ( iImageOperation )
        {
        iImageOperation->Cancel();
        }
        
    if ( iVideoOperation )
        {
        iVideoOperation->Cancel();
        }  
#ifdef RD_MSG_XHTML_SUPPORT 
    if ( iTextOperation )
        {
        iTextOperation->Cancel();
        }
#endif      
    }

// ---------------------------------------------------------
// CUniEditorSendUiOperation::RunL
//
// ---------------------------------------------------------
//
void CUniEditorSendUiOperation::RunL()
    { 
    PrintOperationAndState();
    
    if ( !SetErrorAndReport( iStatus.Int() ) )
        {
        DoSendUiStepL();
        }
    }

// ---------------------------------------------------------
// CUniEditorSendUiOperation::DoSendUiStepL
//
// ---------------------------------------------------------
//
void CUniEditorSendUiOperation::DoSendUiStepL()
    {
    switch ( iOperationState )
        {
        case EUniEditorSendUiCheck:
            {
            DoSendUiCheckL();
            break;
            }
        case EUniEditorSendUiPrepareObject:
            {
            DoSendUiPrepareObjectL();
            break;
            }
        case EUniEditorSendUiPrepareAttachments:
            {
            DoSendUiPrepareAttachmentsL();
            break;
            }
        case EUniEditorSendUiEnd:
            {
            iObserver.EditorOperationEvent( EUniEditorOperationSendUi,
                                            EUniEditorOperationComplete );
            break;
            }
        default:
            {
            iObserver.EditorOperationEvent( EUniEditorOperationSendUi,
                                            EUniEditorOperationError );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorSendUiOperation::DoSendUiCheckL
// 
// Check message content when message created by SendAs
// ---------------------------------------------------------
//
void CUniEditorSendUiOperation::DoSendUiCheckL()
    {
    // check is it, one embedded object with jpeg image
    // captured from camera\ seleted from photo\ filemanager
    TInt objectCount =iDocument.DataModel()->ObjectList().Count() ;
    TInt slideCount =iDocument.DataModel()->SmilModel().SlideCount() ;
    if ( (objectCount == 1)&&(slideCount == 1)&&
         (iDocument.DataModel()->AttachmentList().Count()== 0))
        {
        CUniObject* obj = 
        iDocument.DataModel()->SmilModel().GetObjectByIndex( slideCount-1, objectCount-1 );
        if(obj->MediaInfo()->MimeType().CompareF( KMsgMimeImageJpeg )== 0)
            {
            iOptimizedFlow = ETrue;
            iObserver.EditorOperationEvent( EUniEditorOperationSendUi,
                                             EUniEditorOperationPartialComplete );   
            iOptimizedFlow = EFalse;
            }
        }
        
    if ( iDocument.DataModel()->ObjectList().Count() ||
         iDocument.DataModel()->AttachmentList().Count() )
        {
        DoCheckFilesL();
        }
        
    if ( iDocument.DataModel()->ObjectList().Count() )
        {
        iDocument.DataModel()->SmilModel().SortSlidesL();
        }
        
    iOperationState = EUniEditorSendUiPrepareObject;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorSendUiOperation::DoCheckFilesL
// ---------------------------------------------------------
//
void CUniEditorSendUiOperation::DoCheckFilesL()
    {
    TInt slideCount = iDocument.DataModel()->SmilModel().SlideCount();

    //It's important to go from end to beginning
    //because slides may be removed
    for ( TInt i = slideCount; --i >= 0 ; )
        {
        //It's important to go from end to beginning
        //because objects may be removed
        for ( TInt ii = iDocument.DataModel()->SmilModel().SlideObjectCount(i); --ii >= 0 ;)
            {
            CUniObject* obj = iDocument.DataModel()->SmilModel().GetObjectByIndex( i, ii );

            TMmsConformance conformance = 
                iDocument.DataModel()->MmsConformance().MediaConformance( *obj->MediaInfo() );

            TUint32 confStatus = conformance.iConfStatus;

            TBool remove = EFalse;
            if ( confStatus & EMmsConfNokDRM )
                {
                remove = ETrue;
                SetError( EUniSendUiForbidden );
                }
            else if ( confStatus & EMmsConfNokCorrupt )
                {
                SetError( EUniSendUiCorrupted );
                }
            else if ( confStatus & ( EMmsConfNokNotEnoughInfo | EMmsConfNokNotSupported ) ||
                      ( iDocument.CreationMode() == EMmsCreationModeRestricted &&
                        confStatus & EMmsConfNokFreeModeOnly ) )
                {
                remove = ETrue;
                SetError( EUniSendUiUnsupported );
                }
            else if ( !conformance.iCanAdapt && 
                      confStatus & EMmsConfNokTooBig ) 
                {
                remove = ETrue;
                SetError( EUniSendUiTooBig );
                }
                
            if ( remove )
                {
                iDocument.DataModel()->SmilModel().RemoveObjectByIndexL( i, ii );
                remove = EFalse;
                }
            }
              
        if ( !iDocument.DataModel()->SmilModel().SlideObjectCount( i ) )
            {
            iDocument.DataModel()->SmilModel().RemoveSlide( i );
            }
        }  

    CUniObjectList& attachments = iDocument.DataModel()->AttachmentList();
    //It's important to go from end to beginning
    //because attachments may be removed
    for ( TInt j = attachments.Count(); --j >= 0 ; )
        {
        CUniObject* obj = attachments.GetByIndex( j );
        
        TMmsConformance conformance = 
            iDocument.DataModel()->MmsConformance().MediaConformance( *obj->MediaInfo() );

        TUint32 confStatus = conformance.iConfStatus;
        
        if ( iDocument.CreationMode() != EMmsCreationModeRestricted )
            {
            // Mask "FreeModeOnly" away in free mode
            confStatus &= ~EMmsConfNokFreeModeOnly;
            // Mask "Scaling needed" away in free mode
            confStatus &= ~EMmsConfNokScalingNeeded;
            }
        
        if ( confStatus & EMmsConfNokDRM )
            {
            attachments.RemoveObjectL( obj );
            SetError( EUniSendUiForbidden );
            delete obj;
            }
        else if ( confStatus & EMmsConfNokCorrupt )
            {
            SetError( EUniSendUiCorrupted );
            }
        else if ( confStatus != EMmsConfOk )
            {
            attachments.RemoveObjectL( obj );
            SetError( EUniSendUiUnsupported );
            delete obj;
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorSendUiOperation::DoSendUiPrepareObjectL
// 
// Processes images and checks whether message size is below
// max size. If not removes as many objects as needed.
// ---------------------------------------------------------
//
void CUniEditorSendUiOperation::DoSendUiPrepareObjectL()
    {
    VerifyPreparedObjectL();
        
    CUniObject* obj = iDocument.DataModel()->SmilModel().GetObjectByIndex( iSlideNum, iObjectNum );
    
    if ( obj )
        {
        DoPrepareObjectL( obj );
        }
    else
        {
        iOperationState = EUniEditorSendUiPrepareAttachments;
        CompleteSelf( KErrNone );
        }
    }

// ---------------------------------------------------------
// CUniEditorSendUiOperation::VerifyPreparedObjectL
// ---------------------------------------------------------
//
void CUniEditorSendUiOperation::VerifyPreparedObjectL()
    {
    if ( iPreparedObject )
        {
        TBool remove = EFalse;
        
        // Conformance status check!
        TMmsConformance conformance = 
            iDocument.DataModel()->MmsConformance().MediaConformance( *iPreparedObject->MediaInfo() );

        TUint32 confStatus = conformance.iConfStatus;
        if ( iPreparedObject->MediaType() == EMsgMediaText )
            {
            // Mask "ConversionNeeded" away for text objects
            // -> Will be always converted to UTF-8 anyway
            confStatus &= ~EMmsConfNokConversionNeeded;
            }
            
        if ( iDocument.CreationMode() != EMmsCreationModeRestricted )
            {
            // Mask "FreeModeOnly" away in free mode
            confStatus &= ~EMmsConfNokFreeModeOnly;
            // Mask "Scaling needed" away in free mode
            confStatus &= ~EMmsConfNokScalingNeeded;
            //Mask "Corrupted" away in free mode
            confStatus &= ~EMmsConfNokCorrupt;
            }
            
        // Mask "Too big" away as there is a separate check for that
        confStatus &= ~EMmsConfNokTooBig;
        
        if ( confStatus != EMmsConfOk )
            {
            SetError( EUniSendUiUnsupported );
            remove = ETrue;
            }

        TInt currentSize = iObjectsSize + 
                           iPreparedObject->Size() + 
                           iDocument.DataModel()->SmilModel().SmilComposeSize( iSlideNum + 1, iObjectNum + 1 );

        if ( !remove && 
             TUint( currentSize ) > iDocument.MaxMessageSize() )
            {
            // Remove if above max size
            if ( TUint( iPreviousSize ) > iDeleteAllLimit )
                {
                //Once above "delete all limit" delete all the rest.
                iDeleteAll = ETrue;
                }
            remove = ETrue;
            SetError( EUniSendUiTooBig );
            }
        else
            {
            // Don't remove. Update previous size.
            iPreviousSize = currentSize;
            }

        if ( remove )
            {
            iDocument.DataModel()->SmilModel().RemoveObjectByIndexL( iSlideNum, iObjectNum );
            
            if ( !iDocument.DataModel()->SmilModel().SlideObjectCount( iSlideNum ) )
                {
                iDocument.DataModel()->SmilModel().RemoveSlide( iSlideNum );
                }
            }
        else
            {
            iObjectsSize += iPreparedObject->Size();
            iObjectNum++;
            
            if ( iObjectNum >= iDocument.DataModel()->SmilModel().SlideObjectCount( iSlideNum ) )
                {
                iSlideNum++;
                iObjectNum = 0;
                }
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorSendUiOperation::DoPrepareObjectL
// ---------------------------------------------------------
//
void CUniEditorSendUiOperation::DoPrepareObjectL( CUniObject* aObject )
    {
    switch ( aObject->MediaType() )
        {
        case EMsgMediaImage:
            {
            if ( !iImageOperation )
                {
                iImageOperation = CUniEditorProcessImageOperation::NewL( *this, iDocument, iFs );
                }
                
            // Processes if needed:
            iImageOperation->Process( static_cast<CMsgImageInfo*>( aObject->MediaInfo() ),
                                      aObject->AttachmentId(),
                                      iPreviousSize );
            iPreparedObject = aObject;
            
            SetPending();
            return;
            }
        case EMsgMediaVideo:
            {
            //We need video conversion only for mp4 videos
            if ( aObject->MimeType().Compare( KMsgMimeVideoMp4 ) == 0   )
                {
                if ( CUniEditorConvertVideoOperation::IsImplementedL() )
                    {
                    if ( !iVideoOperation )
                        { 
                        iVideoOperation = CUniEditorConvertVideoOperation::NewL( *this, iDocument, iFs );
                        }
                    
                    // Processes if needed:
                    iVideoOperation->Process( static_cast<CMsgVideoInfo*>( aObject->MediaInfo() ),
                                              aObject->AttachmentId(),
                                              iDocument.MaxMessageSize() );
                    iPreparedObject = aObject;
                    
                    SetPending();
                    return;
                    }
                }
            break;
            }
        case EMsgMediaXhtml:
            {
#ifdef RD_MSG_XHTML_SUPPORT 
            // Processes all XHTML objects and converts them into plain text.
            iTextOperation = CUniEditorProcessTextOperation::NewL( *this, iDocument, iFs );
            iTextOperation->Start();
            
            SetPending();
            return;
#else
            break;
#endif
            }
        case EMsgMediaText:
        case EMsgMediaAudio:
        default:
            {
            // nothing
            break;
            }
        }
        
    iPreparedObject = aObject;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorSendUiOperation::DoSendUiPrepareAttachmentsL
// ---------------------------------------------------------
//
void CUniEditorSendUiOperation::DoSendUiPrepareAttachmentsL()
    {
    TInt smilSize = iObjectsSize + iDocument.DataModel()->SmilModel().SmilComposeSize();
    
    TInt attaSize = 0;
    TInt j = 0;
    
    CUniObjectList& attachments = iDocument.DataModel()->AttachmentList();
    
    if ( attachments.Count() )
        {
        while ( TUint( smilSize + attaSize ) < iDocument.MaxMessageSize() )
            {
            attaSize += attachments.GetByIndex( j )->Size();
            j++;
            if ( j >= attachments.Count()  && 
                 TUint( smilSize + attaSize ) < iDocument.MaxMessageSize() )
                {
                j++;
                break;
                }
            }
        j--;
        while ( j < attachments.Count() )
            {
            CUniObject* obj = attachments.GetByIndex( j );
            attachments.RemoveObjectL( obj );
            SetError( EUniSendUiTooBig );
            delete obj;
            }
        }
    iOperationState = EUniEditorSendUiEnd;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorSendUiOperation::HandleOperationEvent
// ---------------------------------------------------------
//
void CUniEditorSendUiOperation::HandleOperationEvent( TUniEditorOperationType aOperation,
                                                      TUniEditorOperationEvent /*aEvent*/ )
    {
    TBool remove( EFalse );
    
    TMsvAttachmentId attaId( KMsvNullIndexEntryId );
    CMsgMediaInfo* info = NULL;
    
    if ( aOperation == EUniEditorOperationProcessImage )
        {
        // Process image error handling
        CArrayFixFlat<TInt>* errors = iImageOperation->GetErrors();
        for ( TInt i = 0; i < errors->Count(); i++ )
            {
            if ( errors->At( i ) == EUniProcessImgUserAbort )
                {
                iOperationState = EUniEditorSendUiEnd;
                }
            else if ( ( errors->At( i ) == EUniProcessImgCouldNotScale &&
                        iDocument.CreationMode() == EMmsCreationModeRestricted ) ||
                      errors->At( i ) == EUniProcessImgNotFound )
                {
                remove = ETrue;
                }
            
            SetError( errors->At( i ) );
            }
        
        info = iImageOperation->DetachImageInfo();
        iImageOperation->DetachAttachmentId( attaId );
        }
    else if ( aOperation == EUniEditorOperationConvertVideo )
        {
        // Convert video error handling
        CArrayFixFlat<TInt>* errors = iVideoOperation->GetErrors();
        for ( TInt i = 0; i < errors->Count(); i++ )
            {
            if ( errors->At( i ) == EUniConvertVidUserAbort )
                {
                iOperationState = EUniEditorSendUiEnd;
                }
            else if ( ( errors->At( i ) == EUniConvertVidFailed &&
                        iDocument.CreationMode() == EMmsCreationModeRestricted ) ||
                      errors->At( i ) == EUniConvertVidNotFound )
                {
                remove = ETrue;
                }
            
            SetError( errors->At( i ) );
            }
        
        info = iVideoOperation->DetachVideoInfo();
        iVideoOperation->DetachAttachmentId( attaId );
        }
#ifdef RD_MSG_XHTML_SUPPORT 
    else if ( aOperation == EUniEditorOperationProcessText )
        {
        CArrayFixFlat<TInt>* errors = iTextOperation->GetErrors();
        for ( TInt i = 0; i < errors->Count(); i++ )
            {
            SetError( errors->At( i ) );
            }
        }
#endif
    
    if ( info )
        {
        TRAPD( error,
               iDocument.DataModel()->SmilModel().RemoveObjectL( iSlideNum, iPreparedObject ) );
               
        if ( !error )
            {
            iPreparedObject = NULL;
            TRAP_IGNORE( iPreparedObject = iDocument.DataModel()->SmilModel().AddStoredObjectL( iSlideNum, attaId, info ) );
            }
        else // --> original object remains...
            {
            delete info;
            }
        }

    if ( remove && iPreparedObject )
        {
        TRAPD( err, iDocument.DataModel()->SmilModel().RemoveObjectByIndexL( iSlideNum, iObjectNum ) );
        
        if ( !err && 
             !iDocument.DataModel()->SmilModel().SlideObjectCount( iSlideNum ) )
            {
            iDocument.DataModel()->SmilModel().RemoveSlide( iSlideNum );
            }
            
        iPreparedObject = NULL;
        }
        
    //else --> original object remains...
    CompleteOperation( KErrNone );
    }
// ---------------------------------------------------------
// CUniEditorSendUiOperation::IsOptimizedFlagSet
// ---------------------------------------------------------
//
TBool CUniEditorSendUiOperation::IsOptimizedFlagSet()
    {
    return iOptimizedFlow; 
    }
// EOF
