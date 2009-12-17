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
* Description:   TestCase implementation file for "VideoConversionApi.h"
*
*/




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <VideoConversionApi.h>
#include "VideoConversionApiTest.h"
#include "TestVideoConversion.h"

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
// CVideoConversionApiTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CVideoConversionApiTest::Delete() 
    {
    //Needs to be called for doing additional cleanup (when using ECom plugins)
    REComSession::FinalClose();
    }

// -----------------------------------------------------------------------------
// CVideoConversionApiTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CVideoConversionApiTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "VideoConverterAPI", CVideoConversionApiTest::TestVideoConversionAPIL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CVideoConversionApiTest::TestVideoConversionAPIL
// Creates object of CTestVideoConversion and deletes it. CTestVideoConversion 
// internally calls VideoConversionPlugin which will check for implemented 
// converters and creates instance of one of video converter.. 
// -----------------------------------------------------------------------------
//
TInt CVideoConversionApiTest::TestVideoConversionAPIL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log( _L("TestVideoConvNewL_Enter"));
    iLog->Log( _L("Calling CTestVideoConversion::NewL()"));
    CTestVideoConversion* videoConverter = NULL;
    TRAPD( error, videoConverter = CTestVideoConversion::NewL( iLog ) );
    if( error != KErrNone )
        {
        _LIT( KError, "No Implementations found");
        iLog->Log( KError );
        return error;
        }
    else
        {
        iLog->Log( _L("Calling ~CTestVideoConversion"));
        delete videoConverter;
        videoConverter = NULL;
        iLog->Log( _L("TestVideoConvNewL_Exit"));
        return KErrNone;
        }
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
