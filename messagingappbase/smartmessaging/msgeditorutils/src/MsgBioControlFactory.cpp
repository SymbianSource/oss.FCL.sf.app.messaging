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
* Description:  
*		Bio control factory
*
*/



// INCLUDE FILES

#include "MsgBioControlFactory.h"   // for CMsgBioControlFactory
#include "msgbiocontrolObserver.h"  // for MMsgBioControlObserver
#include "MsgEditorUtils.pan"       // for panics and error codes
#include "MsgBioUids.h"

#include <s32file.h>                // for CFileStore
#include <biodb.h>                  // for CBIODatabase
#include <e32uid.h>                 // for uids
#include <msvapi.h>                 // MTM server API
#include <msgbiocontrol.h>
#include <data_caging_path_literals.hrh>
#include "MsgEditorUtilsLog.h"

//  LOCAL CONSTANTS AND MACROS

const TInt KEntryPointOrdinalNumber = 1; // Ordinal for NewL

typedef CMsgBioControl* (*TNewControlL)(MMsgBioControlObserver& aObserver,
                                       CMsvSession* aSession,
                                       TMsvId aId,
                                       TMsgBioMode aEditorOrViewerMode,
                                       const RFile* aFile);

//  MEMBER FUNCTIONS

EXPORT_C CMsgBioControlFactory* CMsgBioControlFactory::NewL()
    {
	LOG("CMsgBioControlFactory::NewL begin");
    CMsgBioControlFactory* self = new(ELeave) CMsgBioControlFactory();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    LOG("CMsgBioControlFactory::NewL end");
    return self;
    }

EXPORT_C CMsgBioControlFactory::~CMsgBioControlFactory()
    {
    iControlDllLibrary.Close();
    iFs.Close();
    }

EXPORT_C CMsgBioControl* CMsgBioControlFactory::CreateControlL(
    MMsgBioControlObserver& aBioControlObserver,
    const TUid& aBioMsgType,
    TMsgBioMode aEditorOrViewerMode,
    TMsvId aId,
    CMsvSession* aSession) //ownership not transferred
    {
    __ASSERT_ALWAYS( ( aId && aSession &&
        ( (aEditorOrViewerMode == EMsgBioEditorMode) ||
          (aEditorOrViewerMode == EMsgBioViewerMode) ) ),
        Panic(EMEUErrArgument) );

    return DoCreateControlL(
        aBioControlObserver,
        aBioMsgType,
        aEditorOrViewerMode,
        aId,
        aSession,
        NULL /* file handle */ );
    }

EXPORT_C CMsgBioControl* CMsgBioControlFactory::CreateControlL(
    MMsgBioControlObserver& aBioControlObserver,
    const TUid& aBioMsgType,
    TMsgBioMode aEditorOrViewerMode,
    const RFile& aFile)
    {
    __ASSERT_ALWAYS( ( ( (aEditorOrViewerMode == EMsgBioEditorMode) ||
                         (aEditorOrViewerMode == EMsgBioViewerMode) ) ),
                     Panic(EMEUErrArgument) );

    TMsvId nullMsvId = NULL;
    return DoCreateControlL(
        aBioControlObserver,
        aBioMsgType,
        aEditorOrViewerMode,
        nullMsvId,
        NULL, // of type CMsvSession*
        &aFile);
    }

CMsgBioControlFactory::CMsgBioControlFactory()
    {
    }

void CMsgBioControlFactory::ConstructL()
    {
    User::LeaveIfError(iFs.Connect());
    }

CMsgBioControl* CMsgBioControlFactory::DoCreateControlL(
    MMsgBioControlObserver& aBioControlObserver,
    const TUid& aBioMsgType,
    TMsgBioMode aEditorOrViewerMode,
    TMsvId aId,
    CMsvSession* aSession,
    const RFile* aFile)
    {
	LOG("CMsgBioControlFactory::DoCreateControlL begin");
    if(aBioMsgType.iUid == 0)
        {
        User::Leave(KMsgBioMessageTypeNotDefined);
        }

    CBIODatabase* bioDatabase = CBIODatabase::NewL(iFs);
    CleanupStack::PushL(bioDatabase);

    TPtrC controlName;
    TRAPD(err,controlName.Set(bioDatabase->GetBioControlNameL(aBioMsgType)));

    if(err == KErrNotFound)
    	User::Leave(KErrNotFound);

    TFileName fullname(NULL);
    fullname.Append( controlName );

    CleanupStack::PopAndDestroy(bioDatabase);

    __ASSERT_ALWAYS( iControlDllLibrary.Handle() == 0,
                Panic(EMEUOnlyOneLibraryAllowed));

    User::LeaveIfError(iControlDllLibrary.Load(fullname));

	TLibraryFunction libFunc = iControlDllLibrary.Lookup(
    	KEntryPointOrdinalNumber);
    if (libFunc == NULL)    // Check that we found the entry point
        {
        User::Leave(KMsgBadLibraryEntryPoint);
        }

    // Create a function pointer to NewL
    TNewControlL pFunc = reinterpret_cast<TNewControlL>( libFunc );

    CMsgBioControl* control = NULL;
    TRAPD(ret, control = ((*pFunc)(
        aBioControlObserver,
        aSession,
        aId,
        aEditorOrViewerMode,
        aFile)));
    if ((ret != KErrNone) || (control == NULL))
        {
        iControlDllLibrary.Close();
        User::Leave(ret);
        }
    LOG("CMsgBioControlFactory::DoCreateControlL end");
    return control;
    }
// End of file
