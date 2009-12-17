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
* Description:   Description
*
*/




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "MsgCentreUiUtilitiesApiTest.h"
#include <msgarrays.h>
#include <MsvFactorySettings.h>
#include <mtmstore.h>
#include <MuiuMessageIterator.h>
#include <gulicon.h>	// CGulIcon
#include <MsgFolderSelectionDialog.h>
#include <MuiuMsvProgressReporterOperation.h>
#include <SendUiConsts.h>   // KSenduiMtmSmsUid, KSenduiMtmSmtpUid
#include <miutset.h>    // KUidMsgTypeIMAP
#include <MTUDCBAS.H>   //For CBaseMtmUiData
#include <MuiuMsvUiServiceUtilities.h>
#include <MSVSTD.H>
#include <MuiuMsvRemoteOperationProgress.H>
#include <MuiuMsgEditorLauncher.h>
#include <muiuOperationWait.h>
#include <muiumsginfo.h>
#include <avkon.rsg>    // R_QGN_GRAF_WAIT_BAR_ANIM
#include <mmsconst.h>
#include <smut.h>
#include <TXTRICH.H> 
#include <avkon.mbg>
#include <COEHELP.H>
#include <COEMAIN.H>
#include <MsgCentreUiUtilitiesTest.rsg>





_LIT( KUiUtilities, "MsgCentreUiUtilitiesApiTest" );

 #ifdef __WINS__
 _LIT(KDir_ResFileName,"z:\\resource\\MsgCentreUiUtilitiesTest.rsc");
 #else
_LIT(KDir_ResFileName,"c:\\resource\\MsgCentreUiUtilitiesTest.rsc");
#endif

const TInt KArrayGranularity = 5;

_LIT(KPhoneNumberString,"9848223361");
const TInt KAOSafetyMargin = 204800;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMsgCentreUiUtilitiesApiTest::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMsgCentreUiUtilitiesApiTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CMsgCentreUiUtilitiesApiTest::ExampleL ),
        ENTRY( "CUidNameArrayTest", CMsgCentreUiUtilitiesApiTest::CUidNameArrayTestL ),
        ENTRY( "ResetL", CMsgCentreUiUtilitiesApiTest::ResetL ),
        ENTRY( "MtmUiDataL", CMsgCentreUiUtilitiesApiTest::MtmUiDataAndReleaseUiDataL ),
        ENTRY( "ClaimAndReleaseMtmUiL", CMsgCentreUiUtilitiesApiTest::ClaimAndReleaseMtmUiL ),
        ENTRY( "GetMtmUiAndSetContextLC", CMsgCentreUiUtilitiesApiTest::GetMtmUiAndSetContextL ),
        ENTRY( "ClaimMtmUiAndSetContextL", CMsgCentreUiUtilitiesApiTest::ClaimMtmUiAndSetContextL ),
        ENTRY( "GetMtmUiLC", CMsgCentreUiUtilitiesApiTest::GetMtmUiL ),
        ENTRY( "MessageIteratorNewL", CMsgCentreUiUtilitiesApiTest::MessageIteratorNewL ),
        ENTRY( "CurrentMessageTest", CMsgCentreUiUtilitiesApiTest::CurrentMessageTest ),
        ENTRY( "CurrentParentTest", CMsgCentreUiUtilitiesApiTest::CurrentParentTest ),
        ENTRY( "NextMessageExistsTest", CMsgCentreUiUtilitiesApiTest::NextMessageExistsTest ),
        ENTRY( "PreviousMessageExistsTest", CMsgCentreUiUtilitiesApiTest::PreviousMessageExistsTest ),
        ENTRY( "SetCurrentMessageTestL", CMsgCentreUiUtilitiesApiTest::SetCurrentMessageTestL ),
        ENTRY( "SetNextAndPreviousMessageTest", CMsgCentreUiUtilitiesApiTest::SetNextAndPreviousMessageTest ),
        ENTRY( "SelectFolderTestL", CMsgCentreUiUtilitiesApiTest::SelectFolderTestL ),
        ENTRY( "OpenCurrentMessageTestL", CMsgCentreUiUtilitiesApiTest::OpenCurrentMessageTestL ),
        ENTRY( "CMsvProgressReporterOperationNewLTestL", CMsgCentreUiUtilitiesApiTest::CMsvProgressReporterOperationNewLTestL ),
        ENTRY( "CMsvProgressReporterOperationTestL", CMsgCentreUiUtilitiesApiTest::CMsvProgressReporterOperationTestL ),
        ENTRY( "PublishNewDialogTestL", CMsgCentreUiUtilitiesApiTest::PublishNewDialogTestL ),
        ENTRY( "GetListOfAccounts", CMsgCentreUiUtilitiesApiTest::GetListOfAccountsL ),
        ENTRY( "GetListOfAccountsWithMTM", CMsgCentreUiUtilitiesApiTest::GetListOfAccountsWithMTML ),
        ENTRY( "IsValidEmailAddressL", CMsgCentreUiUtilitiesApiTest::IsValidEmailAddressL ),
		ENTRY( "DiskSpaceBelowCriticalLevel", CMsgCentreUiUtilitiesApiTest::DiskSpaceBelowCriticalLevelL ),
		ENTRY( "DiskSpaceBelowCriticalLevelWithOverhead", CMsgCentreUiUtilitiesApiTest::DiskSpaceBelowCriticalLevelWithOverheadL ),
		ENTRY( "CMsvRemoteOperationProgressNew", CMsgCentreUiUtilitiesApiTest::CMsvRemoteOperationProgressNewL ),
		ENTRY( "CMsgInfoMessageInfoDialogNew", CMsgCentreUiUtilitiesApiTest::CMsgInfoMessageInfoDialogNewL ),
		ENTRY( "OpenCurrentMessageEmbeddedTestL", CMsgCentreUiUtilitiesApiTest::OpenCurrentMessageEmbeddedTestL ),
		ENTRY( "CMuiuOperationWaitNewL", CMsgCentreUiUtilitiesApiTest::CMuiuOperationWaitNewL ),
		ENTRY( "ResolveAppFileNameTestL", CMsgCentreUiUtilitiesApiTest::ResolveAppFileNameTestL ),
		ENTRY( "CallToSenderQueryTestL", CMsgCentreUiUtilitiesApiTest::CallToSenderQueryTestL ),
		ENTRY( "InternetCallToSenderQueryTestL", CMsgCentreUiUtilitiesApiTest::InternetCallToSenderQueryTestL ),
		ENTRY( "InternetCallServiceTestL", CMsgCentreUiUtilitiesApiTest::InternetCallServiceTestL ),
		ENTRY( "DefaultServiceForMTMTestL", CMsgCentreUiUtilitiesApiTest::DefaultServiceForMTMTestL ),
		ENTRY( "CleanMessageStore", CMsgCentreUiUtilitiesApiTest::CleanAllL ),

 
                //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMsgCentreUiUtilitiesApiTest::ExampleL( CStifItemParser& aItem )
    {
    // Print to UI
    _LIT( KMsgCentreUiUtilitiesApiTest, "MsgCentreUiUtilitiesApiTest" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KMsgCentreUiUtilitiesApiTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KMsgCentreUiUtilitiesApiTest, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::Init
//  Initialisation function used to create messages 
//  into the Inbox folder.
// -----------------------------------------------------------------------------

void CMsgCentreUiUtilitiesApiTest::InitL()
	{
	CleanFolderL(KMsvGlobalInBoxIndexEntryId,*iSession);
	TInt count;
    CMsvEntry* rootEntry;
    TMsvId sourceId= KMsvGlobalInBoxIndexEntryId;
    rootEntry = iSession->GetEntryL( sourceId ); 
    CleanupStack::PushL( rootEntry );
    count = rootEntry->Count();
    switch(count)
    	{
    		case 0: 
    			{
    			CreateDummyMessageL(KMsvGlobalInBoxIndexEntryId);
    			CreateDummyMessageL(KMsvGlobalInBoxIndexEntryId);
    			CreateDummyMessageL(KMsvGlobalInBoxIndexEntryId);
    			break;
    			}
    		case 1: 
    			{
    			CreateDummyMessageL(KMsvGlobalInBoxIndexEntryId);
    			CreateDummyMessageL(KMsvGlobalInBoxIndexEntryId);
    			break;
    			}
    		case 2:
    			{
    			CreateDummyMessageL(KMsvGlobalInBoxIndexEntryId);
    			break;
    			}
    		default:
    			break;
    	}
    CleanupStack::PopAndDestroy(rootEntry);
	}
	
/*********************** CUidNameArray ******************************/
// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::CUidNameArrayTest
// Implementation for testing the methods of CUidNameArray class 
// -----------------------------------------------------------------------------

TInt CMsgCentreUiUtilitiesApiTest::CUidNameArrayTestL()
	{
	iLog->Log(_L("In CUidNameArrayTest"));
	TUid uId1;
     uId1 = KSenduiMtmImap4Uid;
     _LIT( KDummyName1, "Dummy name 1" );
     TUid uId2;
     uId2 = KSenduiMtmPop3Uid;
     _LIT( KDummyName2, "Dummy name 2" );
     TUid uId3;
     uId3 = KSenduiMtmPushMtm1Uid;
     _LIT( KDummyName3, "Dummy name 3" );
     TUid uId4;
     uId4 = KSenduiMtmBioUid;
     _LIT( KDummyName4, "Dummy name 4" );
    CUidNameArray* uidNameArray = new (ELeave) CUidNameArray( KArrayGranularity );
    CleanupStack::PushL( uidNameArray );
    uidNameArray->AppendL(TUidNameInfo( uId1, KDummyName1));
    uidNameArray->AppendL(TUidNameInfo( uId2, KDummyName2));
    uidNameArray->AppendL(TUidNameInfo( uId3, KDummyName3));
    uidNameArray->AppendL(TUidNameInfo( uId4, KDummyName4));
    iLog->Log(_L("calling CUidNameArray::Sort"));
    uidNameArray->Sort( ECmpNormal ); // Other Sorting types are: ECmpNormal8,ECmpNormal16,ECmpFolded
    								  // ECmpFolded8,ECmpFolded16,ECmpCollated,ECmpCollated8,ECmpCollated16
    CleanupStack::PopAndDestroy( uidNameArray );
    return KErrNone;
	}
	/*********************** MsvFactorySettings ******************************/
// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::ResetL
// Implementation for testing the methods of MsvFactorySettings class 
// -----------------------------------------------------------------------------	
TInt CMsgCentreUiUtilitiesApiTest::ResetL()
	{
	iLog->Log(_L("In ResetL"));
	MsvFactorySettings::ResetL( EMsvFactorySettingsLevelNormal );
	return KErrNone;
	}
	/*********************** MtmStore ******************************/
// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::MtmUiDataAndReleaseUiDataL
// Implementation for testing the loading and unloading the UiData Mtm  dll   
// -----------------------------------------------------------------------------
TInt CMsgCentreUiUtilitiesApiTest::MtmUiDataAndReleaseUiDataL()
	{
	iLog->Log(_L("In MtmUiDataAndReleaseUiDataL"));
	CBaseMtmUiData* mtmUiData = NULL;
	TRAPD( ignore, mtmUiData = &(iMtmStore->MtmUiDataL( KUidMsgTypeIMAP4 )) );
	if(mtmUiData)
		{
		iLog->Log(_L("MtmUiDataL passed"));
		}
	else
		{
		iLog->Log(_L("MtmUiDataL left with %d"),ignore);
		}
	TRAPD( ignore1, mtmUiData = &(iMtmStore->MtmUiDataL( KUidMsgTypeIMAP4 )) );
	if(mtmUiData)
		{
		iLog->Log(_L("MtmUiDataL passed"));
		}
	else
		{
		iLog->Log(_L("MtmUiDataL left with %d"),ignore1);
		}
	iMtmStore->ReleaseMtmUiData( KUidMsgTypeIMAP4 );
	iLog->Log(_L("Calling ReleaseMtmUiData for releasing the Uidata Mtm that is not loaded"));
	iMtmStore->ReleaseMtmUiData( KUidMsgMMSNotification );
	iLog->Log(_L("End of MtmUiDataAndReleaseUiDataL "));
	return KErrNone;	
	}
// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::ClaimAndReleaseMtmUiL
// Implementation for testing the loading and unloading the UiMtm dll   
// -----------------------------------------------------------------------------
TInt CMsgCentreUiUtilitiesApiTest::ClaimAndReleaseMtmUiL()
	{
	iLog->Log(_L("In ClaimAndReleaseMtmUiL"));
	CBaseMtmUi* baseMtmUi = NULL;
	TRAPD(error,baseMtmUi = &( iMtmStore->ClaimMtmUiL( KUidMsgTypeIMAP4 ) ));
	if(baseMtmUi)
		{
		iLog->Log(_L("ClaimMtmUiL passed"));
		}
	else
		{
		iLog->Log(_L("ClaimMtmUiL left with %d"),error);
	    return error;

		}
	iMtmStore->ReleaseMtmUi( KUidMsgTypeIMAP4 );
	return KErrNone;
	}
// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::GetMtmUiAndSetContextL
// Implementation for testing the loading  the UiMtm dll and setting the context 
// -----------------------------------------------------------------------------
TInt CMsgCentreUiUtilitiesApiTest::GetMtmUiAndSetContextL()
	{
	InitL();
	TInt count;
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
	    TMsvEntry currentEntry;
	    TMsvId serviceId; // not used here but needed by GetEntry function
	    iSession->GetEntry( children->At(0), serviceId, currentEntry );
	    
        CBaseMtmUi* baseMtmUi = NULL;
	    baseMtmUi = &(iMtmStore->GetMtmUiAndSetContextLC( currentEntry ));
	    if(baseMtmUi)
	    	{
	    	iLog->Log(_L("GetMtmUiAndSetContextLC passed"));
	    	CleanupStack::PopAndDestroy(  );//baseMtmUi
	    	}
	    else
    		{
    		iLog->Log(_L("GetMtmUiAndSetContextLC failed"));
    		CleanupStack::PopAndDestroy(  ); // children
    		return KErrGeneral;
    		}
	    }
   
    CleanupStack::PopAndDestroy(  ); // children
	return KErrNone;
	}
// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::ClaimMtmUiAndSetContextL
// Implementation for testing the loading and unloading the UiMtm dll   
// -----------------------------------------------------------------------------
TInt CMsgCentreUiUtilitiesApiTest::ClaimMtmUiAndSetContextL()
	{
	InitL();
	TInt count;
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
	    TMsvEntry currentEntry;
	    TMsvEntry dummyEntry;
	    TMsvId serviceId; // not used here but needed by GetEntry function
	    iSession->GetEntry( children->At(0), serviceId, currentEntry );
        CBaseMtmUi* baseMtmUi = NULL;
        TRAPD(error,baseMtmUi = &(iMtmStore->ClaimMtmUiAndSetContextL( currentEntry )));
        if(baseMtmUi)
        	{
        	iLog->Log(_L("ClaimMtmUiAndSetContextL passed"));
        	}
        else
        	{
        	iLog->Log(_L("ClaimMtmUiAndSetContextL left with %d"),error);
        	CleanupStack::PopAndDestroy(  ); // children
        	return KErrGeneral;
        	}
        }
    CleanupStack::PopAndDestroy(  ); // children
    return KErrNone;
	}
// --------------------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::GetMtmUiLC
// Implementation for testing the loading  UiMtm dll  using GetMtmUiLC of CMtmStore Class
// --------------------------------------------------------------------------------------
TInt CMsgCentreUiUtilitiesApiTest::GetMtmUiL()
	{
	CBaseMtmUi* baseMtmUi = NULL;
	baseMtmUi = &(iMtmStore->GetMtmUiLC( KUidMsgTypeIMAP4 ));
	if(baseMtmUi)
		{
		iLog->Log(_L("GetMtmUiLC passed"));
		}
	else
    	{
    	iLog->Log(_L("ClaimMtmUiAndSetContextL left "));
    	return KErrGeneral;
    	}
	CleanupStack::PopAndDestroy(); // baseMtmUi
	return KErrNone;
	}
	
	/***********************MessageIterator******************************/
// --------------------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::MessageIteratorNewL
// Method to create the Message Iterator instance. This instance will be used across all
// the methods of MessageIterator Class.
// Also CurrentMessageIndex() and MessagesInFolder() are also tested here
// --------------------------------------------------------------------------------------
	
TInt CMsgCentreUiUtilitiesApiTest::MessageIteratorNewL()
	{
	InitL();
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
	    iSession->GetEntry( children->At(2), serviceId, iCurrentEntry );
	   
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
// --------------------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::CurrentMessageTest
// Implementation  to test the CurrentMessage() method of MessageIterator Class
// 
// --------------------------------------------------------------------------------------
TInt CMsgCentreUiUtilitiesApiTest::CurrentMessageTest()
	{
	TMsvEntry currentEntry;
	currentEntry = iMessageIterator->CurrentMessage();
	TBool result = currentEntry.operator==(iCurrentEntry);
	if(result)
		{
		iLog->Log(_L("CurrentMessage() returned correct entry"));
		return KErrNone;
		}
	else
		{
		iLog->Log(_L("CurrentMessage() returned wrong entry"));
		return KErrGeneral;
		}
	}
// --------------------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::CurrentParentTest
// Implementation  to test the CurrentParent() method of MessageIterator Class
// 
// --------------------------------------------------------------------------------------
TInt CMsgCentreUiUtilitiesApiTest::CurrentParentTest()
	{
	TMsvEntry currentParent;
	currentParent = iMessageIterator->CurrentParent();
	TMsvId parentId = currentParent.Id();
	if(parentId == KMsvGlobalInBoxIndexEntryId)
		{
		iLog->Log(_L("CurrentParent() returned correct Parent Id"));
		return KErrNone;
		}
	else
		{
		iLog->Log(_L("CurrentParent() returned wrong Parent Id"));
		return KErrGeneral;
		}
	}
// --------------------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::NextMessageExistsTest
// Implementation  to test the NextMessageExists() method of MessageIterator Class
// --------------------------------------------------------------------------------------
TInt CMsgCentreUiUtilitiesApiTest::NextMessageExistsTest()
	{
	TBool result;
	TInt index = iMessageIterator->CurrentMessageIndex();
	result = iMessageIterator->NextMessageExists(); 
	if(result)
		{
		iLog->Log(_L("NextMessageExists() returned True ,means next message exists"));
		return KErrNone;
		}
	else
		{
		iLog->Log(_L("NextMessageExists() returned False , means there are no messages in Inbox or next message doesnot exist "));
		return KErrNone;
		}
	}
// --------------------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::PreviousMessageExistsTest
// Implementation  to test the PreviousMessageExists() method of MessageIterator Class
// --------------------------------------------------------------------------------------
TInt CMsgCentreUiUtilitiesApiTest::PreviousMessageExistsTest()
	{
	TBool result;
	result = iMessageIterator->PreviousMessageExists(); 
	if(result)
		{
		iLog->Log(_L("PreviousMessageExists() returned True ,means previous message exists"));
		return KErrNone;
		}
	else
		{
		iLog->Log(_L("PreviousMessageExists() returned False , means previous message doesnot exist "));
		return KErrNone;
		}
	}
// --------------------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::SetCurrentMessageTestL
// Implementation  to test the SetCurrentMessageL() method of MessageIterator Class
// --------------------------------------------------------------------------------------
TInt CMsgCentreUiUtilitiesApiTest::SetCurrentMessageTestL()
	{
    TRAPD(err,iMessageIterator->SetCurrentMessageL(iCurrentEntry));
    if(!err)
    	{
    	iLog->Log(_L("SetCurrentMessage passed"));
    	}
    else
    	{
    	iLog->Log(_L("SetCurrentMessage left with %d"),err);
    	return err;
    	}
    
    return KErrNone; 
	}
// --------------------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::SetNextAndPreviousMessageTest
// Implementation to test the SetNextMessage() and SetPreviousMessage() methods of MessageIterator Class
// --------------------------------------------------------------------------------------
TInt CMsgCentreUiUtilitiesApiTest::SetNextAndPreviousMessageTest()
	{
	TInt messages = iMessageIterator->MessagesInFolder();
	iLog->Log(_L("Total Messages in the folder are %d"),messages);
	TInt index = iMessageIterator->CurrentMessageIndex();
	TInt errorCount = (messages*2);
	for(TInt i = index; i < messages;i++)
		{
		TInt error = iMessageIterator->SetNextMessage();
		iLog->Log(_L("SetNextMessage returned %d for message:%d"),error,i);
		if((!error)  )
			{
			//It sould return KErrNone only when Iterator is not pointing to the last message.
			if((i != (messages -1)))
				{
				errorCount--;
				}
			else
				{
				iLog->Log(_L("SetNextMessage failed, returned KErrNone for the last message"));
				errorCount++;
				}
			}
		else
			{
			if((i == (messages -1)))
				{
				errorCount--;
				}
			else
				{
				iLog->Log(_L("SetNextMessage failed, returned error when the message is not the last message"));
				errorCount++;
				}
			
			}
		}
	index = iMessageIterator->CurrentMessageIndex();
	iLog->Log(_L("Current index is %d"),index);
	for(TInt j = index; j >=0 ;j--)
		{
		TInt error = iMessageIterator->SetPreviousMessage();
		iLog->Log(_L("SetPreviousMessage returned %d for message:%d"),error,j);
		if((!error) )
			{
			if((j != 0))
				{
				errorCount--;
				}
			else
				{
				iLog->Log(_L("SetPreviousMessage failed, returned KErrNone for first message"));
				errorCount++;
				}
			
			}
		else
			{
			if(j ==0)
				{
				errorCount--;
				}
			else
				{
				iLog->Log(_L("SetPreviousMessage failed, returned error even when the message is not the first message"));
				errorCount++;
				}
			
			}		
		}
	return errorCount;
	}
// --------------------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::OpenCurrentMessageTestL
// Implementation  to test the OpenCurrentMessageL() method of MessageIterator Class
// --------------------------------------------------------------------------------------
TInt CMsgCentreUiUtilitiesApiTest::OpenCurrentMessageTestL() 
	{
	CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL(*this);
    CleanupStack::PushL(singleOpWatcher);
    CMsvOperation* op;
    op = iMessageIterator->OpenCurrentMessageL( singleOpWatcher->iStatus, EFalse );
    singleOpWatcher->SetOperation(op);
    iOpWatchers->AppendL(singleOpWatcher);
    CleanupStack::Pop(); // singleOpWatcher
    iLog->Log(_L("OpenCurrentMessageL is called with Embedded status as ETrue"));
    return KErrNone;
	}
TInt CMsgCentreUiUtilitiesApiTest::OpenCurrentMessageEmbeddedTestL()
	{
	CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL(*this);
    CleanupStack::PushL(singleOpWatcher);
    CMsvOperation* op;
    op = iMessageIterator->OpenCurrentMessageL( singleOpWatcher->iStatus, ETrue );
    singleOpWatcher->SetOperation(op);
    iOpWatchers->AppendL(singleOpWatcher);
    CleanupStack::Pop(); // singleOpWatcher
    iLog->Log(_L("OpenCurrentMessageL is called with Embedded status as ETrue"));
    return KErrNone;
	
	}
	/*********************** MsgfolderSelectionDialog ******************************/
// --------------------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::SelectFolderTestL
// Implementation  to test the SelectFolderL() method of MsgfolderSelectionDialog Class
// --------------------------------------------------------------------------------------	
TInt CMsgCentreUiUtilitiesApiTest::SelectFolderTestL()
	{
	TMsvId destinationId = KMsvGlobalInBoxIndexEntryId;
	TBuf <20> buffer;
	buffer.Copy(_L("Move to: "));
	TBool selected = EFalse;
	TRAPD(err,selected = CMsgFolderSelectionDialog::SelectFolderL(destinationId,buffer));
	if(err)
		{
		iLog->Log(_L("CMsgFolderSelectionDialog::SelectFolderL left with %d"),err);
		}
	if(selected)
		{
		iLog->Log(_L("Folder has been selected"));
		}
	else
		{
		iLog->Log(_L("Cancel has been pressed"));
		}
		
	return KErrNone;
	}
	/*********************** MuiuMsvProgressReporterOperation ******************************/

// --------------------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::CMsvProgressReporterOperationNewLTestL
// Implementation  to test the NewL() methods of MuiuMsvProgressReporterOperation Class
// --------------------------------------------------------------------------------------
TInt CMsgCentreUiUtilitiesApiTest::CMsvProgressReporterOperationNewLTestL()
	{
	CMsvSingleOpWatcher* runningOperation;
    runningOperation = CMsvSingleOpWatcher::NewL( *this );
    CMsvProgressReporterOperation* msvProgressReporterOperation = CMsvProgressReporterOperation::NewL( *iSession, 
                                                        runningOperation->iStatus, EMbmAvkonQgn_note_progress );
    if(msvProgressReporterOperation)
    	{
    	iLog->Log(_L("CMsvProgressReporterOperation's object created successfully NewL1"));
    	}
    delete runningOperation;
    runningOperation = NULL;
    delete msvProgressReporterOperation;
    msvProgressReporterOperation = NULL;
    
    runningOperation = CMsvSingleOpWatcher::NewL( *this );
    TBool progressVisibilityDelayOff = EFalse;
    msvProgressReporterOperation = CMsvProgressReporterOperation::NewL( progressVisibilityDelayOff, *iSession, 
                                                        runningOperation->iStatus, EMbmAvkonQgn_note_progress );
    if(msvProgressReporterOperation)
    	{
    	iLog->Log(_L("CMsvProgressReporterOperation's object created successfully NewL2"));
    	}                                                   
    delete runningOperation;
    runningOperation = NULL;
    delete msvProgressReporterOperation;
    msvProgressReporterOperation = NULL;
    
    runningOperation = CMsvSingleOpWatcher::NewL( *this );
    progressVisibilityDelayOff = ETrue;
    msvProgressReporterOperation = CMsvProgressReporterOperation::NewL( progressVisibilityDelayOff, *iSession, 
                                                        runningOperation->iStatus );
    if(msvProgressReporterOperation)
    	{
    	iLog->Log(_L("CMsvProgressReporterOperation's object created successfully NewL3"));
    	}
    delete runningOperation;
    runningOperation = NULL;
    delete msvProgressReporterOperation;
    msvProgressReporterOperation = NULL;
    
    runningOperation = CMsvSingleOpWatcher::NewL( *this );
    progressVisibilityDelayOff = EFalse;
    TBool displayWaitAnimation = EFalse;
    msvProgressReporterOperation = CMsvProgressReporterOperation::NewL( progressVisibilityDelayOff, 
                                                                                                       displayWaitAnimation,
                                                                                                       *iSession, 
                                                                                                       runningOperation->iStatus,
                                                                                                      EMbmAvkonQgn_note_progress );
    if(msvProgressReporterOperation)
    	{
    	iLog->Log(_L("CMsvProgressReporterOperation's object created successfully NewL4"));
    	}
    delete runningOperation;
    runningOperation = NULL;
    delete msvProgressReporterOperation;
    msvProgressReporterOperation = NULL;
    
    runningOperation = CMsvSingleOpWatcher::NewL( *this );
    msvProgressReporterOperation = CMsvProgressReporterOperation::NewL( *iSession,  
                                            runningOperation->iStatus,0,TUid::Uid(0) ); 
	if(msvProgressReporterOperation)
    	{
    	iLog->Log(_L("CMsvProgressReporterOperation's object created successfully NewL5"));
    	}
    delete runningOperation;
    runningOperation = NULL;
    delete msvProgressReporterOperation;
    msvProgressReporterOperation = NULL;  
    
    runningOperation = CMsvSingleOpWatcher::NewL( *this );
    msvProgressReporterOperation = CMsvProgressReporterOperation::NewL( *iSession,  
                                            runningOperation->iStatus); 
	if(msvProgressReporterOperation)
    	{
    	iLog->Log(_L("CMsvProgressReporterOperation's object created successfully NewL6"));
    	} 
    msvProgressReporterOperation->MakeProgressVisibleL(EFalse); 
    delete runningOperation;
    runningOperation = NULL;
    delete msvProgressReporterOperation;
    msvProgressReporterOperation = NULL;                                             
    return KErrNone;
	}
// --------------------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::CMsvProgressReporterOperationTestL
// Implementation  to test the methods of MuiuMsvProgressReporterOperation Class
// --------------------------------------------------------------------------------------
TInt CMsgCentreUiUtilitiesApiTest::CMsvProgressReporterOperationTestL()
	{
	InitL();
	TInt count;
    CMsvEntry* rootEntry;
    TMsvId sourceId= KMsvGlobalInBoxIndexEntryId;
    rootEntry = iSession->GetEntryL( sourceId ); 
    CleanupStack::PushL( rootEntry );
    count = rootEntry->Count();
    iEntry=iSession->GetEntryL(KMsvRootIndexEntryId);
    if(count)
		{
		CMsvEntrySelection* children;
		children = rootEntry->ChildrenL();
		CleanupStack::PopAndDestroy(); //rootEntry
		iSelectedEntries->Reset();
		TMsvId id = ( *children ) [count - 1];  //Select Dummy message which  is created
		iSelectedEntries->AppendL( id );
		CMsvOperation* op=NULL;
		CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL(*this);
		CleanupStack::PushL(singleOpWatcher);

		CMsvProgressReporterOperation* progOp =
		CMsvProgressReporterOperation::NewL( EFalse, ETrue, *iSession,singleOpWatcher->iStatus, EMbmAvkonQgn_note_progress );
		CleanupStack::PushL(progOp);
		TBufC8<50> progressBuff;
		TBuf<50> titleBuf;
		TCoeHelpContext coeHelpContext;
		progressBuff = progOp->ProgressL();
		progOp->MakeProgressVisibleL(EFalse);
		progOp->MakeProgressVisibleL(ETrue);


		CMsvOperation* subOp=NULL;
		iEntry->SetEntryL( sourceId );
		
		
		TInt res = CCoeEnv::Static()->AddResourceFileL(KDir_ResFileName);
		progOp->SetTitleL( R_QTN_PROGRESS_TITLE );

		subOp = iEntry->DeleteL( *iSelectedEntries, progOp->RequestStatus());
		progOp->SetOperationL(subOp); // this takes ownership immediately, so no cleanupstack needed.

		progressBuff = progOp->ProgressL();
		progOp->MakeProgressVisibleL(EFalse);
		progOp->MakeProgressVisibleL(EFalse);
		progOp->MakeProgressVisibleL(ETrue);
		progOp->MakeProgressVisibleL(EFalse);
		progOp->MakeProgressVisibleL(ETrue);
		progOp->MakeProgressVisibleL(ETrue);

	    titleBuf.Copy( _L("Progress Info") ); 
		progOp->MakeProgressVisibleL(EFalse);
		//Dialog is not open
		//progOp->SetOperationL(subOp);
		progOp->SetHelpContext(coeHelpContext);
		progOp->SetTitleL(titleBuf); 
		progOp->SetTitleL(R_QTN_PROGRESS_TITLE);
		progOp->SetCanCancelL(EFalse);
		progOp->UnsetProgressDecoder();
		progOp->SetProgressDecoder(*this);
		progOp->SetSeeding( ETrue );
		progOp->SetProgressVisibilityDelay( ETrue );
		TRAPD( ignore, progOp->SetAnimationL( R_QGN_GRAF_WAIT_BAR_ANIM ) );
		if( ignore )
			{ 
			ignore = KErrNone;
			}
		progOp->MakeProgressVisibleL(ETrue);
		//Dialog is open
		progOp->SetHelpContext(coeHelpContext);
		progOp->SetTitleL(_L("Progress Info"));
		progOp->SetTitleL(R_QTN_PROGRESS_TITLE);
		progOp->SetCanCancelL(ETrue); 
		progOp->UnsetProgressDecoder();
		progOp->SetProgressDecoder(*this);
		progOp->SetSeeding( EFalse );
		progOp->SetProgressVisibilityDelay( EFalse);
		TRAP( ignore, progOp->SetAnimationL( R_QGN_GRAF_WAIT_BAR_ANIM ) );
		if( ignore )
			{ 
			ignore = KErrNone;
			}
		CMtmStore*  mtmStore;
		mtmStore = &(progOp->MtmStore());
		if(mtmStore)
			{
			iLog->Log(_L("MtmStore() passed;"));
			}
			else
			{
			iLog->Log(_L("MtmStore() failed;"));
			}
		op = progOp;
		CleanupStack::Pop(); // progOp
		CleanupStack::PushL(op);
		iOpWatchers->AppendL(singleOpWatcher);
		CleanupStack::Pop(2); // singleOpWatcher, op
		singleOpWatcher->SetOperation(op);
		CCoeEnv::Static()->DeleteResourceFile(res);
		}
		
	return KErrNone;
	}
// --------------------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::PublishNewDialogTestL
// Implementation  to test the PublishNewDialogL method of MuiuMsvProgressReporterOperation Class
// --------------------------------------------------------------------------------------
	
TInt CMsgCentreUiUtilitiesApiTest::PublishNewDialogTestL()
	{
	InitL();
	TInt count;
    CMsvEntry* rootEntry;
    TMsvId sourceId= KMsvGlobalInBoxIndexEntryId;
    rootEntry = iSession->GetEntryL( sourceId ); 
    CleanupStack::PushL( rootEntry );
    count = rootEntry->Count();
    iEntry=iSession->GetEntryL(KMsvRootIndexEntryId);
    if(count)
		{
		CMsvEntrySelection* children;
		//CMsvOperation* subOp=NULL;
		children = rootEntry->ChildrenL();
		CleanupStack::PopAndDestroy(); //rootEntry
		iSelectedEntries->Reset();
		TMsvId id = ( *children ) [count - 1];  //Select Dummy message which  is created
		iSelectedEntries->AppendL( id );
	//	CMsvOperation* op=NULL;
		CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL(*this);
		CleanupStack::PushL(singleOpWatcher);

		CMsvProgressReporterOperation* progOp = CMsvProgressReporterOperation::NewL( EFalse, ETrue, *iSession,singleOpWatcher->iStatus, EMbmAvkonQgn_note_progress );
		CleanupStack::PushL(progOp);
	    TInt dialogIndex = 0;
	    TUid categeoryUid(TUid::Uid(0));                                                   
	    progOp->PublishNewDialogL(dialogIndex,categeoryUid);  
	    progOp->MakeProgressVisibleL(EFalse);
	    progOp->PublishNewDialogL(dialogIndex,categeoryUid);
	    progOp->SetTitleL(_L("Progress Info"));  
	    iEntry->SetEntryL( sourceId );
	   // subOp = iEntry->DeleteL( *iSelectedEntries, progOp->RequestStatus());
	//	progOp->SetOperationL(subOp);
	//	progOp->PublishNewDialogL(dialogIndex,categeoryUid);
	//	singleOpWatcher->SetOperation(op);
    //	iOpWatchers->AppendL(singleOpWatcher);
	    CleanupStack::Pop(2); // singleOpWatcher,progOp
	    delete singleOpWatcher;
	    singleOpWatcher = NULL;
	    delete progOp;
	    progOp = NULL;
		}
		iLog->Log(_L(" End of PublishNewDialogTestL"));
    return KErrNone;                                                
	}
	/*********************** MsvUiServiceUtilities ******************************/
// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::GetListOfAccountsL
// Test method function to test MsvUiServiceUtilities::GetListOfAccountsL
// -----------------------------------------------------------------------------
//
	
	
TInt CMsgCentreUiUtilitiesApiTest::GetListOfAccountsL()
	{
	_LIT( KApiName, "In GetListOfAccountsL" );
    TestModuleIf().Printf( 0, KUiUtilities, KApiName );
    CMsvEntrySelection* selection = NULL;
    TRAPD(err,CMsvEntrySelection* selection = MsvUiServiceUtilities::GetListOfAccountsL( *iSession, EFalse ));
    if ((err != KErrNone) || (selection == NULL))
    	{
        iLog->Log(_L("GetListOfAccountsL failed "));
    	return err;
    	}
    else
    	{
		iLog->Log(_L("GetListOfAccountsL passed"));
     	return KErrNone;
    	}
	}

// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::GetListOfAccountsWithMTML
// Test method function to test MsvUiServiceUtilities::GetListOfAccountsWithMTML
// -----------------------------------------------------------------------------
//

	
TInt CMsgCentreUiUtilitiesApiTest::GetListOfAccountsWithMTML()
	{
	_LIT( KApiName, "In GetListOfAccountsWithMTML" );
    TestModuleIf().Printf( 0, KUiUtilities, KApiName );
    // Print to log file
    iLog->Log( KApiName );
    TUid uId = KSenduiMtmPop3Uid; 
    iLog->Log(_L("Calling GetListOfAccountsWithMTML"));
    CMsvEntrySelection* selection = NULL;
    TRAPD(err,CMsvEntrySelection* selection = MsvUiServiceUtilities::GetListOfAccountsWithMTML(*iSession,uId,EFalse));
    if ((err != KErrNone) || (selection == NULL))
		{
		iLog->Log(_L("GetListOfAccountsWithMTML Failed") );
		iLog->Log(_L("End of GetListOfAccountsWithMTML" ) );   	
		return err;
		}
    else
		{
		iLog->Log(_L("GetListOfAccountsWithMTML Passed") );
		iLog->Log(_L("End of GetListOfAccountsWithMTML" ) );
		return KErrNone;
		}
	}
// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::IsValidEmailAddressL
// Test method function to test MsvUiServiceUtilities::IsValidEmailAddressL
// -----------------------------------------------------------------------------
//
TInt CMsgCentreUiUtilitiesApiTest::IsValidEmailAddressL()
	{
	_LIT( KApiName, "In IsValidEmailAddressL" );
    _LIT(KEmailAddress1,"Abc.xyz@Yahoo.com");
    _LIT(KEmailAddress2,"Abc@Not Valid.com");
    _LIT(KEmailAddress3,"@Yahoo.com");//inv
    _LIT(KEmailAddress4,"A bc@.com");
    _LIT(KEmailAddress5,"Abc.xyz@.com");
    _LIT(KEmailAddress6,"@NotValid.com");//inv
    _LIT(KEmailAddress7,".@NotValid.com");
    _LIT(KEmailAddress8,"NotValid");
    _LIT(KEmailAddress9,"NotValid@cvkdjv");
    
    
    TestModuleIf().Printf( 0, KUiUtilities, KApiName );
    // Print to log file
    iLog->Log(_L("Calling IsValidEmailAddressL"));
    TBool valid = EFalse;
    TRAPD(err,TBool valid = MsvUiServiceUtilities::IsValidEmailAddressL(KEmailAddress1));
      
    if (err != KErrNone)
    	{
        iLog->Log(_L(" IsValidEmailAddressL left with %d "),err);
       	return err;
    	}
    else
   	 {
     if(valid)
     	{
     	iLog->Log(_L("IsValidEmailAddressL passed"));
     	}
     else
     	{
     	iLog->Log(_L("IsValidEmailAddressL failed"));
     	}
             	     	
    	}
    iLog->Log(_L("Calling IsValidEmailAddressL with Invalid Email Address"));
    TRAP(err,valid = MsvUiServiceUtilities::IsValidEmailAddressL(KEmailAddress2));
    if (err != KErrNone)
    	{
        iLog->Log(_L(" IsValidEmailAddressL left with %d "),err);
       	return err;
    	}
    else
   	 	{
	     if(!valid)
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL passed "));
	     	}
	     else
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL failed"));
	     	}
    	}
    TRAP(err,valid = MsvUiServiceUtilities::IsValidEmailAddressL(KEmailAddress3));
    if (err != KErrNone)
    	{
        iLog->Log(_L(" IsValidEmailAddressL left with %d "),err);
       	return err;
    	}
    else
   	 	{
	     if(!valid)
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL passed "));
	     	}
	     else
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL failed"));
	     	}
    	}
    TRAP(err,valid = MsvUiServiceUtilities::IsValidEmailAddressL(KEmailAddress4));
    if (err != KErrNone)
    	{
        iLog->Log(_L(" IsValidEmailAddressL left with %d "),err);
       	return err;
    	}
    else
   	 	{
	     if(!valid)
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL passed "));
	     	}
	     else
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL failed"));
	     	}
    	}
    TRAP(err,valid = MsvUiServiceUtilities::IsValidEmailAddressL(KEmailAddress5));
    if (err != KErrNone)
    	{
        iLog->Log(_L(" IsValidEmailAddressL left with %d "),err);
       	return err;
    	}
    else
   	 	{
	     if(!valid)
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL passed "));
	     	}
	     else
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL failed"));
	     	}
    	}
    TRAP(err,valid = MsvUiServiceUtilities::IsValidEmailAddressL(KEmailAddress6));
    if (err != KErrNone)
    	{
        iLog->Log(_L(" IsValidEmailAddressL left with %d "),err);
       	return err;
    	}
    else
   	 	{
	     if(!valid)
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL passed "));
	     	}
	     else
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL failed"));
	     	}
    	}
    TRAP(err,valid = MsvUiServiceUtilities::IsValidEmailAddressL(KEmailAddress7));
    if (err != KErrNone)
    	{
        iLog->Log(_L(" IsValidEmailAddressL left with %d "),err);
       	return err;
    	}
    else
   	 	{
	     if(!valid)
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL passed "));
	     	}
	     else
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL failed"));
	     	}
    	}
    TRAP(err,valid = MsvUiServiceUtilities::IsValidEmailAddressL(KEmailAddress8));
    if (err != KErrNone)
    	{
        iLog->Log(_L(" IsValidEmailAddressL left with %d "),err);
       	return err;
    	}
    else
   	 	{
	     if(!valid)
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL passed "));
	     	}
	     else
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL failed"));
	     	}
    	}
    TRAP(err,valid = MsvUiServiceUtilities::IsValidEmailAddressL(KEmailAddress9));
    if (err != KErrNone)
    	{
        iLog->Log(_L(" IsValidEmailAddressL left with %d "),err);
       	return err;
    	}
    else
   	 	{
	     if(!valid)
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL passed "));
	     	}
	     else
	     	{
	     	iLog->Log(_L("IsValidEmailAddressL failed"));
	     	}
    	}
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::DiskSpaceBelowCriticalLevelL
// Test method function to test MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL
// -----------------------------------------------------------------------------
//
TInt CMsgCentreUiUtilitiesApiTest::DiskSpaceBelowCriticalLevelL()
	{
	_LIT( KApiName, "In DiskSpaceBelowCriticalLevelL" );
    TestModuleIf().Printf( 0, KUiUtilities, KApiName );
    TBool criticalLevel;
    TInt bytes(100);
	iLog->Log(_L("Calling DiskSpaceBelowCriticalLevelL ") );
	TRAPD(err, criticalLevel = MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(*iSession,bytes));
	if(criticalLevel)
		{
		// for Arm warnings
		}
	if(err != KErrNone)
		{
		iLog->Log(_L("DiskSpaceBelowCriticalLevelL failed") );
		return err;
		}
	iLog->Log(_L("DiskSpaceBelowCriticalLevelL passed") );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::DiskSpaceBelowCriticalLevelWithOverheadL
// Test method function to test MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelWithOverheadL
// -----------------------------------------------------------------------------
//
TInt CMsgCentreUiUtilitiesApiTest::DiskSpaceBelowCriticalLevelWithOverheadL()
	{
	_LIT( KApiName, "In DiskSpaceBelowCriticalLevelWithOverheadL" );
    TestModuleIf().Printf( 0, KUiUtilities, KApiName );
    TBool criticalLevel;
    TInt bytes(100);
	iLog->Log(_L("Calling DiskSpaceBelowCriticalLevelWithOverheadL") );
	TRAPD(err, criticalLevel=MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelWithOverheadL(*iSession,bytes,KAOSafetyMargin));
	if(criticalLevel)
		{
		// To reduce the arm warnings
		}
	if(err != KErrNone)
		{
		iLog->Log(_L("DiskSpaceBelowCriticalLevelWithOverheadL left with %d"),err );
		return err;
		}
	iLog->Log(_L("DiskSpaceBelowCriticalLevelWithOverheadL passed") );
	return KErrNone;
	}
TInt CMsgCentreUiUtilitiesApiTest::CallToSenderQueryTestL() //deepak
	{
	// This is a deprecated method
	TBuf <10> PhoneNumber;
	TBuf <10> PersonName;
	PhoneNumber.AppendNum(12345);
	PersonName.Copy(_L("Name"));
	TBool ret = MsvUiServiceUtilities::CallToSenderQueryL(PhoneNumber,PhoneNumber,EFalse);
	return KErrNone;
	}
TInt CMsgCentreUiUtilitiesApiTest::InternetCallToSenderQueryTestL() //deepak
	{
	// This is a deprecated method
	TBuf <10> PhoneNumber;
	TBuf <10> PersonName;
	PhoneNumber.AppendNum(12345);
	PersonName.Copy(_L("Name"));
	TBool ret = MsvUiServiceUtilities::InternetCallToSenderQueryL(PhoneNumber,PhoneNumber,EFalse);
	return KErrNone;
	}
TInt CMsgCentreUiUtilitiesApiTest::InternetCallServiceTestL() //deepak
	{
	// This is a deprecated method
	TBuf <10> PhoneNumber;
	TBuf <10> PersonName;
	PhoneNumber.AppendNum(12345);
	PersonName.Copy(_L("Name"));
	TBool ret = MsvUiServiceUtilities::InternetCallServiceL(PhoneNumber,PhoneNumber,PhoneNumber,NULL);
	return KErrNone;
	}
TInt CMsgCentreUiUtilitiesApiTest::DefaultServiceForMTMTestL() //deepak
	{
	// This is a deprecated method
	TMsvId serviceId;
	serviceId = MsvUiServiceUtilities::DefaultServiceForMTML(*iSession, KSenduiMtmSmtpUid, ETrue);
	if(serviceId)
		{
		// To Prevent Arm warnings
		}
	return KErrNone;
	}

/******************************CMsvRemoteOperationProgressNewL************************/
// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::NewL
// Test method function to test CMsvRemoteOperationProgressNewL::NewL
// -----------------------------------------------------------------------------
//

TInt CMsgCentreUiUtilitiesApiTest::CMsvRemoteOperationProgressNewL()
{
	_LIT( KApiName, "In CMsvRemoteOperationProgressNewL" );
    TestModuleIf().Printf( 0, KUiUtilities, KApiName );
	RemoteOperationObserver* observer = new(ELeave)RemoteOperationObserver; 
	CMsvRemoteOperationProgress* operationProgress = NULL;
	TInt errorCount = 2;
	TRAPD(err, operationProgress=CMsvRemoteOperationProgress::NewL(*observer,CActive::EPriorityStandard,5000000));
	
	if(operationProgress)
		{
		errorCount--;
		iLog->Log(_L("CMsvRemoteOperationProgress object created successfully"));
		delete operationProgress;
	    operationProgress = NULL;
		}
	if(err != KErrNone)
		{
		iLog->Log(_L("CMsvRemoteOperationProgressNewL1 failed"));
		}
		else
		{
		iLog->Log(_L("CMsvRemoteOperationProgressNewL1 left with %d"),err );
		}
	TRAP(err, operationProgress=CMsvRemoteOperationProgress::NewL(*observer,5000000));
	if(operationProgress)
		{
		errorCount--;
		iLog->Log(_L("CMsvRemoteOperationProgress object created successfully"));
		delete operationProgress;
	    operationProgress = NULL;
		}
	if(err != KErrNone)
		{
		iLog->Log(_L("CMsvRemoteOperationProgressNewL2 failed"));
		}
		else
		{
		iLog->Log(_L("CMsvRemoteOperationProgressNewL2 left with %d"),err );
		}
		if(observer)
		{
		delete observer;
	    observer = NULL;
		}
		return errorCount;	
	}

/*************************** CMsgInfoMessageInfoDialog *******************/
// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::CMsgInfoMessageInfoDialogNewL
// Test method function to test CMsgInfoMessageInfoDialog::NewL and ExecuteLD
// -----------------------------------------------------------------------------
//
TInt CMsgCentreUiUtilitiesApiTest::CMsgInfoMessageInfoDialogNewL()
	{
    _LIT(KFrom,"ABC");
    _LIT(KTo,"XYZ");
    _LIT(KSubject,"Meeting Cancelled");

    _LIT(KType,"PostCard");

    _LIT(KSize,"100kb");

    _LIT(KPriority,"High");
   
    
	TMsgInfoMessageInfoData infoData;
	
	TTime dateTime;
	
	dateTime.HomeTime();
	
	infoData.iFrom.Set(KFrom);
	infoData.iSubject.Set(KSubject);
	infoData.iType.Set(KType);
	infoData.iSize.Set(KSize);
	infoData.iTo.Set(KTo);
	infoData.iPriority.Set(KPriority);
	
	infoData.iDateTime=dateTime;
	CMsgInfoMessageInfoDialog* messageInfoDialog = NULL;
	TRAPD(err,messageInfoDialog=CMsgInfoMessageInfoDialog::NewL());
	if(err != KErrNone || messageInfoDialog==NULL)
		{
		iLog->Log(_L("CMsgInfoMessageInfoDialog::NewL left with %d"),err);
		return err;
		}
	else
		{
		iLog->Log(_L("CMsgInfoMessageInfoDialog object created successfully"));
		iLog->Log(_L("CMsgInfoMessageInfoDialog is getting launched"));
	
		TRAPD(err1,messageInfoDialog->ExecuteLD( infoData, CMsgInfoMessageInfoDialog::EMmsViewer ));
			if(err1)
				{
				iLog->Log(_L("CMsvRemoteOperationProgressNewL left with %d"),err1 );
				return err1;
				}
			else
				{
				iLog->Log(_L("CMsgInfoMessageInfoDialog launch is successful"));
				return KErrNone;
				}
		}

	}
	
/**************************** CMuiuOperationWait ********************************/
// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::CMsgInfoMessageInfoDialogNewL
// Test method function to test CMuiuOperationWait::NewL and destructor
// -----------------------------------------------------------------------------
//
TInt CMsgCentreUiUtilitiesApiTest::CMuiuOperationWaitNewL() 
	{
	CMuiuOperationWait* wait = CMuiuOperationWait::NewLC(CActive::EPriorityStandard);
	if(wait)
		{
		iLog->Log(_L("CMuiuOperationWait passed "));
		CleanupStack::PopAndDestroy(wait);
		return KErrNone;
		}
	else
		{
		iLog->Log(_L("CMuiuOperationWait failed"));
		}
	return KErrGeneral;
	}
/**************************** MsvUiEditorUtilities ********************************/
// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::ResolveAppFileNameTestL
// Test method function to test MsvUiEditorUtilities::ResolveAppFileNameL
// -----------------------------------------------------------------------------
//
TInt CMsgCentreUiUtilitiesApiTest::ResolveAppFileNameTestL() 
	{
	HBufC* retBuf = NULL;
	TRAPD(err, retBuf= MsvUiEditorUtilities::ResolveAppFileNameL( TUid::Uid( 0x100058C5 ) )); // This is the uid of MCE
	if(!err)
	{
		TBuf<50> buffer;
		buffer.Copy(retBuf->Des());
		TInt compare = buffer.Compare(_L("Z:\\sys\\bin\\Mce.exe"));
		if(!compare)
			{
			iLog->Log(_L("MsvUiEditorUtilities::ResolveAppFileNameL passed"));
			}
		else
			{
			iLog->Log(_L("MsvUiEditorUtilities::ResolveAppFileNameL left with error %d "),err);
			}
		delete retBuf;
		return KErrNone;
	}
	else
		{
		delete retBuf;
		iLog->Log(_L("MsvUiEditorUtilities::ResolveAppFileNameL failed"));
		return KErrGeneral;	
		}
	}

	
/*************************** Virtual method Implementations**********************/

//-----------------------------------------------------------------------------
// Method from MMessageIteratorObserver class
//-----------------------------------------------------------------------------
void CMsgCentreUiUtilitiesApiTest::HandleIteratorEventL( TMessageIteratorEvent /*aEvent*/ )
	{
	//No Implementation
	}
//------------------------------------------------------------------------------
// Method from MMsvSingleOpWatcher class
//-----------------------------------------------------------------------------
void CMsgCentreUiUtilitiesApiTest::OpCompleted( CMsvSingleOpWatcher& aOpWatcher, TInt aCompletionCode )
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
//------------------------------------------------------------------------------
// Method from MMsvProgressDecoder class
//-----------------------------------------------------------------------------	
TInt CMsgCentreUiUtilitiesApiTest::DecodeProgress(const TDesC8& aProgress,
        TBuf<CBaseMtmUi::EProgressStringMaxLen>& /*aReturnString*/,
        TInt& /*aTotalEntryCount*/, TInt& /*aEntriesDone*/,
        TInt& /*aCurrentEntrySize*/, TInt& /*aCurrentBytesTrans*/, TBool /*aInternal*/)
    {
    if(aProgress.Length()==0)
        return KErrNone;

    TPckgC<TInt> type(0);
    type.Set(aProgress.Left(sizeof(TInt)));
    return KErrNone;
    }
// --------------------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::CreateDummyMessageL
// Creates a dummy message into a specified folder
// --------------------------------------------------------------------------------------
void CMsgCentreUiUtilitiesApiTest::CreateDummyMessageL(const TMsvId aFolderId)	
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
	    _LIT( KSMSBody, " Hello Phone Owner !!!" );
	    CRichText& body = iSmsClientMtm->Body();
	    body.Reset();
	    body.InsertL( 0, KSMSBody );
	    indexEntry.iDescription.Set(KSMSBody);
	    
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
void CMsgCentreUiUtilitiesApiTest::CleanFolderL(TMsvId aBoxId, CMsvSession& aSession)
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
    
 void CMsgCentreUiUtilitiesApiTest::CleanAllL()
	{
	iLog->Log(_L("In Clean All"));
	CleanBoxL(KMsvGlobalInBoxIndexEntryId,*iSession);
    CleanBoxL(KMsvGlobalOutBoxIndexEntryId,*iSession);
    CleanBoxL(KMsvSentEntryId,*iSession);
    CleanBoxL(KMsvDraftEntryId,*iSession);
    iLog->Log(_L("End of Clean All"));
    	
	}
void CMsgCentreUiUtilitiesApiTest::CleanBoxL(TMsvId aBoxId, CMsvSession& aSession)
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
//  [End of File] - Do not remove
