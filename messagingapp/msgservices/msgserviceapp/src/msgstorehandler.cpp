/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:   Reads message information from message store.
 *
 */

#include <msvids.h>
#include <mmsconst.h>
#include <SendUiConsts.h>

#include "msgstorehandler.h"
#include "msgbiouids.h"
#include "convergedmessage.h"

//----------------------------------------------------------------------------
// MsgStoreHandler::MsgStoreHandler
// @see header
//----------------------------------------------------------------------------
MsgStoreHandler::MsgStoreHandler():
iMsvSession(NULL)
        {
        InitL();
        }

//----------------------------------------------------------------------------
// MsgStoreHandler::~MsgStoreHandler
// @see header
//----------------------------------------------------------------------------
MsgStoreHandler::~MsgStoreHandler()
    {
    if(iMsvSession)
        {
        delete iMsvSession;
        iMsvSession = NULL;
        }
    }

//----------------------------------------------------------------------------
// MsgStoreHandler::InitL
// @see header
//----------------------------------------------------------------------------
void MsgStoreHandler::InitL( )
    {
    iMsvSession = CMsvSession::OpenSyncL(*this);
    }

//----------------------------------------------------------------------------
// MsgStoreHandler::markAsReadAndGetType
// @see header
//----------------------------------------------------------------------------
void MsgStoreHandler::markAsReadAndGetType(int msgId,
                                          int& msgType,
                                          int& msgSubType)
    {
    msgType = ConvergedMessage::None;
    
    CMsvEntry* cEntry = NULL;
    TRAPD(err, cEntry = iMsvSession->GetEntryL(msgId));
    if ( err == KErrNone)
        {
        TMsvEntry entry = cEntry->Entry();
        if ( entry.Unread() ) 
            {
            // Mark the entry as read
            entry.SetUnread( EFalse );
            cEntry->ChangeL( entry );
            }
        // extract message type
        extractMsgType(entry,msgType,msgSubType);
        }
    
    delete cEntry;
    }

//----------------------------------------------------------------------------
// MsgStoreHandler::HandleSessionEventL
// @see header
//----------------------------------------------------------------------------
void MsgStoreHandler::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
        TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    // Nothing to do
    }

//----------------------------------------------------------------------------
// MsgStoreHandler::extractMsgType
// @see header
//----------------------------------------------------------------------------
void MsgStoreHandler::extractMsgType(const TMsvEntry& entry,
                                    int& msgType,
                                    int& msgSubType)
    {
    msgType = ConvergedMessage::None;
    msgSubType = ConvergedMessage::None;

    switch(entry.iMtm.iUid)   
        {
        case KSenduiMtmSmsUidValue:            
            msgType = ConvergedMessage::Sms;
            break;
        case KSenduiMtmBtUidValue:
            msgType = ConvergedMessage::BT;
            break;
        case KSenduiMtmMmsUidValue:        
            msgType = ConvergedMessage::Mms;
            break;
        case KSenduiMMSNotificationUidValue:            
            msgType = ConvergedMessage::MmsNotification;
            break;
        case KSenduiMtmBioUidValue:
            { 
        if (entry.iMtmData1 == KSenduiMtmBtUidValue) 
		{
            msgType = ConvergedMessage::BT;

            if (entry.iBioType == KMsgBioUidVCard.iUid) 
			{
                msgSubType = ConvergedMessage::VCard;
            }
            else if (entry.iBioType == KMsgBioUidVCalendar.iUid) 
			{
                msgSubType = ConvergedMessage::VCal;
            }

            break;
        }
            msgType = ConvergedMessage::BioMsg; 

            // based on the biotype uid set message type
            if(entry.iBioType == KMsgBioUidRingingTone.iUid)
                {
                msgSubType = ConvergedMessage::RingingTone;
                }
            else if(entry.iBioType == KMsgBioProvisioningMessage.iUid)
                {
                msgSubType = ConvergedMessage::Provisioning;
                }     
            else if (entry.iBioType == KMsgBioUidVCard.iUid)
                {
                msgSubType = ConvergedMessage::VCard;
                }
            else if (entry.iBioType == KMsgBioUidVCalendar.iUid)
                {
                msgSubType = ConvergedMessage::VCal;
                }        
            }
            break;
        default:
            msgType = ConvergedMessage::None;       
            break;
        }
    }

//----------------------------------------------------------------------------
// MsgStoreHandler::deleteMessage
// @see header
//----------------------------------------------------------------------------
void MsgStoreHandler::deleteMessage(int msgId)
    {
    iMsvSession->RemoveEntry(msgId);
    }

//----------------------------------------------------------------------------
// MsgStoreHandler::isDraftMessage
// @see header
//----------------------------------------------------------------------------
bool MsgStoreHandler::isDraftMessage(int msgId)
{       
    bool draftmsg = false;
    CMsvEntry* cEntry = NULL;
    TRAPD(err, cEntry = iMsvSession->GetEntryL(msgId));
    if ( err == KErrNone)
    {
        TMsvEntry msvEntry = cEntry->Entry();
        TMsvId parent = msvEntry.Parent();
        if(parent == KMsvDraftEntryIdValue)
        {
            draftmsg = true;  
        }
    }

    delete cEntry;
    return draftmsg;
}
// End of file
