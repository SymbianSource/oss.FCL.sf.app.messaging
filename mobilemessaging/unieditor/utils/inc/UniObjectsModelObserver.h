/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Unified Message Editor model observer.
*
*/


#ifndef M_UNIOBJECTSMODELOBSERVER_H
#define M_UNIOBJECTSMODELOBSERVER_H

// ========== INCLUDE FILES ================================

#include <unimodelconst.h>
#include <MsgAttachmentModelObserver.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgAttachmentInfo;

// ========== CLASS DECLARATION ============================

/**
*  Attachment model observer.
*
*  @lib UniUtils.lib
*
*  @since S60 3.2
*/
class MUniObjectsModelObserver : public MMsgAttachmentModelObserver
    {

    public:

    /**
     * This function is called when attachment model adds or removes
     * attachments from its list.
     *
     * @since S60 3.2
     *
     * @param aCommand Given command that has caused the change.
     * @param aAttachmentInfo Attachment info that is affected by the change.
     */
    virtual void NotifyChangesL( TMsgAttachmentCommand aCommand, 
                                 CMsgAttachmentInfo* aAttachmentInfo ) = 0;
    
    /**
     * This function is called when attachment order of the objects 
     * in the model is changed
     *
     * @since S60 3.2
     *
     * @param aLayout  New layout.
     */
    virtual void NotifyOrderChangeL( TUniLayout aLayout ) = 0;
    
    };

#endif // M_UNIOBJECTSMODELOBSERVER_H

// End of File
