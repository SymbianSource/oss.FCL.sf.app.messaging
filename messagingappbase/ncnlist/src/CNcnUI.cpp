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
* Description:   Methods for CNcnUI class.
*
*/



// INCLUDE FILES
#include    "CNcnUI.h"
#include    "NcnModelBase.h"
#include    "NcnHandlerAudio.h"
#include    "CNcnClass0Viewer.h"
#include    "NcnSNNotifier.h"
#include    "NcnSubscriber.h"

#include    <f32file.h>                             // For TParse
#include    <data_caging_path_literals.hrh>         // For literals
#include    <stringresourcereader.h>                // For CStringResourceReader
#include    <StringLoader.h>                        // For StringLoader
#include    <AknUtils.h>                            // For AknTextUtils
#include    <AknGlobalNote.h>                       // For CAknGlobalNote
#include    <viewcli.h>                             // For CVwsSessionWrapper
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include    <viewclipartner.h>
#endif
#include    <e32property.h>                         // For RProperty
#include    <BTSapDomainPSKeys.h>                   // For KPSUidBluetoothSapConnectionState
#include    <activeidle2domainpskeys.h>
#include    <coreapplicationuisdomainpskeys.h>

#include    <Ncnlist.rsg>                           // New Contacts Note resource

// ================= LOCAL CONSTANTS =======================
namespace
    {
    _LIT( KNcnResourceFileName,"z:ncnlist.rsc" );
    
    const TInt KNcnDeliveryResourceBufLen = 128;
    const TInt KNcnDeliveryTextBufLen = 256;
    
    const TUid KMCEAppUid = { 0x100058C5 };
    const TUid KMCEViewUid = { 0x02 };
    }

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
//  CNcnUI::CNcnUI
// ----------------------------------------------------
//
CNcnUI::CNcnUI( CNcnModelBase& aModel ) :
    iNcnModel( aModel ),
    iIdleState( EFalse ),
    iBootPhase( EFalse ),
    iSAPConnected( EFalse )
    //iVisualNotificationsAllowed( ETrue ),
    //iAudioNotificationsAllowed( ETrue )
    {
    // empty
    }

// ----------------------------------------------------
//  CNcnUI::ConstructL
// ----------------------------------------------------
//
void CNcnUI::ConstructL()
    {
    iAudioHandler = CNcnHandlerAudio::NewL( &iNcnModel );
    iClass0Viewer = CNcnClass0Viewer::NewL( iNcnModel.MsvSessionHandler() );
    iNotifier = CNcnSNNotifier::NewL();
        
    // initialize and subscribe SAP connection subscriber
    iSAPSubscriber = CNcnSubscriber::NewL( *this,
        KPSUidBluetoothSapConnectionState,
        KBTSapConnectionState );
    iSAPSubscriber->Subscribe();
    
    // set SAP connection status
    iSAPConnected = SAPConnected();
    }
    
// ----------------------------------------------------
//  CNcnUI::NewL
// ----------------------------------------------------
//
CNcnUI* CNcnUI::NewL( CNcnModelBase& aModel )
    {
    CNcnUI* self = new (ELeave) CNcnUI( aModel );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ----------------------------------------------------
//  CNcnUI::~CNcnUI
// ----------------------------------------------------
//
CNcnUI::~CNcnUI()
    {
    delete iSAPSubscriber;
    delete iNotifier;
    delete iClass0Viewer;
    delete iAudioHandler;    
    }

// ----------------------------------------------------
//  CNcnUI::ShowMessageL
// ----------------------------------------------------
//    
void CNcnUI::ShowMessageL( const TMsvEntry& aEntry )
    {
    // use class0 viewer for immidiate display
    iClass0Viewer->ShowClass0SmsL( aEntry );
    }

// ----------------------------------------------------
//  CNcnUI::OpenMessageL
// ----------------------------------------------------
//    
void CNcnUI::OpenMessageL( const TMsvEntry& aEntry )
    {
    TVwsViewId messagingCentreInboxView( KMCEAppUid, KMCEViewUid );
    TUid uid = TUid::Uid( aEntry.Id() );
    
    // Create view wrapper object and launch MCE application
    CVwsSessionWrapper* viewCli = CVwsSessionWrapper::NewLC();    
    viewCli->CreateActivateViewEvent( 
        messagingCentreInboxView, uid, KNullDesC8() );
    CleanupStack::PopAndDestroy( viewCli );
    
    iNcnModel.NotifyPublishAndSubscribe(
        KPSUidAiInformation, // Idle status category
        KActiveIdleState,
        EPSAiBackground );// ActiveIdle to background
    }

// ----------------------------------------------------
//  CNcnUI::ShowDeliveryNoteL
// ----------------------------------------------------
//    
void CNcnUI::ShowDeliveryNoteL( const TMsvEntry& aEntry )
    {
	TParse *fp = new(ELeave) TParse();
	fp->Set( KNcnResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
	CleanupStack::PushL( fp );

    TFileName myFileName( fp->FullName() );
    CStringResourceReader* reader = CStringResourceReader::NewL( myFileName );
    CleanupStack::PushL( reader );

    TBuf<KNcnDeliveryResourceBufLen> deliveredText =
        reader->ReadResourceString( R_NCN_DELIVERED_NOTE_TEXT );

    TBuf<KNcnDeliveryTextBufLen> theWholeString;

    StringLoader::Format( theWholeString, deliveredText, -1, aEntry.iDetails );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
        theWholeString );

    CAknGlobalNote * note = CAknGlobalNote::NewL();

    CleanupStack::PushL( note );
    note->ShowNoteL( EAknGlobalInformationNote, theWholeString );
    CleanupStack::PopAndDestroy( 3 );
    }

// ----------------------------------------------------
//  CNcnUI::PlayMessageAlertToneL
// ----------------------------------------------------
//    
void CNcnUI::PlayMessageAlertToneL()
    {
    if( AudioNotificationsAllowed() )
        {
        iAudioHandler->PlayAlertToneL( CNcnModelBase::EIndexUnreadMessages );    
        }    
    }
    
// ----------------------------------------------------
//  CNcnUI::PlayMessageAlertTone
// ----------------------------------------------------
//    
void CNcnUI::PlayMessageAlertTone()
    {
    TRAP_IGNORE( PlayMessageAlertToneL() );
    }

// ----------------------------------------------------
//  CNcnUI::PlayEMailAlertToneL
// ----------------------------------------------------
//    
void CNcnUI::PlayEMailAlertToneL()
    {
    if( AudioNotificationsAllowed() )
        {
        iAudioHandler->PlayAlertToneL( CNcnModelBase::EIndexNewEmails );
        }    
    }
    
// ----------------------------------------------------
//  CNcnUI::PlayEMailAlertTone
// ----------------------------------------------------
//    
void CNcnUI::PlayEMailAlertTone()
    {
    TRAP_IGNORE( PlayEMailAlertToneL() );
    }

// ----------------------------------------------------
//  CNcnUI::PlayEMailAlertTone
// ----------------------------------------------------
//
void CNcnUI::AbortAlertTonePlayback()
    {
    iAudioHandler->StopMsgReceivedTonePlaying();
    }
    
// ----------------------------------------------------
//  CNcnUI::FlashDisplay
// ----------------------------------------------------
//    
void CNcnUI::FlashDisplay()
    {
    // only flash display if notifications are allowed
    if( VisualNotificationsAllowed() )
        {
        // try to set the P&S property
        TInt err = RProperty::Set(
            KPSUidCoreApplicationUIs,
            KLightsControl,
            ELightsOn );
        
        // only set it back if setting succeeded in first place
        if( err == KErrNone )
            {
            RProperty::Set(
                KPSUidCoreApplicationUIs,
                KLightsControl,
                ELightsOff );
            }
        }
    }

// ----------------------------------------------------
//  CNcnUI::SetIdleState
// ----------------------------------------------------
//    
void CNcnUI::SetIdleState( TBool aIdleState )
    {
    // update idle state if changed
    if( iIdleState != aIdleState )
        {
        iIdleState = aIdleState;
        iNotifier->IdleState( iIdleState );       
        }    
    }

// ----------------------------------------------------
//  CNcnUI::IdleState
// ----------------------------------------------------
//    
TBool CNcnUI::IdleState()
    {
    return iIdleState;
    }

// ----------------------------------------------------
//  CNcnUI::SetBootPhase
// ----------------------------------------------------
//    
void CNcnUI::SetBootPhase( TBool aBootPhase )
    {
    iBootPhase = aBootPhase;
    }
    
// ----------------------------------------------------
//  CNcnUI::BootPhase
// ----------------------------------------------------
//
TBool CNcnUI::BootPhase()
    {
    return iBootPhase;
    }
    
// ----------------------------------------------------
//  CNcnUI::UpdateSoftNotification
// ----------------------------------------------------
//    
void CNcnUI::UpdateSoftNotification(
    MNcnNotifier::TNcnNotificationType aNotification,
    TUint aAmount )
    {
    // only update notification if visual notifications are allowed
    if( VisualNotificationsAllowed() )
        {  
        // update SN notification
        iNotifier->NewItems( aNotification, aAmount );
        }
    }    
    
// ----------------------------------------------------
//  CNcnUI::VisualNotificationsAllowed
// ----------------------------------------------------
//    
TBool CNcnUI::VisualNotificationsAllowed()
    {
    // visual notifications are not allowed
    // when SAP is connected
    return !iSAPConnected;
    }
    
// ----------------------------------------------------
//  CNcnUI::AudioNotificationsAllowed
// ----------------------------------------------------
//    
TBool CNcnUI::AudioNotificationsAllowed()
    {
    // audio notifications are not allowed when SAP
    // is connected or during boot phase
    return ( !iSAPConnected && !iBootPhase );
    }   

// ----------------------------------------------------
//  CNcnUI::HandlePropertyChangedL
// ----------------------------------------------------
//    
void CNcnUI::HandlePropertyChangedL( const TUid& aCategory, TInt aKey )
    {
    // SAP connection state changed
    if( aCategory == KPSUidBluetoothSapConnectionState &&
        aKey == KBTSapConnectionState )
        {
        // update SAP connection status
        iSAPConnected = SAPConnected();
        
        //Clear notes if we have changed to connected or we are connecting
        //TSW bug ID PRIA-6HTCCK
        if(iSAPConnected == TRUE)
	        {
        	NCN_RDEBUG( _L( "CNcnUI::SAP is connected. clear all soft notifications!" ) );
			iNotifier->NewItems(MNcnNotifier::ENcnMessagesNotification,0);
			iNotifier->NewItems(MNcnNotifier::ENcnAudioMessagesNotification,0);
			iNotifier->NewItems(MNcnNotifier::ENcnEmailNotification,0);
			iNotifier->NewItems(MNcnNotifier::ENcnVoiceMailNotification,0);
			iNotifier->NewItems(MNcnNotifier::ENcnVoiceMailOnLine1Notification,0);
			iNotifier->NewItems(MNcnNotifier::ENcnVoiceMailOnLine2Notification,0);
			iNotifier->NewItems(MNcnNotifier::ENcnMissedCallsNotification,0);
	        }
        }
    }
    
// ----------------------------------------------------
//  CNcnUI::SAPConnected
// ----------------------------------------------------
//    
TBool CNcnUI::SAPConnected()
    {
    // default to false
    TBool ret = EFalse;            

    // get SAP connection state
    TInt state = 0;
    TInt err = RProperty::Get(
        KPSUidBluetoothSapConnectionState,
        KBTSapConnectionState,
        state );
    
    if( err == KErrNone )
        {
        NCN_RDEBUG_INT( _L( "CNcnUI::SAPConnected - SAP connection = %d" ), state );
        
        // if SAP is connecting or connected, return ETrue
        if( state == EBTSapConnecting || state == EBTSapConnected )
            {
            NCN_RDEBUG( _L( "CNcnUI::SAPConnected - SAP connected" ) );
            ret = ETrue;
            }
        else
            {
            NCN_RDEBUG( _L( "CNcnUI::SAPConnected - SAP not connected" ) );
            }
        }
    else
        {
        NCN_RDEBUG_INT( _L( "CNcnUI::SAPConnected - Could not read SAP connection value, err = %d" ), err );
        }
    
    return ret;
    }    

//  End of File
