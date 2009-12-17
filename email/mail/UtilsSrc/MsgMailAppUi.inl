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
*     Application UI base class of Mail editor and viewer, inline functions
*
*/


#ifndef MSGMAILAPPUI_INL
#define MSGMAILAPPUI_INL

// FORWARD DECLARATIONS

// INCLUDES
#include <MsgExpandableControl.h>
#include <MsgAddressControl.h>
#include <MsgAttachmentControl.h>
#include <MsgBodyControl.h>
#include <MsgEditorView.h>
#include <MsgEditor.hrh>

// INLINE FUNCTIONS

// ----------------------------------------------------------------------------
//  CMsgMailAppUi::AppCoeEnv()
// ----------------------------------------------------------------------------
// 
inline CCoeEnv& CMsgMailAppUi::AppCoeEnv()
	{
	return *iCoeEnv;
	}


// ----------------------------------------------------------------------------
//  CMsgMailAppUi::SubjectControl()
//  Returns the pointer to the subject control.
// ----------------------------------------------------------------------------
// 
inline CMsgExpandableControl* CMsgMailAppUi::SubjectControl() const
    {
    return static_cast<CMsgExpandableControl*> 
        (iView->ControlById(EMsgComponentIdSubject));
    }

// ----------------------------------------------------------------------------
//  CMsgMailAppUi::AddressControl()
//  Returns the pointer to the given address control (header field).
// ----------------------------------------------------------------------------
//
inline CMsgAddressControl* CMsgMailAppUi::AddressControl(
    const TInt aId) const
    {
    CMsgAddressControl* addressCtrl(NULL);
    
    CMsgBaseControl* ctrl = iView->ControlById(aId);
    if ( ctrl && ctrl->ControlType() == EMsgAddressControl )
    	{
    	addressCtrl = static_cast<CMsgAddressControl*> (ctrl);
    	}
    return addressCtrl; 
    }
    
// ----------------------------------------------------------------------------
//  CMsgMailAppUi::AttachmentControl()
// ----------------------------------------------------------------------------
//
inline CMsgAttachmentControl* CMsgMailAppUi::AttachmentControl() const
    {
    CMsgBaseControl* ctrl = iView->ControlById( EMsgComponentIdAttachment );
    ASSERT( ctrl );
            
    return static_cast<CMsgAttachmentControl*>( ctrl );
    }

// ----------------------------------------------------------------------------
//  CMsgMailAppUi::BodyControl()
//  Returns the pointer to the body control.
// ----------------------------------------------------------------------------
//
inline CMsgBodyControl* CMsgMailAppUi::BodyControl() const
    {
    return static_cast<CMsgBodyControl*> (iView->ControlById(EMsgComponentIdBody));
    }


#endif      // MSGMAILAPPUI_INL

// End of File
