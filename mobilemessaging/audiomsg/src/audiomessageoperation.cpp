/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Active object to handle asynchronious.
*                A base class for AudiomessageLaunchOperation-,
*                AudiomessageInsertOperation-,AudiomessageSaveOperation- and
*                AudiomessageSendOperation -classes
*
*/



#include <basched.h>
#include <StringLoader.h>

#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <mmsclient.h>
#include <MsgMimeTypes.h>
#include <MsgEditorCommon.h>
#include <cmsvmimeheaders.h>
#include <audiomessage.rsg>
#include "audiomessageoperation.h"

const TInt KSecondAsMilliSeconds( 1000 );
_LIT( KSmilFile,"pres.smil");

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
// CAudioMessageOperation::Constructor
// ---------------------------------------------------------
//
CAudioMessageOperation::CAudioMessageOperation(
        MAmsOperationObserver& aObserver,
        CAudioMessageDocument& aDocument ) : CActive( EPriorityStandard ),
    	iObserver( aObserver ),
    	iDocument( aDocument ),
   	    iError( 0 )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------
// CAudioMessageOperation::Destructor
// ---------------------------------------------------------
//
CAudioMessageOperation::~CAudioMessageOperation()
    {
    delete iStore;
    }

// ---------------------------------------------------------
// CAudioMessageOperation::CompleteSelf
// ---------------------------------------------------------
//
void CAudioMessageOperation::CompleteSelf( TInt aError )
    {
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive();
    User::RequestComplete( pStatus, aError );
    }

// ---------------------------------------------------------
// CAudioMessageOperation::DoEditInitL
// ---------------------------------------------------------
//
void CAudioMessageOperation::DoEditInitL( )
    {
	iStore = iDocument.Mtm( ).Entry( ).EditStoreL( );
    iManager = &( iStore->AttachmentManagerL( ) );
    iManagerSync = &( iStore->AttachmentManagerExtensionsL( ) );
    }
    
// ---------------------------------------------------------
// CAudioMessageOperation::DoReadInitL
// ---------------------------------------------------------
//
void CAudioMessageOperation::DoReadInitL( )
    {
   	iStore = iDocument.Mtm( ).Entry( ).ReadStoreL( );
    iManager = &( iStore->AttachmentManagerL( ) );
    iManagerSync = &( iStore->AttachmentManagerExtensionsL( ) );
    }
  
// ---------------------------------------------------------
// CAudioMessageOperation::MakeCommitL
// ---------------------------------------------------------
//
void  CAudioMessageOperation::MakeCommitL( )
   	{
   	iStore->CommitL( );
   	}
   
// ---------------------------------------------------------
// CAudioMessageOperation::UnInit
// ---------------------------------------------------------
//
void CAudioMessageOperation::UnInit( )
    {
    delete iStore;
    iStore = NULL;
    }   

// ---------------------------------------------------------------------------
// CAudioMessageOperation::CheckAttasL
// Check how many voice and smil attas there are
// Save atta id and smil id
// ---------------------------------------------------------------------------
//    
TInt CAudioMessageOperation::CheckAttasL( )
    {
    TInt count = iManager->AttachmentCount();
    CMsvAttachment* atta = NULL;
    TInt numOfSmils( 0 );
    TInt numOfAudios( 0 );
    for( TInt a = 0; a < count; a++ )
        {
        atta = iManager->GetAttachmentInfoL( a );
        CleanupStack::PushL( atta );
        TDataType  k = atta->MimeType();

   		if ( k.Des8().CompareF( KMsgMimeAudioAmr ) == 0 )
   			{
			numOfAudios++;
   		    iDocument.iAudio = atta->Id( );
   			}

   		if ( k.Des8().CompareF( KMsgMimeSmil ) == 0 )
   			{
			numOfSmils++;
   		    iDocument.iSmil = atta->Id( );
   			}
        CleanupStack::PopAndDestroy( atta );
        }
        
	if( numOfAudios > 1 || numOfSmils > 1 )
        {
        // This case seems to be impossible in real life
        SetError( EAmsMultipleFiles );
        CompleteSelf( -1 );
        return EAmsMultipleFiles;
        }

 	return KErrNone;
    }


// ---------------------------------------------------------
// CAudioMessageOperation::AddSmilL
// ---------------------------------------------------------
//
void CAudioMessageOperation::AddSmilL()
	{
 	TFileName file( KSmilFile );
    CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( attachment );
    
    if( iDocument.iSmil )
		{ // Remove the old smil
		TInt oldSmil = TMsvIdToIndexL( iDocument.iSmil );
		if ( oldSmil != KErrNotFound )
		    {
		    iStore->AttachmentManagerExtensionsL( ).RemoveAttachmentL( oldSmil );
		    }
		}
    
    RFile *editFile = new ( ELeave ) RFile;
   	iManager->CreateAttachmentL( file, *editFile, attachment, iStatus );
   	CleanupStack::Pop( attachment );
    delete iHeaders;
    iHeaders = NULL;
 	iHeaders = CMsvMimeHeaders::NewL( );
    iHeaders->SetContentLocationL( KSmilFile );
    TInt slash = KMsgMimeSmil().Locate( '/' );
    iHeaders->SetContentTypeL( KMsgMimeSmil().Left( slash ) );
    iHeaders->SetContentSubTypeL( KMsgMimeSmil().Mid( slash + 1 ) );
	attachment->SetMimeTypeL( KMsgMimeSmil );
    iDocument.iSmil = attachment->Id( );

 	TInt dur = I64INT( iDocument.GetDuration().Int64() ) / KSecondAsMilliSeconds;
	
    CArrayFixFlat<TInt>* array = new ( ELeave ) CArrayFixFlat<TInt>( 3 );
    CleanupStack::PushL( array );
    array->AppendL( MsgEditorCommons::ScreenWidth() );
    array->AppendL( MsgEditorCommons::ScreenHeigth() );
    array->AppendL( dur );
	TInt resId = R_AUDIOMESSAGE_SMIL;
    
    CDesCArrayFlat* arrayForFileName = new ( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( arrayForFileName );
    //дце replaced earlier in iFileName
    arrayForFileName->AppendL( iContentLocationFileName );  
    HBufC* smil = StringLoader::LoadLC( resId, *arrayForFileName, *array );
    
    // convert the smil to 8-bit
    HBufC8* smil8 = HBufC8::NewLC( smil->Des( ).Length( ) );
    smil8->Des().Copy( smil->Des( ) );

    RFileWriteStream smilStream( *editFile );
    smilStream.PushL( );    
    delete editFile; 
    editFile = NULL;
 	smilStream.WriteL( *smil8 );
    smilStream.Close( );
    smilStream.Pop( );
    CleanupStack::PopAndDestroy( 4, array ); // smil, array, arrayForFileName
 
  	iHeaders->StoreL( *attachment );
   	if ( iHeaders )
	    {
	   	delete iHeaders;
	    iHeaders=NULL;
	    }
	MakeCommitL( );
	}


// ---------------------------------------------------------
// CAudioMessageOperation::TMsvIdToIndexL
// ---------------------------------------------------------
//
TInt CAudioMessageOperation::TMsvIdToIndexL( TMsvAttachmentId& aId )
	{
    TInt count = iManager->AttachmentCount( );
    CMsvAttachment* atta = NULL;
    for(TInt a = 0; a<count; a++)
        {
        atta = iManager->GetAttachmentInfoL( a );
        if ( atta->Id( ) == aId )
        	{
        	delete atta;
        	return a;
        	}
        delete atta;
        }
	return KErrNotFound;
	}

// ---------------------------------------------------------
// CAudioMessageOperation::RunError
// ---------------------------------------------------------
//
TInt CAudioMessageOperation::RunError( TInt aError )
    {
 	if ( aError == KLeaveExit )
        {
        return KLeaveExit;
        }
    else
        {
        SetError( aError );
        CompleteSelf( aError );
        return KErrNone;
        }
    }

// ---------------------------------------------------------
// CAudioMessageOperation::GetError
// ---------------------------------------------------------
//
TInt CAudioMessageOperation::GetError( )
    {
    return iError;
    }

// ---------------------------------------------------------
// CAudioMessageOperation::SetError
// ---------------------------------------------------------
//
void CAudioMessageOperation::SetError( TInt aError )
    {
    iError = aError;
    }
    
// ---------------------------------------------------------
// CAudioMessageOperation::ResetError
// ---------------------------------------------------------
//
void CAudioMessageOperation::ResetError( )
    {
    iError = 0;
    }
