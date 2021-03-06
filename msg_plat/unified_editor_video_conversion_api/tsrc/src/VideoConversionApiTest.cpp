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
* Description:   VideoConversionApiTest implementation*
*/




// INCLUDE FILES
#include <Stiftestinterface.h>
#include "VideoConversionApiTest.h"
#include <SettingServerClient.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVideoConversionApiTest::CVideoConversionApiTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVideoConversionApiTest::CVideoConversionApiTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CVideoConversionApiTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVideoConversionApiTest::ConstructL()
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
        logFileName.Format(KVideoConversionApiTestLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KVideoConversionApiTestLogFile);
        }

    iLog = CStifLogger::NewL( KVideoConversionApiTestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    
    SendTestClassVersion();
    }

// -----------------------------------------------------------------------------
// CVideoConversionApiTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVideoConversionApiTest* CVideoConversionApiTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CVideoConversionApiTest* self = new (ELeave) CVideoConversionApiTest( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CVideoConversionApiTest::~CVideoConversionApiTest()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

    }

//-----------------------------------------------------------------------------
// CVideoConversionApiTest::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CVideoConversionApiTest::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("VideoConversionApiTest.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
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

    return ( CScriptBase* ) CVideoConversionApiTest::NewL( aTestModuleIf );

    }


//  End of File
