/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*      Sms viewer
*
*/



// INCLUDE FILES
#include <cntdef.h>             // ContactField definitions
#include <msvapi.h>             // CMsvOperation, CMsvEntry
#include <msvuids.h>            // Msgs Uids
#include <smut.h>               // Sms utilities
#include <akninputblock.h>	    // CAknInputBlock
#include "MsgSmsViewerDoc.h"    // CMsgSmsViewerDocument
#include "MsgSmsViewerAppUi.h"  // CMsgSmsViewerAppUi
#include "MsgSmsViewerGsmCdmaCreator.h"

// ================= MEMBER FUNCTIONS =======================

// C++ constructor can NOT contain any code, that
// might leave.
//
CMsgSmsViewerDocument::CMsgSmsViewerDocument( 
    CEikApplication& aApp)
        : CMsgEditorDocument(aApp)
    {
    }

// Two-phased constructor.
CMsgSmsViewerDocument* CMsgSmsViewerDocument::NewL(
    CEikApplication& aApp)
    {
    CMsgSmsViewerDocument* sed = new (ELeave) CMsgSmsViewerDocument(aApp);
    CleanupStack::PushL(sed);
    sed->ConstructL();
    sed->PrepareMtmL( KUidMsgTypeSMS );
    CleanupStack::Pop();
    return sed;
    }
    
// Destructor
CMsgSmsViewerDocument::~CMsgSmsViewerDocument()
    {
    }

// ---------------------------------------------------------
// CMsgSmsViewerDocument::CreateAppUiL
// (other items were commented in a header).
// ---------------------------------------------------------
CEikAppUi* CMsgSmsViewerDocument::CreateAppUiL()
    {
    return SmsViewerCreator::CreateAppUiL();
    }

// ---------------------------------------------------------
// CMsgSmsViewerDocument::CreateNewL
// (other items were commented in a header).
// ---------------------------------------------------------
TMsvId CMsgSmsViewerDocument::CreateNewL(
    TMsvId /*aService*/,
    TMsvId /*aFolder*/)
    {
    __ASSERT_DEBUG( 0, Panic(EMsgSmsViewerTriesEditorFuncs ));
    return KMsvNullIndexEntryIdValue; 
    }

// ---------------------------------------------------------
// CMsgSmsViewerDocument::EntryChangedL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerDocument::EntryChangedL()
    {
#ifdef RD_MSG_FAST_PREV_NEXT
    static_cast<CMsgSmsViewerAppUi*>(iAppUi)->EntryChangedL();
#endif
    }

// ---------------------------------------------------------
// CMsgSmsViewerDocument::DefaultMsgFolder
// (other items were commented in a header).
// ---------------------------------------------------------
TMsvId CMsgSmsViewerDocument::DefaultMsgFolder() const
    {
    return KMsvDraftEntryIdValue;
    }

// ---------------------------------------------------------
// CMsgSmsViewerDocument::DefaultMsgService
// (other items were commented in a header).
// ---------------------------------------------------------
TMsvId CMsgSmsViewerDocument::DefaultMsgService() const
    {
    return KMsvLocalServiceIndexEntryId;
    }

//  End of File  
