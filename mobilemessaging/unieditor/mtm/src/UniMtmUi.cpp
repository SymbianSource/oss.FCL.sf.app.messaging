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
* Description:  
*       Provides Uni MTM UI methods.
*
*/



// INCLUDE FILES

#include <apgcli.h>
#include <msvuids.h>
#include <mtmuidef.hrh>
#include <mtmdef.h>
#include <UniMtms.rsg>

#include <msvstore.h>
#include <mmsvattachmentmanager.h>

#include <e32keys.h>
#include <coemain.h>
#include <eikenv.h>
#include <eikdef.h>
#include <eikmfne.h>
#include <eikedwin.h>
#include <eikcapc.h>
#include <gulicon.h>
#include <ErrorUI.h> //CErrorUI

#include <aknPopup.h>
#include <aknlists.h>
#include <aknconsts.h>
#include <akncontext.h>
#include <akntabgrp.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>

#include <centralrepository.h>    // link against centralrepository.lib
#include <messaginginternalcrkeys.h> // for Central Repository keys
#include <messagingvariant.hrh>     // additional headers variation flags. 
									// Currently MMS variation, no variataion for unieditor available.

#include <StringLoader.h>

#include <Muiumsginfo.h>
#include <MuiuMsvUiServiceUtilities.h>
#include <MuiuOperationWait.h>
#include <muiumsguploadparam.h>

#include <MtmExtendedCapabilities.hrh>

#include <mmsgenutils.h>

#include <unimsventry.h>

#include "UniMtmUi.h"
#include "UniMtmPanic.h"
#include "UniMtmConst.h"
#include "UniClientMtm.h"


#include "UniEditorUids.hrh"

// EXTERNAL FUNCTION PROTOTYPES  

extern void Panic( TUniMtmPanic aPanic );

// CONSTANTS


_LIT( KCharLeftAddressIterator, "<" );
_LIT( KCharRightAddressIterator, ">" );
_LIT( KMmsCharCommaAndSpace, ", " );

const TInt KMessageSize = 20;
const TInt KZero = 0; //constant for memory check
const TInt KMmsAdditionalCharsStringLength = 6;
const TUint KMmsLRMarker = 0x200E;


// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------
// NewUniMtmUiL
// Factory function
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUi* NewUniMtmUiL( CBaseMtm& aMtm, CRegisteredMtmDll& aRegisteredDll )
    {
    return CUniMtmUi::NewL( aMtm, aRegisteredDll );
    }


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CUniMtmUi::CUniMtmUi
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CUniMtmUi::CUniMtmUi( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll )
    :   CBaseMtmUi( aBaseMtm, aRegisteredMtmDll )
    {
    }

// ---------------------------------------------------------
// CUniMtmUi::ConstructL
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CUniMtmUi::ConstructL()
    {
    CBaseMtmUi::ConstructL();
    }

// ---------------------------------------------------------
// CUniMtmUi::NewL
// Two-phased constructor.
// ---------------------------------------------------------
// 
CUniMtmUi* CUniMtmUi::NewL( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll )
    {
    CUniMtmUi* self = new ( ELeave ) CUniMtmUi( aBaseMtm, aRegisteredMtmDll );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

    
// ---------------------------------------------------------
// CUniMtmUi::~CUniMtmUi
// Destructor
// ---------------------------------------------------------
//
CUniMtmUi::~CUniMtmUi()
    {
    
    }


// ---------------------------------------------------------
// CUniMtmUi::QueryCapability
// Capability check for UI mtm specific functionality. These should
// be minimal, as the default capability checking places are UI data MTM
// (UI) and client MTM (engine).
// ---------------------------------------------------------
TInt CUniMtmUi::QueryCapability( TUid aCapability, TInt& aResponse )
    {
    return CBaseMtmUi::QueryCapability( aCapability, aResponse );
    }

// ---------------------------------------------------------
// CUniMtmUi::CreateL
// Launches Uni editor for creating a new message.
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::CreateL( const TMsvEntry& aEntry, 
                                  CMsvEntry& aParent, 
                                  TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( aEntry.iMtm == Type(), Panic( EUniWrongMtm ) );
    __ASSERT_DEBUG( aEntry.iType == KUidMsvMessageEntry, Panic( EUniNotAMessage ) );

    if( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(
    	Session(),
        KZero ) )
        {
        User::Leave( KErrDiskFull );
        }
    
    return LaunchEditorApplicationL( aStatus, aParent.Session(), ECreateNewMessage );
    }

// ---------------------------------------------------------
// CUniMtmUi::DeleteServiceL
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::DeleteServiceL( const TMsvEntry& /*aService*/, 
                                         TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CUniMtmUi::DeleteFromL
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::DeleteFromL( const CMsvEntrySelection& /*aSelection*/, 
                                      TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CUniMtmUi::CopyToL
// Copy local outbox entry to remote server = called when sending the entry from MCE outbox
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::CopyToL( const CMsvEntrySelection& aSelection, 
                                  TRequestStatus& aStatus )
    {
    return CopyMoveToL( aSelection, aStatus, ETrue );
    }

// ---------------------------------------------------------
// CUniMtmUi::MoveToL
// Move local outbox entry to remote server.
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::MoveToL( const CMsvEntrySelection& aSelection, 
                                  TRequestStatus& aStatus )
    {
    return CopyMoveToL( aSelection, aStatus, EFalse );
    }

// ---------------------------------------------------------
// CUniMtmUi::CopyMoveToL
// Performs either CopyToL or MoveToL operation.
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::CopyMoveToL( const CMsvEntrySelection& aSelection, 
                                      TRequestStatus& aStatus, TBool aCopyOnly )
    {
    __ASSERT_DEBUG( iBaseMtm.Entry().Entry().iMtm==Type(), Panic( EUniWrongMtm ) );
    __ASSERT_DEBUG( iBaseMtm.Entry().Entry().iType==KUidMsvServiceEntry, Panic( EUniNotAService ) );

    // --- Carry out the operation ---
    TMsvEntry service ( BaseMtm().Entry().Entry() );
    //    Get CMsvEntry of parent of messages to be sent
    CMsvEntry* parentEntry = Session().GetEntryL( aSelection[0] );    

    CleanupStack::PushL( parentEntry );
    parentEntry->SetEntryL( parentEntry->Entry().Parent() );
    
    // Do the copy/move
    CMsvOperation* op = aCopyOnly ?
        parentEntry->CopyL( aSelection, service.Id(), aStatus ) :
        parentEntry->MoveL( aSelection, service.Id(), aStatus );
    
    CleanupStack::PopAndDestroy( parentEntry ); 
    return op;
    }

// ---------------------------------------------------------
// CUniMtmUi::CopyFromL
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::CopyFromL(const CMsvEntrySelection& /*aSelection*/, 
                                    TMsvId /*aTargetId*/, 
                                    TRequestStatus& /*aStatus*/)
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CUniMtmUi::MoveFromL
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::MoveFromL(const CMsvEntrySelection& /*aSelection*/, 
                                    TMsvId /*aTargetId*/, 
                                    TRequestStatus& /*aStatus*/)
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CUniMtmUi::OpenL
// Handles request to open existing message server entry (message/service)
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::OpenL( TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iBaseMtm.Entry().Entry().iMtm==Type(), Panic( EUniWrongMtm ) );
    const TMsvEntry& entry = BaseMtm().Entry().Entry();
    __ASSERT_DEBUG( !entry.ReadOnly(), Panic( EUniWrongParameters ));
    return EditL( aStatus );    
    }

// ---------------------------------------------------------
// CUniMtmUi::OpenL
// Opens the first entry in a selection
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::OpenL( TRequestStatus& aStatus, 
                                const CMsvEntrySelection& aSelection )
    {
    __ASSERT_DEBUG( iBaseMtm.Entry().Entry().iMtm==Type(), Panic( EUniWrongMtm ) );

    BaseMtm().SwitchCurrentEntryL( aSelection[0] );
    return OpenL( aStatus );
    }

// ---------------------------------------------------------
// CUniMtmUi::CloseL
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::CloseL( TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CUniMtmUi::CloseL
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::CloseL( TRequestStatus& /*aStatus*/, 
                                 const CMsvEntrySelection& /*aSelection*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CUniMtmUi::EditL
// Opens a message (editor) for editing.
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::EditL( TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iBaseMtm.Entry().Entry().iMtm==Type(), Panic( EUniWrongMtm ) );

    CMsvOperation* msvoperation = NULL;

    switch ( iBaseMtm.Entry().Entry().iType.iUid )
        {
        case KUidMsvMessageEntryValue:
            {
            if ( iBaseMtm.Entry().Entry().Connected() )
                {
                User::Leave( KErrLocked ); 
                }
            msvoperation = LaunchEditorApplicationL( aStatus, iBaseMtm.Entry().Session() );
            }
            break;

        case KUidMsvServiceEntryValue:
        case KUidMsvAttachmentEntryValue:   // flow through
        case KUidMsvFolderEntryValue:       // flow through
        default:
            User::Leave( KErrNotSupported );
            break;
        }

    return msvoperation;
    }

// ---------------------------------------------------------
// CUniMtmUi::EditL
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::EditL( TRequestStatus& aStatus, 
                                const CMsvEntrySelection& aSelection )
    {
    BaseMtm().SwitchCurrentEntryL( aSelection[0] );
    return EditL( aStatus );
    }

// ---------------------------------------------------------
// CUniMtmUi::ViewL
// Opens the message for reading in Viewer.
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::ViewL( TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CUniMtmUi::ViewL
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::ViewL( TRequestStatus& /*aStatus*/, 
                                const CMsvEntrySelection& /*aSelection*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CUniMtmUi::LaunchEditorApplicationL()
// Does the actual opening.
// -----------------------------------------------------------------------------
//
CMsvOperation* CUniMtmUi::LaunchEditorApplicationL( TRequestStatus& aStatus, 
                                                   CMsvSession& aSession, 
                                                   TEditorType aEditorType )
    {
    TEditorParameters temp;    
    
    if ( iBaseMtm.HasContext() )
        {
        temp.iId = iBaseMtm.Entry().EntryId();
        }
    
    switch( aEditorType )
        {
        case EReadOnly:
        case ESpecialEditor:
            User::Leave( KErrNotSupported );
            break;                             
        case ECreateNewMessage:
            temp.iFlags|=EMsgCreateNewMessage ;
            temp.iId=KMsvDraftEntryIdValue;
            break;
        case EEditExisting:  //fall through
        default:
            break;
        };

    temp.iFlags &= ~( EMtmUiFlagEditorPreferEmbedded|EMtmUiFlagEditorNoWaitForExit );
    temp.iFlags |= ( Preferences() & EMtmUiFlagEditorPreferEmbedded?EMsgLaunchEditorEmbedded:EMsgLaunchEditorStandAlone );

    TPckgC<TEditorParameters> paramPack( temp );
    
    return MsgEditorLauncher::LaunchEditorApplicationL(
        aSession,
        Type(),
        aStatus,
        temp,
        KNullDesC(),
        paramPack );
    }

// ---------------------------------------------------------
// CUniMtmUi::ForwardL
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::ForwardL( TMsvId /*aDestination*/,
                                   TMsvPartList /*aPartList*/, 
                                   TRequestStatus& /*aCompletionStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }


// ---------------------------------------------------------
// CUniMtmUi::ReplyL
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::ReplyL( TMsvId /*aDestination*/, 
                                 TMsvPartList /*aPartList*/, 
                                 TRequestStatus& /*aCompletionStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CUniMtmUi::CancelL
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::CancelL( TRequestStatus& /*aStatus*/, 
                                  const CMsvEntrySelection& /*aSelection*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CUniMtmUi::InvokeSyncFunctionL
// ---------------------------------------------------------
//
void CUniMtmUi::InvokeSyncFunctionL( TInt aFunctionId, 
                                    const CMsvEntrySelection& aSelection, 
                                    TDes8& aParameter )
    {
    CBaseMtmUi::InvokeSyncFunctionL( aFunctionId, aSelection, aParameter );
    }

// ---------------------------------------------------------
// CUniMtmUi::InvokeAsyncFunctionL
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::InvokeAsyncFunctionL( TInt aFunctionId, 
                                               const CMsvEntrySelection& aSelection, 
                                               TRequestStatus& aCompletionStatus, 
                                               TDes8& aParameter )
    {
    CMsvOperation* msvoperation = NULL;

    switch ( aFunctionId )
        {
    case KMtmUiFunctionMessageInfo:
        msvoperation = OpenMessageInfoL( aCompletionStatus, aParameter );
        break;
    default:
        msvoperation = CBaseMtmUi::InvokeAsyncFunctionL( aFunctionId, 
            aSelection, 
            aCompletionStatus, 
            aParameter);
        break;
        }
    return msvoperation;
    }

// ---------------------------------------------------------
// CUniMtmUi::DisplayProgressSummary
// ---------------------------------------------------------
//
TInt CUniMtmUi::DisplayProgressSummary( const TDesC8& /*aProgress*/ ) const
    {
    // Not in use.
    return KErrNone;
    }

// ---------------------------------------------------------
// CUniMtmUi::GetResourceFileName
// ---------------------------------------------------------
//
void CUniMtmUi::GetResourceFileName( TFileName& aFileName ) const
    {
    aFileName = KUniMtmResourceFile;
    }


// ---------------------------------------------------------
// CUniMtmUi::OpenMessageInfoL(TRequestStatus& aCompletionStatus, TDes8& aParameter)
// Opens the Message Info popup.
// ---------------------------------------------------------
//
CMsvOperation* CUniMtmUi::OpenMessageInfoL( TRequestStatus& aCompletionStatus, 
                                           TDes8& aParameter )
    {
    TInt popCount( 0 );
       
    // get additional headers variation info
    TInt addHeadersBitmask = 0;
    
    CRepository* repository = CRepository::NewL( KCRUidMuiuVariation );
    repository->Get( KMuiuMmsFeatures, addHeadersBitmask );
    delete repository;
    repository = NULL;
    
    TBool priorityFeatureSupported = EFalse;
    
    // MMS Priority feature is enabled
    if ( addHeadersBitmask & KMmsFeatureIdPrioritySupport )
        {
        priorityFeatureSupported = ETrue;
        }
    
    // The items for Info are read from the message entry.
    // Note: Most of the stuff is in TMsvEntry, but not all -> MMS entry used all the way.
    TMsvEntry entry = BaseMtm().Entry().Entry();
    __ASSERT_DEBUG( entry.iType == KUidMsvMessageEntry, Panic( EUniNotAMessage ) );
    __ASSERT_DEBUG( entry.iMtm==Type(), Panic( EUniWrongMtm ) );

    static_cast<CUniClientMtm&>( iBaseMtm ).LoadMessageL();

    TUniMessageCurrentType currType = TUniMsvEntry::CurrentMessageType( entry );
    CMsgInfoMessageInfoDialog::TMessageInfoType infoType =
        CMsgInfoMessageInfoDialog::ESmsEditor;

    TMsgInfoMessageInfoData infoData;
    
    // Subject
    // TODO: When should we show subject?
    //       - MMS: yes
    //       - ordinary SMS: no?
    //       - SMS with header field
    //       - Email over SMS: yes?
    infoData.iSubject.Set( static_cast<CUniClientMtm&>( iBaseMtm ).SubjectL() );

    // Get correct time stamps
    infoData.iDateTime = entry.iDate;
    HBufC* msgType( NULL );
    switch ( currType )
        {
        case EUniMessageCurrentTypeMms:
            {
            infoType = CMsgInfoMessageInfoDialog::EMmsEditor;
            msgType = StringLoader::LoadLC( R_MSGTYPE_TEXT_MMS, iCoeEnv );
            popCount++;
            infoData.iType.Set( *msgType );
            break;
            }
        case EUniMessageCurrentTypeSms:
        default:
            {
            //infoType already set
            msgType = StringLoader::LoadLC( R_MSGTYPE_TEXT_SMS, iCoeEnv );
            popCount++;
            infoData.iType.Set( *msgType ); 
            break;
            }
        }
    
    // Size of message.
    if ( currType == EUniMessageCurrentTypeMms )
        {
        TInt sizeOfMessage = 0;
        if ( aParameter.Length() )
            { // We are called from editor.
            TPckgBuf <TInt> paramPack;
            const TInt paramSize = sizeof( TInt );

            paramPack.Copy( aParameter.Ptr(), paramSize );
            sizeOfMessage = KKilo * paramPack();
            }
        else
            {
            // Cannot get empty message size ok from client MTM, as it always contains something.
            if ( !IsMessageEmptyL() )
                {
                sizeOfMessage = static_cast<CUniClientMtm&>( iBaseMtm ).MessageSize();
                }
            }
        // Finally make the UI string
        TBuf<KMessageSize> sizeString;
        MessageSizeToStringL( sizeString, sizeOfMessage );
        infoData.iSize.Set( sizeString );
        
        if ( priorityFeatureSupported )
            {
            HBufC* priority = NULL;
            switch ( entry.Priority() )
        	    {
                case EMsvHighPriority:
            	    {
                    priority  = StringLoader::LoadLC( R_UNI_MMS_MESSAGE_PRIORITY_HIGH, iCoeEnv );
                    popCount++;
                    break;
            	    }
        	    case EMsvLowPriority:
        		    {
                    priority  = StringLoader::LoadLC( R_UNI_MMS_MESSAGE_PRIORITY_LOW, iCoeEnv );
                    popCount++;
        		    break;
        		    }
                case EMsvMediumPriority:
                default:
            	    {
        	        priority  = StringLoader::LoadLC( R_UNI_MMS_MESSAGE_PRIORITY_NORMAL, iCoeEnv );
                    popCount++;
                    break;
                    }
                }
            
            infoData.iPriority.Set( *priority ); 
            }
        }

    // To
    const CMsvRecipientList& recipients = static_cast<CUniClientMtm&>( iBaseMtm ).AddresseeList();
    HBufC* toList( NULL );
    HBufC* ccList( NULL );
    HBufC* bccList( NULL );
      
    TurnRecipientsArrayIntoStringsLC(
                            recipients,
                            toList,
                            ccList,
                            bccList, 
                            addHeadersBitmask );
    if ( toList )
        {
        infoData.iTo.Set( *toList );
        popCount++;
        }
    if ( ccList )
        {
        infoData.iCC.Set( *ccList );
        popCount++;
        }
    if ( bccList )
        {
        infoData.iBCC.Set( *bccList );
        popCount++;
        }

    CMsgInfoMessageInfoDialog* infoDialog = CMsgInfoMessageInfoDialog::NewL();

    infoDialog->ExecuteLD( infoData, infoType );
    
    CleanupStack::PopAndDestroy( popCount );

    return CMsvCompletedOperation::NewL( Session(), 
        KUidMsvLocalServiceMtm, 
        KNullDesC8, 
        KMsvLocalServiceIndexEntryId, 
        aCompletionStatus );
    }

      
// ---------------------------------------------------------
// CUniMtmUi::TurnRecipientsArrayIntoStringsLC
// Every created recipient buffer object increases pushed object count by one
// ---------------------------------------------------------
//
void CUniMtmUi::TurnRecipientsArrayIntoStringsLC(
                            const   CMsvRecipientList&  aRecip,
                            HBufC*&                     aTo,
                            HBufC*&                     aCc,
                            HBufC*&                     aBcc, 
                            TInt                        aAddHeadersVariation ) const
    {
    TInt count = aRecip.Count();
    if ( count == 0 )
        {
        return;
        }
        
    TMsvEntry entry = BaseMtm().Entry().Entry();
        
    // indeces refers To, Cc, Bcc
    TInt recipTypesIndex = 0;
    const TInt KRecipTypesCount = 3;
    
    TMsvRecipientTypeValues recipType ( EMsvRecipientTo );
    
    // needs first round to check what is there
    TInt index = 0;
    TInt stringLengthTo = 0;
    TInt stringLengthCc = 0;
    TInt stringLengthBcc = 0;
    TInt* stringLength = &stringLengthTo;    
    for ( recipTypesIndex = 0; recipTypesIndex < KRecipTypesCount; recipTypesIndex++ )
        {
        if ( recipTypesIndex == 1 )
            {
            stringLength = &stringLengthCc;
            recipType = EMsvRecipientCc;
            }
        else if ( recipTypesIndex == 2 )
            {
            stringLength = &stringLengthBcc;
            recipType = EMsvRecipientBcc;
            }
            
        for ( index = 0; index < count; index++ )
            {
            if ( aRecip.Type( index ) == recipType )
                {
                ( *stringLength ) += TMmsGenUtils::Alias( aRecip[index] ).Length();
                ( *stringLength ) += TMmsGenUtils::PureAddress( aRecip[index] ).Length();
                // Additional space for chars: '<' '>' ',' + 2 x KMmsLRMarker
                ( *stringLength ) += KMmsAdditionalCharsStringLength;
                }
            }
        }

    if (    stringLengthTo
        ||  stringLengthCc
        ||  stringLengthBcc )
        {
        // needs second round to copy strigs:
        // if CC not variated On, append CC recipients to To: field
        // if BCC not variated On but CC variated On, append BCC recipients to Cc: field
        // if BCC not variated On and also CC variated Off, append BCC recipients to To: field    
        HBufC** recipients = NULL;
        recipType = EMsvRecipientTo;
        TInt bufLength( stringLengthTo ); 
        for ( recipTypesIndex = 0; recipTypesIndex < KRecipTypesCount; recipTypesIndex++ )
            {       
            recipients = NULL;
            if ( recipTypesIndex == 0 )
                {
                if ( ! ( aAddHeadersVariation & KMmsFeatureIdEditorCcField ) )
                    {
                    // CC not supported on the UI
                    bufLength += stringLengthCc;
                    if ( ! ( aAddHeadersVariation & KMmsFeatureIdEditorBccField ) )
                        {
                        // BCC not supported on the UI
                        bufLength += stringLengthBcc;
                        }
                    }
                if ( bufLength )
                    {
                    aTo = HBufC::NewLC( bufLength );
                    recipients = &aTo;
                    }
                else
                    {
                    continue;
                    }
                }        
            else if ( recipTypesIndex == 1 )
                {
                recipType = EMsvRecipientCc;
                if ( aAddHeadersVariation & KMmsFeatureIdEditorCcField )
                    {
                    // Cc supported on the UI
                    bufLength = stringLengthCc;
                    if ( ! ( aAddHeadersVariation & KMmsFeatureIdEditorBccField ) )
                        {
                        // BCC not supported on the UI
                        bufLength += stringLengthBcc;
                        }
                    if ( bufLength )
                        {
                        aCc = HBufC::NewLC( bufLength );    
                        recipients = &aCc;
                        }
                    else
                        {
                        continue;
                        }
                    }
                else
                    {
                    // Cc not supported on the UI
                    recipients = &aTo;
                    }
                }
            else if ( recipTypesIndex == 2 )
                {
                
                recipType = EMsvRecipientBcc;
                if ( aAddHeadersVariation & KMmsFeatureIdEditorBccField )
                    {
                    // BCC supported on the UI
                    bufLength = stringLengthBcc;
                    if ( bufLength )
                        {
                        aBcc = HBufC::NewLC( bufLength );    
                        recipients = &aBcc;
                        }
                    else
                        {
                        continue;
                        }
                    }
                else if ( aAddHeadersVariation & KMmsFeatureIdEditorCcField )
                    {
                    // CC supported on the UI
                    recipients = &aCc;
                    }
                else 
                    {
                    // BCC not supported on the UI
                    recipients = &aTo;
                    }
                    
                }
            
            if (    recipients
                &&  *recipients )
                {                
                TPtr listPtr = (*recipients)->Des();
                for ( index=0; index < count; index++ )
                    {
                    if ( aRecip.Type( index ) == recipType )
                        {
                        TPtrC name = TMmsGenUtils::Alias( aRecip[index] );
                        TPtrC number = TMmsGenUtils::PureAddress( aRecip[index] );
                        if ( name.Length() )
                            {
                            listPtr.Append( name );
                            listPtr.Append( KMmsLRMarker );
                            listPtr.Append( KCharLeftAddressIterator );
                            listPtr.Append( number );
                            listPtr.Append( KCharRightAddressIterator );
                            listPtr.Append( KMmsLRMarker );
                            }
                        else
                            {
                            listPtr.Append( number );
                            }
                        
                        //append always and remove last one in the very end
                        listPtr.Append( KMmsCharCommaAndSpace );
                        }
                    }
                }
            }
        
        // third round to strip the ", " off
        for ( recipTypesIndex = 0; recipTypesIndex < KRecipTypesCount; recipTypesIndex++ )
            {
            recipients = NULL;
            if ( recipTypesIndex == 0 
                &&  aTo )
                {
                recipients = &aTo;
                }   
            else if ( recipTypesIndex == 1 
                    &&  aCc )
                {
                recipients = &aCc;
                }   
            else if ( recipTypesIndex == 2 
                    &&  aBcc )
                {
                recipients = &aBcc;
                }
            if ( recipients
                &&  *recipients )
                {
                TPtr listPtr = (*recipients)->Des();
                if ( listPtr.Length() > KMmsCharCommaAndSpace().Length() )
                    {
                    listPtr.SetLength( listPtr.Length() - KMmsCharCommaAndSpace().Length() );
                    }
                }
            }
        }
    }


// ---------------------------------------------------------
// CUniMtmUi::MessageSizeToStringL(TDes& aTarget, TInt aFileSize)
// Converts message size in bytes to a kilobyte string. Rounds the bytes up to the
// next full kilo. I.e:
// 0 -> 0KB
// 1 -> 1KB
// 1024 -> 1KB
// 1025 -> 2KB
// ---------------------------------------------------------
//
void CUniMtmUi::MessageSizeToStringL( TDes& aTarget, TInt aFileSize ) const
    {
    TInt fileSize = aFileSize / KKilo;
    if ( aFileSize % KKilo )
        {
        fileSize++;
        }

    HBufC* buf = StringLoader::LoadLC( R_MSG_SIZE_KB, fileSize, iEikonEnv );
    TPtr ptr = buf->Des();
    ptr.Trim(); // remove possible white space from beginning
    aTarget.Append( ptr );
    CleanupStack::PopAndDestroy( buf ); 
    }

// ---------------------------------------------------------
// CUniMtmUi::IsMessageEmptyL()
// Checks if the message is effectively empty or not
// ---------------------------------------------------------
//
TBool CUniMtmUi::IsMessageEmptyL() const
    {
    TBool empty = EFalse;
        
    CMsvStore* store = static_cast<CUniClientMtm&>( iBaseMtm ).Entry().ReadStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& attaManager = store->AttachmentManagerL();    
    TInt attaCount = attaManager.AttachmentCount();
    CleanupStack::PopAndDestroy( store );

    // Only attachments and subject are calculated
    // in message size.
    if ( attaCount == 0 &&
        static_cast<CUniClientMtm&>( iBaseMtm ).SubjectL().Length() == 0 )
        {
        // No attachments nor subject
        empty = ETrue;
        }

    return empty;
    }
    
// ---------------------------------------------------------
// CUniMtmUi::ConvertUtcToLocalTime
//
// ---------------------------------------------------------
//
void CUniMtmUi::ConvertUtcToLocalTime( TTime& aTime )
    {
    TLocale locale;
    // Add time difference
    aTime += locale.UniversalTimeOffset();
    if (locale.QueryHomeHasDaylightSavingOn())          
        { // and possible daylight saving time
        TTimeIntervalHours daylightSaving(1);          
        aTime += daylightSaving;
        }
    }

//  End of File
