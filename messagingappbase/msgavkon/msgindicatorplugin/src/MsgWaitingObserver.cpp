/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MsgIndicatorPlugin plugin implementation
*
*/



#include <etelmm.h>
#include <MessagingDomainCRKeys.h>
#include "MsgWaitingObserver.h"
#include "msgindicatorpluginlog.h"
#include "msgindicatorpluginimplementation.h"
	
// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::CMsgIndicatorPluginImplementation
// ---------------------------------------------------------------------------
//
CWaitingObserver::CWaitingObserver( RMobilePhone &aMobilePhone,
    CMsgIndicatorPluginImplementation& iIndicatorPlugin ) :
    CActive( EPriorityNormal ),
	iChangeNotifySubscribed( EFalse ),
    iMobilePhone( aMobilePhone ),
    iMsgWaiting(),
  	iMsgWaitingPckg( iMsgWaiting ),
  	iIndicatorPlugin(iIndicatorPlugin)
    {
    CActiveScheduler::Add( this );
    }
// -----------------------------------------------------------------------------
// CWaitingObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWaitingObserver* CWaitingObserver::NewL( RMobilePhone &aMobilePhone,
                            CMsgIndicatorPluginImplementation& iIndicatorPlugin )
    {
    CWaitingObserver* self = new (ELeave) CWaitingObserver( aMobilePhone, iIndicatorPlugin);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
// ---------------------------------------------------------------------------
// CMsgIndicatorPluginImplementation::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CWaitingObserver::ConstructL()
    {
    MSGPLUGINLOGGER_WRITE_FORMAT( "::ConstructL: this 0x%x", 
    	this );

    User::LeaveIfError( iTelServer.Connect() );

    User::LeaveIfError( iTelServer.GetPhoneInfo( 0, iPhoneInfo ) );
        
    User::LeaveIfError( iMobilePhone.Open( iTelServer, iPhoneInfo.iName ) );   

    iMobilePhone.GetIccMessageWaitingIndicators( iStatus, iMsgWaitingPckg );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CWaitingObserver::~CWaitingObserver()
// ---------------------------------------------------------------------------
//
CWaitingObserver::~CWaitingObserver()
    {
    MSGPLUGINLOGGER_WRITE_FORMAT( "Destructor: this 0x%x", 
    	this );
    Cancel();
    }
    
// ---------------------------------------------------------------------------
// CWaitingObserver::RunL()
// ---------------------------------------------------------------------------
//
void CWaitingObserver::RunL()
    {
    TInt err = iStatus.Int();
    MSGPLUGINLOGGER_WRITE_FORMAT("RunL(): err: %d", err);    
    // subscribe for notifications

    if ( err == KErrNone )
        {
        UpdateIndicatorStatusL();
        }
        
    iMobilePhone.NotifyIccMessageWaitingIndicatorsChange( iStatus, iMsgWaitingPckg );
    SetActive();

    // get-request completed, DoCancel() must now cancel notify request
    iChangeNotifySubscribed = ETrue; 
    }

// ---------------------------------------------------------------------------
// CWaitingObserver::DoCancel()
// ---------------------------------------------------------------------------
//    
void CWaitingObserver::DoCancel()
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

// ---------------------------------------------------------------------------
// CWaitingObserver::GetMsgCount(const TIndicatorMessageType aMsgType)
// ---------------------------------------------------------------------------
//
TInt CWaitingObserver::GetMsgCount( const TIndicatorMessageType aMsgType )
    {
    TInt msgCount = 0;
    MSGPLUGINLOGGER_WRITE( "GetMsgCount()");
    switch( aMsgType )
        {
    	case EIndicatorMessageTypeFax:
    		msgCount = iMsgWaiting.iFaxMsgs;			
        	MSGPLUGINLOGGER_WRITE_FORMAT( "GetMsgCount->FAX %d", msgCount );
        	MSGPLUGINLOGGER_WRITE_FORMAT( "GetMsgCount->FAX %d", iMsgWaiting.iFaxMsgs );
    		break;
    		
    	case EIndicatorMessageTypeVMLine1:
    		msgCount = iMsgWaiting.iVoiceMsgs;
        	MSGPLUGINLOGGER_WRITE_FORMAT( "GetMsgCount->LINE1 %d", msgCount );
        	MSGPLUGINLOGGER_WRITE_FORMAT( "GetMsgCount->LINE1 %d", iMsgWaiting.iVoiceMsgs );
    		break;
    		
    	case EIndicatorMessageTypeVMLine2:
    		msgCount = iMsgWaiting.iAuxVoiceMsgs;
        	MSGPLUGINLOGGER_WRITE_FORMAT( "GetMsgCount->LINE2 %d", msgCount );
        	MSGPLUGINLOGGER_WRITE_FORMAT( "GetMsgCount->LINE2 %d", iMsgWaiting.iAuxVoiceMsgs );
    		break;  
        
    	default:
    		break;	    	
        }
    MSGPLUGINLOGGER_WRITE_FORMAT( "GetMsgCount->RETURN %d", msgCount );	    
    return msgCount;
    }
    
// ---------------------------------------------------------------------------
// CWaitingObserver::UpdateIndicatorStatusL()
// ---------------------------------------------------------------------------
//	    	    
void CWaitingObserver::UpdateIndicatorStatusL()
	{
    MSGPLUGINLOGGER_WRITE( "UpdateIndicatorStatus()");

    MSGPLUGINLOGGER_WRITE_FORMAT( "::UpdateIndicatorStatus: Voice mail line 1 count %d", iMsgWaiting.iVoiceMsgs );
    MSGPLUGINLOGGER_WRITE_FORMAT( "::UpdateIndicatorStatus: Fax count %d", iMsgWaiting.iFaxMsgs );

    iMsgWaiting = iMsgWaitingPckg();
    if(!CheckSupressNotificationSettingL())
        {
    	if(iMsgWaiting.iDisplayStatus & RMobilePhone::KDisplayVoicemailActive)
        	{
        	if ( !iIndicatorPlugin.IsALSSupported() )
            	{
            	MSGPLUGINLOGGER_WRITE( "iIndicatorPlugin.UpdateTextL( EAknIndicatorVoiceMailWaiting )");
            	iIndicatorPlugin.UpdateL( EAknIndicatorVoiceMailWaiting );
            	}
        	else
            	{
            	MSGPLUGINLOGGER_WRITE( "iIndicatorPlugin.UpdateTextL( EAknIndicatorVoiceMailWaitingOnLine1 )");
            	iIndicatorPlugin.UpdateL( EAknIndicatorVoiceMailWaitingOnLine1 );
            	}
        	}
        }
    if ( iMsgWaiting.iDisplayStatus & RMobilePhone::KDisplayAuxVoicemailActive )
        {
        MSGPLUGINLOGGER_WRITE( "iIndicatorPlugin.UpdateTextL( EAknIndicatorVoiceMailWaitingOnLine2 )");
    	iIndicatorPlugin.UpdateL( EAknIndicatorVoiceMailWaitingOnLine2 );
        }
    if ( iMsgWaiting.iDisplayStatus & RMobilePhone::KDisplayFaxActive) 
        {
        MSGPLUGINLOGGER_WRITE( "iIndicatorPlugin.UpdateTextL( EAknIndicatorFaxMessage )");
        iIndicatorPlugin.UpdateL( EAknIndicatorFaxMessage );
        }
	}	    
// -------------------------------------------------------------------
// Check the KMuiuSupressAllNotificationConfiguration value
// -------------------------------------------------------------------
//
TBool CWaitingObserver::CheckSupressNotificationSettingL()
{
    TBool result = EFalse; 
    TInt value = 0;
    CRepository* repository = NULL;
   
   TRAPD( err, repository = CRepository::NewL( KCRUidMuiuMessagingConfiguration ) );
   if( err == KErrNone && repository != NULL )
       {
       CleanupStack::PushL( repository ); 
       err = repository->Get( KMuiuSupressAllNotificationConfiguration, value );
       
       if(err == KErrNone && (value & KMuiuNotificationSupressedForVoiceMail ))
           {
           result = ETrue;
           }
        } 
       MSGPLUGINLOGGER_WRITE_FORMAT( "CWaitingObserver:  SupressNotification %d", result );
       CleanupStack::PopAndDestroy( repository );
       return result;
}	    
//  End of File
