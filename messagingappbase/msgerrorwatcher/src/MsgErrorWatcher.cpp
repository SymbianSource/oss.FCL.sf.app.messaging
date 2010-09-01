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
*     CMsgErrorWatcher implementation file
*
*/



// INCLUDE FILES
#include <e32base.h>

#include <msvapi.h>
#include <msvids.h>          //Msv index entry ID's
#include <msvuids.h>
#include <mmscmds.h>         //EMmsScheduledReceiveForced
#include <mmsconst.h>        //Notification folder
#include <SenduiMtmUids.h>   //MTM Uids
#include <mtclreg.h>         //ClientMtmRegistry
#include <watcher.h>
#include <mmserrors.h>
#include <mmsclient.h>
#include <mmssettings.h>
#include <MsgErrorWatcherData.rsg>   // resouce identifiers
#include <avkon.rsg>            // resouce identifiers
#include <data_caging_path_literals.hrh> 

#include <avkon.hrh>
#include <AknGlobalNote.h>
#include <AknPhoneNumberGrouping.h>
#include <AknUtils.h>

#include <stringresourcereader.h>
#include <StringLoader.h>
#include <textresolver.h>
#include <in_iface.h>            // KErrIfAuthenticationFailure
#include <etelpckt.h>            // KErrGprsInsufficientResources, etc.
#include <exterror.h>            // KErrGsmMMServiceOptionTemporaryOutOfOrder
#include <gsmerror.h>

#include <centralrepository.h>    // link against centralrepository.lib
#include <messaginginternalcrkeys.h> // for Central Repository keys

#include <messagingvariant.hrh>  // Variation

#include "MsgErrorWatcher.h"
#include "MsgErrorWatcher.hrh"
#include "MsgSentItemsObserver.h"
#include "MsgErrorCommDbObserver.h"
#include "MsgCenRepObserver.h"
#include "MsgErrorConnectionObserver.h"
#include "MsgErrorDiskSpaceObserver.h"
#include "MsgErrorSmsDiskSpaceObserver.h"
#include "MsgErrorExtSmsDiskSpaceObserver.h"
#include "MsgErrorRoamingObserver.h"
#include "MsgErrorGlobalQuery.h"
#include "MsgErrorDisconnectDlg.h"
#include "MsgErrorStartupObserver.h"
#include "MsgLogsObserver.h"
#include "MsgGarbageCollection.h"

#include "MsgErrorWatcherLogging.h"

#include <implementationproxy.h>

// LOCAL CONSTANTS AND MACROS
_LIT( KMsgErrorWatcherResourceFileName, "msgerrorwatcherdata.rsc" );
const TUint KArrayGranularity = 5;
const TInt KDelayBetweenNotes = 2000000; //2 seconds
const TInt KDelayAfterDisconnect = 3000000; //3 seconds
const TInt KConnectionRetries = 3;
const TInt KIntMaxLength = 10; //2~32 =~ 4000000000
const TUint KInitialDelayBetweenSessionConnectRetries = 5000000; //five seconds
const TUint KMaxTimerRetries = 50;
//Total delay between first and last retry is
//( A * (B^2 + B) ) / 2
// - where A is KInitialDelayBetweenSessionConnectRetries
// -   and B is KMaxTimerRetries
//If A = 5 seconds and B = 50 times last retry is made
//after about 106 minutes from the first one.
const TMsvId KWatcherInboxFolderId = KMsvGlobalInBoxIndexEntryIdValue;
const TMsvId KWatcherOutboxFolderId = KMsvGlobalOutBoxIndexEntryIdValue;
const TUid KMessageEntryUid = { KUidMsvMessageEntryValue };
// This is more than what we need, but we are cautious
const TInt KExtraSpaceForDirectionalityMarkers = 6;
// ==================== LOCAL FUNCTIONS ====================


const TImplementationProxy ImplementationTable[] = 
	{
    IMPLEMENTATION_PROXY_ENTRY( 0x100059CF, CMsgErrorWatcher::NewL ) //lint !e611
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	return ImplementationTable;
	}

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgErrorWatcher::CMsgErrorWatcher
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorWatcher::CMsgErrorWatcher( RFs& aFs )
    : CActive( EPriorityStandard ),
    iMmsServiceId( KMsvNullIndexEntryId ),
    iNotificationFolderId( KMsvNullIndexEntryId ),
    iCurrentEntryId ( KMsvNullIndexEntryId ),
    iFs( aFs ) 
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::ConstructL
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgErrorWatcher::ConstructL()
    {
MEWLOGGER_ENTERFN( "ConstructL" );
    User::LeaveIfError( iTimer.CreateLocal() );

    TParse fileParse;
    fileParse.Set( KMsgErrorWatcherResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName resource( fileParse.FullName() );
    iResourceReader = CStringResourceReader::NewL( resource );

    TInt features = 0;   
    CRepository* repository = CRepository::NewL( KCRUidMuiuVariation );
    repository->Get( KMuiuMmsFeatures, features );
    delete repository;
    repository = NULL;
    if ( features & KMmsFeatureIdDelayDisconnectDialog )
        {
        iWatcherFlags |= EReceivingDisconnectDelay;
        }

    StartWatcherL();
MEWLOGGER_LEAVEFN( "ConstructL" );
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorWatcher* CMsgErrorWatcher::NewL( TAny* aWatcherParams )
    {
	TWatcherParams* params = reinterpret_cast<TWatcherParams*>( aWatcherParams );
    CMsgErrorWatcher* self = new ( ELeave ) CMsgErrorWatcher( params->iFs );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// ---------------------------------------------------------
// CMsgErrorWatcher::~CMsgErrorWatcher
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorWatcher::~CMsgErrorWatcher()
    {
    StopWatcher();
    delete iResourceReader;
    iTimer.Close();
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::StartWatcherL
//
// Does the actual construction of the watcher.
// ---------------------------------------------------------
//
void CMsgErrorWatcher::StartWatcherL()
    {
    MEWLOGGER_ENTERFN( "StartWatcherL" );
    iSession = CMsvSession::OpenSyncL( *this );
    iMmsReceiveErrorMessages = new ( ELeave ) CMsvEntrySelection;
    iMmsSendErrorMessages = new ( ELeave ) CMsvEntrySelection;
    iErrorMessages = new ( ELeave ) CMsvEntrySelection;
    iErrorMsgNotes = new ( ELeave ) CMsvEntrySelection;
    iNoteIds = new ( ELeave ) CArrayFixFlat<TInt>( KArrayGranularity );

    // Get message server session
    iClientMtmRegistry = CClientMtmRegistry::NewL( *iSession );
    TRAPD( err,
        {
        iMmsClient = static_cast<CMmsClientMtm*>
            ( iClientMtmRegistry->NewMtmL( KUidMsgTypeMultimedia ) );
        } );
    if ( err == KErrNotFound )
        {
        MEWLOGGER_WRITE( "MmsClientMtm not found!" );
        // Make sure iMmsClient is NULL - just in case.
        delete iMmsClient;
        iMmsClient = NULL;
        }
    else
        {
        User::LeaveIfError( err );
        }

    if ( iMmsClient )
        {
        // We're interested in roaming events only if MMS is enabled.
        iRoamingObserver = CMsgErrorRoamingObserver::NewL( this ); 
        }
    if ( iClientMtmRegistry->IsPresent( KSenduiMtmPushMtmUid ) )
        {
        iGarbageCollection = CMsgGarbageCollection::NewL( *iClientMtmRegistry );
        }

    iSentItemsObserver = CMsgSentItemsObserver::NewL( this, iSession );
    iStartupObserver = CMsgErrorStartupObserver::NewL( this );
    
    if(!( iWatcherFlags & EWatcherRunning))
        {
        //if not yet created
        iSmsDiskSpaceObserver = CMsgErrorSmsDiskSpaceObserver::NewL( this );
        iSmsExtDiskSpaceObserver = CMsgErrorExtSmsDiskSpaceObserver::NewL( this );
        }
    // If log database is corrupted, logs observer start leaves.
    // We ignore the leave, we can function otherwise, but we get no log events
    TRAP_IGNORE( iLogsObserver = CMsgLogsObserver::NewL( this, iFs ) );

    iWatcherFlags |= EWatcherRunning;

    if ( GetMmsServiceL() )
        {
        CheckMmsReceivingModeL();
        }
    
    InitMessageArraysL();
    // Startup successful if we got this far. Reset retry counter.
    iTimerRetries = 0;
    MEWLOGGER_LEAVEFN( "StartWatcherL" );
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::StopWatcher
//
// Stops (and destructs) the watcher.
// ---------------------------------------------------------
//
void CMsgErrorWatcher::StopWatcher()
    {
    MEWLOGGER_ENTERFN( "StopWatcher" );
    iWatcherFlags &= ~EWatcherRunning;
    iWatcherFlags &= ~ENoAPErrorPending;
    iMmsServiceId = KMsvNullIndexEntryId;
    iNotificationFolderId = KMsvNullIndexEntryId;

    Cancel();
    delete iGarbageCollection;
    iGarbageCollection = NULL;
    delete iLogsObserver;
    iLogsObserver = NULL;
    delete iCommDbObserver;
    iCommDbObserver = NULL;
    delete iCenRepObserver;
    iCenRepObserver = NULL;
    delete iConnectionObserver;
    iConnectionObserver = NULL;
    delete iDiskSpaceObserver;
    iDiskSpaceObserver = NULL;
    delete iSmsDiskSpaceObserver;
    iSmsDiskSpaceObserver = NULL;
    delete iSmsExtDiskSpaceObserver;
    iSmsExtDiskSpaceObserver = NULL;    
    delete iRoamingObserver;
    iRoamingObserver = NULL;
    delete iMmsClient;
    iMmsClient = NULL;
    delete iClientMtmRegistry;
    iClientMtmRegistry = NULL;
    delete iOperation;
    iOperation = NULL;
    delete iSentItemsObserver;
    iSentItemsObserver = NULL;
    delete iStartupObserver;
    iStartupObserver = NULL;
    delete iSession;
    iSession = NULL;
    delete iMmsReceiveErrorMessages;
    iMmsReceiveErrorMessages = NULL;
    delete iMmsSendErrorMessages;
    iMmsSendErrorMessages = NULL;
    delete iNoteIds;
    iNoteIds = NULL;

    delete iErrorMsgText;
    iErrorMsgText = NULL;
    delete iErrorMsgDetails;
    iErrorMsgDetails = NULL;
    delete iErrorQuery;
    iErrorQuery = NULL;
    delete iDisconnectQuery;
    iDisconnectQuery = NULL;
    delete iDisconnectDlg;
    iDisconnectDlg = NULL;
    delete iErrorMsgNotes;
    iErrorMsgNotes = NULL;

    delete iErrorMessages;
    iErrorMessages = NULL;
    
    MEWLOGGER_LEAVEFN( "StopWatcher" );
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::StartRestartTimer
//
// Start session reconnect timer.
// ---------------------------------------------------------
//
void CMsgErrorWatcher::StartRestartTimer()
    {
    if ( !IsActive() ) 
        {
    MEWLOGGER_WRITE( "Starting timer" );
        iStatus = KRequestPending;
        iRequestType = EMsgRequestStartingUp;
        iTimerRetries++;
        //The interval between retries comes longer every time
        iTimer.After( iStatus,
            iTimerRetries * KInitialDelayBetweenSessionConnectRetries );
        SetActive();
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::InitMessageArraysL
//
// Initialise arrays
// ---------------------------------------------------------
//
void CMsgErrorWatcher::InitMessageArraysL()
    {
    MEWLOGGER_ENTERFN( "InitMessageArraysL" );
    CMsvEntry* outbox = iSession->GetEntryL( KWatcherOutboxFolderId );
    CleanupStack::PushL( outbox );
    CMsvEntrySelection* outboxMessages =
        outbox->ChildrenWithTypeL( KMessageEntryUid );
    CleanupStack::PushL( outboxMessages );
    TInt count = outboxMessages->Count();
    while ( count-- )
        {
        TMsvId dummy;
        TMsvEntry entry;
        TInt error = iSession->GetEntry(
            outboxMessages->At( count ), dummy, entry );

        TUid mtm = entry.iMtm;
        if ( !error &&
            ( mtm == KSenduiMtmSmsUid ||
            mtm == KSenduiMtmMmsUid  ||
            mtm == KSenduiMtmSmtpUid ||
            mtm == KUidMsgMMSNotification ) )
            {
            iErrorMessages->AppendL( entry.Id() );
            MEWLOGGER_ENTERFN( "Send msg added" );
            }
        }
    CleanupStack::PopAndDestroy( outboxMessages );  
    CleanupStack::PopAndDestroy( outbox );
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::ResetWatcher
//
// Resets watcher.
// ---------------------------------------------------------
//
void CMsgErrorWatcher::ResetWatcher()
    {
    MEWLOGGER_ENTERFN( "ResetWatcher" );

    iWatcherFlags &= ~ENoAPErrorPending;
    
    //Drop all observers
    delete iCommDbObserver;
    iCommDbObserver = NULL;
    delete iCenRepObserver;
    iCenRepObserver = NULL;
    delete iConnectionObserver;
    iConnectionObserver = NULL;
    delete iDiskSpaceObserver;
    iDiskSpaceObserver = NULL;
	delete iSmsDiskSpaceObserver;
    iSmsDiskSpaceObserver = NULL;
    delete iSmsExtDiskSpaceObserver;
    iSmsExtDiskSpaceObserver = NULL;    
    //Reset disk space errors
    iDiskSpaceErrors = 0;
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::GetMmsServiceL
//
// Retrieves MMS service id from MsgStore
// ---------------------------------------------------------
//
TBool CMsgErrorWatcher::GetMmsServiceL()
    {
    if ( !iMmsClient )
        {
        return EFalse;
        }
    if ( iMmsServiceId == KMsvNullIndexEntryId )
        {
        MEWLOGGER_WRITE( "Looking for MMS service" );
        iMmsClient->RestoreSettingsL();
        iMmsServiceId = iMmsClient->MmsSettings().Service();
        iNotificationFolderId = iMmsClient->MmsSettings().NotificationFolder();
        iMaxReceiveSize = iMmsClient->MmsSettings().MaximumReceiveSize();
        }
    return ( iMmsServiceId != KMsvNullIndexEntryId );
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::StartMmsFetchL
//
// Initiates MMS fetch
// ---------------------------------------------------------
//
void CMsgErrorWatcher::StartMmsFetchL()
    {
    MEWLOGGER_ENTERFN( "StartMmsFetchL" );
    if ( !IsActive() && iMmsReceiveErrorMessages->Count() )
        {
        MEWLOGGER_WRITEF( _L("Fetching %d message(s)"),
            iMmsReceiveErrorMessages->Count() );
        CancelNotesL();
        TCommandParameters parameters; // initialized to zero
        TCommandParametersBuf paramPack( parameters );

        //Add service entry as the first entry in the array
        iMmsReceiveErrorMessages->InsertL( 0, iMmsServiceId );

        // First remove existing schedules:
        MEWLOGGER_WRITE( "Deleting old schedules" );
        // Calling synchronous TransferCommandL method
        const TInt KBufSize = 256;
        TBuf8<KBufSize> dummy;
        iSession->TransferCommandL( *iMmsReceiveErrorMessages,
            EMmsDeleteSchedule,
            paramPack,
            dummy );
        MEWLOGGER_WRITE( "Old schedules deleted!" );

        // Then reschedule:
        iStatus = KRequestPending;
        iRequestType = EMsgRequestFetching;
        delete iOperation;
        iOperation = NULL;
        MEWLOGGER_WRITE( "Request fetch" );
        iOperation = iSession->TransferCommandL( *iMmsReceiveErrorMessages,
            EMmsScheduledReceive,
            paramPack,
            iStatus );
        SetActive();
        MEWLOGGER_WRITE( "Reset array" );
        iMmsReceiveErrorMessages->Reset();
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::StartMmsSendL
//
// Initiates MMS fetch
// ---------------------------------------------------------
//
void CMsgErrorWatcher::StartMmsSendL()
    {
    MEWLOGGER_ENTERFN( "StartMmsSendL" );
    if ( !IsActive() && iMmsSendErrorMessages->Count() )
        {
        MEWLOGGER_WRITEF( _L("Sending %d message(s)"),
            iMmsSendErrorMessages->Count() );
        CancelNotesL();
        TCommandParameters parameters; // initialized to zero
        TCommandParametersBuf paramPack( parameters );

        //Add service entry as the first entry in the array
        iMmsSendErrorMessages->InsertL( 0, iMmsServiceId );

        iStatus = KRequestPending;
        iRequestType = EMsgRequestSending;
        delete iOperation;
        iOperation = NULL;
        MEWLOGGER_WRITE( "Request send" );
        iOperation = iSession->TransferCommandL( *iMmsSendErrorMessages,
            EMmsScheduledSend,
            paramPack,
            iStatus );
        SetActive();
        MEWLOGGER_WRITE( "Reset array" );
        iMmsSendErrorMessages->Reset();
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::CheckMmsReceivingModeL
//
// Checks MMS receiving mode
// ---------------------------------------------------------
//
void CMsgErrorWatcher::CheckMmsReceivingModeL()
    {
    if ( !( iWatcherFlags & EStartupReady ) || !GetMmsServiceL() )
        {
        return;
        }
    TBool validAP = ValidateMmsServiceL(); //Refreshes MMS settings
    MEWLOGGER_WRITEF( _L("ReceivingModeHome: %d"), iMmsClient->MmsSettings().ReceivingModeHome() );
    MEWLOGGER_WRITEF( _L("ReceivingModeForeign: %d"), iMmsClient->MmsSettings().ReceivingModeForeign() );

    TInt fetchHome = iMmsClient->MmsSettings().ReceivingModeHome();
    TInt fetchRoam = iMmsClient->MmsSettings().ReceivingModeForeign();

    if ( validAP &&
        fetchRoam == EMmsReceivingReject &&
        fetchHome != EMmsReceivingReject )
        {
        MEWLOGGER_WRITE( "CheckMmsReceivingModeL, ShowNote flag enabled" );
        iWatcherFlags |= EShowRoamingNote;
        }
    else
        {
        //Reset roaming note flag
        MEWLOGGER_WRITE( "CheckMmsReceivingModeL, ShowNote flag disabled" );
        iWatcherFlags &= ~EShowRoamingNote;
        iWatcherFlags &= ~ERoamingNoteShown;
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::ValidateMmsServiceL
//
// Validates MMS service
// ---------------------------------------------------------
//
TBool CMsgErrorWatcher::ValidateMmsServiceL()
    {
    if ( !GetMmsServiceL() )
        {
        return EFalse;
        }
    iMmsClient->RestoreSettingsL(); //Refreshes MMS settings
    MEWLOGGER_WRITEF( _L("ValidateService: %d"), iMmsClient->ValidateService( iMmsServiceId ) );

    TInt errorCode = iMmsClient->ValidateService( iMmsServiceId );
    
    //TODO: check if the following is needed at all.
    //Is ENoAPErrorPending needed and is it att all
    //possible to go into there?
    if ( iWatcherFlags & ENoAPErrorPending &&
        ( errorCode == KMmsErrorNoURI1 ||
          errorCode == KMmsErrorAP1Invalid ) )
        {
        iWatcherFlags &= ~ENoAPErrorPending;
        ShowGlobalNoteL( EInvalidAccessPointNote );
        }
    return ( errorCode == KErrNone );
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::ShowGlobalNoteL
//
// Shows AVKON global note
// ---------------------------------------------------------
//
void CMsgErrorWatcher::ShowGlobalNoteL( TMsgErrorNoteIds aNoteId )
    {
    if ( !( iWatcherFlags & EStartupReady ) )
        {
        return;
        }
    MEWLOGGER_WRITEF( _L("ShowGlobalNoteL, id: %d"), aNoteId );

    TPtrC string;
    HBufC* text = NULL;
    TAknGlobalNoteType noteType = EAknGlobalErrorNote;
    TInt softkeys = R_AVKON_SOFTKEYS_OK_EMPTY;

    switch ( aNoteId )
        {
        case ENoAccesPointsNote:
            {
            string.Set( iResourceReader->ReadResourceString( R_MSG_ERRH_NO_AP ) );
            }
            break;
        case EInvalidAccessPointNote:
            {
            string.Set( iResourceReader->ReadResourceString( R_MSG_ERRH_INVALID_AP ) );            
            }
            break;
        case EDiskLowNote1:
            {
            string.Set( iResourceReader->ReadResourceString( R_MSG_ERRH_DISK_LOW_1 ) );           
            }
            break;
        case EDiskLowNoteN:
            {
            string.Set( iResourceReader->ReadResourceString( R_MSG_ERRH_DISK_LOW_N ) );          
            text = HBufC::NewLC( string.Length() + KIntMaxLength );
            TPtr tempPtr = text->Des();
            StringLoader::Format( tempPtr, string, -1, iDiskSpaceErrors );
            }
            break;
        case ESMSIncomingLowDiskSpace:
            {
            string.Set( iResourceReader->ReadResourceString( R_MEMLO_MEMORY_LOW_MESSAGES ) );    
            }
            break;
        case EMemoryLowNote:
            {
            string.Set( iResourceReader->ReadResourceString( R_MSG_ERRH_MEMORY_LOW ) );          
            }
            break;
        case ERoamingNote:
            {
            string.Set( iResourceReader->ReadResourceString( R_MSG_ERRH_ROAMING_REMINDER ) );            
            noteType = EAknGlobalConfirmationNote;
            }
            break;
        case ESendingMessageNote:
            {
            string.Set( iResourceReader->ReadResourceString( R_MSG_ERRH_SENDING_MESSAGE ) );           
            noteType = EAknGlobalInformationNote;
            softkeys = 0;
            break;
            }
        default:
            //Show nothing... 
            return;
        }
    if ( !text )
        {
        text = string.AllocLC();
        }

    CAknGlobalNote* note = CAknGlobalNote::NewLC();
    if ( softkeys )
        {
        // If softkeys are set the global note will be "waiting".
        // If softkeys are NOT set the global note will be "non-waiting".
        note->SetSoftkeys( softkeys );
        }
    TInt id = note->ShowNoteL( noteType, *text );
    CleanupStack::PopAndDestroy( note  );
    CleanupStack::PopAndDestroy( text );

    iNoteIds->AppendL( id );
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::CancelNotesL
//
// Cancels all AVKON global notes. 
// ---------------------------------------------------------
//
void CMsgErrorWatcher::CancelNotesL()
    {
    TInt count = iNoteIds->Count();
    if ( count )
        {
        CAknGlobalNote* note = CAknGlobalNote::NewLC();
        while ( count-- )
            {
            //Does nothing if note is already dismissed.
            note->CancelNoteL( iNoteIds->At( count ) );
            }
        CleanupStack::PopAndDestroy( note ); // note
        iNoteIds->Reset();
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::ShowErrorMessageQueryL
//
// Shows AVKON global query
// ---------------------------------------------------------
//
void CMsgErrorWatcher::ShowErrorMessageQueryL()
    {
    MEWLOGGER_ENTERFN( "ShowErrorMessageQueryL" );
    delete iErrorQuery;
    iErrorQuery = NULL;

    TMsvEntry entry;
    if ( GetNextErrorMsg( entry ) == KErrNone )
        {
        ResolveErrorTextL( entry );
        ResolveErrorDetailsL( entry );
        TInt softkeys = ( iErrorMsgDetails != 0 ) ?
            R_AVKON_SOFTKEYS_OK_DETAILS : 
            R_AVKON_SOFTKEYS_OK_EMPTY;
    
        MEWLOGGER_ENTERFN( "Displaying global query" );
        iErrorQuery = CMsgErrorGlobalQuery::NewL( this );
        iErrorQuery->ShowQueryL(
            *iErrorMsgText,
            softkeys,
            ESendFailureNote );
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::GetNextErrorMsg
// ---------------------------------------------------------
//
TInt CMsgErrorWatcher::GetNextErrorMsg( TMsvEntry& aEntry )
    {
    if ( iErrorMsgNotes->Count() &&
        iWatcherFlags & EStartupReady )
        {
        TMsvId dummy;
        TInt error = iSession->GetEntry( iErrorMsgNotes->At( 0 ), dummy, aEntry );
        iErrorMsgNotes->Delete( 0 );
        return error;
        }
    else
        {
        return KErrNotFound;
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::ResolveErrorTextL
// ---------------------------------------------------------
//
void CMsgErrorWatcher::ResolveErrorTextL( TMsvEntry& aEntry )
    {
    MEWLOGGER_ENTERFN( "ResolveErrorTextL" );
    delete iErrorMsgText;
    iErrorMsgText = NULL;

    HBufC* info = NULL;
    if ( aEntry.iDetails.Length() )
        {
        // Sender or recipient info
        info = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( aEntry.iDetails ); 
        CleanupStack::PushL( info ); 
        }
    
    TPtrC string;
    if ( aEntry.iMtm == KUidMsgMMSNotification )
        {
        if ( aEntry.Parent() == KWatcherInboxFolderId )
            {
            MEWLOGGER_ENTERFN( "Retrieval failure." );
            string.Set( iResourceReader->ReadResourceString( info
                ? R_MSG_ERRH_RETRIEVAL_ERROR_INFO
                : R_MSG_ERRH_RETRIEVAL_ERROR ) );
            }
        else
            {
            MEWLOGGER_ENTERFN( "Forward failure." );
            string.Set( iResourceReader->ReadResourceString( info
                ? R_MSG_ERRH_FORWARD_ERROR_INFO
                : R_MSG_ERRH_FORWARD_ERROR ) );
            }
        }
    else
        {
        MEWLOGGER_ENTERFN( "Send failure." );
        string.Set( iResourceReader->ReadResourceString( info
            ? R_MSG_ERRH_SEND_ERROR_INFO
            : R_MSG_ERRH_SEND_ERROR ) );
        }

    if ( info )
        {
        iErrorMsgText = HBufC::NewL( string.Length() + info->Length() ); 
        TPtr tempPtr = iErrorMsgText->Des(); 

        StringLoader::Format( tempPtr, string, -1, *info ); 
        CleanupStack::PopAndDestroy( info );
        }
    else
        {
        iErrorMsgText = string.AllocL();
        }
    MEWLOGGER_LEAVEFN( "ResolveErrorTextL" );
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::ResolveErrorDetailsL
// ---------------------------------------------------------
//
void CMsgErrorWatcher::ResolveErrorDetailsL( TMsvEntry& aEntry )
    {
    MEWLOGGER_ENTERFN( "ResolveErrorDetailsL" );
    delete iErrorMsgDetails;
    iErrorMsgDetails = NULL;

    TBool useResolver = EFalse;
    TInt errorId = aEntry.iError;

    if ( iMmsClient &&
        ( aEntry.iMtm == KSenduiMtmMmsUid || aEntry.iMtm == KUidMsgMMSNotification ) )
        {
        switch ( errorId )
            {
            case KMmsErrorStatusMessageAddressUnresolved:
            case KMmsErrorStatusTransientAddressUnresolved:
            case KMmsErrorStatusMessageNotFound:
            case KMmsErrorStatusTransientMessageNotFound:
            case KMmsErrorStatusNetworkProblem:
            case KMmsErrorStatusServiceDenied:
            case KMmsErrorStatusMessageFormatCorrupt:
            case KMmsErrorStatusContentNotAccepted:
            case KMmsErrorStatusReplyChargingLimitationsNotMet:
            case KMmsErrorStatusReplyChargingRequestNotAccepted:
            case KMmsErrorStatusReplyChargingForwardingDenied:
            case KMmsErrorStatusReplyChargingNotSupported:
            case KMmsErrorStatusTransientFailure:
            case KMmsErrorStatusUnspecified:
            case KMmsErrorStatusPermanentFailure:
            case KMmsErrorStatusUnsupportedMessage:
                {
                //See whether MMSC has delivered some
                //response text and use it if found.
                //Else use normal text resolver.
                iMmsClient->SwitchCurrentEntryL( aEntry.Id() );
                iMmsClient->LoadMessageL();
                TPtrC responseText = iMmsClient->ResponseText();
                if ( responseText.Length() )
                    {
                    MEWLOGGER_WRITEF( _L("Response text found: %d"), errorId );
                    iErrorMsgDetails = responseText.AllocL();
                    useResolver = EFalse;
                    }
                else
                    {
                    useResolver = ETrue;
                    }
                }
                break;
            case KErrCouldNotConnect:
                errorId = KMmsErrorCouldNotConnect;
                //lint -fallthrough
            default:
                useResolver = ETrue;
                break;
            }
        }
    else if ( aEntry.iMtm == KSenduiMtmSmsUid )
        {
        useResolver = ETrue;
        }
    else
        {
        useResolver = EFalse;
        }

    if ( useResolver )
        {
        TInt textId;
        TUint textFlags;
        MEWLOGGER_WRITEF( _L("Resolving error: %d"), errorId );
        CTextResolver* resolver = CTextResolver::NewLC();
        const TDesC& error = resolver->ResolveErrorString( errorId, textId, textFlags );
       
        if ( !( textFlags & EErrorResBlankErrorFlag ||
            textFlags & ETextResolverUnknownErrorFlag ) )
            {
            iErrorMsgDetails = error.AllocL();
            }
        CleanupStack::PopAndDestroy( resolver ); 
        }
    MEWLOGGER_LEAVEFN( "ResolveErrorDetailsL" );
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleNoAPErrorL
//
// Handles "no access point defined" error
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleNoAPErrorL( TMsvEntry& aEntry )
    { 
    MEWLOGGER_ENTERFN( "HandleNoAPErrorL" );
    iMmsReceiveErrorMessages->AppendL( aEntry.Id() );

    iWatcherFlags |= ENoAPErrorPending;

    ShowGlobalNoteL( ENoAccesPointsNote );
    if ( !iCenRepObserver )
        {
        MEWLOGGER_WRITE( "HandleNoAPErrorL, Creating CenRep observer" );
        iCenRepObserver = CMsgCenRepObserver::NewL( this );
        iCenRepObserver->SubscribeNotification();
        }
    //Let's reset the TMsvEntry::iError to get rig of excess warning note
    ResetErrorFieldL( aEntry );
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleInvalidAPErrorL
//
// Handles "invalid access point" error
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleInvalidAPErrorL( TMsvEntry& aEntry, TBool aStartObserver )
    {
    MEWLOGGER_ENTERFN( "HandleInvalidAPErrorL" );
    iMmsReceiveErrorMessages->AppendL( aEntry.Id() );
    ShowGlobalNoteL( EInvalidAccessPointNote );
    if ( aStartObserver && !iCommDbObserver )
        {
        MEWLOGGER_WRITE( "HandleInvalidAPErrorL, Creating CommDB observer" );
        iCommDbObserver = CMsgErrorCommDbObserver::NewL( this );
        }
    if ( aStartObserver && !iCenRepObserver )
        {
        //We must also start cenrep observer
        MEWLOGGER_WRITE( "HandleInvalidAPErrorL, Creating CenRep observer" );
        iCenRepObserver = CMsgCenRepObserver::NewL( this );
        iCenRepObserver->SubscribeNotification();
        }    
    
    //Let's reset the TMsvEntry::iError to get rig of excess warning note
    ResetErrorFieldL( aEntry ); 
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleConnectionErrorL
//
// Handles "connection reserved" error
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleConnectionErrorL( TMsvEntry& aEntry, TBool aReceive )
    {
    MEWLOGGER_ENTERFN( "HandleConnectionErrorL" );
    if ( !iConnectionObserver )
        {
        MEWLOGGER_WRITE( "HandleConnectionErrorL, Creating connection observer" );
        iConnectionObserver = CMsgErrorConnectionObserver::NewL( this );
        }
    if ( iConnectionObserver->ConnectionsOpen() )
        {
        MEWLOGGER_WRITE( "HandleConnectionErrorL, Open connections detected" );
        if ( aReceive )
            {
            iMmsReceiveErrorMessages->AppendL( aEntry.Id() );
            }
        else
            {
            iMmsSendErrorMessages->AppendL( aEntry.Id() );
            }
        //No matter if already started
        iConnectionObserver->StartL();

        delete iDisconnectQuery;
        iDisconnectQuery = NULL;
    
        MEWLOGGER_ENTERFN( "Reading disconnect error text" );
        TPtrC string( iResourceReader->ReadResourceString( aReceive
            ? R_MSG_ERRH_CONN_IN_USE_RECEIVE
            : R_MSG_ERRH_CONN_IN_USE_SEND ) );

        MEWLOGGER_ENTERFN( "Displaying global query" );
        iDisconnectQuery = CMsgErrorGlobalQuery::NewL( this );
        iDisconnectQuery->ShowQueryL(
            string,
            R_AVKON_SOFTKEYS_YES_NO,
            EConnectionInUseNote );
        }
    else
        {
        MEWLOGGER_WRITE( "HandleConnectionErrorL, No open connections" );
        delete iConnectionObserver;
        iConnectionObserver = NULL;
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleDiskSpaceErrorL
//
// Handles "no disk space" error
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleDiskSpaceErrorL( TMsvEntry& aEntry )
    {
    MEWLOGGER_ENTERFN( "HandleDiskSpaceErrorL" );
    iMmsReceiveErrorMessages->AppendL( aEntry.Id() );

    iDiskSpaceErrors++;
    if ( iDiskSpaceErrors == 1 )
        {
        ShowGlobalNoteL( EDiskLowNote1 );
        }
    else
        {
        ShowGlobalNoteL( EDiskLowNoteN );
        }
    TUint triggerLevel = Max( KDefaultTriggerLevel, iMaxReceiveSize );
    //Activate DiskSpace observer
    if ( !iDiskSpaceObserver )
        {
        MEWLOGGER_WRITE( "HandleDiskSpaceErrorL, Creating disk space observer" );
        iDiskSpaceObserver = CMsgErrorDiskSpaceObserver::NewL( this, *iSession, iFs );
        }
    MEWLOGGER_WRITEF( _L("Limit set to: %d"),
        KCriticalLevel + triggerLevel + KTriggerMargin );
    iDiskSpaceObserver->SetLimitAndActivateL( KCriticalLevel +
        triggerLevel +
        KTriggerMargin );
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleMemoryErrorL
//
// Handles "no memory" error
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleMemoryErrorL( TMsvEntry& aEntry )
    {
    MEWLOGGER_ENTERFN( "HandleMemoryErrorL" );
    iMmsReceiveErrorMessages->AppendL( aEntry.Id() );
    ShowGlobalNoteL( EMemoryLowNote );
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleRoamingEventL
//
// Handles events from roaming observer
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleRoamingEventL( TBool aRoaming )
    {
    MEWLOGGER_ENTERFN( "HandleRoamingEventL" );
    if ( !iMmsClient )
        {
        // We should never get here if MMS Client MTM is not present
        // since roaming observer is not started in that case.
        // This return is here just in case...
        return;
        }

    TInt fetchHome = iMmsClient->MmsSettings().ReceivingModeHome();
    TInt fetchRoam = iMmsClient->MmsSettings().ReceivingModeForeign();
    TBool fetchAll = EFalse;

    if ( aRoaming )
        {
        //We are in roaming network
        if ( fetchRoam == EMmsReceivingAutomatic &&
            fetchHome != EMmsReceivingAutomatic )
            {
            fetchAll = ETrue;
            }
        if ( ( iWatcherFlags & EShowRoamingNote ) &&
            !( iWatcherFlags & ERoamingNoteShown ) )
            {
            //Show roaming note if requested
            ShowGlobalNoteL( ERoamingNote );
            iWatcherFlags |= ERoamingNoteShown;
            }
        }
    else
        {
        //We are in home network
        if ( fetchHome == EMmsReceivingAutomatic &&
            fetchRoam != EMmsReceivingAutomatic )
            {
            fetchAll = ETrue;
            }
        //Reset roaming note flag
        iWatcherFlags &= ~ERoamingNoteShown;
        }

    if ( fetchAll && !IsActive() )
        {
        MEWLOGGER_WRITE( "HandleRoamingEventL, starting fetch all" );
        iStatus = KRequestPending;
        iRequestType = EMsgRequestFetchingAll;
        delete iOperation;
        iOperation = NULL;
        iOperation = iMmsClient->FetchAllL( iStatus, EFalse );
        SetActive();
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleCommDbEventL
//
// Handles events from CommDB observer
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleCommDbEventL()
    {
    if ( ValidateMmsServiceL() )
        {
        MEWLOGGER_WRITE( "HandleCommDbEventL, starting fetch" );
        StartMmsFetchL();
        }
    else
        {
        //Wait for another event
        MEWLOGGER_WRITE( "HandleCommDbEventL, restart CommDB observer" );
        iCommDbObserver->Restart();
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleConnectionEvent
//
// Handles events from connection observer
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleConnectionEvent()
    {
    if ( !IsActive() )
        {
        MEWLOGGER_WRITE( "HandleConnectionEvent, Starting delay timer" );
        iStatus = KRequestPending;
        iRequestType = EMsgRequestWaitingDisconnection;
        iTimer.After( iStatus, KDelayAfterDisconnect );
        SetActive();
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleDiskSpaceEventL
//
// Handles events from disk space observer
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleDiskSpaceEventL()
    {
    MEWLOGGER_WRITE( "HandleDiskSpaceEventL, starting fetch" );
    StartMmsFetchL();
    }


// ---------------------------------------------------------
// CMsgErrorWatcher::HandleDiskSpaceEvent2L
//
// Handles events from disk space observer
// -------k--------------------------------------------------
//
void CMsgErrorWatcher::HandleDiskSpaceEvent2L()
    {
    MEWLOGGER_WRITE( "HandleDiskSpaceEvent2L, show note" );
    ShowGlobalNoteL( ESMSIncomingLowDiskSpace );
    
    }
// ---------------------------------------------------------
// CMsgErrorWatcher::HandleGlobalQueryEventL
//
// Handles events from global query observer
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleGlobalQueryEventL( TInt aQueryId, TInt aStatus )
    {
    switch ( aQueryId )
        {
        case ESendFailureNote:
            {
            if ( aStatus == EAknSoftkeyDetails && iErrorMsgDetails )
                {
                CAknGlobalNote* note = CAknGlobalNote::NewLC();
                note->SetSoftkeys( R_AVKON_SOFTKEYS_OK_EMPTY );
                TInt id = note->ShowNoteL( EAknGlobalInformationNote,
                    *iErrorMsgDetails );
                CleanupStack::PopAndDestroy( note ); 
                iNoteIds->AppendL( id );
                }
            if ( iErrorMsgNotes->Count() && !IsActive() )
                {
                iStatus = KRequestPending;
                iRequestType = EMsgRequestWaitingErrorNote;
                if ( !IsActive() ) 
                    {
                    iTimer.After( iStatus, KDelayBetweenNotes );
                    SetActive();
                    }
                // Don't delete error query yet. If iErrorQuery
                // exists other error notes are queued instead
                // of shown.
                }
            else
                {
                delete iErrorQuery;
                iErrorQuery = NULL;
                }
            }
            break;
        case EConnectionInUseNote:
            {
        MEWLOGGER_WRITEF( _L("ConnectionInUseNote return status: %d"), aStatus );
            if ( aStatus == EAknSoftkeyYes ||
                aStatus == EAknSoftkeyOk )
                {
                delete iDisconnectDlg;
                iDisconnectDlg = NULL;
                MEWLOGGER_WRITE( "HandleGlobalQueryEventL, Starting disconnect dialog" );
                iDisconnectDlg = CMsgErrorDisconnectDlg::NewL();
                iDisconnectDlg->Start();
                }
            delete iDisconnectQuery;
            iDisconnectQuery = NULL;
            }
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleStartupReadyL
//
// Handles events from startup state observer (currently 
// CMsgSentItemsObserver)
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleStartupReadyL()
    {
    MEWLOGGER_WRITE( "Startup ready!" );
    iWatcherFlags |= EStartupReady;
    CheckMmsReceivingModeL();
    }


// ---------------------------------------------------------
// CMsgErrorWatcher::HandleCenRepNotificationL
//
// Handles events from Central Repository observer
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleCenRepNotificationL()
    {
    if ( ValidateMmsServiceL() )
        {
        MEWLOGGER_WRITE( "HandleCenRepNotificationL, starting fetch" );
        StartMmsFetchL();
        }
    else
        {
        //Wait for another event
        MEWLOGGER_WRITE( "HandleCenRepNotificationL, restart CenRep observer" );
        iCenRepObserver->SubscribeNotification();
        }
    }


// ---------------------------------------------------------
// CMsgErrorWatcher::ShowDeliveredNote
//
// Handles events from Logs observer
// ---------------------------------------------------------
//
void CMsgErrorWatcher::ShowDeliveredNoteL( const TDesC& aRemoteParty )
	{
	ShowReadOrDeliveredNoteL( aRemoteParty, R_MMS_DELIVERED );
	}


// ---------------------------------------------------------
// CMsgErrorWatcher::ShowReadNoteL
//
// Handles events from Logs observer
// ---------------------------------------------------------
//
void CMsgErrorWatcher::ShowReadNoteL( const TDesC& aRemoteParty )
	{
	ShowReadOrDeliveredNoteL( aRemoteParty, R_MMS_READ );
	}

// ---------------------------------------------------------
// CMsgErrorWatcher::ShowReadNoteL
//
// Handles events from Logs observer
// ---------------------------------------------------------
//
void CMsgErrorWatcher::ShowReadOrDeliveredNoteL( const TDesC& aRemoteParty, TInt aResourceId )
	{
    TPtrC original;
    original.Set( iResourceReader->ReadResourceString( aResourceId ) );
    
    HBufC* wholeTextBuf = HBufC::NewLC( original.Length() + aRemoteParty.Length() 
        + KExtraSpaceForDirectionalityMarkers  );
            
    TPtr wholeText = wholeTextBuf->Des();
    StringLoader::Format ( wholeText, original, -1, aRemoteParty );
    AknTextUtils::LanguageSpecificNumberConversion( wholeText );
    CAknGlobalNote* note = CAknGlobalNote::NewLC();
    note->ShowNoteL( EAknGlobalInformationNote, wholeText );
    CleanupStack::PopAndDestroy( note );
    CleanupStack::PopAndDestroy( wholeTextBuf );
	}


// ---------------------------------------------------------
// CMsgErrorWatcher::HandleSessionEventL
//
// Handles events from MsgServer observer
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleSessionEventL( TMsvSessionEvent aEvent,
                                           TAny* aArg1,
                                           TAny* aArg2,
                                           TAny* aArg3 )
    {
#ifdef USE_LOGGER
    if ( aEvent == EMsvServerReady )
        {
        MEWLOGGER_WRITE( "Message Server Ready event." );
        }
#endif
    if ( ( aEvent == EMsvCloseSession ||
        aEvent == EMsvServerTerminated ||
        aEvent == EMsvMediaUnavailable ||
        aEvent == EMsvMediaChanged ) &&
        iWatcherFlags & EWatcherRunning )
        {
        MEWLOGGER_WRITEF( _L("StopWatcher event: %d"), aEvent );
        StopWatcher();
        //Start retry timer
        StartRestartTimer();
        return;
        }
    if ( aEvent == EMsvServerReady &&
        !( iWatcherFlags & EWatcherRunning ) )
        {
        TRAPD ( err, StartWatcherL() );
        if ( err ) //make sure watcher is not left in obscure state
            {
            StopWatcher();
            }
        return;
        }
    if ( aArg1 == 0 || aArg2 == 0 || !( iWatcherFlags & EWatcherRunning ) )
        {
        return;
        }
    // If for some reason MMS service is not yet found,
    // we try to find it now...
    GetMmsServiceL();

    CMsvEntrySelection* entries = static_cast<CMsvEntrySelection*>( aArg1 );
    TInt count = entries->Count();
    
    // Mark the _original_ folder as parent for "entries moved" events (in "aArg3").
    // For other events the parent is in "aArg2".
    TMsvId parentId = ( aEvent == EMsvEntriesMoved )
        ? *( static_cast<TMsvId*>( aArg3 ) )
        : *( static_cast<TMsvId*>( aArg2 ) );
    
    
    if ( count < 1 )
        {
        return;
        }
    if ( parentId == KMsvRootIndexEntryIdValue &&
        iMmsServiceId != KMsvNullIndexEntryId )
        {
        // We're not interested in these events if MMS Service is not present.
        HandleRootEventL( aEvent, entries );
        }
    else if ( parentId == KMsvLocalServiceIndexEntryIdValue )
        {
        HandleLocalServiceEventL( aEvent, entries );
        }
    else if ( parentId == KWatcherInboxFolderId )
        {
        HandleInboxEventL( aEvent, entries );
        }
    else if ( parentId == KWatcherOutboxFolderId )
        {
        HandleOutboxEventL( aEvent, entries );
        }
    else if ( ( iMmsServiceId != KMsvNullIndexEntryId && parentId == iMmsServiceId ) ||
        ( iNotificationFolderId != KMsvNullIndexEntryId && parentId == iNotificationFolderId ) )
        {
        HandleMmsServiceEventL( aEvent, entries );
        }
    else
        {
        //do nothing
        }    
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleRootEventL
//
// Handles root events from MsgServer observer
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleRootEventL( TMsvSessionEvent aEvent,
                                        CMsvEntrySelection* aEntries )
    {
    switch ( aEvent )
        {
        case EMsvEntriesChanged:
            {
            TInt count = aEntries->Count();
            TInt i = 0;
            while ( i < count )
                {
                TMsvId dummy;
                TMsvEntry entry;
                TInt error = iSession->GetEntry(
                    aEntries->At( i ), dummy, entry );

                //We're only interested in MMS service
                if ( !error && 
                    iMmsServiceId != KMsvNullIndexEntryId &&
                    entry.Id() == iMmsServiceId )
                    {
                    MEWLOGGER_WRITE( "HandleSessionEventL, MMS service changed" );
                    //Check whether the roaming setting has changed
                    CheckMmsReceivingModeL();

                    //We're waiting for the the user to change access points
                    //if iCommDbObserver exists
                    if ( iCommDbObserver && ValidateMmsServiceL() )
                        {
                        MEWLOGGER_WRITE( "HandleSessionEventL, deleting CommDB observer" );
                        StartMmsFetchL();
                        }
                    }
                i++;
                }
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleLocalServiceEventL
//
// Handles local service events from MsgServer observer
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleLocalServiceEventL( TMsvSessionEvent aEvent,
                                                CMsvEntrySelection* aEntries )
    {
    if ( iNotificationFolderId == KMsvNullIndexEntryId &&
        aEvent == EMsvEntriesCreated )
        {
        TInt count = aEntries->Count();
        TInt i = 0;
        while ( i < count )
            {
            TMsvId dummy;
            TMsvEntry entry;
            TInt error = iSession->GetEntry(
                aEntries->At( i ), dummy, entry );
            if ( !error &&
                entry.iDetails.Compare( KMMSNotificationFolder ) == 0 )
                {
                iNotificationFolderId = aEntries->At( i );
                MEWLOGGER_WRITEF( _L("Notification folder created: %d"), iNotificationFolderId );
                }
            i++;
            }
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleInboxEventL
//
// Handles outbox events from MsgServer observer
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleInboxEventL( TMsvSessionEvent aEvent,
                                         CMsvEntrySelection* aEntries )
    {
    switch (aEvent)
        {
        case EMsvEntriesChanged:
            {
            TInt count = aEntries->Count();
            TInt i = 0;
            while ( i < count )
                {
                TMsvId dummy;
                TMsvEntry entry;
                TInt error = iSession->GetEntry(
                    aEntries->At( i ), dummy, entry );
                TUid mtm = entry.iMtm;
                //We're only interested in MMS notifications
                if ( !error && 
                    ( mtm == KUidMsgMMSNotification ) &&
                    entry.iType == KMessageEntryUid )
                    {
                    TUint sendingState = entry.SendingState();
                    MEWLOGGER_WRITEF( _L("Sendstate: %d"), sendingState );
                    MEWLOGGER_WRITEF( _L("Failed: %d"), (TInt)entry.Failed() );
                    MEWLOGGER_WRITEF( _L("Error: %d"), entry.iError );
                    TInt selectionId = iErrorMessages->Find( entry.Id() );

                    if ( sendingState == KMsvSendStateFailed &&
                        selectionId == KErrNotFound )
                        {
                        MEWLOGGER_WRITE( "Send msg added" );
                        iErrorMessages->AppendL( entry.Id() );
                        iErrorMsgNotes->AppendL( entry.Id() );
                        if ( !iErrorQuery )
                            {
                            ShowErrorMessageQueryL();
                            }
                        }
                    else if ( sendingState != KMsvSendStateFailed &&
                        selectionId != KErrNotFound )
                        {
                        MEWLOGGER_WRITE( "Send msg removed" );
                        iErrorMessages->Delete( selectionId );
                        }
                    else
                        {
                        //Do nothing
                        }
                    }
                i++;
                }
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleOutboxEventL
//
// Handles outbox events from MsgServer observer
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleOutboxEventL( TMsvSessionEvent aEvent,
                                          CMsvEntrySelection* aEntries )
    {
    TInt count = aEntries->Count();
    switch (aEvent)
        {
        case EMsvEntriesChanged:
            {
            TInt i = 0;
            while ( i < count )
                {
                TMsvId dummy;
                TMsvEntry entry;
                TInt error = iSession->GetEntry(
                    aEntries->At( i ), dummy, entry );
                TUid mtm = entry.iMtm;

                if ( mtm == KSenduiMtmMmsUid &&
                    iMmsSendErrorMessages->Find( entry.Id() ) == KErrNotFound )
                    {
                    if ( // the first error is activated again to synchronize
                        // with connection manager (MPM) 
                        entry.iError == KErrPacketDataTsyMaxPdpContextsReached ||
                        entry.iError == KErrUmtsMaxNumOfContextExceededByPhone ||
                        entry.iError == KErrUmtsMaxNumOfContextExceededByNetwork ||
                        // add the following error to the list to synchronize with 
                        // connection manager (MPM)
                        entry.iError == KErrGprsInsufficientResources )
                        {
                        MEWLOGGER_WRITE( "MMS send - connection active" );
                        
                        //Let's now save the id. This way we can reset the entrys
                        //error field just before the fetch start. This prevents
                        //duplicate error notes because this case branch is reached many times.
                        iCurrentEntryId = entry.Id();
                        
                        HandleConnectionErrorL( entry, EFalse );
                        }
                    }

                //We're only interested in Mail, SMS & MMS messages
                if ( !error && 
                    ( mtm == KSenduiMtmSmsUid ||
                    mtm == KSenduiMtmMmsUid  ||
                    mtm == KSenduiMtmSmtpUid ||
                    mtm == KUidMsgMMSNotification ) &&
                    entry.iType == KMessageEntryUid )
                    {
                    TUint sendingState = entry.SendingState();
                    MEWLOGGER_WRITEF( _L("Sendstate: %d"), sendingState );
                    MEWLOGGER_WRITEF( _L("Failed: %d"), (TInt)entry.Failed() );
                    MEWLOGGER_WRITEF( _L("Error: %d"), entry.iError );
                    if ( mtm == KSenduiMtmSmtpUid && entry.Failed() )
                        {
                        // special Failed flag handling for e-mail
                        sendingState = KMsvSendStateFailed;
                        }
                    TInt selectionId = iErrorMessages->Find( entry.Id() );

                    if ( sendingState == KMsvSendStateFailed &&
                        selectionId == KErrNotFound )
                        {
                        MEWLOGGER_WRITE( "Send msg added" );
                        iErrorMessages->AppendL( entry.Id() );
                        iErrorMsgNotes->AppendL( entry.Id() );
                        if ( !iErrorQuery )
                            {
                            ShowErrorMessageQueryL();
                            }
                        }
                    else if ( sendingState != KMsvSendStateFailed &&
                        selectionId != KErrNotFound )
                        {
                        MEWLOGGER_WRITE( "Send msg removed" );
                        iErrorMessages->Delete( selectionId );
                        }
                    else
                        {
                        
                        }    
                    }
                i++;
                }
            }
            break;
        case EMsvEntriesMoved: // Messages are moved _from_ this folder.
        case EMsvEntriesDeleted:
            {
            TInt i = 0;
            MEWLOGGER_WRITEF( _L("Entries deleted: %d"), count );
            while ( i < count )
                {
                TInt selectionId = iMmsSendErrorMessages->Find( aEntries->At( i ) );
                if ( selectionId != KErrNotFound )
                    {
                    MEWLOGGER_WRITE( "MMS Send msg removed" );
                    iMmsSendErrorMessages->Delete( selectionId );
                    }
                i++;
                }
            }
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::HandleMmsServiceEventL
//
// Handles MMS service events from MsgServer observer
// ---------------------------------------------------------
//
void CMsgErrorWatcher::HandleMmsServiceEventL( TMsvSessionEvent aEvent,
                                              CMsvEntrySelection* aEntries )
    {
    TInt count = aEntries->Count();
    switch (aEvent)
        {
        case EMsvEntriesChanged:
            {
            TInt i = 0;
            while ( i < count )
                {
                TMsvId dummy;
                TMsvEntry entry;
                TInt error = iSession->GetEntry(
                    aEntries->At( i ), dummy, entry );
#ifdef USE_LOGGER
                if ( !error )
                    {
                    MEWLOGGER_WRITEF( _L("FetchState: %d"), entry.SendingState() );
                    MEWLOGGER_WRITEF( _L("Error: %d"), entry.iError );
                    MEWLOGGER_WRITEF( _L("Retries: %d"), entry.iMtmData3 );
                    MEWLOGGER_WRITEF( _L("Failed: %d"), (TInt)entry.Failed() );
                    MEWLOGGER_WRITEF( _L("ArrayId: %d"), iMmsReceiveErrorMessages->Find( entry.Id() ) );
                    }
#endif
                //Check that reception has failed and that the entry is not
                //already in iMmsReceiveErrorMessages
                if ( !error &&
                    iMmsReceiveErrorMessages->Find( entry.Id() ) == KErrNotFound )
                    {
                    TInt entryErr = entry.iError;
                    if ( entryErr == KErrGprsMissingorUnknownAPN )
                        {
                        // Map to "invalid ap" error.
                        entryErr = KMmsErrorAP1Invalid;
                        }
                    switch ( entryErr )
                        {
                        case KErrDiskFull:
                            {
                            MEWLOGGER_WRITE( "MMS fetch - disk full" );
                            HandleDiskSpaceErrorL( entry );
                            }
                            break;
                        case KErrNoMemory:
                            {
                            MEWLOGGER_WRITE( "MMS fetch - out of memory" );
                            HandleMemoryErrorL( entry );
                            }
                            break;
                        // the first error is activated again to synchronize
                        // with connection manager (MPM)
                        case KErrPacketDataTsyMaxPdpContextsReached:
                        case KErrUmtsMaxNumOfContextExceededByPhone:
                        case KErrUmtsMaxNumOfContextExceededByNetwork:
                        // add the following error to the list to synchronize with 
                        // connection manager (MPM)
                        case KErrGprsInsufficientResources:
                            {
                            //Let's now save the id. This way we can reset the entrys
                            //error field just before the fetch start. This prevents
                            //duplicate error notes because this case branch is reached many times.
                            iCurrentEntryId = entry.Id();
                            //Connection already active should be "detected"
                            //only after third retry failure if "disconnect
                            //delay" feature is activated.
                            TInt retries = ( iWatcherFlags & EReceivingDisconnectDelay )
                                ? KConnectionRetries
                                : 0;
                            if ( ( entry.iMtmData3 & KMmsRetryCountMask ) >= retries ) //lint !e574
                                {
                                MEWLOGGER_WRITE( "MMS fetch - connection active" );
                                
                                
                                HandleConnectionErrorL( entry, ETrue );
                                }
                            }

                            break;

                        
                        
                        case KMmsErrorNoWAPAccessPoint:
                            {
                            MEWLOGGER_WRITE( "MMS fetch - no access point" );
                            HandleNoAPErrorL( entry );
                            }
                            break;
                        case KMmsErrorAP1Invalid:
                        case KMmsErrorNoURI1:
                            {
                            MEWLOGGER_WRITE( "MMS fetch - invalid access point" );
                            HandleInvalidAPErrorL( entry, ETrue );
                            }
                            break;
                        case KErrIfAuthenticationFailure: //username/passwd
                            {
                            MEWLOGGER_WRITE( "MMS fetch - username/passwd" );
                            HandleInvalidAPErrorL( entry, EFalse );
                            }
                            break;
                        default:
                            //nothing to do
                            break;
                        }
                    }
                i++;
                }
            }
            break;
        case EMsvEntriesMoved: // Messages are moved _from_ this "folder".
        case EMsvEntriesDeleted:
            {
            TInt i = 0;
            TInt originalCount = iMmsReceiveErrorMessages->Count();

MEWLOGGER_WRITEF( _L("Entries deleted: %d"), count );

            while ( i < count )
                {
                TInt selectionId = iMmsReceiveErrorMessages->Find( aEntries->At( i ) );
                if ( selectionId != KErrNotFound )
                    {
MEWLOGGER_WRITE( "Fetch msg removed" );
                    iMmsReceiveErrorMessages->Delete( selectionId );
                    }
                i++;
                }
            if ( originalCount && !iMmsReceiveErrorMessages->Count() )
                {
                // array was emptied
                ResetWatcher();
                }
            }
            break;
        default:
            break;
        }
    }


// ---------------------------------------------------------
// CMsgErrorWatcher::ResetErrorFieldL
//
// Reset TMsvEntry::iError of the current notification
// ---------------------------------------------------------
//
void CMsgErrorWatcher::ResetErrorFieldL( )
    {
    //Makes sure the entry is set
    if( iCurrentEntryId != KMsvNullIndexEntryId )
        {
        CMsvEntry *cEntry( NULL );
        TRAPD( err, cEntry = iSession->GetEntryL( iCurrentEntryId ) );
        if ( err == KErrNotFound )
        	{
        	iCurrentEntryId = KMsvNullIndexEntryId;
        	return;
        	}
        CleanupStack::PushL( cEntry );
        TMsvEntry tEntry = cEntry->Entry();
        tEntry.iError = KErrNone;
        cEntry -> ChangeL( tEntry );
        CleanupStack::PopAndDestroy( cEntry );
        //This prevents getting here to often
        iCurrentEntryId = KMsvNullIndexEntryId;
        }   
    }
    
// ---------------------------------------------------------
// CMsgErrorWatcher::ResetErrorFieldL
//
// Reset TMsvEntry::iError 
// ---------------------------------------------------------
//
void CMsgErrorWatcher::ResetErrorFieldL( TMsvEntry& aEntry )
    {
    CMsvEntry *cEntry = iSession->GetEntryL( aEntry.Id() );  
    CleanupStack::PushL( cEntry );
    aEntry.iError = KErrNone;
    cEntry -> ChangeL( aEntry );
    CleanupStack::PopAndDestroy( cEntry ); 
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorWatcher::DoCancel()
    {
    iTimer.Cancel();
    if ( iOperation )
        {
        iOperation->Cancel();
        }
    }

// ---------------------------------------------------------
// CMsgErrorWatcher::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorWatcher::RunL()
    {
MEWLOGGER_WRITEF( _L("RunL, iStatus: %d"), iStatus.Int() );
MEWLOGGER_WRITEF( _L("RunL, iRequestType: %d"), iRequestType );
    switch ( iRequestType )
        {
        case EMsgRequestStartingUp:
            {
            if ( !( iWatcherFlags & EWatcherRunning ) )
                {
                TRAPD ( err, StartWatcherL() );
                if ( err ) //make sure watcher is not left in obscure state
                    {
MEWLOGGER_WRITEF( _L("Leave from StartWatcherL: %d"), err );
                    StopWatcher();
                    if ( iTimerRetries < KMaxTimerRetries )
                        {
                        StartRestartTimer();
                        }
                    //else give up
                    }
                }
            }
            break;
        case EMsgRequestSending:
            if ( iStatus.Int() == KErrNone )
                {
                ShowGlobalNoteL( ESendingMessageNote );
                }
            if ( iMmsReceiveErrorMessages->Count() )
                {
                StartMmsFetchL();
                }
            else
                {
                MEWLOGGER_WRITE( "RunL, Deleting connection observer" );
                delete iConnectionObserver;
                iConnectionObserver = NULL;
                ResetErrorFieldL();
                }
            break;
        case EMsgRequestFetching:
            ResetErrorFieldL();
            ResetWatcher();
            break;
        case EMsgRequestWaitingErrorNote:
            ShowErrorMessageQueryL();
            break;
        case EMsgRequestWaitingDisconnection:
            {
            MEWLOGGER_WRITE( "RunL, Disconnect delay passed" );
            if ( iMmsSendErrorMessages->Count() )
                {
                StartMmsSendL();
                }
            else
                {
                StartMmsFetchL();
                }
            //ResetErrorFieldL();     
            }
            break;
        case EMsgRequestFetchingAll:
        default:
            break;
        }
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//
// ---------------------------------------------------------
// Panic implements
// panic, for debug version only
// ---------------------------------------------------------
//
GLDEF_C void Panic( TInt aPanic ) // enum for panic codes
    {
    _LIT( KPanicText, "MsgErrorWatcher" );
    User::Panic( KPanicText, aPanic );
    }   

//  End of File  

