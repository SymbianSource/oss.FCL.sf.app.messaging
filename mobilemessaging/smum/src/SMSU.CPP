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
* Description:  
*     Sms Ui Mtm.
*
*/



// INCLUDE FILES

// Specific includes
#include <SMUM.rsg>                     // resource ids
#include <etelmm.h>
#include <smuthdr.h>                    // for CSmsHeader
#include <smutset.h>                    // CSmsNumber
#include <smscmds.h>                    // TSmsMtmCommand
#include <aknViewAppUi.h>               // iAvkonViewAppUi
#include <MtmExtendedCapabilities.hrh>
// Standard includes
#include <txtrich.h>                    // CRichText
#include <aknnotewrappers.h>
// Comms includes
#include <MuiuMsgEditorLauncher.h>          // MsgEditorLauncher
#include <MsgBioUids.h>					 // KMsgBioUidPictureMsg
#include <akninputblock.h>			    // CAknInputBlock
#include <Muiumsginfo.h>                // CMsgInfoMessageInfoDialog
#include <MuiuOperationWait.h>              // MuiuOperationWait
// other includes
#include <StringLoader.h>               // StringLoader
#include <ErrorUI.h>                    // CErrorUI

#include <centralrepository.h>
#include <messaginginternalcrkeys.h>
#include <messagingvariant.hrh>
#include <BTSapDomainPSKeys.h>          // KPSUidBluetoothSapConnectionState, KBTSapConnectionState
#include <startupdomainpskeys.h>
#include <gmsModel.h>

#include <e32property.h>                // RProperty
#include <PSVariables.h>                // RProperty
#include <sacls.h>

#include <csmsaccount.h>
#include <csmsemailfields.h>            // CSmsEmailFields
// locals
#include "SmumSettingsDialogGSM.h"         // CSmumMainSettingsDialogGSM
#include "SmumUtil.h"                   // SmumUtil
#include "SMSU.HRH"                     // for KMtmUiFunctionSimDialog
#include "SMSU.H"                       // CSmsMtmUi
#include "smsui.pan"                    // panics
#include "simdlgplugininterface.h"
#include "BioOpenOp.h"					// CBioOpenOp
#include "MsgSimSCNumberDetector.h"     // CMsgSimOperation    

// Includes for fix to error SPAN-6B7K33
#include <rmmcustomapi.h>
#include <mmtsy_names.h>

//For logging
#include "SmumLogging.h"

//  LOCAL CONSTANTS AND MACROS
const TInt KMaxSubjectLength = 30;
const TInt KSmumAdditionalCharsStringLength = 7;
const TUint KSmumLRMarker = 0x200E;
const TUint KSmumChineseOffset = 8;
_LIT( KSmsuResourceFile, "SMUM" );
_LIT( KSmumSpace, " " );
_LIT( KSmumCharLeftAddressIterator, "<" );
_LIT( KSmumCharRightAddressIterator, ">" );
_LIT( KSmumCharCommaAndSpace, ", " );
_LIT( KSmumSmsService, "Sms Service" );
_LIT( KSmumChinaCountryCode, "+86");
_LIT( KSmumInternationalPrefix, "+");

// Default values, if shared data reading fails
const TInt KDefDeliveryReport = 1;
const TInt KDefValidityPeriod = 1440;
const TInt KDefMessageConversion = 0;
const TInt KDefPreferredConnection = 3;
const TInt KDefReplyViaSameCentre = 0;
// Constants for sms initialization
const TInt KInitializationReady = 2;
const TInt KInitializationReadSim = 3;

//  MEMBER FUNCTIONS

// Factory constructor function
CSmsMtmUi* CSmsMtmUi::NewL( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll )
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUi::NewL");
    CSmsMtmUi* smsmtmui=new ( ELeave ) CSmsMtmUi( aBaseMtm, aRegisteredMtmDll );
    CleanupStack::PushL( smsmtmui );
    smsmtmui->ConstructL();
    CleanupStack::Pop();
    SMUMLOGGER_LEAVEFN(" CSmsMtmUi::NewL");
    return smsmtmui;
    }

// C++ constructor can NOT contain any code, that
// might leave.
CSmsMtmUi::CSmsMtmUi( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll ):
    CBaseMtmUi( aBaseMtm, aRegisteredMtmDll )
    {
    }

// Symbian OS default constructor can leave.
void CSmsMtmUi::ConstructL()
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUi::ConstructL");
    CBaseMtmUi::ConstructL();

	TInt err( KErrGeneral );
	// connect etel server
    User::LeaveIfError( iServer.Connect() );
    // load tsy
    err = iServer.LoadPhoneModule( KMmTsyModuleName );
    if ( err != KErrAlreadyExists )
        {
        // may return also KErrAlreadyExists if some other
        // is already loaded the tsy module. And that is
        // not an error.
        User::LeaveIfError( err );
        }
	User::LeaveIfError( iPhone.Open( iServer, KMmTsyPhoneName ) );
	
    // Error resolver
    iErrorResolver = CTextResolver::NewL( *iCoeEnv );
    // Create the session
    iCenRepSession = CRepository::NewL( KCRUidSmum );
	// create sim-dialog plugin interface
    iSimDlgPluginIntf = CSimDlgPluginInterface::NewL(*this, Session());    
    SMUMLOGGER_LEAVEFN(" CSmsMtmUi::ConstructL");
    }

// Destructor
CSmsMtmUi::~CSmsMtmUi()
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUi::~CSmsMtmUi");
    //close ETel connection
    if (iServer.Handle())
        {
        iPhone.Close();           
        iServer.UnloadPhoneModule(KMmTsyModuleName);
        iServer.Close();
        }
    delete iErrorResolver;
    delete iCenRepSession;
    delete iSimDlgPluginIntf;
    SMUMLOGGER_LEAVEFN(" CSmsMtmUi::~CSmsMtmUi");
    }

// ---------------------------------------------------------
// CSmsMtmUi::CreateL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::CreateL(const TMsvEntry& aEntry, CMsvEntry& aParent, TRequestStatus& aStatus)
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUi::CreateL");
    //    Create a Sms message (services are provided "for nothing", created invisibly by the UI layer.
    __ASSERT_DEBUG( aEntry.iMtm == Type(), Panic( ESmsuWrongMtmType ));
    __ASSERT_DEBUG( aEntry.iType == KUidMsvMessageEntry, Panic( ESmsuNotAMessage ));
    __ASSERT_DEBUG( aParent.Entry().iServiceId == KMsvLocalServiceIndexEntryId ||
        aParent.Entry().iServiceId == KMsvNullIndexEntryId, Panic( ESmsuMessageNotLocal ));
    __ASSERT_DEBUG(( aParent.Entry().iType == KUidMsvFolderEntry &&
        aParent.Entry().iServiceId == KMsvLocalServiceIndexEntryId), 
        Panic( ESmsuMessageParentNotLocalFolder ));

    if( !CheckEntry( aEntry ))
        {
        User::Leave( KErrNotSupported );    //    Not SMS, or not a message
        }
 
    CheckAndEnsureSmsServiceOkL( ETrue, ETrue, ETrue );
    
    SMUMLOGGER_LEAVEFN(" CSmsMtmUi::CreateL");
    return LaunchEditorApplicationL( aStatus, aParent.Session(), ECreateNewMessage );
    }

// ---------------------------------------------------------
// CSmsMtmUi::OpenL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::OpenL(TRequestStatus& aStatus)
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUi::OpenL");

    __ASSERT_DEBUG( 
        BaseMtm().Entry().Entry().iMtm == Type(), Panic( ESmsuWrongMtmType ));
    __ASSERT_DEBUG( BaseMtm().Entry().Entry().iType != 
        KUidMsvFolderEntry, Panic( ESmsuFoldersNotSupported ));
    __ASSERT_DEBUG( BaseMtm().Entry().Entry().iType != KUidMsvAttachmentEntry, 
        Panic( ESmsuAttachmentsNotSupported ));

    const TMsvEntry myEntry = BaseMtm().Entry().Entry();
    if( myEntry.iType == KUidMsvFolderEntry || myEntry.iType == KUidMsvAttachmentEntry )
        {
        User::Leave(KErrNotSupported);
        }
    else if ( myEntry.iType == KUidMsvMessageEntry )
        {
        CheckAndEnsureSmsServiceOkL( ETrue, ETrue, ETrue );
        }
    else // Lint
        {
        }
    CSmsClientMtm& smsClientMtm = SmsClientMtm();
    smsClientMtm.LoadMessageL();
    const CSmsPDU::TSmsPDUType pduType = smsClientMtm.SmsHeader().Type();
    CSmsPDU& pdu = smsClientMtm.SmsHeader().Message().SmsPDU();
    TSmsDataCodingScheme::TSmsClass smsClass;
    pdu.Class( smsClass );
    /* 
    Fix to error SPAN-6B7K33
    In case this happens to be an unread message stored on sim, 
    TSY sets the status of the message to 'read' on the sim also.
    */
    if( pduType == CSmsPDU::ESmsDeliver && 
            smsClass == TSmsDataCodingScheme::ESmsClass2)
        {
        CSmsDeliver& deliv = smsClientMtm.SmsHeader().Deliver();

        TTime scTime;
        TInt scQh;
        deliv.ServiceCenterTimeStamp(scTime, scQh);

        RMmCustomAPI ca;
        if ( KErrNone == ca.Open( iPhone ) )
            {
            ca.SetSimMessageStatusRead( scTime, scQh );
            ca.Close();
            }
        }
    // Check are we dealing with delivery raport	
    // No support for 8-bit non GMS messages
    if ( !(pduType == CSmsPDU::ESmsDeliver || pduType == CSmsPDU::ESmsSubmit) || 
        ( pdu.Alphabet() == TSmsDataCodingScheme::ESmsAlphabet8Bit && !IsBioTypeSupported( myEntry ) ) )
        {
        if ( myEntry.Unread() )
            {
            // Mark the entry as read
            CMsvEntry* cEntry = &(BaseMtm().Entry());
            TMsvEntry entry = cEntry->Entry();
            entry.SetUnread( EFalse );
            cEntry->ChangeL( entry );
            }
            
        TSmsSmumProgress progress;
        progress.iError = KErrNotSupported;
        progress.iType = TSmsSmumProgress::ESmsSmumSmsOpening;
        TSmsSmumProgressBuf progressBuf( progress );

        SMUMLOGGER_ENTERFN(" CSmsMtmUi::OpenL - completedOp");
        return CMsvCompletedOperation::NewL(
            Session(), 
            Type(), 
            progressBuf, 
            KMsvLocalServiceIndexEntryId, 
            aStatus, 
            KErrNotSupported );
        }

    if (KMsvDraftEntryIdValue == myEntry.Parent())
        {
        SMUMLOGGER_ENTERFN(" CSmsMtmUi::OpenL- EditL");
        return EditL( aStatus );
        }
    else
        {
        SMUMLOGGER_ENTERFN(" CSmsMtmUi::OpenL- viewL");
        return ViewL( aStatus );
        }
    }

// ---------------------------------------------------------
// CSmsMtmUi::CloseL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::CloseL(TRequestStatus& /*aStatus*/)
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUi::CloseL");
    User::Leave( KErrNotSupported );
    SMUMLOGGER_LEAVEFN(" CSmsMtmUi::CloseL");
    return NULL;
    }

// ---------------------------------------------------------
// CSmsMtmUi::EditL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::EditL( TRequestStatus& aStatus ) 
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUi::EditL");
    __ASSERT_DEBUG( BaseMtm().Entry().Entry().iMtm == 
        Type(), Panic( ESmsuWrongMtmType ));

    CMsvOperation* msvoperation = NULL;
    
    if( BaseMtm().Entry().Entry().iBioType != 0 )
        { // BIO msg
        msvoperation = CBioOpenOp::NewL( 
            Session(),
            aStatus,
            BaseMtm().Entry().EntryId(), 
            Preferences());
        }
    else
        { // not BIO msg
        // What kind of entry are we going to edit?
        switch ( iBaseMtm.Entry().Entry().iType.iUid )
            {
            case KUidMsvServiceEntryValue:
                CheckAndEnsureSmsServiceOkL( ETrue, ETrue, ETrue );
                msvoperation = LaunchSettingsDialogL( aStatus );
                break;
            case KUidMsvMessageEntryValue:
                if ( BaseMtm().Entry().Entry().ReadOnly() )
                    {
                    User::Leave(KErrAccessDenied);
                    }
                msvoperation = LaunchEditorApplicationL( aStatus, iBaseMtm.Entry().Session());
                break;
            case KUidMsvFolderEntryValue:
            case KUidMsvAttachmentEntryValue:
            default:
            #if defined( _DEBUG )
                Panic( ESmsuWrongEntryType );
            #else
                User::Leave( KErrNotSupported );
            #endif
            }   
        }

    SMUMLOGGER_LEAVEFN(" CSmsMtmUi::EditL");
    return msvoperation;
    }

// ---------------------------------------------------------
// CSmsMtmUi::ViewL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::ViewL( TRequestStatus& aStatus )
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUi::ViewL");
     __ASSERT_DEBUG( BaseMtm().Entry().Entry().iMtm == Type(), 
         Panic( EBioViewNotAllowed ));
 
    CMsvOperation* msvoperation = NULL;
    
    if( iBaseMtm.Entry().Entry().iType.iUid == KUidMsvMessageEntryValue )
        {
        msvoperation = LaunchEditorApplicationL( aStatus, iBaseMtm.Entry().Session(), EReadOnly);
        }
    else
        {
        #if defined( _DEBUG )
            Panic( ESmsuWrongEntryType );
            #else
            User::Leave( KErrNotSupported );
            #endif        
        }
    SMUMLOGGER_LEAVEFN(" CSmsMtmUi::ViewL");

    return msvoperation;
    }

// ---------------------------------------------------------
// CSmsMtmUi::OpenL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::OpenL( TRequestStatus& aStatus, const CMsvEntrySelection& aSelection )
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUi::OpenL");
    __ASSERT_DEBUG( aSelection.Count() > 0, Panic( ESmsuEmptySelection ));
    
    //    Open the first entry in the selection
    BaseMtm().SwitchCurrentEntryL( aSelection[0] );
    SMUMLOGGER_LEAVEFN(" CSmsMtmUi::OpenL");
    return OpenL( aStatus );
    }

// ---------------------------------------------------------
// CSmsMtmUi::CloseL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::CloseL( TRequestStatus& /*aStatus*/, const CMsvEntrySelection& /*aSelection*/ )
    {
    SMUMLOGGER_WRITE(" CSmsMtmUi::CloseL");
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CSmsMtmUi::EditL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::EditL( TRequestStatus& aStatus, const CMsvEntrySelection& aSelection )  
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUi::EditL - AGAIN");
    __ASSERT_ALWAYS( aSelection.Count() > 0 , Panic( ESmsuEmptySelection ));

    //    Edit the first entry in the selection
    BaseMtm().SwitchCurrentEntryL( aSelection[0] );
    SMUMLOGGER_LEAVEFN(" CSmsMtmUi::EditL - AGAIN");
    return EditL( aStatus );
    }

// ---------------------------------------------------------
// CSmsMtmUi::ViewL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::ViewL( TRequestStatus& aStatus, const CMsvEntrySelection& aSelection )
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUi::ViewL - AGAIN");
    __ASSERT_DEBUG( aSelection.Count() > 0, Panic( ESmsuEmptySelection ));

    //    View the first entry in the selection
    BaseMtm().SwitchCurrentEntryL( aSelection[0] );
    SMUMLOGGER_LEAVEFN(" CSmsMtmUi::ViewL - AGAIN");
    return ViewL( aStatus );
    }

// ---------------------------------------------------------
// CSmsMtmUi::CancelL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::CancelL( TRequestStatus& aStatus, const CMsvEntrySelection& aSelection )
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUi::CancelL");
    TInt loop = aSelection.Count();

    __ASSERT_DEBUG( loop > 0, Panic( ESmsuEmptySelection ));

    CMsvEntry* entry = Session().GetEntryL( aSelection[0] );
    CleanupStack::PushL( entry );

    while ( loop-- )
        {
        entry->SetEntryL( aSelection[loop] );
        TMsvEntry tempEntry = entry->Entry();
        tempEntry.SetSendingState( KMsvSendStateSuspended );
        tempEntry.iDate.UniversalTime();
        tempEntry.iRelatedId = tempEntry.iServiceId;
        tempEntry.iServiceId = KMsvLocalServiceIndexEntryId;
        entry->ChangeL( tempEntry );
        }

    CleanupStack::PopAndDestroy();// entry
    
    TSmsProgressBuf progress;
    Session().ServiceProgress( SmsClientMtm().ServiceId(), progress ); //ignore error

    TBuf8<1> dummy;
    dummy.Zero();
    SMUMLOGGER_LEAVEFN(" CSmsMtmUi::CancelL");
    return CBaseMtmUi::InvokeAsyncFunctionL( ESmsMtmCommandDeleteSchedule, aSelection, aStatus, dummy );
    }

// ---------------------------------------------------------
// CSmsMtmUi::DeleteFromL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::DeleteFromL( const CMsvEntrySelection& /*aSelection*/, TRequestStatus& /*aStatus*/ )
    {
    SMUMLOGGER_WRITE(" CSmsMtmUi::DeleteFromL");
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CSmsMtmUi::DeleteServiceL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::DeleteServiceL( const TMsvEntry& /*aService*/, TRequestStatus& /*aStatus*/ )
    {
    SMUMLOGGER_WRITE(" CSmsMtmUi::DeleteServiceL");
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CSmsMtmUi::ReplyL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::ReplyL( 
    TMsvId aDestination, 
    TMsvPartList /*aPartlist*/, 
    TRequestStatus& aCompletionStatus )
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUi::ReplyL");
    CheckAndEnsureSmsServiceOkL( ETrue, ETrue, ETrue );

    // Set params
    TEditorParameters editorParams;
    editorParams.iFlags |= EMsgReplyToMessageSender;
    
    editorParams.iId = BaseMtm().Entry().EntryId();
    editorParams.iDestinationFolderId = aDestination; 
	editorParams.iFlags &=~ ( EMtmUiFlagEditorPreferEmbedded | EMtmUiFlagEditorNoWaitForExit );
	editorParams.iFlags |= ( Preferences() & EMtmUiFlagEditorPreferEmbedded ? EMsgLaunchEditorEmbedded : 0 );
	editorParams.iFlags |= ( Preferences() & EMtmUiFlagEditorNoWaitForExit ? 0 : EMsgLaunchEditorThenWait );

    SMUMLOGGER_LEAVEFN(" CSmsMtmUi::ReplyL");
    return MsgEditorLauncher::LaunchEditorApplicationL(Session(), Type(), aCompletionStatus, editorParams, KNullDesC /*, paramPack*/);
    }

// ---------------------------------------------------------
// CSmsMtmUi::ForwardL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::ForwardL(
    TMsvId aDestination, 
    TMsvPartList /*aPartList*/, 
    TRequestStatus& aCompletionStatus )
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUi::ForwardL");
    CheckAndEnsureSmsServiceOkL( ETrue, ETrue, ETrue );

    // Set params
    TEditorParameters editorParams;
    editorParams.iFlags |= EMsgForwardMessage;
    
    editorParams.iId = BaseMtm().Entry().EntryId();
	editorParams.iFlags &=~ ( EMtmUiFlagEditorPreferEmbedded | EMtmUiFlagEditorNoWaitForExit );
	editorParams.iFlags |= ( Preferences() & EMtmUiFlagEditorPreferEmbedded ? EMsgLaunchEditorEmbedded : 0 );
	editorParams.iFlags |= (Preferences() & EMtmUiFlagEditorNoWaitForExit ? 0 : EMsgLaunchEditorThenWait );
    editorParams.iDestinationFolderId = aDestination;
    
    SMUMLOGGER_LEAVEFN(" CSmsMtmUi::ForwardL");
    return MsgEditorLauncher::LaunchEditorApplicationL(Session(), Type(), aCompletionStatus, editorParams, KNullDesC /*, paramPack*/);
    }

//    In the following two functions, the user has chosen to move or copy the message
//    to a service i.e. send it.
//

// ---------------------------------------------------------
// CSmsMtmUi::CopyToL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::CopyToL( const CMsvEntrySelection& aSelection, TRequestStatus& aStatus )
    {
    return CopyMoveToL( aSelection, aStatus, ETrue );
    }

// ---------------------------------------------------------
// CSmsMtmUi::MoveToL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::MoveToL( const CMsvEntrySelection& aSelection, TRequestStatus& aStatus )
    {
    return CopyMoveToL( aSelection, aStatus, EFalse );
    }

// ---------------------------------------------------------
// CSmsMtmUi::CopyMoveToL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::CopyMoveToL(
    const CMsvEntrySelection& aSelection, 
    TRequestStatus& aStatus, 
    TBool aCopyOnly )
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUi::CopyMoveToL");
    __ASSERT_DEBUG(
        BaseMtm().Entry().Entry().iMtm == Type(), Panic( ESmsuWrongMtmType ));
    __ASSERT_DEBUG(
        BaseMtm().Entry().Entry().iType == KUidMsvServiceEntry, 
        Panic( ESmsuNotAServiceEntry ));
    
  	TInt count = aSelection.Count();
    __ASSERT_DEBUG( count > 0 , Panic( ESmsuEmptySelection ));

    // --- Carry out the operation ---
    TMsvEntry smsServiceEntry( BaseMtm().Entry().Entry());
    CMsvEntry* parentEntry= Session().GetEntryL( aSelection[count-1] );
    CleanupStack::PushL( parentEntry );

	//Must set the sending state of Suspended messages to Waiting, otherwise they will not send
	while ( count-- )
		{
		parentEntry->SetEntryL( aSelection[count] );
		TMsvEntry entry( parentEntry->Entry());
		switch ( entry.SendingState())
            {
            case KMsvSendStateSuspended:
            case KMsvSendStateUponRequest:
            case KMsvSendStateScheduled:
            case KMsvSendStateResend:
            case KMsvSendStateFailed:
			    entry.SetSendingState( KMsvSendStateWaiting );
			    parentEntry->ChangeL( entry );
                break;
            default:
                break;
            }
		}

    parentEntry->SetEntryL( parentEntry->Entry().Parent());
    
    //    Do the copy/move
    CMsvOperation* op= aCopyOnly ? 
        parentEntry->CopyL( aSelection, smsServiceEntry.Id(), aStatus ) : 
        parentEntry->MoveL( aSelection, smsServiceEntry.Id(), aStatus );
    
    CleanupStack::PopAndDestroy();    //parentEntry
    SMUMLOGGER_LEAVEFN(" CSmsMtmUi::CopyMoveToL");
    return op;
    }

//    Copy/move from commands return KErrNotSupported because SMS messages are 
//    automatically placed in the inbox after being received. Should therefore 
//    be no need for this kind of thing.

// ---------------------------------------------------------
// CSmsMtmUi::CopyFromL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::CopyFromL( 
    const CMsvEntrySelection& /*aSelection*/, 
    TMsvId /*aTargetId*/, 
    TRequestStatus& /*aStatus*/ )
    {
    SMUMLOGGER_WRITE(" CSmsMtmUi::CopyFromL");
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CSmsMtmUi::MoveFromL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::MoveFromL(
    const CMsvEntrySelection& /*aSelection*/, 
    TMsvId /*aTargetId*/, 
    TRequestStatus& /*aStatus*/ )
    {
    SMUMLOGGER_WRITE(" CSmsMtmUi::MoveFromL");
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CSmsMtmUi::QueryCapability
//
// ---------------------------------------------------------
TInt CSmsMtmUi::QueryCapability( TUid aCapability, TInt& aResponse )
    {
    SMUMLOGGER_WRITE(" CSmsMtmUi::QueryCapability");
    switch ( aCapability.iUid )
        {
        case KUidMsvMtmQuerySupportValidateService:
        case KUidMsvMtmUiQueryMessagingInitialisation:
        case KUidMsvMtmQueryFactorySettings:
            {
            aResponse=ETrue;
            return KErrNone;
            }
        default:
            return CBaseMtmUi::QueryCapability(aCapability, aResponse);
        }
    }

// ---------------------------------------------------------
// CSmsMtmUi::InvokeSyncFunctionL
//
// ---------------------------------------------------------
void CSmsMtmUi::InvokeSyncFunctionL(
    TInt aFunctionId, 
    const CMsvEntrySelection& aSelection, 
    TDes8& aParameter )
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUi::InvokeSyncFunctionL");
    switch ( aFunctionId )
        {
        case KMtmUiFunctionMessageInfo:
            {
            ShowMessageInfoL( );
            break;
            }
        case KMtmUiFunctionValidateService:
            {
            CheckAndEnsureSmsServiceOkL( ETrue, ETrue, ETrue );     
            
            // Suppose everything is ok, because function above didn't Leave.
            // return the result via aParameter as a TInt packaged into a TDes8
            TPckgBuf<TInt> resultPackage( KErrNone );
            aParameter.Copy( resultPackage );
            break;
            }
        case KMtmUiFunctionSimDialog:
            {
            TInt simState = CheckSimStateL();
			TInt phoneOnOff( ESAPhoneOn );
			TInt err = RProperty::Get( KUidSystemCategory, KUidPhonePwrValue, phoneOnOff );
            if( simState == ESimNotPresent ) // Sim removed
                {
                // Show note and exit 
                HBufC* noteString = StringLoader::LoadLC( R_QTN_SU_NOTE_INSERT_SIM, iCoeEnv );
                CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
                note->ExecuteLD( *noteString );
                CleanupStack::PopAndDestroy(); // noteString
                }
            else if( err || phoneOnOff == ESAPhoneOff )    
            	{            	
            	// Show note and exit 
                HBufC* noteString = StringLoader::LoadLC( R_QTN_OFFLINE_NOT_POSSIBLE, iCoeEnv );
                CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );                
                note->ExecuteLD( *noteString );
                CleanupStack::PopAndDestroy(); // noteString                
            	}            	
            else // Sim is present
                {
                // Check if the Sim access profile is on
                TInt btSapState( EBTSapNotConnected );
			    TInt err = RProperty::Get( 
			    	KPSUidBluetoothSapConnectionState, 
			        KBTSapConnectionState, 
			        btSapState );
			    if ( err )
			        { // Reset 
			        btSapState = EBTSapNotConnected;
			        }
			    if( EBTSapNotConnected == btSapState  ) // Normal case
			    	{
			    	CheckAndEnsureSmsServiceOkL( ETrue, ETrue, ETrue );
					//launch sim-dialog
	                iSimDlgPluginIntf->LaunchL();
			    	}
                else // Sim Access Profile is on
	                {
	                // Show note and exit 
	                HBufC* noteString = StringLoader::LoadLC( R_QTN_OFFLINE_NOT_POSSIBLE_SAP, iCoeEnv );
	                CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
	                note->ExecuteLD( *noteString );
	                CleanupStack::PopAndDestroy(); // noteString
	                }
                }
            break;
            }
        case KMtmUiMessagingInitialisation:
        case KMtmUiFunctionRestoreFactorySettings: 
            {
            TMsvId serviceEntryId = KMsvNullIndexEntryId;
            serviceEntryId = CreateSmsServiceL( EFalse );
            __ASSERT_DEBUG( serviceEntryId, Panic( ESmsuNotAServiceEntry ));
            CreateDefaultSettingsL( serviceEntryId, ( aFunctionId == KMtmUiMessagingInitialisation ) );
            break;
            }
        default:
            CBaseMtmUi::InvokeSyncFunctionL( aFunctionId, aSelection, aParameter );
            SMUMLOGGER_LEAVEFN("CSmsMtmUi::InvokeSyncFunctionL- DEFAULT");
            return;
        }
    SMUMLOGGER_LEAVEFN("CSmsMtmUi::InvokeSyncFunctionL");
    }

// ---------------------------------------------------------
// CSmsMtmUi::InvokeAsyncFunctionL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::InvokeAsyncFunctionL(
    TInt aFunctionId, 
    const CMsvEntrySelection& aSelection, 
    TRequestStatus& aCompletionStatus, 
    TDes8& aParameter )
    {
    SMUMLOGGER_WRITE("CSmsMtmUi::InvokeAsyncFunctionL");

    //    Invoke asynchronous functions supported by client     
    switch ( aFunctionId )
        {
        case KMtmUiFunctionMessageInfo:
            {
            return ShowMessageInfoL( aCompletionStatus, aParameter );
            }
        default:
            {
            return CBaseMtmUi::InvokeAsyncFunctionL( aFunctionId, aSelection, aCompletionStatus, aParameter );
            }
        }
    }

// ---------------------------------------------------------
// CSmsMtmUi::DisplayProgressSummary
//
// ---------------------------------------------------------
TInt CSmsMtmUi::DisplayProgressSummary( const TDesC8& aProgress ) const
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUi::DisplayProgressSummary");
    
    // Check are we surely dealing with sms progress    
    const TInt progressSize = aProgress.Size();

    if ( progressSize == sizeof( TSmsSmumProgress ))
	    {// yes, smum-progress
	    TSmsSmumProgressBuf smumProgressPack;
	    smumProgressPack.Copy( aProgress );
	    TSmsSmumProgress smumProgress( smumProgressPack());

	    // Show "not supported"-notes if msg cannot be shown
    	if ( smumProgress.iError == KErrNotSupported && 
            smumProgress.iType == TSmsSmumProgress::ESmsSmumSmsOpening )
            {
            TRAP_IGNORE
                (
                HBufC* string = StringLoader::LoadLC( R_QTN_MCE_INFO_MESSAGE_NOT_OPENED, iCoeEnv );
                CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
                note->ExecuteLD( *string );
                CleanupStack::PopAndDestroy(); // string
                )
            }         
        }
    else if ( progressSize == sizeof( TSmsProgress ))
        {// yes, sms-progress
        TSmsProgressBuf progressPack;
        progressPack.Copy( aProgress );
        TSmsProgress progress ( progressPack());
        
        if ( progress.iError && ( progress.iType != TSmsProgress::ESmsProgressTypeSending ))
            {
            TRAP_IGNORE( 
                const TDesC& errorTxt = iErrorResolver->ResolveErrorString( progress.iError );
                CAknErrorNote* note = new ( ELeave ) CAknErrorNote();
                note->ExecuteLD( errorTxt ); );
            }
        }
    else
        {// not sms progress, don't care
        return KErrCancel;
        }
    
    SMUMLOGGER_LEAVEFN("CSmsMtmUi::DisplayProgressSummary - no err");
    return KErrNone;
    }

// ---------------------------------------------------------
// CSmsMtmUi::LaunchSettingsDialogL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::LaunchSettingsDialogL( TRequestStatus& aStatus )
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUi::LaunchSettingsDialogL");
    __ASSERT_DEBUG(
        BaseMtm().Entry().Entry().iMtm == Type(), Panic( ESmsuWrongMtmType ));
    __ASSERT_DEBUG(
        BaseMtm().Entry().Entry().iType.iUid == 
        KUidMsvServiceEntryValue, Panic( ESmsuNotAServiceEntry ));
    TInt exitCode = ESmumSmsSettingsInitialvalue;
    TInt err = KErrNone;
    // load settings
    CSmsSettings* smsSettings = CSmsSettings::NewLC();
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
	TRAPD( error, smsAccount->LoadSettingsL( *smsSettings ) );
	if ( error == KErrNone )
		{
		//    Launch the service settings dialog on the current entry
        TInt notUsed( 0 );
        CSmumMainSettingsDialog* mainSettingsDialog = CSmumMainSettingsDialog::NewL(
            *smsSettings, ESmumMainSmsSettings, exitCode, notUsed );
        mainSettingsDialog->ExecuteLD( R_MAIN_SETTINGS_DIALOG );
        // Try to save, continue anyway
        TRAP( err, smsAccount->SaveSettingsL( *smsSettings ) );
		}
	else
    	{
    	User::Leave( error );
    	}
    CleanupStack::PopAndDestroy( 2 ); // smsSettings, smsAccount
    // Check if user wants to exit to idle = ESmumSmsSettingsMenuExit
    // (...in system exits system keeps closing apps one by one until all down, not done here...)
    if ( exitCode == ESmumSmsSettingsMenuExit )
            { // Exiting...
            err = EEikCmdExit;
            }

    SMUMLOGGER_LEAVEFN("CSmsMtmUi::LaunchSettingsDialogL");
    
    return CMsvCompletedOperation::NewL(
        Session(), 
        Type(), 
        KNullDesC8, 
        KMsvLocalServiceIndexEntryId, 
        aStatus, 
        err);
    }

// ---------------------------------------------------------
// CSmsMtmUi::LaunchEditorApplicationL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::LaunchEditorApplicationL(
    TRequestStatus& aStatus, 
    CMsvSession& aSession, 
    TEditorType aEditorType, 
    TMsvId aParentId )
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUi::LaunchEditorApplicationL");

    TEditorParameters temp;    
  	temp.iFlags &=~ ( EMtmUiFlagEditorPreferEmbedded | EMtmUiFlagEditorNoWaitForExit );
	temp.iFlags |= ( Preferences() & EMtmUiFlagEditorPreferEmbedded ? EMsgLaunchEditorEmbedded : 0 );
    temp.iFlags |= ( Preferences() & EMtmUiFlagEditorNoWaitForExit ? 0 : EMsgLaunchEditorThenWait );

    if ( iBaseMtm.HasContext())
        {
        temp.iId = iBaseMtm.Entry().EntryId();
        }

    switch( aEditorType )
        {
        case EReadOnly:
            temp.iFlags |= EMsgReadOnly;
            break;                             
        case ECreateNewMessage:
            temp.iFlags |= EMsgCreateNewMessage ;
            temp.iId = aParentId;
            break;
        case EEditExisting:
        default:
            break;
        }
    SMUMLOGGER_LEAVEFN("CSmsMtmUi::LaunchEditorApplicationL");
       
	return MsgEditorLauncher::LaunchEditorApplicationL( 
        aSession, 
        Type(), 
        aStatus, 
        temp, 
        KNullDesC, 
        KNullDesC8 );
    }

// ---------------------------------------------------------
// CSmsMtmUi::GetResourceFileName
//
// ---------------------------------------------------------
void CSmsMtmUi::GetResourceFileName( TFileName& aFileName ) const
    {
    aFileName = KSmsuResourceFile;
    }

// ---------------------------------------------------------
// CSmsMtmUi::CheckEntry
//
// ---------------------------------------------------------
TBool CSmsMtmUi::CheckEntry( const TMsvEntry& aEntry ) const
    {
    //    Ensures that the TMsvEntry is a suitable candidate for treatment by
    //    this MtmUi layer.
    return aEntry.iType == KUidMsvMessageEntry && aEntry.iMtm == Type();
    }

// ---------------------------------------------------------
// CSmsMtmUi::CreateSmsServiceL
//
// ---------------------------------------------------------
TMsvId CSmsMtmUi::CreateSmsServiceL( TBool aNoServiceNoCheckNeeded )
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUi::CreateSmsServiceL");
    // Keep shared data uptodate
    SetSharedDataL( 
        KSmumSmsStatus, 
        1 );// sms initialisation in progress

    // Check for service if needed
    TMsvId serviceEntryId = KMsvNullIndexEntryId;
    TInt err = KErrNone;
    if ( !aNoServiceNoCheckNeeded )
        {
        TRAP( err, serviceEntryId = ServiceIdL());
        }

    // If no service, create one
    if ( err == KErrNotFound || aNoServiceNoCheckNeeded )
        {
	    TMsvEntry entry;
	    entry.iMtm = KUidMsgTypeSMS;
	    entry.iType = KUidMsvServiceEntry;
	    entry.SetReadOnly( EFalse );
        entry.SetVisible( EFalse );
        entry.iDate.HomeTime();
        entry.iDetails.Set( KSmumSmsService );
        CMsvEntry* root = Session().GetEntryL( KMsvRootIndexEntryId );
        CleanupStack::PushL( root );
        // In case no root store, create one...
        if ( !root->HasStoreL() )
            {
            // --- The entry does not have a store. EditStoreL() will create one ---
            CMsvStore* store = root->EditStoreL();
            CleanupStack::PushL( store );
            store->CommitL();
            CleanupStack::PopAndDestroy(); // store
            store = NULL; // destroyed
            }
        root->CreateL( entry );
        CleanupStack::PopAndDestroy(); // root
        BaseMtm().SwitchCurrentEntryL( entry.Id());
        serviceEntryId = entry.Id();
        iNewSmsSettings = ETrue;
        }
    else
        {
		User::LeaveIfError( err );
        }

    SMUMLOGGER_LEAVEFN("CSmsMtmUi::CreateSmsServiceL");
    return serviceEntryId;
    }

// ---------------------------------------------------------
// CSmsMtmUi::ServiceIdL
//
// ---------------------------------------------------------
TMsvId CSmsMtmUi::ServiceIdL()
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUi::ServiceIdL");
    TMsvId id = KMsvNullIndexEntryId;
    CMsvEntry* root = Session().GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(root);
    TSmsUtilities::ServiceIdL(*root, id);
    CleanupStack::PopAndDestroy(root);

    SMUMLOGGER_LEAVEFN("CSmsMtmUi::ServiceIdL");
    return id;
    }

// ---------------------------------------------------------
// CSmsMtmUi::ReadDefaultSettingsFromSharedDataL
//
// ---------------------------------------------------------
void CSmsMtmUi::ReadDefaultSettingsFromSharedDataL( CSmsSettings* aServiceSettings )
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUi::ReadDefaultSettingsFromSharedDataL");
    if ( iCenRepSession )
        {
        TInt readedSetting;
        // Delivery report 
        if ( iCenRepSession->Get( KSmumDeliveryReport, readedSetting ) != KErrNone )
        	{
	        readedSetting = KDefDeliveryReport;
	        }
        aServiceSettings->SetDeliveryReport( readedSetting );

        // Validity period
        if ( iCenRepSession->Get( KSmumValidityPeriod, readedSetting ) != KErrNone )
            {
            readedSetting = KDefValidityPeriod;
            }
        aServiceSettings->SetValidityPeriod( readedSetting );

        // Message conversion
        if ( iCenRepSession->Get( KSmumMessageConversion, readedSetting ) != KErrNone )
            {
            readedSetting = KDefMessageConversion;
            }
        aServiceSettings->SetMessageConversion(( TSmsPIDConversion ) readedSetting );

        // Preferred connection
        if ( iCenRepSession->Get( KSmumPreferredConnection, readedSetting ) != KErrNone )
            {
            readedSetting = KDefPreferredConnection;
            }
        aServiceSettings->SetSmsBearer( ( CSmsSettings::TMobileSmsBearer ) readedSetting );

        // Reply via same centre 
        if(iCenRepSession->Get( KSmumRemoveReplyViaSameCentre, readedSetting ) != KErrNone )
        	{
        	if( iCenRepSession->Get( KSmumReplyViaSameCentre, readedSetting ) != KErrNone )	
            	{
        		readedSetting = KDefReplyViaSameCentre;
            	}
        	}
        else 
            {
            if(!readedSetting)
            	{
            	if( iCenRepSession->Get( KSmumReplyViaSameCentre, readedSetting ) != KErrNone )	
            		{
            		readedSetting = KDefReplyViaSameCentre;	
            		}
            	}
            }
        	
        aServiceSettings->SetReplyPath( readedSetting );
        
        }
    SMUMLOGGER_LEAVEFN("CSmsMtmUi::ReadDefaultSettingsFromSharedDataL");
    }

// ---------------------------------------------------------
// CSmsMtmUi::CopySCentresFromOldToNewSettingsL
//
// ---------------------------------------------------------
void CSmsMtmUi::CopySCentresFromOldToNewSettingsL(CSmsSettings* aServiceSettings)
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUi::CopySCentresFromOldToNewSettingsL");
    // Copy Service centres from old setting to new settings
    CSmsSettings* oldServiceSettings = &SmsClientMtm().ServiceSettings();
    TInt serviceCentres = oldServiceSettings->ServiceCenterCount();
    // Do copying, if we have something to copy
    if ( serviceCentres )
        {
        for ( TInt loop=0; loop < serviceCentres; loop++ )
            {
            aServiceSettings->AddServiceCenterL(
                oldServiceSettings->GetServiceCenter( loop ).Name(),
                oldServiceSettings->GetServiceCenter( loop ).Address());
            }
        aServiceSettings->SetDefaultServiceCenter( 
            oldServiceSettings->DefaultServiceCenter());
        }
    SMUMLOGGER_LEAVEFN("CSmsMtmUi::CopySCentresFromOldToNewSettingsL");
    }
        
// ---------------------------------------------------------
// CSmsMtmUi::CreateDefaultSettingsL
//
// ---------------------------------------------------------
void CSmsMtmUi::CreateDefaultSettingsL( TMsvId aServiceId, TBool aMailInit )
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUi::CreateDefaultSettingsL");
    CSmsSettings* serviceSettings = CSmsSettings::NewL();
    CleanupStack::PushL( serviceSettings );
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    // Read the RFS related settings from shared data.
    TInt originalCount = 0;
    if ( aMailInit )
        {
        smsAccount->LoadSettingsL( *serviceSettings );
        originalCount = serviceSettings->ServiceCenterCount();
        }

    if ( !originalCount )
        {        
        ReadDefaultSettingsFromSharedDataL( serviceSettings );

        // Rest of the sms settings, which are fixed.
        serviceSettings->SetValidityPeriodFormat( TSmsFirstOctet::ESmsVPFInteger ); //relative
        serviceSettings->SetDelivery( ESmsDeliveryImmediately );
        serviceSettings->SetCanConcatenate( ETrue );
        serviceSettings->SetStatusReportHandling( CSmsSettings::EMoveReportToInboxInvisible );
        serviceSettings->SetSpecialMessageHandling( CSmsSettings::EMoveReportToInboxVisible );
        serviceSettings->SetRejectDuplicate( ETrue );
        TInt descriptionLength = KSmsDescriptionLength;
        // Read the value for description length 
        CRepository* repository = CRepository::NewLC( KCRUidMuiuSettings );
        if (KErrNone == repository->Get(KMuiuDescriptionLength, descriptionLength))
          {
          //Make sure value is not zero
          descriptionLength = Max(descriptionLength, KSmsDescriptionLength);
          }
        CleanupStack::PopAndDestroy(); // repository
        serviceSettings->SetDescriptionLength( descriptionLength );
        // Set saving to commsdb
        serviceSettings->SetCommDbAction( CSmsSettings::EStoreToCommDb ); 
        serviceSettings->SetSmsBearerAction( CSmsSettings::EStoreToCommDb );
        
        // If we're refreshing old settings (RFS:normal level)
        // then copy the scentres from old to new.
        if ( !iNewSmsSettings )
            {
            CopySCentresFromOldToNewSettingsL( serviceSettings );
            }
        }
    else
        {
        iNewSmsSettings = EFalse;
        }
    
    // Save settings
    CMsvEntry* service = Session().GetEntryL( aServiceId );
    CleanupStack::PushL( service );
    CMsvStore* msvstore = service->EditStoreL();
    CleanupStack::PushL( msvstore );

    TInt maxTries( 5 );
    TBool done( EFalse );
    while ( maxTries && !done )
        {
        TRAPD( err, smsAccount->SaveSettingsL( *serviceSettings ) );
        if ( err == KErrNone )
            {
            done = ETrue;
            }
        else if ( err == KErrLocked )
            {
            // Some other thread has locked (probably the CommDb) repository.
            // Wait a while and retry.
            User::After( 100000 ); // 0.1 seconds
            maxTries--;
            }
        else
            {
            User::Leave( err );
            }
        }

    msvstore->CommitL();    
    CleanupStack::PopAndDestroy( 2 ); // msvstore, service
    
    // Keep shared data uptodate
    if ( iNewSmsSettings )
        { // Let's try reading SCs from SIM
        SetSharedDataL( 
            KSmumSmsStatus, 
            KInitializationReadSim ); // notify NCN to read SCs from SIM
        iNewSmsSettings = EFalse;
        }
    else
        {
        SetSharedDataL( 
            KSmumSmsStatus, 
            KInitializationReady ); // sms initialisation ready
        }
    CleanupStack::PopAndDestroy( 2 ); // serviceSettings, smsAccount
    SMUMLOGGER_LEAVEFN("CSmsMtmUi::CreateDefaultSettingsL");
    }

// ---------------------------------------------------------
// CSmsMtmUi::ShowMessageInfoL
//
// ---------------------------------------------------------
void CSmsMtmUi::ShowMessageInfoL() const
    {
    SMUMLOGGER_WRITE("CSmsMtmUi::ShowMessageInfoL");
    ConstructAndShowInfoDialogL();    
    }

// ---------------------------------------------------------
// CSmsMtmUi::ShowMessageInfoL
//
// ---------------------------------------------------------
CMsvOperation* CSmsMtmUi::ShowMessageInfoL(TRequestStatus& aCompletionStatus, TDes8& /*aParameter*/) const
    {
    SMUMLOGGER_WRITE("CSmsMtmUi::ShowMessageInfoL(...)");
    ConstructAndShowInfoDialogL();
    return CMsvCompletedOperation::NewL( 
        Session(), KUidMsvLocalServiceMtm, KNullDesC8, 
        KMsvLocalServiceIndexEntryId, aCompletionStatus, KErrNone );
    }

// ---------------------------------------------------------
// CSmsMtmUi::ConstructAndShowInfoDialogL
//
// ---------------------------------------------------------
void CSmsMtmUi::ConstructAndShowInfoDialogL() const
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUi::ConstructAndShowInfoDialogL");
    CSmsClientMtm& smsClientMtm = SmsClientMtm();
    smsClientMtm.LoadMessageL();
    CSmsHeader& smsHeader = smsClientMtm.SmsHeader();
    TMsgInfoMessageInfoData infoData;
    TBool emailMessage( EFalse );
    
    //Get info if local timestamping needed for SMS
    CRepository* cenRep = CRepository::NewLC( KCRUidMuiuVariation );
    TBool useLocalSmsTimeStamp = EFalse;
    TBool useChineseTimezoneOverriding = EFalse;
    TInt  featureFlags = 0;
     
    cenRep->Get(KMuiuSmsFeatures,  featureFlags);
 	CleanupStack::PopAndDestroy(cenRep);
 	useLocalSmsTimeStamp = (featureFlags & KSmsFeatureIdUseLocalTimeStamp);
 	useChineseTimezoneOverriding = (featureFlags & KSmsFeatureIdOverrideChineseTimeZone);
  		
 	//Make sure only one flag is enabled
 	if( useChineseTimezoneOverriding ) 	
 	    {
 	    useLocalSmsTimeStamp = EFalse;
 	    }        
	      	
    // Check if we are dealing with E-mail sms message
    // We use TP-PID as the only validation rule
    CSmsPDU& pdu = smsHeader.Message().SmsPDU();
	if( pdu.ProtocolIdentifierPresent() )
		{
		if(	pdu.PIDType() == TSmsProtocolIdentifier::ESmsPIDTelematicInterworking &&
			pdu.TelematicDeviceIndicator() == TSmsProtocolIdentifier::ESmsTelematicDevice &&
			pdu.TelematicDeviceType() == TSmsProtocolIdentifier::ESmsInternetElectronicMail )
			{
			emailMessage = ETrue;
			}
		}
    const TMsvEntry tentry = smsClientMtm.Entry().Entry();

    // Subject
    HBufC* subject = NULL;
    // Set the subject text
    GetSubjectTextL( tentry, smsClientMtm, subject, emailMessage );
    CleanupStack::PushL( subject );
    infoData.iSubject.Set( *subject );
    
    // Holds the message info text for both sms and bio messages
    HBufC* messageType = NULL; 
    if( tentry.iBioType )
        { //this is a bio message which has been created on the phone
        TInt resourceId = GetBioResourceId( tentry );
        if ( resourceId != KErrNotFound )
            {
            messageType = iEikonEnv->AllocReadResourceLC( resourceId );
            infoData.iType.Set( *messageType );
            }
        else
            {
            infoData.iType.Set( tentry.iDescription );
            }
        }
    else
        {
        //sms type
        messageType = StringLoader::LoadLC( R_QTN_SMS_MINFO_TYPE_SMS, iCoeEnv );
        infoData.iType.Set( *messageType );
        }

    //sms date and time
    TInt type = smsHeader.Type();
    TTime smsDate;    
       
    TInt notUsed = 0;
    if ( CSmsPDU::ESmsDeliver == type || CSmsPDU::ESmsStatusReport == type )
        {        
        //UTC time or local time
      	if( useLocalSmsTimeStamp )
      		{
      		smsDate = tentry.iDate;
      		}
      	else if( useChineseTimezoneOverriding )
      	    {
      	     //get Service Center address and recipient's address. Check if chinese countrycode 
      	     //is found or if national number 
            if ( (pdu.ServiceCenterAddress()).Find( KSmumChinaCountryCode ) != KErrNotFound ||
                smsHeader.FromAddress().Find( KSmumChinaCountryCode ) != KErrNotFound ||
                ( pdu.ServiceCenterAddress().Find( KSmumInternationalPrefix ) == KErrNotFound && 
                  smsHeader.FromAddress().Find( KSmumInternationalPrefix ) == KErrNotFound ) )
                {
                CSmsDeliver& deliv = smsHeader.Deliver();

                TTime smsstackSCTime; // In UTC time
                TTime origSCTime; // In local time
                TTime utcSCTime;
                
                TInt origSCQuarterHours;
                deliv.ServiceCenterTimeStamp( smsstackSCTime, origSCQuarterHours );
                        
                //Calculate orig (local) time from SMSStack's SC-time
                TInt offsetInHours = origSCQuarterHours * CSmsMessage::E15MinutesRepresentedInSeconds;
                TTimeIntervalSeconds offsetFromUTCToLocal( offsetInHours );        
                origSCTime = smsstackSCTime + offsetFromUTCToLocal;
                      
                //subtract 8 eight hours ( = Chinese timezone offset )
                TTimeIntervalSeconds chineseOffset( KSmumChineseOffset * 60 * 60 );        
                utcSCTime = origSCTime - chineseOffset;

                smsDate = utcSCTime;                        
                }
            else
             	{//make sure smsDate is not empty
      		    smsHeader.Deliver().ServiceCenterTimeStamp( smsDate, notUsed );
      		    }
      		}
      	else
      		{
      		smsHeader.Deliver().ServiceCenterTimeStamp( smsDate, notUsed );
      		}
      		       	           
        }
    else
        {
        // message entry time is in UTC time
        smsDate = tentry.iDate;
        }
    
    infoData.iDateTime = smsDate;
    //
    // Recipient info
    if ( CSmsPDU::ESmsDeliver == type )
        { // want to show 'viewer' type fields
        //sms 'from' field
        HBufC* senderNumber = smsHeader.FromAddress().AllocLC();
        HBufC* fromText = HBufC::NewLC( 
            tentry.iDetails.Length() 
            + senderNumber->Length() 
            // for the chars: '<' '>' 2 x KSmumLRMarker
            + KSmumAdditionalCharsStringLength ); 
        
        if ( !emailMessage ) // TP-PID sms
            {
	        // Do we have alias?
	        if ( senderNumber->Compare( tentry.iDetails ))
	            { // yes
	            TPtr fromTextPtr = fromText->Des();
	            fromTextPtr.Append( tentry.iDetails );
	            fromTextPtr.Append( KSmumSpace );
	            fromTextPtr.Append( KSmumLRMarker );
	            fromTextPtr.Append( KSmumCharLeftAddressIterator );
	            fromTextPtr.Append( *senderNumber );
	            fromTextPtr.Append( KSmumCharRightAddressIterator );
	            fromTextPtr.Append( KSmumLRMarker );
	            infoData.iFrom.Set( *fromText );
	            }
	        else
	            { // no
	            infoData.iFrom.Set( *senderNumber );
	            }
            }
        else
            { // TP-PID set for interworking with email
            TPtr fromTextPtr = fromText->Des();
            fromTextPtr.Append( tentry.iDetails );
            infoData.iFrom.Set( *fromText );
            }
        
        CMsgInfoMessageInfoDialog* infoDialog = CMsgInfoMessageInfoDialog::NewL();
        infoDialog->ExecuteLD( infoData, CMsgInfoMessageInfoDialog::ESmsViewer );
        
        CleanupStack::PopAndDestroy( 2 ); // senderNumber, fromText
        }
    else  
        { //editType is ETrue, want to show 'editor' type fields
        //sms 'to:' list
        HBufC* toList = TurnRecipientsArrayIntoStringLC( 
            smsHeader.Recipients(), 
            emailMessage );

        infoData.iTo.Set( *toList );

        CMsgInfoMessageInfoDialog* infoDialog = CMsgInfoMessageInfoDialog::NewL();
        if( tentry.iBioType )
            {
            infoDialog->ExecuteLD( infoData, CMsgInfoMessageInfoDialog::EBiosEditor );
            }
        else
            {
            infoDialog->ExecuteLD( infoData, CMsgInfoMessageInfoDialog::ESmsEditor );
            }

        CleanupStack::PopAndDestroy(toList);
        }
    if ( messageType )
        {
        CleanupStack::PopAndDestroy( messageType );
        }
    if ( subject )
        {
        CleanupStack::PopAndDestroy( subject );
        }
    SMUMLOGGER_LEAVEFN("CSmsMtmUi::ConstructAndShowInfoDialogL");
    }

// ---------------------------------------------------------
// CSmsMtmUi::ExtractTitleFromGMSBodyL
//
// ---------------------------------------------------------
HBufC* CSmsMtmUi::ExtractTitleFromGMSBodyL( const CRichText& aBody, TInt aMaxLength ) const
    {           
    SMUMLOGGER_ENTERFN("CSmsMtmUi::ExtractTitleFromGMSBodyL");
    CGmsModel* model = CGmsModel::NewL(CCoeEnv::Static()->FsSession(), KErrCorrupt);
    CleanupStack::PushL(model);
    model->ImportGMSL(aBody);
    HBufC* text = model->TextAsHBufC16LC();
    HBufC* result = NULL;
    if (text->Length() <= aMaxLength)
        {
        result = text->AllocL();
        }
    else
        {
        // length is greater than max
        result = text->Left(aMaxLength).AllocL();
        }
    CleanupStack::PopAndDestroy(2, model); // text model
    SMUMLOGGER_LEAVEFN("CSmsMtmUi::ExtractTitleFromGMSBodyL");
    return result;
    }

// ---------------------------------------------------------
// CSmsMtmUi::TurnRecipientsArrayIntoStringLC
//
// ---------------------------------------------------------
HBufC* CSmsMtmUi::TurnRecipientsArrayIntoStringLC( 
    const CArrayPtrFlat<CSmsNumber>& aRecip, 
    const TBool aEmailMessage ) const
    {
    //calc length of string needed
    SMUMLOGGER_ENTERFN("CSmsMtmUi::TurnRecipientsArrayIntoStringLC");
    TInt stringLength = 0;
    TInt index;
    HBufC* toList = NULL;
    if ( !aEmailMessage )
        {
        TInt count = aRecip.Count();
        for( index = 0; index < count; index++ )
            {
            stringLength += aRecip[index]->Name().Length();
            stringLength += aRecip[index]->Address().Length();
            // for the 7 chars: '<' '>' ', ' 2 x KSmumLRMarker, ' '
            stringLength += KSmumAdditionalCharsStringLength; 
            }
        //construct string
        toList = HBufC::NewLC( stringLength );
        TPtr toListPtr = toList->Des();
        // Loop through recipients
        for( index=0; index < count; index++ )
            {
            TPtrC name = aRecip[index]->Name();
            TPtrC number = aRecip[index]->Address();
            if( name.Length() )
                {
                toListPtr.Append( name );
                toListPtr.Append( KSmumSpace );
                toListPtr.Append( KSmumLRMarker );
                toListPtr.Append( KSmumCharLeftAddressIterator );
                toListPtr.Append( number );
                toListPtr.Append( KSmumCharRightAddressIterator );
                toListPtr.Append( KSmumLRMarker );
                }
            else
                {
                toListPtr.Append( number );
                }
        
            if ( index < ( count - 1 )) // comma needed?
                {
                toListPtr.Append( KSmumCharCommaAndSpace );
                }
            }
        }
    else // Loop the Email addresses
        {
        CSmsHeader& smsHeader = SmsClientMtm().SmsHeader();
        const MDesCArray& array = smsHeader.EmailFields().Addresses();
        TInt emailAddressCount = array.MdcaCount();
        for( index = 0; index < emailAddressCount; index++ )
            {
            stringLength += array.MdcaPoint(index).Length() + 2;
            }
        toList = HBufC::NewLC( stringLength );
        TPtr toListPtr = toList->Des();
        for( index = 0; index < emailAddressCount; index++ )
            {
            toListPtr.Append( array.MdcaPoint(index) );
            if ( index < ( emailAddressCount - 1 ) ) // comma needed?
                {
                toListPtr.Append( KSmumCharCommaAndSpace );
                }
            }
        
        }
    SMUMLOGGER_LEAVEFN("CSmsMtmUi::TurnRecipientsArrayIntoStringLC");
    return toList;
    }       

// ---------------------------------------------------------
// CSmsMtmUi::SetSharedDataL
//
// ---------------------------------------------------------
void CSmsMtmUi::SetSharedDataL( const TUint32 aId, const TInt aVal )
    {
    if( iCenRepSession )    
        {
        //IMPORT_C TInt Set(TUint32 aId, TInt aVal);
        iCenRepSession->Set( aId, aVal );
        }
    }

// ---------------------------------------------------------
// CSmsMtmUi::CheckAndEnsureSmsServiceOkL
//
// ---------------------------------------------------------
TInt CSmsMtmUi::CheckAndEnsureSmsServiceOkL( TBool aCreateService, TBool aCreateStore, TBool aFetchSMSCs )
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUi::CheckAndEnsureSmsServiceOkL");
    TInt recreation = ESmsReCreatedNone;
    TMsvId currContext = iBaseMtm.Entry().Entry().Id(); // take current context to safe
    TMsvId serviceEntryId = KMsvNullIndexEntryId;
    
    // Check first do we have service entry...
    TRAPD( err, serviceEntryId = ServiceIdL());
    if ( err == KErrNotFound && aCreateService )
        { // ...no, let's create one
        serviceEntryId = CreateSmsServiceL( ETrue );
        __ASSERT_DEBUG( serviceEntryId, Panic( ESmsuNotAServiceEntry ));
        iNewSmsSettings = EFalse;
        CreateDefaultSettingsL( serviceEntryId );
        recreation = ESmsReCreatedService;
        iBaseMtm.SwitchCurrentEntryL( currContext );
        }
    else if ( err == KErrNone ) 
        {
        // If service ok, then check do we have msg store for it...
        iBaseMtm.SwitchCurrentEntryL( serviceEntryId );
        if ( !iBaseMtm.Entry().HasStoreL() && aCreateStore )
            { // ...no, let's create one
            iNewSmsSettings = EFalse;
            CreateDefaultSettingsL( serviceEntryId );
            recreation = recreation ? ESmsReCreatedBoth : ESmsReCreatedServicesStore;
            }
        iBaseMtm.SwitchCurrentEntryL( currContext );
        }
    else // Lint
        {
        }
        
    // Read SMSC from SIM, if some recreation done
    if ( ( recreation != ESmsReCreatedNone ) && aFetchSMSCs )
        {
        if ( KErrNone != ReadSMSCsFromSIML())
            {
            // Something went wrong, notify NCN to read SCs from SIM in next boot
            SetSharedDataL( 
                KSmumSmsStatus, 
                KInitializationReadSim ); 
            }
        }
    
    SMUMLOGGER_LEAVEFN("CSmsMtmUi::CheckAndEnsureSmsServiceOkL");
    return recreation;
    }

// ---------------------------------------------------------
// CSmsMtmUi::ReadSMSCsFromSIML
// ---------------------------------------------------------
TInt CSmsMtmUi::ReadSMSCsFromSIML()
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUi::ReadSMSCsFromSIML");
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    CMsgSimOperation* simOperation = CMsgSimOperation::NewL( wait->iStatus );
    CleanupStack::PushL( simOperation );
    wait->Start();
    TInt returnValue = wait->iStatus.Int();
    CleanupStack::PopAndDestroy( 2 ); // wait, simOperation
    SMUMLOGGER_LEAVEFN("CSmsMtmUi::ReadSMSCsFromSIML");
    return returnValue;
    }

// ---------------------------------------------------------
// CSmsMtmUi::CheckSimOkL
// ---------------------------------------------------------
TInt CSmsMtmUi::CheckSimStateL()
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUi::CheckSimStateL");
    TInt simState( ESimUsable );
    TInt err = RProperty::Get( KPSUidStartup, KPSSimStatus, simState );
    if ( err != KErrNone )
        {
        simState = ESimNotPresent;
        }
    SMUMLOGGER_LEAVEFN("CSmsMtmUi::CheckSimStateL");
    return simState;
    }

// ---------------------------------------------------------
// CSmsMtmUi::GetSubjectTextL
// ---------------------------------------------------------
void CSmsMtmUi::GetSubjectTextL( 
    const TMsvEntry& aTEntry, 
    const CSmsClientMtm& aSmsClientMtm,
    HBufC*& aSubject,
    TBool aIsEmailMessage /*= EFalse*/) const
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUi::GetSubjectTextL");
	// show subject only if email-over-sms
    TInt subLength = aSmsClientMtm.SmsHeader().EmailFields().Subject().Length();
    if ( !aTEntry.iBioType && aIsEmailMessage && subLength)
        { 
        // dialog truncates text to fit screen width and 
        // adds '...' so we don't need to do it here
        TBuf<KMaxSubjectLength> subjText;
        // Read the subject from CSmsHeader
        subjText = 
        	aSmsClientMtm.SmsHeader().EmailFields().Subject().Left( KMaxSubjectLength );
        subjText.TrimAll();
        aSubject = subjText.AllocL();
        }
    else
        { // Alloc as empty so it can be pushed to CleanupStack
        aSubject = KNullDesC().AllocL();
        }
    SMUMLOGGER_LEAVEFN("CSmsMtmUi::GetSubjectTextL");
    }

// ---------------------------------------------------------
// CSmsMtmUi::GetBioResourceId
// ---------------------------------------------------------
TInt CSmsMtmUi::GetBioResourceId( const TMsvEntry& aTEntry ) const
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUi::GetBioResourceId");
    TInt resourceId( KErrNotFound );
    if ( aTEntry.iBioType == KMsgBioUidVCard.iUid )
        {
        resourceId = R_QTN_SM_INFO_BUSINESS_CARD;
        }
    else if ( aTEntry.iBioType == KMsgBioUidVCalendar.iUid )
        {
        resourceId = R_QTN_SM_INFO_CALENDAR;
        }
    else if ( aTEntry.iBioType == KMsgBioUidWmlBrSettings.iUid )
        {
        resourceId = R_QTN_SM_TITLE_CONFIGURATION;
        }
    else if ( aTEntry.iBioType == KMsgBioUidPictureMsg.iUid )
        {
        resourceId = R_QTN_SM_INFO_GMS;
        }
    else
        {
        // Don't change the resourceId
        }
    SMUMLOGGER_LEAVEFN("CSmsMtmUi::GetBioResourceId");
    return resourceId;
    }

// ---------------------------------------------------------
// CSmsMtmUi::GetBioResourceId
// ---------------------------------------------------------
TBool CSmsMtmUi::IsBioTypeSupported( const TMsvEntry& aTEntry ) const    
	{
    SMUMLOGGER_WRITE("CSmsMtmUi::IsBioTypeSupported");
	TBool retryVal( EFalse );
	 if ( aTEntry.iBioType == KMsgBioUidVCard.iUid ||
	 	  aTEntry.iBioType == KMsgBioUidVCalendar.iUid ||
	 	  aTEntry.iBioType == KMsgBioUidWmlBrSettings.iUid ||
	 	  aTEntry.iBioType == KMsgBioUidPictureMsg.iUid )
	 	{
	 	retryVal = ETrue;
	 	}
	 else
	 	{
	 	}
	 return retryVal;
	}
//  End of File
