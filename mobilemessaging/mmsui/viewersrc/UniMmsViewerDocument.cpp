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
* Description:  
*       Provides CMmsViewerDocument methods.
*
*/




// ========== INCLUDE FILES ================================

#include <eikenv.h>
#include <eikdoc.h>

#include <msvstd.h>                     // TMsvEntry
#include <msvapi.h>
#include <msvuids.h>                    // Msgs Uids
#include <mtclbase.h>                   // CBaseMtm
#include <mmsvattachmentmanager.h>
#include <cmsvattachment.h>
#include <cmsvmimeheaders.h>
#include <mmsvattachmentmanagersync.h>

#include <bautils.h>

#include <mmsclient.h>                  // Client Mtm API
#include <MuiuMsvUiServiceUtilities.h>  //Disk space check
#include <MsgAttachmentInfo.h>

#include <gmxmldocument.h>
#include <gmxmlnode.h>
#include <gmxmlelement.h>

// Features
#include <featmgr.h>	
#include <bldvariant.hrh>
#include <messagingvariant.hrh>

#include <MsgMimeTypes.h>
#include <MsgMediaResolver.h>
#include <MsgMediaInfo.h>
#include <MsgTextInfo.h>
#include <MmsConformance.h>

#include <centralrepository.h>          // link against centralrepository.lib
#include <MmsEngineInternalCRKeys.h>
#include <messaginginternalcrkeys.h>    // for Central Repository keys

#include "UniMmsViewerDocument.h"       // CMmsViewerDocument
#include "UniMmsViewerAppUi.h"          // CMmsViewerAppUi

#include "MmsMtmUi.h"
#include "MmsMtmConst.h"                // File logger

#include "MmsViewer.hrh"                // panics

// UniModel stuff
#include <unidatamodel.h>   
#include <unismilmodel.h>
#include <unidatautils.h>
#include <uniobjectsinfo.h>
#include <uniobjectsmodel.h> 
#include <uniobjectlist.h>
#include <unismillist.h>
#include <uniobject.h>
#include <unitextobject.h>
#include <unimimeinfo.h>
#include <unismilutils.h>
#include <unidrminfo.h>

// ========== CONSTANTS ====================================

const TInt KDefaultMaxSize = 300 * 1024;

// ---------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------
//
CMmsViewerDocument* CMmsViewerDocument::NewL(   CEikApplication& aApp )
    {
    CMmsViewerDocument* self = new ( ELeave ) CMmsViewerDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    

// ---------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMmsViewerDocument::CMmsViewerDocument( CEikApplication& aApp ) :
    CMsgEditorDocument( aApp ),
    iCurrentSlide( 0 ),
    iMaxMessageSize( KDefaultMaxSize ),
    iCreationMode( EMmsCreationModeWarning )
    {
    LOGTIMESTAMP("CMmsViewerDocument::CMmsViewerDocument ");
    }


// ---------------------------------------------------------
// CMmsViewerDocument::ConstructL
// ---------------------------------------------------------
//
void CMmsViewerDocument::ConstructL()
    {
    CMsgEditorDocument::ConstructL();
    
    TInt maxSize = KDefaultMaxSize;
    TInt creationMode = EMmsCreationModeWarning;
    
    CRepository* repository = CRepository::NewL( KCRUidMmsEngine );
    TInt err = repository->Get( KMmsEngineMaximumSendSize, maxSize );
    if ( !err )
        {
        iMaxMessageSize = maxSize;
        }
    err = repository->Get( KMmsEngineCreationMode, creationMode );
    if ( !err )
        {
        iCreationMode = creationMode;
        }
    delete repository;
    repository = NULL;

    if ( !iMaxMessageSize )
        {
        //MaximumSendSize == 0 means "anything goes"
        iMaxMessageSize = KMaxTUint32;
        }

    PrepareMtmL( KUidMsgTypeMultimedia );
    iFs = CEikonEnv::Static()->FsSession( ); 
    iDataModel = CUniDataModel::NewL( iFs, Mtm( ) );
    }

// ---------------------------------------------------------
// CMmsViewerDocument::~CMmsViewerDocument
//
// Destructor.
// ---------------------------------------------------------
//
CMmsViewerDocument::~CMmsViewerDocument()
    {
    delete iDataModel;
    }

// ---------------------------------------------------------
// CMmsViewerDocument::DefaultMsgFolder
// ---------------------------------------------------------
//
TMsvId CMmsViewerDocument::DefaultMsgFolder() const
    {
    return KMsvGlobalOutBoxIndexEntryIdValue;
    }

// ---------------------------------------------------------
// CMmsViewerDocument::DefaultMsgService
// ---------------------------------------------------------
//
TMsvId CMmsViewerDocument::DefaultMsgService() const
    {
    return KMsvLocalServiceIndexEntryId;
    }

// ---------------------------------------------------------
// CMmsViewerDocument::Mtm
// ---------------------------------------------------------
//
CMmsClientMtm& CMmsViewerDocument::Mtm()
    {
    return static_cast <CMmsClientMtm&> ( CMsgEditorDocument::Mtm() );
    }

// ---------------------------------------------------------
// CMmsViewerDocument::MtmUiL
// ---------------------------------------------------------
//
CMmsMtmUi& CMmsViewerDocument::MtmUiL()
    {
    return static_cast <CMmsMtmUi&> ( CMsgEditorDocument::MtmUiL() );
    }


// ---------------------------------------------------------
// CMmsViewerDocument::EntryChangedL
// Intentionally empty implementation.
// ---------------------------------------------------------
//
void CMmsViewerDocument::EntryChangedL()    // empty implementation
    {
    }


// ---------------------------------------------------------
// CMmsViewerDocument::CreateNewAttachmentModelL
//
// Creates new objects model object (virtual from CMsgEditorDocument).
// ---------------------------------------------------------
//
CMsgAttachmentModel* CMmsViewerDocument::CreateNewAttachmentModelL(
    TBool aReadOnly)
    {
    return CUniObjectsModel::NewL( aReadOnly );
    }


// ---------------------------------------------------------
// CMmsViewerDocument::InitializeL
// 
// Initialise the model. 
// ---------------------------------------------------------
//
void CMmsViewerDocument::InitializeL( MMmsDocumentObserver& aObserver )
    {
    iObserver = &aObserver;
    iSmilType = ENoSmil;
    iDataModel->RestoreL( *this );

    }

// ---------------------------------------------------------
// CMmsViewerDocument::RestoreReady
// ---------------------------------------------------------
//
void CMmsViewerDocument::RestoreReady( TInt aParseResult, TInt aError )
    {
    // Map NoSmil and MultipleSmil to MmsSmil
    iSmilType = iDataModel->SmilType( );
    if ( iSmilType == ENoSmil ||
         iSmilType == EMultipleSmil )
        {
        iSmilType = EMmsSmil;
        }
    iParseResult = aParseResult;

    iOrigAttachmentCount = iDataModel->OriginalAttachmentCount();
    iObserver->InitializeReady( iParseResult, aError );
    }

// ---------------------------------------------------------
// MessageSize
//
// ---------------------------------------------------------
TInt CMmsViewerDocument::MessageSize( )
    {
    // If this function is modified, CMmsMtmUi::MessageSizeL() must be kept in sync.
    TInt msgSizeBytes( 0 );
    TInt count = iDataModel->ObjectList().Count();
    while ( count-- )
        {
        msgSizeBytes += iDataModel->ObjectList().GetByIndex( count )->Size();
        }

    msgSizeBytes += iDataModel->SmilList().SmilByteSize();

    count = iDataModel->AttachmentList().Count();
    while ( count-- )
        {
        msgSizeBytes += iDataModel->AttachmentList().GetByIndex( count )->Size();
        }

    // subject size
    TPtrC subject;
    TRAP_IGNORE( subject.Set( Mtm().SubjectL() ) );
    msgSizeBytes += CUniDataUtils::UTF8Size( subject );

    return msgSizeBytes;    
    }

// ---------------------------------------------------------
// CMmsViewerDocument::CreateAppUiL
// ---------------------------------------------------------
//
CEikAppUi* CMmsViewerDocument::CreateAppUiL()
    {
    CMsgEditorAppUi* appui = new ( ELeave ) CMmsViewerAppUi();
    return appui;
    }

// ---------------------------------------------------------
// CMmsViewerDocument::ErrorResponseMessage
// ---------------------------------------------------------
//
TBool CMmsViewerDocument::ErrorResponseMessage()
    {
    TBool retVal( EFalse );
    if (    iOrigAttachmentCount == 0 
        &&  static_cast<CMmsClientMtm&>( Mtm() ).ResponseStatus() > KMmsResponseStatusOK 
        &&  static_cast<CMmsClientMtm&>( Mtm() ).ResponseText().Length() )
        {
        retVal = ETrue;
        }
    return retVal;
    }

// ---------------------------------------------------------
// CMmsViewerDocument::CreateNewL
// ---------------------------------------------------------
//
TMsvId CMmsViewerDocument::CreateNewL(TMsvId /*aService*/, TMsvId /*aDestFolder*/ )
    {
    __ASSERT_ALWAYS( !Mtm().Entry().Entry().ReadOnly(), Panic( EMmsNotSupportedInReadOnly ) );
    return 0;
    }

// ----------------------------------------------------
// GetAttachmentFileL
// ----------------------------------------------------
//
RFile CMmsViewerDocument::GetAttachmentFileL( TMsvAttachmentId aId )
    {
    RFile file;
    CMsvStore* store = Mtm().Entry().ReadStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& manager = store->AttachmentManagerL();
    file = manager.GetAttachmentFileL( aId );
    CleanupStack::PopAndDestroy( store );
    return file;
    }

            
//  End of File
