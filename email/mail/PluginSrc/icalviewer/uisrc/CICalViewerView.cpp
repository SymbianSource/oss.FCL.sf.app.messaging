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
* Description:  CICalViewerView
*
*/



// INCLUDE FILES
#include    "CICalViewerView.h"
#include 	"MailLog.h"
#include 	"CICalEngine.h"
#include 	<MAgnEntryUi.h> 	//entry ui
#include 	<SendUiConsts.h> 	// for resolving the MTM UID

// CONSTANTS
//const ?type ?constant_var = ?constant;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CICalViewerView::CICalViewerView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CICalViewerView::CICalViewerView()
    {
    }

// -----------------------------------------------------------------------------
// CICalViewerView::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CICalViewerView::ConstructL( TUid aMtmUid )
    {
    LOG("CICalViewerView::ConstructL");
    iEngine = CICalEngine::NewL();
    LOG("CICalViewerView::ConstructL - engine created");
   
	TBuf8<KMaxUidName> uidName;
	uidName.Copy( aMtmUid.Name() );
	iEntryUI = iEngine->CreateEntryUIL( uidName );
    LOG("CICalViewerView::ConstructL - end"); 
    }

// -----------------------------------------------------------------------------
// CICalViewerView::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CICalViewerView* CICalViewerView::NewL( TUid aMtmUid )
    {
    CICalViewerView* self = new( ELeave ) CICalViewerView;
    
    CleanupStack::PushL( self );
    self->ConstructL( aMtmUid );
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CICalViewerView::~CICalViewerView()
    {
    delete iEntryUI;
    delete iEngine;
    }


// -----------------------------------------------------------------------------
// CICalViewerView::ExecuteViewL
// -----------------------------------------------------------------------------
//
TInt CICalViewerView::ExecuteViewL( RPointerArray<CCalEntry>& aEntries,					// CSI: 40 # We must return 
																						// the integer value although this 
																						// is a leaving method.
                                   const MAgnEntryUi::TAgnEntryUiInParams& aInParams,
                                   MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,                                   
                                   MAgnEntryUiCallback& aCallback )
	{
	return iEntryUI->ExecuteViewL(aEntries, aInParams, aOutParams, aCallback);
	}

//  End of File  
