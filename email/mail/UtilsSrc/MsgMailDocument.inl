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
* Description:  Document base class of Mail editor and viewer, inline functions
*
*/


#ifndef MSGMAILDOCUMENT_INL
#define MSGMAILDOCUMENT_INL

// FORWARD DECLARATIONS
class CImHeader;
class CImEmailMessage;
class CMailCRHandler;

// INCLUDES
#include <miuthdr.h>
#include <miutset.h>

#include "MsgMailPreferences.h"
#include "MsgEditorDocument.h"


// INLINE FUNCTIONS

// -----------------------------------------------------------------------------
// CMsgMailDocument::SetPreferences()
// -----------------------------------------------------------------------------
//
inline void CMsgMailDocument::SetPreferences(CMsgMailPreferences* aPrefs) 
    { 
    iPrefs = aPrefs; 
    }

// -----------------------------------------------------------------------------
// CMsgMailDocument::HeaderL()
// -----------------------------------------------------------------------------
//
inline CImHeader& CMsgMailDocument::HeaderL() const 
    {
    ASSERT( iHeader );
    return *iHeader;
    }

// -----------------------------------------------------------------------------
//  CMsgMailDocument::DefaultMsgFolder()
//  Returns:    TMsvId of the default Draft folder
// -----------------------------------------------------------------------------
//
inline TMsvId CMsgMailDocument::DefaultMsgFolder() const
    {
    return KMsvDraftEntryIdValue;
    }

// -----------------------------------------------------------------------------
// CMsgMailDocument::SMTPServiceTypeUid()
// -----------------------------------------------------------------------------
//
inline TMsvId CMsgMailDocument::DefaultMsgService() const
    {
    return TUid(KUidMsgTypeSMTP).iUid;
    }


// -----------------------------------------------------------------------------
//  CMsgMailDocument::SendOptions
// -----------------------------------------------------------------------------
//
inline CMsgMailPreferences& CMsgMailDocument::SendOptions() const
    {
    return *iPrefs;
    }

// -----------------------------------------------------------------------------
//  CMsgMailDocument::SetConfirmation()
// -----------------------------------------------------------------------------
//
inline void CMsgMailDocument::SetConfirmation(TBool aConfNote)
    {
    iConfNote = aConfNote;
    }

// -----------------------------------------------------------------------------
//  CMsgMailDocument::ShowConfirmation()
// -----------------------------------------------------------------------------
//
inline TBool CMsgMailDocument::ShowConfirmation() const
    {
    return iConfNote;
    }

// -----------------------------------------------------------------------------
// CMsgMailDocument::CreateNewL()
// -----------------------------------------------------------------------------
//
inline TMsvId CMsgMailDocument::CreateNewL(TMsvId aService, TMsvId aTarget,
    TInt aTypeList)
    {
    return CreateNewL(aService, aTarget, aTypeList, KUidMsgTypeSMTP);
    }

// -----------------------------------------------------------------------------
// CMsgMailDocument::CreateNewL()
// -----------------------------------------------------------------------------
//
inline TMsvId CMsgMailDocument::CreateNewL(TMsvId aService, TMsvId aTarget)
    {
    return CreateNewL(aService, aTarget, 0, KUidMsgTypeSMTP);
    }

// -----------------------------------------------------------------------------
// CMsgMailDocument::MailCRHandler()
// -----------------------------------------------------------------------------
//
inline CMailCRHandler* CMsgMailDocument::MailCRHandler() const
    {
    return iMailCRHandler;
    }    

#endif      // MSGMAILDOCUMENT_INL

// End of File
