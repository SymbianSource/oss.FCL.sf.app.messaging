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

#include <txtrich.h>

#include <centralrepository.h>

#include <messagingvariant.hrh>
#include <messaginginternalcrkeys.h> // for Central Repository keys

#include <MmsConformance.h>
#include <MsgMimeTypes.h>
#include <MsgTextInfo.h>
#include <unimsventry.h>

#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <cmsvattachment.h>

#include <msgtextutils.h>

#include <mmssettingsdefs.h>

#include "UniClientMtm.h"
#include "UniEditorEnum.h"
#include "UniEditorDocument.h"
#include "unimimeinfo.h"
#include "UniEditorProcessTextOperation.h"

// ========== CONSTANTS ====================================

_LIT( KUniExtPlainText,  ".txt" );

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::NewL
//
// Factory method.
// ---------------------------------------------------------
//
CUniEditorProcessTextOperation* CUniEditorProcessTextOperation::NewL( 
        MUniEditorOperationObserver& aObserver,
        CUniEditorDocument& aDocument,
        RFs& aFs )
    {
    CUniEditorProcessTextOperation* self = new ( ELeave ) CUniEditorProcessTextOperation(
            aObserver,
            aDocument,
            aFs );
            
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::CUniEditorProcessImageOperation.
// ---------------------------------------------------------
//
CUniEditorProcessTextOperation::CUniEditorProcessTextOperation(
        MUniEditorOperationObserver& aObserver,
        CUniEditorDocument& aDocument,
        RFs& aFs ) :
    CUniEditorOperation( aObserver, aDocument, aFs, EUniEditorOperationProcessText )
    {
    }

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniEditorProcessTextOperation::ConstructL()
    {
    BaseConstructL();
    }

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::~CUniEditorProcessTextOperation
// ---------------------------------------------------------
//
CUniEditorProcessTextOperation::~CUniEditorProcessTextOperation()
    {
    Cancel();
    
    delete iEditStore;
    delete iParser;
    }

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::Start
// ---------------------------------------------------------
//
void CUniEditorProcessTextOperation::Start()
    {
    ResetErrors();
    
    iOperationState = EUniProcessTxtCheck;
    
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::RunL
// ---------------------------------------------------------
//
void CUniEditorProcessTextOperation::RunL()
    {
    PrintOperationAndState();
    if ( iStatus.Int() != KErrNone )
        {
        SetError( iStatus.Int() );
        iOperationState = EUniProcessTxtError;
        }
    
    switch ( iOperationState )
        {
        case EUniProcessTxtCheck:
            {
            DoStartCheck();
            break;
            }
        case EUniProcessTxtProcess:
            {
            DoStartProcessL();
            break;
            }
        case EUniProcessTxtReady:
            {
            NotifyObserver( EUniEditorOperationComplete );
            break;
            }
        case EUniProcessTxtError:
            {
            NotifyObserver( EUniEditorOperationError );
            break;
            }
        default:
            {
            SetError( KErrUnknown );
            NotifyObserver( EUniEditorOperationError );
            break;
            }
        }    
    }

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::DoCancelCleanup
// ---------------------------------------------------------
//
void CUniEditorProcessTextOperation::DoCancelCleanup()
    {
    if ( iParser )
        {
        iParser->Cancel();
        }
        
    Reset();
    }

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::DoStartCheck
// ---------------------------------------------------------
//
void CUniEditorProcessTextOperation::DoStartCheck()
    {
    TInt slideCount = iDocument.DataModel()->SmilModel().SlideCount();
    
    iProcessedObject = NULL;
    
    for ( TInt currentSlide = 0; 
          currentSlide < slideCount && iProcessedObject == NULL;
          currentSlide++ )
        {
        TInt objectCount = iDocument.DataModel()->SmilModel().SlideObjectCount( currentSlide );
        for ( TInt currentObject = 0; 
              currentObject < objectCount && iProcessedObject == NULL; 
              currentObject++ )
            {
            CUniObject* object = iDocument.DataModel()->SmilModel().GetObjectByIndex( currentSlide, currentObject );
            
            if ( object &&
                 object->MediaType() == EMsgMediaXhtml )
                {
                iProcessedObject = object;
                iSlideNum = currentSlide;
                }
            }
        }  
    
    if ( iProcessedObject )
        {
        iOperationState = EUniProcessTxtProcess;
        CompleteSelf( KErrNone );
        }
    else
        {
        iOperationState = EUniProcessTxtReady;
        CompleteSelf( KErrNone );
        }
    }
  
// ---------------------------------------------------------
// CUniEditorProcessTextOperation::DoStartProcessL
// ---------------------------------------------------------
//
void CUniEditorProcessTextOperation::DoStartProcessL()
    {
    if ( !iEditStore )
        {
        iEditStore = iDocument.Mtm().Entry().EditStoreL();
        }
    
    if ( !iParser )
        {
        iParser = CXhtmlParser::NewL( this );
        iParser->SetMode( ETrue, ETrue );
        }
    
    RFile sourceFile = 
        iEditStore->AttachmentManagerL().GetAttachmentFileL( iProcessedObject->AttachmentId() );
    
    iParser->CreateDomL( sourceFile ); // Takes ownership
    SetPending();
    }

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::Reset
// ---------------------------------------------------------
//
void CUniEditorProcessTextOperation::Reset()
    {
    delete iEditStore;
    iEditStore = NULL;
    }

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::ParseCompleteL
// ---------------------------------------------------------
//
void CUniEditorProcessTextOperation::ParseCompleteL()
    {
    CRichText* text = CRichText::NewL( iDocument.EikonEnv()->SystemParaFormatLayerL(), 
                                       iDocument.EikonEnv()->SystemCharFormatLayerL() );
    CleanupStack::PushL( text );
    
    iParser->ParseL( *text );
    
    RFile fileHandle;
    CreateEmptyTextAttachmentL( fileHandle );
    
    CleanupClosePushL( fileHandle );
    
    WriteTextToFileL( *text, fileHandle );
    
    ReplaceXhtmlAttachmentL( fileHandle );
    
    CleanupStack::PopAndDestroy( 2, text );
    
    iOperationState = EUniProcessTxtCheck;
    CompleteOperation( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::ParseError
// ---------------------------------------------------------
//
void CUniEditorProcessTextOperation::ParseError( TInt aError )
    {
    TRAP_IGNORE( HandleParserErrorL( aError ) );
    
    iOperationState = EUniProcessTxtError;
    CompleteOperation( aError );
    }

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::HandleParserErrorL
// ---------------------------------------------------------
//
void CUniEditorProcessTextOperation::HandleParserErrorL( TInt /*aError*/ )
    {
    if ( iNewAttachmentId != KMsvNullIndexEntryId )
        {
        if ( !iEditStore )
            {
            iEditStore = iDocument.Mtm().Entry().EditStoreL();
            }
            
        MMsvAttachmentManager& manager = iEditStore->AttachmentManagerL();
        MMsvAttachmentManagerSync& managerSync = iEditStore->AttachmentManagerExtensionsL();    
        managerSync.RemoveAttachmentL( CUniDataUtils::IndexPositionOfAttachmentL( manager, iNewAttachmentId ) );
        iEditStore->CommitL();
        
        iNewAttachmentId = KMsvNullIndexEntryId;
        }
    }

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::CreateEmptyTextAttachmentL
// ---------------------------------------------------------
//
void CUniEditorProcessTextOperation::CreateEmptyTextAttachmentL( RFile& aFileHandle )
    {
    // Get the file name from original full path name.
    TParsePtrC parser( iProcessedObject->MimeInfo()->ContentLocation() );

    MMsvAttachmentManagerSync& managerSync = iEditStore->AttachmentManagerExtensionsL();
    
    CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( attachment );
    
    attachment->SetMimeTypeL( KMsgMimeTextPlain );
    
    TFileName* newFileName = new( ELeave ) TFileName;
    CleanupStack::PushL( newFileName );
    
    newFileName->Append( parser.Name() );
    newFileName->Append( KUniExtPlainText() );    
    
    managerSync.CreateAttachmentL( *newFileName, aFileHandle, attachment );
    
    CleanupStack::PopAndDestroy( newFileName );
    CleanupStack::Pop( attachment ); // ownership transferred
    
    // Store the ID as it needs to be removed if something goes wrong after
    // this point and before the attachment is completely initialized.
    iNewAttachmentId = attachment->Id();
    
    iEditStore->CommitL();
    }

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::ReplaceXhtmlAttachmentL
// ---------------------------------------------------------
//
void CUniEditorProcessTextOperation::ReplaceXhtmlAttachmentL( RFile& aFileHandle )
    {   
    // edit store must be closed as RemoveObject tries to open new store
    // that will fail if other handles to store are open.
    delete iEditStore;
    iEditStore = NULL;
    
    TDataType mime( KMsgMimeTextPlain );
    CMsgTextInfo* info = CMsgTextInfo::NewL( aFileHandle,
                                             mime,
                                             iFs );
    CleanupStack::PushL( info );
    
    info->SetCharacterSet( KCharacterSetIdentifierUtf8 );
    
    iDocument.DataModel()->SmilModel().RemoveObjectL( iSlideNum, iProcessedObject );
    iDocument.DataModel()->SmilModel().AddStoredObjectL( iSlideNum, iNewAttachmentId, info );
    
    // Attachment ownership is transferred to newly created UniObject
    iNewAttachmentId = KMsvNullIndexEntryId;
    
    // Info ownership is transferred to newly created UniObject
    CleanupStack::Pop( info ); 
    
    iDocument.SetBodyModified( ETrue );
    }
        
// ---------------------------------------------------------
// CUniEditorProcessTextOperation::RunError
// ---------------------------------------------------------
//
TInt CUniEditorProcessTextOperation::RunError( TInt aError )
    {    
    return CUniEditorOperation::RunError( aError );
    }

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::WriteTextToFileL
// ---------------------------------------------------------
//
void CUniEditorProcessTextOperation::WriteTextToFileL( CRichText& aText, RFile& aFileHandle )
    {
    RFile writerHandle;
    writerHandle.Duplicate( aFileHandle );
    
    RFileWriteStream writer( writerHandle );
    CleanupClosePushL( writer );
    
    HBufC* buffer = NULL;
    
	TInt textLength = aText.DocumentLength();
	if ( textLength > 0 )
		{
		buffer = HBufC::NewL( textLength );
		TPtr bufferPtr = buffer->Des();
		aText.Extract( bufferPtr );
		}
    else 
        {
        return;
        }
        
    CleanupStack::PushL( buffer );

    // Convert the text to utf8
    CCnvCharacterSetConverter* converter = CCnvCharacterSetConverter::NewLC();
    
    if ( converter->PrepareToConvertToOrFromL( KCharacterSetIdentifierUtf8, iFs ) != 
                                                    CCnvCharacterSetConverter::EAvailable )
        {
        User::Leave( KErrNotSupported );
        }

    TBuf8<128> outputBuffer;
    TPtrC16 remainderOfUnicodeText( *buffer );
    FOREVER
        {
        TInt returnValue = converter->ConvertFromUnicode( outputBuffer, remainderOfUnicodeText );
        if ( returnValue == CCnvCharacterSetConverter::EErrorIllFormedInput )
            {
            User::Leave( KErrCorrupt );
            }
        else if ( returnValue < 0 ) // future-proof against "TError" expanding
            {
            User::Leave( KErrGeneral );
            }

        // ? - do something here with outputBuffer
        writer.WriteL( outputBuffer );

        if ( returnValue == 0 )
            {
            break; // all of aUnicodeText has been converted and handled
            }
        remainderOfUnicodeText.Set( remainderOfUnicodeText.Right( returnValue ) );
        }

    CleanupStack::PopAndDestroy( 3, &writer ); //converter, buffer, writer
    }

// ---------------------------------------------------------
// CUniEditorProcessTextOperation::NotifyObserver
// ---------------------------------------------------------
//
void CUniEditorProcessTextOperation::NotifyObserver( TUniEditorOperationEvent aEvent )
    {
    Reset();
    iObserver.EditorOperationEvent( EUniEditorOperationProcessText,
                                    aEvent );
    }

// End of file
