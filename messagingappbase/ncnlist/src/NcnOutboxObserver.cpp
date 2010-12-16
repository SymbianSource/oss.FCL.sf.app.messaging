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
* Description:   Implements the class
*
*/



// INCLUDE FILES
#include    <e32def.h>
#include    <msvids.h>              // Entry Ids
#include    <e32property.h>
#include    <PSVariables.h>

#include    <NcnListInternalPSKeys.h>
#include    <NcnListInternalCRKeys.h>
#include    <NcnListSDKPSKeys.h>

#include    "NcnDebug.h"
#include    "NcnOutboxSender.h"
#include    "NcnOutboxObserver.h"
#include    "NcnModelBase.h"
#include    "CNcnMsvSessionHandler.h"
#include    "CNcnMobileSignalStrengthHandler.h"
#include    <miutset.h>
#include    <smut.h>

const TUid KUidMsgTypeCmailMtmVal = {0x2001F406};
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNcnOutboxObserver::CNcnOutboxObserver
// ---------------------------------------------------------
//
CNcnOutboxObserver::CNcnOutboxObserver( CNcnModelBase* aModel ) :
    iModel( aModel )
    {
    }

// ---------------------------------------------------------
// CNcnOutboxObserver::ConstructL
// ---------------------------------------------------------
//
void CNcnOutboxObserver::ConstructL()
    {    
    NCN_RDEBUG( _L("CNcnOutboxObserver::ConstructL - Constructing signal strength handler") );
    
    // instantiate the signal strength handler
    iSignalStrengthHandler = CNcnMobileSignalStrengthHandler::NewL();    
    
    // Set observer and start observing, if supported
    if( iSignalStrengthHandler->ObservingSupported() )
        {
        iSignalStrengthHandler->SetSignalStrengthObserverL( this );
        iSignalStrengthHandler->StartObservingL();
        NCN_RDEBUG( _L("CNcnOutboxObserver::ConstructL - Signal strength observing started") );
        }
    else
        {
        NCN_RDEBUG( _L("CNcnOutboxObserver::ConstructL - Signal strength observing not supported") );
        }
    
    // add observer to msv session handler
    iModel->MsvSessionHandler().AddObserverL( this );
    }


// ---------------------------------------------------------
// CNcnOutboxObserver::NewL
// ---------------------------------------------------------
//
CNcnOutboxObserver* CNcnOutboxObserver::NewL( CNcnModelBase* aModel )
    {
    // Create the instance of the outbox observer
    CNcnOutboxObserver* self = new (ELeave) CNcnOutboxObserver( aModel );

    // Push it to stack while executing the constructor
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CNcnOutboxObserver::~CNcnOutboxObserver
// ---------------------------------------------------------
//
CNcnOutboxObserver::~CNcnOutboxObserver()
    {
    if( iModel )
        {
        // remove observer from msv session handler
        iModel->MsvSessionHandler().RemoveObserver( this );
        }
    
    delete iSignalStrengthHandler;
        
    EndSessions();
    }

// ---------------------------------------------------------
// CNcnOutboxObserver::CheckOutboxAndNotifyL
// ---------------------------------------------------------
//
void CNcnOutboxObserver::CheckOutboxAndNotifyL()
    {
    NCN_RDEBUG( _L("CNcnOutboxObserver::CheckOutboxAndNotifyL" ) );
    // Check messagecount in outbox
    TInt msgCount = iOutboxFolder->Count();
    
    CMsvEntrySelection *smtpselection = NULL ;
    CMsvEntrySelection *pop3selection = NULL;
    CMsvEntrySelection *imapselection = NULL ;
    CMsvEntrySelection *cmailselection = NULL ;

    smtpselection = iOutboxFolder ->ChildrenWithMtmL( KUidMsgTypeSMTP );
    CleanupStack::PushL( smtpselection );
    TInt smtpCount= smtpselection ->Count();
    
    pop3selection = iOutboxFolder ->ChildrenWithMtmL( KUidMsgTypePOP3 );
    CleanupStack::PushL( pop3selection );    
    TInt pop3Count= pop3selection ->Count();

    imapselection = iOutboxFolder ->ChildrenWithMtmL( KUidMsgTypeIMAP4 );
    CleanupStack::PushL( imapselection );
    TInt imapCount= imapselection ->Count();
    
    cmailselection = iOutboxFolder ->ChildrenWithMtmL( KUidMsgTypeCmailMtmVal);
    CleanupStack::PushL( cmailselection );
    TInt cmailCount= cmailselection ->Count();
    
    CleanupStack::PopAndDestroy( 4 );
    // Check messagecount in outbox
    msgCount  = msgCount - (smtpCount+pop3Count+ imapCount+cmailCount);
    // Set msg count to the CR key
    NCN_RDEBUG_INT( _L("CNcnOutboxObserver::CheckOutboxAndNotifyL SetCRInt(KNcnMessageCountInOutbox) %d" ), msgCount );
    User::LeaveIfError( iModel->SetCRInt( 
        KCRUidNcnList, KNcnMessageCountInOutbox, msgCount ) );

    if( msgCount )
        {
        // Turn indicator on, if any messages in outbox
        iModel->NotifyPublishAndSubscribe(
            KUidSystemCategory,
            KPSNcnOutboxStatus,
            ENcnDocumentsInOutbox );
        }
    else
        {
        // If no messages in outbox, remove indicator
        iModel->NotifyPublishAndSubscribe(
            KUidSystemCategory,
            KPSNcnOutboxStatus, ENcnOutboxEmpty);
        }

    }


// ---------------------------------------------------------
// CNcnOutboxObserver::CheckOutboxForSendingMessageL
// ---------------------------------------------------------
//
void CNcnOutboxObserver::CheckOutboxForSendingMessageL(CMsvSession& aMsvSession)
    {
    CMsvEntrySelection *smsSelection = NULL;
    smsSelection = iOutboxFolder ->ChildrenWithMtmL(KUidMsgTypeSMS);
    CleanupStack::PushL(smsSelection);
    
    // Get count of messages in queue
    TInt count = smsSelection->Count();

    CMsvEntry* msvEntry = aMsvSession.GetEntryL(KMsvGlobalOutBoxIndexEntryId);
    CleanupStack::PushL(msvEntry);

    // Go through all messages
    while (count--)
        {
        // Select message
        msvEntry->SetEntryL(smsSelection->At(count));
        TMsvEntry entry(msvEntry->Entry());

        if (entry.Connected())
            {
            NCN_RDEBUG_INT2(_L("MarkFailed: entry was in send/failed or suspended state: state %d, error %d" ), entry.SendingState(), entry.iError );
            entry.SetConnected(EFalse);
			TInt err = KErrNone;
            TRAP(err, msvEntry->ChangeL(entry));
            NCN_RDEBUG_INT2(_L("MarkFailed: entry Changed with error state %d, error %d" ), entry.SendingState(), err );			
            }
        }
    CleanupStack::PopAndDestroy(2);
    }

// ---------------------------------------------------------
// CNcnOutboxObserver::SignalStrengthUpdatedL
// ---------------------------------------------------------
//
void CNcnOutboxObserver::SignalStrengthAndBarUpdatedL( TInt /*aNewSignalValue*/, TInt aNewBarValue )
    {
    NCN_RDEBUG_INT( _L("CNcnOutboxObserver::SignalStrengthUpdatedL - Signal bars %d"), aNewBarValue );
    
    // pass new bar value to outbox sender
    InformOutboxSenderL( aNewBarValue );
    }
    
// ---------------------------------------------------------
// CNcnOutboxObserver::InformOutboxSenderL
// ---------------------------------------------------------
//
void CNcnOutboxObserver::InformOutboxSenderL( const TInt& aNetworkBars )
    {
    if( iOutboxSender )
        {
        NCN_RDEBUG( _L("CNcnOutboxObserver::InformOutboxSenderL - Informing outbox sender.") );
        
        iOutboxSender->CheckAndStartSendingL( aNetworkBars );
        }
    else
        {
        NCN_RDEBUG( _L("CNcnOutboxObserver::InformOutboxSenderL - Outbox sender not initialized." ) );
        }
    }

// ---------------------------------------------------------
// CNcnOutboxObserver::StartSessionsL
// ---------------------------------------------------------
//
void CNcnOutboxObserver::StartSessionsL( CMsvSession& aMsvSession )
    {    
    // Set outbox folder
    iOutboxFolder = aMsvSession.GetEntryL( KMsvGlobalOutBoxIndexEntryId );
    iOutboxFolder->AddObserverL( *this );
    
    // Remove the Sender, if it exists
    delete iOutboxSender;
    iOutboxSender = NULL;

    // Check if feature is supported
    if( iModel->IsSupported( KNcnOfflineSupport ) )
        {
        NCN_RDEBUG( _L("CNcnOutboxObserver::StartSessionsL - Initializing outbox sender.." ) );
        iOutboxSender = CNcnOutboxSender::NewL( iModel, aMsvSession );

        NCN_RDEBUG( _L("CNcnOutboxObserver::StartSessionsL - Informing outbox sender of network status.." ) );
        
        // get current signal bar value
        TInt signalBars = 0;                
        TRAPD( err, signalBars = iSignalStrengthHandler->BarValueL() );
        
        // check error
        if( err == KErrNone )
            {
            NCN_RDEBUG_INT( _L( "CNcnOutboxObserver::StartSessionsL - Got signal bar value %d, notifying.."), signalBars );
            iOutboxSender->CheckAndStartSendingL( signalBars );
            }
        else
            {
            NCN_RDEBUG_INT( _L("CNcnOutboxObserver::StartSessionsL - Failed to get signal bar value (err = %d)"), err );
            }
        }
        
    // Check if there are messages in the OutBox.
    CheckOutboxAndNotifyL();
    if (iModel->CheckIfBootPhase())
        {
        CheckOutboxForSendingMessageL(aMsvSession);
        }
    }

// ---------------------------------------------------------
// CNcnOutboxObserver::EndSessions
// ---------------------------------------------------------
//
void CNcnOutboxObserver::EndSessions()
    {
    // Delete Outbox sender
    delete iOutboxSender;
    iOutboxSender = NULL;   
    
    if( iOutboxFolder )
        {
        // Delete outbox folder
        iOutboxFolder->RemoveObserver( *this );
        delete iOutboxFolder;
        iOutboxFolder = NULL;
        }    
    }

// ---------------------------------------------------------
// CNcnOutboxObserver::HandleMsvSessionReadyL
// ---------------------------------------------------------
//    
void CNcnOutboxObserver::HandleMsvSessionReadyL( CMsvSession& aMsvSession )
    {
    StartSessionsL( aMsvSession );
    }
// ---------------------------------------------------------
// CNcnOutboxObserver::HandleMsvSessionClosedL
// ---------------------------------------------------------
//    
void CNcnOutboxObserver::HandleMsvSessionClosedL()
    {
    EndSessions();
    }

// ---------------------------------------------------------
// CNcnOutboxObserver::HandleEntryEventL
// ---------------------------------------------------------
//
void CNcnOutboxObserver::HandleEntryEventL( 
    TMsvEntryEvent  /*aEvent*/, 
    TAny*           /*aArg1*/, 
    TAny*           /*aArg2*/, 
    TAny*           /*aArg3*/ )
    {
    // Count objects in folders for every event
    CheckOutboxAndNotifyL();
    }

//  End of File
