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
*    CMsgConfirmSendQuery implementation file
*
*/



#include <e32base.h>
#include <bautils.h>
#include <s32mem.h>

#include <data_caging_path_literals.hrh>

#include <aknmessagequerydialog.h>

#include <tmsvpackednotifierrequest.h>

// SIS registry
#include <swi/sisregistrysession.h>
#include <swi/sisregistrypackage.h>

#include <mtclreg.h>
#include <msvreg.h>

#include <StringLoader.h>

#include <avkon.rsg>
#include <MsgNotifiers.rsg>

#include "MsgConfirmSendNotifier.h"
#include "MsgConfirmSendQuery.h"

// LOCAL CONSTANTS AND MACROS

_LIT( KMsgNotifiersResourceFile, "MsgNotifiers.rsc" );

// ---------------------------------------------------------
// CMsgConfirmSendQuery::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgConfirmSendQuery* CMsgConfirmSendQuery::NewL( CMsgConfirmSendNotifier& aNotifier )
    {
    CMsgConfirmSendQuery* self = new ( ELeave ) CMsgConfirmSendQuery( aNotifier );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CMsgConfirmSendQuery::ConstructL
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgConfirmSendQuery::ConstructL()
    {
    iCoe = CEikonEnv::Static();
    if ( !iCoe )
        {
        User::Leave( KErrGeneral );
        }

    TFileName resourceFile;
    TParse parse;
    parse.Set( KMsgNotifiersResourceFile, &KDC_RESOURCE_FILES_DIR, NULL );  
    resourceFile = parse.FullName();
    
    BaflUtils::NearestLanguageFile( iCoe->FsSession(), resourceFile );
    iResourceFileOffset = iCoe->AddResourceFileL( resourceFile );

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMsgConfirmSendQuery::CMsgConfirmSendQuery
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgConfirmSendQuery::CMsgConfirmSendQuery( CMsgConfirmSendNotifier& aNotifier ) 
    : CActive( CActive::EPriorityStandard ),
    iNotifier( aNotifier )
    {
    }

// ---------------------------------------------------------
// CMsgConfirmSendQuery::~CMsgConfirmSendQuery
//
// Destructor
// ---------------------------------------------------------
//
CMsgConfirmSendQuery::~CMsgConfirmSendQuery()
    {
    Cancel();
    if ( iCoe && iResourceFileOffset )
        {
        iCoe->DeleteResourceFile( iResourceFileOffset );
        }
    delete iAppName;
    delete iMtmName;
    delete iSelection;
    }

// ---------------------------------------------------------
// CMsgConfirmSendQuery::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgConfirmSendQuery::RunL() 
    {
    // "Cancel" returns "0"
    TInt ret( 0 );
    
    HBufC* label = CreateMessageTextLC();
    CAknMessageQueryDialog* query = CAknMessageQueryDialog::NewL( *label );
    ret = query->ExecuteLD( R_CONFIRM_QUERY );
    CleanupStack::PopAndDestroy( label );
    
    iNotifier.HandleActiveEvent( ret );
    }

// ---------------------------------------------------------
// CMsgConfirmSendQuery::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgConfirmSendQuery::DoCancel()
    {
    }

// ---------------------------------------------------------
// CMsgConfirmSendQuery::Start
//
// Starts the active object
// ---------------------------------------------------------
//
void CMsgConfirmSendQuery::ShowQueryL( const TDesC8& aBuffer, TBool aSendAs )
    {
    if ( IsActive() )
        {
        Cancel();
        }
        
    //delete iBuf;
    //iBuf = NULL;
    //iBuf = aBuffer.AllocL();
    //iClient.ShowQuery( *iBuf, iStatus );
    
    if ( aSendAs )
        {
        ResolveSendAsConfirmDetailsL( aBuffer );
        }
    else
        {
        ResolveSendUiConfirmDetailsL( aBuffer );
        }

    // Complete self:
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive();
    User::RequestComplete( pStatus, KErrNone );
    }


// ---------------------------------------------------------
// CMsgConfirmSendSession::ResolveSendAsConfirmDetailsL
// ---------------------------------------------------------
//
void CMsgConfirmSendQuery::ResolveSendAsConfirmDetailsL( const TDesC8& aBuffer )
    {
    TSecurityInfo securityInfo;
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection();
    CleanupStack::PushL( selection );
	TMsvPackedNotifierRequest::UnpackL( aBuffer, *selection, securityInfo );
    
    TInt count = selection->Count();
    if ( count == 0 )
        {
        User::Leave( KErrArgument );
        }

	CDummyObserver* dummyObs = new ( ELeave ) CDummyObserver;
	CleanupStack::PushL( dummyObs );
	CMsvSession* session = CMsvSession::OpenSyncL( *dummyObs );
	CleanupStack::PushL( session );

    TMsvId dummy;
    TMsvEntry entry;
    User::LeaveIfError( session->GetEntry( selection->At( 0 ), dummy, entry ) );
    
    iMultipleMessages = ( count > 1 );
    iMultipleRecipients = entry.MultipleRecipients();

    // Assume all entries have same MTM.
    NameFromMtmL( session, entry.iMtm );
    NameFromSecurityInfoL( securityInfo );
    
    CleanupStack::PopAndDestroy( 3, selection ); // selection, dummyObs, session
    }

// ---------------------------------------------------------
// CMsgConfirmSendSession::ResolveSendUiConfirmDetailsL
// ---------------------------------------------------------
//
void CMsgConfirmSendQuery::ResolveSendUiConfirmDetailsL( const TDesC8& aBuffer )
    {
    iMultipleMessages = EFalse;
    iMultipleRecipients = EFalse;
    
    RDesReadStream stream( aBuffer );
    delete iMtmName;
    iMtmName = NULL;
    TInt length = stream.ReadInt32L();
    if ( length )
        {
        iMtmName = HBufC::NewL( stream, length );
        }
    else
        {
        iMtmName = StringLoader::LoadL( R_CONFIRM_UNKNOWN_MTM, iCoe );
        }
    TSecurityInfo securityInfo;
    securityInfo.iSecureId = stream.ReadUint32L();
    NameFromSecurityInfoL( securityInfo );
    }

// ---------------------------------------------------------
// CMsgConfirmSendSession::NameFromSecurityInfoL
// ---------------------------------------------------------
//
void CMsgConfirmSendQuery::NameFromSecurityInfoL( TSecurityInfo& securityInfo )
    {
    delete iAppName;
    iAppName = NULL;
    Swi::RSisRegistrySession sisSession;
    if ( sisSession.Connect() == KErrNone )
        {
        Swi::CSisRegistryPackage* sisRegistry = NULL;
        TRAPD( err, sisRegistry = sisSession.SidToPackageL( securityInfo.iSecureId ) );
        if ( !err )
            {
            iAppName = sisRegistry->Name().AllocL();
            }
        delete sisRegistry;
        sisSession.Close();
        }
    if ( !iAppName || !iAppName->Length() )
        {
        iAppName = StringLoader::LoadL( R_CONFIRM_UNKNOWN_APP, iCoe );
        }
    }

// ---------------------------------------------------------
// CMsgConfirmSendSession::NameFromMtmL
// ---------------------------------------------------------
//
void CMsgConfirmSendQuery::NameFromMtmL( CMsvSession* aSession, TUid aMtmTypeUid )
    {
    delete iMtmName;
    iMtmName = NULL;
	//HBufC* name( NULL );
	// Create a client registry
	CClientMtmRegistry* clientReg = CClientMtmRegistry::NewL( *aSession );
	CleanupStack::PushL( clientReg );

    // Resolve human readable name for the MTM
	if ( clientReg->IsPresent( aMtmTypeUid ) )
	    {
    	iMtmName = clientReg->RegisteredMtmDllInfo( aMtmTypeUid ).HumanReadableName().AllocL();
	    }
    if ( !iMtmName || !iMtmName->Length() )
        {
        iMtmName = StringLoader::LoadL( R_CONFIRM_UNKNOWN_MTM, iCoe );
        }
    CleanupStack::PopAndDestroy( clientReg );
    }

// ---------------------------------------------------------
// CMsgConfirmSendQuery::CreateMessageTextLC
// ---------------------------------------------------------
//
HBufC* CMsgConfirmSendQuery::CreateMessageTextLC()
    {
    TInt resourceId = R_CONFIRM_SEND_MANY_MSG;
    if ( !iMultipleMessages )
        {
        resourceId = iMultipleRecipients
            ? R_CONFIRM_SEND_MANY_RECIP
            : R_CONFIRM_SEND_ONE;
        }

    CPtrCArray* array = new ( ELeave ) CPtrCArray( 2 );
    CleanupStack::PushL( array );
    
    array->AppendL( *iAppName );
    array->AppendL( *iMtmName );
    
    HBufC* label = StringLoader::LoadL(
        resourceId,
        *array,
        iCoe );
    
    CleanupStack::PopAndDestroy( array );
    CleanupStack::PushL( label );
    return label;
    }


// End of File
