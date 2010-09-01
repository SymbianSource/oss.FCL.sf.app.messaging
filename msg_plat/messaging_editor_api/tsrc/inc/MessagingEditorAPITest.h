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
* Description:   Tests MessagingEditorAPI headers
*
*/




#ifndef MESSAGINGEDITORAPITEST_H
#define MESSAGINGEDITORAPITEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <cmsvattachment.h>
#include <MsgEditorObserver.h>
#include <msvapi.h>
#include <SMSCLNT.H>
#include <MuiuMessageIterator.h>
#include <msvprgreporter.h>
#include <MuiuMsvRemoteOperationProgress.h>
#include <MTMStore.h>
#include <MuiuMsvSingleOpWatcher.h>
#include <msgeditor.hrh>
#include <mtclreg.h>    // CClientMtmRegistry 
#include <MsgAttachmentModelObserver.h>
#include "MsgBaseControl.h"                // for CMsgBaseControl
#include "MsgEditor.hrh"                   // for typedefs

// CONSTANTS
typedef CArrayPtrFlat < CMsvSingleOpWatcher > CSingleOpWatchers;

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KMessagingEditorAPITestLogPath, "\\logs\\testframework\\MessagingEditorAPITest\\" ); 
// Log file
_LIT( KMessagingEditorAPITestLogFile, "MessagingEditorAPITest.txt" ); 
_LIT( KMessagingEditorAPITestLogFileWithTitle, "MessagingEditorAPITest_[%S].txt" );
//TestData Path
#ifdef __WINSCW__
_LIT( KTestFilePath,"C:\\testing\\Data\\MsgEditorAPITestData\\%S");
#endif

//TestResource Path
#ifdef __WINSCW__
_LIT(KDir_ResFileName, "z:\\resource\\apps\\MessagingEditorAPITest.rsc");
_LIT(KDir_AppUiResFileName, "z:\\resource\\MsgEditorAppUi.rsc");
#else
_LIT(KDir_ResFileName, "c:\\resource\\apps\\MessagingEditorAPITest.rsc");
_LIT(KDir_AppUiResFileName, "z:\\resource\\MsgEditorAppUi.r01");
#endif

_LIT(KPhoneNumberString,"9848223361");
// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMessagingEditorAPITest;
class CMsgAttachmentModel;
class CMsgRecipientItem;
class CMsgAttachmentInfo;
class CMsgEditorEngine;
class CCoeAppUi;
class MMsgEditorObserver;
class CMsgBaseControl;
class TMsgEditorObserverFunc;
class CMsgEditorView;
class CMsvSession;
class CMtmStore;
class CClientMtmRegistry;
class CSmsClientMtm;
class CMessageIterator;
class CMsgExpandableControlEditor;
class CEikonEnv;	

// DATA TYPES
//enum 
//typedef 
//extern 

// CLASS DECLARATION

/**
*  CMessagingEditorAPITest test class for STIF Test Framework TestScripter.
*
*  @since Series60_version 5.0
*/
NONSHARABLE_CLASS(CMessagingEditorAPITest) : public CScriptBase,
                                             public MMsgEditorObserver,
											 public MEikEdwinObserver,
										   	 public MMsvSessionObserver ,
											 public MMessageIteratorObserver,
											 public MMsvSingleOpWatcher,
											 public MMsgAttachmentModelObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMessagingEditorAPITest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMessagingEditorAPITest();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
        
        //from MMsgEditorObserver
        /**
        * Create custom control.
        * @param
        * @return
        */
        CMsgBaseControl* CreateCustomControlL(TInt aControlType);

        /**
        * EditorObserver
        */
        void EditorObserver(TMsgEditorObserverFunc aFunc, TAny* aArg1, TAny* aArg2, TAny* aArg3);
        
        //from MEikEdwinObserver
        /**
        * HandleEdwinEventL
        */
        void HandleEdwinEventL( CEikEdwin* aEdwin, TEdwinEvent aEventType );  
        
        //from MMessageIteratorObserver
        /**
        * HandleIteratorEventL
        */
        void HandleIteratorEventL( TMessageIteratorEvent aEvent );
        
        //from MmsvSessionObserver
        /**
        * HandleSessionEventL
        */
        virtual void HandleSessionEventL(TMsvSessionEvent aEvent, TAny *aArg1, TAny *aArg2, TAny *aArg3);
        
        //from MMsvSingleOpWatcher
        /**
        * OpCompleted
        */
        virtual void OpCompleted( CMsvSingleOpWatcher& aOpWatcher, TInt aCompletionCode );
        
        //from MMsgAttachmentModelObserver
        /**
         * 
         */
        void NotifyChanges( TMsgAttachmentCommand aCommand, CMsgAttachmentInfo* aAttachmentInfo );
        
        /**
         * 
         */
        RFile GetAttachmentFileL( TMsvAttachmentId aId );

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CMessagingEditorAPITest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.

        // Prohibit assigment operator if not deriving from CBase.


        /**
        * Frees all resources allocated from test methods.
        * @since Series60_version 5.0
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
        * Tests MsgAttachmentModel functions 
        */
        TInt TestMsgAttachmentModelL( CStifItemParser& aItem );
         
        /**
        * Tests BitmapforFile
        */
        TInt TestBitmapforFileL( CStifItemParser& aItem );         
         
        /**
        * Tests MsgEditorDocument functions.
        */
         TInt TestMsgEditorDocumentL( CStifItemParser& aItem );     
         
        /**
        * Tests MsgAttachmentInfo NewL
        */
        TInt TestMsgAttachmentInfo( CStifItemParser& aItem );
        
        /**
        * Tests MsgAttachmentInfo AttachmentId() and SetAttachmentId()
        */
        TInt TestAtchInfoID( CStifItemParser& aItem );        
        
        /**
        * Tests MsgAttachmentInfo FileName() and SetFileName()
        */
        TInt TestAtchInfoFileName( CStifItemParser& aItem );        

        /**
        * Tests MsgAttachmentInfo Size() IsFetched() SetFetched() and SizeString()
        */
        TInt TestAtchInfoSizenFetched( CStifItemParser& aItem );        
        
        /**
        * Tests MsgAttachmentInfo DataType() and SetDataType()
        */
        TInt TestAtchInfoDataType( CStifItemParser& aItem );        

        /**
        * Tests MsgAttachmentInfo 
        */
        TInt TestAtchInfoSessionAttached( CStifItemParser& aItem );        

        /**
        * Tests MsgAttachmentInfo IsSupported() SetSupported() and Icon()
        */
        TInt TestAtchInfoSupportednIcon( CStifItemParser& aItem );        

        /**
        * Tests MsgAttachmentInfo SetSeparator() and IsSeparator()
        */
        TInt TestAtchInfoSeparator( CStifItemParser& aItem );        

        /**
        * Tests MsgAttachmentInfo SetDRMDataType() and DRMDataType()
        */
        TInt TestAtchInfoDRMDataType( CStifItemParser& aItem );        

        /**
        * Tests MsgAttachmentInfo IsSaved() and SetSaved(TBool)
        */
        TInt TestAtchInfoSaved( CStifItemParser& aItem );        

        /**
        * Tests MsgAttachmentInfo IsEmptyAttachment() and SetEmptyAttachment(TBool) 
        */
        TInt TestAtchInfoEmptyAttachment( CStifItemParser& aItem );        
       
        /**
        * Tests MsgAttachmentInfo NewL
        */
        TInt TestMsgRecipientItemL( CStifItemParser& aItem );
        
        /**
        * Tests MsgAttachmentInfo NewL
        */
        TInt TestMsgRecipientListL( CStifItemParser& aItem );
        
        /**
        * Tests methods of MsgEditorFlags
        */
        TInt TestMsgEditorFlagsL( CStifItemParser& aItem );
        
        /**
        * Tests Methods of MsgAttachmentUtils
        */
        TInt TestMsgAttachmentUtilsL( CStifItemParser& aItem );
        
        /**
        * Tests Methods of MsgEditorView
        */
        TInt TestMsgEditorViewL( CStifItemParser& aItem );
        
        /**
        * Tests MsgExpandableControls MsgExpandableControl.h and 
        * MsgExpandableTextEditorControl.h
        */
        TInt TestMsgExpandableControlsL( CStifItemParser& aItem );
        
        /**
        * Test Exported Methods of MsgExpandableControlEditor 
        */
        TInt TestMsgExpandableControlEditorL( CStifItemParser& aItem );        
        /**
         * Tests MsgBodyControl
         */
        TInt TestMsgBodyControlL();
        
        /**
         * Tests MsgBaseControl
         */
        TInt TestMsgBaseControlL();
        
        /**
        * 
        */
        TInt MsgIteratorForInboxL();
        
        /**
         * Tests MsgEditorAppUi
         */
		TInt TestMsgEditorAppUiL();
		
		/**
		* Test MsgEditorCommon
		*/
		TInt TestMsgEditorCommonL();
		
		/**
		* Test MsgAttachmentControl
		*/
		TInt TestMsgAttachmentControlL( CStifItemParser& aItem );
		
		/**
		*Test MsgViewAttachmentsDialog
		*/		
		TInt TestMsgViewAttachmentDialogL( CStifItemParser& aItem );

		/**
		*Test Protected funcions of MsgViewAttachmentsDialog.h
		*/
		TInt TestMsgViewAttachmentDialogproL( CStifItemParser& aItem );
		
		/**
		*Test MsgAddressControl
		*/		
		TInt TestMsgAddressControlL( CStifItemParser& aItem );
		
		/**
		*To Create MessageEditorView
		*/		
		TInt CreateMsgEditorViewL();

        //ADD NEW METHOD DEC HERE^
        //[TestMethods] - Do not remove      
        
        //Helper Functions
        
        /**
        * Creates recipientItem and returns pointer to CMsgRecipientItem
        */
        CMsgRecipientItem* CreateItemL();
        
        /**
        * Initializes MessageAttachmentModel
        */
        TInt InitMsgAtchmodelL( CStifItemParser& aItem );
        
        /**
        * Creates and sets observer to msgattachmentmodel
        */
        void SetObserverL();
        
        /**
        * Internally calls InitMsgAtchmodelL to help getting MessageAttachmentInfo's reference
        */
        TInt InitMsgAtchInfo( CStifItemParser& aItem );
        
        /**
        * CleanUp
        */
        void AtchModelCleanUp();
        
        /**
         * Gets File path from configuration file
         */
        void AssignFilePath( CStifItemParser& aItem );        

        /**
        * This function creates MsgEditorView
        */
        void CreateEditorViewL();
        
        /**
        * This function adds controls to msgeditorview (should be called afer
        * Creating view - otherwise leaves with KErrNotFound)
        */
        void AddControlstoEditorViewL();
        
        /**
        * This function deletes created view and removes resources from environment
        */
        TInt EditorViewCleanUp();
        
        /*
        * Method to open the current Message
        */
        TInt OpenCurrentMessageTestL();
        
        /*
        * Method to create the Iterator
        */
        TInt CreateMessageIteratorL(CStifItemParser& aItem);
        
        /*
        * Method to create a dummy message
        */
        void CreateDummyMessageL(const TMsvId aFolderId,TBool aBigMsg);
        
        /*
        * Method to clean the folder
        */
        void CleanFolderL(TMsvId aBoxId, CMsvSession& aSession);
        
        /*
        * Method to Create CRichText
        */        
        CRichText& CreateTextL();
        
        /*
        * method for cleaning all the messages
        */
        void CleanAllL();
        /*
        * method for cleaning a folder
        */
        void CleanBoxL(TMsvId aBoxId, CMsvSession& aSession);
        
    public:     // Data


    protected:  // Data


    private:    // Data
        
        CMsgAttachmentModel*    iMsgAttachModel;
        
        TBuf16<256>             iFileName;
        
        TMsvAttachmentId        iAtchId;
        
        TUint                   iCount;
        
        TInt                    iResView;
        
        TInt					iResAppUi; 
        
        CMsgEditorEngine*       iEngine;
        
        TBuf16<256>             iFilePath;

        TBool                   iFilePathflag;

        CCoeAppUi*              iappUi;
        
        CMsgEditorView*         iView;
        
        CMsvSession*            iSession;
        
        CMtmStore*              iMtmStore;
        
        CClientMtmRegistry*     iMtmRegistry;
        
        CSmsClientMtm*          iSmsClientMtm;
        
        TMsvId                  iSmsId;
        
        CMessageIterator*       iMessageIterator;
        
        TMsvEntry               iCurrentEntry;
        
        CSingleOpWatchers*      iOpWatchers;
        
        CMsvEntrySelection*     iSelectedEntries;
        
        CMsvEntry* 	            iEntry;
        
        CEikonEnv*              iAppEikonEnv;
        
        // Reserved pointer for future extension


    public:     // Friend classes
 
    protected:  // Friend classes

    private:    // Friend classes


    };

//class CDerMsgBaseControl;
//Derived class of CMsgBaseControl
class CDerMsgBaseControl: public CMsgBaseControl
    {
public:
    //Adding all pure virtuals of CMsgBaseControl
    CDerMsgBaseControl( CStifLogger* aLog );
    virtual TRect CurrentLineRect(){return TRect();}
    virtual void SetAndGetSizeL( TSize& /*aSize*/ ) {}
    virtual void ClipboardL( TMsgClipboardFunc /*aFunc*/ ){}
    virtual void EditL( TMsgEditFunc /*aFunc*/ ){}
    TBool IsFocusChangePossible( TMsgFocusDirection /*aDirection*/ )const{return ETrue;}
    virtual TBool IsCursorLocation( TMsgCursorLocation /*aLocation*/ )const{return ETrue;}
    virtual TUint32 EditPermission()const{return EMsgEditNone;}
    virtual void PrepareForReadOnly( TBool /*aReadOnly*/ ){}
    
    //Function to call all protected functions of CMsgBaseControl 
    void CheckMsgBaseCtrlPrtFunsL(void);
private:
    //For Logging
    CStifLogger*        iLog;
    };

#endif      // MESSAGINGEDITORAPITEST_H

// End of File
