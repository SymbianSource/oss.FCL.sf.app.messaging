/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     Server side message editor/viewer service implementations
*
*/



#include <bldvariant.hrh>
#include <apgcli.h>
#include <eikenv.h>
#include <eikappui.h>
#include <aknappui.h>
#include <mmsconst.h>
#include <AknServerApp.h>
#include <s32mem.h>
#include "MuiuMsgEditorServiceSession.h"
#include "MuiuMsgEditorLauncher.h"

const TInt KMemoryStoreBuffSize = 127;

// ---------------------------------------------------------
// CMuiuMsgEditorServiceSession::CMuiuMsgEditorServiceSesson
// ---------------------------------------------------------
//
EXPORT_C CMuiuMsgEditorServiceSession::CMuiuMsgEditorServiceSession()
: iDoc(CEikonEnv::Static()->EikAppUi()->Document())
	{	
	}

// ---------------------------------------------------------
// CMuiuMsgEditorServiceSession::~CMuiuMsgEditorServiceSesson
// ---------------------------------------------------------
//
CMuiuMsgEditorServiceSession::~CMuiuMsgEditorServiceSession()
	{
	}

// ---------------------------------------------------------
// CMuiuMsgEditorServiceSession::CreateL
// ---------------------------------------------------------
//
void CMuiuMsgEditorServiceSession::CreateL()
	{
	CAknAppServiceBase::CreateL();		
	}
	
// ---------------------------------------------------------
// CMuiuMsgEditorServiceSession::ServiceL
// ---------------------------------------------------------
//
void CMuiuMsgEditorServiceSession::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
		case EMuiuMsgEditorServiceCmdOpen:
		    {
		    TRAPD( error, OpenEmbeddedL( aMessage ) );
		    if ( error != KErrNone )
                {
                static_cast<CAknAppUi*>( CEikonEnv::Static()->EikAppUi() )->RunAppShutter();
                User::Leave( error );
                }
			break;
		    }
		default:
		    {
		    CAknAppServiceBase::ServiceL(aMessage);
			break;
		    }
		}		
	}
	
// ---------------------------------------------------------
// CMuiuMsgEditorServiceSession::ServiceError
// ---------------------------------------------------------
//
void CMuiuMsgEditorServiceSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	CAknAppServiceBase::ServiceError(aMessage, aError);		
	}
	
// ---------------------------------------------------------
// CMuiuMsgEditorServiceSession::OpenEmbeddedL
// ---------------------------------------------------------
//
void CMuiuMsgEditorServiceSession::OpenEmbeddedL( const RMessage2& aMessage )
	{
	TEditorParameters params;
	params.iId = aMessage.Int0( );
	params.iFlags = aMessage.Int1( );
	params.iDestinationFolderId = aMessage.Int2( );
	params.iPartList = aMessage.Int3( );
    CBufStore* store = CBufStore::NewLC( KMemoryStoreBuffSize );
    RStoreWriteStream outStream;
    TStreamId id = outStream.CreateLC( *store );
    params.ExternalizeL( outStream );
    store->CommitL();
    CleanupStack::PopAndDestroy( );//outStream. 
	
    CStreamDictionary* dic = CStreamDictionary::NewLC();
    dic->AssignL( KUidMsvEditorParameterValue, id );
    outStream.CreateLC( *store );
    dic->ExternalizeL( outStream );
    store->CommitL();
    CleanupStack::PopAndDestroy( );// outstream

    // Restore the document from this store
    RStoreReadStream readStream;
    readStream.OpenLC( *store, id );
    iDoc->RestoreL( *store, *dic );

    CleanupStack::PopAndDestroy( 3, store );// dic, store, readStream
    aMessage.Complete(KErrNone);	
	}
