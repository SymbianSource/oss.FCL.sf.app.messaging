/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  init implementation
*
*/




// INCLUDE FILES
#include <e32base.h>
#include <e32test.h>
#include <eikenv.h>
#include <eikappui.h>
#include <msvapi.h>
#include <msvuids.h>
#include <mtmuibas.h>
#include <mtuireg.h>
#include <mtudcbas.h>
#include <bautils.h>
#include <mtmuidef.hrh>
#include <eikdialg.h>
#include <featmgr.h>

#include "init.h"

// CONSTANTS
const TInt KUidMsgTypeSMSValue  = 0x1000102C;
const TUid KUidMsgTypeSMSUid   = {KUidMsgTypeSMSValue}; 
const TInt KUidMsgTypeMMSValue  = 0x100058E1;
const TUid KUidMsgTypeMMSUid   = {KUidMsgTypeMMSValue}; 
const TInt KUidMsgTypePocaValue = 0x10207245;
const TUid KUidMsgTypePocaUid   = {KUidMsgTypePocaValue};
const TInt KUidMsgTypeAMSValue  = 0x1020745B;
const TUid KUidMsgTypeAMSUid    = {KUidMsgTypeAMSValue};
const TInt KMailInitPanicStringLength = 15;

_LIT(KInitSpaceBuffer, " ");
_LIT(KInitColon, " ");
_LIT(KThread,   "MTMInit");
_LIT(KPanic,    "MTMInit");
#ifdef __WINS__
_LIT(KDllName,"\\system\\programs\\mailinit.dll");
#endif
#ifdef __EPOC32__
_LIT(KProcess,  "MTMInit");
#endif
#ifdef _DEBUG
const TInt KInitErrorMaxTextLength=80;
#endif

const TInt KMailInitRetryCounter = 10;
const TInt KMailInitRetryTimeout = 2000000; // two seconds

#ifdef __MARM__
//  To stop warnings of missing .def file for a EXEDLL compilation
//  under MARM (as suggested by Alistair Bradley)
//
EXPORT_C void Dummy() {};
#endif



/**
*  CDummyObserver 
*  This is just a temporary MMsvSessionObserver during the mail init
*/
class CDummyObserver : public CBase, public MMsvSessionObserver
    {
    public:
        CDummyObserver() {};
        void HandleSessionEventL(
            TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/) {};
    };

// ----------------------------------------------------
// DeleteStoreInitFlagFileL
// ----------------------------------------------------
void DeleteStoreInitFlagFileL()
    {
    }

// ----------------------------------------------------
// DoInitialiseSingleMtmL
// ----------------------------------------------------
LOCAL_C void DoInitialiseSingleMtmL(CClientMtmRegistry& aBaseReg, CMtmUiRegistry& aUiReg, const TUid& anId)
    {
    INITLOGGER_WRITE_FORMAT2( "DoInitialiseSingleMtmL() - mtm uid: %d 0x%x", anId.iUid, anId.iUid);

    INITLOGGER("DoInitialiseSingleMtmL() - Creating BaseMtm");
    //create base mtm
    CBaseMtm* baseMtm=aBaseReg.NewMtmL(anId);
    CleanupStack::PushL(baseMtm);
    
    INITLOGGER("DoInitialiseSingleMtmL() - Creating Mtm UI");
    CBaseMtmUi* mtmUi=aUiReg.NewMtmUiL(*baseMtm);
    CleanupStack::PushL(mtmUi);

    TInt response;
    TUid isInstalled={KUidMsvMtmUiQueryMessagingInitialisation};
    TInt err=mtmUi->QueryCapability(isInstalled, response);

    if (err==KErrNone)
        {
        INITLOGGER("DoInitialiseSingleMtmL() - Invoking initialisation method");
        // Need to install this type
        CMsvEntrySelection* tempobject = new (ELeave) CMsvEntrySelection;
        CleanupStack::PushL(tempobject);
        TBuf8<1> tempdes;
        TInt installNow={KMtmUiMessagingInitialisation};    
        mtmUi->InvokeSyncFunctionL(installNow,*tempobject,tempdes);
        CleanupStack::PopAndDestroy( tempobject );
        }
    else if (err!=KErrNotSupported)
        {
        User::Leave(err);
        }

    CleanupStack::PopAndDestroy( 2, baseMtm ); // mtmUi,baseMtm
    INITLOGGER("DoInitialiseSingleMtmL() - Complete");
    }

// ----------------------------------------------------------------------------
// GetMtmName()
// ----------------------------------------------------------------------------
//
LOCAL_C void GetMtmName( 
    TBuf<KMaxKernelName>& aMtmName, 
    const TUid& aUid,
    const CMtmUiRegistry& aUiRegistry )
    {    
    aMtmName = aUiRegistry.RegisteredMtmDllInfo(
        aUid ).HumanReadableName().Left( KMailInitPanicStringLength - 1 ); 
    }
    
// ----------------------------------------------------------------------------
// AppendPanicString()
// ----------------------------------------------------------------------------
//
LOCAL_C void AppendPanicString( 
    TBuf<KMaxKernelName>& aPanicString, 
    const TUid& aUid,
    const CMtmUiRegistry& aUiRegistry )
    {    
    // Make sure the new length fits to total name
    if ( aPanicString.Length() < KMaxKernelName - KMailInitPanicStringLength )
        {
        GetMtmName( aPanicString, aUid, aUiRegistry );
        aPanicString.Append( aPanicString );
        aPanicString.Append( KInitSpaceBuffer );
        }
    }
    
// ----------------------------------------------------
// DoInitialiseMtmsL
// ----------------------------------------------------
#ifdef _DEBUG
LOCAL_C void DoInitialiseMtmsL( TDes& aMtmBuffer )
#else
LOCAL_C void DoInitialiseMtmsL( )
#endif
    {
    TBuf<KMaxKernelName> panicString;
    panicString.Copy( KThread );
    panicString.Append( KInitColon );
    
    INITLOGGER("DoInitialiseMtmsL()");
    // connect to the server
    CDummyObserver* ob = new(ELeave)CDummyObserver;
    CleanupStack::PushL(ob);

    INITLOGGER("DoInitialiseMtmsL() : Creating CMsvSession");
    TInt error = KErrNone;
    TInt counter = KMailInitRetryCounter;
    CMsvSession* session = NULL;
    TRAP( error, session = CMsvSession::OpenSyncL(*ob) );
    while ( counter > 0 && error != KErrNone )
        {
        INITLOGGER("DoInitialiseMtmsL() : Creating CMsvSession failed, wait and try again");
        counter--;
        User::After( KMailInitRetryTimeout );
        TRAP( error, session = CMsvSession::OpenSyncL(*ob) );
        }
    if ( error != KErrNone || session == NULL )
        {
        INITLOGGER("DoInitialiseMtmsL() : Creating CMsvSession failed, leave now");
        User::Leave( error != KErrNone ? error : KErrNotFound );
        }
    CleanupStack::PushL(session);
    
    //create ui registry
    INITLOGGER("DoInitialiseMtmsL() : Creating Registries");
    CClientMtmRegistry* basemtmregistry=CClientMtmRegistry::NewL(*session);
    CleanupStack::PushL(basemtmregistry);
    CMtmUiRegistry* uiRegistry=CMtmUiRegistry::NewL(*session);
    CleanupStack::PushL(uiRegistry);

    //check through mtms
    TInt ret = 0;

    // try to initialise SMS first
    INITLOGGER( "-----------------------< Start INIT: SMS >-----------------------" );
    if ( uiRegistry->IsPresent( KUidMsgTypeSMSUid ) )
        {
        TRAP(ret, DoInitialiseSingleMtmL(*basemtmregistry, *uiRegistry, KUidMsgTypeSMSUid));
        }
#ifdef _DEBUG
    else
        {
        INITLOGGER("DoInitialiseMtmsL() - SMS Not present, this might be ok?");
        }
#endif

    if (ret)
        {
        error = ret;
        ret = 0;
        
        // Track SMS, which can't fail of course
        AppendPanicString( panicString, KUidMsgTypeSMSUid, *uiRegistry );
        
#ifdef _DEBUG
        INITLOGGER_WRITE_FORMAT( "***** !!! SMS INIT FAILED (%d) !!! *****", error );  
        aMtmBuffer.Append( KInitSpaceBuffer );
        aMtmBuffer.AppendNum( KUidMsgTypeSMSValue );
#endif
        }
    INITLOGGER( "------------------------< End INIT: SMS >------------------------" );        
        
    INITLOGGER( "-----------------------< Start INIT: MMS >-----------------------" );    
    // then try to initialise MMS 
    if ( uiRegistry->IsPresent( KUidMsgTypeMMSUid ) )
        {
        TRAP(ret, DoInitialiseSingleMtmL(*basemtmregistry, *uiRegistry, KUidMsgTypeMMSUid ));
        }
#ifdef _DEBUG
    else
        {
        INITLOGGER("DoInitialiseMtmsL() - MMS Not present, this might be ok?");
        }
#endif

    if (ret)
        {         
        // Track MMS
        AppendPanicString( panicString, KUidMsgTypeMMSUid, *uiRegistry );

        error = ret;
        ret = 0;       
        
#ifdef _DEBUG
        INITLOGGER_WRITE_FORMAT( "***** !!! MMS INIT FAILED (%d) !!! *****", error );  
        aMtmBuffer.Append( KInitSpaceBuffer );
        aMtmBuffer.AppendNum( KUidMsgTypeMMSValue );
#endif
        }

    INITLOGGER( "------------------------< End INIT: MMS >------------------------" );

    //now check through the other mtms
#ifdef _DEBUG    
    TBuf<KMaxKernelName> debugString;
#endif // _DEBUG    
    TInt count=uiRegistry->NumRegisteredMtmDlls();
    for (TInt cc=0; cc<count; ++cc)
        {
        TUid uid=uiRegistry->MtmTypeUid(cc);
        
        INITLOGGER_WRITE_FORMAT( "-------------------< Start INIT: 0x%x >------------------", uid.iUid );
#ifdef _DEBUG
        debugString.Zero();
        GetMtmName( debugString, uid, *uiRegistry );
        debugString.Append( '\0' );
        INITLOGGER_WRITE_FORMAT2( "0x%x: %s", uid.iUid, debugString.Ptr() );
#endif // _DEBUG
        
        TBool initialise = ETrue;
        // Postcard or Audio messaging MTMs can be variated by runtime, so depending
        // on the flags they might not be initialized at all.
        if( uid == KUidMsgTypePocaUid || uid == KUidMsgTypeAMSUid)
            {
            TInt flag = -1;
            if ( uid == KUidMsgTypePocaUid )
                {
                flag = KFeatureIdMmsPostcard;
                }
            else if ( uid == KUidMsgTypeAMSUid )
                {
                flag = KFeatureIdAudioMessaging;
                }
            FeatureManager::InitializeLibL();
            if ( flag != -1 )
                {
                if ( !FeatureManager::FeatureSupported( flag ) )
                    {
                    INITLOGGER( "* Feature NOT supported *" );
                    initialise = EFalse;
                    }
                }
            FeatureManager::UnInitializeLib();
            }

        if (uid.iUid == KUidMsgTypeSMSValue || uid.iUid == KUidMsgTypeMMSValue )
            {
            INITLOGGER( "* Already Initialized *" );
            initialise = EFalse;
            }


        if ( initialise )        
            {
            // return value is ignored, so as not to prevent other MTMs being initialised
            TRAP(ret, DoInitialiseSingleMtmL(*basemtmregistry, *uiRegistry, uid));
            if (ret)
                {
                     
                // Track Other Uids
                AppendPanicString( panicString, uid, *uiRegistry );

                error=ret;
#ifdef _DEBUG
                INITLOGGER_WRITE_FORMAT( "***** !!! INIT FAILED (%d) !!! *****", error );            
                aMtmBuffer.Append( KInitSpaceBuffer );
                aMtmBuffer.AppendNum( (TInt)uid.iUid );
#endif
                }
                
            INITLOGGER_WRITE_FORMAT( "--------------------< End INIT: 0x%x >-------------------", uid.iUid );    
            }
#ifdef _DEBUG
        else
            {
            INITLOGGER_WRITE_FORMAT( "--------------------< End INIT: NOT done for 0x%x, see reason above >-------------------", uid.iUid );
            }
#endif
        }
    
    if (error)
        {
        INITLOGGER_WRITE_FORMAT( "DoInitialiseMtmsL() - %S", &panicString );
        INITLOGGER_WRITE_FORMAT( "DoInitialiseMtmsL() - Leave error %d", error);

        User::RenameThread( panicString );
        Panic( EMailInitisationFailed );
        }

    CleanupStack::PopAndDestroy(4); // uiRegistry,basemtmregistry,session,ob
    }


// ----------------------------------------------------
// CMailInitAppUi::~CMailInitAppUi
// ----------------------------------------------------
CMailInitAppUi::~CMailInitAppUi()
    {
    }


// ----------------------------------------------------
// CMailInitAppUi::ConstructL
// ----------------------------------------------------
void CMailInitAppUi::ConstructL()
    {
    CEikAppUi::BaseConstructL(ENonStandardResourceFile|ENoScreenFurniture);
    }



// ----------------------------------------------------
// CMailInitServerEnv::DestroyEnvironment
// ----------------------------------------------------
void CMailInitServerEnv::DestroyEnvironment()
    {
    CEikonEnv::DestroyEnvironment();
    }

// ----------------------------------------------------
// CMailInitServerEnv::ConstructL
// ----------------------------------------------------
void CMailInitServerEnv::ConstructL()
    {
    CEikonEnv::ConstructL( EFalse );
    SetAutoForwarding( ETrue );
    }

// ----------------------------------------------------
// DoWork
// ----------------------------------------------------
LOCAL_C void DoWork()
    {
    INITLOGGER("DoWork()");

#ifndef _DEBUG
    TRAPD(leave, DoInitialiseMtmsL());
    if(leave == KErrNone)
        {
        // Delete flag file if present.
        TRAP_IGNORE(DeleteStoreInitFlagFileL());
        }
#else
    TBuf<2*KInitErrorMaxTextLength> errorBuffer;
    TRAPD(leave, DoInitialiseMtmsL( errorBuffer ));
    if(leave == KErrNone)
        {
        // Delete flag file if present.
        TRAP_IGNORE(DeleteStoreInitFlagFileL());
        return;
        }

#endif // _DEBUG
    }


// ----------------------------------------------------
// StartFunction
// ----------------------------------------------------
GLDEF_C TInt StartFunction()
    {
    INITLOGGER("StartFunction()");
#ifdef __EPOC32__
    // Rename our process (on h/w only).
    User::RenameProcess( KProcess );
#endif
    // Rename our thread.
    //User::RenameThread
    User::RenameThread( KThread );

    INITLOGGER("StartFunction(): Constructing Env and AppUi");
    CMailInitServerEnv* dialogEnv = new CMailInitServerEnv;
    __ASSERT_ALWAYS(dialogEnv!=NULL, Panic(EMailInitPanicStartAllocEnv));
    CMailInitAppUi* dialogAppUi = new CMailInitAppUi;
    __ASSERT_ALWAYS(dialogAppUi!=NULL, Panic(EMailInitPanicStartAllocAppUi));

    TRAPD(leave,{dialogEnv->ConstructL(); dialogAppUi->ConstructL();});
    __ASSERT_ALWAYS(leave==KErrNone, Panic(EMailInitPanicStartConstructL));

    dialogEnv->DisableExitChecks(ETrue);

    CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
    __ASSERT_ALWAYS(cleanup!=NULL, Panic(EMailInitPanicStartAllocCleanupStack));
    DoWork();
    delete cleanup; // destroy clean-up stack
    
    // finished
    dialogAppUi->PrepareToExit();

    dialogEnv->DestroyEnvironment();

    return KErrNone;
    }

// ----------------------------------------------------
// WinsMain
// ----------------------------------------------------
#ifdef __WINS__
EXPORT_C TInt WinsMain(TAny* /*aParam*/)
    {
	  return KErrNone;
    }
#endif

// ----------------------------------------------------
// E32Main
// ----------------------------------------------------
GLDEF_C TInt E32Main()
    {
    return StartFunction();
    }

// ----------------------------------------------------
// Panic
// ----------------------------------------------------
GLDEF_C void Panic(TMailInitPanic aPanic) // called if the server fails to be created
    {
    User::Panic(KPanic, aPanic);
    }


// ----------------------------------------------------
// E32Dll
// ----------------------------------------------------
#ifdef __WINS__
GLDEF_C TInt E32Dll(TInt /*aReason*/)
    {
    return KErrNone;
    }
#endif

// End of file
