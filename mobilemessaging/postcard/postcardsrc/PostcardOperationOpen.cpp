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
*       CPostcardOperationOpen
*
*/



// ========== INCLUDE FILES ================================

#include <AknsConstants.h>
#include <data_caging_path_literals.hrh> 
#include <apmstd.h>  // TDataType
#include <gulicon.h> // CGulIcon
#include <txtetext.h> // For TDocumentStorage

#include <MsgMediaInfo.h>
#include <MsgMedia.hrh>
#include <MsgImageInfo.h>
#include <MsgMediaResolver.h>		// for CMmsMediaResolver
#include <MsgTextInfo.h>
#include <msgtextutils.h>
#include <MsgMimeTypes.h>

#include <IHLImageFactory.h>
#include <IHLViewerFactory.h>
#include <MIHLFileImage.h>
#include <MIHLBitmap.h>
#include <MIHLImageViewer.h>

#include <mmsvattachmentmanager.h>		// AttachmentManager
#include <mmsvattachmentmanagersync.h>	// AttachmentManagerSync
#include <cmsvmimeheaders.h>		// MimeHeaders
#include <mmsclient.h>

#include <vprop.h>	// VCardPropertyValue
#include <cntitem.h>
#include <vcard.h>
#include <cntfldst.h> // CContactTextField

#include <StringLoader.h>			// for StringLoader

#include <Postcard.rsg>       	// for Postcard resources
#include "PostcardLaf.h"
#include "PostcardUtils.h"
#include "Postcard.hrh"
#include "PostcardOperationOpen.h"
#include "PostcardRecipientWrapper.h"
#include "PostcardContact.h"
#include "PostcardPrivateCRKeys.h"  // cenrep keys
#include "PostcardCenRep.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// This was "FN" in 3.0-3.1 but OMA says it should be "N". Accept
// both when opening and be compatible with both.
_LIT8(KPostcardVCardName, "N");
_LIT8(KPostcardVCardNameFN, "FN");
_LIT8(KPostcardVCardAddr, "ADR");

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CPostcardOperationOpen::NewL
// ---------------------------------------------------------
CPostcardOperationOpen* CPostcardOperationOpen::NewL(
            MPostcardOperationObserver& aObserver,
            CPostcardDocument& aDocument,
            CPostcardAppUi& aAppUi,
            RFs& aFs )
    {
    CPostcardOperationOpen* self = 
        new ( ELeave ) CPostcardOperationOpen( aObserver, aDocument, aAppUi, aFs );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CPostcardOperationOpen::CPostcardOperationOpen
// ---------------------------------------------------------
CPostcardOperationOpen::CPostcardOperationOpen(
            MPostcardOperationObserver& aObserver,
            CPostcardDocument& aDocument,
            CPostcardAppUi& aAppUi,
            RFs& aFs ) :
    CPostcardOperation( aObserver, aDocument, aAppUi, aFs ),
    iSpecialFormat( aAppUi.CenRep().GetString( KPocaKeyServiceSpecialFormat ) ),
    iSeparatorChar( aAppUi.CenRep().GetString( KPocaKeyServiceSeparator ) )
    {
    }

// ---------------------------------------------------------
// CPostcardOperationOpen::CPostcardOperationOpen
// ---------------------------------------------------------
CPostcardOperationOpen::~CPostcardOperationOpen( )
    {
    if( iEditFile )
    	{
    	iEditFile->Close();
    	delete iEditFile;
    	}
    delete iImageInfo;
    delete iImageProcessor;
    }

// ---------------------------------------------------------
// CPostcardOperationOpen::Start
// ---------------------------------------------------------
void CPostcardOperationOpen::Start( TInt /*aArgument*/ )
    {
    ToState( EPostcardOpenInitializing );
    }

// ---------------------------------------------------------
// CPostcardOperationOpen::Launch
// ---------------------------------------------------------
void CPostcardOperationOpen::DoLaunchStepL()
    {
    iFlags &= ~EPostcardOpenRequestActive;
    switch ( iLaunchState )
        {
        case EPostcardOpenInitializing:
            {
            InitL( );
            iDocument.InitializeL( iFs );
            ToState( EPostcardOpenCheckAttas );
            break;
            }
        case EPostcardOpenCheckAttas:
            {
            if( !CheckAttasL( ) )
            	{ // If there was error, this has been completed already
	            ToState( EPostcardOpenHandleImage );            		
            	}
            else
                {
                iLaunchState = EPostcardOpenError;
                }
            break;
            }
        case EPostcardOpenHandleImage:
            {
            DoHandleImageL( );
            break;
            }
        case EPostcardOpenProcessImage:
            {
            DoStartProcessImageL( );
            break;
            }
        case EPostcardOpenHandleText:
            {
            DoHandleTextL( );
            break;
            }
        case EPostcardOpenHandleRecipient:
            {
            DoHandleRecipientL( );
            ToState( EPostcardOpenFinished );
            break;
            }
        case EPostcardOpenFinished:
            {
            UnInitL( );
            iObserver.PostcardOperationEvent(
                EPostcardOperationOpen,
                EPostcardOperationComplete ); 
            break;
            }
        default:
            UnInitL( );
            iObserver.PostcardOperationEvent(
                EPostcardOperationOpen,
                EPostcardOperationError ); 
            break;
        }
    }

// ---------------------------------------------------------
// DoHandleL
// ---------------------------------------------------------
void CPostcardOperationOpen::DoHandleImageL( )
    {
    if ( iImage != 0 )
        {
        if( iDocument.MessageType( ) == EPostcardSendAs )
        	{ // SendAs -> Lets see what's inside
			RFile imageFile = iManager->GetAttachmentFileL( iImage );
            CleanupClosePushL( imageFile );
			iImageInfo = iDocument.MediaResolver()->CreateMediaInfoL( imageFile );
			iDocument.MediaResolver()->ParseInfoDetailsL( iImageInfo, imageFile );

            // Lets first check if the file seems to be ok for inserting
            if( iImageInfo->Protection( ) )
                {
                CleanupStack::PopAndDestroy( &imageFile ); // imageFile
				ToErrorState( R_POSTCARD_CANNOT_SEND_PROTECTED );
				return;
                }
            if( iImageInfo->Corrupt( ) )
		        {
		        // Corrupt is set to only supported media types
                CleanupStack::PopAndDestroy( &imageFile ); // imageFile
				ToErrorState( R_POSTCARD_OBJECT_CORRUPTED );
				return;
		        }
		    else if ( !iDocument.IsImage( iImageInfo->MimeType( ) ) )
		        {
                CleanupStack::PopAndDestroy( &imageFile ); // imageFile
				ToErrorState( R_POSTCARD_FORMAT_NOT_SUPPORTED );
				return;
		        }
		    else if ( iImageInfo->MediaType() != EMsgMediaImage )
		        {
                CleanupStack::PopAndDestroy( &imageFile ); // imageFile
				ToErrorState( R_POSTCARD_FORMAT_NOT_SUPPORTED );
				return;
		        }
		    else if ( static_cast<CMsgImageInfo*>(iImageInfo)->IsAnimation( ) )
		        {
                CleanupStack::PopAndDestroy( &imageFile ); // imageFile
				ToErrorState( R_POSTCARD_FORMAT_NOT_SUPPORTED );
				return;
		        }

			TSize dimensions = static_cast<CMsgImageInfo*>(iImageInfo)->Dimensions( );
			TInt imageWidth = dimensions.iWidth>dimensions.iHeight?dimensions.iWidth:dimensions.iHeight;
			TInt imageHeight = dimensions.iWidth>dimensions.iHeight?dimensions.iHeight:dimensions.iWidth;
            
            TSize maxSize = iDocument.MaxImageDimensions( );
            
            // OK, it seems to be pretty ok, now lets see what we can do
            if( ( 	iImageInfo->MimeType( ).CompareF( KMsgMimeImagePng ) == 0 ) 
            	||  ( 		( imageWidth > maxSize.iWidth || imageHeight > maxSize.iHeight ) 
            			&&	iImageInfo->FileSize( ) > iDocument.MaxImageSize( ) ) )
                { // This is PNG or it's a too wide JPEG file
			    iEditFile = new ( ELeave ) RFile;
			    CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    			CleanupStack::PushL( attachment );
			    attachment->SetAttachmentNameL( KPostcardFilenameScaled );
		        iManager->CreateAttachmentL( KPostcardFilenameScaled, *iEditFile, attachment, iStatus );
                iFlags |= EPostcardOpenRequestActive;                
		        CleanupStack::Pop( attachment );    // attachment
			    iEditAtta = attachment;
				iLaunchState = EPostcardOpenProcessImage;	
				SetActive();                
                CleanupStack::PopAndDestroy( &imageFile ); // imagefile
                return;
                }
			else if( iImageInfo->FileSize( ) > iDocument.MaxImageSize( ) )
		    	{ // Image is too big, but not too wide -> create empty attachment for now
			    iEditFile = new ( ELeave ) RFile;
			    CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    			CleanupStack::PushL( attachment );
			    attachment->SetAttachmentNameL( KPostcardFilenameCompressed );
		        iManager->CreateAttachmentL( KPostcardFilenameCompressed, *iEditFile, attachment, iStatus );
                iFlags |= EPostcardOpenRequestActive;
		        CleanupStack::Pop( attachment );    // attachment
			    iEditAtta = attachment;
				iLaunchState = EPostcardOpenProcessImage;	
				SetActive();	        
                CleanupStack::PopAndDestroy( &imageFile ); // imagefile
                return;
				}
			else
			    { // OK, the file is ok, we just need to update the MsvHeaders
			    DoUpdateSendasAttachmentL( iImageInfo->MimeType( ) );
			    }
            CleanupStack::PopAndDestroy( ); // imagefile
        	}
        // Either this is not sendas or the image did not need conversion or compression
		RFile imageFile = iManager->GetAttachmentFileL( iImage );
        CleanupClosePushL( imageFile );
        
        // Scale image to bitmap to be drawn on the screen
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
    else
        {
        ToState( EPostcardOpenHandleText );
        }
    }

// ---------------------------------------------------------
// DoStartProcessImageL
// ---------------------------------------------------------
void CPostcardOperationOpen::DoStartProcessImageL( )
	{
	RFile sourceFile = iManager->GetAttachmentFileL( iImage );
	CleanupClosePushL( sourceFile );	
	if( !iImageProcessor )
	    {
        iImageProcessor = new (ELeave) CUniImageProcessor( this );	    
	    }
	TSize maxDimensions = iDocument.MaxImageDimensions( );

	TSize origDimensions = static_cast<CMsgImageInfo*>(iImageInfo)->Dimensions( );

	if( origDimensions.iWidth < origDimensions.iHeight )
		{ // It's portrait so swap max dimensions
		origDimensions = maxDimensions;
		maxDimensions.iWidth = origDimensions.iHeight;
		maxDimensions.iHeight = origDimensions.iWidth;
		}
	
    iImageProcessor->ProcessImageL( 	sourceFile, 
    									*iEditFile, 
    									maxDimensions,
    									KMsgMimeImageJpeg, 
    									ETrue,
    									iDocument.MaxImageSize( ) );
    CleanupStack::PopAndDestroy(); // sourceFile
	}

// ---------------------------------------------------------
// DoUpdateSendasAttachmentL
// ---------------------------------------------------------
void CPostcardOperationOpen::DoUpdateSendasAttachmentL( const TDataType& aMimeType )
    {
	CMsvMimeHeaders* headers = CMsvMimeHeaders::NewLC( );
	if( aMimeType.Des8( ).CompareF( KMsgMimeImageGif ) == 0  )
	    { // Image is gif
    	headers->SetContentLocationL( KPostcardFilenameNoOpGif);
	    }
	else
	    {
    	headers->SetContentLocationL( KPostcardFilenameNoOp );		    	    
	    }
	headers->SetContentTypeL( aMimeType.Des8( ) );

    // As this function is called only when in SendAs
    // there's certainly just one attachment so we index it directly
    CMsvAttachment* attachment = iManager->GetAttachmentInfoL( 0 );
    CleanupStack::PushL( attachment );
	headers->StoreL( *attachment );
    attachment->SetMimeTypeL( aMimeType.Des8( ) );
    iStore->AttachmentManagerExtensionsL( ).ModifyAttachmentInfoL( attachment );
    CleanupStack::Pop( attachment );

	CleanupStack::PopAndDestroy( headers ); 
	            	
	iStore->Commit( );
    // Lets check again (to get right AttachmentIds)
	CheckAttasL( );        
    }

// ---------------------------------------------------------
// ImageProcessingReady
// ---------------------------------------------------------
void CPostcardOperationOpen::ImageProcessingReady( TSize aBitmapSize, TInt aFileSize, TBool /*aCompressed*/ )
	{
    if( iEditFile )
        {
        iEditFile->Close();
        delete iEditFile;
        iEditFile = NULL;            
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

	if( !imageFits || aFileSize > iDocument.MaxImageSize( ) )
		{ // The process was not successful
		ToErrorState( R_POSTCARD_PROCESS_NOT_SUCCESSFUL );
		return;
		}

    // Call this to start scaling to screen (deletes the previous image etc..)
    TRAPD( err, StartScalingToScreenL( ) );
    if( err )
        {
        ToErrorState( R_POSTCARD_FORMAT_NOT_SUPPORTED );
        }
	}

// ---------------------------------------------------------
// StartScalingToScreenL
// ---------------------------------------------------------
void CPostcardOperationOpen::StartScalingToScreenL( )
    {
    TInt oldImage =  TMsvIdToIndexL( iImage );
    iStore->AttachmentManagerExtensionsL( ).RemoveAttachmentL( oldImage );
	iStore->Commit( );

    DoUpdateSendasAttachmentL( iImageInfo->MimeType( ) );
    
    // Lets add headers
	RFile imageFile = iManager->GetAttachmentFileL( iImage );
	CleanupClosePushL( imageFile );
	
    iSourceImage = IHLImageFactory::OpenFileImageL( imageFile );
    iDestinationBitmap = IHLBitmap::CreateL();
    
    iImageHandler = IHLViewerFactory::CreateImageViewerL( PostcardLaf::Image( ).Size( ), 
                                                    *iSourceImage, 
                                                    *iDestinationBitmap, 
                                                    *this, 
                                                    TUint32( 0 ) );
    User::LeaveIfError( iImageHandler->SetSourceRectPosition( TPoint( 0, 0 ) ) );
	        	
    CleanupStack::PopAndDestroy( &imageFile ); // imageFile
    }

// ---------------------------------------------------------
// ViewerBitmapChangedL
// ---------------------------------------------------------
void CPostcardOperationOpen::ViewerBitmapChangedL()
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
    
    iLaunchState = EPostcardOpenHandleText;
        
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
// ViewerError
// ---------------------------------------------------------
void CPostcardOperationOpen::ViewerError( TInt /*aError*/ )
    {
	ToErrorState( R_POSTCARD_FORMAT_NOT_SUPPORTED );
    }

// ---------------------------------------------------------
// DoHandleTextL
// ---------------------------------------------------------
void CPostcardOperationOpen::DoHandleTextL( )
    {
/*    if ( iText != 0 )
        { // Lets externalize the text itself
        RFile file = iManager->GetAttachmentFileL( iText );
        
		CMmsTextInfo* info = static_cast<CMmsTextInfo*>(iDocument.MediaResolver()->ResolveFileTransferInfoL( file ));
		CleanupStack::PushL( info );
        TUint attaCharset = info->CharacterSet();
        CleanupStack::PopAndDestroy( info );
        RFileReadStream input_stream( file );

        CPlainText::TImportExportParam param;
        param.iForeignEncoding = attaCharset; //charconvCharsetID;
        param.iOrganisation = CPlainText::EOrganiseByParagraph; 
        CPlainText::TImportExportResult result;
        TRAPD( error, text->ImportTextL( 0, input_stream, param, result) );
        input_stream.Close( );
        file.Close( );
        }
*/

    TPostcardMsgType type = iDocument.MessageType( );
    if( iText > 0 && type == EPostcardSendAs )
        { // If type is SendAs and file is from Phonebook -> Handle this case first
        CMsvAttachment* atta = iManager->GetAttachmentInfoL( iText );
        CleanupStack::PushL( atta );
        if( atta->AttachmentName( ).Find( _L("X-Nokia-PhonebookId_") ) != -1  )
            {
            // All right, it was from Phonebook
            RFile file = iManager->GetAttachmentFileL( iText );
            CleanupClosePushL( file );

            CPostcardContact* contact = CPostcardContact::NewL( iFs );
            CleanupStack::PushL( contact );
            contact->FetchContactL( file );
            DoHandlePhonebookSendAsL( *contact );
            CleanupStack::PopAndDestroy( 3, atta ); // contact, file, atta

			ToState( EPostcardOpenFinished );
			return;                     
            }
        // Ok, it was not from Phonebook, so lets continue normally..
        CleanupStack::PopAndDestroy( atta );
        }

    CPlainText* text = CPlainText::NewL( CEditableText::ESegmentedStorage, KPostcardDefaultGreetingSegment );
    CleanupStack::PushL( text );

    if ( iText > 0 )
        { // Lets externalize the text itself
        RFile file = iManager->GetAttachmentFileL( iText );
        CleanupClosePushL( file );

        CMsvAttachment* atta = iManager->GetAttachmentInfoL( iText );
        CleanupStack::PushL( atta );
        
        CMsvMimeHeaders* msvMime = CMsvMimeHeaders::NewLC();
        msvMime->RestoreL( *atta );
        
        TUint attaCharset = msvMime->MimeCharset( ); // only charset needed
        
        CleanupStack::PopAndDestroy( 2, atta ); // mime, atta

        if( attaCharset == 0 )
            {
            //assume US-ASCII - mandated by RFC 2046
            attaCharset = KMmsUsAscii;
            }
  
        TUint finalCharset = iDocument.TextUtils()->MibIdToCharconvIdL( attaCharset );
          
        RFileReadStream input_stream( file );

        CPlainText::TImportExportParam param;
        param.iForeignEncoding = finalCharset;
        param.iOrganisation = CPlainText::EOrganiseByParagraph; 
        CPlainText::TImportExportResult result;
        TRAP_IGNORE( text->ImportTextL( 0, input_stream, param, result) );
        input_stream.Close( );
        CleanupStack::PopAndDestroy( &file ); // file
        }

    if( type == EPostcardSendAs )
        {
        // Otherwise there's no text or text is just text
	    CleanupStack::Pop( text ); 
	    iAppUi.SetTextL( *text ); // AppUI takes the ownership        		
		ToState( EPostcardOpenHandleRecipient );
	    return;
        }
	else if( 	type == EPostcardForward || 
        		type == EPostcardEdit || 
        		type == EPostcardSent ||
        		type == EPostcardDraft )
        {
        // The recipient might be added into the greeting text only when the entry
        // is opened from Sent folder or is forwarded or edited
        if( iSpecialFormat.Length( ) > 0 && text->DocumentLength( ) > 0 && iRecipient == 0 )
			{ // if special send format is defined and there's no separate recipient attachment
		    TPtrC16 textBuf ( text->Read( 0 ) );
            if( textBuf.Find( iSeparatorChar ) != KErrNotFound )
                { 	// There are separators -> this msg has been sent
					// lets dig the recipient info from the text
			    DoHandleSpecialFormatOpenL( *text );
				CleanupStack::PopAndDestroy( text );
    			ToState( EPostcardOpenFinished );
				return;         
                }
            else
                {
                // There are no separators -> we can normally continue
                // and use "text" as the real greeting text
                }
			}
        }
    CleanupStack::Pop( text ); 
    iAppUi.SetTextL( *text ); // AppUI takes the ownership
    ToState( EPostcardOpenHandleRecipient );
    }

// ---------------------------------------------------------
// DoHandleRecipientL
// ---------------------------------------------------------
void CPostcardOperationOpen::DoHandleRecipientL( )
    {
    CContactCard* card = CContactCard::NewLC();

    if( iRecipient != 0 )
        {
        TPostcardMsgType type = iDocument.MessageType( );

        if( type == EPostcardForward )
            { // We are opening a forward entry -> delete VCard
            TInt oldImage = TMsvIdToIndexL( iRecipient );
            iStore->AttachmentManagerExtensionsL( ).RemoveAttachmentL( oldImage );
            iCommit = ETrue;    		
            }
        else
            {
            ParseRecipientL( *card );
            }
        }
    CleanupStack::Pop( card );
	iAppUi.SetRecipientL( *card ); //ownership moves..    the function does not leave before card is set as a member
    }

// ---------------------------------------------------------
// CPostcardOperationOpen::ParseRecipientL
// ---------------------------------------------------------
void CPostcardOperationOpen::ParseRecipientL( CContactCard& aContactCard )
    {
    RFile file = iManager->GetAttachmentFileL( iRecipient );
    CleanupClosePushL( file );

    CParserVCard* parser = CParserVCard::NewL();
    CleanupStack::PushL(parser);

    RFileReadStream vCardStream( file );
    vCardStream.PushL();
    MStreamBuf* buf = vCardStream.Source();
    if( !buf )
        {
        User::Leave( KErrGeneral );
        }

    parser->InternalizeL(vCardStream);

    vCardStream.Close();
    vCardStream.Pop();

    // Does not take ownership
    CArrayPtr<CParserProperty>* arr = parser->ArrayOfProperties(EFalse);
    for( TInt a = 0; a < arr->Count( ); a++ )
        {
        CParserProperty* prop = arr->At( a );
        TPtrC8 propName = prop->Name();
        CParserPropertyValue *propValue = prop->Value();
        if( propValue )
            {
            if( propValue->Uid().iUid == KVersitPropertyCDesCArrayUid )
                {
                CParserPropertyValueCDesCArray* arrayProperty = 
			        reinterpret_cast<CParserPropertyValueCDesCArray*>( propValue );
                CDesCArray* values = arrayProperty->Value();
                if( propName.Compare( KPostcardVCardName ) == 0 )
                    {
                    TPostcardUtils::AddContactFieldL( aContactCard,
                        TPostcardUtils::ContactItemNameFromId( EPostcardAddressName ),
                        values->MdcaPoint( 0 ) );
                    }
                else if( propName.Compare( KPostcardVCardAddr ) == 0 )
                    {
                    // This is the order items are read from the VCard store
                    static const TInt indexToId[] =
                        {
                        0, // PO Box in the array will be ignored
                        EPostcardAddressInfo, EPostcardAddressStreet,
                        EPostcardAddressCity, EPostcardAddressState,
                        EPostcardAddressZip, EPostcardAddressCountry
                        };
                    const TInt KNumIds =
                        sizeof( indexToId ) / sizeof( indexToId[0] );
                    TPostcardMsgType type = iDocument.MessageType();
                    // Skip PO Box at beginning of the array
                    for (TInt i = 1; i < values->Count() && i < KNumIds; i++)
                        {
                        TPtrC value = values->MdcaPoint(i);
                        // If viewing a sent postcard, all available fields are
                        // initialized. Otherwise the field is initialized if
                        // it's visible in address dialog (max. length is not zero).
                        if( value.Length() &&
                          ( type == EPostcardSent || iAppUi.MaxTextLength( indexToId[i] ) ) )
                            {
                            TPostcardUtils::AddContactFieldL( aContactCard,
                                TPostcardUtils::ContactItemNameFromId( indexToId[i] ),
                                value );
                            }
                        }
                    }
                }
            else if( propValue->Uid().iUid == KVersitPropertyHBufCUid )
                {
                // Postcard before S60 3.2 release used FN (full name) property.
                // Since 3.2, N property is used
                if( propName.Compare( KPostcardVCardNameFN ) == 0 )
                    {
                    TPostcardUtils::AddContactFieldL( aContactCard,
                        TPostcardUtils::ContactItemNameFromId( EPostcardAddressName ),
                        static_cast<CParserPropertyValueHBufC*>( propValue )->Value() );
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy( 2, &file ); // file, parser
    }

// ---------------------------------------------------------
// CPostcardOperationOpen::ConstructL
// ---------------------------------------------------------
void CPostcardOperationOpen::ConstructL( )
    {
    }

// ---------------------------------------------------------
// CPostcardOperationOpen::DoCancel
// ---------------------------------------------------------
void CPostcardOperationOpen::DoCancel( )
    {
    if( iManager )
        {
        if( iFlags & EPostcardOpenRequestActive )
            {
            iManager->CancelRequest();        
            }
        }
    }

// ---------------------------------------------------------
// CPostcardOperationOpen::RunL
// ---------------------------------------------------------
void CPostcardOperationOpen::RunL( )
    {
    DoLaunchStepL( );
    }

// ---------------------------------------------------------
// RunError
// ---------------------------------------------------------
TInt CPostcardOperationOpen::RunError( TInt aError )
    {
    iLaunchState = -1;
    SetError( aError );
    iObserver.PostcardOperationEvent(
        EPostcardOperationOpen,
        EPostcardOperationError );     
    return KErrNone;
    }

// ---------------------------------------------------------
// CPostcardOperationOpen::DoHandlePhonebookSendAsL
// ---------------------------------------------------------
void CPostcardOperationOpen::DoHandlePhonebookSendAsL( CPostcardContact& aContact )
    {
    CPostcardRecipientWrapper* recWrapper = 
        CPostcardRecipientWrapper::NewL( iDocument, iAppUi, aContact );
    CleanupStack::PushL( recWrapper );
    
    // First remove wait note so that the dialog can be shown
    iAppUi.RemoveWaitNote( );

    // Location asking is done in recipientWrapper
    CPostcardContact::TLocation location;
    TBool hasLocation = recWrapper->AskLocationL( location );
    CleanupStack::PopAndDestroy( recWrapper );

    iAppUi.ShowWaitNoteL( R_POSTCARD_WAIT_OPENING, EFalse );
        
    if ( !hasLocation )
        {
        // Contact not found or cancel selected..
        CContactCard* card = CContactCard::NewL();
        // Ownership moves..
        // The function does not leave before card is set as a member
	    iAppUi.SetRecipientL( *card );
        CPlainText* emptyText = CPlainText::NewL(
            CEditableText::ESegmentedStorage, KPostcardDefaultGreetingSegment );
        // Ownership moves..
        // The function does not leave before card is set as a member
    	iAppUi.SetTextL( *emptyText );
        }
    else
        {
        // Otherwise location now tells the chosen location

        CContactCard* card = CContactCard::NewLC();
        UpdateContactCardL( *card, aContact, location );

        CleanupStack::Pop( card ); // card
        // Ownership moves..
        // The function does not leave before card is set as a member
	    iAppUi.SetRecipientL( *card );
	    iDocument.SetChanged( EPostcardRecipient );
	    iCommit = ETrue; // Remove the original atta way as it's just a phonebook atta
        MMsvAttachmentManagerSync& managerSync = iStore->AttachmentManagerExtensionsL();
	    TInt textIndex = TMsvIdToIndexL( iText );
        managerSync.RemoveAttachmentL( textIndex );
        iText = 0;
        CPlainText* emptyText = CPlainText::NewL(
            CEditableText::ESegmentedStorage, KPostcardDefaultGreetingSegment );
        // Ownership moves..
        // The function does not leave before card is set as a member
	    iAppUi.SetTextL( *emptyText );
        }
    }

// ---------------------------------------------------------
// CPostcardOperationOpen::UpdateContactCardL
// ---------------------------------------------------------
void CPostcardOperationOpen::UpdateContactCardL( CContactCard& aCard,
    CPostcardContact& aContact, CPostcardContact::TLocation aLocation )
    {

    // Get contact name, combined last and first name
    HBufC* contactName = aContact.GetNameLC();
    if ( contactName->Length() )
        {
        // Create a new CContactCard field
        AddContactCardFieldL( aCard, *contactName, EPostcardAddressName );
        }
   CleanupStack::PopAndDestroy( contactName ); // contactName

    // Handle rest of the address fields
    for( TInt i = EPostcardAddressInfo; i <= EPostcardAddressCountry; i++)
        {
        // Initialize address field only if it's visible to user
        // (max. length is not zero)
        if ( iAppUi.MaxTextLength( i ) )
            {
            HBufC* fieldText = aContact.GetAddressFieldLC( aLocation,
                CPostcardContact::ControlIdToAddrField( i ) );
            if ( fieldText->Length() )
                {
                // Create a new CContactCard field
                AddContactCardFieldL( aCard, *fieldText, i );
                }
            CleanupStack::PopAndDestroy( fieldText ); // fieldText
            }
        }
    }

// ---------------------------------------------------------
// CPostcardOperationOpen::AddContactCardFieldL
// ---------------------------------------------------------
void CPostcardOperationOpen::AddContactCardFieldL( CContactCard& aCard,
    const TPtrC& aText, TInt aFieldSelect )
    {
    TPostcardUtils::AddContactFieldL(aCard,
        TPostcardUtils::ContactItemNameFromId( aFieldSelect ),
        aText);
    }

// ---------------------------------------------------------
// CPostcardOperationOpen::ParseSpecialFormatTextL
// ---------------------------------------------------------
void CPostcardOperationOpen::ParseSpecialFormatTextL( CPlainText& aText,
    CContactCard& aCard, CPlainText& aGreeting )
    {
    TPtrC format( iSpecialFormat );
    TPtrC srcText( aText.Read( 0 , aText.DocumentLength() ) );

    const TInt KTagLen = 3;

    // Find tags from format and parse source text accordingly
    TInt tag;
    TInt position = TPostcardUtils::NextTag( format, tag );
    // Eat prefix and tag from format. Assume the format has at least one tag.
    format.Set( format.Right( format.Length() - position - KTagLen ) );
    // Eat prefix from source text
    srcText.Set( srcText.Right( srcText.Length() - position ) );
    while( position != KErrNotFound )
        {
        TInt newTag;
        position = TPostcardUtils::NextTag( format, newTag );
        TPtrC text;
        if ( position != KErrNotFound )
            {
            // Separator string from previous tag to this one. This cannot
            // be empty, otherwise text fields cannot be separated from each other.
            TPtrC separator( format.Left( position ) );
            TInt separatorPos = srcText.Find( separator );
            TInt srcLen = srcText.Length() - position - separatorPos;
            if ( separatorPos == KErrNotFound || srcLen < 0 )
                {
                // Unexpected. Cannot parse.
                return;
                }
            text.Set( srcText.Left( separatorPos ) );
            // Eat separator and tag from format
            format.Set( format.Right( format.Length() - position - KTagLen ) );
            // Eat text and separator from source
            srcText.Set( srcText.Right( srcLen ) );
            }
        else
            {
            // No new tags. Text is source string minus postfix
            text.Set( srcText.Left( srcText.Length() - format.Length() ) );
            }
        if ( text.Length() )
            {
            if ( tag == 0 )
                {
                aGreeting.InsertL( 0, text );
                }
            else
                {
                if ( iDocument.MessageType() != EPostcardForward )
                    { // Add the fields only when not in Forward mode
                    TPostcardUtils::AddContactFieldL(aCard,
                        TPostcardUtils::ContactItemNameFromTag( tag ),
                        text);
                    }
                }
            }
        tag = newTag;
        }
    }

// ---------------------------------------------------------
// CPostcardOperationOpen::DoHandleSpecialFormatOpenL
// ---------------------------------------------------------
void CPostcardOperationOpen::DoHandleSpecialFormatOpenL( CPlainText& aText )
    {
    CContactCard* newCard = CContactCard::NewLC(); // New ContactCard
    CPlainText* newText = CPlainText::NewL( CEditableText::ESegmentedStorage, KPostcardDefaultGreetingSegment ); // New PlainText for greeting text
    CleanupStack::PushL( newText );

    ParseSpecialFormatTextL( aText, *newCard, *newText );

    CleanupStack::Pop(); // newText
    iAppUi.SetTextL( *newText ); // AppUI takes the ownership
    CleanupStack::Pop(); // newCard
    iAppUi.SetRecipientL( *newCard );
    }

// ---------------------------------------------------------
// CPostcardOperationOpen::ToState
// ---------------------------------------------------------
void CPostcardOperationOpen::ToState( TPostcardOpenState aState )
    {
    iLaunchState = aState;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CPostcardOperationOpen::ToErrorState
// ---------------------------------------------------------
void CPostcardOperationOpen::ToErrorState( TInt aResourceId )
    {
    SetError( aResourceId );
    iLaunchState = EPostcardOpenError;
    CompleteSelf( KErrCancel );
    }

// EOF
