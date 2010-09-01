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
* Description:   Tests MessagingEditorAPI header files
*
*/




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "MessagingEditorAPITest.h"
#include <MsgAttachmentModel.h>
#include <MsgRecipientItem.h>
#include <MsgMimeTypes.h>
#include "MsgEditorEngine.h"
#include <smut.h>
#include "MsgFlagsnUtilsTest.h" 
//#include <MsgCheckNames.h>
#include <MsgEditorView.h>                  // for CMsgEditorView
#include <MsgExpandableControl.h>
#include <COEAUI.H>
#include <MsgEditor.HRH>
#include <EIKAPPUI.H>
#include <MsgEditorAppUi.rsg>
#include <MessagingEditorAPITest.rsg>
#include <MsgBodyControl.h>
#include <TXTRICH.H>
#include <MsgExpandableControlEditor.h>
#include <msgaddresscontrol.h>
#include <mmsconst.h>
#include <GDI.H>
#include <itemfinder.h>
#include <MsgEditorCommon.h>
#include <MsgAttachmentControl.h>
#include <MsgViewAttachmentsDialog.h>
#include <StringLoader.h>                   // StringLoader
#include "TestAttachDlg.h"
#include <coedef.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
_LIT( KName, "myname");
_LIT( KAddress, "NOKIA");
_LIT8( KEMailMessageMimeType, "message/rfc822" );

// MACROS
#define ATCHH_ID    3333
#define INVALID_ID  99860
#define NEWATCH_ID  7777
#define SIZE        10
#define DRMCOUNT    5
#define BUFLEN  	20
#define SOME_FLAGS  0
    
// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES
//enum 
//typedef 

// LOCAL FUNCTION PROTOTYPES


// FORWARD DECLARATIONS

    
// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMessagingEditorAPITest::Delete() 
    {
    AtchModelCleanUp();
	CCoeEnv::Static()->DeleteResourceFile(iResAppUi);
	CCoeEnv::Static()->DeleteResourceFile(iResView);
    iResView 	= 0;
    iResAppUi 	= 0;
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::RunMethodL( 
    CStifItemParser& aItem ) 
    {
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        //Test case for MsgAttachmentModel.h
        ENTRY( "MsgAttachmentModel", CMessagingEditorAPITest::TestMsgAttachmentModelL ),
        ENTRY( "TestBitmapforFileL", CMessagingEditorAPITest::TestBitmapforFileL ),        
        //Test cases for MsgAttachmentInfo.h
        ENTRY( "MsgAttachmentInfo", CMessagingEditorAPITest::TestMsgAttachmentInfo ),
        ENTRY( "MsgAttachmentInfoID", CMessagingEditorAPITest::TestAtchInfoID ),
        ENTRY( "MsgAtchInfoFileName", CMessagingEditorAPITest::TestAtchInfoFileName ),
        ENTRY( "MsgAtchInfoSizenFetched", CMessagingEditorAPITest::TestAtchInfoSizenFetched ),
        ENTRY( "MsgAtchInfoDatatype", CMessagingEditorAPITest::TestAtchInfoDataType ),
        ENTRY( "MsgAtchInfoSessionAtch", CMessagingEditorAPITest::TestAtchInfoSessionAttached ),
        ENTRY( "MsgAtchInfoSupportednIcon", CMessagingEditorAPITest::TestAtchInfoSupportednIcon ),
        ENTRY( "MsgAtchInfoSeparator", CMessagingEditorAPITest::TestAtchInfoSeparator ),
        ENTRY( "MsgAtchInfoDRMDataType", CMessagingEditorAPITest::TestAtchInfoDRMDataType ),
        ENTRY( "MsgAtchInfoSaved", CMessagingEditorAPITest::TestAtchInfoSaved ),
        ENTRY( "MsgAtchInfoEmpty", CMessagingEditorAPITest::TestAtchInfoEmptyAttachment ),
        //Test case for MsgRecipientItem.h class MsgRecipientItem
        ENTRY( "MsgRecipientItem", CMessagingEditorAPITest::TestMsgRecipientItemL ),
        //Test case for MsgRecipientItem.h class MsgRecipientList
        ENTRY( "MsgRecipientList", CMessagingEditorAPITest::TestMsgRecipientListL ),
        //Test case for MsgEditorDocument.h
        ENTRY( "MsgEditorDocument", CMessagingEditorAPITest::TestMsgEditorDocumentL),
        //Test case MsgEditorFlags.h
        ENTRY( "MsgEditorFlags", CMessagingEditorAPITest::TestMsgEditorFlagsL),
        //Test case MsgAttachmentUtils.h
        ENTRY( "MsgAttachmentUtils", CMessagingEditorAPITest::TestMsgAttachmentUtilsL),
        //Test case MsgEditorView.h
        ENTRY( "MsgEditorView", CMessagingEditorAPITest::TestMsgEditorViewL),
        //Test case MsgExpandableControl.h & MsgExpandableTextEditorControl.h
        ENTRY( "MsgExpandableControls", CMessagingEditorAPITest::TestMsgExpandableControlsL),
        //Test case MsgExpandableControlEditor.h
        ENTRY( "MsgExpandableEditor", CMessagingEditorAPITest::TestMsgExpandableControlEditorL),
        //EditorViewCleanup can be called from cfg
        ENTRY( "EditorViewCleanUp", CMessagingEditorAPITest::EditorViewCleanUp),
        //Test case MsgBodyControl.h
        ENTRY( "TestMsgBodyControlL", CMessagingEditorAPITest::TestMsgBodyControlL),
        //Creates MessageIterator for opening message
        ENTRY( "CreateMessageIteratorL", CMessagingEditorAPITest::CreateMessageIteratorL),
        //Open message with MessageIterator
        ENTRY( "OpenCurrentMessageTestL", CMessagingEditorAPITest::OpenCurrentMessageTestL),
        //Test case MsgBaseControl.h
        ENTRY( "TestMsgBaseControlL", CMessagingEditorAPITest::TestMsgBaseControlL),
        //Create Iterator to open message from Inbox
        ENTRY( "MsgIteratorForInboxL", CMessagingEditorAPITest::MsgIteratorForInboxL),
	    //Create Iterator to open message from Inbox
        ENTRY( "TestMsgEditorAppui", CMessagingEditorAPITest::TestMsgEditorAppUiL),
        //TestCase for MsgEditorCommon.h
        ENTRY( "TestMsgEditorCommon", CMessagingEditorAPITest::TestMsgEditorCommonL),
        //TestCase for MsgAttachmentControl
        ENTRY( "TestMsgAttachmentControlL", CMessagingEditorAPITest::TestMsgAttachmentControlL),
       //TestCase for MsgAttachmentDialog.h
        ENTRY( "TestMsgViewAttachmentDialogL", CMessagingEditorAPITest::TestMsgViewAttachmentDialogL),
        //TestCase for MsgAttachmentDialog.h
         ENTRY( "TestMsgViewAttachmentDialogproL", CMessagingEditorAPITest::TestMsgViewAttachmentDialogproL),        
        //TestCase for MsgAddressControl.h
        ENTRY( "TestMsgAddressControlL", CMessagingEditorAPITest::TestMsgAddressControlL),
        //For Creating MsgEditorView
        ENTRY( "CreateMsgEditorViewL", CMessagingEditorAPITest::CreateMsgEditorViewL),
        //For Creating MsgEditorView
        ENTRY( "CleanMessageStore", CMessagingEditorAPITest::CleanAllL),
        
        //ADD NEW ENTRY HERE ^
        // [test cases entries] - Do not remove
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::CreateCustomControlL
// from MMsgEditorObserver
// -----------------------------------------------------------------------------
//
CMsgBaseControl* CMessagingEditorAPITest::CreateCustomControlL(TInt /*aControlType*/ )
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::EditorObserver
// from MMsgEditorObserver
// -----------------------------------------------------------------------------
//
void CMessagingEditorAPITest::EditorObserver(TMsgEditorObserverFunc aFunc, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    if ( aFunc == MMsgEditorObserver::EMsgHandleFocusChange )
        {
        return;
        }
    }

//-----------------------------------------------------------------------------
// CMessagingEditorAPITest::HandleEdwinEventL
// Method from MMessageIteratorObserver class
//-----------------------------------------------------------------------------
void CMessagingEditorAPITest::HandleEdwinEventL( CEikEdwin* /*aEdwin*/, TEdwinEvent /*aEventType*/ )
    {
    return;
    }

//-----------------------------------------------------------------------------
// HandleIteratorEventL
// Method from MMessageIteratorObserver class
//-----------------------------------------------------------------------------
void CMessagingEditorAPITest::HandleIteratorEventL( TMessageIteratorEvent /*aEvent*/ )
	{
	//No Implementation
	}

//-----------------------------------------------------------------------------
// CMessagingEditorAPITest::OpCompleted
// Method from MMsvSingleOpWatcher class
//-----------------------------------------------------------------------------
void CMessagingEditorAPITest::OpCompleted( CMsvSingleOpWatcher& aOpWatcher, TInt aCompletionCode )
	{
	const TInt count = iOpWatchers->Count();
    TInt i = 0;
    for ( ; i < count; i++ )
        {
        // looking for right operation in array.
        if ( ( (*iOpWatchers)[i] ) == &aOpWatcher)
            {
            break;
            }
         }
     if(i >= count)
     	{
     	iLog->Log(_L("Watcher Not found"));
     	return;
     	}
     if ( aCompletionCode != KErrNone )
     	{
     	iLog->Log(_L("Operation Failed"));
     	}

    delete (*iOpWatchers)[i];
    iOpWatchers->Delete( i );
    iOpWatchers->Compress();
	}

//-----------------------------------------------------------------------------
// CMessagingEditorAPITest::NotifyChanges
// Method from MMsgAttachmentModelObserver class
//-----------------------------------------------------------------------------
void CMessagingEditorAPITest::NotifyChanges( TMsgAttachmentCommand /*aCommand*/, CMsgAttachmentInfo* /*aAttachmentInfo*/ )
    {
    return;
    }

//-----------------------------------------------------------------------------
// CMessagingEditorAPITest::GetAttachmentFileL
// Method from MMsgAttachmentModelObserver class
//-----------------------------------------------------------------------------
RFile CMessagingEditorAPITest::GetAttachmentFileL( TMsvAttachmentId aId )
    {
    if ( iMsgAttachModel )
        {
        //Check for number of Items
        TInt num = iMsgAttachModel->NumberOfItems();
        while( num >= 1 )
            {
            //Get msgAttachmentInfo to comare ID
            CMsgAttachmentInfo& msgInfo = iMsgAttachModel->AttachmentInfoAt( num-1 );
            //Compare Id of attachment
            if( msgInfo.AttachmentId() == aId )
                {
                //Get File
                TFileName name = msgInfo.FileName();
                RFile file; 
                RFs& fs = iAppEikonEnv->FsSession(); 
                TInt err = file.Open( fs, name, EFileRead );
                return file;
                }
            else
                {
                num--;
                }
            }
        }
    //No attachment found with given ID so --- Leave
    User::Leave( KErrNotFound );
    RFile file;
    return file;
    }

//MSGATTACHMENTMODEL_BEGIN
// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgAttachmentModelL
// Tests all exported functions of MsgAttachmentModel.h .
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgAttachmentModelL( CStifItemParser& aItem )
    {
    _LIT( KTestFunction, "CMsgAttachmentModel: All functions");
    iLog->Log( KTestFunction );    
    //Initializing iMsgAttachModel
    TRAPD( errModel, InitMsgAtchmodelL( aItem ));
    if( errModel != KErrNone )
        {
        iLog->Log( _L("Problem Creating MsgAttachementModel object"));
        return KErrCancel;
        }
    
    TInt   error = KErrNone;

    //SetObserver
    SetObserverL();
    
    //Modified flag test
    TBool flag =  iMsgAttachModel->IsModified();
    iMsgAttachModel->SetModified( !flag );
    if( iMsgAttachModel->IsModified() == flag )
        {
        iLog->Log( _L("CMsgAttachmentModel::IsModified/SetModified failed"));
        error = KErrCancel;
        }
    iMsgAttachModel->SetModified( flag );
    
    //check readonly
    if( iMsgAttachModel->IsReadOnly() != EFalse )
        {
        iLog->Log( _L("CMsgAttachmentModel::IsReadOnly failed"));
        error = KErrCancel;
        }
    
    //check fot attached file
    if( !iMsgAttachModel->IsFileAttached( iFileName ) )
        {
        iLog->Log( _L("CMsgAttachmentModel::IsFileAttached failed"));
        error = KErrCancel;
        }
    //Check with Invalid file name
    _LIT( KInvalidFile, "InvalidTestFile.txt" );
    if( iMsgAttachModel->IsFileAttached( KInvalidFile ))
        {
        iLog->Log( _L("CMsgAttachmentModel::IsFileAttached failed"));
        error = KErrCancel;
        }
    
    //Check for attached ID
    if( !iMsgAttachModel->IsIdAttached( iAtchId ) )
        {
        iLog->Log( _L("CMsgAttachmentModel::IsIdAttached failed"));
        error = KErrCancel;
        }
    //Check with invalid attachment ID
    TMsvAttachmentId invId = INVALID_ID;
    if( iMsgAttachModel->IsIdAttached( invId ) )
        {
        iLog->Log( _L("CMsgAttachmentModel::IsIdAttached failed"));
        error = KErrCancel;
        }    
    
    //this is ui related
    if( iMsgAttachModel->NumberOfItems() )
        {
        TRAPD( errView, iMsgAttachModel->ViewAttachmentL( 0 ));
        if( errView )
            {
            iLog->Log( _L("CMsgAttachmentModel::ViewAttachmentL leaves with %d error"),errView );
            error = KErrCancel;             
            }
        }
    
    //LoadResourcesL test
    TRAPD( err, iMsgAttachModel->LoadResourcesL());
    if( err != KErrNone )
        {
        iLog->Log( _L("CMsgAttachmentModel::GetAttachmentFileL failed"));
        error = KErrCancel;        
        }
    
    //GetAttachmentFile
    RFile file;
    TRAPD( errfile, file = iMsgAttachModel->GetAttachmentFileL( 0 ) )
    if( errfile != KErrNone && errfile != KErrNotSupported )
        {
        iLog->Log( _L("CMsgAttachmentModel::GetAttachmentFileL failed"));
        error = KErrCancel;
        }
    
    //DeleteAttachement test
    iMsgAttachModel->DeleteAttachment( iCount - 1 );
    if( iMsgAttachModel->NumberOfItems() != iCount - 1 )
        {
        iLog->Log( _L("CMsgAttachmentModel::DeleteAttachment failed"));
        error = KErrCancel;
        }
    else
        {
        iCount--;
        }
    //AddAttachmentL
    _LIT( KFileName,"notpresent" );
    TRAP( errfile , iMsgAttachModel->AddAttachmentL( KFileName, SIZE, iAtchId+1, EFalse, iMsgAttachModel->NumberOfItems() ));
    if( errfile )
        {
        _LIT( KError, "AddAttachmentL returned %d for file which is not present" );
        iLog->Log( KError, errfile );
        }
    iMsgAttachModel->Reset();
    iMsgAttachModel->AddAttachmentL( iFileName, SIZE, KMsvNullIndexEntryId, EFalse );
    iCount++;
    
    //GetAttachmentFile
    TRAP( errfile, file = iMsgAttachModel->GetAttachmentFileL( 0 ) )
    if( errfile != KErrNone && errfile != KErrNotSupported )
        {
        iLog->Log( _L("CMsgAttachmentModel::GetAttachmentFileL failed"));
        error = KErrCancel;
        }
    if( errfile == KErrNone )
        {
        CleanupClosePushL( file );
        //compare file name with retrieved filename
        TBuf16<256> fileName;
        file.Name( fileName );
        if( iFileName.Find( fileName ) == KErrNotFound )
            {
            iLog->Log( _L("CMsgAttachmentModel::GetAttachmentFileL failed"));
            error = KErrCancel;        
            }
        CleanupStack::PopAndDestroy( &file );
        }    
    
    //Reset
    iMsgAttachModel->Reset();
    
    if( iMsgAttachModel->NumberOfItems() != 0 )
        {
        iLog->Log( _L("CMsgAttachmentModel::Reset failed"));
        error = KErrCancel;        
        }
    else
        {
        iCount = 0;
        }
    
    return error;
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestBitmapforFileL
// MsgEditorDocument.h
// Tests all methods of MsgEditorDocument class.
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestBitmapforFileL( CStifItemParser& aItem )
    {
    _LIT( KTestFunction, "CMsgAttachmentModel: All functions");
     iLog->Log( KTestFunction );    
     //Initializing iMsgAttachModel
     TRAPD( errModel, InitMsgAtchmodelL( aItem ));
     if( errModel != KErrNone )
         {
         iLog->Log( _L("Problem Creating MsgAttachementModel object"));
         return KErrCancel;
         }
     
     TInt   error = KErrNone;
     SetObserverL();
    
     if( iMsgAttachModel->NumberOfItems() )
         {
         TPtrC string;
         TBool flag = EFalse;
         TInt  count = DRMCOUNT;
         do
             {
             //AttachmentInfoAt returns reference to attachmentinfo at given index
             CMsgAttachmentInfo& atchInfo = iMsgAttachModel->AttachmentInfoAt( iCount - 1 );             
             if( atchInfo.AttachmentId() != iAtchId && !flag )
                 {
                 iLog->Log( _L("CMsgAttachmentModel::AttachmentInfoAt failed"));
                 error = KErrCancel;
                 }

             if( flag )
                 {
                 TDataType mime( KEMailMessageMimeType );
                 atchInfo.SetDataType( mime );
                 }
             //BitmapForFileL
             while( count >= 0)
                 {
                 if( !flag )
                     {
                     CMsgAttachmentInfo::TDRMDataType type;
                     switch( count )
                         {
                         case CMsgAttachmentInfo::ENoLimitations:
                             type = CMsgAttachmentInfo::ENoLimitations;
                             break;
                         case CMsgAttachmentInfo::EForwardLockedOrCombinedDelivery:
                             type = CMsgAttachmentInfo::EForwardLockedOrCombinedDelivery;
                             break;
                         case CMsgAttachmentInfo::ESeparateDeliveryValidRights:
                             type = CMsgAttachmentInfo::ESeparateDeliveryValidRights;
                             break;
                         case CMsgAttachmentInfo::ECombinedDeliveryInvalidRights:
                             type = CMsgAttachmentInfo::ECombinedDeliveryInvalidRights;
                             break;
                         case CMsgAttachmentInfo::ESeparateDeliveryInvalidRights:
                             type = CMsgAttachmentInfo::ESeparateDeliveryInvalidRights;
                             break;
                         default: type = (CMsgAttachmentInfo::TDRMDataType)DRMCOUNT;
                             break;                         
                         }
                     atchInfo.SetDRMDataType( type );
                     count--;
                     }
                 else
                     {
                     count--;
                     }
                 
                 TRAPD( err, iMsgAttachModel->BitmapForFileL( atchInfo ));
                 if( err != KErrNone )
                     {
                     _LIT( KError, "CMsgAttachmentModel::BitmapForFileL failed with %d code" );
                     iLog->Log( KError, err );
                     error = err;        
                     }
                 }
             if( !flag )
                 {
                 iFileName.Format( iFilePath, &string ); 				 
                 iMsgAttachModel->AddAttachmentL( iFileName, SIZE, iAtchId+iCount, EFalse );
                 iCount++;
                 flag = ETrue;
                 count++;
                 }
             }
         while( aItem.GetNextString( string ) == KErrNone );
         }
     else
         {
         _LIT( KLog, "Test for BitmapForFileL & AttachmentInfoAt not Done");
         iLog->Log( KLog );
         error = KErrCancel;
         }
    return error;
    }

//MSGATTACHMENTMODEL_END

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgEditorDocumentL
// MsgEditorDocument.h
// Tests all methods of MsgEditorDocument class.
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgEditorDocumentL( CStifItemParser& /*aItem*/ )
    {
        _LIT( KTestMsgEditorDocument, "In TestMsgEditorDocument");
    iLog->Log( KTestMsgEditorDocument );
    
    //Create the Engine which creates the CMsgEditorDocument Class
    if( !iEngine )
        {
        iEngine = CMsgEditorEngine::NewL( iLog );
        }
    //Init the Engine var
    TInt error = iEngine->Init();
    if( error != KErrNone )
        {
        iLog->Log( _L("Initialization failure with Editor Engine"));
        return error;
        }
     
    _LIT( KSetnGetMTMEntryL, "CMsgEditorDocument::SetnGetMTMEntryL");
     iLog->Log( KSetnGetMTMEntryL );
     //Set the MTM Entry
    if( !iEngine->SetnGetMTMEntryL() )
        {
        error = KErrCancel;
        }
        
    _LIT( KCurrentEntry, "CMsgEditorDocument::CurrentEntry");
     iLog->Log( KCurrentEntry );
    if( !iEngine->CurrentEntry() )
        {
        error = KErrCancel;
        }
        
    _LIT( KMtm, "CMsgEditorDocument::Mtm");
        iLog->Log( KMtm );
    if( !iEngine->Mtm() )
        {
        error = KErrCancel;
        }
        
    _LIT( KMtmUi, "CMsgEditorDocument::MtmUi");
           iLog->Log( KMtmUi );
    if( !iEngine->MtmUi() )
        {
        error = KErrCancel;
        }
        
    _LIT( KMtmUiData, "CMsgEditorDocument::MtmUiData");
           iLog->Log( KMtmUiData );
    CBaseMtmUiData*  retValData = NULL;
    retValData = &(iEngine->MtmUiData());
    if( !retValData )
        {
        error = KErrCancel;
        }
        
    _LIT( KMtmUiL, "CMsgEditorDocument::MtmUiL");
    iLog->Log( KMtmUiL );
    CBaseMtmUi*  retVal = NULL;
    retVal = &(iEngine->MtmUiL());
    if(!retVal )
        {
        error = KErrCancel;
        }
        
    _LIT( KMtmUiDataL, "CMsgEditorDocument::MtmUiDataL");
     iLog->Log( KMtmUiDataL );
    CBaseMtmUiData*  retValDatal = NULL;
    retValDatal = &(iEngine->MtmUiDataL());
    if( !retValDatal )
        {
        error = KErrCancel;
        }
    
    TRAPD( err, iEngine->PrepareMtmL(KUidMsgTypeSMS));
    if(err == KErrNone)
        {
        _LIT( KPrepareMtmL, "CMsgEditorDocument::PrepareMtmL returns KErrNone");
        iLog->Log( KPrepareMtmL );
        }
    else
        {
        _LIT( KPrepareMtmL, "CMsgEditorDocument::PrepareMtmL returns error");
        iLog->Log( KPrepareMtmL );
        } 
    
    TRAP( err, iEngine->RestoreL());
    if(err == KErrNone)
        {
        _LIT( KRestoreL, "CMsgEditorDocument::RestoreL returns KErrNone");
        iLog->Log( KRestoreL );
        }
    else
        {
        _LIT( KRestoreL, "CMsgEditorDocument::RestoreL returns error");
        iLog->Log( KRestoreL );
        }
    
    if(iEngine->HasModel())
        {
        _LIT( KHasModel, "CMsgEditorDocument::HasModel returns TRUE");
        iLog->Log( KHasModel );
        }
    else
        {
        _LIT( KHasModel, "CMsgEditorDocument::HasModel returns FALSE");
        iLog->Log( KHasModel );
        }
    
    if(iEngine->IsLaunched())
        {
        _LIT( KIsLaunched, "CMsgEditorDocument::IsLaunched returns TRUE");
        iLog->Log( KIsLaunched );
        }
    else
        {
        _LIT( KIsLaunched, "CMsgEditorDocument::IsLaunched returns FALSE");
        iLog->Log( KIsLaunched );
        }
    if(iEngine->MediaAvailable())
        {
        _LIT( KIsLaunched, "CMsgEditorDocument::MediaAvailable returns TRUE");
        iLog->Log( KIsLaunched );
        }
    else
        {
        _LIT( KIsLaunched, "CMsgEditorDocument::MediaAvailable returns FALSE");
        iLog->Log( KIsLaunched );
        }
        
    _LIT( KSetEditorModelObserver, "call CMsgEditorDocument::SetEditorModelObserver");
    iLog->Log( KSetEditorModelObserver );
    iEngine->SetEditorModelObserver(NULL);
    
    return error;
    }


//MSGATTACHMENTINFO_BEGIN
// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgAttachmentInfo
// calls CMsgAttachmentInfo::NewL
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgAttachmentInfo( CStifItemParser& aItem )
    {    
    _LIT( KTestFunction, "MsgAttachmentInfo::NewL");
    iLog->Log( KTestFunction );
    if( InitMsgAtchInfo( aItem ) != KErrNone )
        {
        return KErrCancel;
        }
    CMsgAttachmentInfo& msgInfo = iMsgAttachModel->AttachmentInfoAt( 0 );    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestAtchInfoID
// Tests AttachmentId() and setAttachmentId() of MsgAttachmentInfo
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestAtchInfoID( CStifItemParser& aItem )
    {
    _LIT( KTestFunction, "MsgAttachmentInfo::AttachmentId(), setAttachmentId()");
    iLog->Log( KTestFunction );
    if( InitMsgAtchInfo( aItem ) != KErrNone )
        {
        return KErrCancel;
        }
    CMsgAttachmentInfo& msgInfo = iMsgAttachModel->AttachmentInfoAt( 0 );
    //AttachmentId, setAttachmentId
    TMsvAttachmentId atchId = NEWATCH_ID;
    msgInfo.SetAttachmentId( atchId );
    iLog->Log(_L("MsgAttachmentInf::AttachmentId()"));
    if ( msgInfo.AttachmentId() != atchId )
        {
        iLog->Log(_L("MsgAttachmentInf::AttachmentId() failed"));
        return KErrCancel;
        }
    msgInfo.SetAttachmentId( iAtchId );    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestAtchInfoFileName
// Tests FileName() and SetFileName() of MsgAttachmentInfo
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestAtchInfoFileName( CStifItemParser& aItem )
    {
    _LIT( KTestFunction, "MsgAttachmentInfo::FileName(), SetFileName()");
    iLog->Log( KTestFunction );
    if( InitMsgAtchInfo( aItem ) != KErrNone )
        {
        return KErrCancel;
        }
    CMsgAttachmentInfo& msgInfo = iMsgAttachModel->AttachmentInfoAt( 0 );
    
   
    //Filename, setfilename
    TPtrC string;
    AssignFilePath( aItem );
    if( aItem.GetNextString( string ) == KErrNone )
        {
        iFileName.Format( iFilePath, &string ); 

        //for checking shared condition(if file is already open)
        RFile file; 
        RFs& fs = iAppEikonEnv->FsSession(); 
        TInt err = file.Open( fs, iFileName, EFileWrite );
        msgInfo.SetFileName( iFileName );
        file.Close();
        //Normal file
        msgInfo.SetFileName( iFileName );        
        }  
    
    const TFileName& fileName = msgInfo.FileName();
    if ( iFileName.Compare( fileName ) != 0 )
        {
        iLog->Log(_L("MsgAttachmentInf::FileName() failed"));
        return KErrCancel;
        }
    
    //Providing invalid path for filename
    _LIT( KInvalidFile, "NoFile" );
    iFileName.Format( KInvalidFile );
    msgInfo.SetFileName( iFileName );
 
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestAtchInfoSizenFetched
// Tests Size(),SizeString(),IsFetched(), and SetFetched() of MsgAttachmentInfo
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestAtchInfoSizenFetched( CStifItemParser& aItem )
    {
    _LIT( KTestFunction, "MsgAttachmentInfo::Size(),SizeString(),IsFetched(),SetFetched()");
    iLog->Log( KTestFunction );
    if( InitMsgAtchInfo( aItem ) != KErrNone )
        {
        return KErrCancel;
        }
    CMsgAttachmentInfo& msgInfo = iMsgAttachModel->AttachmentInfoAt( 0 );
    
    //size, sizestring
    TUint size = msgInfo.Size();
    TBuf<KMsgAttaSizeStringLength> sizestring = msgInfo.SizeString();
    
    TBool fetched = msgInfo.IsFetched();
    msgInfo.SetFetched( !fetched );
    if ( msgInfo.IsFetched()== fetched )
        {
        iLog->Log(_L("MsgAttachmentInfo::IsFetched() failed"));
        return KErrCancel;
        }
    msgInfo.SetFetched( fetched );    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestAtchInfoDataType
// Tests DataType() and SetDataType() of MsgAttachmentInfo
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestAtchInfoDataType( CStifItemParser& aItem )
    {
    _LIT( KTestFunction, "MsgAttachmentInfo::DataType(),SetDataType()");
    iLog->Log( KTestFunction );
    if( InitMsgAtchInfo( aItem ) != KErrNone )
        {
        return KErrCancel;
        }
    CMsgAttachmentInfo& msgInfo = iMsgAttachModel->AttachmentInfoAt( 0 );
    
    //Datatype, setDatatype
    TDataType mime( KMsgMimeAudioSpMidi );
    const TDataType& mimeType = msgInfo.DataType();
    msgInfo.SetDataType( mime );
    if ( msgInfo.DataType().Uid() != mime.Uid() )
        {
        iLog->Log(_L("MsgAttachmentInfo::DataType() failed"));
        return KErrCancel;
        }
    msgInfo.SetDataType( mimeType );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestAtchInfoSessionAttached
// Tests SetAttachedThisSession() and IsAttachedThisSession() of MsgAttachmentInfo
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestAtchInfoSessionAttached( CStifItemParser& aItem )
    {
    _LIT( KTestFunction, "MsgAttachmentInfo::SetAttachedThisSession(),IsAttachedThisSession()");
    iLog->Log( KTestFunction );
    if( InitMsgAtchInfo( aItem ) != KErrNone )
        {
        return KErrCancel;
        }
    CMsgAttachmentInfo& msgInfo = iMsgAttachModel->AttachmentInfoAt( 0 );
    
    //AttachedThisSession
    TBool isAttached = msgInfo.IsAttachedThisSession();
    msgInfo.SetAttachedThisSession( !isAttached );
    if( msgInfo.IsAttachedThisSession() == isAttached )
        {
        iLog->Log(_L("MsgAttachmentInfo::IsAttachedThisSession() failed"));
        return KErrCancel;
        }
    msgInfo.SetAttachedThisSession( isAttached );    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestAtchInfoSupportednIcon
// Tests IsSupported(),SetSupported() and Icon() of MsgAttachmentInfo
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestAtchInfoSupportednIcon( CStifItemParser& aItem )
    {
    _LIT( KTestFunction, "MsgAttachmentInfo::IsSupported(),SetSupported(),Icon()");
    iLog->Log( KTestFunction );
    if( InitMsgAtchInfo( aItem ) != KErrNone )
        {
        return KErrCancel;
        }
    CMsgAttachmentInfo& msgInfo = iMsgAttachModel->AttachmentInfoAt( 0 );
    
    //Supported flag test
    TBool supported = msgInfo.IsSupported();
    msgInfo.SetSupported( !supported );
    if( msgInfo.IsSupported() == supported )
        {
        iLog->Log(_L("MsgAttachmentInfo::IsSupported() failed"));
        return KErrCancel;
        }
    msgInfo.SetSupported( supported );    
    CGulIcon* guiIcon = msgInfo.Icon();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestAtchInfoSeparator
// Tests IsSeparator() and SetSeparator() of MsgAttachmentInfo
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestAtchInfoSeparator( CStifItemParser& aItem )
    {
    _LIT( KTestFunction, "MsgAttachmentInfo::IsSeparator(),SetSeparator()");
    iLog->Log( KTestFunction );
    if( InitMsgAtchInfo( aItem ) != KErrNone )
        {
        return KErrCancel;
        }
    CMsgAttachmentInfo& msgInfo = iMsgAttachModel->AttachmentInfoAt( 0 );
    
    //Separator flag
    TBool separator = msgInfo.IsSeparator();
    msgInfo.SetSeparator( !separator );
    if( msgInfo.IsSeparator() == separator )
        {
        iLog->Log(_L("MsgAttachmentInfo::IsSeparator() failed"));
        return KErrCancel;
        }
    msgInfo.SetSeparator( separator );    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestAtchInfoDRMDataType
// Tests DRMDataType() and SetDRMDataType() of MsgAttachmentInfo
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestAtchInfoDRMDataType( CStifItemParser& aItem )
    {
    _LIT( KTestFunction, "MsgAttachmentInfo::DRMDataType(),SetDRMDataType()");
    iLog->Log( KTestFunction );
    if( InitMsgAtchInfo( aItem ) != KErrNone )
        {
        return KErrCancel;
        }
    CMsgAttachmentInfo& msgInfo = iMsgAttachModel->AttachmentInfoAt( 0 );
    
    //DRMDataType test
    CMsgAttachmentInfo::TDRMDataType type = CMsgAttachmentInfo::ENoLimitations;
    msgInfo.SetDRMDataType( type );
    if ( msgInfo.DRMDataType() != type )
        {
        iLog->Log(_L("MsgAttachmentInfo::DRMDataType() failed"));
        return KErrCancel;
        }
    msgInfo.SetDRMDataType( CMsgAttachmentInfo::EForwardLockedOrCombinedDelivery );    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestAtchInfoSaved
// Tests IsSaved() and SetSaved() of MsgAttachmentInfo
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestAtchInfoSaved( CStifItemParser& aItem )
    {
    _LIT( KTestFunction, "MsgAttachmentInfo::IsSaved(),SetSaved()");
    iLog->Log( KTestFunction );
    if( InitMsgAtchInfo( aItem ) != KErrNone )
        {
        return KErrCancel;
        }
    CMsgAttachmentInfo& msgInfo = iMsgAttachModel->AttachmentInfoAt( 0 );
    
    //Saved flag
    TBool saved = msgInfo.IsSaved();
    msgInfo.SetSaved( !saved );
    if( msgInfo.IsSaved() == saved )
        {
        iLog->Log(_L("MsgAttachmentInfo::IsSaved() failed"));
        return KErrCancel;
        }
    msgInfo.SetSaved( saved );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestAtchInfoEmptyAttachment
// Tests IsEmptyAttachment() and SetEmptyAttachment() of MsgAttachmentInfo
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestAtchInfoEmptyAttachment( CStifItemParser& aItem )
    {
    _LIT( KTestFunction, "MsgAttachmentInfo::IsEmptyAttachment(),SetEmptyAttachment()");
    iLog->Log( KTestFunction );
    if( InitMsgAtchInfo( aItem ) != KErrNone )
        {
        return KErrCancel;
        }
    CMsgAttachmentInfo& msgInfo = iMsgAttachModel->AttachmentInfoAt( 0 );
    
    //Empty attachment flag
    TBool empty = msgInfo.IsEmptyAttachment();
    msgInfo.SetEmptyAttachment( !empty );
    if( msgInfo.IsEmptyAttachment() == empty )
        {
        iLog->Log(_L("MsgAttachmentInfo::IsEmptyAttachment() failed"));
        return KErrCancel;
        }
    msgInfo.SetEmptyAttachment( empty );
    return KErrNone;
    }
//MSGATTACHMENTINFO_END


//MSGRECIPIENTITEM & LIST_BEGIN
// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgRecipientItem
// Tests all methods of MsgRecipientItem class.
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgRecipientItemL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestFunction, "CMsgRecipientItem: All functions");
    iLog->Log( KTestFunction );
    
    CMsgRecipientItem* msgRecpItem = CMsgRecipientItem::NewLC( KName, KAddress );
    TPtrC string;
    TInt err = KErrNone;
    
    //set and get name
    TRAPD( errname, msgRecpItem->SetNameL( KName ));
    if( errname != KErrNone )
        {
        iLog->Log(_L("MsgAttachmentInfo::SetNameL failed"));
        err = KErrCancel;       
        }
    string.Set( KName );
    HBufC16 *desString1 = msgRecpItem->Name();
    TPtr16 ptr = desString1->Des();
    if( string.Compare( ptr ) != 0 )
        {
        iLog->Log(_L("MsgAttachmentInfo::Name failed"));
        err = KErrCancel;
        }
    
    //set and get Address
    TRAPD( erraddress, msgRecpItem->SetAddressL( KAddress ) );
    if( erraddress != KErrNone )
        {
        iLog->Log(_L("MsgAttachmentInfo::SetAddressL failed"));
        err = KErrCancel;        
        }
    string.Set( KAddress );
    HBufC16 *desString2 = msgRecpItem->Address();
    ptr = desString2->Des();
    if( string.Compare( ptr ) != 0 )
        {
        iLog->Log(_L("MsgAttachmentInfo::Address() failed"));
        err = KErrCancel;
        }
  
  /*  //set and get contact ID (these functions are depricated)    
#ifndef _DEBUG
    //could be called in release mode
    TContactItemId contactID = KNullContactId;
    msgRecpItem->SetId( 1 );
    if( msgRecpItem->Id() != contactID )
        {
        iLog->Log(_L("MsgAttachmentInfo::SetId/Id failed"));
        err = KErrCancel;
        }
    else
        {
        iLog->Log(_L("MsgAttachmentInfo::SetId/Id called succesfully"));
        }
#endif

    */
    //set and get validation flag
    msgRecpItem->SetValidated( ETrue );
    if( !msgRecpItem->IsValidated())
        {
        iLog->Log(_L("MsgAttachmentInfo::SetValidated/IsValidated failed"));
        err = KErrCancel;
        }
    
    //set and get verified flag
    msgRecpItem->SetVerified( ETrue );
    if( !msgRecpItem->IsVerified() )
        {
        iLog->Log(_L("MsgAttachmentInfo::SetVerified/IsVerified failed"));
        err = KErrCancel;
        }
    
    //set and get contactlink (phobebook) -- Needs to be derived from MVPbkContackLink(abstarct class - To be Done)
    msgRecpItem->SetContactLink( (MVPbkContactLink*)NULL );
    msgRecpItem->ContactLink();
    
    CleanupStack::PopAndDestroy( msgRecpItem );
    
    return err;
    }


// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgRecipientList
// Tests all methods of MsgRecipientList class.
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgRecipientListL( CStifItemParser& aItem )
    {
    //NewL
    CMsgRecipientList* msgRecpList = CMsgRecipientList::NewL();
    CleanupStack::PushL( msgRecpList );
    TInt error = KErrNone;
    //Count
    int count = msgRecpList->Count();
    if ( !aItem.GetNextInt(count) )
        {
        for ( int items = 0; items < count; items++)
            {
            CMsgRecipientItem* msgRecpItem = CreateItemL();            
            //AppendL
            TRAPD( err, msgRecpList->AppendL( msgRecpItem ) );
            if( err != KErrNone)
                {
                iLog->Log(_L("CMsgRecipientList::AppendL failed"));
                err = KErrCancel;                
                }
            }
        }
    count = msgRecpList->Count();
    
    //InsertL
    TRAPD( err, msgRecpList->InsertL( count, CreateItemL()));
    if( msgRecpList->Count() != count+1 || err != KErrNone )
        {
        iLog->Log(_L("CMsgRecipientList::InsertL failed"));
        error = KErrCancel;
        }
    
    //At
    CMsgRecipientItem* msgRecpItem = msgRecpList->At( count - 1  );
    
    //operator []
    if( msgRecpList->operator [](count - 1) != msgRecpItem )
        {
        iLog->Log(_L("CMsgRecipientList::operator [] failed"));
        error = KErrCancel;
        } 
    
    CleanupStack::PopAndDestroy( msgRecpList );
    return error;
    }
//MSGRECIPIENTITEM & LIST_END


//MSGEDITORFLAGS & UTILS_BEGIN
// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgEditorFlagsL
// Tests all methods of MsgEditorFlags class.
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgEditorFlagsL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestFunction, "MsgEditorFlags: All functions");
    iLog->Log( KTestFunction );
    CMsgFlagsnUtilsTest* msgFlags = new (ELeave) CMsgFlagsnUtilsTest( iLog );
    CleanupStack::PushL( msgFlags );
    TInt error = KErrNone;
    
    if ( msgFlags->MsgFlagsDefCtor() != KErrNone )
        {
        _LIT( KError,"flags::clear failed" );
        iLog->Log( KError );
        error = KErrCancel;
        }
    
    if ( msgFlags->MsgFlagsCopyCtor() != KErrNone )
        {
        _LIT( KError,"flags::clear failed" );
        iLog->Log( KError );
        error = KErrCancel;
        }
    
    if ( msgFlags->MsgFlagsArgCtor() != KErrNone )
        {
        _LIT( KError,"flags::clear failed" );
        iLog->Log( KError );
        error = KErrCancel;
        }
    
    if( msgFlags->SetFlag() != KErrNone )
        {
        _LIT( KError,"flags::clear failed" );
        iLog->Log( KError );
        error = KErrCancel;        
        }
    
    if( msgFlags->IsSet() != KErrNone )
        {
        _LIT( KError,"flags::clear failed" );
        iLog->Log( KError );
        error = KErrCancel;        
        }    
    
    if ( msgFlags->Clear() != KErrNone )
        {
        _LIT( KError,"flags::clear failed" );
        iLog->Log( KError );
        error = KErrCancel;
        }
    
    if ( msgFlags->EqualToOperator() != KErrNone )
        {
        _LIT( KError,"flags::clear failed" );
        iLog->Log( KError );
        error = KErrCancel;
        }    
    
    TRAPD( err, msgFlags->ExternalizenInternalizeL() );
    if( err != KErrNone )
        {
        _LIT( KError,"flags::clear failed" );
        iLog->Log( KError );
        error = KErrCancel;        
        }
    
    CleanupStack::PopAndDestroy();
    return error;
    }


// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgAttachmentUtilsL
// Tests all methods of MsgAttachmentUtils class.
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgAttachmentUtilsL( CStifItemParser& /*aItem*/ )
    {
    CMsgFlagsnUtilsTest* msgUtils = new (ELeave) CMsgFlagsnUtilsTest( iLog );
    CleanupStack::PushL( msgUtils );
    TInt error = KErrNone;
    
	iAppEikonEnv->AppUiFactory()->ReadAppInfoResourceL(0,iAppEikonEnv->EikAppUi());
	
    if( msgUtils->GetFileNameFromBuffer() != KErrNone )
        {
        _LIT( KError,"MsgAttachmentUtils::GetFileNameFromBuffer failed" );
        iLog->Log( KError );
        error = KErrCancel;  
        }
    
    if( msgUtils->GetMsgEditorTempPath() != KErrNone )
        {
        _LIT( KError,"MsgAttachmentUtils::GetMsgEditorTempPath failed" );
        iLog->Log( KError );
        error = KErrCancel;        
        }       

    TRAPD( err, msgUtils->ThreePmtrsFetchL() );
    if( err != KErrNone )
        {
        _LIT( KError,"MsgAttachmentUtils::ThreePmtrsFetchL failed" );
        iLog->Log( KError );
        error = KErrCancel;        
        }    
      
    TRAP( err, msgUtils->FourPmtrsFetchL() );
    if( err != KErrNone )
        {
        _LIT( KError,"MsgAttachmentUtils::FourPmtrsFetchL failed" );
        iLog->Log( KError );
        error = KErrCancel;        
        }    
    
    TRAP( err, msgUtils->SixPmtrsFetchL() );
    if( err != KErrNone )
        {
        _LIT( KError,"MsgAttachmentUtils::SixPmtrsFetchL failed" );
        iLog->Log( KError );
        error = KErrCancel;        
        }    
    
    TRAP( err, msgUtils->SixPmtrsParamListFetchL() );
    if( err != KErrNone )
        {
        _LIT( KError,"MsgAttachmentUtils::SixPmtrsParamListFetchL failed" );
        iLog->Log( KError );
        error = KErrCancel;        
        }    
      
    TRAP( err, msgUtils->FetchAnyFileL() );
    if( err != KErrNone )
        {
        _LIT( KError,"MsgAttachmentUtils::FetchAnyFileL failed" );
        iLog->Log( KError );
        error = KErrCancel;        
        }
            
    CleanupStack::PopAndDestroy();      
    return error;
    }
//MSGEDITORFLAGS & UTILS_END

//CMsgEditorView
// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgEditorViewL
// Tests all exported functions of MsgEditorView.h 
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgEditorViewL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestEntry, "TestMsgEditorViewL_Entry" );
    iLog->Log( KTestEntry );
    //NewL
    TRAPD( err, CreateEditorViewL());
    if( err )
        {
        iLog->Log(_L("View Not Created"));
        return err;
        }
    else
        {
        TInt error = KErrNone;
        TRect rect = iAppEikonEnv->EikAppUi()->ClientRect();
        //SetEdwinObserverL
        TRAPD( err, iView->SetEdwinObserverL( this ));
        TRAP( err, iView->SetEdwinObserverL( NULL ));
        
        CItemFinder* itemFind = iView->ItemFinder();
        
        iView->SetControlsModified( ETrue );
        
        TRAPD( errExec, iView->ExecuteL( rect, EMsgComponentIdNull ));
        
        iLog->Log(_L("Calling AddControlstoEditorViewL"));
        //Add the control to the view
        TRAP(err, AddControlstoEditorViewL());
        if(err)
            {
            iLog->Log(_L("AddControlstoEditorViewL has left"));
            return err;
            }
        
        TRAP( errExec, iView->ExecuteL( rect, EMsgComponentIdNull ));
        
        //SetEdwinObserverL
        TRAP( err, iView->SetEdwinObserverL( this ));
        TRAP( err, iView->SetEdwinObserverL( NULL ));        
        if( err )
            {
            _LIT( KErrObserver, "SetEdwinObserverL leaves with %d error" );
            iLog->Log( KErrObserver, err );
            error = err;
            }
        else
            {
            _LIT( KErrObserver, "SetEdwinObserverL called successfully ");
            iLog->Log( KErrObserver );
            }

        //ExecuteL
        TRAP( errExec, iView->ExecuteL( rect, EMsgComponentIdTo ));
        if( errExec )
            {
            _LIT( KErrExec, "ExecuteL leaves with %d error" );
            iLog->Log( KErrExec, errExec );
            error = errExec;
            }
        else
            {
            _LIT( KErrExec, "ExecuteL called successfully ");
            iLog->Log( KErrExec );
            }        
        
        //SetFocus
        iView->SetFocus( EMsgComponentIdBody );
        //HandleScreenSizeChangeL
        iView->HandleScreenSizeChangeL( rect );
        CMsgBaseControl* ctrl = NULL;
        //FocusedControl
        ctrl = iView->FocusedControl();
        //ControlId
        TInt controlId = ctrl->ControlId();
        //MopNext
        MObjectProvider* mop = iView->MopNext();
        //FormComponent
        CCoeControl& formb = iView->FormComponent( EMsgBody );
        CCoeControl& formh = iView->FormComponent( EMsgHeader );
        //CItemFinder* ItemFinder();
        itemFind = iView->ItemFinder();
        //SetControlsModified
        iView->SetControlsModified( EFalse );
        //IsAnyControlModified
        TBool modified = iView->IsAnyControlModified();
        
        iView->SetControlsModified( ETrue );
        
        //ResetControls
        iView->ResetControls( EMsgHeader );
        modified = iView->IsAnyControlModified();
        iView->ResetControls( EMsgBody );
        modified = iView->IsAnyControlModified();
        //DeleteControlL
        TRAPD( errDel, iView->DeleteControlL( EMsgComponentIdBody ));
        if( errDel )
            {
            _LIT( KErrDel, "DeleteControl Leaves with %d error" );
            iLog->Log( KErrDel, errDel );
            error = errDel;
            }
        else
            {
            _LIT( KErrDel, "DeleteControl called successfully" );
            iLog->Log( KErrDel );            
            }
        //ResetControls
        iView->ResetControls();
        
        EditorViewCleanUp( );
        
        _LIT( KTestExit, "TestMsgEditorViewL_Exit" );
        iLog->Log( KTestExit );        
        return error;
        }    
    }

//
// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgExpandableControlsL
// Tests all exported functions of MsgEditorView.h 
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgExpandableControlsL( CStifItemParser& /*aItem*/ )
    {    
    //Create MessageEditorView 
    TRAPD( err, CreateEditorViewL());
    if( err )
        {
        iLog->Log(_L("MessageEditorView not Created"));
        return err;
        }
    else
        {
        iLog->Log(_L("MessageEditorView Created"));
        iappUi = CCoeEnv::Static()->AppUi();
        iappUi->AddToStackL(iView);

		 iLog->Log(_L("Calling AddControlstoEditorViewL"));
        //Add the control to the view
        TRAPD(err, AddControlstoEditorViewL());
        if(err)
            {
            iLog->Log(_L("AddControlstoEditorViewL has left"));
            return err;
            }
        TRect rect = iAppEikonEnv->EikAppUi()->ClientRect();

        TInt errExecute;
        //Launch the view
        TRAP(errExecute, iView->ExecuteL( rect, EMsgComponentIdTo ));
        if(errExecute)
            {
            iLog->Log(_L("MessageEditorView ExecuteL has left"));
            return errExecute;
            }

        //functions of MsgExpandableControl.h
        CMsgExpandableControl* exControl = (CMsgExpandableControl*)iView->FocusedControl();       
        exControl->Caption();
        
        //Get the Editor
        CEikRichTextEditor* editor = NULL;
        editor = &static_cast<CMsgExpandableControl*>( exControl )->Editor();
        if( editor )
            {
            //for ARM Warnings
            }

        iView->SetFocus( EMsgComponentIdBody );

        //Creating CMsgExpandableControl explicitly
        CMsgExpandableControl* msgExpCtrl = new CMsgExpandableControl;
        if(msgExpCtrl)
            {
            iLog->Log(_L("MsgExpandableControl is created"));
            msgExpCtrl->ConstructFromResourceL( R_TEST_EDITOR_TO );

            msgExpCtrl->Reset();

            if(msgExpCtrl->IsFocusChangePossible(EMsgFocusUp))
                {
                iLog->Log(_L("Focus change up is possible"));
                }
            else
                {
                iLog->Log(_L("Focus change up is not possible"));
                }

            if(msgExpCtrl->IsFocusChangePossible(EMsgFocusDown))
                {
                iLog->Log(_L("Focus change down is possible"));
                }
            else
                {
                iLog->Log(_L("Focus change down is not possible"));
                }

            if(msgExpCtrl->IsFocusChangePossible(TMsgFocusDirection(0)))
                {
                iLog->Log(_L("Focus change down is possible"));
                }
            else
                {
                iLog->Log(_L("Focus change down is not possible"));
                }   

            //Set the RichText to control
            TRAPD(err, msgExpCtrl->SetTextContentL(CreateTextL()));

			//Set the DescText to control
            _LIT(KText,"API Automation - Des!");
            TDesC des(KText);
            TRAP(err ,msgExpCtrl->SetTextContentL(des));

            HBufC* buf = HBufC::NewLC(BUFLEN);
            TPtr ptr = buf->Des();
            msgExpCtrl->TextContentStrippedL(ptr, BUFLEN);
            CleanupStack::PopAndDestroy(buf);

            //functions of MsgExpandableTextEditorControl
            CMsgExpandableTextEditorControl* exTextControl = (CMsgExpandableTextEditorControl*)iView->FocusedControl();


            TInt id = exTextControl->ControlId();
            CMsgBodyControl* body = static_cast<CMsgBodyControl*>(iView->ControlById(EMsgComponentIdBody));
            if (body)
                {
                iLog->Log(_L("Calling CMsgBodyControl CopyDocumentContentL method"));
                body->CopyDocumentContentL(body->TextContent(), msgExpCtrl->TextContent());
                }

            iLog->Log(_L("CMsgExpandableTextEditorControl ClipboardL"));
            exTextControl->ClipboardL(EMsgPaste);

            iLog->Log(_L("CMsgExpandableTextEditorControl SetPlainTextMode"));
            exTextControl->SetPlainTextMode( EFalse );

            iLog->Log(_L("CMsgExpandableTextEditorControl IsPlainTextMode"));
            exTextControl->IsPlainTextMode();

            iLog->Log(_L("CMsgExpandableTextEditorControl TextContent"));
            CRichText& rtext = exTextControl->TextContent();

            iLog->Log(_L("CMsgExpandableTextEditorControl SetMaxNumberOfChars"));
            exTextControl->SetMaxNumberOfChars( 100 );

            iLog->Log(_L("CMsgExpandableTextEditorControl SetCursorPosL"));
            exTextControl->SetCursorPosL( 0 );

            iLog->Log(_L("CMsgExpandableTextEditorControl ScrollL"));
            exTextControl->ScrollL( SIZE, EMsgScrollDown );
            exTextControl->ScrollL( SIZE, EMsgScrollUp );

#ifndef __WINSCW__
            exTextControl->ScrollL( SIZE, (TMsgScrollDirection)SIZE );
#endif
            
            iLog->Log(_L("CMsgExpandableTextEditorControl EditL"));
            exTextControl->EditL( EMsgSelectAll );
            exTextControl->EditL( EMsgUndo );

            iLog->Log(_L("CMsgExpandableTextEditorControl EditPermission"));
            exTextControl->EditPermission();

            delete msgExpCtrl;

            iappUi->RemoveFromStack(iView);
            EditorViewCleanUp();   

            return KErrNone;     
            }
        else
            {
            iLog->Log(_L("MsgExpandableControl creation is failed"));
            return KErrNone;
            }
        }
    }


//
// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgExpandableControlsL
// Tests all exported functions of MsgEditorView.h 
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgExpandableControlEditorL( CStifItemParser& /*aItem*/ )
    {
    _LIT(KTest, "TestMsgExpandableControlEditor_Entry");
    iLog->Log( KTest );
    TRAPD( err, CreateEditorViewL());
    if( err )
        {
        iLog->Log(_L("View Not Created"));
        return err;
        }
    else
        {
        iLog->Log(_L("Calling AddControlstoEditorViewL"));
		TRAP(err, AddControlstoEditorViewL());
		if(err)
			{
			iLog->Log(_L("AddControlstoEditorViewL has left"));
			return err;
			}
        iView->ExecuteL( iAppEikonEnv->EikAppUi()->ClientRect(), EMsgComponentIdTo );
        //Get Body (CMsgBaseControl)
        CMsgBodyControl* body = ( CMsgBodyControl* )iView->ControlById( EMsgComponentIdBody );
        // Set text to body so that clipboard functions could be called 
        TRAP(err , body->SetTextContentL(CreateTextL()));
        
        //Get Editor (CMsgExpandableControlEditor)
        CMsgExpandableControlEditor& exCtrEditor = ( CMsgExpandableControlEditor& )body->Editor();
        
        //DefaultHeight
        TInt dheight = exCtrEditor.DefaultHeight();
        //IsFirstLineVisible
        TBool visible = exCtrEditor.IsFirstLineVisible();
        //IsLastLineVisible
        visible = exCtrEditor.IsLastLineVisible();
        //AdjustLineHeightL
        TRAPD( err, exCtrEditor.AdjustLineHeightL());
        if( err )
            {
            _LIT( KErr, "CMsgExpandableControlEditor::AdjustLineHeightL leaves with %d error");
            iLog->Log( KErr, err );
            }
        else
            {
            _LIT( KErr, "CMsgExpandableControlEditor::AdjustLineHeightL Passed");
            iLog->Log( KErr );            
            }
        
        if( !exCtrEditor.CcpuIsFocused())
            {
            iView->SetFocus( EMsgComponentIdBody );
            body->EditL( EMsgSelectAll );
            }
        
        //ClipBoard functions
        //CcpuCanCopy and CcpuCopyL
        if( exCtrEditor.CcpuCanCopy() )
            {
            TRAP( err, exCtrEditor.CcpuCopyL());
            if( err )
                {
                _LIT( KErr, "CMsgExpandableControlEditor::CcpuCopyL leaves with %d error");
                iLog->Log( KErr, err );
                }
            else
                {
                _LIT( KErr, "CMsgExpandableControlEditor::CcpuCopyL Passed");
                iLog->Log( KErr );
                }
            }
        //CcpuCanCut and CcpuCutL
        if( exCtrEditor.CcpuCanCut() )
            {
            exCtrEditor.CcpuCutL();
            if( err )
                {
                _LIT( KErr, "CMsgExpandableControlEditor::CcpuCutL leaves with %d error");
                iLog->Log( KErr, err );
                }
            else
                {
                _LIT( KErr, "CMsgExpandableControlEditor::CcpuCutL Passed");
                iLog->Log( KErr );
                }            
            }
        //CcpuCanUndo and CcpuUndoL
        if( exCtrEditor.CcpuCanUndo() )
            {
            exCtrEditor.CcpuUndoL();
            if( err )
                {
                _LIT( KErr, "CMsgExpandableControlEditor::CcpuUndoL leaves with %d error");
                iLog->Log( KErr, err );
                }
            else
                {
                _LIT( KErr, "CMsgExpandableControlEditor::CcpuUndoL Passed");
                iLog->Log( KErr );
                }            
            } 
        //CcpuCanPaste and CcpuPasteL
        if( exCtrEditor.CcpuCanPaste() )
            {
            exCtrEditor.CcpuPasteL();
            if( err )
                {
                _LIT( KErr, "CMsgExpandableControlEditor::CcpuPasteL leaves with %d error");
                iLog->Log( KErr, err );
                }
            else
                {
                _LIT( KErr, "CMsgExpandableControlEditor::CcpuPasteL Passed");
                iLog->Log( KErr );
                }            
            }
        
        exCtrEditor.VirtualHeight();
        
        exCtrEditor.VirtualVisibleTop();
        
        exCtrEditor.Reset();
        
        EditorViewCleanUp( );
        
        //calls constructor with 2 parameters
        TUint32 flags = SOME_FLAGS ;
        CMsgExpandableControlEditor* editor =new (ELeave)CMsgExpandableControlEditor( flags, (MMsgBaseControlObserver*)NULL );
        delete editor;
        }
    _LIT(KTestExit, "TestMsgExpandableControlEditor_Exit");
    
    iLog->Log( KTestExit );
    
    return KErrNone;
    }

//
// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgBodyControlL
// Tests Exported functions from MsgBodyControl.h
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgBodyControlL()
	{
 	TRAPD( err, CreateEditorViewL())
    if( err )
        {
        iLog->Log(_L("View Not Created"));
        return err;
        }
    else
        {
        iView->AddControlFromResourceL(  R_TEST_EDITOR_TO, 
                                        EMsgAddressControl, 
                                        EMsgFirstControl,
                                        EMsgHeader );  
        //SetEdwinObserverL
        iView->SetEdwinObserverL( this );
        CMsgBodyControl* textControl = CMsgBodyControl::NewL( iView );
	    CleanupStack::PushL( textControl );
	    textControl->SetControlId( EMsgComponentIdBody );
		CParaFormatLayer* globalParaLayer = iAppEikonEnv->SystemParaFormatLayerL();
		CCharFormatLayer* globalCharLayer = iAppEikonEnv->SystemCharFormatLayerL();
		CRichText* bodyText = CRichText::NewL(globalParaLayer, globalCharLayer);
		CleanupStack::PushL(bodyText);
	    textControl->SetTextContentL(*bodyText);
	    textControl->Reset();
	    textControl->SetTextContentL(*bodyText);
	    CleanupStack::PopAndDestroy(bodyText);
	    TChar ch( 'a' );
	    textControl->InsertCharacterL( ch );
	    textControl->InsertTextL(_L("Hi!!!"));
	    textControl->InsertCharacterL( ch );
	    iView->AddControlL( textControl, EMsgComponentIdBody, 0, EMsgBody );
	    CleanupStack::Pop( textControl );
	    iView->ExecuteL( iAppEikonEnv->EikAppUi()->ClientRect(), EMsgComponentIdNull );
        }
    return KErrNone;
	}

//
// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgBaseControlL
// Tests Exported functions from MsgBaseControl.h
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgBaseControlL()
	{
	iLog->Log(_L("In TestMsgBaseControlL"));
	TRAPD( err, CreateEditorViewL())
    if( err )
        {
        iLog->Log(_L("View Not Created"));
        return err;
        }
    else
        {
        iLog->Log(_L("Calling AddControlstoEditorViewL"));
        //Add the control to the view
        TRAP(err, AddControlstoEditorViewL());
		if(err)
			{
			iLog->Log(_L("AddControlstoEditorViewL has left"));
			return err;
			}
        TRect rect = iAppEikonEnv->EikAppUi()->ClientRect();
        iView->ExecuteL( rect, EMsgComponentIdTo );
		CMsgBaseControl* baseControl = iView->FocusedControl(); 
		CleanupStack::PushL(baseControl);
		if(baseControl)
			{
			iLog->Log(_L("Base Control object created"));
			}
		baseControl->ConstructFromResourceL(R_TEST_EDITOR_TO);
		baseControl->Reset();
		baseControl->SetModified(ETrue);
		TBool modify = baseControl->IsModified(); // This should return True as we have set the modified flag
		if(modify)
			{
			iLog->Log(_L("IsModified passed"));
			}
		TMargins margin = baseControl->Margins();
		baseControl->SetMargins(margin);
		TInt virtualHeight = baseControl->VirtualHeight();
		TInt top = baseControl->VirtualVisibleTop();
		baseControl->SetupAutomaticFindAfterFocusChangeL(ETrue);
		CItemFinder* item = baseControl->ItemFinder();
		baseControl->ScrollL( 0, EMsgScrollDown );
		CleanupStack::Pop(baseControl);
        }
	CDerMsgBaseControl* derMsgBaseControl = new CDerMsgBaseControl(iLog); 
	TRAPD(errbasectrl, derMsgBaseControl->CheckMsgBaseCtrlPrtFunsL());
	if(errbasectrl)
	    {
	    iLog->Log(_L("CDerMsgBaseControl::CheckMsgBaseCtrlPrtFunsL has left with errid %d"),errbasectrl);
	    }
	else
	    {
	    iLog->Log(_L("CDerMsgBaseControl::CheckMsgBaseCtrlPrtFunsL is called successfully "));
	    }
	delete derMsgBaseControl;
    return KErrNone;
	}

//
// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgEditorCommonL
// Tests Exported functions from MsgEditorCommon.h
// -----------------------------------------------------------------------------
////Soumin
TInt CMessagingEditorAPITest::TestMsgEditorCommonL()
	{
	iLog->Log(_L("In TestMsgEditorCommonL"));
	TRAPD( err, CreateEditorViewL())
	if( err )
		{
		iLog->Log(_L("View Not Created"));
		return err;
		}
	else
		{
		iLog->Log(_L("Calling AddControlstoEditorViewL"));
		//Add the control to the view
		TRAP(err, AddControlstoEditorViewL());
		if(err)
		{
		iLog->Log(_L("AddControlstoEditorViewL has left"));
		return err;
		}
		
		//Validate them @Guns
		//Call the MsgEditorCommon functions
		TInt scrWdth = MsgEditorCommons::ScreenWidth();
		TInt scrhgth = MsgEditorCommons::ScreenHeigth();
		TInt edtrViewWdth = MsgEditorCommons::EditorViewWidth();   
		
		return KErrNone;
		}
	}
	
// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgEditorAppui
// Tests Exported functions from MsgEditorAppui.h
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgEditorAppUiL()
{
    _LIT( KTestMsgEditorAppUi, "In TestMsgEditorAppUi");
    iLog->Log( KTestMsgEditorAppUi );
    
    //Create the Engine which creates the CMsgEditorDocument Class
    if( !iEngine )
        {
        iEngine = CMsgEditorEngine::NewL( iLog );
        }

	CMsgDerAppUi* msgDerAppUI = (CMsgDerAppUi*)iEngine->GetDocument()->CreateAppUiL();	

	TRAPD(err, msgDerAppUI->HandleNavigationControlEventL(MMsgNaviPaneControlObserver::EMsgNaviRightArrowPressed ));
	if(err)
		{
		iLog->Log(_L("HandleNavigationControlEventL has left!"));
		}
	else
		{
		iLog->Log(_L("HandleNavigationControlEventL has not left!"));			
		}	
	//msgDerAppUI->CallToSenderQueryL(_L( "1234567" ), _L( "Joe Smith" ) );
	
	if(msgDerAppUI->IsLockedEntry())
		{
			iLog->Log(_L("IsLockedEntry returns ETrue"));
		}
	else
		{
			iLog->Log(_L("IsLockedEntry returns EFalse"));
		}
	
	TRAPD(errUpdate, msgDerAppUI->UpdateNaviPaneL());
	if(errUpdate)
		{
		iLog->Log(_L("UpdateNaviPaneL called left!"));
		return errUpdate;
		}
	else
		{
		iLog->Log(_L("UpdateNaviPaneL called has not left!"));			
		}
	
	iLog->Log(_L("Calling CheckCMsgEditorAppUiFuns "));
	TRAPD(errappui, msgDerAppUI->CheckProtcdCMsgEditorAppUiFunsL());
	if( errappui )
	    {
        iLog->Log(_L("CheckProtcdCMsgEditorAppUiFunsL called left!"));
        return errappui;	    
	    }
    else
        {
        iLog->Log(_L("CheckProtcdCMsgEditorAppUiFunsL called has not left!"));          
        }	
	
	return KErrNone;	
	
}
  
// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgAttachmentControlL
// Tests Exported functions from MsgAttachmentControl.h
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgAttachmentControlL( CStifItemParser& aItem )
{
	iLog->Log(_L("In TestMsgAttachmentControlL"));
	TRAPD( err, CreateEditorViewL())
	if( err )
		{
		iLog->Log(_L("View Not Created"));
		return err;
		}
	else
		{
		iLog->Log(_L("Calling AddControlstoEditorViewL"));
		//Add the control to the view
		TRAP(err, AddControlstoEditorViewL());
		if(err)
			{
			iLog->Log(_L("AddControlstoEditorViewL has left"));
			return err;
			}
		//Creating the 	CMsgAttachmentControl
		CMsgAttachmentControl* attachmentControl = CMsgAttachmentControl::NewL( *iView, *iView );
		if(!attachmentControl)
			{
			iLog->Log(_L("Not able to create CMsgAttachmentControl"));
			return KErrGeneral;
			}
		iLog->Log(_L("CMsgAttachmentControl Created"));
		CleanupStack::PushL( attachmentControl );

		TPtrC string;
		aItem.GetNextString ( string );

		//Calling the AppendAttachmentL
		TRAPD(errAttchCtrl, attachmentControl->AppendAttachmentL(string));
		if(errAttchCtrl)
			{
			iLog->Log(_L("AddControlstoEditorViewL has left with errorid %d"),errAttchCtrl);
			CleanupStack::PopAndDestroy();
			return errAttchCtrl;
			}
		else
			{
			iLog->Log(_L("AddControlstoEditorViewL has not left"));
			}
		
		//Create a Descriptor Array
		CDesCArrayFlat* descflat = new (ELeave) CDesCArrayFlat(BUFLEN);		
		//Get the String from Cfg file
		TPtrC stringdesc;
		aItem.GetNextString ( stringdesc );
		//Append the String to Desc Array
		descflat->AppendL(stringdesc);	
		//Add the array as Attachment
		TRAPD(erradddesc , attachmentControl->AddAttachmentsL(*descflat));
		if(erradddesc)
			{
			iLog->Log(_L("CMsgAttachmentControl::AddAttachmentsL with Descriptor has left with errorid %d"),erradddesc);
			}
		else
			{
			iLog->Log(_L("CMsgAttachmentControl::AddAttachmentsL with Descriptor is called successfully"));
			}
			
		//Get the Attachment at 0 location and Compare
		if(stringdesc.Compare(attachmentControl->Attachment(0)))
			{
			iLog->Log(_L("Attachment is same"));
			}
		else
			{
			iLog->Log(_L("Attachment is different"));
			}
		//Remove the Attachment 
		TRAPD(errremdesc , attachmentControl->RemoveAttachmentL(0));
		if(errremdesc)
			{
			iLog->Log(_L("CMsgAttachmentControl::RemoveAttachmentL with Descriptor has left with errorid %d"),errremdesc);
			}
		else
			{
			iLog->Log(_L("CMsgAttachmentControl::RemoveAttachmentL with Descriptor is called successfully"));
			}
		
		//Calling AddAttachmentsL with AttchModel
		TRAPD(erraddmdl , attachmentControl->AddAttachmentsL(*iMsgAttachModel));
		if(erraddmdl)
			{
			iLog->Log(_L("CMsgAttachmentControl::AddAttachmentsL has left with errorid %d"),erraddmdl);
			}
		else
			{
			iLog->Log(_L("CMsgAttachmentControl::AddAttachmentsL is called successfully"));
			}
		
		//Remove the Model
		TRAPD(errremindx , attachmentControl->RemoveAttachmentL(0));
		if(errremindx)
			{
			iLog->Log(_L("CMsgAttachmentControl::RemoveAttachmentL has left with errorid %d"),errremindx);
			}
		else
			{
			iLog->Log(_L("CMsgAttachmentControl::RemoveAttachmentL is called successfully"));
			}
	
		CleanupStack::PopAndDestroy();
		delete descflat;
		return KErrNone;
		}
}

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgViewAttachmentDialogL
// Tests Exported functions from MsgViewAttachmentsDialog.h
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgViewAttachmentDialogL( CStifItemParser& aItem )
    {
    _LIT( KTitle, "TestAttachmentDlg" );
    iLog->Log( KTitle );
    TRAPD( err , InitMsgAtchmodelL( aItem ));
    if( err )
        {
        _LIT( KError, "Attachmentmodel creation failded with %d error");
        iLog->Log( KError, err );
        return err;
        }
    else
        {
        _LIT( KError, "Attachmentmodel created");
        iLog->Log( KError );
        }
    iMsgAttachModel->SetObserver( this );
    
    HBufC* title = StringLoader::LoadLC( R_TEST_ATTACHMENTS, iAppEikonEnv );
    //Constructor
    CMsgViewAttachmentsDialog* dlg = new( ELeave )CMsgViewAttachmentsDialog( *title, R_TEST_VIEW_ATTACHMENT_MENU, *iMsgAttachModel );
    //ConstructL
    TRAP( err, dlg->ConstructL());
    if( err )
        {
        _LIT( KError, "AtchDlg ConstructL leaves %d error");
        iLog->Log( KError, err );
        return err;
        }
    else
        {
        _LIT( KError, "AtchDlg ConstructL called");
        iLog->Log( KError );
        }
    delete dlg;
    dlg = new( ELeave )CMsgViewAttachmentsDialog( *title, R_TEST_VIEW_ATTACHMENT_MENU, *iMsgAttachModel );
    CMsvEntry* noEntry  = NULL;
    TRAP( err, dlg->ConstructL( *noEntry ));
    if( err )
        {
        _LIT( KError, "AtchDlg ConstructL(param) leaves %d error");
        iLog->Log( KError, err );
        return err;
        }
    else
        {
        _LIT( KError, "AtchDlg onstructL(param) called");
        iLog->Log( KError );
        }
    iAppEikonEnv->AppUiFactory()->ReadAppInfoResourceL(0,iAppEikonEnv->EikAppUi());
    dlg->ExecuteLD( R_MEB_VIEW_ATTACHMENT_DIALOG );
    CleanupStack::PopAndDestroy();//title
    _LIT( KTestExit, "TestMsgViewAttachmentDialogL Passed:Exit");
    iLog->Log( KTestExit );
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgViewAttachmentDialogproL
// Tests protected functions from MsgViewAttachmentsDialog.h
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgViewAttachmentDialogproL( CStifItemParser& aItem )
    {
    _LIT( KTitle, "TestAttachmentDlgProtected" );
    iLog->Log( KTitle );
    TRAPD( err , InitMsgAtchmodelL( aItem ));
    if( err )
        {
        _LIT( KError, "Attachmentmodel creation failded with %d error");
        iLog->Log( KError, err );
        return err;
        }
    else
        {
        _LIT( KError, "Attachmentmodel created");
        iLog->Log( KError );
        }
    iMsgAttachModel->SetObserver( this );
    
    HBufC* title = StringLoader::LoadLC( R_TEST_ATTACHMENTS, iAppEikonEnv );
    //Constructor
    CTestEditorViewAttachmentDialog* testDlg = new( ELeave )CTestEditorViewAttachmentDialog( *title, R_TEST_VIEW_ATTACHMENT_MENU, *iMsgAttachModel );
    //ConstructL    
    TRAP( err, testDlg->ConstructL());
    iAppEikonEnv->AppUiFactory()->ReadAppInfoResourceL(0,iAppEikonEnv->EikAppUi());
    testDlg->ExecuteLD( R_MEB_VIEW_ATTACHMENT_DIALOG );
    CleanupStack::PopAndDestroy();//title
    _LIT( KTestExit, "TestMsgViewAttachmentDialogProL Passed:Exit");
    iLog->Log( KTestExit );    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::TestMsgAddressControlL
// Tests Exported functions from MsgAddressControl.h
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::TestMsgAddressControlL( CStifItemParser& aItem )
    {	
    iLog->Log(_L("In TestMsgAddressControlL"));

    //Check if EditorView is Existing
    if(iView)
        {
        CMsgAddressControl* toctrl = STATIC_CAST( CMsgAddressControl*, iView->ControlById( EMsgComponentIdTo ) );
        TPtrC cfgstring;
        aItem.GetNextString ( cfgstring );

        HBufC* ctrlstring = NULL;

        TRAPD( err , toctrl->GetFirstUnverifiedStringL(ctrlstring));
        if(ctrlstring->Compare(cfgstring))
            {
            iLog->Log(_L("Address are different"));
            }
        else
            {
            iLog->Log(_L("Address are same"));
            }
        //Get the Next Address
        aItem.GetNextString ( cfgstring );

        TRAP( err , toctrl->GetNextUnverifiedStringL(ctrlstring));
        if(ctrlstring->Compare(cfgstring))
            {
            iLog->Log(_L("Second Address are different"));
            }
        else
            {
            iLog->Log(_L("Second Address are same"));
            }

        CMsgRecipientArray* recips = toctrl->GetRecipientsL();

        if ( recips )
            {
            for( int i = 0; i < recips->Count(); i++ )
                {
                CMsgRecipientItem* item = recips->At( i );
                if ( ! item->IsVerified() )
                    {
                    item->SetVerified( ETrue );
                    }
                }

            TRAPD( err, toctrl->ReplaceUnverifiedStringL( *recips ) );
            if(err)
                {
                iLog->Log(_L("CMsgAddressControl::ReplaceUnverifiedStringL has left with Error id = %d"),err);
                }
            else
                {
                iLog->Log(_L("CMsgAddressControl::ReplaceUnverifiedStringL called successfully"));
                }
            }

        if(toctrl->HighlightUnverifiedStringL())
            {
            iLog->Log(_L("CMsgAddressControl::HighlightUnverifiedStringL returns TRUE"));
            }
        else
            {
            iLog->Log(_L("CMsgAddressControl::HighlightUnverifiedStringL returns FALSE"));
            }
        if(toctrl->HighlightUnvalidatedStringL())
            {
            iLog->Log(_L("CMsgAddressControl::HighlightUnvalidatedStringL returns TRUE"));
            }
        else
            {
            iLog->Log(_L("CMsgAddressControl::HighlightUnvalidatedStringL returns FALSE"));
            }

        //Check if any char is their before Semicolon
        if(toctrl->IsPriorCharSemicolonL())
            {
            iLog->Log(_L("CMsgAddressControl::IsPriorCharSemicolonL returns TRUE"));
            }
        else
            {
            iLog->Log(_L("CMsgAddressControl::IsPriorCharSemicolonL returns FALSE"));
            }

        //Make Address Highlight ON
        toctrl->SetAddressFieldAutoHighlight(ETrue);

        //Check if Adddress Highlight is ON or Not
        if(toctrl->AddressFieldAutoHighlight())
            {
            iLog->Log(_L("CMsgAddressControl::AddressFieldAutoHighlight is ON"));
            }
        else
            {
            iLog->Log(_L("CMsgAddressControl::AddressFieldAutoHighlight is OFF"));
            }
        iappUi->RemoveFromStack(toctrl);

        EditorViewCleanUp( );

        return KErrNone;  
        }
    else
        {
        return KErrNotFound;	
        }
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::CreateMsgEditorViewL
// Helper Function to Create View
// -----------------------------------------------------------------------------
//
TInt CMessagingEditorAPITest::CreateMsgEditorViewL()
    {	
    iLog->Log(_L("In CreateMsgEditorViewL"));
    TRAPD( err, CreateEditorViewL())
    if( err )
        {
        iLog->Log(_L("View Not Created"));
        return err;
        }
    else
        {
        iLog->Log(_L("Calling AddControlstoEditorViewL"));
        //Add the control to the view
        TRAP(err, AddControlstoEditorViewL());
        if(err)
            {
            iLog->Log(_L("AddControlstoEditorViewL has left"));
            return err;
            }
        //Launch The Editor
        TRAPD( errExec, iView->ExecuteL(  iAppEikonEnv->EikAppUi()->ClientRect(), EMsgComponentIdNull ));
        if( errExec )
            {
            iLog->Log(_L("CMsgEditorView::ExecuteL has left"));
            EditorViewCleanUp();
            return errExec;
            }
        //Get the Address Control 
        CMsgAddressControl* to = STATIC_CAST( CMsgAddressControl*, iView->ControlById( EMsgComponentIdTo ) );
        //Add the control to AppUI Stack and Setfocus
        iappUi = CCoeEnv::Static()->AppUi();
        iappUi->AddToStackL(to);
        iView->SetFocus( EMsgComponentIdTo );
        }
    return KErrNone;  
    }



//Helper Functions_Begin
// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::CreateItemL
// Creates a New CMsgRecipientItem
// -----------------------------------------------------------------------------
//
CMsgRecipientItem* CMessagingEditorAPITest::CreateItemL()
    {
    return CMsgRecipientItem::NewL( KName, KAddress );
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::InitMsgAtchmodelL
// 
// -----------------------------------------------------------------------------
TInt CMessagingEditorAPITest::InitMsgAtchmodelL( CStifItemParser& aItem )
    {
    TBool   readOnly    = EFalse;
    iCount = 0;
    if( !iMsgAttachModel )
        {
        iMsgAttachModel     = CMsgAttachmentModel::NewL( readOnly );
        }
    
    //Filename, setfilename
    TPtrC string;
    AssignFilePath( aItem );
    if( aItem.GetNextString ( string ) == KErrNone )
        {	   
        iFileName.Format(iFilePath, &string ); 
        iMsgAttachModel->AddAttachmentL( iFileName, SIZE, iAtchId, EFalse );
        TInt noOfAttachments = iMsgAttachModel->NumberOfItems();
        iCount++;
        }
    
    if ( !iCount )
        {
        //parameters have not been provided or improper parameters
        iLog->Log( _L("Problem with parameters") );
        return KErrCancel;
        }
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::SetObserverL
// 
// -----------------------------------------------------------------------------
void CMessagingEditorAPITest::SetObserverL()
    {
    //iEngine is used to set observer to msgAttachmentModel
    if( !iEngine )
        {
        iEngine             = CMsgEditorEngine::NewL( iLog );
        if ( iMsgAttachModel )
            {
            //Here GetDocument returns pointer to Document which is derived from-
            //-CMsgEditorDocument
            //SetObserver
            iMsgAttachModel->SetObserver( iEngine->GetDocument());
            }
        else
            {
            ;
            }
        }
    else
        {
        ;
        }
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::InitMsgAtchInfo
// 
// -----------------------------------------------------------------------------
TInt CMessagingEditorAPITest::InitMsgAtchInfo( CStifItemParser& aItem )
    {
    if( !iMsgAttachModel )
        {
        TRAPD( err, InitMsgAtchmodelL( aItem ) )
        if( err != KErrNone)
            {
            iLog->Log(_L("error creating attahcmentmodel") );
            }
        return err;
        }
    return KErrNone;
    }

void CMessagingEditorAPITest::AtchModelCleanUp()
    {
    if( iMsgAttachModel )
        {
        delete iMsgAttachModel;
        iMsgAttachModel = NULL;
        }
    if( iEngine )
        {
        delete iEngine;
        iEngine = NULL;
        }
    iCount = 0;
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::CreateEditorViewL
// 
// -----------------------------------------------------------------------------
void CMessagingEditorAPITest::CreateEditorViewL()
    {
    iLog->Log(_L("In CreateEditorViewL"));
    if( !iView )
        {
        iView  = CMsgEditorView::NewL( *this, CMsgEditorView::EMsgDoNotUseDefaultBodyControl );
        }
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::AddControlstoEditorViewL
// Adds Addresscontrol and bodycontrol to MsgEditorview
// -----------------------------------------------------------------------------
void CMessagingEditorAPITest::AddControlstoEditorViewL()
    {
	iLog->Log(_L("In AddControlstoEditorViewL"));    
    if( iView )
        {
        iLog->Log(_L("Calling AddControlFromResourceL"));    
        //Here Address control (To) is added to view
        //AddControlFromResourceL
        TRAPD(err , iView->AddControlFromResourceL(  R_TEST_EDITOR_TO, 
                                            EMsgAddressControl, 
                                            EMsgFirstControl,
                                            EMsgHeader ));
        if(err != KErrNone)
        {
        	iLog->Log(_L("AddControlFromResourceL has left with error id %d"),err);
        	EditorViewCleanUp();
        	User::Leave( err );
        }
        CMsgBodyControl* textControl = CMsgBodyControl::NewL( iView );
    
        CleanupStack::PushL( textControl );
        
        textControl->SetControlId( EMsgComponentIdBody );
    
        //body control is added to view
        //AddControlL
        iView->AddControlL( textControl, EMsgComponentIdBody, 0, EMsgBody );
      
        CleanupStack::Pop( textControl );
        }
    else
        {
        //Should not comehere (Do not call AddControlstoEditorViewL with out view )
        User::Leave( KErrNotFound );
        }
    }

void CMessagingEditorAPITest::AssignFilePath( CStifItemParser& aItem )
    {
    if( !iFilePathflag )
        {
        TPtrC attachfilepath;
#ifdef __WINSCW__
        aItem.GetNextString( attachfilepath );
        iFilePath = KTestFilePath;
#else
        aItem.GetNextString( attachfilepath );
        iFilePath = attachfilepath;
#endif
        iFilePathflag = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::EditorViewCleanUp
// Deletes created EditorView and resource file
// -----------------------------------------------------------------------------
TInt CMessagingEditorAPITest::EditorViewCleanUp()
    {
    if( iView )
        {
        delete iView;
        iView = NULL;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::CreateDummyMessageL
// Creates a dummy message into a specified folder
// -----------------------------------------------------------------------------
void CMessagingEditorAPITest::CreateDummyMessageL(const TMsvId aFolderId,TBool aBigMsg)	
	{
	TMsvEntry indexEntry;
	indexEntry.SetInPreparation(ETrue);
	indexEntry.iMtm = KUidMsgTypeSMS;
	indexEntry.iType = KUidMsvMessageEntry;
	indexEntry.iServiceId = iSmsClientMtm->ServiceId();
	indexEntry.iDate.HomeTime();

	//Create entry from this index entry
	iSmsClientMtm->SwitchCurrentEntryL( aFolderId );

	iSmsClientMtm->Entry().CreateL(indexEntry);

	// Set the MTM's active context to the new message
	iSmsId = indexEntry.Id();
	iSmsClientMtm->SwitchCurrentEntryL(iSmsId);

	//AddBody
	
		_LIT( KSMSBody, " Hi,Hello Phone Owner !!!");
		_LIT(KSMSBigBody,"Hi,This is a big message \n \n aaaa \n \n bbbb \n \n  cccc \n\n dddd \n\n eeee \n ffff");
	    if(aBigMsg)
	    	{
	    	CRichText& body = iSmsClientMtm->Body();
		    body.Reset();
		    body.InsertL( 0, KSMSBigBody );
		    indexEntry.iDescription.Set(KSMSBigBody);
	    	}
	    else
	    	{
	    	CRichText& body = iSmsClientMtm->Body();
		    body.Reset();
		    body.InsertL( 0, KSMSBody );
		    indexEntry.iDescription.Set(KSMSBody);
	    	}
	    
	    
	// Add addressee
	TBuf<15> phoneNumberString;
	phoneNumberString.Copy(KPhoneNumberString);
	iSmsClientMtm->AddAddresseeL(phoneNumberString);
	indexEntry.iDetails.Set( phoneNumberString );

	// Update index entry
	 iSmsClientMtm->Entry().ChangeL( indexEntry );
	 
	 // Update strore entry
	 iSmsClientMtm->SaveMessageL();	
	
	}

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::CreateMessageIteratorL
// 
// -----------------------------------------------------------------------------
TInt CMessagingEditorAPITest::CreateMessageIteratorL(CStifItemParser& aItem)
	{
	TInt val;
	aItem.GetNextInt(val);
	CleanFolderL(KMsvDraftEntryId, *iSession);
	if(val)
		{
		CreateDummyMessageL(KMsvDraftEntryId , ETrue);
		CreateDummyMessageL(KMsvDraftEntryId , ETrue);
		}
	else
		{
		CreateDummyMessageL(KMsvDraftEntryId , EFalse);
		CreateDummyMessageL(KMsvDraftEntryId , EFalse);
		}
	
	TInt count;
	TInt error = KErrGeneral;;
    CMsvEntrySelection* children;
    CMsvEntry* rootEntry;
    TMsvId sourceId= KMsvDraftEntryId;
    rootEntry = iSession->GetEntryL( sourceId );
    CleanupStack::PushL( rootEntry );
    count = rootEntry->Count(); 
    children = rootEntry->ChildrenL();
    CleanupStack::PopAndDestroy(); //rootEntry
    CleanupStack::PushL( children );
    if( count > 0 )
	    {
		TMsvId serviceId; // not used here but needed by GetEntry function
	    iSession->GetEntry( children->At(0), serviceId, iCurrentEntry );
	   
	    iMessageIterator = CMessageIterator::NewL( *iSession, iCurrentEntry );//Preserve the current entry in global variable 
	    															 // which can be used for future verifications
	    if(iMessageIterator)
	    	{
	       	iLog->Log(_L("Message Iterator object created successfully"));
	    	error = KErrNone;
	    	iMessageIterator->SetMessageIteratorObserver(this);// Set the observer to get the deletion events.
		    TInt index = iMessageIterator->CurrentMessageIndex(); 
		    iLog->Log(_L("Current Message Index is %d"),index);
		    TInt total = iMessageIterator->MessagesInFolder();
		    iLog->Log(_L("Total number of messages in Drafts are %d"),total);
	    	}
	    else
	    	{
	    	return error;
	    	}
	    }
	    
	   	CleanupStack::PopAndDestroy(); // children
	return error;
	}

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::MsgIteratorForInboxL
// 
// -----------------------------------------------------------------------------
TInt CMessagingEditorAPITest::MsgIteratorForInboxL()
	{
	CleanFolderL(KMsvGlobalInBoxIndexEntryId, *iSession);
	CreateDummyMessageL(KMsvGlobalInBoxIndexEntryId , EFalse);
	CreateDummyMessageL(KMsvGlobalInBoxIndexEntryId , EFalse);
	TInt count;
	TInt error = KErrGeneral;;
    CMsvEntrySelection* children;
    CMsvEntry* rootEntry;
    TMsvId sourceId= KMsvGlobalInBoxIndexEntryId;
    rootEntry = iSession->GetEntryL( sourceId );
    CleanupStack::PushL( rootEntry );
    count = rootEntry->Count(); 
    children = rootEntry->ChildrenL();
    CleanupStack::PopAndDestroy(); //rootEntry
    CleanupStack::PushL( children );
    if( count > 0 )
	    {
		TMsvId serviceId; // not used here but needed by GetEntry function
	    iSession->GetEntry( children->At(0), serviceId, iCurrentEntry );
	   
	    iMessageIterator = CMessageIterator::NewL( *iSession, iCurrentEntry );//Preserve the current entry in global variable 
	    															 // which can be used for future verifications
	    if(iMessageIterator)
	    	{
	       	iLog->Log(_L("Message Iterator object created successfully"));
	    	error = KErrNone;
	    	iMessageIterator->SetMessageIteratorObserver(this);// Set the observer to get the deletion events.
		    TInt index = iMessageIterator->CurrentMessageIndex(); 
		    iLog->Log(_L("Current Message Index is %d"),index);
		    TInt total = iMessageIterator->MessagesInFolder();
		    iLog->Log(_L("Total number of messages in Inbox are %d"),total);
	    	}
	    else
	    	{
	    	return error;
	    	}
	    }
	    
	   	CleanupStack::PopAndDestroy(); // children
	return error;
	}

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::CleanAllL
// 
// -----------------------------------------------------------------------------
void CMessagingEditorAPITest::CleanAllL()
	{

	iLog->Log(_L("In Clean All"));
	CleanBoxL(KMsvGlobalInBoxIndexEntryId,*iSession);
    CleanBoxL(KMsvGlobalOutBoxIndexEntryId,*iSession);
    CleanBoxL(KMsvSentEntryId,*iSession);
    CleanBoxL(KMsvDraftEntryId,*iSession);
    iLog->Log(_L("End of Clean All"));
    	
	}
	
// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::CleanBoxL
// 
// -----------------------------------------------------------------------------
void CMessagingEditorAPITest::CleanBoxL(TMsvId aBoxId, CMsvSession& aSession)
    {
    iLog->Log(_L("In CleanBoxL"));
    CMsvEntry* cEntry = NULL;
    // delete all messages from the specified box
    cEntry = aSession.GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(cEntry);
    cEntry->SetEntryL(aBoxId);
    // show invisible entries
    cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    CMsvEntrySelection* msvEntrySelection = cEntry->ChildrenWithMtmL(KUidMsgTypeSMS);
    CleanupStack::PushL(msvEntrySelection);

    CMsvEntrySelection* selection = NULL;
    cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    selection = cEntry->ChildrenWithMtmL(KUidMsgMMSNotification);
    CleanupStack::PushL( selection );
    if ( selection->Count() > 0 )
        {
        msvEntrySelection->AppendL( selection->Back( 0 ), selection->Count() );
        }
    CleanupStack::PopAndDestroy(); // selection

    int i;
    for (i = 0; i < msvEntrySelection->Count(); i++)
        {
        CMsvEntry* entry = aSession.GetEntryL( msvEntrySelection->At(i) );
        CleanupStack::PushL( entry );
        TMsvEntry tEntry = entry->Entry();
        tEntry.SetReadOnly(EFalse);
        entry->ChangeL(tEntry);
        cEntry->DeleteL( msvEntrySelection->At(i) );
        CleanupStack::PopAndDestroy( entry );
        }

    CleanupStack::PopAndDestroy(msvEntrySelection);
    CleanupStack::PopAndDestroy(cEntry);
    iLog->Log(_L("End of CleanBoxL"));
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::CleanFolderL
// 
// -----------------------------------------------------------------------------
void CMessagingEditorAPITest::CleanFolderL(TMsvId aBoxId, CMsvSession& aSession)
    {
    iLog->Log(_L("In CleanFolderL"));
    CMsvEntry* cEntry = NULL;
    // delete all messages from the specified box
    cEntry = aSession.GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(cEntry);
    cEntry->SetEntryL(aBoxId);
    // show invisible entries
    cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    CMsvEntrySelection* msvEntrySelection = cEntry->ChildrenWithMtmL(KUidMsgTypeSMS);
    CleanupStack::PushL(msvEntrySelection);

    CMsvEntrySelection* selection = NULL;
    cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    selection = cEntry->ChildrenWithMtmL(KUidMsgMMSNotification);
    CleanupStack::PushL( selection );
    if ( selection->Count() > 0 )
        {
        msvEntrySelection->AppendL( selection->Back( 0 ), selection->Count() );
        }
    CleanupStack::PopAndDestroy(); // selection

    int i;
    for (i = 0; i < msvEntrySelection->Count(); i++)
        {
        CMsvEntry* entry = aSession.GetEntryL( msvEntrySelection->At(i) );
        CleanupStack::PushL( entry );
        TMsvEntry tEntry = entry->Entry();
        tEntry.SetReadOnly(EFalse);
        entry->ChangeL(tEntry);
        cEntry->DeleteL( msvEntrySelection->At(i) );
        CleanupStack::PopAndDestroy( entry );
        }

    CleanupStack::PopAndDestroy(msvEntrySelection);
    CleanupStack::PopAndDestroy(cEntry);
    iLog->Log(_L("End of CleanFolderL"));
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::OpenCurrentMessageTestL
// 
// -----------------------------------------------------------------------------
TInt CMessagingEditorAPITest::OpenCurrentMessageTestL() 
	{
	CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL(*this);
    CleanupStack::PushL(singleOpWatcher);
    CMsvOperation* op;
    op = iMessageIterator->OpenCurrentMessageL( singleOpWatcher->iStatus, EFalse );
    singleOpWatcher->SetOperation(op);
    iOpWatchers->AppendL(singleOpWatcher);
    CleanupStack::Pop(); // singleOpWatcher
    iLog->Log(_L("OpenCurrentMessageL is called with Embedded status as EFalse"));
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::CreateTextL
// 
// -----------------------------------------------------------------------------
CRichText& CMessagingEditorAPITest::CreateTextL()
    {
    CRichText* richText; // rich text document
    CParaFormatLayer* paraFormatLayer;// global paragraph format layer
    CCharFormatLayer* charFormatLayer;// global character format layer
    paraFormatLayer=CParaFormatLayer::NewL(); // required para format
    charFormatLayer=CCharFormatLayer::NewL(); // required char format
    richText=CRichText::NewL(paraFormatLayer, charFormatLayer);
    TInt pos=0; // will be insertion position
    // insert some rich text
    richText->InsertL(pos,_L("API Automation - RichText"));
    return *richText;
    }

CDerMsgBaseControl::CDerMsgBaseControl(CStifLogger* aLog ):
iLog(aLog)
        {
        iLog->Log(_L("CDerMsgBaseControl constructor called"));
        }


void CDerMsgBaseControl::CheckMsgBaseCtrlPrtFunsL(void)
    {
    if(NormalTextFontL())
        {
        iLog->Log(_L("CMsgBaseControl::NormalTextFontL called"));
        }
    
    iLog->Log(_L("Calling CMsgBaseControl::LabelFontL"));
    LabelFontL();    

#ifndef _DEBUG    
    iLog->Log(_L("Calling CMsgBaseControl::ComponentControl"));
    CCoeControl*  ctrl = ComponentControl(0);
    if(ctrl)
        {
        iLog->Log(_L(" CMsgBaseControl::ComponentControl returns Control"));
        }
    else
        {
        iLog->Log(_L(" CMsgBaseControl::ComponentControl  returns NULL "));
        }

    iLog->Log(_L("Calling CMsgBaseControl::CountComponentControls"));
    TInt cnt = CountComponentControls(); 

    iLog->Log(_L("Calling CMsgBaseControl::SizeChanged"));
    SizeChanged();

    iLog->Log(_L("Calling CMsgBaseControl::FocusChanged"));
    FocusChanged(ENoDrawNow);
#endif
    
    iLog->Log(_L("Calling CMsgBaseControl::VirtualHeight"));
    TInt vht = VirtualHeight();
    iLog->Log(_L("CMsgBaseControl::VirtualHeight returns virtualht = %d"),vht);

    iLog->Log(_L("Calling CMsgBaseControl::VirtualVisibleTop"));
    TInt vvsbtop = VirtualVisibleTop();
    iLog->Log(_L("CMsgBaseControl::VirtualVisibleTop returns vvsbtop = %d"),vvsbtop);
    }

//Helper Functions_End


// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
