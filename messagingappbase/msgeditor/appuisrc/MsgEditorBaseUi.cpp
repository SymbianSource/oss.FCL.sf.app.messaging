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
* Description:  MsgEditorBaseUi implementation
*
*/



// ========== INCLUDE FILES ================================
#include "MsgEditorBaseUi.h"

#include <centralrepository.h>
#include <messaginginternalcrkeys.h>
#include <MessagingSDKCRKeys.h>
#include <messagingvariant.hrh>      // for KMsgEditorFeatureIdStayInViewerAfterReply

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgEditorAppUi::CMsgEditorAppUi
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgEditorBaseUi::CMsgEditorBaseUi()
    {
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CMsgEditorBaseUi::ConstructL()
    {
    // must set the local zoom factor before BaseConstructL to 
    // avoid an unnecessary layout switch, but therefore we also must
    // avoid leaving.
    CRepository* repository = NULL;
    TRAPD( err, repository = CRepository::NewL( KCRUidMessagingUiSettings ) );
    if( !err )
        {
        TInt localZoom = EAknUiZoomAutomatic;
        TInt err = repository->Get( KMessagingEditorLocalUiZoom, localZoom );
        if( !err )
            {
            TAknUiZoom zoom = static_cast<TAknUiZoom>( localZoom );
            CAknAppUiBase::SetLocalUiZoom( zoom ); // this is safe, as it doesn't leave before BaseConstructL is called
          //  CAknAppUiBase::SetLocalUiZoom(EAknUiZoomLarge);//testcode - remove
            }
        delete repository;
        }
    
    TRAP( err, repository = CRepository::NewL( KCRUidMuiuVariation ) );
    if( !err )
        {
        TInt flag;
        TInt err = repository->Get( KMuiuMsgEditorFeatures, flag );
        if( !err )
            {
            if ( flag & KMsgEditorFeatureIdStayInViewerAfterReply)
            	{
            	iEditorBaseFeatures |= EStayInViewerAfterReply;
            	}
            }
        delete repository;
        }
    
    
    if ( iEditorBaseFeatures & EDisableMSKSupport )
        {
        BaseConstructL( EAknEnableSkin | EAknSingleClickCompatible );
        }
    else
        {
        BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );
        }
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::~CMsgEditorAppUi
//
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgEditorBaseUi::~CMsgEditorBaseUi()
    {
    }


//  End of File
