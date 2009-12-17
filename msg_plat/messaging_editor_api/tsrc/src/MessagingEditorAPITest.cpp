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
* Description:   MessagingEditorAPITest implementation*
*/




// INCLUDE FILES
#include <Stiftestinterface.h>
#include "MessagingEditorAPITest.h"
#include <SettingServerClient.h>
#include <MsgAttachmentModel.h>
#include <MuiuMsvSingleOpWatcher.h>
#include <EIKENV.H>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define ATCH_ID 12345

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::CMessagingEditorAPITest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMessagingEditorAPITest::CMessagingEditorAPITest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf ),
        iMsgAttachModel( NULL ),
        iAtchId( ATCH_ID ),
        iCount( 0 ),
        iEngine( NULL ),
        iFilePathflag( EFalse )
    {
    iAppEikonEnv = CEikonEnv::Static();
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMessagingEditorAPITest::ConstructL()
    {
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if(ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);
    if(ret != KErrNone)
        {
        User::Leave(ret);
        } 
    // Close Setting server session
    settingServer.Close();

    TFileName logFileName;
    
    if(loggerSettings.iAddTestCaseTitle)
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        logFileName.Format(KMessagingEditorAPITestLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KMessagingEditorAPITestLogFile);
        }

    iLog = CStifLogger::NewL( KMessagingEditorAPITestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    iSession        = CMsvSession::OpenSyncL( *this);
    iMtmRegistry    = CClientMtmRegistry::NewL( *iSession );
    iSmsClientMtm   = STATIC_CAST( CSmsClientMtm*, iMtmRegistry->NewMtmL( KUidMsgTypeSMS ));
    iOpWatchers     = new ( ELeave ) CSingleOpWatchers( 5 );
	iLog->Log( _L("Before LoadingResources"));
	iResView       = CCoeEnv::Static()->AddResourceFileL(KDir_ResFileName);
	iResAppUi      = CCoeEnv::Static()->AddResourceFileL(KDir_AppUiResFileName);
	iLog->Log( _L("After Loading Resources"));
    SendTestClassVersion();
    }

// -----------------------------------------------------------------------------
// CMessagingEditorAPITest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMessagingEditorAPITest* CMessagingEditorAPITest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CMessagingEditorAPITest* self = new (ELeave) CMessagingEditorAPITest( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CMessagingEditorAPITest::~CMessagingEditorAPITest()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 
        	
    if(iSmsClientMtm)
    	{
    	delete iSmsClientMtm;
    	iSmsClientMtm = NULL;
    	}
    if(iMtmRegistry)
    	{
    	delete iMtmRegistry;
    	iMtmRegistry = NULL;
    	}
    if(iMessageIterator)
    	{
    	delete iMessageIterator;
    	iMessageIterator = NULL;
    	}
    if (iOpWatchers)
        {
        iOpWatchers->ResetAndDestroy();
        }
    
     if(iSession)
    	{
    	delete iSession;
    	iSession = NULL;
    	}

    }

//-----------------------------------------------------------------------------
// CMessagingEditorAPITest::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CMessagingEditorAPITest::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("MessagingEditorAPITest.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
	}
void CMessagingEditorAPITest::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny */*aArg1*/, TAny */*aArg2*/, TAny */*aArg3*/)
	{
	//No Implementation 
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CMessagingEditorAPITest::NewL( aTestModuleIf );

    }


//  End of File
