/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for attachment verification
*
*/


// INCLUDE FILES
#include "Msgattachmentverifier.h"
#include "MsgMailDRMHandler.h"
#include "MailUtils.h"
#include <MsgMailEditor.rsg>
#include <MuiuMsvUiServiceUtilities.h>
#include <e32base.h>
#include <coemain.h>

// LOCAL CONSTANTS AND MACROS


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor cannot contain any code that might leave
CMsgAttachmentVerifier::CMsgAttachmentVerifier( CMsvSession& aMsvSession )
    : iMsvSession( aMsvSession )
    {
    }
// Symbian OS default constructor can leave.
void CMsgAttachmentVerifier::ConstructL()
    {
    iDRMHandler = MsgMailDRMHandler::NewL();
    }
// NewLC
CMsgAttachmentVerifier* CMsgAttachmentVerifier::NewLC(
    CMsvSession& aMsvSession )
    {
    CMsgAttachmentVerifier* self =
        new (ELeave) CMsgAttachmentVerifier( aMsvSession );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

    
// Destructor
CMsgAttachmentVerifier::~CMsgAttachmentVerifier()
    {
    delete iDRMHandler;
    }

// ---------------------------------------------------------------------------
// Verify selected files or show a note if something wrong in a selection.
// ---------------------------------------------------------------------------
//
TBool CMsgAttachmentVerifier::VerifySelectionL( 
    const MDesCArray* aSelectedFiles )
    {
    ASSERT( aSelectedFiles );
    TBool result( ETrue );
    TInt attachmentsByteSize( 0 );
    TInt selectionCount = aSelectedFiles->MdcaCount();
    for (TInt i=0; i<selectionCount && result; i++)
        {
        TInt fileSize( 0 );
      	result = VerifyFileL( aSelectedFiles->MdcaPoint(i), fileSize );
      	attachmentsByteSize += fileSize;
        }
    if ( result )
        {
        result = VerifyDiskSpaceL( attachmentsByteSize );
        }
    return result;
    }

// ---------------------------------------------------------------------------
// Verify selected files or show a note if something wrong in a selection.
// ---------------------------------------------------------------------------
//
TBool CMsgAttachmentVerifier::OkToExitL( 
	const TDesC& aDriveAndPath, const TEntry& aEntry )
	{
	TBool result( ETrue );
	TInt length = aDriveAndPath.Length() + aEntry.iName.Length();
	if ( length )
		{
		HBufC* filename = HBufC::NewLC( length );
		TPtr ptr = filename->Des();
		ptr.Append( aDriveAndPath );
		ptr.Append( aEntry.iName );
		TInt fileSize( 0 );
		result = VerifyFileL( *filename, fileSize );
		CleanupStack::PopAndDestroy( filename );
        if ( result )
            {
            result = VerifyDiskSpaceL( fileSize );
            }		
		}
	return result;	
	}
	
// ---------------------------------------------------------------------------
// Verify selected files or show a note if something wrong in a selection.
// ---------------------------------------------------------------------------
//
TBool CMsgAttachmentVerifier::VerifyFileL(
    const TDesC& aFilename,
    TInt& aFileByteSize )
	{
    RFs& fs = CCoeEnv::Static()->FsSession();
    TBool result( ETrue );
   	RFile attachment;
    User::LeaveIfError( attachment.Open(
        fs, 
        aFilename, 
        EFileShareReadersOnly) );
        
    CleanupClosePushL(attachment);
    
    // Size() may return error value but then we can just ignore it, file
    // size checking is not considered that necessary
    aFileByteSize = 0;
    attachment.Size( aFileByteSize );
    
    if ( MailUtils::IsClosedFileL( attachment ) ) 
        {
        MailUtils::InformationNoteL(R_QTN_MAIL_SEND_FORBID_1);
        result = EFalse;
        }
    else if ( iDRMHandler->IsSuperDistributableFileL( attachment ) )
        {
        result = MailUtils::ConfirmationQueryL(
            R_QTN_MAIL_SEND_WO_RIGHTS_SD_1, 
            R_MAIL_EDITOR_DELETE_MESSAGE );
        }
    // attachment
    CleanupStack::PopAndDestroy();  // CSI: 12 # RFile
    return result;       
	}

// ---------------------------------------------------------------------------
// Verifies that there is enough disk space to add attachment(s)
// ---------------------------------------------------------------------------
//
TBool CMsgAttachmentVerifier::VerifyDiskSpaceL( TInt aAttachmentsByteSize )
    {
    TBool result( ETrue );
    if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(
            iMsvSession, aAttachmentsByteSize ) )
        {
        MailUtils::ConfirmationQueryL( R_QTN_MEMLO_NOT_ENOUGH_MEMORY,
                                       R_MAIL_CONFIRM_NOT_ENOUGH_MEMORY );
        result = EFalse;
        }
    return result;
    }


//  End of File  
