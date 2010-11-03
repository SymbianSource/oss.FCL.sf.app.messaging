/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: UniClientMtm implementation
*
*/



// INCLUDE FILES 
#include <txtrich.h>
#include <msvids.h>
#include <badesca.h>
#include <msvstore.h>
#include <mtmuids.h> 
#include <mtclbase.h>
#include <mtmdef.h>
#include <msvftext.h>       // CMsvFindText 
#include <cmsvmimeheaders.h>
#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>

#include <msgtextutils.h>
#include <mmsgenutils.h>
#include <mmsattachmentwaiter.h>
#include <mmsattachmenthandler.h>

#include "UniMsvEntry.h"
#include "UniMtmPanic.h"
#include "UniHeaders.h"
#include "UniClientMtm.h"
#include "UniEditorUids.hrh"
#include "UniEditorLogging.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Factory function
// ---------------------------------------------------------
// 
EXPORT_C CUniClientMtm* CUniClientMtm::NewL(
    CRegisteredMtmDll& aRegisteredMtmDll,
    CMsvSession& aSession )
    {
    CUniClientMtm* self=new ( ELeave ) CUniClientMtm(
        aRegisteredMtmDll, aSession );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// Constructor
// ---------------------------------------------------------
// 
CUniClientMtm::CUniClientMtm(
    CRegisteredMtmDll& aRegisteredMtmDll,
    CMsvSession& aSession )
    : CBaseMtm( aRegisteredMtmDll, aSession ),
    iUniHeaders ( NULL )
    {
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//     
CUniClientMtm::~CUniClientMtm()
    {
    delete iAttaWaiter;
    delete iUniHeaders;
    delete iTextUtils;
    }

// ---------------------------------------------------------
// CUniClientMtm::CreateNewEntryL
// ---------------------------------------------------------
//
TMsvId CUniClientMtm::CreateNewEntryL( TMsvId aDestination )
    {
    UNILOGGER_WRITE_TIMESTAMP( "CUniClientMtm::CreateNewEntryL start" );
    // Create new message entry
    TMsvEntry entry;
    entry.iType = KUidMsvMessageEntry;
    entry.iServiceId = KMsvLocalServiceIndexEntryId;
    entry.iMtm = TUid::Uid( KUidUniMtm );
    entry.iDate.UniversalTime();

    // The following ones will be set in completion.
    entry.SetInPreparation( ETrue );    
    entry.SetVisible( EFalse );         
    entry.iSize = 0;
            
    // Check that sufficient disk space available
    // for index entry
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL(
            &Session().FileSession(), 
            sizeof( TMsvEntry ),
            Session().CurrentDriveL() ) )
        {
        // we use standard error code here
        User::Leave( KErrDiskFull );
        }

    SwitchCurrentEntryL( aDestination );
    iMsvEntry->CreateL( entry );
    SwitchCurrentEntryL( entry.Id() );
    
    UNILOGGER_WRITE_TIMESTAMP( "CUniClientMtm::CreateNewEntryL end" );
    
    return entry.Id();
    }

//----------------------------------------------------------
// METHODS FROM BASE CLASS
//----------------------------------------------------------

// ---------------------------------------------------------
// CUniClientMtm::SaveMessageL
// Stores the multimedia message
// ---------------------------------------------------------
//
void CUniClientMtm::SaveMessageL()
    {
    // First we should assert that iMsvEntry is not NULL, and panic, if it is
    __ASSERT_DEBUG( iMsvEntry!=NULL, Panic( EUniNotAMessage ));
    // SaveMessageL should only be supported for message entries.
    __ASSERT_DEBUG( iMsvEntry->Entry().iType.iUid == KUidMsvMessageEntryValue,
        Panic( EUniNotAMessage ) );

    CMsvStore* store = iMsvEntry->EditStoreL();
    CleanupStack::PushL( store );

    TMsvEntry indexEntry = iMsvEntry->Entry();

    SaveMessageL( *store, indexEntry ); 
    
    // Commit the stream store
    store->CommitL();
    CleanupStack::PopAndDestroy( store );

    // commit the index changes.
    iMsvEntry->ChangeL( indexEntry );
    }
    
// ---------------------------------------------------------
// CUniClientMtm::SaveMessageL
// Stores the multimedia message
// ---------------------------------------------------------
//
void CUniClientMtm::SaveMessageL( CMsvStore& aEditStore, TMsvEntry& aEntry ) 
    {
    // Store headers of a message

    // Because of attachments are handled using the new
    // attacment manager, the caller must store and commit the attachments
    // either one by one or after all have been added.
    // After saving all attachments the edit store used for that purpose
    // must be freed.
    // The store must be freed because all attachment info and uni headers
    // are saved in the actual message entry, there are no separate 
    // attachment entries anymore.
    
    // Check that sufficient disk space available
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
        &Session().FileSession(), 
        iUniHeaders->Size(),
        iMessageDrive ) )
        {
        User::Leave( KErrDiskFull );
        }
       
    // Note: Body text not supported.
    iUniHeaders->StoreL( aEditStore );
    
    // needed to convert unimessages to sms messages
    StoreBodyL( aEditStore ); 
    
    // attachment size
    TInt32 totalSizeOfAllAttachments = AttachmentsSizeL( aEditStore );
    aEntry.iSize = iUniHeaders->Size() + totalSizeOfAllAttachments;

    // If there are multiple recipients then set the flag
    if (( iUniHeaders->ToRecipients().Count() + //lint !e115 !e1013 !e48 !e10 !e1055 !e746 !e628
        iUniHeaders->CcRecipients().Count() +  //lint !e115 !e1013 !e48 !e10 !e1055 !e746 !e628
        iUniHeaders->BccRecipients().Count() ) >= 2 ) //lint !e115 !e1013 !e48 !e10 !e1055 !e746 !e628
        {
        aEntry.SetMultipleRecipients( ETrue );
        }
    else
        {
        // clear multiple recipients in case recipients have
        // been deleted after the message was saved the last time
        aEntry.SetMultipleRecipients( EFalse );
        }

    // Set iDetails (recipient)
    TPtrC to;
    if ( iUniHeaders->ToRecipients().Count() ) //lint !e115 !e1013 !e48 !e10
        {
        to.Set( TMmsGenUtils::Alias( iUniHeaders->ToRecipients()[0] ) ); //lint !e115 !e1013 !e48 !e10 !e64
        if ( to.Length() <= 0 )
            {
            // If no alias part then set the real address in details
            to.Set( iUniHeaders->ToRecipients()[0] ); //lint !e115 !e1013 !e48 !e10 !e1025 !e1703 !e64 !e118
            }
        }
    aEntry.iDetails.Set( to );
    
    if ( totalSizeOfAllAttachments > 0 )
        {
        aEntry.SetAttachment( ETrue );
        }
        
    switch ( iUniHeaders->MessageTypeLocking() )
        {
        case EUniMessageTypeLocked:
            TUniMsvEntry::SetMessageTypeLocked( aEntry, ETrue );
            break;
        case EUniMessageTypeNotLocked:
            TUniMsvEntry::SetMessageTypeLocked( aEntry, EFalse );
            break;
        case EUniMessageTypeLockingNotSet:
        default:
            // Don't touch TMsvEntry.
            break;
        }
    }

// ---------------------------------------------------------
// CUniClientMtm::LoadMessageL
// Loads the unified message
// ---------------------------------------------------------
//
void CUniClientMtm::LoadMessageL()
    {
    // First we should assert that iMsvEntry is not NULL, and panic, if it is
   	__ASSERT_DEBUG( iMsvEntry, Panic( EUniNotAMessage ) );
    // LoadMessageL should only be supported for message entries.
    __ASSERT_DEBUG( iMsvEntry->Entry().iType.iUid == KUidMsvMessageEntryValue,
        Panic( EUniNotAMessage ) );

    // Old data must be reset first....
    // CMmsSettings resets itself before doing restore, so that's ok.

    TMsvEntry indexEntry = iMsvEntry->Entry(); //lint !e1924
    

    // load the correct data
    // get read-only message store
    CMsvStore* store = iMsvEntry->ReadStoreL();
    CleanupStack::PushL( store );

    // Restore headers of unified message
    // if headers do not exist does nothing. Default headers will be used.
    iUniHeaders->RestoreL( *store );

    if ( iUniHeaders->MessageTypeLocking() == EUniMessageTypeLockingNotSet &&
        TUniMsvEntry::IsMessageTypeLocked( indexEntry ) )
        {
        switch ( TUniMsvEntry::CurrentMessageType( indexEntry ) )
            {
            case EUniMessageCurrentTypeSms:
                {
                iUniHeaders->SetMessageTypeSetting( EUniMessageTypeSettingSms );
                iUniHeaders->SetMessageTypeLocking( EUniMessageTypeLocked );
                break;
                }
            case EUniMessageCurrentTypeMms:
                {
                iUniHeaders->SetMessageTypeSetting( EUniMessageTypeSettingMms );
                iUniHeaders->SetMessageTypeLocking( EUniMessageTypeLocked );
                break;
                }
            default:
                break;
            }
        }

    // Attachment info is not restored.
    // It makes no sense to cache the attachment info as new attachments
    // can be added with the help of the attachment magager without 
    // informing uni Client MTM of the additions.
    // Caller must use attachment manager to get attachment info.
    
    CleanupStack::PopAndDestroy( store );  
    store = NULL;

    // Build the iAddresseeList up
    BuildAddresseeListL();
    }

// ---------------------------------------------------------
// CUniClientMtm::ReplyL
// Send a reply to current message
// ---------------------------------------------------------
//
CMsvOperation* CUniClientMtm::ReplyL(
    TMsvId /*aDestination*/,
    TMsvPartList /*aPartList*/,
    TRequestStatus& /*aCompletionStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CUniClientMtm::ForwardL
// Forward current message
// ---------------------------------------------------------
//
CMsvOperation* CUniClientMtm::ForwardL(
    TMsvId /*aDestination*/,
    TMsvPartList /*aPartList*/,
    TRequestStatus& /*aCompletionStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CUniClientMtm::ValidateMessage
// Validate selected parts of current message
// ---------------------------------------------------------
//
TMsvPartList CUniClientMtm::ValidateMessage(
    TMsvPartList aPartList )
    {
    __ASSERT_DEBUG( iMsvEntry, Panic( EUniNotAMessage ));
    
    TMsvPartList retVal = 0;
    if ( iMsvEntry->Entry().iType.iUid != KUidMsvMessageEntryValue )
        {
        // not a message, no part is valid
        retVal = aPartList;
        }

    if ( aPartList & KMsvMessagePartRecipient )
        {
        if ( iAddresseeList->Count() == 0)
            {
            retVal |= KMsvMessagePartRecipient;
            }
        else
            {
            // check the recipient list for valid 'addresses'
            for (TInt ii=0; ii < iAddresseeList->Count(); ++ii)
                {
                TPtrC oneAddress = (*iAddresseeList)[ii];
                TPtrC pureAddress = TMmsGenUtils::PureAddress( oneAddress );
                if ( ( pureAddress.Length() == 0 ) || 
                    !TMmsGenUtils::IsValidAddress( pureAddress, ETrue ) )
                    {
                    retVal |= KMsvMessagePartRecipient;
                    break;
                    }
                }
            }
        }

    // all attachments are considered valid - even no attachments

    return retVal;

    }

// ---------------------------------------------------------
// CUniClientMtm::Find
// Find text in selected message parts
// ---------------------------------------------------------
//
TMsvPartList CUniClientMtm::Find(
    const TDesC& aTextToFind,
    TMsvPartList aPartList )
	{
	TMsvPartList foundList( KMsvMessagePartNone );
	TRAP_IGNORE( foundList = DoFindL( aTextToFind, aPartList) );
	return foundList;
	}

// ---------------------------------------------------------
// CUniClientMtm::DoFindL
// Find text in selected message parts
// ---------------------------------------------------------
//
TMsvPartList CUniClientMtm::DoFindL(
    const TDesC& aTextToFind,
    TMsvPartList aPartList )
    {
    // The final version will not have a rich text body, but we could
    // search for example the originator and description.

    __ASSERT_DEBUG( iMsvEntry, Panic( EUniNotAMessage ) );
    TMsvPartList foundList = KMsvMessagePartNone;
    TMsvEntry entry = iMsvEntry->Entry();

    CMsvFindText* findText = CMsvFindText::NewL();
    CleanupStack::PushL( findText );

    if ( aPartList & KMsvMessagePartRecipient )
        {
        // Find from To, Cc and Bcc fields
        if ( FindInRecipientL( aTextToFind, 
            aPartList, iUniHeaders->ToRecipients(), *findText))
            {
            foundList |= KMsvMessagePartRecipient;
            }
        else if ( FindInRecipientL( aTextToFind, 
            aPartList, iUniHeaders->CcRecipients(), *findText ))
            {
            foundList |= KMsvMessagePartRecipient;
            }
        else if ( FindInRecipientL( aTextToFind, 
            aPartList, iUniHeaders->BccRecipients(), *findText ))
            {
            foundList |= KMsvMessagePartRecipient;
            }
        }
    
    if ( aPartList & KMsvMessagePartDescription )
        {
        if ( findText->FindTextL( aTextToFind, entry.iDescription, 
            aPartList ) )
            {
            foundList |= KMsvMessagePartDescription;
            }
        }

    CleanupStack::PopAndDestroy( findText );
    return foundList;
    }

// ---------------------------------------------------------
// CUniClientMtm::AddAddresseeL
// ---------------------------------------------------------
//
void CUniClientMtm::AddAddresseeL( const TDesC& aRealAddress )
    {
    // Add to general list
    // When no type is specified, the address will have type "to"
    iAddresseeList->AppendL( EMsvRecipientTo, aRealAddress );
    
    // Add to "To" recipient list
    iUniHeaders->AddTypedAddresseeL( aRealAddress, EMsvRecipientTo ); //lint !e115 !e1013 !e48 !e10 !e1514 !e64
    }

// ---------------------------------------------------------
// CUniClientMtm::AddAddresseeL
// ---------------------------------------------------------
//
void CUniClientMtm::AddAddresseeL(
    const TDesC& aRealAddress,
    const TDesC& aAlias )
    {

    if ( aAlias.Length() > 0 )
        {
        HBufC* buf = TMmsGenUtils::GenerateAddressL( aRealAddress, aAlias );
        CleanupStack::PushL( buf );
        AddAddresseeL( buf->Des() );
        CleanupStack::PopAndDestroy( buf );  
        }
    else
        {        
        AddAddresseeL( aRealAddress );
        }
    }
    
// ---------------------------------------------------------
// CUniClientMtm::AddAddresseeL
// ---------------------------------------------------------
//
void CUniClientMtm::AddAddresseeL( TMsvRecipientType aType,
    const TDesC& aRealAddress )
    {
    // Add to general list
    // When no type is specified, the address will have type "to",
    // "cc" or "bcc"
    iAddresseeList->AppendL( aType, aRealAddress );
    
    // Add to recipient list
    iUniHeaders->AddTypedAddresseeL( aRealAddress, aType );

    }

// ---------------------------------------------------------
// CUniClientMtm::AddAddresseeL
// ---------------------------------------------------------
//
void CUniClientMtm::AddAddresseeL(
    TMsvRecipientType aType,
    const TDesC& aRealAddress,
    const TDesC& aAlias )
    {

    if ( aAlias.Length() > 0 )
        {
        HBufC* buf = TMmsGenUtils::GenerateAddressL( aRealAddress, aAlias );
        CleanupStack::PushL( buf );
        AddAddresseeL(aType, buf->Des());
        CleanupStack::PopAndDestroy( buf ); 
        }
    else
        {        
        AddAddresseeL( aType, aRealAddress );
        }
    }

// ---------------------------------------------------------
// CUniClientMtm::RemoveAddressee
// ---------------------------------------------------------
//
void CUniClientMtm::RemoveAddressee( TInt aIndex )
    {

	if ( aIndex < 0 )
		{
		return;	
		}
    if ( iAddresseeList->Count() > aIndex )
        {
        // Delete from typed list
        TPtrC address = (*iAddresseeList)[ aIndex ];
        iUniHeaders->RemoveAddressee( address );
        // delete from untyped list
        iAddresseeList->Delete( aIndex );
        }
        
    }

// ---------------------------------------------------------
// CUniClientMtm::SetSubjectL
// ---------------------------------------------------------
//
void CUniClientMtm::SetSubjectL( const TDesC& aSubject )
    {
    iUniHeaders->SetSubjectL( aSubject ); //lint !e115 !e1013 !e48 !e10 !e1514 !e64
    }

// ---------------------------------------------------------
// CUniClientMtm::SubjectL
// ---------------------------------------------------------
//
const TPtrC CUniClientMtm::SubjectL() const
    {
    return iUniHeaders->Subject(); //lint !e115 !e1013 !e48 !e10 !e746 !e64 !e1055 !e628
    }

// ---------------------------------------------------------
// CUniClientMtm::AddAttachmentL
// ---------------------------------------------------------
//
void CUniClientMtm::AddAttachmentL( const TDesC& aFilePath,
    const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus )
    {
    
    TInt error = KErrNone;
    TUint charset = aCharset;
    RFile file;
    if ( aMimeType.CompareF( KMmsTextPlain ) == 0 && charset == 0 )
        {
        // try to recognize character set
        // We trap the recognization process
        // If cannot recognize, the result will be 0 and default to us-ascii
        TRAP ( error,
            {
            error = file.Open( Session().FileSession(), aFilePath,
                EFileRead|EFileShareReadersOnly );
            if ( error == KErrNone )
                {
                CleanupClosePushL( file );
                charset = RecognizeCharSetL( file );
                CleanupStack::PopAndDestroy( &file ); 
                }
            }
            );
        }
        
    // if the attachment character set is unicode, it should be converted to utf-8
    // (see MMS conformance document)
    
    if ( aMimeType.CompareF( KMmsTextPlain ) == 0 &&
        ( charset == KCharacterSetMIBEnumIso10646Ucs2  ||
        charset == KCharacterSetMIBEnumUTF16 ||
        charset == KCharacterSetMIBEnumUTF16BE ||
        charset == KCharacterSetMIBEnumUTF16LE ) )
        {
        // If we have unicode character set, we must convert the file to utf8
        error = file.Open( Session().FileSession(), aFilePath,
            EFileRead|EFileShareReadersOnly );
        if ( error == KErrNone )
            {
            CleanupClosePushL( file );
              CMsvStore* store = iMsvEntry->EditStoreL();
            CleanupStack::PushL( store );
            TMsvAttachmentId attaId = 0;
            CMmsAttachmentHandler::CreateUTF8TextAttachmentFromFileL(
                *store, attaId, file, Session().FileSession(),
                Session().CurrentDriveL() );
                
            SetContentLocationForConvertedAttL( *store, attaId, aFilePath ); 
                
            store->CommitL();
            CleanupStack::PopAndDestroy( store );
            CleanupStack::PopAndDestroy( &file );  
            }
        TRequestStatus* status = &aStatus;
        aStatus = KRequestPending;
        User::RequestComplete( status, error );
        return;    
        }
    else
        {
        // Disk space is checked in AddFilePathAttachmentL after everything has been initialized
        AddFilePathAttachmentL( aFilePath, aMimeType, CMsvAttachment::EMsvFile, aStatus, charset );
        }
    }
    
// ---------------------------------------------------------
// CUniClientMtm::AddAttachmentL
// ---------------------------------------------------------
//
void CUniClientMtm::AddAttachmentL( RFile& aFile, const TDesC8& aMimeType,
    TUint aCharset, TRequestStatus& aStatus )
    {
    TInt size = 0;
    User::LeaveIfError( aFile.Size( size ) );

    TFileName fileName;
    User::LeaveIfError( aFile.Name( fileName ) );

    TInt charset = aCharset;
    if ( aMimeType.CompareF( KMmsTextPlain ) == 0  && charset == 0 )
        {
        // If no character set defined for a plain text attachment
        // we try to recognize the character set.
        // But if recogization fails, we say 0 (us-ascii)
        TRAP_IGNORE ( charset = RecognizeCharSetL( aFile ) );
        }

    if ( aMimeType.CompareF( KMmsTextPlain ) == 0 )
        {
        // If we have unicode character set, we must convert the file to utf8
        if ( charset == KCharacterSetMIBEnumIso10646Ucs2  ||
            charset == KCharacterSetMIBEnumUTF16 ||
            charset == KCharacterSetMIBEnumUTF16BE ||
            charset == KCharacterSetMIBEnumUTF16LE ) 
            {
            CMsvStore* store = iMsvEntry->EditStoreL();
            CleanupStack::PushL( store );
            TMsvAttachmentId attaId = 0;
            CMmsAttachmentHandler::CreateUTF8TextAttachmentFromFileL( *store,
                attaId, aFile, Session().FileSession(),
                Session().CurrentDriveL() );
              
            SetContentLocationForConvertedAttL( *store, attaId, fileName );    

            store->CommitL();
            CleanupStack::PopAndDestroy( store ); // store
            // We must close the file handle because the attachment manager will also
            // close the handle.
            // The open file handle is always closed unless the funtion leaves
            aFile.Close();
            TRequestStatus* status = &aStatus;
            aStatus = KRequestPending;
            User::RequestComplete( status, KErrNone );
            return;
            }
        }
    
    if( !iAttaWaiter )
        {
        iAttaWaiter = CMmsAttachmentWaiter::NewL();
        }

    // store must be the first item allocated because it is the last one to be popped
    CMsvStore* store = iMsvEntry->EditStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& manager = store->AttachmentManagerL();
    
    CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( attachment );

    size += InitializeAttachmentL(
        manager,
        *attachment,
        fileName,
        aMimeType,
        size,
        charset );
    
    // Check that sufficient disk space available
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
        &Session().FileSession(), 
        size,
        iMessageDrive ) )
            {
            User::Leave( KErrDiskFull );
            }

    // attachment is initialised, pass to the attachment manager
    if ( iAttaWaiter->IsActive() )
        {
        // can't start an active operation because already active
        User::Leave( KErrInUse );
        }
    manager.AddAttachmentL( aFile, attachment, iAttaWaiter->iStatus ); 
    CleanupStack::Pop( attachment ); //ownership passed to manager
    // We cannot start waiting before we know that the function we are waiting for
    // did not leave. If we become active, and the function leaves, we are in trouble    
    iAttaWaiter->StartWaitingL( aStatus, store, &manager );
    CleanupStack::Pop( store ); //ownership passed
    }
// ---------------------------------------------------------
// CUniClientMtm::AddLinkedAttachmentL
// ---------------------------------------------------------
//
void CUniClientMtm::AddLinkedAttachmentL( const TDesC& aFilePath,
    const TDesC8& aMimeType, TUint aCharset, TRequestStatus& aStatus )
    {
    TInt error = KErrNone;
    TUint charset = aCharset;
    if ( aMimeType.CompareF( KMmsTextPlain ) == 0 && aCharset == 0)
        {
        // try to recognize character set
        // We trap the recognization process
        // If cannot recognize, the result will be 0 and default to us-ascii
        TRAP ( error,
            {
            RFile file;
            error = file.Open( Session().FileSession(), aFilePath,
                EFileRead|EFileShareReadersOnly);
            if ( error == KErrNone )
                {
                CleanupClosePushL( file );
                charset = RecognizeCharSetL( file );
                CleanupStack::PopAndDestroy( &file );  
                }
            }
            );
        }
    // Linked files cannot be converted to utf8. They must be sent as is no
    // matter what the character set is.
    
    // Disk space is checked in AddFilePathAttachmentL after everything has
    // been initialized.
    if ( aMimeType.CompareF( KMmsTextPlain ) == 0 &&
        ( charset == KCharacterSetMIBEnumIso10646Ucs2  ||
        charset == KCharacterSetMIBEnumUTF16 ||
        charset == KCharacterSetMIBEnumUTF16BE ||
        charset == KCharacterSetMIBEnumUTF16LE ) )
        {
        AddAttachmentL( aFilePath, aMimeType, charset, aStatus );
        }
    else
        {
        AddFilePathAttachmentL( aFilePath, aMimeType,
            CMsvAttachment::EMsvLinkedFile, aStatus, charset );
        }
    }

// ---------------------------------------------------------
// CUniClientMtm::AddEntryAsAttachmentL
// ---------------------------------------------------------
//
void CUniClientMtm::AddEntryAsAttachmentL( TMsvId /*aAttachmentId*/,
    TRequestStatus& aStatus )
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNotSupported );
    } 

// ---------------------------------------------------------
// CUniClientMtm::CreateAttachmentL
// ---------------------------------------------------------
//
void CUniClientMtm::CreateAttachmentL( const TDesC& aFileName,
    RFile& aAttachmentFile, const TDesC8& aMimeType,
    TUint aCharset, TRequestStatus& aStatus )
    {
    if( !iAttaWaiter )
        {
        iAttaWaiter = CMmsAttachmentWaiter::NewL();
        }

    // store must be the first item allocated because it is the last one to be popped
    CMsvStore* store = iMsvEntry->EditStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& manager = store->AttachmentManagerL();
    
    CMsvAttachment* attachment = 
        CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( attachment );
    
    TInt size = InitializeAttachmentL(
        manager,
        *attachment,
        aFileName,
        aMimeType,
        0, // Creating empty attachment!
        aCharset );

    // Check that sufficient disk space available
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
        &Session().FileSession(), 
        size,
        iMessageDrive ) )
            {
            User::Leave( KErrDiskFull );
            }

    if ( iAttaWaiter->IsActive() )
        {
        // can't start an active operation because already active
        User::Leave( KErrInUse );
        }
    manager.CreateAttachmentL( aFileName, aAttachmentFile, attachment,
        iAttaWaiter->iStatus );
    CleanupStack::Pop( attachment ); //ownership passed to manager
    iAttaWaiter->StartWaitingL( aStatus, store, &manager );
    CleanupStack::Pop( store ); //ownership passed
    }
// ---------------------------------------------------------
// CUniClientMtm::CancelAttachmentOperation
// ---------------------------------------------------------
//
void CUniClientMtm::CancelAttachmentOperation()
    {
    
    if ( iAttaWaiter )
        {
        iAttaWaiter->Cancel();
        delete iAttaWaiter;
        iAttaWaiter = NULL;
        }
    }

    
// ---------------------------------------------------------
// CUniClientMtm::CreateTextAttachmentL
// ---------------------------------------------------------
//
void CUniClientMtm::CreateTextAttachmentL(
            CMsvStore& aStore,
            TMsvAttachmentId& aAttachmentId,
            const TDesC& aText,
            const TDesC& aFile,
            TBool aConvertParagraphSeparator )
    {

    CMmsAttachmentHandler::CreateTextAttachmentL( aStore,
        aAttachmentId,
        aText,
        aFile,
        Session().FileSession(),
        Session().CurrentDriveL(),
        aConvertParagraphSeparator );
 
    }

// ---------------------------------------------------------
// CUniClientMtm::MessageTypeSetting
// ---------------------------------------------------------
//
TUniMessageTypeSetting CUniClientMtm::MessageTypeSetting() const
    {
    return TUniMessageTypeSetting( iUniHeaders->MessageTypeSetting() );
    }

// ---------------------------------------------------------
// CUniClientMtm::SetMessageTypeSetting
// ---------------------------------------------------------
//
void CUniClientMtm::SetMessageTypeSetting( TUniMessageTypeSetting aSetting )
    {
    iUniHeaders->SetMessageTypeSetting( aSetting );
    }

// ---------------------------------------------------------
// CUniClientMtm::MessageTypeLocking
// ---------------------------------------------------------
//
TUniMessageTypeLocking CUniClientMtm::MessageTypeLocking() const
    {
    return TUniMessageTypeLocking( iUniHeaders->MessageTypeLocking() );
    }

// ---------------------------------------------------------
// CUniClientMtm::SetMessageTypeLocking
// ---------------------------------------------------------
//
void CUniClientMtm::SetMessageTypeLocking( TUniMessageTypeLocking aLocking )
    {
    iUniHeaders->SetMessageTypeLocking( aLocking );
    }

// ---------------------------------------------------------
// CUniClientMtm::MessageRoot
// ---------------------------------------------------------
//
TMsvAttachmentId CUniClientMtm::MessageRoot() const
    {
    return iUniHeaders->MessageRoot();
    }

// ---------------------------------------------------------
// CUniClientMtm::SetMessageRoot
// ---------------------------------------------------------
//
void CUniClientMtm::SetMessageRoot( TMsvAttachmentId aRoot )
    {
    iUniHeaders->SetMessageRoot( aRoot );
    }

// ---------------------------------------------------------
// CUniClientMtm::CreateMessageL
// ---------------------------------------------------------
//
void CUniClientMtm::CreateMessageL(
    TMsvId aServiceId )
    {
    // Check that sufficient disk space available
    // for index entry
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
        &Session().FileSession(), 
        KMmsIndexEntryExtra,
        iMessageDrive ) )
            {
            // we use standard error code here
            User::Leave( KErrDiskFull );
            }

    // just call the base class function
    CBaseMtm::CreateMessageL( aServiceId );
    
    iUniHeaders->Reset();
    }
    
// ---------------------------------------------------------
// CUniClientMtm::BioTypeChangedL
// ---------------------------------------------------------
//
void CUniClientMtm::BioTypeChangedL( TUid /*aBioTypeUid*/ )
    {
    // Do nothing.
    }
    
// ---------------------------------------------------------
// CUniClientMtm::DefaultServiceL
// ---------------------------------------------------------
//
TMsvId CUniClientMtm::DefaultServiceL() const
    {
    return KMsvLocalServiceIndexEntryId;
    }

// ---------------------------------------------------------
// CUniClientMtm::RemoveDefaultServiceL
// ---------------------------------------------------------
//
void CUniClientMtm::RemoveDefaultServiceL()
    {
    // not supported
    }

// ---------------------------------------------------------
// CUniClientMtm::ChangeDefaultServiceL
// ---------------------------------------------------------
//
void CUniClientMtm::ChangeDefaultServiceL( const TMsvId& /*aService*/ )
    {
    // not supported
    }

// ---------------------------------------------------------
// CUniClientMtm::QueryCapability
// ---------------------------------------------------------
//
TInt CUniClientMtm::QueryCapability(
    TUid aCapability,
    TInt& aResponse )
    {
    TInt error = KErrNone;
    switch ( aCapability.iUid )
        {
        // Supported:
        case KUidMtmQueryMaxTotalMsgSizeValue:
            aResponse = KMaxTInt;
            break;
        case KUidMsvMtmQueryEditorUidValue:
            aResponse = KUidMsgMmsEditor;
            break;
        case KUidMtmQuerySupportSubjectValue:
        case KUidMtmQuerySupportAttachmentsValue:
        case KUidMtmQueryCanSendMsgValue:
        case KUidMtmQuerySendAsMessageSendSupportValue:
        case KUidMtmQuerySupportsRecipientTypeValue:
            // returns KErrNone
            break;
        // All others - Not Supported:
        default:
            error = KErrNotSupported;
        }
    return error;

    } //lint !e1746

// ---------------------------------------------------------
// CUniClientMtm::InvokeSyncFunctionL
// ---------------------------------------------------------
//
void CUniClientMtm::InvokeSyncFunctionL(
    TInt /*aFunctionId*/,
    const CMsvEntrySelection& /*aSelection*/,
    TDes8& /*aParameter*/ )
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------
// CUniClientMtm::InvokeAsyncFunctionL
// ---------------------------------------------------------
//
CMsvOperation* CUniClientMtm::InvokeAsyncFunctionL(
    TInt /* aFunctionId */,
    const CMsvEntrySelection& /* aSelection*/,
    TDes8& /* aParameter*/,
    TRequestStatus& aCompletionStatus )
    {
    TPckgC < TMsvId > progress = 0;
    return  CMsvCompletedOperation::NewL( Session(), Type(), progress, 
            KMsvLocalServiceIndexEntryId, aCompletionStatus, KErrNotSupported );
    }

// ---------------------------------------------------------
// CUniClientMtm::ContextEntrySwitched
// ---------------------------------------------------------
//
void CUniClientMtm::ContextEntrySwitched()
    {
    // Context change notification.
    // Reset data.

    // Note: Body text reset would be here if supported.
    iAddresseeList->Reset();
    iUniHeaders->Reset();
    }

// ---------------------------------------------------------
// CUniClientMtm::HandleEntryEventL
// ---------------------------------------------------------
//
void CUniClientMtm::HandleEntryEventL(
    TMsvEntryEvent /*aEvent*/,
    TAny* /*arg1*/,
    TAny* /*arg2*/,
    TAny* /*arg3*/ )
    {
    // No operation
    }

// ---------------------------------------------------------
// CUniClientMtm::ConstructL
// ---------------------------------------------------------
//
void CUniClientMtm::ConstructL()
    {
    iMessageDrive = EDriveC;
    TInt error = KErrNone;

    TRAP ( error, { iMessageDrive = Session().CurrentDriveL(); } );
    
    if ( error != KErrNone )
        {
        // if cannot ask, use default
        iMessageDrive = EDriveC;
        }

    iUniHeaders = CUniHeaders::NewL();
    }

// ---------------------------------------------------------
// CUniClientMtm::MessageSize
// ---------------------------------------------------------
//
TInt32 CUniClientMtm::MessageSize()
    {
    // First we should assert that iMsvEntry is not NULL, and panic, if it is
    __ASSERT_DEBUG( iMsvEntry, Panic( EUniNotAMessage ));
    TUint size = 0;
    TRAPD( error, {size = AttachmentsSizeL() + iUniHeaders->Size();} );
    if ( error != KErrNone ) // this is needed to get rid of compiler warning
        {
        size = 0;
        }
    return size;  //lint !e713 nothing lost here
    }

// ---------------------------------------------------------
// CUniClientMtm::SetMessageDescriptionL
// ---------------------------------------------------------
//
void CUniClientMtm::SetMessageDescriptionL( const TDesC& aText )
    {
    // First we should assert that iMsvEntry is not NULL, and panic, if it is
    __ASSERT_DEBUG( iMsvEntry, Panic( EUniNotAMessage ) );
    TMsvEntry entry = iMsvEntry->Entry();
    entry.iDescription.Set( aText );
    iMsvEntry->ChangeL( entry );
    }
    
// ---------------------------------------------------------
// CUniClientMtm::BuildAddresseeListL
// ---------------------------------------------------------
//
void CUniClientMtm::BuildAddresseeListL() 
    {

    iAddresseeList->Reset();

    const CDesCArray& array1 = iUniHeaders->ToRecipients(); //lint !e115 !e1013 !e48 !e10 !e64 
    const CDesCArray& array2 = iUniHeaders->CcRecipients(); //lint !e115 !e1013 !e48 !e10 !e64
    const CDesCArray& array3 = iUniHeaders->BccRecipients(); //lint !e115 !e1013 !e48 !e10 !e64

    BuildAddresseeListL( array1, EMsvRecipientTo );
    BuildAddresseeListL( array2, EMsvRecipientCc );
    BuildAddresseeListL( array3, EMsvRecipientBcc );

    }

// ---------------------------------------------------------
// CUniClientMtm::BuildAddresseeListL
// ---------------------------------------------------------
//
void CUniClientMtm::BuildAddresseeListL(
    const CDesCArray& aArray, TMsvRecipientType aValue ) 
    {

    TInt size;
    size = aArray.Count();
    for ( TInt i=0; i < size; i++ )
        {
        iAddresseeList->AppendL( aValue, aArray[i] );
        }
    }

// ---------------------------------------------------------
// CUniClientMtm::AttachmentsSizeL
// ---------------------------------------------------------
//
TInt32 CUniClientMtm::AttachmentsSizeL()
    {
    CMsvStore* store = iMsvEntry->ReadStoreL();
    CleanupStack::PushL( store );
    TInt32 attaSize = AttachmentsSizeL( *store );
    CleanupStack::PopAndDestroy( store );
    return attaSize;
    }

// ---------------------------------------------------------
// CUniClientMtm::AttachmentsSizeL
// ---------------------------------------------------------
//
TInt32 CUniClientMtm::AttachmentsSizeL( CMsvStore& aStore )
    {
    TInt32 size = 0;

    MMsvAttachmentManager& attachMan = aStore.AttachmentManagerL();
    TInt numAttachments = attachMan.AttachmentCount();

    for ( TInt i = 0; i < numAttachments; i++ )
        {
        CMsvAttachment* attachmentInfo = attachMan.GetAttachmentInfoL( i );
        CleanupStack::PushL( attachmentInfo );
        
        CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
        CleanupStack::PushL( mimeHeaders );
        
        mimeHeaders->RestoreL( *attachmentInfo );
        
        RFile attaFile = attachMan.GetAttachmentFileL( i );
        CleanupClosePushL( attaFile );
        TInt fileSize = 0;
        
        // If we cannot access the file, we are in trouble
        User::LeaveIfError( attaFile.Size( fileSize ) ); 
        
        // This adds up mime header size + actual attachment binary data
        size += mimeHeaders->Size() + fileSize;

        CleanupStack::PopAndDestroy( &attaFile );
        CleanupStack::PopAndDestroy( mimeHeaders );
        CleanupStack::PopAndDestroy( attachmentInfo );
        }

    return size;
    }

// ---------------------------------------------------------
// CUniClientMtm::FindInRecipientL
// ---------------------------------------------------------
//
TBool CUniClientMtm::FindInRecipientL( 
    const TDesC& aTextToFind,
    TMsvPartList aPartlist,
    const CDesCArray& aRecipients,
    CMsvFindText& aFindText )
    {
    TInt count = aRecipients.Count();
    TBool found = EFalse;
    for  (TInt i=0; i < count; i++ )
        {
        // Check alias and real address parts
        // separately. Otherwise separator character could 
        // spoil the check.
        if ( aFindText.FindTextL( aTextToFind, 
            TMmsGenUtils::Alias( aRecipients[i] ), aPartlist ) )
            {
            found = ETrue;
            break;
            }
        else if ( aFindText.FindTextL( aTextToFind, 
            TMmsGenUtils::PureAddress( aRecipients[i] ), aPartlist ) )
            {
            found = ETrue;
            break;
            }
        }
    return found;
    }

// ---------------------------------------------------------
// CUniClientMtm::AddFilePathAttachmentL
// ---------------------------------------------------------
//
void CUniClientMtm::AddFilePathAttachmentL(
    const TDesC& aFilePath,
    const TDesC8& aMimeType,
    CMsvAttachment::TMsvAttachmentType aType,
    TRequestStatus& aStatus,
    const TUint aCharacterSet /* = 0 */ )
	{
    __ASSERT_DEBUG( aType != CMsvAttachment::EMsvMessageEntry,
        User::Invariant() );

    TEntry fileEntry;
    User::LeaveIfError( Session().FileSession().Entry( aFilePath, fileEntry ) );

    if( !iAttaWaiter )
        {
        iAttaWaiter = CMmsAttachmentWaiter::NewL();
        }
    
    // store must be the first item allocated because it is the last one to be popped
    CMsvStore* store = iMsvEntry->EditStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& manager = store->AttachmentManagerL();
    
    CMsvAttachment* attachment = CMsvAttachment::NewL( aType );
    CleanupStack::PushL( attachment );

    TInt size = InitializeAttachmentL(
        manager,
        *attachment,
        aFilePath,
        aMimeType,
        fileEntry.iSize,
        aCharacterSet );
        
    // now we know how much disk space we need
    if ( aType == CMsvAttachment::EMsvFile )
        {
        size += attachment->Size();
        }
        
    // Check that sufficient disk space available
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
        &Session().FileSession(), 
        size,
        iMessageDrive ) )
            {
            // we use standard error code here
            User::Leave( KErrDiskFull );
            }
    
    // attachment is initialised, pass to the attachment manager
    if ( iAttaWaiter->IsActive() )
        {
        // can't start an active operation because already active
        User::Leave( KErrInUse );
        }
    switch( aType )
        {
        case CMsvAttachment::EMsvLinkedFile:
            manager.AddLinkedAttachmentL( aFilePath, attachment,
                iAttaWaiter->iStatus );
            break;
        default: // CMsvAttachment::EMsvFile
            manager.AddAttachmentL( aFilePath, attachment,
                iAttaWaiter->iStatus );
            break;
        }

    CleanupStack::Pop( attachment); //ownership passed to manager
    // We cannot start waiting before we know that the function we are waiting for
    // did not leave. If we become active, and the function leaves, we are in trouble    
    iAttaWaiter->StartWaitingL( aStatus, store, &manager );
    CleanupStack::Pop( store ); // ownership passed to iAttaWaiter
    }
    
// ---------------------------------------------------------
// CUniClientMtm::RecognizeCharSetL
// ---------------------------------------------------------
//
TUint CUniClientMtm::RecognizeCharSetL( RFile& aFile )
    {
    TUint charSet = CMsgTextUtils::RecognizeCharSetL( Session().FileSession(), aFile );
    if ( !iTextUtils )
        {
        iTextUtils = CMsgTextUtils::NewL( Session().FileSession() );
        }
    return iTextUtils->CharconvIdToMibIdL( charSet );
    }
    
// ---------------------------------------------------------
// CUniClientMtm::InitializeAttachmentL
// ---------------------------------------------------------
//
TInt CUniClientMtm::InitializeAttachmentL(
    MMsvAttachmentManager& aManager,
    CMsvAttachment& aAttachment,
    const TDesC& aFileName,
    const TDesC8& aMimeType,
    TInt aFileSize,
    TUint aCharset )
    {
    CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
    CleanupStack::PushL( mimeHeaders );
    
    aAttachment.SetSize( aFileSize );
    
    // set the mime-type if provided
    if ( aMimeType.Length() > 0 )
        {
        aAttachment.SetMimeTypeL( aMimeType );
        TInt position = aMimeType.Find( KMmsSlash8 );
        if ( position > 0 )
            {
            mimeHeaders->SetContentTypeL( aMimeType.Left( position ) );
            }
        if ( position < aMimeType.Length() - 1 )
            {
            mimeHeaders->SetContentSubTypeL( aMimeType.Mid( position + 1 ) );
            }
        }

    TParsePtrC parse( aFileName );
    mimeHeaders->SetSuggestedFilenameL( parse.NameAndExt() );
    HBufC* contentLocation = CMsgTextUtils::GetSafeAttachmentNameLC(
        aManager,
        parse.NameAndExt(),
        aAttachment.Id(),
        ETrue );
	mimeHeaders->SetContentLocationL( *contentLocation );
    CleanupStack::PopAndDestroy( contentLocation );

    mimeHeaders->SetMimeCharset( aCharset );

    TInt size = KMmsIndexEntryExtra + mimeHeaders->Size();    
    mimeHeaders->StoreL( aAttachment );
    
    // mime headers have been streamed to CMsvAttachment, they can go now
    CleanupStack::PopAndDestroy( mimeHeaders );  
    return size;
    }
    
// ---------------------------------------------------------
// CUniClientMtm::SetContentLocationForConvertedAttL
// ---------------------------------------------------------
//
void CUniClientMtm::SetContentLocationForConvertedAttL(
    CMsvStore& aStore,
    TMsvAttachmentId aAttaId,
    const TDesC& aFileName )
    {
    MMsvAttachmentManager& manager = aStore.AttachmentManagerL();    
    CMsvAttachment* attachment = manager.GetAttachmentInfoL( aAttaId );    
    CleanupStack::PushL( attachment );
    
    CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
    CleanupStack::PushL( mimeHeaders );
    mimeHeaders->RestoreL( *attachment );
    
    TParsePtrC parse( aFileName );
    HBufC* contentLocation = CMsgTextUtils::GetSafeAttachmentNameLC(
        manager,
        parse.NameAndExt(),
        aAttaId,
        ETrue );
    mimeHeaders->SetContentLocationL( *contentLocation );
    CleanupStack::PopAndDestroy( contentLocation );
    
    mimeHeaders->StoreL( *attachment );
    // Mime headers have been streamed into the attachment info
    CleanupStack::PopAndDestroy( mimeHeaders );

    MMsvAttachmentManagerSync& syncManager = aStore.AttachmentManagerExtensionsL();
        
    syncManager.ModifyAttachmentInfoL( attachment );
    // Ownership transferred
    CleanupStack::Pop( attachment );
    }
    

//  End of File
