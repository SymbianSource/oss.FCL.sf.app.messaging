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
* Description:  
*       Provides CNotViewerDocument methods.
*
*/



// INCLUDES
#include "NotViewerDocument.h"  // CNotViewerDocument
#include "NotViewerAppUi.h"     // CNotViewerAppUi

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CNotViewerDocument::CNotViewerDocument( CEikApplication& aApp ) :
        CMsgEditorDocument( aApp ), iHeaderModified( 0 )
    {
    }

// ---------------------------------------------------------
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CNotViewerDocument::ConstructL( )
    {
    CMsgEditorDocument::ConstructL( );

    PrepareMtmL( KUidMsgMMSNotification );
    }

// ---------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------
//
CNotViewerDocument* CNotViewerDocument::NewL( CEikApplication& aApp )
    {
    CNotViewerDocument* self = new ( ELeave ) CNotViewerDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( );
    return self;
    }
    
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CNotViewerDocument::~CNotViewerDocument( )
    {
    }

// ---------------------------------------------------------
// CNotViewerDocument::CreateAppUiL
// ---------------------------------------------------------
//
CEikAppUi* CNotViewerDocument::CreateAppUiL( )
    {
    CMsgEditorAppUi* appui = new ( ELeave ) CNotViewerAppUi( );
    return appui;
    }


// ---------------------------------------------------------
// CNotViewerDocument::DefaultMsgFolder
// ---------------------------------------------------------
//
TMsvId CNotViewerDocument::DefaultMsgFolder( ) const
    {
    return KMsvGlobalOutBoxIndexEntryIdValue;
    }

// ---------------------------------------------------------
// CNotViewerDocument::DefaultMsgService
// ---------------------------------------------------------
//
TMsvId CNotViewerDocument::DefaultMsgService( ) const
    {
    return KMsvLocalServiceIndexEntryId;
    }

// ---------------------------------------------------------
// CNotViewerDocument::CreateNewL
// ---------------------------------------------------------
//
TMsvId CNotViewerDocument::CreateNewL( TMsvId /*aService*/, TMsvId /*aFolder*/ )
    {
    TMsvId tmpId = 0;
    return tmpId;
    }

// ---------------------------------------------------------
// CNotViewerDocument::EntryChangedL
// ---------------------------------------------------------
//
void CNotViewerDocument::EntryChangedL( )
    {
    if( iAppUi )
        {
        ( static_cast<CNotViewerAppUi*>( iAppUi ) )->EntryChangedL( );
        }
    }

// ---------------------------------------------------------
// CNotViewerDocument::Mtm
// ---------------------------------------------------------
//
CMmsNotificationClientMtm& CNotViewerDocument::Mtm( ) const
    {
    return static_cast <CMmsNotificationClientMtm&> ( CMsgEditorDocument::Mtm( ) );
    }

// ---------------------------------------------------------
// CNotViewerDocument::MtmUi
// ---------------------------------------------------------
//
CNotMtmUi& CNotViewerDocument::MtmUi( ) const
    {
    return static_cast <CNotMtmUi&> ( CMsgEditorDocument::MtmUi( ) );
    }

// ---------------------------------------------------------
// CNotViewerDocument::MtmUiDataL
// ---------------------------------------------------------
//
CNotMtmUiData& CNotViewerDocument::MtmUiDataL( ) const
    {
    return static_cast <CNotMtmUiData&> ( CMsgEditorDocument::MtmUiDataL( ) );
    }

//  End of File  
