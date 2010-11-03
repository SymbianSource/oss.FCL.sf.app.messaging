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
* Description:  Engine for ICalViewer
*
*/


// INCLUDE FILES
#include "CICalEngine.h"

//RPointerArray
#include <e32std.h>
//RFileReadStream
#include <s32file.h>
//CRichText
#include <txtrich.h>
//CPlainText
#include <txtetext.h>
//CCalEntry (Calendar entry API V2)
#include <calentry.h>
//attachments
#include <CMsvAttachment.h>
//Entry ui ECOM implemantion
#include <CAgnEntryUi.h>


#include <MMsvAttachmentManager.h>

// Temporary files could always be saved to drive C.
_LIT(KDefaultTmpFileName,"C:\\system\\temp\\");				// CSI: 25 # See comment above.
_LIT(KDefaultSettingsFileName,"C:\\system\\temp\\icaltmp");	// CSI: 25 # See comment above.

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CICalEngine::CICalEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CICalEngine::CICalEngine()
    {
    }

// -----------------------------------------------------------------------------
// CICalEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CICalEngine::ConstructL()
    {

    }

// -----------------------------------------------------------------------------
// CICalEngine::~CICalEngine
// Destructor
// -----------------------------------------------------------------------------
//
CICalEngine::~CICalEngine()
    {
    }
    
// -----------------------------------------------------------------------------
// CICalEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CICalEngine* CICalEngine::NewL()
    {
    CICalEngine* self = new( ELeave ) CICalEngine;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CICalEngine::ConvertRichTextToStreamL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CICalEngine::ConvertRichTextToStreamL(	// CSI: 40 # We must return 
		  												// the integer value although this 
		  												// is a leaving method.
    CRichText* aBuffer, RFileReadStream& aReadStream)
	{
	HBufC* extracted = HBufC::NewL(1000);				// CSI: 47 # <insert a comment here>
	CleanupStack::PushL(extracted);
	TPtr extractedBuf = extracted->Des();
	aBuffer->Extract(extractedBuf);

	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	fs.MkDirAll( KDefaultTmpFileName );

	HBufC8* copy8Bit = HBufC8::NewL( 1000 );			// CSI: 47 # <insert a comment here>
	CleanupStack::PushL( copy8Bit );
	TPtr8 copy8BitPtr = copy8Bit->Des();

	copy8BitPtr.Copy( extractedBuf );

	RFile file;
	file.Replace( fs, KDefaultSettingsFileName, EFileWrite );
	file.Write( copy8BitPtr );
	file.Flush();
	file.Close();

	CleanupStack::PopAndDestroy( copy8Bit );

	aReadStream.Open( fs, KDefaultSettingsFileName, EFileRead );
	
	CleanupStack::PopAndDestroy( 2, extracted );			// CSI: 47 # extracted, fs

	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CICalEngine::ParseStreamToCalEntryL
// -----------------------------------------------------------------------------
//
EXPORT_C CCalEntry* CICalEngine::ParseStreamToCalEntryL(
    RFileReadStream& /*aReadStream*/)
	{
	// Not supported
	return NULL;
	}

// -----------------------------------------------------------------------------
// CICalEngine::ResolveAttachmentsVisibilityL
// -----------------------------------------------------------------------------
//
EXPORT_C void CICalEngine::ResolveAttachmentsVisibilityL(
    MMsvAttachmentManager& /*aManager*/,
    RPointerArray<CMsvAttachment>& /*aAttachementsArray*/, 
    TBool /*aBodyHasOMR*/)
	{
//	RFs fs;
//	User::LeaveIfError(fs.Connect());
//	CleanupClosePushL(fs);
/*	TInt count(aManager.AttachmentCount());
	for (TInt index=0; index<count; ++index)
		{
		RFile attachmentFile = aManager.GetAttachmentFileL(index);
        CleanupClosePushL( attachmentFile );
		TInt fileSize = 0;
		User::LeaveIfError( attachmentFile.Size(fileSize) );

		HBufC8* fileBuf = HBufC8::NewLC(fileSize);
		TPtr8 fileBufPtr = fileBuf->Des();
		User::LeaveIfError(attachmentFile.Read(fileBufPtr));

		TBool isOMR;

		//isOMR = RecogniseOMREntryL(fileBufPtr);

		CleanupStack::PopAndDestroy();//fileBuf is not needed any more
		
		//if index is 0 AND attachment is OMR it is not included in 
		// the attachments model
		// UNLESS a body already has OMR. 
		// In that case this another OMR attachment IS included in model
		if ((isOMR && index > 0) || aBodyHasOMR)
			{
			CMsvAttachment* info = aManager.GetAttachmentInfoL(index);
			CleanupStack::PushL(info);
			aAttachementsArray.Append(info);
			}
		}*/
//	CleanupStack::PopAndDestroy(); //fs
	}

// -----------------------------------------------------------------------------
// CICalEngine::CreateEntryUIL
// -----------------------------------------------------------------------------
//
EXPORT_C MAgnEntryUi* CICalEngine::CreateEntryUIL( const TDesC8& aMtmId )
	{
	MAgnEntryUi* entryUI;
	entryUI = CAgnEntryUi::NewL( aMtmId );
	return entryUI;
	}

//  End of File

