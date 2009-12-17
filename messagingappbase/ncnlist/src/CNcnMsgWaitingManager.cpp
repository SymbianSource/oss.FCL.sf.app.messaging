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
* Description:   Methods for CNcnMsgWaitingManager class.
*
*/



// INCLUDE FILES
#include 	<etelmm.h>

#include	"NcnVariant.hrh"
#include    "NcnDebug.h"
#include    "NcnModelBase.h"
#include    "CNcnMsgWaitingManager.h"
#include    "CVoiceMailManager.h"

#ifdef _DEBUG

class CNcnMsgWaitingManagerObserver : 
	public CActive
	{
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aModel Reference to ncn model
         */
        static CNcnMsgWaitingManagerObserver* NewL( RMobilePhone &aMobilePhone );

        /**
         * Destructor.
         */
        virtual ~CNcnMsgWaitingManagerObserver();
                
    public: // From CActive
    
        /**
         * RunL
         */
        void RunL();

        /**
         * DoCancel
         */
        void DoCancel();
                                                                
    private:  // Constructors and destructor

        /**
         * C++ default constructor.
         * @param aMsvSessionHandler Msv session handler reference.
         */
        CNcnMsgWaitingManagerObserver( RMobilePhone &aMobilePhone );
        
        /**
         * 2nd. phase constructor.
         */
        void ConstructL( );
        
	private:
	        
		void PrintStatus();        
		        
    private: // Data
            
		TBool iChangeNotifySubscribed;            
            
		RMobilePhone &iMobilePhone;
		
        // Struct is a container for indicator flags and message counts
        RMobilePhone::TMobilePhoneMessageWaitingV1      iMsgWaiting;  
                      
		// Package for iMsgWaitingV1                      
        RMobilePhone::TMobilePhoneMessageWaitingV1Pckg  iMsgWaitingPckg;   
            
            
	}; // CNcnMsgWaitingManagerObserver
	
	
	
	
	CNcnMsgWaitingManagerObserver::CNcnMsgWaitingManagerObserver( RMobilePhone &aMobilePhone ) :
	    CActive( EPriorityNormal ),
		iChangeNotifySubscribed( EFalse ),
	    iMobilePhone( aMobilePhone ),
	    iMsgWaiting(),
	  	iMsgWaitingPckg( iMsgWaiting )
	    {
	    CActiveScheduler::Add( this );            
	    }
	
	CNcnMsgWaitingManagerObserver* CNcnMsgWaitingManagerObserver::NewL( RMobilePhone &aMobilePhone )
	    {
	    CNcnMsgWaitingManagerObserver* self = new (ELeave) CNcnMsgWaitingManagerObserver( aMobilePhone );
	    
	    CleanupStack::PushL( self );
	    self->ConstructL();
	    CleanupStack::Pop( self );
	    
	    return self;
	    }
	
	void CNcnMsgWaitingManagerObserver::ConstructL()
	    {
	    //iMobilePhone.GetIccMessageWaitingIndicators( iStatus, iMsgWaitingPckg );
	    //SetActive();
	    }
	    
	CNcnMsgWaitingManagerObserver::~CNcnMsgWaitingManagerObserver()
	    {
	    NCN_RDEBUG( _L("~CNcnMsgWaitingManager") );
	    Cancel();
	    }
	    
	    
	void CNcnMsgWaitingManagerObserver::RunL()
	    {
	    NCN_RDEBUG_INT( _L("CNcnMsgWaitingManagerObserver::RunL: iStatus = %d"), iStatus.Int() );	    
	    TInt err = iStatus.Int();
	    
	    // subscribe for notifications
	    iMobilePhone.NotifyIccMessageWaitingIndicatorsChange( iStatus, iMsgWaitingPckg );
	    SetActive();
	    
	    iChangeNotifySubscribed = ETrue; // get-request completed, DoCancel() must now cancel notify request
	    
	    if ( err == KErrNone )
	        {
	        PrintStatus();
	        }
	    }
	    
	void CNcnMsgWaitingManagerObserver::DoCancel()
	    {	    
	    if ( IsActive() )
	        {
	        if ( iChangeNotifySubscribed )
	            {
	            iMobilePhone.CancelAsyncRequest( EMobilePhoneNotifyIccMessageWaitingIndicatorsChange );  
	            }
	        else
	            {
	            iMobilePhone.CancelAsyncRequest( EMobilePhoneGetIccMessageWaitingIndicators );    
	            }
	        }
	    }
	    	    
	void CNcnMsgWaitingManagerObserver::PrintStatus()
		{
		NCN_RDEBUG_INT( _L("CNcnMsgWaitingManagerObserver::PrintStatus: Voice mail line 1 active %d"), 
			( iMsgWaiting.iDisplayStatus& RMobilePhone::KDisplayVoicemailActive ) ? ETrue : EFalse );
		NCN_RDEBUG_INT( _L("CNcnMsgWaitingManagerObserver::PrintStatus: Voice mail line 2 active %d"), 
			( iMsgWaiting.iDisplayStatus& RMobilePhone::KDisplayAuxVoicemailActive ) ? ETrue : EFalse );
		NCN_RDEBUG_INT( _L("CNcnMsgWaitingManagerObserver::PrintStatus: Fax active %d"), 
			( iMsgWaiting.iDisplayStatus& RMobilePhone::KDisplayFaxActive ) ? ETrue : EFalse );
		NCN_RDEBUG_INT( _L("CNcnMsgWaitingManagerObserver::PrintStatus: Email active %d"), 
			( iMsgWaiting.iDisplayStatus& RMobilePhone::KDisplayEmailActive ) ? ETrue : EFalse );
		NCN_RDEBUG_INT( _L("CNcnMsgWaitingManagerObserver::PrintStatus: Other active %d"), 
			( iMsgWaiting.iDisplayStatus& RMobilePhone::KDisplayOtherActive ) ? ETrue : EFalse );
		NCN_RDEBUG_INT( _L("CNcnMsgWaitingManagerObserver::PrintStatus: Display data active %d"), 
			( iMsgWaiting.iDisplayStatus& RMobilePhone::KDisplayDataActive ) ? ETrue : EFalse );

		NCN_RDEBUG_INT( _L("CNcnMsgWaitingManagerObserver::PrintStatus: Voice mail line 1 count %d"), 
			iMsgWaiting.iVoiceMsgs );
		NCN_RDEBUG_INT( _L("CNcnMsgWaitingManagerObserver::PrintStatus: Voice mail line 2 count %d"), 
			iMsgWaiting.iAuxVoiceMsgs );
		NCN_RDEBUG_INT( _L("CNcnMsgWaitingManagerObserver::PrintStatus: Fax count %d"), 
			iMsgWaiting.iFaxMsgs );
		NCN_RDEBUG_INT( _L("CNcnMsgWaitingManagerObserver::PrintStatus: Email count %d"), 
			iMsgWaiting.iEmailMsgs );
		NCN_RDEBUG_INT( _L("CNcnMsgWaitingManagerObserver::PrintStatus: Other message count %d"), 
			iMsgWaiting.iOtherMsgs );
		}	    
	    
	
#endif // _DEBUG

// ================= LOCAL CONSTANTS =======================


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
//  CNcnMsgWaitingManager::CNcnMsgWaitingManager
// ----------------------------------------------------
// 
CNcnMsgWaitingManager::CNcnMsgWaitingManager( CNcnModelBase &aModel ) :
    CActive( EPriorityNormal ),
    iModel( aModel ),    
    iMsgWaiting(),
  	iMsgWaitingPckg( iMsgWaiting )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------
//  CNcnMsgWaitingManager::NewL
// ----------------------------------------------------
//  
CNcnMsgWaitingManager* CNcnMsgWaitingManager::NewL( CNcnModelBase &aModel )
    {
    CNcnMsgWaitingManager* self = new (ELeave) CNcnMsgWaitingManager( aModel );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ----------------------------------------------------
//  CNcnMsgWaitingManager::ConstructL
// ----------------------------------------------------
//  
void CNcnMsgWaitingManager::ConstructL()
    {
    TInt err;       
    
    err = iTelServer.Connect();
    NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::ConstructL: iTelServer.Connect returned %d"), err );
	User::LeaveIfError( err );

	err = iTelServer.GetPhoneInfo( 0, iPhoneInfo );
    NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::ConstructL: iTelServer.GetPhoneInfo returned %d"), err );	
    User::LeaveIfError( err ); 
        
	err = iMobilePhone.Open( iTelServer, iPhoneInfo.iName );   
    NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::ConstructL: iMobilePhone.Open returned %d"), err );		     
    User::LeaveIfError( err );                  
                
#if 0
	iObserver = CNcnMsgWaitingManagerObserver::NewL( iMobilePhone );
#endif	                

    }

// ----------------------------------------------------
//  CNcnMsgWaitingManager::GetFirstIndicatorStatus
// ----------------------------------------------------
//  
void CNcnMsgWaitingManager::GetFirstIndicatorStatus()
    {
    Cancel();
    iState = ENcnGetIndicator;
	iStatus = KRequestPending;
	iMobilePhone.GetIccMessageWaitingIndicators( iStatus, iMsgWaitingPckg );                          
	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::GetFirstIndicatorStatus: iMobilePhone.GetIccMessageWaitingIndicators iStatus %d"), iStatus.Int() );		     	
    SetActive();  
            
    NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::GetFirstIndicatorStatus: GetIccMessageWaitingIndicators returned %d"), iStatus.Int() );

    }

// ----------------------------------------------------
//  CNcnMsgWaitingManager::~CNcnMsgWaitingManager
// ----------------------------------------------------
//    
CNcnMsgWaitingManager::~CNcnMsgWaitingManager()
    {
    Cancel();

#ifdef _DEBUG    
	if( iObserver )
		{
	    iObserver->Cancel();
	    delete iObserver;			
		}
#endif
    
    iMobilePhone.Close();
    iTelServer.Close();
    
    iReqQueue.Close();
    }

// ----------------------------------------------------
//  CNcnMsgWaitingManager::RunL
// ----------------------------------------------------
// 
void CNcnMsgWaitingManager::RunL()
    {
	TInt err = iStatus.Int();
	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::RunL: iState %d"), iState );
	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::RunL: return value %d"), err );		
    NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::RunL: there are %d requests in queue"), iReqQueue.Count() );

#ifdef _DEBUG
    NCN_RDEBUG( _L("CNcnMsgWaitingManager::RunL: Following requests are in queue:") );
    for( int i = 0; i < iReqQueue.Count(); ++i )
        {
        TNcnReqQueueElement elem = iReqQueue[0];	
        NCN_RDEBUG_INT2( _L("%d: iCmd = %d"), i, elem.iCmd );			
        NCN_RDEBUG_INT2( _L("%d: iField = %d"), i, elem.iField );			
        NCN_RDEBUG_INT2( _L("%d: iParam1 = %d"), i, elem.iParam1 );			
        NCN_RDEBUG_INT2( _L("%d: iParam2 = %d"), i, elem.iParam2 );									
        }
#endif		

    // Check if there is any requests in work queue. 
    if( iReqQueue.Count() )		
        {
        TNcnReqQueueElement elem = iReqQueue[0];
        iReqQueue.Remove( 0 );
#ifdef _DEBUG
        NCN_RDEBUG( _L("CNcnMsgWaitingManager::RunL: Handling request:") );
        NCN_RDEBUG_INT( _L("0: iCmd = %d"), elem.iCmd );			
        NCN_RDEBUG_INT( _L("0: iField = %d"), elem.iField );			
        NCN_RDEBUG_INT( _L("0: iParam1 = %d"), elem.iParam1 );			
        NCN_RDEBUG_INT( _L("0: iParam2 = %d"), elem.iParam2 );									
#endif					
        iState = elem.iCmd;							
        if( elem.iCmd == ENcnSetCount )
            {
            SetMessageCount( (TNcnMessageType)elem.iField, elem.iParam1, elem.iParam2 );
            }
        else if( elem.iCmd == ENcnSetIndicator )				
            {
            SetIndicator( (TNcnIndicator)elem.iField, elem.iParam1 );					
            }
        }
    else
        {
        NCN_RDEBUG( _L("CNcnMsgWaitingManager::RunL: state ENcnIdle") );
        iState = ENcnIdle;				
        }		
    
    
    if ( !iConstructionReady )
        {
        iConstructionReady = ETrue;
        iModel.VoiceMailManager().UpdateVMNotifications();
        
        }
    }

// ----------------------------------------------------
//  CNcnMsgWaitingManager::RunError
// ----------------------------------------------------
// 
TInt CNcnMsgWaitingManager::RunError( TInt aError )
    {
	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::RunError: aError %d"), aError );    

    return aError;
    }

// ----------------------------------------------------
//  CNcnMsgWaitingManager::DoCancel
// ----------------------------------------------------
//  
void CNcnMsgWaitingManager::DoCancel()
    {
	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::DoCancel: iState %d"), iState );
    
    if( IsActive() )
	    {
		switch( iState )
			{
			case ENcnGetIndicator:
			case ENcnGetCount:
				iMobilePhone.CancelAsyncRequest( EMobilePhoneGetIccMessageWaitingIndicators );
				iState = ENcnIdle;
				break;
			case ENcnSetIndicator:
			case ENcnSetCount:
				iMobilePhone.CancelAsyncRequest( EMobilePhoneSetIccMessageWaitingIndicators );
				iState = ENcnIdle;
				break;
			case ENcnNotifyMsgWaiting:
				iMobilePhone.CancelAsyncRequest( EMobilePhoneNotifyIccMessageWaitingIndicatorsChange );
				iState = ENcnIdle;
				break;
			default:
				break;						
			}    	    	
	    }
    }


// ----------------------------------------------------
//  CNcnMsgWaitingManager::SetIndicator
// ----------------------------------------------------
//  
TInt CNcnMsgWaitingManager::SetIndicator( 
    const TNcnIndicator aIndicator, 
    TBool aEnable )
    {    
	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::SetIndicator: iState %d"), iState );
	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::SetIndicator: indicator %d"), aIndicator );
	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::SetIndicator: enable %d"), aEnable );	
		
    switch( aIndicator )
	    {
		case ENcnIndicatorFax:
			DoSetIndicator( RMobilePhone::KDisplayFaxActive, aEnable );			
			break;
			
		case ENcnIndicatorEmail:
			DoSetIndicator( RMobilePhone::KDisplayEmailActive, aEnable );			
			break;
			
		case ENcnIndicatorOther:
			DoSetIndicator( RMobilePhone::KDisplayOtherActive, aEnable );	
			break;
						
		case ENcnIndicatorVMLine1:
			DoSetIndicator( RMobilePhone::KDisplayVoicemailActive, aEnable );		
			break;
			
		case ENcnIndicatorVMLine2:
			DoSetIndicator( RMobilePhone::KDisplayAuxVoicemailActive, aEnable );
			break;  
	    
		default:
			break;	    	
	    }
    
    if( IsActive() )
	    {
	    NCN_RDEBUG( _L("CNcnMsgWaitingManager::SetIndicator: Already actived. Adding request to queue") );
		TNcnReqQueueElement elem( ENcnSetIndicator, aIndicator, aEnable );
		iReqQueue.Append( elem );
	    }
	else
		{
		NCN_RDEBUG( _L("CNcnMsgWaitingManager::SetIndicator: Activating waiting manager") );
	    iState = ENcnSetIndicator;
		iStatus = KRequestPending;	    
	    iMobilePhone.SetIccMessageWaitingIndicators( iStatus, iMsgWaitingPckg );	    
	    SetActive();	    	   
	    NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::SetIndicator: iStatus %d"), iStatus.Int() );	    			
		}	    

    return KErrNone;
    }
      
// ----------------------------------------------------
//  CNcnMsgWaitingManager::DoSetIndicator
// ----------------------------------------------------
//          
void CNcnMsgWaitingManager::DoSetIndicator( TInt aId, TBool aEnable )        
	{	
	NCN_RDEBUG( _L("CNcnMsgWaitingManager::DoSetIndicator") );
	if( aEnable )		
		{
		iMsgWaiting.iDisplayStatus |= aId;			
		}
	else
		{
		iMsgWaiting.iDisplayStatus &= ~ aId;					
		}		
	}
      
// ----------------------------------------------------
//  CNcnMsgWaitingManager::SetMessageCount
// ----------------------------------------------------
//                  
TInt CNcnMsgWaitingManager::SetMessageCount( 
    const TNcnMessageType aMsgType, 
    TUint aCount,
    TBool aEnableIndicator )
    {    
	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::SetMessageCount: iState %d"), iState );
	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::SetMessageCount: message type %d"), aMsgType );
	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::SetMessageCount: message count %d"), aCount );
	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::SetMessageCount: enable indicator %d"), aEnableIndicator );	
						
    switch( aMsgType )
	    {
		case ENcnMessageTypeFax:
			DoSetIndicator( RMobilePhone::KDisplayFaxActive, aEnableIndicator );
			iMsgWaiting.iFaxMsgs = aCount;			
			break;
			
		case ENcnMessageTypeEmail:
			DoSetIndicator( RMobilePhone::KDisplayEmailActive, aEnableIndicator );
			iMsgWaiting.iEmailMsgs = aCount;			
			break;
			
		case ENcnMessageTypeOther:
			DoSetIndicator( RMobilePhone::KDisplayOtherActive, aEnableIndicator );	
			iMsgWaiting.iOtherMsgs = aCount;
			break;
						
		case ENcnMessageTypeVMLine1:
			DoSetIndicator( RMobilePhone::KDisplayVoicemailActive, aEnableIndicator );
			iMsgWaiting.iVoiceMsgs = aCount;					
			break;
			
		case ENcnMessageTypeVMLine2:
			DoSetIndicator( RMobilePhone::KDisplayAuxVoicemailActive, aEnableIndicator );
			iMsgWaiting.iAuxVoiceMsgs = aCount;			
			break;  
	    
		default:
			break;	    	
	    }	   	    

	if( IsActive() )
		{
	    NCN_RDEBUG( _L("CNcnMsgWaitingManager::SetMessageCount: Already actived. Adding request to queue") );		
		TNcnReqQueueElement elem( ENcnSetCount, aMsgType, aEnableIndicator, aCount );
		iReqQueue.Append( elem );					
		}
	else
		{
		NCN_RDEBUG( _L("CNcnMsgWaitingManager::SetMessageCount: Activating waiting manager") );		
	    iState = ENcnSetCount;
		iStatus = KRequestPending;	    
	    iMobilePhone.SetIccMessageWaitingIndicators( iStatus, iMsgWaitingPckg );	    
	    SetActive();	    
	    NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::SetMessageCount: iStatus %d"), iStatus.Int() );	    			
		}		

    return KErrNone;
    }

// ----------------------------------------------------
//  CNcnMsgWaitingManager::GetMessageCount
// ----------------------------------------------------
//      
TInt CNcnMsgWaitingManager::GetMessageCount( const TNcnMessageType aMsgType, TUint &aCount )
    {
	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::GetMessageCount: iState %d"), iState );
	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::GetMessageCount: message type %d"), aMsgType );
    
	// Is there a need to refresh message counts at this point?	
    switch( aMsgType )
	    {
		case ENcnMessageTypeFax:
			aCount = iMsgWaiting.iFaxMsgs;			
			break;
			
		case ENcnMessageTypeEmail:
			aCount = iMsgWaiting.iEmailMsgs;
			break;
			
		case ENcnMessageTypeOther:
			aCount = iMsgWaiting.iOtherMsgs;
			break;
						
		case ENcnMessageTypeVMLine1:
			aCount = iMsgWaiting.iVoiceMsgs;
			break;
			
		case ENcnMessageTypeVMLine2:
			aCount = iMsgWaiting.iAuxVoiceMsgs;
			break;  
	    
		default:
			break;	    	
	    }
	    
	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::GetMessageCount: message count %d"), aCount );	    	

    return KErrNone;
    }                    
            
// ----------------------------------------------------
//  CNcnMsgWaitingManager::GetIndicator
// ----------------------------------------------------
//    
TInt CNcnMsgWaitingManager::GetIndicator( const TNcnIndicator aIndicator, TBool& aEnabled )
    {
 	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::GetIndicator: iState %d"), iState );
 	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::GetIndicator: aIndicator %d"), aIndicator );
 
    switch( aIndicator )
	    {
		case ENcnIndicatorFax:
			aEnabled = ( iMsgWaiting.iDisplayStatus & RMobilePhone::KDisplayFaxActive ) ? ETrue : EFalse;
			break;
			
		case ENcnIndicatorEmail:
			aEnabled = ( iMsgWaiting.iDisplayStatus & RMobilePhone::KDisplayEmailActive ) ? ETrue : EFalse;		
			break;
			
		case ENcnIndicatorOther:
			aEnabled = ( iMsgWaiting.iDisplayStatus & RMobilePhone::KDisplayOtherActive ) ? ETrue : EFalse;				
			break;
						
		case ENcnIndicatorVMLine1:
			aEnabled = ( iMsgWaiting.iDisplayStatus & RMobilePhone::KDisplayVoicemailActive ) ? ETrue : EFalse;		
			break;
			
		case ENcnIndicatorVMLine2:
			aEnabled = ( iMsgWaiting.iDisplayStatus & RMobilePhone::KDisplayAuxVoicemailActive ) ? ETrue : EFalse;		
			break;  
	    
		default:
			break;	    	
	    }
	        
 	NCN_RDEBUG_INT( _L("CNcnMsgWaitingManager::GetIndicator: indicator state %d"), aEnabled );	        	        

    return KErrNone;
    }

// ----------------------------------------------------
//  CNcnMsgWaitingManager::ConstructionReady
// ----------------------------------------------------
//    
TBool CNcnMsgWaitingManager::ConstructionReady()
    {
    return iConstructionReady;
    }



//  End of File
