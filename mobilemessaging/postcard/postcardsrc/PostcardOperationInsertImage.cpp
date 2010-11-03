/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*       CPostcardOperationInsertImage
*
*/



// ========== INCLUDE FILES ================================

#include <AknsConstants.h>
#include <data_caging_path_literals.hrh> 
#include <apmstd.h>  // TDataType
#include <f32file.h> // RFs, RFile
#include <gulicon.h> // CGulIcon
#include <apmstd.h>  // TDataType
#include <e32des8.h>

#include <IHLImageFactory.h>
#include <IHLViewerFactory.h>
#include <MIHLFileImage.h>
#include <MIHLBitmap.h>
#include <MIHLImageViewer.h>

#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <cmsvattachment.h>
#include <cmsvmimeheaders.h>

#include <MsgMediaResolver.h>
#include <MsgMediaInfo.h>
#include <MsgImageInfo.h>
#include <MsgMimeTypes.h>

#include <mmsclient.h>
#include <Postcard.rsg>

#include "PostcardOperationInsertImage.h"
#include "PostcardLaf.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CPostcardOperationInsertImage::NewL
//
// Factory method.
// ---------------------------------------------------------
//
CPostcardOperationInsertImage* CPostcardOperationInsertImage::NewL(
            MPostcardOperationObserver& aObserver,
            CPostcardDocument& aDocument,
            CPostcardAppUi& aAppUi,
            CMsgImageInfo* aInfo,
            RFs& aFs )
    {
    CPostcardOperationInsertImage* self = 
        new ( ELeave ) CPostcardOperationInsertImage( aObserver, aDocument, aAppUi, aInfo, aFs );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CPostcardOperationInsertImage::CPostcardOperationInsertImage
// ---------------------------------------------------------
CPostcardOperationInsertImage::CPostcardOperationInsertImage(
            MPostcardOperationObserver& aObserver,
            CPostcardDocument& aDocument,
            CPostcardAppUi& aAppUi,
            CMsgImageInfo* aInfo,
            RFs& aFs ) :
    CPostcardOperation( aObserver, aDocument, aAppUi, aFs ),
    iInfo( aInfo )
    {
    }

// ---------------------------------------------------------
// CPostcardOperationInsertImage::CPostcardOperationInsertImage
// ---------------------------------------------------------
CPostcardOperationInsertImage::~CPostcardOperationInsertImage( )
    {
    delete iImageProcessor;
    delete iInfo;
    if( iCompressFile )
    	{
    	iCompressFile->Close();
        delete iCompressFile;
        iCompressFile = NULL;
    	}
    Cancel( );
    }

// ---------------------------------------------------------
// CPostcardOperationInsertImage::Start
// ---------------------------------------------------------
void CPostcardOperationInsertImage::Start( TInt /*aArgument*/ )
    {
    iLaunchState = EPostcardInsertImageInitializing;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CPostcardOperationInsertImage::Launch
// ---------------------------------------------------------
void CPostcardOperationInsertImage::DoLaunchStepL()
    {
    iFlags &= ~EPostcardInsertImageRequestActive;
    switch ( iLaunchState )
        {
        case EPostcardInsertImageInitializing:
            {
            InitL( );
            iLaunchState = EPostcardInsertImageCheckAttas;
            CompleteSelf( KErrNone );
            break;
            }
        case EPostcardInsertImageCheckAttas:
            {
            if( !CheckAttasL( ) )
            	{ // If there was error, this has been completed already
	            iLaunchState = EPostcardInsertImageCreateAtta;
	            CompleteSelf( KErrNone );
            	}
            break;
            }
        case EPostcardInsertImageCreateAtta:
            {
            DoCreateAttaL( );
            break;
            }
        case EPostcardInsertImageRename:
        	{
        	DoRenameAttaL( );
        	break;
        	}
        case EPostcardInsertProcessImage:
        	{
        	DoStartProcessImageL( );
        	break;
        	}
        case EPostcardInsertImageFinalizeSave:
            {
            DoFinalizeSaveL( );
            iLaunchState = EPostcardInsertImageCreateBitmap;
            CompleteSelf( KErrNone );
            break;
            }
      	case EPostcardInsertImageCreateBitmap:
      		{
      		DoCreateBitmapL( );
      		break;	
      		}
        case EPostcardInsertImageFinished:
            {
		    UnInitL( );
            iObserver.PostcardOperationEvent(
                EPostcardOperationInsertImage,
                EPostcardOperationComplete ); 
            break;
            }
        default:
		    UnInitL( );
            iObserver.PostcardOperationEvent(
                EPostcardOperationInsertImage,
                EPostcardOperationError ); 
            break;
        }
    }

// ---------------------------------------------------------
// DoCreateAttaL
// ---------------------------------------------------------
void CPostcardOperationInsertImage::DoCreateAttaL( )
    {
    CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( attachment );
    
	if( iImage )
		{ // Remove the old image
	 	TInt oldImage = TMsvIdToIndexL( iImage );
		iStore->AttachmentManagerExtensionsL( ).RemoveAttachmentL( oldImage );
		}

	TSize size = iInfo->Dimensions( );

	TInt width = size.iWidth;
	TInt height = size.iHeight;
	if( height > width )
		{
		width = size.iHeight;
		height = size.iWidth;
		}
	
	TBool needsScaling = EFalse;
	TBool isJpeg = EFalse;
	
    if( iInfo->MimeType( ).CompareF( KMsgMimeImageJpeg ) == 0 )
		{
		isJpeg = ETrue;
		}
	else if( iInfo->MimeType( ).CompareF( KMsgMimeImagePng ) == 0 )
        { // PNG always needs to be converted into jpeg
        needsScaling = ETrue;
        }
        
	if( needsScaling )   
		{
        // Needs converting first -> create just an empty RFile for now
   	    iCompressFile = new ( ELeave ) RFile;
	    attachment->SetMimeTypeL( KMsgMimeImageJpeg );
	    attachment->SetAttachmentNameL( KPostcardFilenameScaled );
        iManager->CreateAttachmentL( KPostcardFilenameScaled, *iCompressFile, attachment, iStatus );
        iLaunchState = EPostcardInsertProcessImage;
	    iCompressAtta = attachment;

        iFlags |= EPostcardInsertImageRequestActive;
        }
    else
        {
        TBool needsCompress = NeedsCompress( ); 
        TParsePtrC parse( iInfo->FullFilePath() );

		if( isJpeg )
			{
		    attachment->SetMimeTypeL( KMsgMimeImageJpeg );
			}
		else
			{
		    attachment->SetMimeTypeL( KMsgMimeImageGif );
			}

        if( needsCompress )    
        	{ // Compression needed -> create just an empty attachment for now
    	    iCompressFile = new ( ELeave ) RFile;
    	    attachment->SetAttachmentNameL( KPostcardFilenameCompressed );
            iManager->CreateAttachmentL( KPostcardFilenameCompressed, *iCompressFile, attachment, iStatus );
            iFlags |= EPostcardInsertImageRequestActive;
	        iLaunchState = EPostcardInsertProcessImage;
			iCompressAtta = attachment;
        	}
        else
        	{
        	if( isJpeg )
        		{
	    	    attachment->SetAttachmentNameL( KPostcardFilenameNoOp );
        		}
        	else
        		{
	    	    attachment->SetAttachmentNameL( KPostcardFilenameNoOpGif );
        		}
            iManager->AddAttachmentL( iInfo->FullFilePath( ), attachment, iStatus );
            iFlags |= EPostcardInsertImageRequestActive;
            iLaunchState = EPostcardInsertImageFinalizeSave;
            iNoOpAtta = attachment;
        	}
        }

    CleanupStack::Pop( attachment );
    SetActive();    	    			
    }

// ---------------------------------------------------------
// DoStartProcessImageL
// ---------------------------------------------------------
void CPostcardOperationInsertImage::DoStartProcessImageL( )
	{
	if( !iImageProcessor )
	    {
        iImageProcessor = new (ELeave) CUniImageProcessor( this );	    
	    }
    RFile tempFile;
	User::LeaveIfError( tempFile.Open( iFs, iInfo->FullFilePath( ), EFileRead | EFileShareAny ) );
    CleanupClosePushL( tempFile );    
	
	TSize maxDimensions = iDocument.MaxImageDimensions( );

	TSize origDimensions = static_cast<CMsgImageInfo*>(iInfo)->Dimensions( );

	if( origDimensions.iWidth < origDimensions.iHeight )
		{ // It's portrait so swap max dimensions
		origDimensions = maxDimensions;
		maxDimensions.iWidth = origDimensions.iHeight;
		maxDimensions.iHeight = origDimensions.iWidth;
		}

    iImageProcessor->ProcessImageL( 	tempFile, 
    									*iCompressFile, 
    									maxDimensions,
    									KMsgMimeImageJpeg, 
    									ETrue,
    									iDocument.MaxImageSize( ) );
    CleanupStack::PopAndDestroy( &tempFile ); // tempFile
	}

// ---------------------------------------------------------
// DoFinalizeSaveL
// ---------------------------------------------------------
void CPostcardOperationInsertImage::DoFinalizeSaveL( )
    {
    
    iHeaders = CMsvMimeHeaders::NewL( );
    
    CMsvAttachment* atta = iNoOpAtta;
    if( iScaleAtta )
    	{
    	atta = iScaleAtta;
    	}
    
    if( iCompressAtta )
    	{
		atta = iCompressAtta;
    	}
    
    if( atta )
    	{
		iHeaders->SetContentLocationL( atta->AttachmentName( ) );
		iHeaders->SetContentTypeL( atta->MimeType( ) );
	    iHeaders->StoreL( *atta );
    	}
	
    // Lets remove the flag just in case
    iFlags &= ~EPostcardInsertImageConvertingImage;
    
	iStore->Commit( ); // Is this needed?
    
    CheckAttasL( );
    
    iCommit = ETrue;
    }

// ---------------------------------------------------------
// DoCreateBitmapL
// ---------------------------------------------------------
void CPostcardOperationInsertImage::DoCreateBitmapL( )
	{
	RFile imageFile = iManager->GetAttachmentFileL( iImage );
	CleanupClosePushL( imageFile );
	
	// The scaling for screen bitmap is started here

    iSourceImage = IHLImageFactory::OpenFileImageL( imageFile );
    iDestinationBitmap = IHLBitmap::CreateL();
    
    iImageHandler = IHLViewerFactory::CreateImageViewerL( PostcardLaf::Image( ).Size( ), 
                                                    *iSourceImage, 
                                                    *iDestinationBitmap, 
                                                    *this, 
                                                    TUint32( 0 ) );
    User::LeaveIfError( iImageHandler->SetSourceRectPosition( TPoint( 0, 0 ) ) );

    CleanupStack::PopAndDestroy( &imageFile );   // imageFile
	}

// ---------------------------------------------------------
// NeedsCompress
// ---------------------------------------------------------
TBool CPostcardOperationInsertImage::NeedsCompress( )
    {
    if ( iInfo->FileSize( ) > iDocument.MaxMessageSize( ) )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// ImageProcessingReady
// ---------------------------------------------------------
void CPostcardOperationInsertImage::ImageProcessingReady( TSize aBitmapSize, TInt aFileSize, TBool /*aCompressed*/ )
	{
    if( iCompressFile )
	    {
        iCompressFile->Close();
        delete iCompressFile;
        iCompressFile = NULL;
	    }

	TBool imageFits = EFalse;
	TSize maxDimensions = iDocument.MaxImageDimensions( );
	if( aBitmapSize.iWidth <= maxDimensions.iWidth && 
		aBitmapSize.iHeight <= maxDimensions.iHeight )
		{
		imageFits = ETrue;
		}
	else if( aBitmapSize.iWidth <= maxDimensions.iHeight && 
		aBitmapSize.iHeight <= maxDimensions.iWidth )
		{
		imageFits = ETrue;
		}

	if( aBitmapSize.iWidth == 0 || !imageFits ||
	    aFileSize > iDocument.MaxImageSize( ) )
		{ // The process was not successful
        SetError( R_POSTCARD_PROCESS_NOT_SUCCESSFUL );
        iLaunchState = EPostcardInsertImageError;
        CompleteSelf( -1 );
        return;
		}

	iLaunchState = EPostcardInsertImageFinalizeSave;
    CompleteSelf( KErrNone );
	}

// ---------------------------------------------------------
// CPostcardOperationInsertImage::DoRenameAttaL
// ---------------------------------------------------------
void CPostcardOperationInsertImage::DoRenameAttaL( )
	{
	if( iNoOpAtta )
		{
	    TInt count = iManager->AttachmentCount( );
	    CMsvAttachment* atta = NULL;
	    for(TInt a = 0; a<count; a++)
	        {
	        atta = iManager->GetAttachmentInfoL( a );
	        CleanupStack::PushL( atta );
			if( iScaleAtta->FilePath( ).CompareF( atta->FilePath( ) ) == 0 )
				{
				iManager->RenameAttachmentL( a, iNoOpAtta->AttachmentName( ), iStatus );			
				SetActive( );
				break;
				}
	        }
		}
    iLaunchState = EPostcardInsertImageFinalizeSave;
	}

// ---------------------------------------------------------
// CPostcardOperationInsertImage::ConstructL
// ---------------------------------------------------------
void CPostcardOperationInsertImage::ConstructL( )
    {
    }

// ---------------------------------------------------------
// CPostcardOperationInsertImage::ViewerBitmapChangedL
// ---------------------------------------------------------
void CPostcardOperationInsertImage::ViewerBitmapChangedL()
    {
    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CFbsBitmap* mask = NULL;
    bitmap->Duplicate( iDestinationBitmap->Bitmap().Handle( ) );
    
    if ( iDestinationBitmap->HasMask() )
        {
        mask = new (ELeave) CFbsBitmap;
        mask->Duplicate( iDestinationBitmap->Mask().Handle( ) );
        }
    
    delete iDestinationBitmap;
    iDestinationBitmap = NULL;
    
    delete iSourceImage;
    iSourceImage = NULL;
    
    delete iImageHandler;
    iImageHandler = NULL;
    
    iLaunchState = EPostcardInsertImageFinished;
        
    CGulIcon* icon = NULL;
    TRAPD( err, icon = CGulIcon::NewL( bitmap, mask ) );

    if( err )
        { // Lets nullify it if there was an error
        icon = NULL;
        }
        
	iAppUi.SetImage( icon );

	CompleteSelf( KErrNone );        
        
    }

// ---------------------------------------------------------
// CPostcardOperationInsertImage::ViewerError
// ---------------------------------------------------------
void CPostcardOperationInsertImage::ViewerError( TInt /*aError*/ )
    {
    delete iDestinationBitmap;
    iDestinationBitmap = NULL;
    
    delete iSourceImage;
    iSourceImage = NULL;
    
    delete iImageHandler;
    iImageHandler = NULL;

	SetError( R_POSTCARD_FORMAT_NOT_SUPPORTED );
    iLaunchState = EPostcardInsertImageError;
	CompleteSelf( -3 );
	return;
    }
    
// ---------------------------------------------------------
// CPostcardOperationInsertImage::DoCancel
// ---------------------------------------------------------
void CPostcardOperationInsertImage::DoCancel( )
    {
    if( iManager )
        {
        if( iFlags & EPostcardInsertImageRequestActive )
            {
            iManager->CancelRequest();                
            }
        }
    }

// ---------------------------------------------------------
// CPostcardOperationInsertImage::RunL
// ---------------------------------------------------------
void CPostcardOperationInsertImage::RunL( )
    {
    DoLaunchStepL( );
    }

// ---------------------------------------------------------
// RunError
// ---------------------------------------------------------
TInt CPostcardOperationInsertImage::RunError( TInt aError )
    {
    iLaunchState = -1;
    SetError( aError );
    iObserver.PostcardOperationEvent(
        EPostcardOperationInsertImage,
        EPostcardOperationError );     
    return KErrNone;
    }


// EOF
