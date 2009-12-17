/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ImumDomainApiTest.h
*
*/



#ifndef IMUMDOMAINAPITEST_H
#define IMUMDOMAINAPITEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>


// CONSTANTS

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KImumDomainApiTestLogPath, "\\logs\\testframework\\ImumDomainApiTest\\" );
// Log file
_LIT( KImumDomainApiTestLogFile, "ImumDomainApiTest.txt" );
_LIT( KImumDomainApiTestLogFileWithTitle, "ImumDomainApiTest_[%S].txt" );

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CImumDomainApiTest;

// DATA TYPES

// CLASS DECLARATION

/**
*  CImumDomainApiTest test class for STIF Test Framework TestScripter.
*
*  @lib ImumDomainApiTest
*/
NONSHARABLE_CLASS(CImumDomainApiTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CImumDomainApiTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CImumDomainApiTest();

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
        CImumDomainApiTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below.
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ExampleL( CStifItemParser& aItem );


        /**
         * CreateMailboxLTestL
         * @since Series60 5.0
         * @param aItem Script line containing parameters.
         * @return Symbian OS error code (test case execution error
         *   that is not returned as test case result in aResult)
         */
        TInt CreateMailboxLTestL( CStifItemParser& aItem );

        /**
         * SetDefaultMailboxLTestL
         * @since Series60 5.0
         * @param aItem Script line containing parameters.
         * @return Symbian OS error code (test case execution error
         *   that is not returned as test case result in aResult)
         */
        TInt SetDefaultMailboxLTestL( CStifItemParser& aItem );

        /**
         * IsMailBoxTestL
         * @since Series60 5.0
         * @param aItem Script line containing parameters.
         * @return Symbian OS error code (test case execution error
         *   that is not returned as test case result in aResult)
         */
        TInt IsMailBoxTestL( CStifItemParser& aItem );

        /**
         * GetMailboxEntryTestL
         * @since Series60 5.0
         * @param aItem Script line containing parameters.
         * @return Symbian OS error code (test case execution error
         *   that is not returned as test case result in aResult)
         */
        TInt GetMailboxEntryTestL( CStifItemParser& aItem );

        /**
         * IsMailboxHealthyTestL
         * @since Series60 5.0
         * @param aItem Script line containing parameters.
         * @return Symbian OS error code (test case execution error
         *   that is not returned as test case result in aResult)
         */
        TInt IsMailboxHealthyTestL( CStifItemParser& aItem );


        // Non UI tests
        /**
        * test case.
        * @since Series60 5.0
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error
        *   that is not returned as test case result in aResult)
        */
        TInt CreateSettingsDataWithValidUidTestL( CStifItemParser& aItem );

        /**
         * test case.
         * @since Series60 5.0
         * @param aResult Test case result (PASS/FAIL)
         * @return Symbian OS error code (test case execution error
         *   that is not returned as test case result in aResult)
         */
        TInt IsMailMtmTestL( CStifItemParser& aItem );

        /**
         * test case.
         * @since Series60 5.0
         * @param aResult Test case result (PASS/FAIL)
         * @return Symbian OS error code (test case execution error
         *   that is not returned as test case result in aResult)
         */
        TInt GetMailboxesTestL( CStifItemParser& aItem );

        /**
         * test case.
         * Tests CImumDaSettingsData class
         * @since Series60 5.0
         * @param aResult Test case result (PASS/FAIL)
         * @return Symbian OS error code (test case execution error
         *   that is not returned as test case result in aResult)
         */
        TInt AddSetTestL( CStifItemParser& aItem );

        /**
         * test case.
         * Tests CImumDaSettingsData class
         * @since Series60 5.0
         * @param aResult Test case result (PASS/FAIL)
         * @return Symbian OS error code (test case execution error
         *   that is not returned as test case result in aResult)
         */
        TInt SetAttrTestL( CStifItemParser& aItem );

        /**
         * test case.
         * Tests MImumDaSettingsDataCollection class
         * @since Series60 5.0
         * @param aResult Test case result (PASS/FAIL)
         * @return Symbian OS error code (test case execution error
         *   that is not returned as test case result in aResult)
         */
        TInt SetAttrToDataCollectionTestL( CStifItemParser& aItem );

        /**
         * SetDataExceptionL
         * Tests CImumDaSettingsDataImpl class
         * @since Series60 5.0
         * @param aItem Script line containing parameters.
         * @return Symbian OS error code. Returns KErrNone if no errors.
         */
        TInt SetDataExceptionL( CStifItemParser& aItem );


        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        /**
         * Setup
         */
        void SetupL();

        /**
         * Teardown
         */
        void TeardownL();

        /**
         * Helper method. Removes all old mailboxes
         */
        void RemoveOldMailboxesL();

        /**
         * Helper method. Calculates next mailbox index number
         */
        TInt NextMailboxIndexL();
        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    };

#endif      // IMUMDOMAINAPITEST_H

// End of File
