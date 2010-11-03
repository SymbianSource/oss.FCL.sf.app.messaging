/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Creates and sends as many MMS messages as there are files
*                to be send. Oversize JPEG images are compressed to fit into
*                message.
*
*/



// INCLUDE FILES
#include    <coemain.h>
#include    <MuiuMsvUiServiceUtilities.h>
#include    <MmsConst.h>
#include    <mmsclient.h>
#include    <MmsDirectUploadUi.rsg>
#include    <eikprogi.h>
#include    <StringLoader.h>
#include    <msvids.h>
#include    <apmstd.h>
#include    <CMsvMimeHeaders.h>
#include    <mtmuibas.h>                // MTM UI base
#include    <mtuireg.h>                 // MTM UI registery

#include    <MTMExtendedCapabilities.hrh>
#include    <UniImageProcessor.h>
#include    <MsgMimeTypes.h>
#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

#include    <SendUiConsts.h>
#include    <sendnorm.rsg>
#include    <senduisingleton.h>

#include    "SendUiFileRightsEngine.h"
#include    "MDUCreateAndSend.h"
#include    "CSendUiAttachment.h"

// CONSTANTS
const TInt KProgressIncrement           = 1;
const TInt KTenKiloBytes                = 10240;
const TInt KImageTargetWidth            = 1600;
const TInt KImageTargetHeight           = 1200;

// Ram drive, path structure is created if it does not exist
_LIT( KTempFilePath, ":\\system\\temp\\mmsupload\\");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMDUCreateAndSend::CMDUCreateAndSend
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMDUCreateAndSend::CMDUCreateAndSend(
    CCoeEnv& aCoeEnv,
    CSendUiSingleton& aSingleton,
    MMDUCreateAndSendCallback* aCallback )
    : CActive( EPriorityStandard ),
    iCoeEnv( aCoeEnv ),
    iSingleton( aSingleton ),
    iFs( iCoeEnv.FsSession() ),
    iCallback( aCallback ),
    iMessageCreated( EFalse )
    {
    CActiveScheduler::Add( this );
    }
    
// -----------------------------------------------------------------------------
// CMDUCreateAndSend::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMDUCreateAndSend::ConstructL()
    {
    iError = iSendAsSession.Connect();
    }

// -----------------------------------------------------------------------------
// CMDUCreateAndSend::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMDUCreateAndSend* CMDUCreateAndSend::NewL(
    CCoeEnv& aCoeEnv,
    CSendUiSingleton& aSingleton,
    MMDUCreateAndSendCallback* aCallback )
    {
    CMDUCreateAndSend* self = CMDUCreateAndSend::NewLC( aCoeEnv, aSingleton, aCallback );
    
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CMDUCreateAndSend::NewLC
// Two-phased constructor.
// ---------------------------------------------------------
//
CMDUCreateAndSend* CMDUCreateAndSend::NewLC(
    CCoeEnv& aCoeEnv,
    CSendUiSingleton& aSingleton,
    MMDUCreateAndSendCallback* aCallback)
    {
    CMDUCreateAndSend* self = new (ELeave) CMDUCreateAndSend( aCoeEnv, aSingleton, aCallback );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }    
    
// Destructor
CMDUCreateAndSend::~CMDUCreateAndSend()
    {
    iSendAsSession.Close();
    Cancel();
    Reset();
    }


// -----------------------------------------------------------------------------
// CMDUCreateAndSend::CMDUCreateAndSend
// Creates and sends as many MMS messages as there are files
// to be send. Oversize JPEG images are compressed to fit into
// message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMDUCreateAndSend::CreateAndSendL(
    CArrayPtrFlat<CSendUiAttachment>* aAttachments,
    const TDesC& aToAddress,
    const TDesC& aToAlias,
    TInt aMaxMsgSize )
    {
    iError = KErrNone;
    iCompressionOk = 0;
    iCompressionFail = 0;
    iMaxMsgSize = aMaxMsgSize;
    
    if ( aAttachments->Count() )
        {
        iAttachments = aAttachments;       
        }
    else
        {
        User::Leave( KErrArgument );
        }

    if( aToAlias.Length() )
        {
        iToAlias = aToAlias.AllocL();
        }
    else
        {
        User::Leave( KErrArgument );
        }

    if( aToAddress.Length() )
        {
        iToAddress = aToAddress.AllocL();
        }
    else
        {
        User::Leave( KErrArgument );
        }

    if ( !CheckMmsServiceAvailabilityL() )
        {
        CSendUiFileRightsEngine* fileRightsEngine = 
            CSendUiFileRightsEngine::NewLC( iCoeEnv.FsSession() );
            
            TUint i;
            if ( iAttachments->Count() > 1 ) 
                {
                i = R_SENDUI_SETTINGS_NOT_OK_MANY;
                }
            else
                {
                i = R_SENDUI_SETTINGS_NOT_OK;   
                }
               
            fileRightsEngine->ShowInformationNoteL( i );
         CleanupStack::PopAndDestroy( fileRightsEngine );
         User::Leave( KErrCompletion );
         }

    CreateProgressNoteL( aAttachments->Count() );

    iState = EStateCreateMessage;

    CreateMessageAndSetDataL();
    }

// -----------------------------------------------------------------------------
// CMDUCreateAndSend::RunL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMDUCreateAndSend::RunL()
    {
    if ( iStatus < 0 )
        {
        iError = iStatus.Int();
        }

    if ( iError != KErrNone )
        {
        Reset();
        iCallback->MessagesReady();
        }
    else 
        {
        switch ( iState )
            {
            case EStateCreateMessage:
                {
                if ( iMessageCount < iAttachments->Count() )
                    {
                    CreateMessageAndSetDataL();                   
                    }
                else
                    {
                    iState = EStateReady;
                    CompleteSelf();
                    }
                }
                break;
            case EStateImageCompression:
                {
                if ( IsCompressionNeededL() )
                    {
                    CompressImageL();
                    }
                else
                    {
                    iState = EStateAddAttachment;
                    CompleteSelf();
                    }
                }
                break;
            case EStateAddAttachment:
                {
                AddAttachmentL();
                }
                break;
            case EStateSendMessage:
                {
                SendMessage();
                }
                break;
            case EStateFinalize:
                {
                TSendAsProgress progress;
                iSendAsMessage.ProgressL( progress );
                
                if ( progress.iError == KErrNone )
                    {
                    FinalizeL();              
                    }
                else
                    {
                    iState = EStateReady;
                    CompleteSelf();
                    }
                }
                break;
            case EStateReady:
                {
                iCallback->MessagesReady();
                Reset();
                iState = EStateIdle;
                }
                break;                
            default:
                {
                // Should never end up here. 
                }
                break;
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CMDUCreateAndSend::DoCancel
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMDUCreateAndSend::DoCancel()
    {
    if ( iCompressor )
	    {
	    iCompressor->Cancel();
	    }
	iSendAsMessage.Cancel();
	iMessageCreated = EFalse;
    }    

// -----------------------------------------------------------------------------
// CMDUCreateAndSend::RunError
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMDUCreateAndSend::RunError( TInt aError )
    {
    iError = aError;
    Reset();
    iCallback->MessagesReady();
    return KErrNone;
    }
    
// ---------------------------------------------------------
// CMDUCreateAndSend::CompleteSelf
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMDUCreateAndSend::CompleteSelf()
    {
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive();
    User::RequestComplete( pStatus, KErrNone );
    }
    
// -----------------------------------------------------------------------------
// CMDUCreateAndSend::Reset
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMDUCreateAndSend::Reset()
    {
    iMessageCount = 0;
    
    delete iCompressor;
    iCompressor = NULL;
   
    delete iToAddress;
    iToAddress = NULL;

    delete iToAlias;
    iToAlias = NULL;

    if ( iMessageCreated )
        {
        iSendAsMessage.Close();
        iMessageCreated = EFalse;
        }

    if( iProgressDialog )
        {
        // Progress note showing, try to dismiss it
        TRAP_IGNORE( iProgressDialog->ProcessFinishedL() );

        // If the ProcessFinishedL() leaves, force the dialog away with delete
        delete iProgressDialog;
        iProgressDialog = NULL;   
        }

    if ( iImageCompressed )
        {
        iCompressedImage.Close();
        // Delete the temporary file from RAM
        iFs.Delete( iCompressedImagePath );
        iImageCompressed = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CMDUCreateAndSend::CreateMessageAndSetDataL
// Starts message entry creation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMDUCreateAndSend::CreateMessageAndSetDataL()
    {
    iStatus = KRequestPending;

    iSendAsMessage.CreateL( iSendAsSession, KSenduiMtmMmsUid );
    iMessageCreated = ETrue;

    iSendAsMessage.SetBioTypeL( KUidMsgSubTypeMmsUpload );

    iSendAsMessage.AddRecipientL(
        *iToAddress,
        *iToAlias,
        RSendAsMessage::ESendAsRecipientTo );

    // Set file name as subject.
    TFileName fileName;
    CSendUiAttachment& attachment = *(iAttachments->At(iMessageCount));
    attachment.Handle()->Name( fileName );

    iSendAsMessage.SetSubjectL( fileName );

    iState = EStateImageCompression;
    CompleteSelf();
    }

// ---------------------------------------------------------
// CMDUCreateAndSend::IsCompressionNeededL
// Checks if file is oversized JPEG image.
// Returns ETrue if image should be compressed. 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CMDUCreateAndSend::IsCompressionNeededL()
    {
    iImageCompressed = EFalse;
    iAttachmentSize = 0;

    CSendUiAttachment& attachment = (*iAttachments->At(iMessageCount));

    // Get file size and type.
    iAttachmentSize = attachment.Size();
    if ( iAttachmentSize > iMaxMsgSize && 
         attachment.MimeType().CompareF( KMsgMimeImageJpeg ) == 0 )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CMDUCreateAndSend::CompressImageL
// Compress oversized JPEG image.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMDUCreateAndSend::CompressImageL()
    {
    TInt targetSize = iMaxMsgSize - KTenKiloBytes;
    RFile sourceFile = *(*iAttachments->At(iMessageCount)).Handle();
    CreateTempFileL(  sourceFile, iCompressedImage );

    TSize targetDimensions( KImageTargetWidth, KImageTargetHeight );
    if ( !iCompressor )
        {
        iCompressor = new ( ELeave ) CUniImageProcessor( this );
        }
    iCompressor->ProcessImageL( 
        sourceFile,
        iCompressedImage,
        targetDimensions,
        KMsgMimeImageJpeg,
        ETrue,
        targetSize );
    }

// ---------------------------------------------------------
// CMDUCreateAndSend::AddAttachment
// Add attachment to message.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMDUCreateAndSend::AddAttachmentL()
    {
    CSendUiAttachment& attachmentPacket = *(iAttachments->At(iMessageCount));
    if ( iAttachmentSize > 0 && iAttachmentSize < iMaxMsgSize )
        {
        if ( iImageCompressed )
            {
            iSendAsMessage.AddAttachment( 
                iCompressedImage, 
                iStatus );
            }
        else
            {
            RFile attachment;
            attachment.Duplicate( *attachmentPacket.Handle());
            iSendAsMessage.AddAttachment( 
                attachment,
                iStatus );                
            }
    	iState = EStateSendMessage;
    	SetActive();
        }
       else
       	{
		// file could not be fit into the message
       	FinalizeL();
       	}

    
    }

// ---------------------------------------------------------
// CMDUCreateAndSend::SendMessage
// Sends message.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMDUCreateAndSend::SendMessage()
    {
    iSendAsMessage.SendMessage( iStatus );        
    iState = EStateFinalize;
    SetActive();
    }
    
// ---------------------------------------------------------
// CMDUCreateAndSend::FinalizeL
// Updates progress bar.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMDUCreateAndSend::FinalizeL()
    {
    iMessageCount++;

    iSendAsMessage.Close();
    iMessageCreated = EFalse;

    // Update progress note text
    if ( iMessageCount < iAttachments->Count() )
        {
        // Delete the temporary file from RAM
        if ( iImageCompressed )
            {
            iCompressedImage.Close();
            iFs.Delete( iCompressedImagePath );
            iImageCompressed = EFalse;
            }
        UpdateProgressTextL( iAttachments->Count() );
        iState = EStateCreateMessage;
        }
    else
        {
        iProgressDialog->ProcessFinishedL();
        iState = EStateReady;
        }
    
    CompleteSelf();
    }

// ---------------------------------------------------------
// CMDUCreateAndSend::CreateTempFileL
// Creates temp file to EDefaultRam:\system\temp\mmsupload. Original filename is used.
// Note: handle to created file is left open.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMDUCreateAndSend::CreateTempFileL(     
    RFile&              aSourceFile,
    RFile&              aTargetFile )
    {
    // Construct target path
    TInt drive;
#ifdef RD_MULTIPLE_DRIVE
    User::LeaveIfError( DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRam, drive ) );
#else
    drive = EDriveD;
#endif    

    iCompressedImagePath.Zero();
    TChar driveLetter = TChar('A'); 
    driveLetter += drive ;
    iCompressedImagePath.Append( driveLetter );  
    iCompressedImagePath.Append( KTempFilePath );
    
    // Create temp directory if needed
    TInt err = iFs.MkDirAll( iCompressedImagePath );

    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }

    TFileName sourceName;
    aSourceFile.Name( sourceName );
    
    iCompressedImagePath.Append( sourceName );

    // Create and open new file
    User::LeaveIfError( aTargetFile.Replace(
        iFs,
        iCompressedImagePath,
        EFileWrite | EFileShareAny ) );
    }

// ---------------------------------------------------------
// CMDUCreateAndSend::CreateProgressNoteL
// Creates progress bar.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMDUCreateAndSend::CreateProgressNoteL( TInt aFinalValue )
    {
    iProgressDialog = new (ELeave) CAknProgressDialog(
                        reinterpret_cast< CEikDialog** > (&iProgressDialog) );

    iProgressDialog->PrepareLC( R_MMS_DIRECT_UPLOAD_PROGRESS_NOTE );
    iProgressDialog->SetTone( CAknNoteDialog::ENoTone );
    iProgressInfo = iProgressDialog->GetProgressInfoL();
    iProgressInfo->SetFinalValue( aFinalValue );

    iProgressCount = 1;
    UpdateProgressTextL( aFinalValue );

    // Register for cancel event and execute the note
    iProgressDialog->SetCallback( this );
    iProgressDialog->RunLD();
    }        

// ---------------------------------------------------------
// CMDUCreateAndSend::UpdateProgressTextL
// Updates progress bar text.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMDUCreateAndSend::UpdateProgressTextL( TInt aFinalValue )
    {
    CArrayFixFlat< TInt >* numbers = new( ELeave ) CArrayFixFlat< TInt > (2);
    CleanupStack::PushL( numbers );

    numbers->AppendL( iProgressCount );
    numbers->AppendL( aFinalValue );

    HBufC* progressText = StringLoader::LoadLC(
        R_MMS_DIRECT_UPLOAD_PROGRESS, *numbers, &iCoeEnv );

    if ( iProgressDialog ) 
        {
        iProgressDialog->SetTextL( *progressText );
        iProgressInfo->IncrementAndDraw( KProgressIncrement );
        iProgressCount++;
        }

    CleanupStack::PopAndDestroy( 2, numbers );  // progressText, numbers
    }

// ---------------------------------------------------------
// CMDUCreateAndSend::DialogDismissedL
// Callback method for progress dialog to inform us when dialog is dismissed.
// This function is called when cancel button of the progress bar was pressed.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMDUCreateAndSend::DialogDismissedL( TInt aButtonId )
    {
    // Check if cancel button was pressed.
    if ( aButtonId == EAknSoftkeyCancel )
        {
        DoCancel();

        iState = EStateReady;
        }
    }

// -----------------------------------------------------------------------------
// CMDUCreateAndSend::CheckMmsServiceAvailabilityL
// Checks that MMS service is available.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMDUCreateAndSend::CheckMmsServiceAvailabilityL()
    {
    CBaseMtm* mtm = iSingleton.ClientMtmRegistryL().NewMtmL( KSenduiMtmMmsUid );
    CleanupStack::PushL( mtm );
    
    CBaseMtmUi* mtmUi = iSingleton.MtmUiRegistryL().NewMtmUiL( *mtm );
    CleanupStack::PushL( mtmUi );
    
    TPckgBuf<TBool> paramPack( ETrue );
        
    // This is not actually used.
    CMsvEntrySelection* dummySelection = new ( ELeave ) CMsvEntrySelection; 
    CleanupStack::PushL( dummySelection );

    mtmUi->InvokeSyncFunctionL( KMtmUiFunctionValidateService, 
                                *dummySelection, 
                                paramPack );

    CleanupStack::PopAndDestroy( 3, mtm );
    return ( KErrNone == paramPack() );
    }


// ---------------------------------------------------------
// CMDUCreateAndSend::ImageProcessingReady
// Callback function called when compression is ready.
// (other items were commented in a header).
// ---------------------------------------------------------
//    
void CMDUCreateAndSend::ImageProcessingReady( TSize /*aBitmapSize*/, TInt aFileSize, TBool /*aCompressed*/ )
    {
    iAttachmentSize = aFileSize;
    
    TInt error = iCompressor->Error();
    
    if ( error == KErrNone )
        {
        iCompressionOk++;
        iState = EStateAddAttachment;
        }
    else
        {
        iCompressionFail++;
        iState = EStateFinalize;
        }
    
    iImageCompressed = ETrue;

    CompleteSelf();
    }
	
//  End of File

