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
* Description:   Methods for NcnMsvSessionObserverBase class.
*
*/



// INCLUDE FILES
#include    <msvids.h>                      // Entry Ids
#include    <MuiuMsvUiServiceUtilities.h>   // Messaging utilites
#include	<PSVariables.h>
#include	<mmsconst.h>					// KUidMsgSubTypeMmsAudioMsg

#include    "NcnDebug.h"
#include    "NcnMsvSessionObserverBase.h"
#include    "NcnModelBase.h"
#include    "NcnHandlerAudio.h"
#include    "CNcnMsvSessionHandler.h"

// CONSTANTS

const TInt KNcnSMSInitRetryCount  = 20;
const TTimeIntervalMicroSeconds32 KNcnSMSInitMaxSleepTime  = 2000000; // 2 seconds

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
CNcnMsvSessionObserverBase::CNcnMsvSessionObserverBase( 
    CNcnModelBase* aModel ) : 
    iModel( aModel )
    {
    // empty
    }

// Symbian OS default constructor can leave.
void CNcnMsvSessionObserverBase::ConstructL()
    {
    // add msv session handler observer
    iModel->MsvSessionHandler().AddObserverL( this );
    }

// Destructor
CNcnMsvSessionObserverBase::~CNcnMsvSessionObserverBase()
    {
    if( iModel )
        {
        // remove msv session handler observer
        iModel->MsvSessionHandler().RemoveObserver( this );
        }
    
    // deletate to session close handler
    TRAP_IGNORE( HandleMsvSessionClosedL() );
    }


// ---------------------------------------------------------
// NcnMsvSessionObserverBase::HandleMsvSessionReadyL
// ---------------------------------------------------------
//
void CNcnMsvSessionObserverBase::HandleMsvSessionReadyL(
    CMsvSession& aMsvSession )
    {
    // check if there is sms service present in the device.
    CMsvEntrySelection* sel = NULL;
    TInt err;
    TTimeIntervalMicroSeconds32 waitTime = 100000; // 0,1 s
    
    for( int i = 0; i < KNcnSMSInitRetryCount; ++i )
	    {
	    TRAP( err, sel = MsvUiServiceUtilities::GetListOfAccountsWithMTML( 
				aMsvSession, KNcnSmsUid, ETrue ) );
		NCN_RDEBUG_INT( _L("CNcnMsvSessionObserverBase::HandleMsvSessionReadyL: MsvUiServiceUtilities::GetListOfAccountsWithMTML returned %d"), err );
		if( err != KErrNone )		
			{
		    if( sel )
		    	{
		    	sel->Reset();
		    	delete sel;
		    	sel = NULL;
		    	}			
			NCN_RDEBUG_INT( _L("CNcnMsvSessionObserverBase::HandleMsvSessionReadyL: waiting #%d"), i + 1 );			
			NCN_RDEBUG_INT( _L("CNcnMsvSessionObserverBase::HandleMsvSessionReadyL: Waiting %d microseconds"), waitTime.Int() );			
			User::After( waitTime ); // CSI: 92 # We must use sleep here before we try to read sms status next time
            if( waitTime < KNcnSMSInitMaxSleepTime )
                {
			    waitTime = waitTime.Int() * 2;
                }
            else                
                {
                waitTime = KNcnSMSInitMaxSleepTime;
                }
			NCN_RDEBUG( _L("CNcnMsvSessionObserverBase::HandleMsvSessionReadyL: Waiting for timer... DONE") );			
			}
		else
			{
			// SMS status successfully read
			break; 				
			}			
	    }
	
    // set sms service status to the model
    if( sel )
	    {
		NCN_RDEBUG_INT( _L("CNcnMsvSessionObserverBase::HandleMsvSessionReadyL: %d entries found"), sel->Count() );	    
	    iModel->SmsServicePresent( sel->Count() != 0 );
	    delete sel;
	    sel = NULL;
	    }
	else 
		{
		NCN_RDEBUG( _L("CNcnMsvSessionObserverBase::HandleMsvSessionReadyL: 0 entries found") );		
		iModel->SmsServicePresent( EFalse );					
		}
    
    // get inbox folder
    iInboxFolder = 	
    	CMsvEntry::NewL( 
			aMsvSession,
			KMsvGlobalInBoxIndexEntryId,
			TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByDateReverse, ETrue ) );        
    iInboxFolder->AddObserverL( *this );     
    
    NCN_RDEBUG( _L("CNcnMsvSessionObserverBase::HandleMsvSessionReadyL: DONE") );
    }

// ---------------------------------------------------------
// NcnMsvSessionObserverBase::HandleMsvSessionClosedL
// ---------------------------------------------------------
//    
void CNcnMsvSessionObserverBase::HandleMsvSessionClosedL()
    {
    NCN_RDEBUG( _L("CNcnMsvSessionObserverBase::HandleMsvSessionClosedL >>") );
    if( iInboxFolder ) 
        {
        iInboxFolder->RemoveObserver( *this );
        delete iInboxFolder;
        iInboxFolder = NULL;
        }
    NCN_RDEBUG( _L("CNcnMsvSessionObserverBase::HandleMsvSessionClosedL <<") );
    }    
    
// End of file
