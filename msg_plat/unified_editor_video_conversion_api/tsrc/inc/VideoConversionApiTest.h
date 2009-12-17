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
* Description:   Header file for class declaration that tests apis from "VideoConversionApi.h"
*
*/




#ifndef VIDEOCONVERSIONAPITEST_H
#define VIDEOCONVERSIONAPITEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

// CONSTANTS

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KVideoConversionApiTestLogPath, "\\logs\\testframework\\VideoConversionApiTest\\" ); 
// Log file
_LIT( KVideoConversionApiTestLogFile, "VideoConversionApiTest.txt" ); 
_LIT( KVideoConversionApiTestLogFileWithTitle, "VideoConversionApiTest_[%S].txt" );

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CVideoConversionApiTest;

// DATA TYPES

// CLASS DECLARATION

/**
*  CVideoConversionApiTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CVideoConversionApiTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVideoConversionApiTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        ~CVideoConversionApiTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */

    private:

        /**
        * C++ default constructor.
        */
        CVideoConversionApiTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.


        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove
        
        /**
         * This method tests Videoconversionapi ( creation and deletion )
         */
        TInt TestVideoConversionAPIL( CStifItemParser& aItem );

    public:     // Data

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes


    };


#endif      // VIDEOCONVERSIONAPITEST_H

// End of File
