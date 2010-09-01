/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ICalendar viewer ECOM plugin
*
*/


// INCLUDE FILES

#include "CICalViewer.h"
#include "CICalViewerView.h"
#include "MailLog.h"
#include "ICalViewer.hrh"
#include "icalvieweruid.h"
#include <avkon.hrh>
#include "cicalattaloader.h"
#include <MRCommands.hrh>

// SYSTEM INCLUDES
#include <stringloader.h>
#include <MsgEditorView.h>  // CMsgEditorView
#include <FeatMgr.h>
#include <icalvieweruires.rsg>
#include <MsgMailViewer.rsg>
#include <MMSVATTACHMENTMANAGER.H>
#include <msgmailviewer.hrh>
#include <CMSVATTACHMENT.H>
#include <APGCLI.H>
#include <calsession.h>
#include <CALENTRY.H>	 	//calendar api V2
#include <MAgnEntryUi.h> 	//entry ui params
#include <MsgMailUIDs.h> 	//mail viewer application uid
#include <CMailMessage.h> 	//mail
#include <e32des16.h> 		//HBufC
#include <caluser.h> 		//CCalAttendee
#include <calalarm.h> 		//CCalAlarm
#include <calrrule.h> 		//TCalRRule
#include <e32std.h> 
#include <e32base.h> 
#include <CalenImporter.h>	// import/export for ical files
#include <SendUiConsts.h>   //MTM uid constants for resolver params


// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT(KICalUiResourceFile, "icalvieweruires.rsc");
_LIT8(KDataTypeVCalendar2, "text/x-vCalendar"); // vCalendar v1.0 MIME type
_LIT8(KDataTypeVCalendar3, "text/calendar"); // vCalendar v2.0 MIME type

enum TStatusFlags
    {
    EFlagRecognized = KBit0,
    EFlagIsUnread = KBit1, // MsgEditor sets unread flag to EFalse too early!
	EFlagIsNavigableForward = KBit2,
	EFlagIsNavigableBackward = KBit3,
    }; 
    
}  // namespace


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CICalViewer::CICalViewer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CICalViewer::CICalViewer(): iFlags(0)
    {
    }

// -----------------------------------------------------------------------------
// CICalViewer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CICalViewer::ConstructL()
    {
    LOG("CICalViewer::ConstructL");    
    FeatureManager::InitializeLibL();
    
    TFileName driveName;
    DoGetCorrectDllDriveL( driveName );
    TInt nameLength = driveName.Length() + KICalUiResourceFile().Length();
    LOG1("CICalViewer::ConstructL: namelength: %d", nameLength);
    iResourceFile = HBufC::NewL( KMaxFileName );
    *iResourceFile = driveName;
    iResourceFile->Des().Append( KICalUiResourceFile );
      
    LOG("CICalViewer::ConstructL -> End");
    }
// -----------------------------------------------------------------------------
// CICalViewer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CICalViewer* CICalViewer::NewL()
	{
    CICalViewer* self = new(ELeave) CICalViewer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CICalViewer::~CICalViewer()
// Destructor
// -----------------------------------------------------------------------------
// 
CICalViewer::~CICalViewer()
    {
    LOG("CICalViewer::~CICalViewer");
    delete iResourceFile;
    delete iAttaLoad;
    delete iICalView;
    FeatureManager::UnInitializeLib();
    iEntries.ResetAndDestroy();
    delete iIdleLauncher;
    delete iImporter;
    delete iEntryUiInParams;
    delete iEntryUiOutParams;
    delete iSession;
    LOG("CICalViewer::~CICalViewer -> End");    
    }

// -----------------------------------------------------------------------------
// CICalViewer::ViewMessageL
// -----------------------------------------------------------------------------
// 
void CICalViewer::ViewMessageL( RFileReadStream& aReadStream, 
	MMailAppUiInterface& aUICallBack)
    {
    LOG("CICalViewer::ViewMessageL (BVA)");
        
    if (!iAppUi)
        {
        iAppUi = &aUICallBack;
        }
	
	if ( !iSession )
		{			
		iSession = CCalSession::NewL();
		}
	
	TRAPD( err, iSession->OpenL( iSession->DefaultFileNameL() ) );
    if ( err == KErrNotFound )
        {
        iSession->CreateCalFileL( iSession->DefaultFileNameL() );
        iSession->OpenL( iSession->DefaultFileNameL() );
        }
    else
        {
        User::LeaveIfError( err );
        }	
	
	if ( !iImporter )
	    {
	    iImporter = CCalenImporter::NewL( *iSession );
	    }
	    
    LOG("CICalViewer::ViewMessageL, starting iCal importing");
	iImporter->ImportICalendarL( aReadStream, iEntries );
	
	if( iEntries.Count() == 0 )
		{
		User::Leave( KErrNotFound );
		}
		
    LOG("CICalViewer:: ...importing finished");				
			
    LOG("CICalViewer::ViewMessageL, creating AgnEntryUi");
    iEntryUiInParams = new(ELeave) MAgnEntryUi::TAgnEntryUiInParams(
    	TUid::Uid( KUidBVAApplication ),
    	*iSession,
    	MAgnEntryUi::EViewEntry );
    	
    LOG("CICalViewer::ViewMessageL, launching viewer");
    // In case of BVA we can always launch the default AgnEntryUi
    // -> this can be achieved by giving SMTP mtm uid
    DoViewMessageL( KSenduiMtmSmtpUid );
    LOG("CICalViewer::ViewMessageL -> End");    
    }

// -----------------------------------------------------------------------------
// CICalViewer::ParseMessageToCalEntryL
// -----------------------------------------------------------------------------
// 
void CICalViewer::ParseMessageToCalEntryL( CMailMessage& aMessage )
	{
    LOG("CICalViewer::ParseMessageToCalEntryL");    	
	if ( !iSession )
		{			
		iSession = CCalSession::NewL();
		}
	
	TRAPD( err, iSession->OpenL( iSession->DefaultFileNameL() ) );
    if ( err == KErrNotFound )
        {
        iSession->CreateCalFileL( iSession->DefaultFileNameL() );
        iSession->OpenL( iSession->DefaultFileNameL() );
        }
    else
        {
        User::LeaveIfError( err );
        }	    

    LOG("CICalViewer::ParseMessageToCalEntryL, checking attachment info");

	MMsvAttachmentManager& attachmentManager = 
				aMessage.AttachmentManager();			
				
	TInt attaCount = attachmentManager.AttachmentCount();
	
	if ( attaCount > 0 )
		{
		CMsvAttachment* info = attachmentManager.GetAttachmentInfoL(0);
		CleanupStack::PushL(info);

		const TDesC& filePath = info->FilePath();

		RFile attachment = attachmentManager.GetAttachmentFileL( 0 );
		CleanupClosePushL( attachment ); // RFile must be closed

		RFileReadStream stream( attachment, 0 );
		CleanupClosePushL( stream );
		
		if ( !iImporter )
		    {
		    iImporter = CCalenImporter::NewL( *iSession );
		    }
				    
        LOG("CICalViewer:: starting iCal importing");
		iImporter->ImportICalendarL( stream, iEntries );
		
		if( iEntries.Count() == 0 )
			{
			User::Leave( KErrNotFound );
			}
		
        LOG("CICalViewer:: ...importing finished");		
		
		//checking of ical entry on rudimentary level
		//so that we can make a leave here...
		//even so... the ical cant be launched from plain text viewer
		//only the alternative description text is shown to user

		CCalEntry& entry = *( iEntries[0] );
		switch ( entry.MethodL() )
			{
			case CCalEntry::EMethodNone:
			case CCalEntry::EMethodPublish:
			case CCalEntry::EMethodAdd:
			case CCalEntry::EMethodRefresh:
			case CCalEntry::EMethodCounter:
			case CCalEntry::EMethodDeclineCounter:
				{
				User::Leave( KErrNotSupported );
				}
			}

		CleanupStack::PopAndDestroy( 3 );				// CSI: 47 # stream, attachment, info
		}
    LOG("CICalViewer::ParseMessageToCalEntryL -> End");		
	}

// -----------------------------------------------------------------------------
// CICalViewer::ViewMessageL
// -----------------------------------------------------------------------------
// 
void CICalViewer::ViewMessageL(
	CMailMessage& aMessage,
	MMailAppUiInterface& aUICallBack, 
	TUint& aParam )
    {
    LOG("CICalViewer::ViewMessageL (mail)");      
        
    aParam |= EMailUseDefaultNaviPane; 
	
	iMsg = aMessage.MessageEntry();
	
    if (!iAppUi)
        {
        iAppUi = &aUICallBack;
        }
        
    LOG("CICalViewer::ViewMessageL, creating AgnEntryUi");
    iEntryUiInParams = new(ELeave) MAgnEntryUi::TAgnEntryUiInParams(
    	TUid::Uid( KUidMsgMailViewer ),
    	*iSession,
    	MAgnEntryUi::EViewEntry );
    	
	iEntryUiInParams->iMailBoxId = iMsg.iServiceId;
	iEntryUiInParams->iMsgSession = aMessage.Session();
	iEntryUiInParams->iMessageId = iMsg.Id();

    // Load attachments, otherwise they won't be visible in the
    // attachement manager. CICalViewer::MessageLoadL() received
    // also attachments ready signal, but attachment manager was
    // resetted after that.
    if ( !iAttaLoad )
        {
        iAttaLoad = CICalAttaLoader::NewL( *iAppUi );
        }
    iAttaLoad->StartLoadingL( aMessage );

    LOG("CICalViewer::ViewMessageL, launching viewer");
    DoViewMessageL( iMsg.iMtm );
    LOG("CICalViewer::ViewMessageL -> End");        
    }

// -----------------------------------------------------------------------------
// CICalViewer::IdleTimerCallbackL
// This method must be leaving so that exiting from viewer
// is possible (done using basched.h KLeaveExit)
// -----------------------------------------------------------------------------
//
TInt CICalViewer::IdleTimerCallbackL(TAny* aViewer)	// CSI: 40 # We must return 
													// the integer value although this 
													// is a leaving method.
	{
	CICalViewer* thisViewer = static_cast<CICalViewer*>( aViewer );
	TRAPD( err, thisViewer->DoOMRViewerLaunchL() );
	if ( err != KErrNone )
	    {
        LOG("CIcalViewer::IdleTimerCallback, leave occurred, exiting");	    
        thisViewer->iAppUi->AppUiHandleCommandL( EAknSoftkeyBack );
	    }
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CICalViewer::DoOMRViewerLaunchL
// -----------------------------------------------------------------------------
// 
void CICalViewer::DoOMRViewerLaunchL()
	{
    LOG("CICalViewer::DoOMRViewerLaunchL");	
	iEntryUiOutParams = new(ELeave) MAgnEntryUi::TAgnEntryUiOutParams();
	ASSERT( iSession );
	    	
    LOG("CICalViewer::DoOMRViewerLaunchL, executing viewer");    	
	iRetVal = iICalView->ExecuteViewL( iEntries,
	                                   *iEntryUiInParams,
	                                   *iEntryUiOutParams,
	                                   *this );
	LOG("CICalViewer::DoOMRViewerLaunchL, viewer finished");
	//we need to call back to ui for exiting the mail application
	iAppUi->AppUiHandleCommandL( EAknSoftkeyBack );
	
    LOG("CICalViewer::DoOMRViewerLaunchL -> End"); 	
	}

// -----------------------------------------------------------------------------
// CICalViewer::DoGetCorrectDllDriveL
// -----------------------------------------------------------------------------
//    
void CICalViewer::DoGetCorrectDllDriveL( TFileName& aDriveName )
    {
    TParse parse;
    Dll::FileName( aDriveName );
	User::LeaveIfError( parse.Set( aDriveName, NULL, NULL ) );
    aDriveName = parse.Drive(); // contains drive, e.g. "c:"
    }

// -----------------------------------------------------------------------------
// CICalViewer::DoViewMessageL
// -----------------------------------------------------------------------------
// 
void CICalViewer::DoViewMessageL( TUid aMtmUid )
	{
	
    if ( iAppUi->IsNextMessageAvailableL( ETrue ) )
        {        
    	iFlags |= EFlagIsNavigableForward;
        }
    
    if ( iAppUi->IsNextMessageAvailableL( EFalse ) )
        {        
    	iFlags |= EFlagIsNavigableBackward;
        }

	
    LOG("CICalViewer::DoViewMessageL");		
	iAppUi->SetTitleTextL(R_QTN_MAIL_TITLE_MTG_REQUEST);

    iICalView = CICalViewerView::NewL( aMtmUid );
    
    LOG("CICalViewer:: adding ical viewer control");    	
	iAppUi->AddControlL(*iICalView);
		
	//idle timer 
	iIdleLauncher = CIdle::NewL(CActive::EPriorityIdle);
    LOG("CICalViewer:: starting idle launcher");	
 	iIdleLauncher->Start(TCallBack(IdleTimerCallbackL,this));
    LOG("CICalViewer::DoViewMessageL -> End");	 	
	}

// -----------------------------------------------------------------------------
// CICalViewer::DynInitMenuPaneL
// -----------------------------------------------------------------------------
// 
void CICalViewer::DynInitMenuPaneL(TInt /*aResourceId*/,
	CEikMenuPane* /*aMenuPane*/)
    {
    }

// -----------------------------------------------------------------------------
// CICalViewer::HandleCommandL
// -----------------------------------------------------------------------------
// 
TBool CICalViewer::HandleCommandL(TInt /*aCommand*/)
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CICalViewer::HandleKeyEventL
// -----------------------------------------------------------------------------
// 
TKeyResponse CICalViewer::HandleKeyEventL(const TKeyEvent& /*aKeyEvent*/,
	TEventCode /*aType*/)
    {
    return EKeyWasConsumed;
    }

// -----------------------------------------------------------------------------
// CICalViewer::ResourceFile
// -----------------------------------------------------------------------------
// 
const TDesC& CICalViewer::ResourceFile()
    {
    return *iResourceFile;
    }

// -----------------------------------------------------------------------------
// CICalViewer::MessageLoadL
// -----------------------------------------------------------------------------
// 
void CICalViewer::MessageLoadL( TInt aState, CMailMessage& aMessage )
	{
    LOG("CICalViewer::MessageLoadL");  	
	switch (aState)
		{
		case CMailMessage::EHeaderReady:
			{
            LOG("CICalViewer::aState == EHeaderReady");
            // This switch case doesn't seem to ever get called
			break;
			}
		case CMailMessage::EAttachmentsReady:
			{
            LOG("CICalViewer::aState == EAttachmentsReady");
            
            if ( !aMessage.MessageEntry().ICalendar() )
                { // check that the entry is ical message
                LOG("TMsvEmailEntry::ICalendar() returned EFalse");
                User::Leave( KErrUnknown );
                }
                            
            RFile attachment;
            TInt count = aMessage.AttachmentManager().AttachmentCount();
            
            if ( count > 0 )
				{
				attachment = 
				    aMessage.AttachmentManager().GetAttachmentFileL( 0 );
                CleanupClosePushL( attachment );
				
				RApaLsSession apaSession;
				User::LeaveIfError( apaSession.Connect() );
				CleanupClosePushL( apaSession );
				TUid appUID( KNullUid );
				TDataType dataType;
				// Check only first attachment 
				User::LeaveIfError( apaSession.AppForDocument( attachment,
				                                               appUID,
				                                               dataType ) );
				CleanupStack::PopAndDestroy( 2 );			// CSI: 47 # apaSession, attachment
				
				if ( dataType.Des8().FindF( 
					 KDataTypeVCalendar2 ) != KErrNotFound ||
                     dataType.Des8().FindF( 
					 KDataTypeVCalendar3 ) != KErrNotFound )
					{
                    LOG("CICalViewer::starting msg parsing");
					//we can do attachment parsing only here as
					//attachment manager is resetted after this
										    		        
					ParseMessageToCalEntryL( aMessage );
					
					iFlags |= EFlagRecognized;
					LOG("CICalViewer::recognized!");
					}
				else
					{
                    LOG("CICalViewer::datatype unknown");
					// unknown
					User::Leave( KErrUnknown );
					}
				}
			else
				{
				User::Leave( KErrUnknown );
				}
			break;	
			}
		case CMailMessage::ELoadEnd:
			{
            LOG("CICalViewer::aState == ELoadEnd");	
			if ( !(iFlags & EFlagRecognized) )
				{
				// Not valid message
				User::Leave( KErrUnknown );
				}            
            if ( aMessage.IsNew() )
                {                
                iFlags |= EFlagIsUnread;
                }
            break;
			}
		default:
		    {		    
		    // do nothing
		    break;
		    }
		}
    LOG("CICalViewer::MessageLoadL -> End"); 
	}

// -----------------------------------------------------------------------------
// CICalViewer::IsCommandAvailable
// -----------------------------------------------------------------------------
// 	
TBool CICalViewer::IsCommandAvailable( TInt aCommandId )
    {
    TBool retVal( EFalse );
    switch ( aCommandId )
        {
        case EMRCommandRetrieve:
            {
            retVal = iMsg.PartialDownloaded();
            break;
            }
        case EMRCommandAttachments:
            {
            retVal = ( iAttaLoad && iAttaLoad->FinishedWithAttachments() );
            break;
            }
        case EMRCommandUnreadOpeningNote:
            {
            retVal = ( ( iFlags & EFlagIsUnread ) != 0 );
            break;
            }
        case EMRCommandNavigateBackward:
        	{
        	retVal = ( ( iFlags & EFlagIsNavigableBackward ) != 0 );
        	}
        case EMRCommandNavigateForward:
        	{
        	retVal = ( ( iFlags & EFlagIsNavigableForward ) != 0 );
        	break;
        	}
        default:
            {
            break;
            }
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CICalViewer::ProcessCommandWithResultL
// -----------------------------------------------------------------------------
// 	
TInt CICalViewer::ProcessCommandWithResultL( TInt aCommandId )	// CSI: 40 # We must return 
																// the integer value although this 
																// is a leaving method.
	{
	LOG("CICalViewer::ProcessCommandWithResultL -> End");
	TInt retVal( KErrNone );
	switch ( aCommandId )
	    {
	    case EMRCommandForward:
	        {
	        LOG("Calling AppUiHandleCommandL with forward cmd...");
	        iAppUi->AppUiHandleCommandL( EMsgMailViewerCmdForward );
	        break;
	        }
	    case EMRCommandRetrieve:
	        {
	        iAppUi->AppUiHandleCommandL( EMsgMailViewerCmdRetrieve );
	        break;
	        }	        
	    case EMRCommandAttachments:
	        {
	        iAppUi->AppUiHandleCommandL( EMsgMailViewerCmdAttachments );
	        break;
	        }
	    case EMRCommandMessageDetails:
	        {
	        iAppUi->AppUiHandleCommandL( EMsgMailViewerCmdMessageInfo );
	        break;
	        }
	    case EMRCommandNavigateBackward:
	    	{
	    	iAppUi->NextMessageL( EFalse );
	    	break;
	    	}
	    case EMRCommandNavigateForward:
	    	{
	    	iAppUi->NextMessageL( ETrue );
	    	break;
	    	}
	    default:
	        {
	        User::Leave( KErrNotSupported );
	        return KErrNotSupported;
	        }
	    }
    LOG("CICalViewer::ProcessCommandWithResultL -> End");
    return retVal;	
	}

// -----------------------------------------------------------------------------
// CICalViewer::ProcessCommandL
// -----------------------------------------------------------------------------
// 
void CICalViewer::ProcessCommandL( TInt aCommandId )
	{
	ProcessCommandWithResultL( aCommandId );
	}

//  End of File

