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
* Description:   This header file declares test module for testing Exported-
*               - functions from MeetingRequestData.h
*
*/




#ifndef SENDUIMEETINGREQUESTAPITEST_H
#define SENDUIMEETINGREQUESTAPITEST_H

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
_LIT( KSendUIMeetingRequestAPITestLogPath, "\\logs\\testframework\\SendUIMeetingRequestAPITest\\" ); 
// Log file
_LIT( KSendUIMeetingRequestAPITestLogFile, "SendUIMeetingRequestAPITest.txt" ); 
_LIT( KSendUIMeetingRequestAPITestLogFileWithTitle, "SendUIMeetingRequestAPITest_[%S].txt" );

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CSendUIMeetingRequestAPITest;
class CMeetingRequestData;

// DATA TYPES

// CLASS DECLARATION

/**
*  CSendUIMeetingRequestAPITest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CSendUIMeetingRequestAPITest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSendUIMeetingRequestAPITest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CSendUIMeetingRequestAPITest();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CSendUIMeetingRequestAPITest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

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

        /**
         * TestMRqDataMailBoxnFileIDL tests CMeetingRequestData's
         * UseMailbox, Mailbox, SetDatabaseFileId, and DatabaseFileId
         */
        TInt TestMRqDataMailBoxnFileIDL( CStifItemParser& aItem );
        
        /**
         * TestMRqDataEntryLocalIDL tests CMeetingRequestData's
         * AppendEntryLocalId and EntryLocalIdArray
         */
        TInt TestMRqDataEntryLocalIDL( CStifItemParser& aItem );
        
        /**
         * TestMRqDataMailHeaderL tests CMeetingRequestData's
         * SetMailHeaderMethodL and MailHeaderMethod
         */
        TInt TestMRqDataMailHeaderL( CStifItemParser& aItem );
        
        /**
         * TestMRqDataMailHeaderL tests CMeetingRequestData's
         * ReadFromBufferL
         */
        TInt TestReadFromBufferL( CStifItemParser& aItem );        
 
        /**
         * TestMRqDataMailHeaderL tests CMeetingRequestData's
         * WriteToBufferLC
         */
        TInt TestWriteToBufferLC( CStifItemParser& aItem );
        
        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove
        

    public:     // Data

    protected:  // Data

    private:    // Data
        
    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes


    };

#endif      // SENDUIMEETINGREQUESTAPITEST_H

// End of File
