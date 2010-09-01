/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:         Tests MsgBioControl.h and CRichBio.h 
*
*/










// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <MsgBioControlObserver.h>
#include <msvapi.h>
#include "PluginBioControlAPITest.h"
#include "MsgBioCtrlTest.h"
#include "EdwinTestControl.h"
#include <PluginBioControlAPITest.rsg>
#include <coeaui.h>//coeappui
#include <CRichBio.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
#define DEFAULT_ENUM 100
#define ID_FILE 0
#define ID_DUMMY 1234
#define SCROLL_PIX 10
#define DUMMY_ARG 1

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPluginBioControlAPITest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CPluginBioControlAPITest::Delete() 
    {
    if( iEdwinTestCtrl )
        {
        CCoeEnv::Static()->AppUi()->RemoveFromStack( iEdwinTestCtrl );
        }
    if( iBioControl )
        {
        delete iBioControl;
        iBioControl = NULL;
        iEdwinTestCtrl = NULL;
        }
    iFileBased = EFalse;
    }

// -----------------------------------------------------------------------------
// CPluginBioControlAPITest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CPluginBioControlAPITest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "BioControlTest1L", CPluginBioControlAPITest::BioControlTestOneL ),
        ENTRY( "BioControlTest2L", CPluginBioControlAPITest::BioControlTestTwoL ),
        ENTRY( "ProtectedFnTestL", CPluginBioControlAPITest::BioCtrlProtectedTestL ),
        ENTRY( "RichBioNewLTestL", CPluginBioControlAPITest::RichBioNewLTestL ),        
        //ADD NEW ENTRY HERE^
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CPluginBioControlAPITest::FirstFreeCommand
// 
// 
// -----------------------------------------------------------------------------
//
TInt CPluginBioControlAPITest::FirstFreeCommand() const
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// CPluginBioControlAPITest::RequestHandleCommandL
// 
// 
// -----------------------------------------------------------------------------
//
TInt CPluginBioControlAPITest::RequestHandleCommandL(TMsgCommands aCommand)
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// CPluginBioControlAPITest::IsCommandSupported
// 
// 
// -----------------------------------------------------------------------------
//
TBool CPluginBioControlAPITest::IsCommandSupported(TMsgCommands aCommand) const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPluginBioControlAPITest::HandleSessionEventL
// 
// 
// -----------------------------------------------------------------------------
//
void CPluginBioControlAPITest::HandleSessionEventL(
            TMsvSessionEvent /*aEvent*/,
            TAny* /*aArg1*/,
            TAny* /*aArg2*/,
            TAny* /*aArg3*/)
    {
    // empty
    }

// -----------------------------------------------------------------------------
// CPluginBioControlAPITest::BioControlTestL
// Tests ConfirmationQueryL, OptionMenuPermissionsL, SetAndGetSizeL,
// VirtualHeight, VirtualVisibleTop, IsCursorLocation of MsgBioControl
// -----------------------------------------------------------------------------
//
TInt CPluginBioControlAPITest::BioControlTestOneL( CStifItemParser& aItem )
    {
    _LIT( KTestEntry, "Entry BioControlTestL");
    iLog->Log( KTestEntry );

    InitL( );
    CreateEdwinL();
    //Set EdwinControl as containerwindow for BioControl
    iBioControl->SetContainerWindowL( *iEdwinTestCtrl );  
    
    //  ConfirmationQueryL with resource 
    _LIT( KQuery, "Query" );
    TRAPD( err, iBioControl->ConfirmationQueryL( KQuery ));
    if( err )
        {
        _LIT( KError, "ConfirmationQueryL leaves with %d error" );
        iLog->Log( KError, err );
        }
    TRAP( err, iBioControl->OptionMenuPermissionsL());
    if( err )
        {
        _LIT( KError, "OptionMenuPermissionsL leaves with %d error" );
        iLog->Log( KError, err );        
        }
    
    //  SetAndGetSizeL- calls CRichBio's fn
    TSize size;
    size.SetSize( 100, 150 );
    TRAP( err, iBioControl->SetAndGetSizeL( size ));
    if( !err )
        {
        _LIT( KError, "SetAndGetSizeL Successful" );
        iLog->Log( KError );  
        }
    else
        {
        _LIT( KError, "SetAndGetSizeL leaves with %d error" );
        iLog->Log( KError, err );   
        }

    //  VirtualHeight
    if( V_HEIGHT == iBioControl->VirtualHeight())
        {
        _LIT( KError, "CMsgBioControl's VirtualHeight successful");
        iLog->Log( KError );        
        }
    else
        {
        _LIT( KError, "CMsgBioControl's VirtualHeight failed ");
        iLog->Log( KError );        
        }
    //  VirtualVisibleTop
    if( V_TOP == iBioControl->VirtualVisibleTop())
        {
        _LIT( KError, "CMsgBioControl's VirtualVisibleTop is successful ");
        iLog->Log( KError );        
        }
    else
        {
        _LIT( KError, "CMsgBioControl's VirtualVisibleTop failed ");
        iLog->Log( KError );        
        }
    //  IsCursorLocation
    TBool location = iBioControl->IsCursorLocation( EMsgTop );
    location = iBioControl->IsCursorLocation( EMsgBottom );
    location = iBioControl->IsCursorLocation( (TMsgCursorLocation)DEFAULT_ENUM );
   
    //  This function calls some of the RichBio functions 
    ((CMsgBioCtrlTest*)iBioControl)->RichBioFunctionCalls();
    
    Delete();
    
    _LIT( KTestExit, "Exit BioControlTestL");
    iLog->Log( KTestExit );    
    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CPluginBioControlAPITest::BioControlTest2L
// Tests ConfirmationQueryL of MsgBiocontrol ScrollL, NotifyViewEvent of MsgBiocontrol
// and CRichBio
// -----------------------------------------------------------------------------
//
TInt CPluginBioControlAPITest::BioControlTestTwoL( CStifItemParser& aItem )
    {
    _LIT( KTestEntry, "Entry BioControlTest2L");
    iLog->Log( KTestEntry );
    
    InitL( );

    //  ConfirmationQueryL with resource 
    TRAPD( err, iBioControl->ConfirmationQueryL( R_API_PASSED ));
    if( err != KErrNone )
        {
        _LIT( KErrQuery, "ConfirmationQueryL Leaves with %d code ");
        iLog->Log( KErrQuery, err);
        }
    else
        {
        _LIT( KErrQuery, "ConfirmationQueryL is successful ");
        iLog->Log( KErrQuery );        
        }
    
    // Tests ScrollL() and NotifyViewEvent()
    // Here we are not setting extension :- for decission coverage
#if  ! defined( __SERIES60_31__ ) && ! defined( __SERIES60_30__ ) 
    iBioControl->ScrollL( SCROLL_PIX, EMsgScrollDown ); 

    iBioControl->NotifyViewEvent( EMsgViewEventPrepareFocusTransitionUp , DUMMY_ARG );
#endif    
    // Extension is been set here
    CreateEdwinL();
    iBioControl->SetContainerWindowL( *iEdwinTestCtrl );    
   
    // ScrollL
#if  ! defined( __SERIES60_31__ ) && ! defined( __SERIES60_30__ ) 
    iBioControl->ScrollL( SCROLL_PIX, EMsgScrollDown );
    iBioControl->ScrollL( SCROLL_PIX, EMsgScrollUp );
    iBioControl->ScrollL( SCROLL_PIX, (TMsgScrollDirection)DEFAULT_ENUM );
    
    // Tests NotifyViewEventL
    iBioControl->NotifyViewEvent( EMsgViewEventPrepareFocusTransitionUp , DUMMY_ARG );
    iBioControl->NotifyViewEvent( EMsgViewEventPrepareFocusTransitionDown , DUMMY_ARG );
    iBioControl->NotifyViewEvent( EMsgViewEventSetCursorFirstPos , DUMMY_ARG );
    iBioControl->NotifyViewEvent( EMsgViewEventSetCursorLastPos , DUMMY_ARG );
    iBioControl->NotifyViewEvent( (TMsgViewEvent)DEFAULT_ENUM , DUMMY_ARG );
#endif    
    Delete();    
    
    _LIT( KTestExit, "Exit BioControlTest2L");
    iLog->Log( KTestExit );    
    return err;
    
    }


// -----------------------------------------------------------------------------
// CPluginBioControlAPITest::BioCtrlProtectedTestL
// Tests protected functions of CMsgBioControl
// -----------------------------------------------------------------------------
//
TInt CPluginBioControlAPITest::BioCtrlProtectedTestL( CStifItemParser& aItem )
    {
    _LIT( KTestEntry, "Entry ProtectedFnTestL Tests CMsgBioControl's protected functions");
    iLog->Log( KTestEntry );
    
    TPtrC filePath;
    aItem.GetNextString( filePath );
#ifdef __WINSCW__
    aItem.GetNextString( filePath );
#endif
    RFs fs;
    User::LeaveIfError(fs.Connect());
    fs.ShareProtected();
    
    //Open the file
    TInt error = iFile.Open(fs, filePath, EFileRead);
    if(error != KErrNone)
        {
        _LIT( KFileError, "Unable to Open File" );
        iLog->Log(KFileError);
        iFileBased = EFalse;
        fs.Close(); 
        }
    else
        {
        iFileBased = ETrue;
        fs.Close();
        }
    
    InitL( );  
    // AddMenuItemL
    TRAPD( err, ((CMsgBioCtrlTest*)iBioControl)->ProtectedCallsL( iFileBased ));
    if( err )
        {
        _LIT( KError, "CMsgBioControl::AddMenuItemL Leaves with %d error ");
        iLog->Log( KError, err );
        }
    else
        {
        _LIT( KError, "CMsgBioControl's protected functions successfully called ");
        iLog->Log( KError );
        }
    
    // NotifyEditorViewL
    TRAP( err, ((CMsgBioCtrlTest*)iBioControl)->CallNotifyEditorViewL());
    if( err )
        {
        _LIT( KError, "CMsgBioControl::NotifyEditorViewL Leaves with %d error ");
        iLog->Log( KError, err );
        }
    else
        {
        _LIT( KError, "CMsgBioControl::NotifyEditorViewL successfully called ");
        iLog->Log( KError );
        }    
    Delete();
    
    _LIT( KTestExit, "Exit ProtectedFnTestL");
    iLog->Log( KTestExit );     
    return error;
    }

// -----------------------------------------------------------------------------
// CPluginBioControlAPITest::RichBioNewLTestL
// Tests CRichBio's NewL
// -----------------------------------------------------------------------------
//
TInt CPluginBioControlAPITest::RichBioNewLTestL( CStifItemParser& aItem )
    {
    _LIT( KTestEntry, "Entry RichBioNewLTestL Tests CRichBio::NewL");
    iLog->Log( KTestEntry );
    CreateEdwinL();
    //  call NewL of CRichBio
    CRichBio* rBio = CRichBio::NewL( iEdwinTestCtrl, ERichBioModeEditorBase );    
    delete rBio;    
    Delete();
    _LIT(KTestExit, "Exit RichBioNewLTestL CRichBio::NewL Passed");
    iLog->Log( KTestExit );
    return KErrNone;
    }


//Helper functions
// -----------------------------------------------------------------------------
// CPluginBioControlAPITest::ProtectedFnTestL
// Creates BioControl and gets CMsvSession
// -----------------------------------------------------------------------------
//
void CPluginBioControlAPITest::InitL( )
    {
    //Get pointer to CMsvSession
    if( iSession )
        {
        ;
        }
    else
        {
        TRAPD( err, iSession = CMsvSession::OpenSyncL(*this));
        if( err != KErrNone )
            {
            _LIT( KErrSession, "CMsvSession::OpenSyncL Leaves with %d error");
            iLog->Log( KErrSession, err);
            User::Leave( err );
            }
        else
            {
            _LIT( KErrSession, "CMsvSession::OpenSyncL gave session succesfully");
            iLog->Log( KErrSession );
            }
        }
    
    //  Create MsgBioControl
    if( iBioControl )
        {
        return;
        }
    else
        {
        TInt id;
        if( iFileBased )
            {
            id = ID_FILE;
            }
        else
            {
            id = ID_DUMMY;
            }
        TRAPD( err, iBioControl = CMsgBioCtrlTest::NewL( *this, iSession, id, EMsgBioViewerMode, &iFile));
        if( err != KErrNone )
            {
            _LIT( KErrBio, "CMsgBioCtrlTest::NewL Leaves with %d error");
            iLog->Log( KErrBio, err);
            User::Leave( err );
            }
        else
            {
            _LIT( KErrBio, "CMsgBioCtrlTest::NewL is succesfull");
            iLog->Log( KErrBio );
            }        
        }
    }

// -----------------------------------------------------------------------------
// CPluginBioControlAPITest::CreateEdwin
// Creates EdwinControl
// -----------------------------------------------------------------------------
//
void CPluginBioControlAPITest::CreateEdwinL()
    {
    if( iEdwinTestCtrl )
        {
        return;
        }
    else
        {
        //Create EdwinControl
        TRAPD( err, iEdwinTestCtrl = CEdwinTestControl::NewL());
        if( err )
            {
            _LIT( KError, "CEdwinTestControl::NewL Leaves with %d error ");
            iLog->Log( KError, err );
            User::Leave( err );
            }
        else
            {
            _LIT( KError, "CEdwinTestControl::NewL is successful ");
            iLog->Log( KError );
            }
        CCoeEnv::Static()->AppUi()->AddToStackL( iEdwinTestCtrl );
        }
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
