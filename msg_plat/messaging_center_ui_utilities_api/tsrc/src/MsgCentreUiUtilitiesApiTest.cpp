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




// INCLUDE FILES
#include <Stiftestinterface.h>
#include "MsgCentreUiUtilitiesApiTest.h"
#include <SettingServerClient.h>
#include <MTMStore.h>

#include <mtclreg.h>    // CClientMtmRegistry 


// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::CMsgCentreUiUtilitiesApiTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgCentreUiUtilitiesApiTest::CMsgCentreUiUtilitiesApiTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMsgCentreUiUtilitiesApiTest::ConstructL()
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
        logFileName.Format(KMsgCentreUiUtilitiesApiTestLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KMsgCentreUiUtilitiesApiTestLogFile);
        }

    iLog = CStifLogger::NewL( KMsgCentreUiUtilitiesApiTestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    iSession = CMsvSession::OpenSyncL( *this);
    iMtmStore = CMtmStore::NewL(*iSession); 
    iMtmRegistry = CClientMtmRegistry::NewL( *iSession );
    iSmsClientMtm = STATIC_CAST( CSmsClientMtm*, iMtmRegistry->NewMtmL( KUidMsgTypeSMS ));
    iOpWatchers = new ( ELeave ) CSingleOpWatchers( 5 );
    iSelectedEntries = new (ELeave) CMsvEntrySelection;
    SendTestClassVersion();
    }

// -----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgCentreUiUtilitiesApiTest* CMsgCentreUiUtilitiesApiTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CMsgCentreUiUtilitiesApiTest* self = new (ELeave) CMsgCentreUiUtilitiesApiTest( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CMsgCentreUiUtilitiesApiTest::~CMsgCentreUiUtilitiesApiTest()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 
    
    if(iMtmStore)
    	{
    	delete iMtmStore;
    	iMtmStore = NULL;
    	}
    	
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
    if (iSelectedEntries)
        {
        delete iSelectedEntries;
    	iSelectedEntries = NULL;
        }
     if(iSession)
    	{
    	delete iSession;
    	iSession = NULL;
    	}
    }

//-----------------------------------------------------------------------------
// CMsgCentreUiUtilitiesApiTest::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CMsgCentreUiUtilitiesApiTest::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("MsgCentreUiUtilitiesApiTest.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
	}
	
void CMsgCentreUiUtilitiesApiTest::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny */*aArg1*/, TAny */*aArg2*/, TAny */*aArg3*/)
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

    return ( CScriptBase* ) CMsgCentreUiUtilitiesApiTest::NewL( aTestModuleIf );

    }


//  End of File
