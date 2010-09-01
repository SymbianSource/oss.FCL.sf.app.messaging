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
*       CPostcardOperation
*
*/



// ========== INCLUDE FILES ================================

#include <badesca.h>
#include <e32std.h>
#include <apmstd.h>  // TDataType

#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <cmsvattachment.h>
#include <cmsvmimeheaders.h>

#include <mmsclient.h>

#include <MsgMimeTypes.h>

#include <Postcard.rsg>
#include "PostcardOperation.h"

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
// CPostcardOperation::CPostcardOperation
// ---------------------------------------------------------
CPostcardOperation::CPostcardOperation(
        MPostcardOperationObserver& aObserver,
        CPostcardDocument& aDocument,
        CPostcardAppUi& aAppUi,
        RFs& aFs ) :
    CActive( EPriorityStandard ),
    iObserver( aObserver ),
    iDocument( aDocument ),
    iAppUi( aAppUi ),
    iFs( aFs ),
    iError( 0 ),
    iStore( NULL ),
    iHeaders( NULL ),
    iImage( 0 ),
    iText( 0 ),
    iRecipient( 0 ),
    iCommit( EFalse )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CPostcardOperation::CPostcardOperation
// ---------------------------------------------------------
CPostcardOperation::~CPostcardOperation( )
    {
    delete iHeaders;
    if( iStore )
        {
        delete iStore;    
        }
    }

// ---------------------------------------------------------
// InitL
// ---------------------------------------------------------
void CPostcardOperation::InitL( )
    {
    if( iDocument.MessageType( ) == EPostcardSent )
    	{
    	iStore = iDocument.Mtm( ).Entry( ).ReadStoreL( );
    	}
    else
    	{
    	iStore = iDocument.Mtm( ).Entry( ).EditStoreL( );
    	}
    iManager = &( iStore->AttachmentManagerL( ) );
    }

// ---------------------------------------------------------
// UnInitL
// ---------------------------------------------------------
void CPostcardOperation::UnInitL( )
    {
    if( iCommit )
    	{
    	iStore->CommitL( );
    	}
    delete iStore;
    iStore = NULL;
    }

// ---------------------------------------------------------
// CheckAttas
// ---------------------------------------------------------
TInt CPostcardOperation::CheckAttasL( )
    {
    // Goes thru the attas and check how many images, text and recipients there are
    TInt count = iManager->AttachmentCount( );
    CMsvAttachment* atta = NULL;
    TInt numOfImages(0), numOfTexts(0), numOfRecipients(0), numOfOthers(0);
    TInt numOfSmils(0);
    for(TInt a = 0; a<count; a++)
        {
        atta = iManager->GetAttachmentInfoL( a );
        CleanupStack::PushL( atta );
        switch( RecogniseFileType( atta->MimeType( ) ) )
            {
            case EPostcardFileImage:
                numOfImages++;
                iImage = atta->Id( );
                break;
            case EPostcardFileText:
                numOfTexts++;
                iText = atta->Id( );
                break;
            case EPostcardFileAddress:
                numOfRecipients++;
                iRecipient = atta->Id( );
                break;
            case EPostcardFileSmil:
                numOfSmils++;
                iSmil = atta->Id( );
                break;
            default:
                {
    	        if( atta->AttachmentName( ).Find( _L("X-Nokia-PhonebookId_") ) != -1  )
    	            {
    	            iText = atta->Id( );
    	            numOfTexts++;
    	            }
                else
                    {
                    numOfOthers++;
                    }
                break;
                }
            }
        CleanupStack::PopAndDestroy( atta );
        }
    if( numOfImages > 1 || numOfTexts > 1 || numOfRecipients > 1 )
        {
        // Do something here as there's invalid number of attachments!
        SetError( R_POSTCARD_MULTIPLE_FILES );
        CompleteSelf( -1 );
        return R_POSTCARD_MULTIPLE_FILES;
        }
    if( numOfOthers > 0 || numOfSmils > 1 )
    	{
        SetError( R_POSTCARD_FORMAT_NOT_SUPPORTED );
        CompleteSelf( -1 );
        return R_POSTCARD_FORMAT_NOT_SUPPORTED;
    	}
    return KErrNone;
    }

// ---------------------------------------------------------
// CompleteSelf
// ---------------------------------------------------------
void CPostcardOperation::CompleteSelf( TInt aError )
    {
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive( );
    User::RequestComplete( pStatus, aError );
    }

// ---------------------------------------------------------
// GetErrors
// ---------------------------------------------------------
TInt CPostcardOperation::GetError( )
    {
    return iError;
    }

// ---------------------------------------------------------
// SetError
// ---------------------------------------------------------
void CPostcardOperation::SetError( TInt aError )
    {
    iError = aError;
    }
    
// ---------------------------------------------------------
// ResetErrors
// ---------------------------------------------------------
void CPostcardOperation::ResetError( )
    {
    iError = 0;
    }

// ---------------------------------------------------------
// RecogniseFileType
// ---------------------------------------------------------
TPostcardFileType CPostcardOperation::RecogniseFileType( const TDataType& aMimeType )
    {
    if ( aMimeType.Des8( ).CompareF( KMsgMimeTextPlain ) == 0 )
        {
        return EPostcardFileText;
        }
    else if ( aMimeType.Des8( ).CompareF( KMsgMimeImageJpeg ) == 0 )
        {
        return EPostcardFileImage;
        }
    else if ( aMimeType.Des8( ).CompareF( KMsgMimeImagePng ) == 0 )
        {
        return EPostcardFileImage;
        }
    else if ( aMimeType.Des8( ).CompareF( KMsgMimeImageGif ) == 0 )
        {
        return EPostcardFileImage;
        }
    else if ( aMimeType.Des8( ).CompareF( KMsgMimeVCard ) == 0 )
        {
        return EPostcardFileAddress;
        }
    else if ( aMimeType.Des8( ).CompareF( KMsgMimeSmil ) == 0 )
        {
        return EPostcardFileSmil;
        }
    else
        {
        return EPostcardFileUnknown;
        }
    }

// ---------------------------------------------------------
// CPostcardOperation::TMsvIdToIndex
// ---------------------------------------------------------
TInt CPostcardOperation::TMsvIdToIndexL( TMsvAttachmentId& aId )
	{
    TInt count = iManager->AttachmentCount( );
    CMsvAttachment* atta = NULL;
    for(TInt a = 0; a<count; a++)
        {
        atta = iManager->GetAttachmentInfoL( a );
        if( atta->Id( ) == aId )
        	{
        	delete atta;
        	return a;
        	}
        delete atta;
        }
	return KErrNotFound;
	}

// EOF
