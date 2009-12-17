/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgSendKeyAcceptingQuery implementation
*
*/



// ========== INCLUDE FILES ================================

#include "MsgSendKeyAcceptingQuery.h"

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
// CMsgSendKeyAcceptingQuery::NewL
//
//
// ---------------------------------------------------------
CMsgSendKeyAcceptingQuery* CMsgSendKeyAcceptingQuery::NewL(TDesC& aPrompt, const TTone& aTone)
{
    CMsgSendKeyAcceptingQuery* self = new (ELeave) CMsgSendKeyAcceptingQuery(aTone);

    CleanupStack::PushL(self);
    self->SetPromptL(aPrompt);
    CleanupStack::Pop();

    return self;
}

// ---------------------------------------------------------
// CMsgSendKeyAcceptingQuery::CMsgSendKeyAcceptingQuery
//
//
// ---------------------------------------------------------
CMsgSendKeyAcceptingQuery::CMsgSendKeyAcceptingQuery(const TTone aTone)
    :
    CAknQueryDialog(aTone)
{
}

// ---------------------------------------------------------
// CMsgSendKeyAcceptingQuery::~CMsgSendKeyAcceptingQuery
//
//
// ---------------------------------------------------------
CMsgSendKeyAcceptingQuery::~CMsgSendKeyAcceptingQuery()
{
}

// ---------------------------------------------------------
// CMsgSendKeyAcceptingQuery::OfferKeyEventL
//
//
// ---------------------------------------------------------
TKeyResponse CMsgSendKeyAcceptingQuery::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
{
    if (aType == EEventKey && aKeyEvent.iCode == EKeyYes)
    {
        TryExitL(EKeyYes);
        return EKeyWasConsumed;
    }
    else
    {
        return CAknDialog::OfferKeyEventL(aKeyEvent, aType);
    }
}

// ---------------------------------------------------------
// CMsgSendKeyAcceptingQuery::OkToExitL
//
//
// ---------------------------------------------------------
TBool CMsgSendKeyAcceptingQuery::OkToExitL(TInt aButtonId)
{
    if (EKeyYes == aButtonId)
    {
        return ETrue;
    }
    else
    {
        return CAknDialog::OkToExitL(aButtonId);
    }
}

// ---------------------------------------------------------
// CMsgSendKeyAcceptingQuery::MappedCommandId
//
//
// ---------------------------------------------------------
TInt CMsgSendKeyAcceptingQuery::MappedCommandId(TInt aButtonId)
{
    switch(aButtonId)
    {
    case EKeyYes:
        return EKeyYes;

    default :
        return CEikDialog::MappedCommandId(aButtonId);
    }
}

//  End of File
