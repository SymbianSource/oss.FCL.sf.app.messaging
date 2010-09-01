/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CUniEditorVCardOperationn, operation for adding VCard into the message
*
*/



// ========== INCLUDE FILES ================================

#include <MsgMediaInfo.h>
#include <contactmatcher.h>
#include <msgtextutils.h>
#include <MsgMediaResolver.h>
#include <MPbk2ContactNameFormatter.h>

#include "UniEditorLogging.h"
#include "UniEditorVCardOperation.h"
#include "UniEditorEnum.h"

_LIT8( KVCardMiMeType, "text/X-vCard" );
_LIT( KTempVCardFileName, "vCard.vcf" );
_LIT( KTempVCardFileSuffix, ".vcf");

_LIT(KInvalidFileNameChars, "?*<>/\"|\\:");
_LIT(KReplaceChar, "_");

// ---------------------------------------------------------
// CUniEditorVCardOperation::NewL
//
// Factory method.
// ---------------------------------------------------------
//
CUniEditorVCardOperation* CUniEditorVCardOperation::NewL(
        MUniEditorOperationObserver& aObserver,
        CUniEditorDocument& aDocument,
        RFs& aFs )
    {
    CUniEditorVCardOperation* self = new ( ELeave ) CUniEditorVCardOperation(
        aObserver, 
        aDocument, 
        aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CUniEditorVCardOperation::CUniEditorVCardOperation
//
// Constructor.
// ---------------------------------------------------------
//
CUniEditorVCardOperation::CUniEditorVCardOperation(
        MUniEditorOperationObserver& aObserver,
        CUniEditorDocument& aDocument,
        RFs& aFs ) :
    CUniEditorOperation( aObserver, aDocument, aFs, EUniEditorOperationVCard ), 
    iNewVCardAttaId( KMsvNullIndexEntryId ),
    iAddedVCardCount( 0 ),
    iCurrentContactIndex( -1 )
    {
    }

// ---------------------------------------------------------
// CUniEditorVCardOperation::CUniEditorVCardOperation
//
// Destructor. iContact, iContacts and iCheckNames must be deleted
// in this order!
// ---------------------------------------------------------
//
CUniEditorVCardOperation::~CUniEditorVCardOperation()
    {
    Cancel();

    delete iContact;
    delete iContacts;
    delete iCheckNames;
    delete iStoreContact;
    delete iVCardEng;
    delete iVCardExportOp;
    delete iEditStore;
    
    iVCardStream.Close();
    }

// ---------------------------------------------------------
// CUniEditorVCardOperation::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniEditorVCardOperation::ConstructL()
    {
    BaseConstructL();

    iCheckNames = CMsgCheckNames::NewL();

    iVCardEng = CVPbkVCardEng::NewL( iCheckNames->ContactMatcher().GetContactManager() );
    }

// ---------------------------------------------------------
// CUniEditorVCardOperation::Start
//
// ---------------------------------------------------------
//
void CUniEditorVCardOperation::Start()
    {
    iOperationState = EUniProcessVCardStart;
    CompleteSelf( KErrNone );    
    }

// ---------------------------------------------------------
// CUniEditorVCardOperation::GetContactsL
//
// ---------------------------------------------------------
//
void CUniEditorVCardOperation::GetContactsL()
    {
    ResetAll();

    // Create a new empty array
    iContacts = CVPbkContactLinkArray::NewL();

    iCheckNames->FetchRecipientsL( *iContacts, CMsgCheckNames::EMsgTypeAll );
    
    if ( iContacts->Count() > 0 )
        {
        iOperationState = EUniProcessVCardExportNext;
        CompleteSelf( KErrNone );
        }
    else
        {
        iOperationState = EUniProcessVCardComplete;
        CompleteSelf( KErrNone );
        }
    }

// ---------------------------------------------------------
// CUniEditorVCardOperation::ExportVCardAndAddAttachmentL
//
// ---------------------------------------------------------
//
void CUniEditorVCardOperation::ExportVCardAndAddAttachmentL()
    {
    Reset();
    
    // Initial value is -1, so we start to handle the contact in index 0.
    iCurrentContactIndex++;
    
    if ( iCurrentContactIndex < iContacts->Count() )
        {
        // Handle next contact.
        iContact = const_cast<MVPbkContactLink *>(
            &iContacts->At( iCurrentContactIndex ) )->CloneLC();
        CleanupStack::Pop();    // Cloned link.

        iOperationState = EUniProcessVCardCreatingEmptyVCardAttachment;
        CompleteSelf( KErrNone );
        }
    else
        {
        // No more contacts, so we can clean up and complete.
        // Clean up is done, in order to get iContact deleted, before
        // its contact manager is deleted. Bad design in virtual phonebook.
        iOperationState = EUniProcessVCardComplete;
        CompleteSelf( KErrNone );
        }
    }

// ---------------------------------------------------------
// CUniEditorVCardOperation::AddedVCardCount
//
// ---------------------------------------------------------
//
TInt CUniEditorVCardOperation::AddedVCardCount()
    {
    return iAddedVCardCount;
    }

// ---------------------------------------------------------
// CUniEditorVCardOperation::CreateEmptyVCardAttachmentL
//
// ---------------------------------------------------------
//    
void CUniEditorVCardOperation::CreateEmptyVCardAttachmentL()
    {
    iCheckNames->ContactMatcher().GetStoreContactL( *iContact, &iStoreContact );
    
    // If this store contact is a group, then we don't add it to object list.
    if ( !iStoreContact->Group() )
        {
        MVPbkStoreContactFieldCollection& fieldCollection = iStoreContact->Fields();

        HBufC* contactName = iCheckNames->ContactMatcher().ContactNameFormatterL().GetContactTitleL( fieldCollection, 0 );

        // Form a name for the attachment. If contactName is NULL, then 
        // attachment is name vCard.vcf. 
        if ( contactName )
            {
            // Reserve more space for filename to include .vcf
            contactName = contactName->ReAllocL( 
                contactName->Length() + KTempVCardFileSuffix().Length() ); // ReAllocL deletes original
            CleanupStack::PushL( contactName );
            
            // Modify contact's name to be a file name.
            TPtr contactFilePtr = contactName->Des();
            contactFilePtr.Append( KTempVCardFileSuffix );
            }
        else
            {
            // Use hard coded filename vCard.vcf.
            contactName = KTempVCardFileName().AllocLC();
            }

        // Remove illegal characters from file name. Phonebook allows characters
        // to be inserted to contact's name, which are not allowed by Symbian file
        // system.
        TPtr contactNamePtr = contactName->Des();   // May contain illegal characters
        HBufC* validName = HBufC::NewLC( contactName->Length() );
        TPtr validNamePtr = validName->Des();   // Does not contain illegal characters

        // Loop throuh characters and replace illegal characters with underscore.
        for ( TInt i = 0; i < contactName->Length(); ++i )
            {
            TChar ch = contactNamePtr[i];
            if ( KInvalidFileNameChars().Locate( ch ) == KErrNotFound )
                {
                validNamePtr.Append( ch );
                }
            else
                {
                validNamePtr.Append( KReplaceChar );
                }
            }
        validNamePtr.TrimAll();
        
        // Create empty stored attachment to store.
        RFile newVCardFile;
        iEditStore = iDocument.Mtm().Entry().EditStoreL();
        MMsvAttachmentManagerSync& managerSync = iEditStore->AttachmentManagerExtensionsL();
        CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
        CleanupStack::PushL( attachment );
        managerSync.CreateAttachmentL( validNamePtr, newVCardFile, attachment );
        CleanupStack::Pop( attachment); // ownership transferred
        CleanupStack::PopAndDestroy( 2, contactName );  // validName, contactName
        
        // Store the id, it is used when attachment created above is saved to 
        // object list.
        iNewVCardAttaId = attachment->Id();
        iVCardStream.Attach( newVCardFile );
        
        // Call vCard engine to export a contact to a stored attachment. 
        // Engine will call VPbkSingleContactOperationComplete or
        // VPbkSingleContactOperationFailed
        iVCardExportOp = iVCardEng->ExportVCardL( iVCardStream,
                                                  *iContact,
                                                  *this);
        SetPending();
        }
    else
        {
        // Don't add group name as a vCard, results are not nice.
        iOperationState = EUniProcessVCardExportNext;
        CompleteSelf( KErrNone );
        }
    }

// ---------------------------------------------------------
// CUniEditorVCardOperation::DoCancelCleanup
//
// ---------------------------------------------------------
//
void CUniEditorVCardOperation::DoCancelCleanup()
    {
    Reset();
    }

// ---------------------------------------------------------
// CUniEditorVCardOperation::RunL
//
// ---------------------------------------------------------
//
void CUniEditorVCardOperation::RunL()
    {
    PrintOperationAndState();
    if ( iStatus.Int() != KErrNone )
        {
        SetError( iStatus.Int() );
        iOperationState = EUniProcessVCardError;
        }
        
    switch( iOperationState )
        {
        case EUniProcessVCardStart:
            {
            // Get contacts to export
            GetContactsL();
            break;
            }
        case EUniProcessVCardExportNext:
            {
            // Start handling of single contact.
            ExportVCardAndAddAttachmentL();
            break;
            }           
        case EUniProcessVCardCreatingEmptyVCardAttachment:
            {
            // Create an empty attachment to message store.
            CreateEmptyVCardAttachmentL();
            break;
            }    
        case EUniProcessVCardComplete:
            {
            // All contacts handled, tell appUi that we are complete.
            ReportEvent( EUniEditorOperationComplete );
            break;
            }
        case EUniProcessVCardCancel:
            {
            // Somebody cancelled us, tell appUi that we were cancelled.
            ReportEvent( EUniEditorOperationCancel );
            break;
            }
        case EUniProcessVCardError:
            {
            Reset();
            iObserver.EditorOperationEvent( EUniEditorOperationVCard,
                                            EUniEditorOperationError );
            break;
            }
        default:
            {
            // Huh, this can also be possible.
            iObserver.EditorOperationEvent( EUniEditorOperationVCard,
                                            EUniEditorOperationError );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorVCardOperation::VPbkSingleContactOperationComplete
//
// ---------------------------------------------------------
//
void CUniEditorVCardOperation::VPbkSingleContactOperationComplete( MVPbkContactOperationBase& aOperation, 
                                                                   MVPbkStoreContact* /*aContact*/ )
    {
    iLastError = KErrNone;
    
    if ( &aOperation == iVCardExportOp )
        {
        TDataType type( KVCardMiMeType );

        // Release and close stream.
        TRAP_IGNORE( iVCardStream.CommitL() );
        iVCardStream.Close();

        // Get attachment handle again, since iVCardStream.Attach() function 
        // call makes the original handle invalid.
        MMsvAttachmentManager* manager = NULL;
        TRAP_IGNORE( manager = &iEditStore->AttachmentManagerL() );
        
        RFile storedVCard;
        TRAPD( err, storedVCard = manager->GetAttachmentFileL( iNewVCardAttaId ) );
        
        if ( err == KErrNone )
            {
            // Add stored attachment to object list.        
            CMsgMediaInfo* media = NULL;
            
            TRAP_IGNORE( media = CMsgMediaInfo::NewL( storedVCard, 
                                                      type, 
                                                      EMsgMediaUnknown ) );
                    
            // Count the size of the message after current vCard is inserted.    
            TInt vCardSize = media->FileSize();
            TInt sizeAfterInsert = iDocument.MessageSize() + vCardSize;
            
            if ( sizeAfterInsert <= iDocument.MaxMessageSize() )
                {
                // Must be committed and deleted since only one client can access 
                // entry in edit mode and AddStoredAttachmentL function want's to 
                // edit it too.
                TRAP_IGNORE( iEditStore->CommitL() );
                
                delete iEditStore;
                iEditStore = NULL;
                
                TRAP_IGNORE( iDocument.DataModel()->AddStoredAttachmentL( iNewVCardAttaId, 
                                                                          media ) );
                media = NULL;   // Not owned anymore
                
                // Increase success counter, used to show correct note
                // when all contacts are handled.
                iAddedVCardCount++;
                
                // Object is now saved to unieditor's object list.
                }
            else
                {
                // Delete created media info
                delete media;
                media = NULL;
                
                TRAP_IGNORE( 
                    {
                    MMsvAttachmentManagerSync& managerSync = iEditStore->AttachmentManagerExtensionsL();    
                    managerSync.RemoveAttachmentL( CUniDataUtils::IndexPositionOfAttachmentL( *manager, 
                                                                                              iNewVCardAttaId ) );
                    iEditStore->CommitL();
                    iNewVCardAttaId = KMsvNullIndexEntryId;
                    });
                    
                SetError( EUniInsertTooBig );
                
                // Try whether next vCard fits to max message size.
                }
            
            // Current contact handled, start next one. Whether there still
            // exist unhandled contacts, it is decided elsewhere.
            iOperationState = EUniProcessVCardExportNext;
            CompleteOperation( KErrNone );
            
            // Close temporary attachment handle.
            storedVCard.Close();
            }
        else
            {
            // Couldn't get a handle to a stored attachment,
            // let's give up.
            iOperationState = EUniProcessVCardError;
            CompleteOperation( err );
            }        
        }
    }

// ---------------------------------------------------------
// CUniEditorVCardOperation::VPbkSingleContactOperationFailed
//
// ---------------------------------------------------------
//
void CUniEditorVCardOperation::VPbkSingleContactOperationFailed(
    MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    iLastError = aError;
    
    SetError( iLastError );

    Reset();
    
    iOperationState = EUniProcessVCardError;
    CompleteOperation( iLastError );
    }

// ---------------------------------------------------------
// CUniEditorVCardOperation::Reset
//
// ---------------------------------------------------------
void CUniEditorVCardOperation::Reset()
    {
    delete iContact;
    iContact = NULL;

    delete iStoreContact;
    iStoreContact = NULL;

    delete iVCardExportOp;
    iVCardExportOp = NULL;

    delete iEditStore;
    iEditStore = NULL;
        
    iNewVCardAttaId = KMsvNullIndexEntryId;
    iVCardStream.Close();
    }

// ---------------------------------------------------------
// CUniEditorVCardOperation::ResetAll
//
// ---------------------------------------------------------
//
void CUniEditorVCardOperation::ResetAll()
    {
    // First reset counters.
    iCurrentContactIndex = -1;
    iAddedVCardCount = 0;

    if ( iContacts )
        {
        delete iContacts;
        iContacts = NULL;
        }
    
    Reset();
    
    // Reset any previous errors, since we are started again.
    ResetErrors();
    }

// ---------------------------------------------------------
// CUniEditorVCardOperation::ReportEvent
//
// ---------------------------------------------------------
//
void CUniEditorVCardOperation::ReportEvent( TUniEditorOperationEvent aEvent )
    {
    iObserver.EditorOperationEvent( EUniEditorOperationVCard, aEvent );
    }

// EOF
