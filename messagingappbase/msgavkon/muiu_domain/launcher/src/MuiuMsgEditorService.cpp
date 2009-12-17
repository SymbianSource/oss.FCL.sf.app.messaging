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
*     Client side message editor/viewer service implementations
*
*/



#include <bldvariant.hrh>
#include <e32std.h>    // KNullUid
#include <apgcli.h>
#include <eikenv.h>
#include <eikappui.h>
#include <AknServerApp.h>
#include <barsread2.h>
#include "MuiuMsgEditorServiceSession.h"
#include "MuiuMsgEditorService.h"
#include "MuiuMsgEditorLauncher.h"


// CONSTANTS
_LIT( KRomDriveLetter, "z:" );


// ---------------------------------------------------------
// RMuiuMsgEditorService::OpenEntryL
// ---------------------------------------------------------
//
void RMuiuMsgEditorService::OpenEntryL( const TEditorParameters aParams )
	{
	TInt err = 
		SendReceive(
			EMuiuMsgEditorServiceCmdOpen, 
			TIpcArgs( 	aParams.iId, 
						aParams.iFlags, 
						aParams.iDestinationFolderId, 
						aParams.iPartList ) );
	User::LeaveIfError(err);
	}

// ---------------------------------------------------------
// RMuiuMsgEditorService::ServiceUid
// ---------------------------------------------------------
//
TUid RMuiuMsgEditorService::ServiceUid() const
	{
	return TUid::Uid( KMuiuMsgEditorServiceUid );
	}

////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------
// CMuiuMsgEditorService::CMuiuMsgEditorService
// ---------------------------------------------------------
//
CMuiuMsgEditorService::CMuiuMsgEditorService( TUid aMtm ) :
	iMtm( aMtm )
	{
	}

// ---------------------------------------------------------
// CMuiuMsgEditorService::~CMuiuMsgEditorService
// ---------------------------------------------------------
//
CMuiuMsgEditorService::~CMuiuMsgEditorService( )
	{
	delete iMonitor;
	iService.Close();
	}

// ---------------------------------------------------------
// CMuiuMsgEditorService::NewL
// ---------------------------------------------------------
//
CMuiuMsgEditorService* CMuiuMsgEditorService::NewL( const TEditorParameters aParams, 
                                                    TUid aMtm, 
                                                    MAknServerAppExitObserver* aObserver)
    {
	CMuiuMsgEditorService* self = new(ELeave) CMuiuMsgEditorService( aMtm );
	CleanupStack::PushL(self);
    self->ConstructL( aParams, aObserver );
	CleanupStack::Pop(self);
	return self;
    }

// ---------------------------------------------------------
// CMuiuMsgEditorService::ConstructL
// ---------------------------------------------------------
//
void CMuiuMsgEditorService::ConstructL( const TEditorParameters aParams, 
                                        MAknServerAppExitObserver* aObserver)
    {
    const TUid uid = DiscoverL( aParams );
    if( uid != KNullUid )
        {
        iService.ConnectChainedAppL( uid );
        iMonitor = CApaServerAppExitMonitor::NewL( iService, 
                                                   *aObserver, 
                                                   CActive::EPriorityStandard );
        iService.OpenEntryL( aParams );
        }
    else
        {
        //Right server application not found
        User::Leave( KErrNotSupported );
        }
    }

// ---------------------------------------------------------
// CMuiuMsgEditorService::DiscoverL
// ---------------------------------------------------------
//
TUid CMuiuMsgEditorService::DiscoverL( const TEditorParameters aParams )
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
					if( TryMatchL( aParams, s[i], tempUid ) )
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
// CMuiuMsgEditorService::TryMatchL
// ---------------------------------------------------------
//
TBool CMuiuMsgEditorService::TryMatchL( const TEditorParameters aParams, 
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
	
	if( iMtm == mtm && useViewer && serviceType == KMsgServiceView )
		{
		aAppUid.iUid = aInfo.Uid( ).iUid;
		return ETrue;
		}
	else  if( iMtm == mtm && !useViewer && serviceType == KMsgServiceEdit )
		{
		aAppUid.iUid = aInfo.Uid( ).iUid;
		return ETrue;			
		}
	
	return EFalse;	
	}


// End of file

