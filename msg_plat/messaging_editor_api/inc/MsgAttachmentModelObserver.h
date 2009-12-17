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
* Description:  MsgAttachmentModelObserver  declaration
*
*/



#ifndef INC_MSGATTACHMENTMODELOBSERVER_H
#define INC_MSGATTACHMENTMODELOBSERVER_H

// ========== INCLUDE FILES ================================

#include <f32file.h>
#include <cmsvattachment.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgAttachmentInfo;

// ========== CLASS DECLARATION ============================

/**
 * Attachment model observer.
 *
 */
class MMsgAttachmentModelObserver
    {
    public:
    
        enum TMsgAttachmentCommand
            {
            EMsgAttachmentAdded = 0,
            EMsgAttachmentRemoved
            };

    public:

        /**
        * This function that is called when attachment model adds or removes
        * attachments from its list.
        * @param aCommand
        * @param aAttachmentInfo
        */
        virtual void NotifyChanges( TMsgAttachmentCommand aCommand, CMsgAttachmentInfo* aAttachmentInfo ) = 0;

        /**
        * This function that is called when attachment model needs to
        * get access to an attachment file
        * @param aId    Attachment id
        * @return       Open (read-only) file handle
        */
        virtual RFile GetAttachmentFileL( TMsvAttachmentId aId ) = 0;

    };

#endif

// End of File
