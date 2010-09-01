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




#ifndef MSGCENTREUIUTILITIESAPITEST_H
#define MSGCENTREUIUTILITIESAPITEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <msvapi.h>
#include <SMSCLNT.H>
#include <MuiuMessageIterator.h>
#include <MuiuMsvSingleOpWatcher.h>
#include <msvprgreporter.h>
#include <MuiuMsvRemoteOperationProgress.h>


// CONSTANTS
typedef CArrayPtrFlat < CMsvSingleOpWatcher > CSingleOpWatchers;


// MACROS

#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KMsgCentreUiUtilitiesApiTestLogPath, "\\logs\\testframework\\MsgCentreUiUtilitiesApiTest\\" ); 
// Log file
_LIT( KMsgCentreUiUtilitiesApiTestLogFile, "MsgCentreUiUtilitiesApiTest.txt" ); 
_LIT( KMsgCentreUiUtilitiesApiTestLogFileWithTitle, "MsgCentreUiUtilitiesApiTest_[%S].txt" );



// FORWARD DECLARATIONS
//class FORWARD_CLASSNAME;
class CMsgCentreUiUtilitiesApiTest;
class CMsvSession;
class CMtmStore;
class CClientMtmRegistry;
class CSmsClientMtm;
class CMessageIterator;
class RemoteOperationObserver:public MMsvRemoteOperationProgressObserver
{
	
	public:
	
			RemoteOperationObserver()
			{
				
				
			}
		  
		  
		   virtual void UpdateRemoteOpProgressL() 
		   {
		   	
		   	//No Definition
		   }

};



// CLASS DECLARATION

/**
*  CMsgCentreUiUtilitiesApiTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMsgCentreUiUtilitiesApiTest) : 
	public CScriptBase,
	public MMsvSessionObserver ,
	public MMessageIteratorObserver,
	public MMsvSingleOpWatcher,
	public MMsvProgressDecoder
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMsgCentreUiUtilitiesApiTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMsgCentreUiUtilitiesApiTest();

    public: // New functions
    
	 //from MmsvSessionObserver
       virtual void HandleSessionEventL(TMsvSessionEvent aEvent, TAny *aArg1, TAny *aArg2, TAny *aArg3);
     // from MMsvSingleOpWatcher 
        virtual void OpCompleted( CMsvSingleOpWatcher& aOpWatcher, TInt aCompletionCode );
       virtual TInt DecodeProgress(const TDesC8& aProgress,
        TBuf<CBaseMtmUi::EProgressStringMaxLen>& /*aReturnString*/,
        TInt& /*aTotalEntryCount*/, TInt& /*aEntriesDone*/,
        TInt& /*aCurrentEntrySize*/, TInt& /*aCurrentBytesTrans*/, TBool /*aInternal*/);

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
        /**
        * From MMessageIteratorObserver
        */
        void HandleIteratorEventL( TMessageIteratorEvent aEvent );

    protected:  // New functions

      

    protected:  // Functions from base classes

       

    private:

        /**
        * C++ default constructor.
        */
        CMsgCentreUiUtilitiesApiTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // classname( const classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // classname& operator=( const classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

		void InitL();
        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ExampleL( CStifItemParser& aItem );
        
        
        /**
        * Test method.
        * for CUidNameArray class
        * 
        * @return Symbian OS error code.
        */
        virtual TInt CUidNameArrayTestL( );
        /**
        * Test method
        * for MsvFactorySettings class
        * 
        * @return Symbian OS error code.
        */
        virtual TInt ResetL();
        /**
        * Test method
        * for CMtmStore class
        * @return Symbian OS error code.
        */
        virtual TInt MtmUiDataAndReleaseUiDataL();
        /**
        * Test method
        * for CMtmStore class
        * @return Symbian OS error code.
        */
        virtual TInt ClaimAndReleaseMtmUiL();
        /**
        * Test method
        * for CMtmStore class
        * @return Symbian OS error code.
        */
        virtual TInt GetMtmUiAndSetContextL();
        /**
        * Test method
        * for CMtmStore class
        * @return Symbian OS error code.
        */
        virtual TInt ClaimMtmUiAndSetContextL();
        /**
        * Test method
        * for CMtmStore class
        * @return Symbian OS error code.
        */
        virtual TInt GetMtmUiL();
        /**
        * Test method
        * for MessageIterator class
        * @return Symbian OS error code.
        */
        virtual TInt MessageIteratorNewL();
        /**
        * Test method
        * for MessageIterator class
        * @return Symbian OS error code.
        */
        virtual TInt CurrentMessageTest();
        
        /**
        * Test method
        * for MessageIterator class
        * @return Symbian OS error code.
        */
        virtual TInt CurrentParentTest();
        /**
        * Test method
        * for MessageIterator class
        * @return Symbian OS error code.
        */
        virtual TInt NextMessageExistsTest();
        
        /**
        * Test method
        * for MessageIterator class
        * @return Symbian OS error code.
        */
        virtual TInt PreviousMessageExistsTest();
        /**
        * Test method
        * for MessageIterator class
        * @return Symbian OS error code.
        */
        virtual TInt SetCurrentMessageTestL();
        
        /**
        * Test method
        * for MessageIterator class
        * @return Symbian OS error code.
        */
        virtual TInt SetNextAndPreviousMessageTest();  
        /**
        * Test method
        * for MessageIterator class
        * @return Symbian OS error code.
        */
        virtual TInt OpenCurrentMessageTestL( );
        /**
        * Test method
        * for MessageIterator class
        * @return Symbian OS error code.
        */
        virtual TInt OpenCurrentMessageEmbeddedTestL();
         /**
        * Test method
        * for MessageIterator class
        * @return Symbian OS error code.
        */
        virtual TInt SelectFolderTestL();    
        
         /**
        * Test method
        * for MuiuMsvProgressReporterOperation class
        * @return Symbian OS error code.
        */
        virtual TInt CMsvProgressReporterOperationNewLTestL (); 
               
         /**
        * Test method
        * for MuiuMsvProgressReporterOperation class
        * @return Symbian OS error code.
        */
         /**
        * Test method
        * for MuiuMsvProgressReporterOperation class
        * @return Symbian OS error code.
        */
        virtual TInt  PublishNewDialogTestL();
        
        virtual TInt CMsvProgressReporterOperationTestL();
        
        virtual TInt GetListOfAccountsL();
        
        
        virtual TInt GetListOfAccountsWithMTML();
        
        virtual TInt IsValidEmailAddressL(); 
        
        virtual TInt DiskSpaceBelowCriticalLevelL();
        
        virtual TInt DiskSpaceBelowCriticalLevelWithOverheadL();
        /**
        * Test method
        * for MsvUiServiceUtilities class
        * @return Symbian OS error code.
        */virtual TInt CallToSenderQueryTestL();
        /**
        * Test method
        * for MsvUiServiceUtilities class
        * @return Symbian OS error code.
        */virtual TInt InternetCallToSenderQueryTestL();
        /**
        * Test method
        * for MsvUiServiceUtilities class
        * @return Symbian OS error code.
        */virtual TInt InternetCallServiceTestL();
        /**
        * Test method
        * for MsvUiServiceUtilities class
        * @return Symbian OS error code.
        */virtual TInt DefaultServiceForMTMTestL();
        
        
        virtual TInt CMsvRemoteOperationProgressNewL();
        
        virtual TInt CMsgInfoMessageInfoDialogNewL();
        /**
        * Test method
        * for CMuiuOperationWait class
        * @return Symbian OS error code.
        */
        virtual TInt CMuiuOperationWaitNewL();
        /**
        * Test method
        * for MsvUiEditorUtilities class
        * @return Symbian OS error code.
        */
        virtual TInt ResolveAppFileNameTestL();
        
        void CreateDummyMessageL(const TMsvId aFolderId);
        void CleanFolderL(TMsvId aBoxId, CMsvSession& aSession);
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();
        
        /*
        * method for cleaning all the messages
        */
        void CleanAllL();
        /*
        * method for cleaning a folder
        */
        void CleanBoxL(TMsvId aBoxId, CMsvSession& aSession);

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
    

    protected:  // Data
      

    private:    // Data
        
        CMsvSession* iSession;
        CMtmStore* iMtmStore;
        CClientMtmRegistry* iMtmRegistry;
        CSmsClientMtm* iSmsClientMtm;
        TMsvId iSmsId;
        CMessageIterator* iMessageIterator;
        TMsvEntry iCurrentEntry;
        CSingleOpWatchers* iOpWatchers;
        CMsvEntrySelection*  iSelectedEntries;
        CMsvEntry* 	iEntry;
    public:     // Friend classes
        //friend_class_declaration;
    protected:  // Friend classes
        //friend_class_declaration;
    private:    // Friend classes
        //friend_class_declaration;

    };


#endif      // MSGCENTREUIUTILITIESAPITEST_H

// End of File
