/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Static helper methods.
*
*/


// INCLUDE FILES
#include "MailUtils.h"            // Own header
#include <MsgMailUtils.rsg>

#include <AknQueryDialog.h>         // CAknTextQueryDialog
#include <aknnotewrappers.h>
#include <StringLoader.h>           // StringLoader
#include <MuiuMsvUiServiceUtilities.h>
#include <CommonContentPolicy.h>
#include <e32property.h>
#include <coemain.h>
#include <CoreApplicationUIsSDKCRKeys.h>

// This is a maximum size in KBs for attachments which are
// fetched without confirmation query
const TInt KMaximumAttachmentSize(100);

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// MailUtils::TextQueryL
// leaves if user cancels query
// -----------------------------------------------------------------------------
//
EXPORT_C void MailUtils::TextQueryL(TInt aQueryResource, TDes& aText)
    {
    CAknTextQueryDialog* dlg = new (ELeave) CAknTextQueryDialog(aText,CAknQueryDialog::ENoTone);
    if (dlg->ExecuteLD(aQueryResource)) //R_DATA_QUERY
        {
        // query approved
        return;
        }
    // query was cancelled
    User::Leave(KLeaveWithoutAlert);
    }
// -----------------------------------------------------------------------------
// MailUtils::ConfirmationNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void MailUtils::ConfirmationNoteL(const TDesC& aText)
    {
    CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote;
    note->ExecuteLD(aText);
    }
// -----------------------------------------------------------------------------
// MailUtils::ConfirmationNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void MailUtils::ConfirmationNoteL(TInt aStringResource)
    {
    HBufC* buf = StringLoader::LoadLC(aStringResource);
    ConfirmationNoteL(*buf);
    CleanupStack::PopAndDestroy(buf);
    }
// -----------------------------------------------------------------------------
// MailUtils::InformationNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void MailUtils::InformationNoteL(const TDesC& aText)
    {
    CAknInformationNote* note = new (ELeave) CAknInformationNote(ETrue);
    note->ExecuteLD(aText);
    }
// -----------------------------------------------------------------------------
// MailUtils::InformationNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void MailUtils::InformationNoteL(TInt aStringResource)
    {
    HBufC* buf = StringLoader::LoadLC(aStringResource);
    InformationNoteL(*buf);
    CleanupStack::PopAndDestroy(buf);
    }

// -----------------------------------------------------------------------------
// MailUtils::ShowNoteIfMsgStoreSpaceLowL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool MailUtils::ShowNoteIfMsgStoreSpaceLowL(const TInt /*aSize*/)
    {
    // Deprecated -> to be removed
    User::Leave(KErrNotSupported);
   
    return EFalse;
    }
// -----------------------------------------------------------------------------
// MailUtils::OfflineProfileActiveL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool MailUtils::OfflineProfileActiveL()
    {
    TInt value(0);
    TBool retVal(EFalse);
        
    CRepository* cenRep = CRepository::NewLC( KCRUidCoreApplicationUIs );
    TInt err = cenRep->Get(KCoreAppUIsNetworkConnectionAllowed, value );
    if ( !err && !value )
    	{ // Phone is offline
        retVal = ETrue;
        }
    CleanupStack::PopAndDestroy(); // cenRep    
    return retVal;
    }
// -----------------------------------------------------------------------------
// MailUtils::IsClosedMimeL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool MailUtils::IsClosedMimeL(const TDesC& aMimeType)
    {
	CCommonContentPolicy* contentPolicy = CCommonContentPolicy::NewLC();

    TBool banned(contentPolicy->IsClosedType( aMimeType ));

	CleanupStack::PopAndDestroy(); //contentPolicy
	
    return banned;
    }

// -----------------------------------------------------------------------------
// MailUtils::IsClosedMimeL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool MailUtils::IsClosedMimeL(const TDesC8& aMimeType)
    {
    HBufC* dataTypeString = HBufC::NewLC(aMimeType.Length());
	dataTypeString->Des().Copy(aMimeType);
    TBool banned(ETrue);
    
    banned = IsClosedMimeL(*dataTypeString);
    
	CleanupStack::PopAndDestroy(); //contentPolicy
	
    return banned;
    }    

// -----------------------------------------------------------------------------
// MailUtils::IsClosedFileL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool MailUtils::IsClosedFileL( RFile& aFileHandle )
    {
    if ( aFileHandle.SubSessionHandle() != 0 )
        {
	    CCommonContentPolicy* contentPolicy = CCommonContentPolicy::NewLC();

        TBool banned( contentPolicy->IsClosedFileL( aFileHandle ) );

	    CleanupStack::PopAndDestroy(); //contentPolicy
        return banned;
        }
    else
        {
        return EFalse;
        }
    }  

// -----------------------------------------------------------------------------
// MailUtils::ConfirmationQueryL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool MailUtils::ConfirmationQueryL(const TDesC& aText, TInt aNoteResource)
    {
    CAknQueryDialog* qDlg = CAknQueryDialog::NewL();

    if (qDlg->ExecuteLD(aNoteResource, aText))
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// MailUtils::ConfirmationQueryL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool MailUtils::ConfirmationQueryL(
	TInt aStringResource, 
	TInt aNoteResource)
    {
    HBufC* buf = StringLoader::LoadLC(aStringResource);
    TBool result = ConfirmationQueryL(*buf, aNoteResource);
    CleanupStack::PopAndDestroy(buf);
    return result;
    }

// -----------------------------------------------------------------------------
// MailUtils::LargeAttachmentQueryL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool MailUtils::LargeAttachmentQueryL( TInt aSize, TInt aStatus )
    {
    
    TInt size( aSize / KKilo );
    // This check is done in MsgAttachmentUtils::FileSizeToStringL
    // so it is done here also, so consistent sizes are shown to user.
    if (size % KKilo)
        {
        size++;
        }  
	enum TResponce
		{
		EMsgMailNotQueried,
		EMsgMailFetch,
		EMsgMailDontFetch	
		};
	TInt noteResource( R_MAIL_QUEST_LARGE_ATTA_FWD );
	TInt secondNoteResource( R_MAIL_QUEST_FETCH_ATTA_FWD );
	if ( (aStatus & EQueryAttachmentFetch ) )
		{
		noteResource = R_MAIL_LARGE_ATTA_QUERY;
		secondNoteResource = R_MAIL_QUEST_FETCH_ATTACHMENT;
		}

    TBool response( EMsgMailNotQueried );
        
    if ( size > KMaximumAttachmentSize )
        {
        HBufC* string = StringLoader::LoadLC(
        	noteResource,
            size );
        if ( !MailUtils::ConfirmationQueryL(*string,
            R_MAIL_YES_NO_QUERY_DLG) )
            {
            response = EMsgMailDontFetch;
            }
       	else
       		{
       		response = EMsgMailFetch;
       		}
        CleanupStack::PopAndDestroy(); // string
        }
    
	if ( response == EMsgMailDontFetch )
		{
		return EFalse;
		}
		
	// if we're offline, ask confirmation to connect
	if ( response == EMsgMailNotQueried && (aStatus & EConnectionOffline) )
		{
        if ( !MailUtils::ConfirmationQueryL(
        	secondNoteResource, 
            R_MAIL_YES_NO_QUERY_DLG) )
			{
			return EFalse;
			}
		}
        
    return ETrue;
    }
// -----------------------------------------------------------------------------
// MailUtils::OfflineProfileActiveWithNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool MailUtils::OfflineProfileActiveWithNoteL()
    {
    User::Leave( KErrNotSupported );
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// MailUtils::ConfirmationQueryL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool MailUtils::ConfirmationQueryL( const TDesC& aText )
	{
	return MailUtils::ConfirmationQueryL( aText, R_MAIL_YES_NO_QUERY_DLG );
	}
     
// -----------------------------------------------------------------------------
// MailUtils::ConfirmationQueryL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool MailUtils::ConfirmationQueryL( TInt aStringResource )
	{
	return MailUtils::ConfirmationQueryL( 
		aStringResource, R_MAIL_YES_NO_QUERY_DLG );
	}

// -----------------------------------------------------------------------------
// MailUtils::FileSize()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt MailUtils::FileSize(const TDesC& aFilename)
    {
    RFile file;
    RFs& fs = CCoeEnv::Static()->FsSession();
    TInt filesize(file.Open(fs, aFilename, EFileShareReadersOnly));
    if (filesize == KErrNone)
        {
        file.Size(filesize);
        }
    file.Close();

    return filesize;    
    }
	        
// End of File
