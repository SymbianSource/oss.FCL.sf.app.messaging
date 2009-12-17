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
*     Mail message container, inline functions
*
*/


#ifndef CMAILMESSAGE_INL
#define CMAILMESSAGE_INL

// FORWARD DECLARATIONS

// INCLUDES
#include <msvapi.h>
#include <miuthdr.h>
#include <miutmsg.h>
#include <cmsvattachment.h>
#include <e32cmn.h>

// INLINE FUNCTIONS

// ----------------------------------------------------------------------------
// CMailMessage::MessageHeader() 
// ----------------------------------------------------------------------------
//
inline CImHeader* CMailMessage::MessageHeader()
	{
	return iHeader;
	}

// ----------------------------------------------------------------------------
// CMailMessage::MessageBody() 
// ----------------------------------------------------------------------------
//
inline CRichText* CMailMessage::MessageBody()
	{
	return iBodyText;
	}

// ----------------------------------------------------------------------------
// CMailMessage::MessageEntry() 
// ----------------------------------------------------------------------------
//
inline TMsvEmailEntry CMailMessage::MessageEntry() 
    { 
    TRAP_IGNORE( iEntry->SetEntryL( iMessageID ) );
	TMsvEmailEntry entry(iEntry->Entry());
    return entry; 
    }
    
// ----------------------------------------------------------------------------
// CMailMessage::MessageType() 
// ----------------------------------------------------------------------------
//
inline TUid CMailMessage::MessageType() 
    { 
    return iEntry->Entry().iMtm; 
    }

// ----------------------------------------------------------------------------
// CMailMessage::HtmlContent() 
// ----------------------------------------------------------------------------
//
inline RFile CMailMessage::HtmlContent() 
    { 
    return iHtmlFile; 
    } 

// ----------------------------------------------------------------------------
// CMailMessage::LinkedHtmlContent() 
// ----------------------------------------------------------------------------
//
inline RFile CMailMessage::LinkedHtmlContent() 
    { 
    return iLinkedHtmlFile; 
    }          
    
// ----------------------------------------------------------------------------
// CMailMessage::Session() 
// ----------------------------------------------------------------------------
//
inline CMsvSession* CMailMessage::Session() 
    { 
    return &iEntry->Session(); 
    }      

// ----------------------------------------------------------------------------
// CMailMessage::Selection() 
// ----------------------------------------------------------------------------
//
inline const CMsvEntrySelection& CMailMessage::Selection() 
    { 
    return iMessage->Selection(); 
    }      
      
// ----------------------------------------------------------------------------
// CMailMessage::LinkedHtmlId()
// ----------------------------------------------------------------------------
//
inline TMsvId CMailMessage::LinkedHtmlId() 
    { 
    return iLinkedfileId; 
    }     

// ----------------------------------------------------------------------------
// CMailMessage::MhtmlPartId()
// ----------------------------------------------------------------------------
//
inline TMsvId CMailMessage::MhtmlPartId() 
    { 
    return iMhtmlPartId; 
    } 
    
// ----------------------------------------------------------------------------
// CMailMessage::SetNew()
// ----------------------------------------------------------------------------
//
inline void CMailMessage::SetNew( TBool aNew ) 
    {
    iNewMessage = aNew;
    }     

// ----------------------------------------------------------------------------
// CMailMessage::IsNew()
// ----------------------------------------------------------------------------
//
inline TBool CMailMessage::IsNew() const 
    { 
    return iNewMessage;
    } 
         
#endif      // CMAILMESSAGE_INL

// End of File
