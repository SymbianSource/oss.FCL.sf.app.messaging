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
* Description:  Mail icalviewer attachment loader
*
*/


// INCLUDE FILES

#include "cicalattaloader.h"
#include  <MMailAppUiInterface.h>
#include  <CMailMessage.h>
#include  <MMsvAttachmentManager.h>

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CICalAttaLoader::CICalAttaLoader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CICalAttaLoader::CICalAttaLoader( MMailAppUiInterface& aAppUI )
    : iAppUI( aAppUI ),
    iFinishedWithAttachments( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CICalAttaLoader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CICalAttaLoader* CICalAttaLoader::NewL( MMailAppUiInterface& aAppUI )
	{
	return new( ELeave ) CICalAttaLoader( aAppUI );
	}
	
// -----------------------------------------------------------------------------
// CICalAttaLoader::~CICalAttaLoader
// Destructor
// -----------------------------------------------------------------------------
//
CICalAttaLoader::~CICalAttaLoader()
    {
    }

// -----------------------------------------------------------------------------
// CICalAttaLoader::StartLoadingL
// -----------------------------------------------------------------------------
//
void CICalAttaLoader::StartLoadingL( CMailMessage& aMessage )
    {
    iFinishedWithAttachments = EFalse;
    aMessage.LoadAttachmentsL( *this );
    }

// -----------------------------------------------------------------------------
// CICalAttaLoader::FinishedWithAttachments
// -----------------------------------------------------------------------------
//
TBool CICalAttaLoader::FinishedWithAttachments()
    {
    return iFinishedWithAttachments;
    }

// -----------------------------------------------------------------------------
// CICalAttaLoader::MessageLoadingL
// -----------------------------------------------------------------------------
//
void CICalAttaLoader::MessageLoadingL( TInt aStatus, CMailMessage& aMessage )
    {
	if ( aStatus == CMailMessage::EAttachmentsReady ) 
		{
	    MMsvAttachmentManager& manager = aMessage.AttachmentManager();
		TInt count = manager.AttachmentCount();
			
        // current implementation (also in the ical viewer) assumes that .ics
        // attachment is the first attachment in the array, if it isn't
        // then e-mail is not viewed with ical viewer. We will hide that
        // first .ics attachment from the user -> start from index 1        
        for ( TInt i( 1 ); i < count; ++i ) 
            {
            iFinishedWithAttachments = ETrue; 
            CMsvAttachment* info = manager.GetAttachmentInfoL( i );
            CleanupStack::PushL( info );			
            iAppUI.AddAttachmentL( *info, ETrue );			
            CleanupStack::PopAndDestroy( info );
            }
		}    
    }

//  End of File

