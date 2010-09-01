/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides CPostcardDocument class methods.
*
*/




// ========== INCLUDE FILES ================================

#include <eikenv.h>
#include <eikdoc.h>

#include <msvstd.h>                     // TMsvEntry
#include <msvapi.h>
#include <msvuids.h>                    // Msgs Uids
#include <mtclbase.h>                   // CBaseMtm

#include <bautils.h>

#include <mmsclient.h>                  // Client Mtm API
#include <MuiuMsvUiServiceUtilities.h>  //Disk space check
#include <MsgAttachmentInfo.h>
#include <mmsmsventry.h>

 #include <centralrepository.h>    // link against centralrepository.lib
 #include <MmsEngineInternalCRKeys.h> // for Central Repository keys

#include <mtmuibas.h>

// Features
#include <featmgr.h>	
#include <bldvariant.hrh>
#include <messagingvariant.hrh>

#include <MsgMediaResolver.h>
#include <msgtextutils.h>
#include <MsgMimeTypes.h>

#include <MuiuMsgEditorLauncher.h>

#include "PostcardDocument.h"
#include "PostcardAppUi.h"

const TInt KPostcardIndexEntryExtra = 2000; // space reserved for the index entry
const TInt KMaxPostcardWidth = 1600;
const TInt KMaxPostcardHeight = 1200;

// ---------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CPostcardDocument::CPostcardDocument( CEikApplication& aApp ) :
    CMsgEditorDocument( aApp ),
    iMsgChanged( 0 ),
    iMsgType( EPostcardSent ),
    iMaxMessageSize( 300 )
    {
    }

// ---------------------------------------------------------
// CPostcardDocument::CreateAppUiL
// ---------------------------------------------------------
//
CEikAppUi* CPostcardDocument::CreateAppUiL()
    {
    CEikAppUi* appui = new ( ELeave ) CPostcardAppUi();
    return appui;
    }
    
// ---------------------------------------------------------
// CPostcardDocument::ConstructL
// ---------------------------------------------------------
//
void CPostcardDocument::ConstructL()
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

// ----------------------------------------------------
// CPostcardDocument::NewL
// ----------------------------------------------------
CPostcardDocument* CPostcardDocument::NewL(CEikApplication& aApp)
    {
    CPostcardDocument* self = new ( ELeave ) CPostcardDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CPostcardDocument::~CPostcardDocument
// ---------------------------------------------------------
CPostcardDocument::~CPostcardDocument()
    {
    delete iMediaResolver;
    delete iTextUtils;
    }

// ---------------------------------------------------------
// CPostcardDocument::DefaultMsgFolder
// ---------------------------------------------------------
TMsvId CPostcardDocument::DefaultMsgFolder() const
    {
    return KMsvDraftEntryIdValue;
    }

// ---------------------------------------------------------
// CPostcardDocument::DefaultMsgService
// ---------------------------------------------------------
TMsvId CPostcardDocument::DefaultMsgService() const
    {
    return KMsvLocalServiceIndexEntryId;
    }

// ---------------------------------------------------------
// CPostcardDocument::CreateNewL
// ---------------------------------------------------------
TMsvId CPostcardDocument::CreateNewL( TMsvId /* aServiceId */, TMsvId aDestFolder )
    {
    // Can only create to draft
    if ( aDestFolder != KMsvDraftEntryIdValue )
        {
        User::Leave( KErrNotSupported );
        }

    // Don't allow creation if will go under CL
    if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL( 
                        Session(),
                        KPostcardIndexEntryExtra ) )
        {
        User::Leave( KErrDiskFull );
        }

    CMmsClientMtm& clientMtm = Mtm();

    // CREATE TO aDestFolder (DRAFT)
    CMsvOperation* createOperation = NULL;
    CMsvOperationWait* wait = CMsvOperationWait::NewLC();
    wait->Start();
    createOperation = clientMtm.CreateNewEntryL( aDestFolder, wait->iStatus );
    CleanupStack::PushL( createOperation );
    CActiveScheduler::Start();

    if ( wait->iStatus.Int() != KErrNone )
        {
        User::Leave( wait->iStatus.Int() );
        }

    // Get the message id
    TPckgBuf<TMsvId> pkg;
    pkg.Copy( createOperation->ProgressL() );
    TMsvId progress = 0;
    progress = pkg();
    CleanupStack::PopAndDestroy( 2, wait ); // wait, createOperation

    return progress;
    }

// ---------------------------------------------------------
// CPostcardDocument::Mtm
// ---------------------------------------------------------
CMmsClientMtm& CPostcardDocument::Mtm()
    {
    return static_cast <CMmsClientMtm&> ( CMsgEditorDocument::Mtm() );
    }

// ---------------------------------------------------------
// CPostcardDocument::MtmUi
// ---------------------------------------------------------
CBaseMtmUi& CPostcardDocument::MtmUi()
    {
    return CMsgEditorDocument::MtmUi();
    }

// ---------------------------------------------------------
// CPostcardDocument::EntryChangedL
// Intentionally empty implementation.
// ---------------------------------------------------------
void CPostcardDocument::EntryChangedL()
    {
    }

// ---------------------------------------------------------
// CPostcardDocument::InitializeL
// ---------------------------------------------------------
void CPostcardDocument::InitializeL( RFs& aFs )
    {
    iFs = aFs;

    iMediaResolver = CMsgMediaResolver::NewL();
    iMediaResolver->SetCharacterSetRecognition( iMsgType == EPostcardSendAs );
	
	iTextUtils = CMsgTextUtils::NewL( iFs );
    }

// ----------------------------------------------------
// CMmsEditorDocument::SetMessageType
// ----------------------------------------------------
void CPostcardDocument::SetMessageType()
    {
    const TInt flags = LaunchFlags();
    TMmsMsvEntry tEntry = static_cast<TMmsMsvEntry>( Entry() );

	if ( tEntry.Parent( ) == KMsvSentEntryId )
		{
        iMsgType = EPostcardSent;
        return;		
		}

    if ( flags & EMsgCreateNewMessage )
        {
        iMsgType = EPostcardNew;
        return;
        }
    else if ( flags & EMsgForwardMessage )
        {
        iMsgType = EPostcardForward;
        return;
        }
    else if ( flags & EMsgCreateDuplicate )
        {
        iMsgType = EPostcardEdit;
        return;
        }
    else if ( flags & EMsgEditNewMessage )
        {
        iMsgType = EPostcardSendAs;
        return;
        }

    // Either sent or sendas or open from draft
    if ( tEntry.ReadOnly( ) )
        {
        iMsgType = EPostcardSent;
        }
    else
        {
        if( tEntry.EditorOriented( ) )
        	{
	        iMsgType = EPostcardDraft;    		
        	}
    	else
    		{
    		iMsgType = EPostcardSendAs;
    		}
        }   
    }

// ----------------------------------------------------
// SetChanged
// ----------------------------------------------------
void CPostcardDocument::SetChanged( TPostcardPart aPart )
    {
    iMsgChanged |= aPart;
    }

// ----------------------------------------------------
// Changed
// ----------------------------------------------------
TBool CPostcardDocument::Changed( TPostcardPart aPart ) const
    {
    return iMsgChanged & aPart;
    }

// ----------------------------------------------------
// Modified
// ----------------------------------------------------
TBool CPostcardDocument::Modified( ) const
    {
    return iMsgChanged;
    }

// ----------------------------------------------------
// MaxMessageSize
// ----------------------------------------------------
TInt CPostcardDocument::MaxMessageSize( ) const
	{
	return iMaxMessageSize;
	}

// ----------------------------------------------------
// MaxImageSize
// ----------------------------------------------------
TInt CPostcardDocument::MaxImageSize( ) const
    {
    return iMaxMessageSize - KPostcardAddressGreetingOverhead;
    }

// ---------------------------------------------------------
// AppFullName
// ---------------------------------------------------------
TFileName CPostcardDocument::AppFullName( )
    {
    return Application( )->AppFullName( );
    }
    
// ---------------------------------------------------------
// IsImage
// ---------------------------------------------------------
TBool CPostcardDocument::IsImage( const TDataType& aMimeType )
	{
    if ( aMimeType.Des8( ).CompareF( KMsgMimeImageJpeg ) == 0 )
        {
        return ETrue;
        }
    else if ( aMimeType.Des8( ).CompareF( KMsgMimeImageGif ) == 0 )
        {
        return ETrue;
        }
    else if ( aMimeType.Des8( ).CompareF( KMsgMimeImagePng ) == 0 )
        {
        return ETrue;
        }
    return EFalse;
	}

// ---------------------------------------------------------
// MaxImageDimensions
// ---------------------------------------------------------
TSize CPostcardDocument::MaxImageDimensions( )
	{
	return TSize( KMaxPostcardWidth, KMaxPostcardHeight );
	}


//  End of File
