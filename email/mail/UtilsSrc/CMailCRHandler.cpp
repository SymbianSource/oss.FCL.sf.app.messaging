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
* Description:  Implementation of CMailCRhandler
*
*/



// INCLUDE FILES
#include    "CMailCRHandler.h"
#include	"MsgMailDocument.h"
#include	<centralrepository.h>
#include	<e32cmn.h>
#include    "MailLog.h"

//repositories
#include	<MailInternalCRKeys.h>
#include 	<CommonUiInternalCRKeys.h>
#include 	<messaginginternalcrkeys.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMailCRHandler::CMailCRHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMailCRHandler::CMailCRHandler(CMsgMailDocument* aModel)
	: iModel( aModel )
    {
    }

// -----------------------------------------------------------------------------
// CMailCRHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMailCRHandler::ConstructL()
    {
	CRepository* mailSession = CRepository::NewLC( KCRUidMail );
	iSessions.AppendL(mailSession);
	CleanupStack::Pop(); // mailSession
	
	CRepository* muiuSession = CRepository::NewLC( KCRUidCommonUi );
	iSessions.AppendL(muiuSession);
	CleanupStack::Pop(); // muiuSession
	
	CRepository* muiuVariation = CRepository::NewLC( KCRUidMuiuVariation );
	iSessions.AppendL(muiuVariation);
	CleanupStack::Pop(); // muiuVariation
	
    GetCRInt( KCRUidMail, KMailLocalVariationFlags, 
        iMailUiLocalVariationFlags );      
    LOG1( "CMailCRHandler::ConstructL LV-Flags:%d", 
        iMailUiLocalVariationFlags ); 
        
    GetCRInt( KCRUidMuiuVariation, KMuiuEmailConfigFlags, 
        iMuiuVariationFlags );
    LOG1( "CMailCRHandler::ConstructL MUIU-email Flags:%d", 
        iMuiuVariationFlags );
    
    GetCRInt( KCRUidMuiuVariation, KMuiuMmsFeatures, 
        iMmsVariationFags );        
    LOG1( "CMailCRHandler::ConstructL MUIU-mms Flags:%d", 
        iMmsVariationFags );                 
    }

// -----------------------------------------------------------------------------
// CMailCRHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMailCRHandler* CMailCRHandler::NewL( CMsgMailDocument* aModel )
    {
    CMailCRHandler* self = new( ELeave ) CMailCRHandler( aModel );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
EXPORT_C CMailCRHandler::~CMailCRHandler()
    {
	iSettingsNotifyHandlers.ResetAndDestroy();    
	iSessions.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
//  CMailCRHandler::HandleNotifyInt
// -----------------------------------------------------------------------------
//
void CMailCRHandler::HandleNotifyInt(
    const TUint32 aID,
    const TInt    aNewValue )
    {
    // then inform the model about it.
    if( aID == KMailAdditionalHeaderSettings )
        {
        TInt additionalHeaderSettings = aNewValue;
        if( additionalHeaderSettings >= 0 )
            {
            TRAPD( err, iModel->SetReceivedIntL(
						CMsgMailDocument::EIndexAddHeaders, 
						additionalHeaderSettings ));
			if ( err )
				{
				LOG1( "CMailCRHandler::HandleNotifyInt failed err:%d", err );
				}
            }
        }
	}

// -----------------------------------------------------------------------------
//  CMailCRHandler::HandleNotifyReal
// -----------------------------------------------------------------------------
//
void CMailCRHandler::HandleNotifyReal(TUint32 /*aId*/, TReal /*aNewValue*/)
	{
	//NO OPERATION
	}
	
// -----------------------------------------------------------------------------
//  CMailCRHandler::HandleNotifyString
// -----------------------------------------------------------------------------
//
void CMailCRHandler::HandleNotifyString(
    TUint32 /*aId*/, const TDesC16& /*aNewValue*/)
	{
	//NO OPERATION
	}
	
// -----------------------------------------------------------------------------
//  CMailCRHandler::HandleNotifyBinary
// -----------------------------------------------------------------------------
//	
void CMailCRHandler::HandleNotifyBinary(
    TUint32 /*aId*/, const TDesC8& /*aNewValue*/)
	{
	//NO OPERATION
	}

// -----------------------------------------------------------------------------
//  CMailCRHandler::HandleNotifyGeneric
// -----------------------------------------------------------------------------
//
void CMailCRHandler::HandleNotifyGeneric( const TUint32 /*aID*/ )
    {
	//NO OPERATION
	}

// -----------------------------------------------------------------------------
//  CMailCRHandler::HandleNotifyError
// -----------------------------------------------------------------------------
//
void CMailCRHandler::HandleNotifyError(
    const TUint32 /*aID*/,
    const TInt    /*aError*/,
	CCenRepNotifyHandler* /*aHandler*/ )
    {
	//NO OPERATION
	}



EXPORT_C void CMailCRHandler::UpdateModelFromCentralRepositoryL()
	{
	TInt headerSettings;
	
	// Get the additional header settings, 
	// if not initialized update with default value
	TInt err = (iSessions[EMailRepository])->Get( 
	    KMailAdditionalHeaderSettings, headerSettings );
	if( err == KErrNotFound || err == KErrUnknown || headerSettings < 0 )
		{
	    (iSessions[EMailRepository])->Set( KMailAdditionalHeaderSettings, 5 );	// CSI: 47 # See a comment above.
        }

    iModel->SetReceivedIntL( 
        CMsgMailDocument::EIndexAddHeaders, headerSettings );
	}

// -----------------------------------------------------------------------------
// CMailCRHandler::GetCRInt
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMailCRHandler::GetCRInt(
    const TUid&     aUid,
    const TUint32   aKey,
    TInt&           aValue ) const
    {
    // Prepare and get client
    CRepository*  client(NULL);

    TInt err = GetClientForUid( aUid, client );

    // Check if problems with getting the client
    if( err == KErrNone )
        {
        err = client->Get( aKey, aValue );
        }

    // Return result
    return err;
    }


// -----------------------------------------------------------------------------
// CMailCRHandler::SetCRInt
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMailCRHandler::SetCRInt(
    const TUid&     aUid,
    const TUint32   aKey,
    const TInt      aValue) const
    {
    // Prepare and get client
    CRepository*  client(NULL);

    TInt err = GetClientForUid( aUid, client );

    // Check if problems with getting the client
    if( err == KErrNone )
        {
        err = client->Set( aKey, aValue );
        }

    // Return result
    return err;
    }

// -----------------------------------------------------------------------------
// CMailCRHandler::GetCRReal
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMailCRHandler::GetCRReal(
    const TUid&     aUid,
    const TUint32   aKey,
    TReal&          aValue ) const
    {
    // Prepare and get client
    CRepository*  client(NULL);

    TInt err = GetClientForUid( aUid, client );

    // Check if problems with getting the client
    if( err == KErrNone )
        {
        err = client->Get( aKey, aValue );
        }

    // Return result
    return err;
    }

// -----------------------------------------------------------------------------
// CMailCRHandler::SetCRReal
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMailCRHandler::SetCRReal(
    const TUid&     aUid,
    const TUint32   aKey,
    const TReal&    aValue ) const
    {
    // Prepare and get client
    CRepository*  client(NULL);

    TInt err = GetClientForUid( aUid, client );

    // Check if problems with getting the client
    if( err == KErrNone )
        {
        err = client->Set( aKey, aValue );
        }

    // Return result
    return err;
    }

// -----------------------------------------------------------------------------
// CMailCRHandler::GetCRBinary
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMailCRHandler::GetCRBinary(
    const TUid&     aUid,
    const TUint32   aKey,
    TDes8&          aValue ) const
    {
    // Prepare and get client
    CRepository*  client(NULL);

    TInt err = GetClientForUid( aUid, client );

    // Check if problems with getting the client
    if( err == KErrNone )
        {
        err = client->Get( aKey, aValue );
        }

    // Return result
    return err;
    }
    
// -----------------------------------------------------------------------------
// CMailCRHandler::SetCRBinary
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMailCRHandler::SetCRBinary(
    const TUid&     aUid,
    const TUint32   aKey,
    const TDesC8&   aValue ) const
    {
    // Prepare and get client
    CRepository*  client(NULL);

    TInt err = GetClientForUid( aUid, client );

    // Check if problems with getting the client
    if( err == KErrNone )
        {
        err = client->Set( aKey, aValue );
        }

    // Return result
    return err;
    }
 
 // ---------------------------------------------------------------------------
// CMailCRHandler::GetCRString
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMailCRHandler::GetCRString(
    const TUid&     aUid,
    const TUint32   aKey,
    TDes16&         aValue ) const
    {
    // Prepare and get client
    CRepository*  client(NULL);

    TInt err = GetClientForUid( aUid, client );

    // Check if problems with getting the client
    if( err == KErrNone )
        {
        err = client->Get( aKey, aValue );
        }

    // Return result
    return err;
    }
    
// -----------------------------------------------------------------------------
// CMailCRHandler::SetCRString
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMailCRHandler::SetCRString(
    const TUid&     aUid,
    const TUint32   aKey,
    const TDesC16&  aValue ) const
    {
    // Prepare and get client
    CRepository*  client(NULL);

    TInt err = GetClientForUid( aUid, client );

    // Check if problems with getting the client
    if( err == KErrNone )
        {
        err = client->Set( aKey, aValue );
        }

    // Return result
    return err;
    }

// -----------------------------------------------------------------------------
// CMailCRHandler::GetClientForUid
// -----------------------------------------------------------------------------
//
TInt CMailCRHandler::GetClientForUid(
    const TUid&         aUid,
    CRepository*&  aClient ) const
    {
    TInt error = KErrNone;
    
    if( aUid == KCRUidMail )
        {
        // Get mail observer
	    aClient = iSessions[EMailRepository];
        }
    else if( aUid == KCRUidCommonUi )
    	{
    	// Get common ui observer
    	aClient = iSessions[ECommonUiRepository];
    	}
    else if( aUid == KCRUidMuiuVariation )
    	{
    	aClient = iSessions[EMuiuVariationKeys];
    	}
    else
        {
        // Return error
        error = KErrNotFound;
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CMailCRHandler::SetNotifyHandlerL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMailCRHandler::SetNotifyHandlerL(
    MCenRepNotifyHandlerCallback* aCallBack,
    CMailCRHandler::ERepository aRepository, 
    CCenRepNotifyHandler::TCenRepKeyType aKeyType, TUint32 aKeyValue )
	{
	__ASSERT_DEBUG( aRepository < ERepositoryOutOfBounds, User::Invariant() );
	
	MCenRepNotifyHandlerCallback* observer;
	if (!aCallBack)
		observer = this;
	else		
		observer = aCallBack;
	
	CCenRepNotifyHandler* mailSettingsNotifyHandler =
		CCenRepNotifyHandler::NewLC( 
		    *observer, 
		    *iSessions[aRepository], 						// CSI: 2 # This is checked above in the assert macro.
		    aKeyType, 
		    aKeyValue );

	mailSettingsNotifyHandler->StartListeningL();
	iSettingsNotifyHandlers.AppendL(mailSettingsNotifyHandler);
	CleanupStack::Pop(mailSettingsNotifyHandler);
	}

//  End of File
