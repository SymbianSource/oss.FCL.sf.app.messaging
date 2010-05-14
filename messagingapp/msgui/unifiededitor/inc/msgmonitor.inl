/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: msgmonitor.inl
*/



// ========== INLINE METHODS ===============================

//---------------------------------------------------------------
// MsgMonitor::setSkipNote
// @see header file
//---------------------------------------------------------------
inline void MsgMonitor::setSkipNote(bool skip)
{
    mSkipNote = skip;
}

//---------------------------------------------------------------
// MsgMonitor::messageType
// @see header file
//---------------------------------------------------------------
inline ConvergedMessage::MessageType MsgMonitor::messageType()
{
    return mMessageType;
}

//---------------------------------------------------------------
// MsgMonitor::messageSize
// @see header file
//---------------------------------------------------------------
inline int MsgMonitor::messageSize()
{
    return mBodySize + mContainerSize + mSubjectSize;
}

//---------------------------------------------------------------
// MsgMonitor::bodySize
// @see header file
//---------------------------------------------------------------
inline int MsgMonitor::bodySize()
{
    return mBodySize;
}

//---------------------------------------------------------------
// MsgMonitor::containerSize
// @see header file
//---------------------------------------------------------------
inline int MsgMonitor::containerSize()
{
    return mContainerSize;
}

//---------------------------------------------------------------
// MsgMonitor::subjectSize
// @see header file
//---------------------------------------------------------------
inline int MsgMonitor::subjectSize()
{
    return mSubjectSize;
}

//---------------------------------------------------------------
// MsgMonitor::maxMmsSize
// @see header file
//---------------------------------------------------------------
inline int MsgMonitor::maxMmsSize()
{
    return mMaxMmsSize;
}

//---------------------------------------------------------------
// MsgMonitor::maxSmsRecipients
// @see header file
//---------------------------------------------------------------
inline int MsgMonitor::maxSmsRecipients()
{
    return mMaxSmsRecipients;
}

//---------------------------------------------------------------
// MsgMonitor::maxMmsRecipients
// @see header file
//---------------------------------------------------------------
inline int MsgMonitor::maxMmsRecipients()
{
    return mMaxMmsRecipients;
}

//---------------------------------------------------------------
// MsgMonitor::msgAddressCount
// @see header file
//---------------------------------------------------------------
inline int MsgMonitor::msgAddressCount()
{
    return mMsgCurrAddressCount;
}

//  End of File
