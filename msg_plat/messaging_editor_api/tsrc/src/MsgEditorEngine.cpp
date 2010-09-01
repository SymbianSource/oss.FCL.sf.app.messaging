/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MsgEditorEngine implementation*
*/



// ========== INCLUDE FILES ================================
#include "MsgEditorEngine.h"
#include <msvuids.h> 
#include <StifLogger.h>

#include <coemain.h>                        // CCoeEnv
#include <coecntrl.h>                       // for CCoeControl
#include <baclipb.h>                        // for CClipboard
#include <eikmenup.h>                       // for CEikMenuPane
#include <eikon.hrh>                        // for EEikMenuItemSymbolOn and -Indeterminate
#include <eikenv.h>                         // for EikonEnv
#include <eikbtgpc.h>                       // for CEikButtonGroupContainer
#include <txtrich.h>                        // for CRichText
#include <eikrted.h>
#include <aknnavide.h>                      // CAknNavigationDecorator
#include <apgicnfl.h>                       // apamasked bitmaps
#include <charconv.h>
#include <akneditstateindicator.h>
#include <aknindicatorcontainer.h>          // CAknIndicatorContainer
#include <aknenv.h>                         // iAvkonEnv
#include <aknwaitdialog.h>                  // CAknWaitDialog
#include <aknnotewrappers.h>
#include <eikstart.h>

#include <msvapi.h>                         // MTM server API
#include <msvids.h>                         // KMsvDraftEntryIdValue
#include <miutset.h>                        // KUidMsgTypeSMTP
#include <miutdef.h>                        // KMsvEmailTypeListMessageInPreparation
#include <miutmsg.h>                        // CImEmailOperation
#include <mtclbase.h>                       // TMsvPartList
#include <mtmdef.h>                         // KMsvMessagePartBody
#include <smtcmtm.h>                        // CSmtpClientMtm
#include <smtpset.h>                        // Smtp Settings
#include <apgwgnam.h>                       // for CApaWindowGroupName
#include <StringLoader.h>                   // StringLoader

#include <MuiuMsgEditorLauncher.h>

#include <NpdApi.h>                         // Notepad API
#include <CPbkMultipleEntryFetchDlg.h>      // Phonebook Multiple Entry Fetch API
#include <CPbkContactEngine.h>              // Phonebook Engine
#include <RPbkViewResourceFile.h>
#include <CPbkContactItem.h>                // Phonebook Contact

#include <avkon.hrh>
#include <avkon.rsg>
#include <avkon.mbg>

#include <MsgAttachmentModel.h>
#include <MsgAttachmentUtils.h>


#include <MsgEditorCommon.h>
#include <MsgEditorView.h>                  // for CMsgEditorView

#include <MsgEditor.hrh>                    // for enums
#include <MsgEditorAppUi.hrh>
#include <MsgEditorAppUi.rsg>               // atta dlg base resouce identifiers
#include <data_caging_path_literals.hrh>

#include <MsgBaseControl.h>                 // for CMsgBaseControl
#include <MsgAddressControl.h>              // for CMsgAddressControl
#include <MsgRecipientItem.h>               // for CMsgRecipientItem
#include <MsgBodyControl.h>                 // for CMsgBodyControl
#include <MsgAttachmentControl.h>           // for CMsgAttachmentControl
/*
_LIT( KAvkonMbm, "z:avkon.mbm" );

_LIT( KMsgEditorBody1, "c:\\TestFiles\\text\\MsgEditorBody.txt" );
_LIT( KMsgEditorBody2, "c:\\TestFiles\\text\\MsgEditorBody1.txt" );
_LIT( KFile1,          "c:\\TestFiles\\image\\pulu.jpg" );
_LIT( KFile2,          "c:\\TestFiles\\text\\huu.txt" );
_LIT( KFile3,          "c:\\TestFiles\\audio\\mail.wav" );

_LIT( KFile4, "c:\\dummy1.avkondemo" );

_LIT( KFile5, "c:\\notexists.wav" );
_LIT( KFile6, "c:\\notexists.txt" );
_LIT( KFile7, "c:\\notexists.jpg" );
*/


// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================
const TUid KUidMsgTypeSMS           = {0x1000102C};
// ========== MACROS =======================================
#define KMemoryStoreBuffSize 1000
// ========== LOCAL CONSTANTS AND MACROS ===================
/*
const TInt KMsgLength        = 100;   // Info message length
const TInt KMaxNameLength    = 100;
const TInt KMaxAddressLength = 100;
*/

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================



GLDEF_C void Panic( TMsgTestEditorPanics aPanic )
    {
    User::Panic( _L( "MsgTestEditor" ), aPanic );
    }

// ---------------------------------------------------------
//  MEMBER FUNCTIONS of CMsgDerAppUi
// ---------------------------------------------------------
CMsgEditorEngine::CMsgEditorEngine( CStifLogger* aLog ):
    iApp( NULL ),
    iDocument( NULL ),
    iCmEntry( NULL ),
    iInitialized( EFalse ),
    iLog( aLog )
    {
    
    }

CMsgEditorEngine* CMsgEditorEngine::NewL( CStifLogger* aLog )
    {
    CMsgEditorEngine* self = new ( ELeave ) CMsgEditorEngine( aLog );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }

CMsgEditorEngine::~CMsgEditorEngine()
    {
    if ( iDocument )
        {
        delete iDocument;
        iDocument = NULL;
        }
    if ( iApp )
        {
        delete iApp;
        iApp = NULL;
        }
    }

void CMsgEditorEngine::ConstructL()
    {
    iApp = new (ELeave)CDerApplication();
    iDocument = CDerEditorDocument::NewL( *iApp, iLog );
    }

CDerEditorDocument* CMsgEditorEngine::GetDocument()
    {
    return iDocument;
    }

TInt CMsgEditorEngine::Init()
    {
    if ( !iCmEntry )
        {
        TRAPD( err, iCmEntry = iDocument->Session().GetEntryL( KMsvRootIndexEntryId ) );
        if( err != KErrNone )
            {
            iInitialized = EFalse;
            return err;
            }
        }
    iTmEntry.iMtm = KUidMsgTypeSMS;
    iTmEntry.iType = KUidMsvServiceEntry;
    iTmEntry.SetVisible(EFalse);
    // create new message entry 
    TRAPD( err, iCmEntry->CreateL(iTmEntry) );
    if( err )
        {
        //for ARM Warnings
        }
    TRAPD( err1, iCmEntry->SetEntryL(iTmEntry.Id()) );
    if( err1 )
        {
        //for ARM Warnings
        }
    iInitialized = ETrue;
    return KErrNone;
    }

/**************************************************************
*Set MTM entry to be current context and compare
 * ************************************************************/
TBool CMsgEditorEngine::SetnGetMTMEntryL()
    {
    
    //Calling SetEntryWithoutNotificationL to Set given entry to be current context.
    TRAPD( err1, iDocument->SetEntryWithoutNotificationL( iTmEntry.Id() ));
    if( err1 )
        {
        //for ARM Warnings
        }    
    //Calling SetEntryL to Set given entry to be current context and notifies after change.
    TRAPD( err, iDocument->SetEntryL( iTmEntry.Id() ));
    if( err )
        {
        //for ARM Warnings
        }
    //Verify the entry
    if( iDocument->Entry() == iTmEntry )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

/**************************************************************
*Get the Session 
 * ************************************************************/
CMsvSession& CMsgEditorEngine::Session() const
    {
    return iDocument->Session();
    }

/**************************************************************
*CMsvEntry of the current context
 * ************************************************************/
TBool CMsgEditorEngine::CurrentEntry()
    {
    if( iDocument->CurrentEntry().EntryId() == iCmEntry->EntryId())
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

/**************************************************************
*Get the MTM 
 * ************************************************************/
TBool CMsgEditorEngine::Mtm()
    {
    CBaseMtm& baseMtm = iDocument->Mtm();
    if( baseMtm.Entry().Entry() == iTmEntry )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

/**************************************************************
*Get MTMUI , this function is to be depricated
 * ************************************************************/
TBool CMsgEditorEngine::MtmUi()
    {
    CBaseMtmUi& baseMtmUi = iDocument->MtmUi();
    return ETrue;
    }

/**************************************************************
**Get MtmUiData , this function is to be depricated
 * ************************************************************/
CBaseMtmUiData& CMsgEditorEngine::MtmUiData() const
	{
	return iDocument->MtmUiData();
	}

/**************************************************************
*current MtmUi
 * ************************************************************/
CBaseMtmUi& CMsgEditorEngine::MtmUiL() const
	{
	return iDocument->MtmUiL();
	}

/**************************************************************
*Current MtmUiData
 * ************************************************************/
CBaseMtmUiData& CMsgEditorEngine::MtmUiDataL() const
	{
	return iDocument->MtmUiDataL();
	}

/**************************************************************
*Loads client and ui mtm's
 * ************************************************************/
void CMsgEditorEngine::PrepareMtmL(const TUid aMtmType) 
	{
	return iDocument->PrepareMtmL(aMtmType);
	}

/**************************************************************
*Called (by the environment) when editor or viewer is opened embedded.
 * ************************************************************/
void CMsgEditorEngine::RestoreL( )
	{
	_LIT( KCMsgEditorEngine, "In CMsgEditorEngine RestoreL");
	iLog->Log( KCMsgEditorEngine );

	TEditorParameters params;
	params.iId = iTmEntry.Id()  ;
	params.iFlags =  EMsgReadOnly;
	params.iDestinationFolderId = iTmEntry.Id() ;
	params.iPartList = 0x1002;
	CBufStore* store = CBufStore::NewLC( KMemoryStoreBuffSize );
	RStoreWriteStream outStream;
	TStreamId id = outStream.CreateLC( *store );
	params.ExternalizeL( outStream );
	store->CommitL();
	CleanupStack::PopAndDestroy( );//outStream. 

	CStreamDictionary* dic = CStreamDictionary::NewLC();
	dic->AssignL( KUidMsvEditorParameterValue, id );
	outStream.CreateLC( *store );
	dic->ExternalizeL( outStream );
	store->CommitL();
	CleanupStack::PopAndDestroy( );// outstream

	// Restore the document from this store
	RStoreReadStream readStream;
	readStream.OpenLC( *store, id );

	 _LIT( KCMessageEditorDocument, "Calling CMessageEditorDocument RestoreL");
	iLog->Log( KCMessageEditorDocument );    

	//calls SetMsgAsReadL which in turn calls AddSingleOperationL
	 _LIT( KCMessageEditorDocumentLPL, "Inside Calling CMessageEditorDocument LaunchParametersL");
	iLog->Log( KCMessageEditorDocumentLPL );  

	 _LIT( KCMessageEditorDocumentPL, "Inside Calling CMessageEditorDocument PrepareToLaunchL");
	iLog->Log( KCMessageEditorDocumentPL ); 

	iDocument->RestoreL(*store , *dic );

	CleanupStack::PopAndDestroy( 3, store );// dic, store, readStream
	}

/**************************************************************
*Check if model is already created.
*************************************************************/
TBool CMsgEditorEngine::HasModel()
	{
	_LIT( KCMsgEditorEngine, "In CMsgEditorEngine HasModel");
	iLog->Log( KCMsgEditorEngine );

	 _LIT( KCMessageEditorDocument, "Calling CMessageEditorDocument HasModel");
	iLog->Log( KCMessageEditorDocument );

	return iDocument->HasModel();
	}

/**************************************************************
*Checks if document has finished launching
 * ************************************************************/
TBool CMsgEditorEngine::IsLaunched()
	{
	_LIT( KCMsgEditorEngine, "In CMsgEditorEngine IsLaunched");
	iLog->Log( KCMsgEditorEngine );

	 _LIT( KCMessageEditorDocument, "Calling CMessageEditorDocument IsLaunched");
	iLog->Log( KCMessageEditorDocument );

	return iDocument->IsLaunched();
	}

/**************************************************************
*Checks if document has finished launching
 * ************************************************************/
TBool CMsgEditorEngine::MediaAvailable()
	{
	_LIT( KCMsgEditorEngine, "In CMsgEditorEngine MediaAvailable");
	iLog->Log( KCMsgEditorEngine );
	return iDocument->MediaAvailable();
	}

/**************************************************************
*Checks if document has finished launching
 * ************************************************************/
void CMsgEditorEngine::SetEditorModelObserver(MMsgEditorModelObserver* /*aObs*/ )
	{
	_LIT( KCMsgEditorEngine, "In CMsgEditorEngine SetEditorModelObserver");
	iLog->Log( KCMsgEditorEngine );
	return iDocument->SetEditorModelObserver(NULL);
	}
// ---------------------------------------------------------
//  MEMBER FUNCTIONS of CMsgDerAppUi
// ---------------------------------------------------------

void CMsgDerAppUi::ConstructL()
    {

    }

CMsgDerAppUi::CMsgDerAppUi( CStifLogger* aLog ):
    iLog(aLog) 
    {
    }

CMsgDerAppUi::~CMsgDerAppUi()
    {


    }

void CMsgDerAppUi::LaunchViewL()
    {
 
    }

CMsgBaseControl* CMsgDerAppUi::CreateCustomControlL( TInt /*aControlType*/ )
    {
    return 0;
    }

void CMsgDerAppUi::EditorObserver( TMsgEditorObserverFunc /*aFunc*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/ )
    {

    }

void CMsgDerAppUi::DoMsgSaveExitL()
    {

    }

void CMsgDerAppUi::HandleCommandL( TInt /*aCommand*/ )
    {

    }


void CMsgDerAppUi::HandleForegroundEventL( TBool /*aForeground*/ )
    {
 
    }

CDerEditorDocument* CMsgDerAppUi::Document() const
    {
    return STATIC_CAST( CDerEditorDocument*, CMsgEditorAppUi::Document() );
    }

void CMsgDerAppUi::DynInitMenuPaneL( TInt /*aMenuId*/, CEikMenuPane* /*aMenuPane*/ )
    {
    
    }
void CMsgDerAppUi::CheckProtcdCMsgEditorAppUiFunsL(void)
    {
    iLog->Log(_L("In CheckProtcdCMsgEditorAppUiFuns"));
    
    iLog->Log(_L("Calling HandleEntryChangeL"));
    HandleEntryChangeL();
    iLog->Log(_L("Calling HandleEntryDeletedL"));
    HandleEntryDeletedL();
    iLog->Log(_L("Calling HandleMtmGroupDeinstalledL"));
    HandleMtmGroupDeinstalledL();
    iLog->Log(_L("Calling HandleGeneralErrorL"));
    HandleGeneralErrorL(0);
    iLog->Log(_L("Calling HandleCloseSessionL"));
    HandleCloseSessionL();
    iLog->Log(_L("Calling HandleServerFailedToStartL"));
    HandleServerFailedToStartL();
    iLog->Log(_L("Calling HandleServerTerminatedL"));
    HandleServerTerminatedL();
    iLog->Log(_L("Calling HandleMediaChangedL"));
    HandleMediaChangedL();
    iLog->Log(_L("Calling HandleMediaUnavailableL"));
    HandleMediaUnavailableL();
    iLog->Log(_L("Calling HandleMediaAvailableL"));
    HandleMediaAvailableL();
    iLog->Log(_L("Calling CheckProtcdCMsgEditorAppUiFuns"));
    HandleMediaIncorrectL();
    iLog->Log(_L("Calling HandleCorruptedIndexRebuildingL"));
    HandleCorruptedIndexRebuildingL();
    iLog->Log(_L("Calling HandleCorruptedIndexRebuiltL"));
    HandleCorruptedIndexRebuiltL();
    }
// ---------------------------------------------------------
//  MEMBER FUNCTIONS of CDerEditorDocument
// ---------------------------------------------------------

CDerEditorDocument* CDerEditorDocument::NewL( CEikApplication& aApp, CStifLogger* aLog)
    {
    CDerEditorDocument* doc = new( ELeave ) CDerEditorDocument( aApp, aLog );

    CleanupStack::PushL( doc );
    doc->ConstructL();
    CleanupStack::Pop();

    return doc;
    }

CDerEditorDocument::CDerEditorDocument( CEikApplication& aApp, CStifLogger* aLog ) :
    CMsgEditorDocument( aApp ),
    iLog( aLog )
    {
    }

CDerEditorDocument::~CDerEditorDocument()
    {
    delete iObjectObserver;
    }

void CDerEditorDocument::ConstructL()
    {
    CMsgEditorDocument::ConstructL();

    iObjectObserver = new( ELeave ) TTestObjectObserver( *this );

    AttachmentModel().SetObserver( iObjectObserver );
    
    PrepareMtmL( KUidMsgTypeSMTP );
    }

CEikAppUi* CDerEditorDocument::CreateAppUiL( )
    {
    return new( ELeave ) CMsgDerAppUi( iLog );
    }

TMsvId CDerEditorDocument::DefaultMsgFolder() const
    {
    return KMsvDraftEntryIdValue;
    }

TMsvId CDerEditorDocument::DefaultMsgService() const
    {
    return TUid( KUidMsgTypeSMTP ).iUid;
    }


TMsvId CDerEditorDocument::CreateNewL( TMsvId aServiceId, TMsvId /*aTargetId*/ )
    {
    // Load mtm. Set default context( root dir )
    PrepareMtmL( KUidMsgTypeSMTP );

    CSmtpClientMtm& clientMtm = STATIC_CAST( CSmtpClientMtm&, Mtm() );

    clientMtm.CreateMessageL( aServiceId );

    CMsvEntry& cEntryRef = clientMtm.Entry();
    TMsvId id = cEntryRef.EntryId();

    SetEntryL( id );

    return id;
    }

void CDerEditorDocument::EntryChangedL()
    {
    }

CMsgAttachmentModel* CDerEditorDocument::CreateNewAttachmentModelL( TBool aReadOnly )
    {
    return CMsgAttachmentModel::NewL( aReadOnly );
    }

void CDerEditorDocument::NotifyChanges( TMsgAttachmentCommand /*aCommand*/, CMsgAttachmentInfo* /*aAttachmentInfo*/ )
    {
    
    }

// ---------------------------------------------------------

TTestObjectObserver::TTestObjectObserver( CDerEditorDocument& aDocument ) :
    iDocument( aDocument )
    {
    }

//void TTestObjectObserver::NotifyChanges( TMsgAttachmentCommand aCommand )     {     }

void TTestObjectObserver::NotifyChanges( TMsgAttachmentCommand aCommand, CMsgAttachmentInfo* /*aAttachmentInfo*/ )
    {
    switch ( aCommand )
        {
        case MMsgAttachmentModelObserver::EMsgAttachmentAdded:
            {
            //MEBLOGGER_WRITE( "TTestObjectObserver::NotifyChanges: added" );
            break;
            }
        case MMsgAttachmentModelObserver::EMsgAttachmentRemoved:
            {
            //MEBLOGGER_WRITE( "TTestObjectObserver::NotifyChanges: removed" );
            break;
            }
        default:
            {
            break;
            }
        }
    }

RFile TTestObjectObserver::GetAttachmentFileL( TMsvAttachmentId /*aId*/ )
    {
    RFile result;
    return result;
    }

// ---------------------------------------------------------
//  MEMBER FUNCTIONS of CTestApplication
// ---------------------------------------------------------

CApaDocument* CDerApplication::CreateDocumentL()
    {
    return CDerEditorDocument::NewL( *this, NULL );
    }

TUid CDerApplication::AppDllUid() const
    {
    return KUidEngineApp;
    }

//End of File

