/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides audioMessage document methods. 
*
*/




#include <MuiuMsvUiServiceUtilities.h>  //Disk space check
#include <centralrepository.h> 
#include <mmsclient.h>    
#include <MmsEngineInternalCRKeys.h> 
#include <mmsmsventry.h>
#include <MuiuMsgEditorLauncher.h>
#include <MsgMediaResolver.h>
#include "audiomessageappui.h"  
#include "audiomessagedocument.h"
#include "MsgMimeTypes.h"

const TInt KAmsIndexEntryExtra = 2000;  // space reserved for the index entry
const TInt KDefaultMaxSize = 300 * 1024;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
// CAudioMessageDocument::Constructor
// ---------------------------------------------------------
//
CAudioMessageDocument::CAudioMessageDocument( CEikApplication& aApp ) :
        CMsgEditorDocument( aApp ),
        iMaxMessageSize( KDefaultMaxSize ),
        iRecordIconStatus( EAudioInsert )
    {
    }

// ---------------------------------------------------------
// CAudioMessageDocument::Destructor
// ---------------------------------------------------------
//
CAudioMessageDocument::~CAudioMessageDocument()
    {
    delete iMediaResolver;
    }

// ---------------------------------------------------------
// CAudioMessageDocument::SetMessageType
// ---------------------------------------------------------
//
void CAudioMessageDocument::SetMessageType()
	{
    const TInt flags = LaunchFlags();
    TMmsMsvEntry tEntry = static_cast<TMmsMsvEntry>( Entry() );

	if ( tEntry.Parent( ) == KMsvSentEntryId )
		{
        iMessageType = EAmsSent;
        return;		
		}

    if ( flags & EMsgCreateNewMessage )
        {
        iMessageType = EAmsNewMessage;
        return;
        }
    else if ( flags & EMsgForwardMessage )
        {
        iMessageType = EAmsForward;
        return;
        }
    else if ( flags & EMsgCreateDuplicate )
        {
        iMessageType = EAmsEdit;
        return;
        }
    else if ( flags & EMsgEditNewMessage )
        {
        iMessageType = EAmsSendAs;
        return;
        }
    else if ( flags & EMsgReadOnly && flags & EMsgLaunchEditorEmbedded )
    	{
        iMessageType = EAmsInbox;
        return;
        }
     
  	// Either sendas or open from draft
    if ( tEntry.IsForwardedMessage() )
        {
        // Forwarded message saved to draft
        iMessageType = EAmsForward;
        }
    else if ( tEntry.EditorOriented() )
        {
        iMessageType = EAmsOpenFromDraft;
        }
    else
        {
        iMessageType = EAmsSendAs;
        }   
	}

// ---------------------------------------------------------
// CAudioMessageDocument::ConstructL
// ---------------------------------------------------------
//
void CAudioMessageDocument::ConstructL()
    {
    CMsgEditorDocument::ConstructL();
    
    CRepository* repository = CRepository::NewL( KCRUidMmsEngine );
    TInt maxSendSize = 0;
    // value is returned as bytes now
   	TInt err = repository->Get( KMmsEngineMaximumSendSize, maxSendSize );
    if ( !err )
        {
        iMaxMessageSize = maxSendSize;
        }
    delete repository;
    PrepareMtmL( KUidMsgTypeMultimedia );
    }
    
// ---------------------------------------------------------
// CAudioMessageDocument::NewL
// ---------------------------------------------------------
//
CAudioMessageDocument* CAudioMessageDocument::NewL(CEikApplication& aApp)
    {
    CAudioMessageDocument* med = new ( ELeave ) CAudioMessageDocument( aApp );
    CleanupStack::PushL( med );
    med->ConstructL();
    CleanupStack::Pop( med );
    return med;
    }

// ---------------------------------------------------------
// CAudioMessageDocument::CreateAppUiL
// ---------------------------------------------------------
//
CEikAppUi* CAudioMessageDocument::CreateAppUiL() 
    {
    return new ( ELeave ) CAudioMessageAppUi;
    }


// ---------------------------------------------------------
// CAudioMessageDocument::CreateNewL
// ---------------------------------------------------------
//
TMsvId CAudioMessageDocument::CreateNewL(TMsvId /* aServiceId */, TMsvId aDestFolder )
    {
    // Can only create to draft
    if ( aDestFolder != KMsvDraftEntryIdValue )
        {
        User::Leave( KErrNotSupported );
        }

    // Don't allow creation if will go under CL
    if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL( 
                        Session(),
                        KAmsIndexEntryExtra ) )
        {
        User::Leave( KErrDiskFull );
        }

    CMmsClientMtm& clientMtm = Mtm();

    // CREATE TO aDestFolder (DRAFT)
    CMsvOperationActiveSchedulerWait* wait = CMsvOperationActiveSchedulerWait::NewLC();
    CMsvOperation* createOperation = clientMtm.CreateNewEntryL( aDestFolder, wait->iStatus );
    CleanupStack::PushL( createOperation );
    wait->Start();

    if ( wait->iStatus.Int() != KErrNone )
        {
        User::Leave( wait->iStatus.Int() );
        }

    // Get the message id
    TPckgBuf<TMsvId> pkg;
    pkg.Copy( createOperation->ProgressL() );
    TMsvId progress = 0;
    progress = pkg();
    CleanupStack::PopAndDestroy( createOperation ); 
    CleanupStack::PopAndDestroy( wait );
    return progress;
    }

// ---------------------------------------------------------------------------
// CAudioMessageDocument::EntryChangedL
// Intentionally empty implementation.
// ---------------------------------------------------------------------------
//
void CAudioMessageDocument::EntryChangedL()
    {
    }
  
// ---------------------------------------------------------
// CAudioMessageDocument::InitializeL
// ---------------------------------------------------------
//
TInt CAudioMessageDocument::InitializeL( )
    {
    iMediaResolver = CMsgMediaResolver::NewL();
    return KErrNone;
    }  

// ---------------------------------------------------------
// CAudioMessageDocument::Mtm
// ---------------------------------------------------------
//
CMmsClientMtm& CAudioMessageDocument::Mtm()
    {
    return static_cast <CMmsClientMtm&> ( CMsgEditorDocument::Mtm() );
    }

// ---------------------------------------------------------
// CAudioMessageDocument::MessageType
// ---------------------------------------------------------
//
TBool CAudioMessageDocument::IsAmrL( RFile& aFile, TDataType& aMimeType ) const
	{
	iMediaResolver->RecognizeL( aFile, aMimeType);
	TDataType testType( KMsgMimeAudioAmr );
	if ( aMimeType != testType )
		{
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	
	}


