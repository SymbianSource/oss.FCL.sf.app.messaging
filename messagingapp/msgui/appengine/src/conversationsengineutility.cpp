/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Utility class for conersationengine.
 */
#include "conversationsengineutility.h"
#include "convergedmessage.h"
#include <senduiconsts.h>

//---------------------------------------------------------------
// ConversationsEngineUtility::messageType
// @see header
//---------------------------------------------------------------
int ConversationsEngineUtility::messageType(TCsType value)
    {
    int mMessageType;
    switch(value)
        {
        case ECsSMS:
            mMessageType = ConvergedMessage::Sms;
            break;
        case ECsMMS:
            mMessageType = ConvergedMessage::Mms;
            break;
        case ECsBioMsg:
            mMessageType = ConvergedMessage::BioMsg;
            break;
        case ECsBlueTooth:
            mMessageType = ConvergedMessage::BT;
            break;
        case ECsRingingTone:
            mMessageType = ConvergedMessage::RingingTone;
            break;
        case ECsProvisioning:
            mMessageType = ConvergedMessage::Provisioning;
            break;
		case ECsBioMsg_VCard:
            mMessageType = ConvergedMessage::VCard;
            break;
		case ECsBioMsg_VCal:
		        mMessageType = ConvergedMessage::VCal;
            break;
        default:
            mMessageType = ConvergedMessage::Sms;
            break;
        }
    return mMessageType;
    }

//---------------------------------------------------------------
// ConversationsEngineUtility::mapMsgType
// @see header
//---------------------------------------------------------------
int ConversationsEngineUtility::messageType(TInt32 value)
    {
    int type = ConvergedMessage::Sms;
    
    switch(value)
        {
        case KSenduiMtmSmsUidValue:
            type = ConvergedMessage::Sms;
            break;
        case KSenduiMtmBtUidValue:
            type = ConvergedMessage::BT;
            break;
        case KSenduiMtmMmsUidValue:
            type = ConvergedMessage::Mms;
            break;
        case KSenduiMtmBioUidValue:
            type = ConvergedMessage::BioMsg;
            break;
        default:
            type =ConvergedMessage::Sms;
            break;
        }
    return (type);
    }


// End of file
