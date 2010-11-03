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
*       CPostcardOperationSave
*
*/



// ========== INCLUDE FILES ================================

#include <badesca.h>
#include <AknsConstants.h>
#include <data_caging_path_literals.hrh> 
#include <apmstd.h>  // TDataType
#include <mmsvattachmentmanager.h>	// AttachmentManager
#include <mmsvattachmentmanagersync.h>	// AttachmentManager
#include <cmsvmimeheaders.h>		// MimeHeaders

#include <StringLoader.h>			// For StringLoader

#include <mmsclient.h>
#include <mmsconst.h>
#include <mmsmsventry.h>

#include <MsgMimeTypes.h>

#include <vprop.h>	// VCardPropertyValue
#include <cntitem.h>
#include <vcard.h>
#include <cntfldst.h> // CContactTextField

#include <Postcard.rsg>
#include "PostcardOperationSave.h"
#include "PostcardUtils.h"
#include "Postcard.hrh"
#include "PostcardAddressForm.h"
#include "PostcardPrivateCRKeys.h"  // cenrep keys
#include "PostcardCenRep.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

_LIT8( KReplaceChar8, "_" );
_LIT8( KPostcardVCardName, "N"); // This was "FN" in 3.0-3.1 but OMA says it should be "N"
_LIT8( KPostcardVCardAddr, "ADR");
_LIT8( KPostcardGreetingXParameter, "X-MMS-GREETING-TEXT" );

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// NewL
// ---------------------------------------------------------
CPostcardOperationSave* CPostcardOperationSave::NewL(
            MPostcardOperationObserver& aObserver,
            CPostcardDocument& aDocument,
            CPostcardAppUi& aAppUi,
            RFs& aFs,
            CContactCard& aContact,
            CPlainText& aPlainText )
    {
    CPostcardOperationSave* self = 
        new ( ELeave ) CPostcardOperationSave( aObserver, aDocument, aAppUi, aFs, aContact, aPlainText );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CPostcardOperationSave
// ---------------------------------------------------------
CPostcardOperationSave::CPostcardOperationSave(
            MPostcardOperationObserver& aObserver,
            CPostcardDocument& aDocument,
            CPostcardAppUi& aAppUi,
            RFs& aFs,
			CContactCard& aContact,
			CPlainText& aPlainText ) :
    CPostcardOperation( aObserver, aDocument, aAppUi, aFs ),
    iContact( aContact ),
    iPlainText( aPlainText )
    {
    }

// ---------------------------------------------------------
// CPostcardOperationSave
// ---------------------------------------------------------
CPostcardOperationSave::~CPostcardOperationSave( )
    {
    Cancel( );
    if( iTempFile )
        {
        iTempFile->Close( );
        delete iTempFile;
        }
    delete iSendOperation;
    delete iCharConv;
    delete iRecipientName;
    }

// ---------------------------------------------------------
// Start
// ---------------------------------------------------------
void CPostcardOperationSave::Start( TInt aArgument )
    {
    if( aArgument )
    	{
    	iFlags |= EPostcardSaveSend;
    	}
    if( iAppUi.CenRep().GetString( KPocaKeyServiceSpecialFormat ).Length() > 0 &&
        ( iFlags & EPostcardSaveSend ) )
        { // if special send format is defined and we are sending -> take it into use
        iFlags |= EPostcardSaveSpecialFormat;
        }
    iLaunchState = EPostcardSaveInitializing;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// DoLaunchStepL
// ---------------------------------------------------------
void CPostcardOperationSave::DoLaunchStepL( )
    {
    iFlags &= ~EPostcardSaveRequestActive;
    switch ( iLaunchState )
        {        
    	case EPostcardSaveInitializing:
    		{
    		InitL( );			
            iLaunchState = EPostcardSaveCheckAttas;
            CompleteSelf( KErrNone );
            break;    			
    		}
        case EPostcardSaveCheckAttas:
            {
            if( !CheckAttasL( ) )
            	{ // If there was an error, this has been completed already
	            iLaunchState = EPostcardSaveRemoveOldAttas; 
	            CompleteSelf( KErrNone );            		
            	}
            break;
            }
        case EPostcardSaveRemoveOldAttas:
            {
            RemoveOldAttasL( );
            // Let's see the next phase..
            if( iPlainText.DocumentLength( ) > 0 && iDocument.Changed( EPostcardText ) )
                { // We have text and it has changed -> so save it..
                iLaunchState = EPostcardSaveCreateTextAtta;
                }
            else if ( ( iFlags & EPostcardSaveSend ) &&
                      ( iFlags & EPostcardSaveSpecialFormat ) )
                { // Text has not changed, but we have special format on -> so create text atta anyway
                iLaunchState = EPostcardSaveCreateTextAtta;                
                }
            else if ( iDocument.Changed( EPostcardRecipient ) )
                { // We are not sending special format, but the recipient has changed
                iLaunchState = EPostcardSaveCreateRecipientAtta;
                }
            else if(   !iSmil && 
                    (  iFlags & EPostcardSaveSend ) &&
                    !( iFlags & EPostcardSaveSpecialFormat ) )
                { // Text and recipient have not changed, but we are sending OMA Postcard and there's no SMIL yet
                iLaunchState = EPostcardSaveCreateSmilAtta;
                }
            else
                {
                iLaunchState = EPostcardSaveUninit;
                }
            CompleteSelf( KErrNone );
            break;
            }
        case EPostcardSaveCreateTextAtta:
            {
            CreateNewTextAttaL( );
            iLaunchState = EPostcardSaveSaveTextAtta;
		    SetActive();    
            break;
            }
        case EPostcardSaveSaveTextAtta:
            {
            HandleTextL( );
            iLaunchState = EPostcardSaveFinalizeTextAtta;
            CompleteSelf( KErrNone );
            break;
            }
        case EPostcardSaveFinalizeTextAtta:
            {
            FinalizeTextL( );
            if ( iDocument.Changed( EPostcardRecipient ) &&
                ( iFlags & EPostcardSaveHasContact ) )
                { // We are not sending special format, but the recipient has changed
                iLaunchState = EPostcardSaveCreateRecipientAtta;
                }
            else if(   !iSmil && 
                    (  iFlags & EPostcardSaveSend ) &&
                    !( iFlags & EPostcardSaveSpecialFormat ) )
                { // Recipient has not changed, but we are sending OMA Postcard and there's no SMIL yet
                iLaunchState = EPostcardSaveCreateSmilAtta;
                }
            else
                {
                iLaunchState = EPostcardSaveUninit;
                }
            CompleteSelf( KErrNone );
            break;
            }
        case EPostcardSaveCreateRecipientAtta:
            {
            CreateNewRecipientAttaL( );
            iLaunchState = EPostcardSaveSaveRecipientAtta;
		    SetActive( );        	
            break;
            }
        case EPostcardSaveSaveRecipientAtta:
            {
            HandleRecipientL( );
            iLaunchState = EPostcardSaveFinalizeRecipientAtta;
            CompleteSelf( KErrNone );
            break;
            }
        case EPostcardSaveFinalizeRecipientAtta:
            {
            FinalizeRecipientL( );
            if( !iSmil && 
                 ( iFlags & EPostcardSaveSend ) &&
                !( iFlags & EPostcardSaveSpecialFormat ) )
                { // Smil is created only to confront OMA Postcard spec
                iLaunchState = EPostcardSaveCreateSmilAtta;
                }
            else
                {
                // Smil seems to exists already or we are not sending
                iLaunchState = EPostcardSaveUninit;                
                }
            CompleteSelf( KErrNone );
            break;
            }
        case EPostcardSaveCreateSmilAtta:
            {
            CreateNewSmilAttaL( );
            iLaunchState = EPostcardSaveSaveSmilAtta;
		    SetActive( );        	
            break;
            }
        case EPostcardSaveSaveSmilAtta:
            {
            HandleSmilL( );
            iLaunchState = EPostcardFinalizeSmilAtta;
            CompleteSelf( KErrNone );
            break;
            }
        case EPostcardFinalizeSmilAtta:
            {
            FinalizeSmilL( );
            iLaunchState = EPostcardSaveUninit;            
            CompleteSelf( KErrNone );
            break;
            }
        case EPostcardSaveUninit:
            {
            UnInitL( );
            iLaunchState = EPostcardSaveFinalizeMessage;
            CompleteSelf( KErrNone );
            break;
            }
        case EPostcardSaveFinalizeMessage:
            {
            FinalizeMessageL( );
            if( iFlags & EPostcardSaveSend )
            	{
	            iLaunchState = EPostcardSaveStartSend;        	            		
            	}
            else
            	{
	            iLaunchState = EPostcardSaveFinished;        	
            	}
            CompleteSelf( KErrNone );
            break;
            }
        case EPostcardSaveStartSend:
            {
            StartSendingL( );
            iLaunchState = EPostcardSaveFinished;        	
		    SetActive();    
            break;
            }
        case EPostcardSaveFinished:
            {
            iObserver.PostcardOperationEvent(
                EPostcardOperationSave,
                EPostcardOperationComplete ); 
            break;
            }
        default:
            iObserver.PostcardOperationEvent(
                EPostcardOperationSave,
                EPostcardOperationError ); 
            break;
        }
    }

// ---------------------------------------------------------
// RemoveOldAttasL
// ---------------------------------------------------------
void CPostcardOperationSave::RemoveOldAttasL( )
	{
	if( 	iDocument.Changed( EPostcardText ) || 
		( ( iFlags & EPostcardSaveSend ) && ( iFlags & EPostcardSaveSpecialFormat ) ) )
		{
		if( iText )
			{ // Remove old
			TInt old = TMsvIdToIndexL( iText );
			if( old != KErrNotFound )
			    {
			    // If there is an old text atta and a no new text atta, we need to
			    // ask commit here, otherwise old atta would be left with the message.
			    iCommit = ETrue;
    			iStore->AttachmentManagerExtensionsL( ).RemoveAttachmentL( old );			    
			    }
			}		
		}
	if( 	iDocument.Changed( EPostcardRecipient ) || 
		( ( iFlags & EPostcardSaveSend ) && ( iFlags & EPostcardSaveSpecialFormat ) ) )
		{
		if( iRecipient )
			{ // Remove old
			TInt old = TMsvIdToIndexL( iRecipient );
			if( old != KErrNotFound )
			    {
    			iStore->AttachmentManagerExtensionsL( ).RemoveAttachmentL( old );
			    }
			}		
		}	
	}
	
// ---------------------------------------------------------
// CreateNewTextAttaL
// ---------------------------------------------------------
void CPostcardOperationSave::CreateNewTextAttaL( )
	{
    // Now lets create a new attachment
    TFileName file( _L("greeting.txt") );
    StartCreatingEmptyAttachmentL( file );
	}
	
// ---------------------------------------------------------
// HandleTextL
// ---------------------------------------------------------
void CPostcardOperationSave::HandleTextL( )
	{
    if( !iCharConv )
        {
        iCharConv = CCnvCharacterSetConverter::NewL( );
        }
		
    if ( iCharConv->PrepareToConvertToOrFromL( KCharacterSetIdentifierUtf8, iFs )
        != CCnvCharacterSetConverter::EAvailable )
        {
        User::Leave( KErrNotSupported );
        }
	
    // Delete possible old headers
    delete iHeaders;
    iHeaders = NULL;
    iHeaders = CMsvMimeHeaders::NewL( );

    if( ( iFlags & EPostcardSaveSend ) && !( iFlags & EPostcardSaveSpecialFormat ) )
        {
        iHeaders->XTypeParams().AppendL( KPostcardGreetingXParameter );
        iHeaders->XTypeParams().AppendL( TPtrC8() );	    
        }

    _LIT(KFileName, "text.txt");
    iHeaders->SetContentLocationL( KFileName );
    iHeaders->SetMimeCharset( KMmsUtf8 );

    TInt slash = KMsgMimeTextPlain().Locate( '/' );
    iHeaders->SetContentTypeL( KMsgMimeTextPlain().Left( slash ) );
    iHeaders->SetContentSubTypeL( KMsgMimeTextPlain().Mid( slash + 1 ) );

    iTempAtta->SetMimeTypeL( KMsgMimeTextPlain );

    iText = iTempAtta->Id( );
    RFileWriteStream writer( *iTempFile );
    writer.PushL( );
    delete iTempFile; // not close as write does it..
    iTempFile = NULL;
    
    TPtrC16 remainderOfUnicodeText;
    TInt numCleanupItems = 0;
    
    if( (iFlags & EPostcardSaveSend) && (iFlags & EPostcardSaveSpecialFormat ) )
        {
        // If we are here -> we are sending so we must have iContact OK
        HBufC* wholeText = MakeSpecialFormatTextL();
        CleanupStack::PushL( wholeText );
        numCleanupItems++;

        remainderOfUnicodeText.Set( wholeText->Des( ) );	
        
        // Now we remove the HasContact flag so that the recipient is not stored separately
		iFlags &=  ~EPostcardSaveHasContact;
		}
    else
        {
        remainderOfUnicodeText.Set(
            iPlainText.Read( 0, iPlainText.DocumentLength() ) );
        }

    // Now remainOfUnicodeText has the stuff needed to store into the greeting text
    TBuf8<128> outputBuffer;
    TInt returnValue;
    do
        {
        returnValue = iCharConv->ConvertFromUnicode(
            outputBuffer, remainderOfUnicodeText );
        if ( returnValue < 0 )
            {
            User::Leave(
                returnValue == CCnvCharacterSetConverter::EErrorIllFormedInput ?
                KErrCorrupt:KErrGeneral );
            }
        writer.WriteL( outputBuffer );
        remainderOfUnicodeText.Set( remainderOfUnicodeText.Right(
            returnValue ) );
        }
    while( returnValue );

    CleanupStack::PopAndDestroy( numCleanupItems );
    writer.Pop( );
    writer.Close( );	
	}

// ---------------------------------------------------------
// MakeSpecialFormatTextL
// ---------------------------------------------------------
HBufC* CPostcardOperationSave::MakeSpecialFormatTextL()
    {
    CContactItemFieldSet& set = iContact.CardFields( );
    // Organise the address into an array
    CDesCArrayFlat* desarr = AddressArrayLC( set );

    const TDesC& replacement =
        iAppUi.CenRep().GetString( KPocaKeyServiceReplaceString );
    const TDesC& separator =
        iAppUi.CenRep().GetString( KPocaKeyServiceSeparator );

    // Add the greeting text.
    
    TInt len = iPlainText.DocumentLength();
    HBufC* buf = HBufC::NewLC( len );
    *buf = iPlainText.Read( 0, len );

    // Replace illegal chars
    TPtr des = buf->Des();
    ReplaceWith( des, separator, replacement );
    // And add this to the desarr
    desarr->InsertL( 0, *buf );

    // Read the format string from cenrep
    const TDesC& specialFormat =
        iAppUi.CenRep().GetString( KPocaKeyServiceSpecialFormat );

    // Construct the text to be sent. Room for each tag, strings between
    // tags and prefix and postfix.
    const TInt KMaxItems = TPostcardUtils::ENumSpecFormatTags * 2 + 1;
    TPtrC textItems[KMaxItems];

    TPtrC format(specialFormat);
    TInt position = 0;
    TInt numItems = 0;
    while(numItems < KMaxItems - 1 && position != KErrNotFound)
        {
        TInt tag;
        position = TPostcardUtils::NextTag(format, tag);
        if ( position != KErrNotFound )
            {
            textItems[numItems++].Set( format.Left( position ) );
            textItems[numItems++].Set( desarr->MdcaPoint( tag ) );
            format.Set( format.Right( format.Length() - position - 3 ) );
            }
        else
            {
            textItems[numItems++].Set( format );
            }
        }
        
    // Calculate buffer length needed to fit the whole text
    TInt buffSize = 0;
    for( TInt i = 0; i < numItems; i++ )
        {
        buffSize += textItems[i].Length();
        }

    // Copy text into buffer
    HBufC* dstBuf = HBufC::NewL( buffSize );
    TPtr dst = dstBuf->Des();
    for( TInt i = 0; i < numItems; i++ )
        {
        dst.Append( textItems[i] );
        }

    CleanupStack::PopAndDestroy( 2 ); // buff, desarr

    return dstBuf;
    }

// ---------------------------------------------------------
// FinalizeTextL
// ---------------------------------------------------------
void CPostcardOperationSave::FinalizeTextL( )
	{
	iHeaders->StoreL( *iTempAtta );
	}
	
// ---------------------------------------------------------
// CreateNewRecipientAttaL
// ---------------------------------------------------------
void CPostcardOperationSave::CreateNewRecipientAttaL( )
	{
    // Now lets create a new attachment
    TFileName file( _L("recipient.vcf") );
    StartCreatingEmptyAttachmentL( file );
	}
	
// ---------------------------------------------------------
// HandleRecipientL
// ---------------------------------------------------------
void CPostcardOperationSave::HandleRecipientL( )
	{
	if( !iCharConv )
		{
	    iCharConv = CCnvCharacterSetConverter::NewL( );
		}
    if ( iCharConv->PrepareToConvertToOrFromL( KCharacterSetIdentifierAscii, iFs ) !=
        CCnvCharacterSetConverter::EAvailable )
        {
        User::Leave( KErrNotSupported );
        }

	// Delete possible old headers
	delete iHeaders;
	iHeaders = NULL;
	iHeaders = CMsvMimeHeaders::NewL( );
   	iHeaders->SetContentLocationL( _L("recipient.vcf") );
	
	TInt slash = KMsgMimeVCard().Locate( '/' );
    iHeaders->SetContentTypeL( KMsgMimeVCard().Left( slash ) );
    iHeaders->SetContentSubTypeL( KMsgMimeVCard().Mid( slash + 1 ) );
	
	iTempAtta->SetMimeTypeL( KMsgMimeVCard );

    iRecipient = iTempAtta->Id( );    
        
    CParserVCard* parser = CParserVCard::NewL();
    CleanupStack::PushL(parser);

    RFileWriteStream vCardStream( *iTempFile );
    vCardStream.PushL( );    
    delete iTempFile; // not close as VCardStream does it..
    iTempFile = NULL;

	iCharConv->SetReplacementForUnconvertibleUnicodeCharactersL( KReplaceChar8 );
	
	CContactItemFieldSet& set = iContact.CardFields( );

    const CContactItemField* field = TPostcardUtils::FieldOrNull( set,
        TPostcardUtils::ContactItemNameFromId( EPostcardAddressName ) );

    if( field )
        {
		CParserPropertyValueHBufC* val =
		    CParserPropertyValueHBufC::NewL( field->TextStorage()->Text() );
		CleanupStack::PushL( val );
		CArrayPtrFlat<CParserParam>* array = new(ELeave)CArrayPtrFlat<CParserParam>(1);
		CleanupStack::PushL( array );
		CParserProperty* property = CParserProperty::NewL( *val, KPostcardVCardName, array);
		CleanupStack::PushL( property );
		parser->AddPropertyL( property );
		CleanupStack::Pop( 3, val );				
		}

	CDesCArrayFlat* desarr = VCardAddressArrayL( set ); // Organise the address into an array
	CleanupStack::PushL( desarr );
	// AddressArrayL does not add empty "PO Box" descriptor into the desarr
	desarr->InsertL( 0, KNullDesC );

    CParserPropertyValue* val = new (ELeave)CParserPropertyValueCDesCArray(desarr);
    CleanupStack::Pop(desarr);
	CleanupStack::PushL( val );
	CArrayPtrFlat<CParserParam>* array = new(ELeave)CArrayPtrFlat<CParserParam>(1);
	CleanupStack::PushL( array );

	CParserProperty* property = CParserProperty::NewL( *val, KPostcardVCardAddr, array);
	CleanupStack::PushL( property );
	parser->AddPropertyL( property );
	CleanupStack::Pop( 3, val );				
        
    parser->ExternalizeL(vCardStream);

    vCardStream.Pop( );
    vCardStream.Close( );

    CleanupStack::PopAndDestroy(parser);
	}
	
// ---------------------------------------------------------
// FinalizeRecipientL
// ---------------------------------------------------------
void CPostcardOperationSave::FinalizeRecipientL( )
	{
	iHeaders->StoreL( *iTempAtta );	
	}

// ---------------------------------------------------------
// CreateNewSmilAttaL
// ---------------------------------------------------------
void CPostcardOperationSave::CreateNewSmilAttaL( )
    {
    // Now lets create a new attachment
    TFileName file( _L("postcard.smil") );
    StartCreatingEmptyAttachmentL( file );
    }
    
// ---------------------------------------------------------
// HandleSmilL
// ---------------------------------------------------------
void CPostcardOperationSave::HandleSmilL( )
    {
	// Delete possible old headers
	delete iHeaders;
	iHeaders = NULL;
	
	iHeaders = CMsvMimeHeaders::NewL( );
   	iHeaders->SetContentLocationL( _L("pres.smil") );
	
	TInt slash = KMsgMimeSmil().Locate( '/' );
    iHeaders->SetContentTypeL( KMsgMimeSmil().Left( slash ) );
    iHeaders->SetContentSubTypeL( KMsgMimeSmil().Mid( slash + 1 ) );
    
	iTempAtta->SetMimeTypeL( KMsgMimeSmil );

    iSmil = iTempAtta->Id( );
    
    // Get the name of the image file to be inserted into the SMIL file
    HBufC* fileName = GetImageFileNameL( );
    CleanupStack::PushL( fileName );
	TInt resId = R_POSTCARD_OMA_SMIL;
	HBufC* smil = StringLoader::LoadL( resId, *fileName );
    CleanupStack::PopAndDestroy( fileName );
    CleanupStack::PushL( smil );

    // convert the smil to 8-bit
    HBufC8* smil8 = HBufC8::NewLC( smil->Des( ).Length( ) );
    smil8->Des().Copy( smil->Des( ) );

    RFileWriteStream smilStream( *iTempFile );
    smilStream.PushL( );    
    
    delete iTempFile; // not close as smilStream does it
    iTempFile = NULL;

    smilStream.WriteL( *smil8 );

    smilStream.Close( );
    smilStream.Pop( );
    CleanupStack::PopAndDestroy( 2, smil ); // smil, smil8
    }

// ---------------------------------------------------------
// FinalizeSmilL
// ---------------------------------------------------------
void CPostcardOperationSave::FinalizeSmilL( )
    {
	iHeaders->StoreL( *iTempAtta );
    }
	
// ---------------------------------------------------------
// FinalizeMessageL
// ---------------------------------------------------------
void CPostcardOperationSave::FinalizeMessageL( )
	{
	
    const CMsvRecipientList& addresses = iDocument.Mtm( ).AddresseeList();
    
    if( addresses.Count( ) < 1 )
    	{
        iDocument.Mtm().AddAddresseeL(
            iAppUi.CenRep().GetString( KPocaKeyServiceProvider ) );
    	}
	
	// Disable delivery reports for postcards
	iDocument.Mtm().SetDeliveryReport( EMmsNo );
	
    iDocument.Mtm().SaveMessageL();

    TMmsMsvEntry tEntry = static_cast<TMmsMsvEntry>( iDocument.Entry() );
	
    // Update timestamp  
    tEntry.iDate.UniversalTime();

    if ( !tEntry.Visible() )
        {
        // Save from close or exit save.
        // Message should be visible after save
        tEntry.SetVisible( ETrue );
        tEntry.SetInPreparation( EFalse );
        }

    if ( !tEntry.EditorOriented() )
        {
        tEntry.SetEditorOriented( ETrue );
        }

	tEntry.iDetails.Set( iRecipientName->Des( ) ) ;
    tEntry.iDescription.Set(
        iPlainText.Read( 0, Min( iPlainText.DocumentLength(), 20 ) ) );
    
	tEntry.iBioType = KUidMsgSubTypeMmsPostcard.iUid;
    
    iDocument.CurrentEntry().ChangeL( tEntry );
	}

// ---------------------------------------------------------
// CPostcardOperationSave::StartSendingL
// ---------------------------------------------------------
void CPostcardOperationSave::StartSendingL( )
	{
    iSendOperation = iDocument.Mtm().SendL( iStatus );
	}

// ---------------------------------------------------------
// CPostcardOperationSave::ConstructL
// ---------------------------------------------------------
void CPostcardOperationSave::ConstructL( )
    {
    if( HasContact() )
        {
        iFlags |=  EPostcardSaveHasContact;
        CContactItemFieldSet& fieldSet =  iContact.CardFields();
        const CContactItemField* field = TPostcardUtils::FieldOrNull( fieldSet,
            TPostcardUtils::ContactItemNameFromId( EPostcardAddressName ) );
        if( field )
            {
            iRecipientName = field->TextStorage()->Text().AllocL();
            }
        }
    if( !iRecipientName )
        {
        iRecipientName = HBufC::NewL( 0 );
        }
    }

// ---------------------------------------------------------
// CPostcardOperationSave::DoCancel
// ---------------------------------------------------------
void CPostcardOperationSave::DoCancel( )
    {
    if( iManager )
        {
        if( iFlags & EPostcardSaveRequestActive )
            { // Attachment Manager request going on -> cancel
            iManager->CancelRequest();        
            }
        }
    }

// ---------------------------------------------------------
// RunL
// ---------------------------------------------------------
void CPostcardOperationSave::RunL( )
    {
    DoLaunchStepL( );
    }

// ---------------------------------------------------------
// RunError
// ---------------------------------------------------------
TInt CPostcardOperationSave::RunError( TInt aError )
    {
    iLaunchState = -1;
    SetError( aError );
    iObserver.PostcardOperationEvent(
        EPostcardOperationSave,
        EPostcardOperationError );     
    return KErrNone;
    }

// ---------------------------------------------------------
// StartCreatingEmptyAttachmentL
// ---------------------------------------------------------
void CPostcardOperationSave::StartCreatingEmptyAttachmentL( const TFileName& aFileName )
    {
    CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( attachment );
    iTempFile = new ( ELeave ) RFile;
    iManager->CreateAttachmentL( aFileName, *iTempFile, attachment, iStatus );
    iFlags |= EPostcardSaveRequestActive;
    CleanupStack::Pop( attachment );
    iTempAtta = attachment;
    iCommit = ETrue;
    }
    
// ---------------------------------------------------------
// HasContact
// ---------------------------------------------------------
TBool CPostcardOperationSave::HasContact( )
	{
	CContactItemFieldSet& set = iContact.CardFields( );
	for( TInt i = 0; i<set.Count( ); i++ )
		{
		CContactItemField& field = set[i];
		if( field.TextStorage( )->Text( ).Length( ) > 0 )
			{
			return ETrue;
			}
		}
	return EFalse;	
	}
    
// ---------------------------------------------------------
// AddressArrayLC
// ---------------------------------------------------------
CDesCArrayFlat* CPostcardOperationSave::AddressArrayLC( CContactItemFieldSet& aSet )
	{
    // Get contact fields into an array. No ownership.
    const CContactItemField* fields[ CPostcardAddressForm::ENumAddrFields ];

    const TInt first = EPostcardAddressName;
    const TInt last = EPostcardAddressCountry;
    for( TInt i = first; i <= last; i++)
        {
        fields[i - first] = TPostcardUtils::FieldOrNull(
            aSet, TPostcardUtils::ContactItemNameFromId( i ) );
        }

    CDesCArrayFlat* desarr =
        new( ELeave ) CDesCArrayFlat( CPostcardAddressForm::ENumAddrFields );
    CleanupStack::PushL(desarr);

    for( TInt i = 0; i < CPostcardAddressForm::ENumAddrFields; i++ )
        {
        if( fields[i] )
            {
            if( ( iFlags & EPostcardSaveSend) &&
                (iFlags & EPostcardSaveSpecialFormat ) )
                { // We are sending & have the special format on
                // so lets replace possible user added separator strings
                TPtr string = fields[i]->TextStorage()->Text().AllocLC()->Des();
                // Trim leading and trailing white space so it doesn't affect
                // parsing of special format string when opened from sent
                // folder.
                string.Trim();
                ReplaceWith( string,
                    iAppUi.CenRep().GetString( KPocaKeyServiceSeparator ),
                    iAppUi.CenRep().GetString( KPocaKeyServiceReplaceString )
                    );
                desarr->AppendL( string );
                CleanupStack::PopAndDestroy(); // string
                }
            else
                { // Otherwise just add the data field
                desarr->AppendL( fields[i]->TextStorage()->Text() );
                }
            }
        else
            {
            desarr->AppendL( KNullDesC );
            }
        }
    return desarr;
    }

// ---------------------------------------------------------
// VCardAddressArrayL
// ---------------------------------------------------------
CDesCArrayFlat* CPostcardOperationSave::VCardAddressArrayL( CContactItemFieldSet& aSet )
    {
    // Exclude name
    const TInt KPocaVCardAddrFields = CPostcardAddressForm::ENumAddrFields - 1;

    // Get contact fields into an array. No ownership.
    const CContactItemField* fields[ KPocaVCardAddrFields ];

    // Order of contact fields in array
    static const TInt ids[KPocaVCardAddrFields] =
        {
        EPostcardAddressInfo, EPostcardAddressStreet, EPostcardAddressCity,
        EPostcardAddressState, EPostcardAddressZip, EPostcardAddressCountry
        };
    
    for( TInt i = 0; i < KPocaVCardAddrFields; i++)
        {
        fields[i] = TPostcardUtils::FieldOrNull(
            aSet, TPostcardUtils::ContactItemNameFromId( ids[i] ) );
        }

    CDesCArrayFlat* desarr = new(ELeave) CDesCArrayFlat( KPocaVCardAddrFields );
    CleanupStack::PushL( desarr );

    for( TInt i = 0; i < KPocaVCardAddrFields; i++ )
        {
        desarr->AppendL(
            fields[i] ? fields[i]->TextStorage()->Text() : KNullDesC() );
        }

	CleanupStack::Pop( desarr );
	return desarr;
    }

// ---------------------------------------------------------
// ReplaceWith
// ---------------------------------------------------------
void CPostcardOperationSave::ReplaceWith( TDes& aOrig, const TDesC& aWhat,
    const TDesC& aWith )
    {    
	TInt pos = 0;
	TInt foundPos = 0;
	TBool found = ETrue;

	while( found )
	    {
	    found = EFalse;
		if( pos >= aOrig.Length( ) )
			{
			continue; // as found is false -> while is not true anymore
			}	
				
    	foundPos =  aOrig.Mid( pos, aOrig.Length()-pos ).Find( aWhat );
    	
		if( foundPos == KErrNotFound )
			{ // Not found anymore, so the rest is ok
			continue; // as found is false -> while is not true anymore
			}
			
        found = ETrue;
		// As foundPos is related to previous sepa char, we need to add the beginning, too
		foundPos += pos;
		aOrig.Replace( foundPos, aWhat.Length( ), aWith );
	    pos = foundPos + aWhat.Length( ) + ( aWith.Length( ) - aWhat.Length( ) );
	    }    
    }

// ---------------------------------------------------------
// GetImageFileName
// ---------------------------------------------------------
HBufC* CPostcardOperationSave::GetImageFileNameL( )
    {
    // Goes thru the attas and check how many images, text and recipients there are
    TInt count = iManager->AttachmentCount( );
    CMsvAttachment* atta = NULL;

    HBufC* nameString = NULL;
    for(TInt a = 0; a<count && !nameString; a++)
        {
        atta = iManager->GetAttachmentInfoL( a );
        CleanupStack::PushL( atta );

        switch( RecogniseFileType( atta->MimeType( ) ) )
            {
            case EPostcardFileImage:
                {
                CMsvMimeHeaders* msvMime = CMsvMimeHeaders::NewLC();
                msvMime->RestoreL( *atta );
                nameString = msvMime->ContentLocation().AllocL();
                CleanupStack::PopAndDestroy( msvMime ); // msvMime, atta
                break;
                }
            default:
                break;
            }
        CleanupStack::PopAndDestroy( atta );
        }
    return nameString ? nameString : HBufC::NewL( 0 );
    }
    
// EOF
