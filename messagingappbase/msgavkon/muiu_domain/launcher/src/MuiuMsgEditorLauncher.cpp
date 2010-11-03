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
*  Static class which launches editor application (provided editor
*  conforms to \msgeditor architecture.
*
*/




// INCLUDE FILES
#include <bldvariant.hrh>  // For feature flags 
#include <apacmdln.h>
#include <apacln.h>
#include <apgcli.h>
#include <eikproc.h>
#include <eikdll.h>
#include <eikenv.h>
#include <eikdoc.h>
#include <aknclearer.h>  // for CAknLocalScreenClearer
#include <s32mem.h>
#include <apaserverapp.h>
#include <barsread2.h>
#include <data_caging_path_literals.hrh> // KDC_PROGRAMS_DIR
#include "MuiuMsgEditorLauncher.h"
#include "MuiuMsgEmbeddedEditorWatchingOperation.h"
#include "MuiuMsgEditorServerWatchingOperation.h"
#include "MuiuMsgEditorService.h"


// CONSTANTS
const TInt KemptyParamPackBuffSize = 1;
 
// Must be higher than the priority of CAknShutter ("aknshut.cpp")
const TInt KPriorityOfWatchingOperation = CActive::EPriorityStandard;

_LIT( KRomDriveLetter, "z:" );



// ============================ MEMBER FUNCTIONS ===============================



//////////////////////////////////////////////////////////////////////
//  MsgEditorLauncher                                               //
//////////////////////////////////////////////////////////////////////



// ---------------------------------------------------------
// MsgEditorLauncher::LaunchEditorApplicationL
// ---------------------------------------------------------
//
EXPORT_C CMsvOperation* MsgEditorLauncher::LaunchEditorApplicationL(
    CMsvSession& aSession,
    TUid aMtmType,
    TRequestStatus& aStatus,
    const TEditorParameters& aParams,
    const TDesC& aAppName,
    const TDesC8* aAddtionalParams )
    {
    TBufC8<KemptyParamPackBuffSize> emptyParamPack;
    return MsgEditorLauncher::LaunchEditorApplicationL(
        aSession,
        aMtmType,
        aStatus,
        aParams,
        aAppName,
        emptyParamPack,
        aAddtionalParams );
    }


// ---------------------------------------------------------
// MsgEditorLauncher::LaunchEditorApplicationL
// ---------------------------------------------------------
//
EXPORT_C CMsvOperation* MsgEditorLauncher::LaunchEditorApplicationL(
    CMsvSession& aSession,
    TUid aMtmType,
    TRequestStatus& aStatus,
    const TEditorParameters& aParams,
    const TDesC& aAppName,
    const TDesC8& aProgressPack,
    const TDesC8* aAddtionalParams )
    {
    //  Work out how the editor needs to be launched, depending
    //  on the values within the TEditorParameters.
    //
    if( aParams.iFlags&EMsgLaunchEditorEmbedded )
        {
        //  NB. Additional parameters ignored for embedded editor
        //  NB2. Embedded launcher _always_ waits for closure of editor
        return MsgEditorLauncher::LaunchEditorApplicationEmbeddedL(
            aSession,
            aMtmType,
            aStatus,
            aParams,
            aAppName,
            aProgressPack );
        }
    else
        {
        return MsgEditorLauncher::LaunchEditorApplicationStandAloneL(
            aSession,
            aMtmType,
            aStatus,
            aParams,
            aAppName,
            aProgressPack,
            aAddtionalParams,
            ( aParams.iFlags&EMsgLaunchEditorThenWait ) );
        }
    }


// ---------------------------------------------------------
// MsgEditorLauncher::LaunchEditorApplicationStandAloneL
// ---------------------------------------------------------
//
CMsvOperation* MsgEditorLauncher::LaunchEditorApplicationStandAloneL(
    CMsvSession& aSession,
    TUid aMtmType,
    TRequestStatus& aStatus,
    const TEditorParameters& aParams,
    const TDesC& /*aAppName*/,
    const TDesC8& aProgressPack,
    const TDesC8* aAddtionalParams,
    TBool /*aLaunchThenWait*/ )
    {
	TUid uid = KNullUid;
   	uid = MsgEditorServiceResolver::DiscoverL( aParams, aMtmType );
    if( uid == KNullUid )
    	{
    	User::Leave( KErrNotSupported );
    	}

    TApaAppInfo appInfo;
    RApaLsSession lsSession;
    User::LeaveIfError( lsSession.Connect() );
    CleanupClosePushL( lsSession );
    User::LeaveIfError( lsSession.GetAppInfo( appInfo, uid ) );

    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
    cmdLine->SetExecutableNameL( appInfo.iFullName );
    cmdLine->SetCommandL( EApaCommandRun );

    TPckgC<TEditorParameters> paramPack( aParams );
    if ( aAddtionalParams && aAddtionalParams->Length() )
        {
        // ---
        HBufC8* buf = HBufC8::NewLC( paramPack.Length() + aAddtionalParams->Length() );
        *buf = paramPack;
        buf->Des().Append( *aAddtionalParams );
        cmdLine->SetTailEndL( *buf );
        CleanupStack::PopAndDestroy( buf );// buf
        }
    else
        {
        cmdLine->SetTailEndL( paramPack );
        }

    User::LeaveIfError( lsSession.StartApp( *cmdLine ) );

    CleanupStack::PopAndDestroy( cmdLine );
    CleanupStack::PopAndDestroy();  // lsSession

    return CMsvCompletedOperation::NewL(
	    aSession,
        aMtmType,
        aProgressPack,
        KMsvUnknownServiceIndexEntryId,
        aStatus );
    }

// ---------------------------------------------------------
// MsgEditorLauncher::LaunchEditorApplicationEmbeddedL
// ---------------------------------------------------------
//
CMsvOperation* MsgEditorLauncher::LaunchEditorApplicationEmbeddedL(
    CMsvSession& aSession,
    TUid aMtmType,
    TRequestStatus& aStatus,
    const TEditorParameters& aParams,
    const TDesC& /*aAppName*/,
    const TDesC8& /*aProgressPack*/ )
    {
    CMsgEditorServerWatchingOperation* watcher =
        CMsgEditorServerWatchingOperation::NewL( 
        aSession, KPriorityOfWatchingOperation,
        aStatus,  aMtmType, aParams );
    return watcher;
    }


//////////////////////////////////////////////////////////////////////////////
//  MsvUiEditorUtilities                                                    //
//////////////////////////////////////////////////////////////////////////////


// ---------------------------------------------------------
// MsvUiEditorUtilities::ResolveAppFileNameL
// ---------------------------------------------------------
//
EXPORT_C HBufC* MsvUiEditorUtilities::ResolveAppFileNameL( TUid aAppUid )
    {
    RApaLsSession appList;
    User::LeaveIfError( appList.Connect() );
    CleanupClosePushL( appList );
    TApaAppInfo info;
    User::LeaveIfError( appList.GetAppInfo( info, aAppUid ) );
    CleanupStack::PopAndDestroy();//applist
    return info.iFullName.AllocL();
    }


//////////////////////////////////////////////////////////////////////////////
//  MsgEditorServiceResolver                                                //
//////////////////////////////////////////////////////////////////////////////



// ---------------------------------------------------------
// MsgEditorServiceResolver::DiscoverL
// ---------------------------------------------------------
//
TUid MsgEditorServiceResolver::DiscoverL( const TEditorParameters aParams, TUid aMtmType )
	{
	// Let's first check if a special editor/viewer is set
    if( aParams.iSpecialAppId != KNullUid )
		{ // If set, simply return that appUid
		return aParams.iSpecialAppId;
		}
	
	CApaAppServiceInfoArray* services = NULL;	
	RApaLsSession ls;
	TUid appUid = KNullUid;  // Set the uid as NULL
	TUid tempUid = KNullUid; // Set the uid as NULL
	
	if( ls.Connect() == KErrNone )
		{
		CleanupClosePushL( ls );
		TInt ret = ls.GetServerApps( TUid::Uid( KMuiuMsgEditorServiceUid ) );
		
		if( ret != KErrNone )
			{
			CleanupStack::PopAndDestroy( );  // closes RApaLsSession		
			return appUid; // return NULL appUid
			}
		
		services = ls.GetServiceImplementationsLC( TUid::Uid( KMuiuMsgEditorServiceUid ) );			
		
		if( services )
			{
			TArray<TApaAppServiceInfo> s = services->Array( );
			
			TInt count = s.Count( );
			
			if( count != 0)
				{	// 
				for( TInt i = 0; i<count; i++ )
					{
					if( MsgEditorServiceResolver::TryMatchL( aParams, aMtmType, s[i], tempUid ) )
						{
						// Match found.
						appUid = tempUid;
						// Check whether the match is from ROM drive
						TApaAppInfo appInfo;
						if ( ls.GetAppInfo( appInfo, appUid ) == KErrNone )
							{
							TParsePtrC parse( appInfo.iFullName );
							if ( parse.Drive().CompareF( KRomDriveLetter ) == KErrNone )
								{
								// Match found from ROM. Select this appUid.
								break;
								}
							}
						// Match not from ROM. Continue discovery. This match will
						// be selected if no others found later.
						}
					}
				}		
			}
		CleanupStack::PopAndDestroy( 2 );  // destroys services + closes RApaLsSession
		}
	return appUid;
	}


// ---------------------------------------------------------
// MsgEditorServiceResolver::TryMatchL
// ---------------------------------------------------------
//
TBool MsgEditorServiceResolver::TryMatchL( const TEditorParameters aParams, 
                                           TUid aMtmType, 
                                           const TApaAppServiceInfo aInfo, 
                                           TUid& aAppUid )
	{
	TBool useViewer = EFalse;

	if( ( aParams.iFlags & EMsgReadOnly ) || ( aParams.iFlags & EMsgPreviewMessage ) )
		{ // If readonly or preview flag is set -> look for "Viewer" service type
		useViewer = ETrue;
		}
	
	RResourceReader r;
	r.OpenLC( aInfo.OpaqueData() );
	
	TUid mtm = TUid::Uid( r.ReadInt32L( ) ); // read the mtm
	TInt serviceType = r.ReadInt16L( ); // the service type (view, edit)

    CleanupStack::PopAndDestroy( );				
	
	if( aMtmType == mtm && useViewer && serviceType == KMsgServiceView )
		{
		aAppUid.iUid = aInfo.Uid( ).iUid;
		return ETrue;
		}
	else  if( aMtmType == mtm && !useViewer && serviceType == KMsgServiceEdit )
		{
		aAppUid.iUid = aInfo.Uid( ).iUid;
		return ETrue;			
		}
	
	return EFalse;	
	}



// End of file
