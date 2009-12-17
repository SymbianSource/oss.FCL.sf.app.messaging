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
* Description: 
*     Preferences class of Mail editor and viewer, inline functions
*
*/


#ifndef MSGMAILPREFERENCES_INL
#define MSGMAILPREFERENCES_INL

// INLINE FUNCTIONS

// ----------------------------------------------------------------------------
// TAdditionalHeaderStatus::TAdditionalHeaderStatus 
// ----------------------------------------------------------------------------
//
inline TAdditionalHeaderStatus::TAdditionalHeaderStatus()
	:iHeaderValue(TMsgControlId(0)), iStatus(EHeaderHidden),
	iHasContent(EFalse)
    {
    }

// ----------------------------------------------------------------------------
// TAdditionalHeaderStatus::TAdditionalHeaderStatus
// ----------------------------------------------------------------------------
//
inline TAdditionalHeaderStatus::TAdditionalHeaderStatus(
	TMsgControlId aControlID):iHeaderValue(aControlID),
	iStatus(EHeaderHidden), iHasContent(EFalse)
    {
    }  
    

// ----------------------------------------------------------------------------
// CMsgMailPreferences::MessageScheduling accesses Message Scheduling value
// ----------------------------------------------------------------------------
//
inline CMsgMailPreferences::TMsgMailScheduling CMsgMailPreferences
    ::MessageScheduling() const 
    {
    return iScheduling;
    }

// ----------------------------------------------------------------------------
// CMsgMailPreferences::SendingCharacterSet
// ----------------------------------------------------------------------------
//
inline TUid CMsgMailPreferences::SendingCharacterSet() const
	{
	return iSendingCharacterSet;
	}

// ----------------------------------------------------------------------------
// CMsgMailPreferences::SetSendingCharacterSet
// ----------------------------------------------------------------------------
//
inline void CMsgMailPreferences::SetSendingCharacterSet(
	TUid aSendingCharacterSet)
	{
	iSendingCharacterSet = aSendingCharacterSet;
	}
	
// ----------------------------------------------------------------------------
// CMsgMailPreferences::ServiceId accesses remote mailbox Service ID
// Returns: Service ID of currently selected remote mailbox
// ----------------------------------------------------------------------------
//
inline TMsvId CMsgMailPreferences::ServiceId() const
    {  
    return iServiceId;
    }

// ----------------------------------------------------------------------------
// CMsgMailPreferences::SetMessageScheduling sets Message Scheduling value 
// ----------------------------------------------------------------------------
//
inline void CMsgMailPreferences::SetMessageScheduling(
    TMsgMailScheduling aScheduling) // new message scheduling value 
    {
    iScheduling = aScheduling;
    }

// ----------------------------------------------------------------------------
// CMsgMailPreferences::SetServiceId sets remote mailbox Service ID
// ----------------------------------------------------------------------------
//
inline void CMsgMailPreferences::SetServiceId(
    TMsvId aServiceId) // new remote mailbox service id
    {
    iServiceId = aServiceId;
    }

// ----------------------------------------------------------------------------
// CMsgMailPreferences::SetEncodingSupport
// ----------------------------------------------------------------------------
//
inline void CMsgMailPreferences::SetEncodingSupport( TBool aSupported )
    {
    iEncodingSupported = aSupported;
    }

#endif      // MSGMAILPREFERENCES_INL

// End of File
