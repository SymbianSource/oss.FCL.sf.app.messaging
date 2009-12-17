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
* Description:   Class that handles message attachment operations:
*              : insert and delete.
*
*/



#include <apmstd.h>
#include <cmsvmimeheaders.h>
#include <StringLoader.h>
#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <MsgEditorCommon.h>
#include <MsgMediaInfo.h>
#include <MsgMimeTypes.h>
#include <audiomessage.rsg>
#include "audiomessageinsertoperation.h"
#include "audiomessagedocument.h"
#include "audiomessagerecorder.h"
#include "AudioMessageLogging.h"
#include "msgtextutils.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------
// CAudioMessageInsertOperation::NewL
// ---------------------------------------------------------
//
CAudioMessageInsertOperation* CAudioMessageInsertOperation::NewL(
				MAmsOperationObserver& aOperationObserver,
				CAudioMessageDocument& aDocument,
				CMsgEditorView& aView )
		{
		CAudioMessageInsertOperation* self = new ( ELeave ) CAudioMessageInsertOperation(
			aOperationObserver,
			aDocument,
			aView );

		return self;
		}

// ---------------------------------------------------------
// CAudioMessageInsertOperation::Constructor
// ---------------------------------------------------------
//
CAudioMessageInsertOperation::CAudioMessageInsertOperation(
		 MAmsOperationObserver& aOperationObserver,
		 CAudioMessageDocument& aDocument,
		 CMsgEditorView& aView ) : CAudioMessageOperation( aOperationObserver, aDocument ),
		 iView( aView ), iState( 0 ), iNew( ETrue )
		{
		}


// ---------------------------------------------------------
// CAudioMessageInsertOperation::Destructor
// ---------------------------------------------------------
//
CAudioMessageInsertOperation::~CAudioMessageInsertOperation()
		{
		Cancel();
		}

// ---------------------------------------------------------
// CAudioMessageInsertOperation::Insert
// ---------------------------------------------------------
//
void CAudioMessageInsertOperation::Insert( CMsgMediaInfo* aInfo, TBool aVal, TBool aNew )
		{
		iRemove = aVal;
		iNew = aNew;
		ResetError();
		iInfo = aInfo;
		iState = EAmsInit;
		CompleteSelf( KErrNone );
		}
			

// ---------------------------------------------------------
// CAudioMessageInsertOperation::RunL
// ---------------------------------------------------------
//
void CAudioMessageInsertOperation::RunL()
	{
	if ( iStatus.Int() != KErrNone )
        {
        SetError( iStatus.Int() );
        UnInit();
        iObserver.EditorOperationEvent(
            EAmsOperationInsert,
            EAmsOperationError );
        }
    else
        {
        DoInsertStepL();
        }    
	}


// ---------------------------------------------------------
// CAudioMessageInsertOperation::DoInsertStepL
// ---------------------------------------------------------
//
void CAudioMessageInsertOperation::DoInsertStepL()
    {
    switch ( iState )
		{
		case EAmsInit:
			DoEditInitL();
			iState = EAmsCheckAttas;
			CompleteSelf( KErrNone );
			break;
		case EAmsCheckAttas:
			CheckAttasL( );
			
			if ( iRemove == EFalse)
				{
				iState = EAmsSetAudio;
				}
			else
				{
				iState = EAmsRemoveAudio;
				}
			CompleteSelf( KErrNone );
			break;
		case EAmsRemoveAudio:
			DoRemoveAudioL();
			iState = EAmsRemoveSmil;
			CompleteSelf( KErrNone );
			break;
		case EAmsRemoveSmil:
			DoRemoveSmilL();
			iState = EAmsCommit;
			CompleteSelf( KErrNone );
			break;
		case EAmsSetAudio:	
			DoCreateNewAudioAttaL( );
			iState = EAmsSetDuration;
			SetActive();
			break;
	   case EAmsSetDuration:
			DoSetDurationL( );
			iState = EAmsInsertSmil;
			CompleteSelf( KErrNone );
			break;	
		case EAmsInsertSmil:
			AddSmilL();
			iState = EAmsCommit;
			SetActive();
			break;
		case EAmsCommit:
			MakeCommitL();
			UnInit();
			iState = EAmsInsertReady;
			CompleteSelf( KErrNone );
			break;
		case EAmsInsertReady:
			if ( iRemove == EFalse )
				{
				iObserver.EditorOperationEvent(
					EAmsOperationInsert,
					EAmsOperationComplete );
				}
			else
				{
				iObserver.EditorOperationEvent(
					EAmsOperationRemove,
					EAmsOperationComplete );
				}
			break;
		default:
			break;
		}
    }

// ---------------------------------------------------------
// CAudioMessageInsertOperation::DoCreateNewAudioAttaL
// ---------------------------------------------------------
//
void CAudioMessageInsertOperation::DoCreateNewAudioAttaL( )
	{
	CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
	CleanupStack::PushL( attachment );
	if ( iDocument.iAudio )
		{ // Remove the old audio
		TInt oldAudio = TMsvIdToIndexL( iDocument.iAudio );
		if ( oldAudio != KErrNotFound )
		    {
		    iStore->AttachmentManagerExtensionsL( ).RemoveAttachmentL( oldAudio );
		    }
		}
    TParsePtrC parse( iInfo->FullFilePath() );
	attachment->SetAttachmentNameL( parse.NameAndExt() );
	iManager->AddAttachmentL( iInfo->FullFilePath(), attachment, iStatus );
	CleanupStack::Pop( attachment );
	iHeaders = CMsvMimeHeaders::NewL( );
	TDataType mime = iInfo->MimeType( );
	
	iContentLocationFileName = parse.NameAndExt();
	CMsgTextUtils::TrimAndRemoveNonAlphaDigit( iContentLocationFileName );
	iHeaders->SetContentLocationL( iContentLocationFileName );
   
    TInt slash = mime.Des8().Locate( '/' );
    iHeaders->SetContentTypeL( mime.Des8().Left( slash ) );
    iHeaders->SetContentSubTypeL( mime.Des8().Mid( slash + 1 ) );
	attachment->SetMimeTypeL( mime.Des8() );
	iDocument.iAudio = attachment->Id( );
	
	iHeaders->StoreL( *attachment );
	if ( iHeaders )
		{
		delete iHeaders;
		iHeaders=NULL;
		}	
	}

// ---------------------------------------------------------
// CAudioMessageInsertOperation::DoCancel
// ---------------------------------------------------------
//
void CAudioMessageInsertOperation::DoCancel()
	{
	if ( iManager )
		{
		iManager->CancelRequest();
		}
	}


// ---------------------------------------------------------
// CAudioMessageInsertOperation::CheckSizeForInsert
// ---------------------------------------------------------
//
TBool CAudioMessageInsertOperation::CheckSizeForInsert()
	{
	if( iInfo )
		{
		if ( TUint( iInfo->FileSize() ) > iDocument.MaxMessageSize() )
			{
			SetError( EAmsInsertTooBig );
			return EFalse;
			}
		else 
			{
			return ETrue;
			}
		}
	 return ETrue;
	}
 
// ---------------------------------------------------------
// CAudioMessageInsertOperation::DoRemoveAudioL
// ---------------------------------------------------------
//
void CAudioMessageInsertOperation::DoRemoveAudioL( )
	{
	if ( iDocument.iAudio )
		{ // Remove the old audio
		TInt oldAudio = TMsvIdToIndexL( iDocument.iAudio );
		iStore->AttachmentManagerExtensionsL( ).RemoveAttachmentL( oldAudio );
		}
	}

// ---------------------------------------------------------
// CAudioMessageInsertOperation::DoRemoveSmilL
// ---------------------------------------------------------
//
void CAudioMessageInsertOperation::DoRemoveSmilL( )
	{
	if ( iDocument.iSmil )
		{ // Remove the old audiosmil
		TInt oldSmil = TMsvIdToIndexL( iDocument.iSmil );
		iStore->AttachmentManagerExtensionsL( ).RemoveAttachmentL( oldSmil );
		}
	}

// ---------------------------------------------------------
// CAudioMessageInsertOperation::DoSetDurationL
// ---------------------------------------------------------
//
void CAudioMessageInsertOperation::DoSetDurationL( )
	{	
    if ( iNew )
        {
        ; //Duration already set into document object
        }
    else
        {
        //We must get the duration for the SMIL
	    RFile attaFile = iManager->GetAttachmentFileL( iDocument.iAudio );
        CleanupClosePushL( attaFile ); // RFile must be closed
        CAudioMessageRecorder *recorder = CAudioMessageRecorder::NewL( attaFile, EFalse );
        iDocument.SetDuration( recorder->GetDuration() );
        delete recorder;
        recorder = NULL;
	    CleanupStack::PopAndDestroy( &attaFile );
        }    

	}
