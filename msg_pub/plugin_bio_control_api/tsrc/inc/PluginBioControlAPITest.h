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
* Description:   Tests MsgBioControl.h and CRichBio.h 
*
*/




#ifndef PLUGINBIOCONTROLAPITEST_H
#define PLUGINBIOCONTROLAPITEST_H

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
_LIT( KPluginBioControlAPITestLogPath, "\\logs\\testframework\\PluginBioControlAPITest\\" ); 
// Log file
_LIT( KPluginBioControlAPITestLogFile, "PluginBioControlAPITest.txt" ); 
_LIT( KPluginBioControlAPITestLogFileWithTitle, "PluginBioControlAPITest_[%S].txt" );

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MMsgBioControlObserver;
class MMsvSessionObserver;
class CPluginBioControlAPITest;
class CMsgBioControl;
class CEdwinTestControl;

// DATA TYPES

// CLASS DECLARATION

/**
*  CPluginBioControlAPITest test class for STIF Test Framework TestScripter.
*
*  @since Series60_version 5.0
*/
NONSHARABLE_CLASS(CPluginBioControlAPITest) : public CScriptBase,
    public MMsgBioControlObserver,
    public MMsvSessionObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPluginBioControlAPITest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CPluginBioControlAPITest();

    public: // New functions


    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
        
        // from MMsgBioControlObserver
        /**
        * FirstFreeCommand
        */
        TInt FirstFreeCommand() const;
        
        /**
        * RequestHandleCommandL
        */
        TInt RequestHandleCommandL(TMsgCommands aCommand);
        
        /**
        * IsCommandSupported
        */
        TBool IsCommandSupported(TMsgCommands aCommand) const;

        // from MMsvSessionObserver
        /**
        * HandleSessionEventL
        */
        void HandleSessionEventL(TMsvSessionEvent aEvent, TAny *aArg1, TAny *aArg2, TAny *aArg3);        

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CPluginBioControlAPITest( CTestModuleIf& aTestModuleIf );

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
        * TEST METHODS are listed below. 
        */

        /**
        * Tests CMsgBioControl and some functions of CRichBio
        */
        virtual TInt BioControlTestOneL( CStifItemParser& aItem );
        
        /**
        * Tests resource based ConfirmationQueryL of CMsgBioControl
        * and ScrollL, NotifyViewEventL
        */
        virtual TInt BioControlTestTwoL( CStifItemParser& aItem );
        
        /**
        * Tests Protected functions of CMsgBioControl
        */
        TInt BioCtrlProtectedTestL( CStifItemParser& aItem );
        
        /**
        * Tests NewL of CRichBio
        */
        TInt RichBioNewLTestL( CStifItemParser& aItem );
        
        /**
        * Test OfferKeyEventL of CRichBio
        */
        TInt RBioOfferKeyEventTestL( CStifItemParser& aItem );
        
        /**
        * Method used to log version of test class
        */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

        //Helper functions
        /**
        * Method used to Create iBioControl variable
        */
        void InitL( );
        
        /**
        * Method used to Create EdwinControl
        */
        void CreateEdwinL();
        
    public:     // Data

    protected:  // Data

    private:    // Data
        
        // Pointer to CMsvSession
        CMsvSession*        iSession;        
        // Pointer to BioControl(sample)
        CMsgBioControl*     iBioControl;
        // Pointer to EdwinControl
        CEdwinTestControl*  iEdwinTestCtrl;
        // Flag for filebased Control
        TBool               iFileBased;
        // File handle
        RFile               iFile;

        // Reserved pointer for future extension

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes


    };

#endif      // PLUGINBIOCONTROLAPITEST_H

// End of File
