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
*      Implementation of Scheme handler interface implementation 
*      for localapp: scheme
*
*/



// INCLUDE FILES

#include "LocalAppHandler.h"
#include "SchemeDefs.h"
#include "SchemeDispLogger.h"
#include "MmsToHandler.h"
#include "SmsHandler.h"
#include <ecom.h>		// For REComSession
#include <eikenv.h>
#include <DocumentHandler.h>
#include <apgcli.h>
#include <apparc.h>
#include <eikdoc.h>
#include <eikproc.h>
#include <f32file.h>
#include <txtrich.h>
#include <eikappui.h>      
#include <apgtask.h>        // for TApaTaskList

// ================= CONSTANTS =======================


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CLocalAppHandler::NewL()
// ---------------------------------------------------------
//
CLocalAppHandler* CLocalAppHandler::NewL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CLocalAppHandler::NewL()" );

	CLocalAppHandler* self = new( ELeave ) CLocalAppHandler();
	CleanupStack::PushL( self );
	self->ConstructL( aUrl );
	CleanupStack::Pop( self );

	CLOG_LEAVEFN( "CLocalAppHandler::NewL()" );

	return self;
	}

// ---------------------------------------------------------
// CLocalAppHandler::~CLocalAppHandler()
// ---------------------------------------------------------
//
CLocalAppHandler::~CLocalAppHandler()
	{
	CLOG_ENTERFN( "CLocalAppHandler::~CLocalAppHandler()" );

	CLOG_LEAVEFN( "CLocalAppHandler::~CLocalAppHandler()" );
	}

// ---------------------------------------------------------
// CLocalAppHandler::CLocalAppHandler()
// ---------------------------------------------------------
//
CLocalAppHandler::CLocalAppHandler() : CBaseHandler(), iToday( EFalse ), iEmbedded( EFalse )
	{
	// Deliberately do nothing here : 
	// See ConstructL() for initialisation completion.
	}

// ---------------------------------------------------------
// CLocalAppHandler::ConstructL()
// ---------------------------------------------------------
//
void CLocalAppHandler::ConstructL( const TDesC& aUrl )
	{
	CLOG_ENTERFN( "CLocalAppHandler::ConstructL()" );

	BaseConstructL( aUrl );
	
	CLOG_LEAVEFN( "CLocalAppHandler::ConstructL()" );
	}

// ---------------------------------------------------------
// CLocalAppHandler::HandleUrlEmbeddedL()
// ---------------------------------------------------------
//
void CLocalAppHandler::HandleUrlEmbeddedL()
	{
	CLOG_ENTERFN( "CLocalAppHandler::HandleUrlEmbeddedL()" );
    iEmbedded = ETrue;
    HandleUrlL();
    NotifyClient();
	CLOG_LEAVEFN( "CLocalAppHandler::HandleUrlEmbeddedL()" );
	}

// ---------------------------------------------------------
// CLocalAppHandler::HandleUrlStandaloneL()
// ---------------------------------------------------------
//
void CLocalAppHandler::HandleUrlStandaloneL()
	{
	CLOG_ENTERFN( "CLocalAppHandler::HandleUrlStandaloneL()" );
    iEmbedded = EFalse;
    HandleUrlL();    
	CLOG_LEAVEFN( "CLocalAppHandler::HandleUrlStandaloneL()" );
	}

// ---------------------------------------------------------
// CLocalAppHandler::HandleUrlL()
// ---------------------------------------------------------
//
void CLocalAppHandler::HandleUrlL()
    {
    TInt start = 0;
    TInt length= 0;
    // Find "localapp://" from the beginning
    if ( 0 == iParsedUrl->FindF( KLocalAppSlash ))
        {
        length = KLocalAppSlash.iTypeLength;
        }
    // Else find  "localapp:" from the beginning
    else if ( 0 == iParsedUrl->FindF( KLocalApp ) )
        {
        length = KLocalApp.iTypeLength;
        }
    else
        {
        return; // not recognized URI.
        }

    TPtrC startPtrC = iParsedUrl->Mid( length );

    // Remove space from the beginning
    while ( start < startPtrC.Length()  && ' ' ==  startPtrC[start] )
        {
        ++start;
        }
    TPtrC headerPtrC = startPtrC.Mid( start );
    
    if ( 0 == headerPtrC.FindF( KContactsId ) )
        {
        HandleContactsL();
        }
    else if ( 0 == headerPtrC.FindF( KCalendarId ) )
        {
        HandleCalendarL();
        }
    else if ( 0 == headerPtrC.FindF( KMessagingId ) )
        {
        HandleMessagingL();
        }
    else if ( 0 == headerPtrC.FindF( KMmsId ) )
        {
        HandleMmsL();
        }
    else if ( 0 == headerPtrC.FindF( KSmsId ) )
        {
        HandleSmsL();
        }
    else if ( 0 == headerPtrC.FindF( KJamId ) )
        {
        HandleJamL();
        }
    else
        {
        return; // do nothing, header was unknown
        }
    }
// ---------------------------------------------------------
// CLocalAppHandler::HandleJamL()
// ---------------------------------------------------------
//
void CLocalAppHandler::HandleJamL()
    {
    // Change "localapp:..." to "javaapp:..."    
    HBufC *modifiedUrl = iParsedUrl->AllocL();
    CleanupStack::PushL( modifiedUrl );
    TPtr ptrModifiedUrl = modifiedUrl->Des();
    ptrModifiedUrl.Replace( 0, KLocalApp.iTypeLength, KJavaApp );
    
    CSchemeHandler *schemeHandler = CSchemeHandler::NewL( ptrModifiedUrl );
    CleanupStack::PushL( schemeHandler );
    
    if (iEmbedded)
        {
        schemeHandler->HandleUrlEmbeddedL();
        }
    else
        {
        schemeHandler->HandleUrlStandaloneL();   
        }
    CleanupStack::PopAndDestroy( schemeHandler );
    CleanupStack::PopAndDestroy( modifiedUrl );
    }
// ---------------------------------------------------------
// CLocalAppHandler::HandleMmsL()
// ---------------------------------------------------------
//
void CLocalAppHandler::HandleMmsL()
    {
    TInt compose = iParsedUrl->FindF( KCompose );
    TInt show = iParsedUrl->FindF( KShow );
    if ( show == KErrNotFound && compose == KErrNotFound )
        {
        return;
        }
    else if ( ( show != KErrNotFound && compose == KErrNotFound ) 
            || show != KErrNotFound && show < compose  )
        {
        TInt inbox = iParsedUrl->FindF( KInbox );
        TInt composer = iParsedUrl->FindF( KComposer );
        if ( ( inbox != KErrNotFound && compose == KErrNotFound ) || 
             ( inbox != KErrNotFound && inbox < composer ) )
            {
            TUid viewUid( KMceInboxViewId );
            TVwsViewId tvwsViewId(KUidMessaging, viewUid);
            TBuf8<1> customMessage;
            CEikonEnv::Static()->EikAppUi()->ActivateViewL( tvwsViewId, KMessageId, customMessage );
            }
        else
            {
            CMmsToHandler* mmsto = CMmsToHandler::NewL( KMmsto );
            CleanupStack::PushL( mmsto );
            if (iEmbedded)
                {
                mmsto->HandleUrlEmbeddedL();
                }
            else
                {
                mmsto->HandleUrlStandaloneL();   
                }
            CleanupStack::PopAndDestroy( mmsto );
            }
        }
    else 
        {
        TPtrC url = iParsedUrl->Mid( compose + KCompose.iTypeLength ); 
        CMmsToHandler* mmsto = CMmsToHandler::NewL( url );
        CleanupStack::PushL( mmsto );
        if (iEmbedded)
            {
            mmsto->HandleUrlEmbeddedL();
            }
        else
            {
            mmsto->HandleUrlStandaloneL();   
            }
        CleanupStack::PopAndDestroy( mmsto );
        }
    }
// ---------------------------------------------------------
// CLocalAppHandler::HandleSmsL()
// ---------------------------------------------------------
//
void CLocalAppHandler::HandleSmsL()
    {
    TInt compose = iParsedUrl->FindF( KCompose );
    TInt show = iParsedUrl->FindF( KShow );
    if ( show == KErrNotFound && compose == KErrNotFound )
        {
        return;
        }
    else if ( ( show != KErrNotFound && compose == KErrNotFound ) 
            || show != KErrNotFound && show < compose  )
        {
        TInt inbox = iParsedUrl->FindF( KInbox );
        TInt composer = iParsedUrl->FindF( KComposer );
        if ( ( inbox != KErrNotFound && compose == KErrNotFound ) || 
             ( inbox != KErrNotFound && inbox < composer ) )
            {
            TUid viewUid( KMceInboxViewId );
            TVwsViewId tvwsViewId( KUidMessaging, viewUid );
            TBuf8<1> customMessage;
            CEikonEnv::Static()->EikAppUi()->ActivateViewL( tvwsViewId, KMessageId, customMessage );
            }
        else
            {
            CSmsHandler* sms = CSmsHandler::NewL( KSms );
            CleanupStack::PushL( sms );
            if ( iEmbedded )
                {
                sms->HandleUrlEmbeddedL();
                }
            else
                {
                sms->HandleUrlStandaloneL();   
                }
            CleanupStack::PopAndDestroy( sms );
            }
        }
    else 
        {
        TPtrC url = iParsedUrl->Mid( compose + KCompose.iTypeLength ); 
        CSmsHandler* sms = CSmsHandler::NewL( url );
        CleanupStack::PushL( sms );
        if ( iEmbedded )
            { 
            sms->HandleUrlEmbeddedL();
            }
        else
            {
            sms->HandleUrlStandaloneL();   
            }
        CleanupStack::PopAndDestroy( sms );
        }
    }
// ---------------------------------------------------------
// CLocalAppHandler::HandleCalendarL()
// ---------------------------------------------------------
//
void CLocalAppHandler::HandleCalendarL()
    {
    iAppUid = KUidCalendar;
    if ( KErrNotFound != iParsedUrl->FindF( KToday ) )
        {
        iToday = ETrue;
        }
    StartAppL();
    }
// ---------------------------------------------------------
// CLocalAppHandler::HandleContactsL()
// ---------------------------------------------------------
//
void CLocalAppHandler::HandleContactsL()
    {
    iAppUid = KUidContacts ;
    StartAppL();
    }
// ---------------------------------------------------------
// CLocalAppHandler::HandleMessagingL()
// ---------------------------------------------------------
//
void CLocalAppHandler::HandleMessagingL()
    {
    iAppUid = KUidMessaging;
    StartAppL();
    }

// ---------------------------------------------------------
// CLocalAppHandler::StartAppL()
// ---------------------------------------------------------
//
void CLocalAppHandler::StartAppL()
    {
    RWsSession wsServer;
    User::LeaveIfError( wsServer.Connect() );
    CleanupClosePushL( wsServer );
    TApaTaskList taskList( wsServer );
    TApaTask task(taskList.FindApp( iAppUid ));

    if ( task.Exists() )
        {
        task.BringToForeground();   
        }
    else
        {
        TApaAppInfo appInfo;
        TApaAppCapabilityBuf capabilityBuf;
        RApaLsSession lsSession;
        User::LeaveIfError( lsSession.Connect() );
        CleanupClosePushL( lsSession );
        User::LeaveIfError(lsSession.GetAppInfo(appInfo, iAppUid ));
        User::LeaveIfError(lsSession.GetAppCapability(capabilityBuf, iAppUid ));
        TApaAppCapability& caps = capabilityBuf();

        TFileName appName = appInfo.iFullName;
        CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
        #if (defined(SYMBIAN_SUPPORT_UI_FRAMEWORKS_V1) || defined(__SERIES60_27__) || defined(__SERIES60_28__)) 
        cmdLine->SetLibraryNameL(appName);
        #else
        cmdLine->SetExecutableNameL(appName);
        #endif
        
        // If Calendar's Day view selected
        if ( iAppUid == KUidCalendar && iToday )
            {
            TBuf8<12> todayBuf;
            todayBuf.Copy( (TUint8*)KDesToday().Ptr(), ( KDesToday.iTypeLength * 2 ) );
            cmdLine->SetTailEndL( todayBuf );
            }

        if (caps.iLaunchInBackground)
            {
            cmdLine->SetCommandL(EApaCommandBackground);
            }
        else
            {
            cmdLine->SetCommandL(EApaCommandRun);
            }

        User::LeaveIfError(lsSession.StartApp(*cmdLine));
        
        CleanupStack::PopAndDestroy(cmdLine);
        CleanupStack::PopAndDestroy(); // lsSession   
        } // else

    CleanupStack::PopAndDestroy(); // wsServer   
    }

// End of file

