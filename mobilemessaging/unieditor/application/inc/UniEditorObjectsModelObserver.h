/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CUniEditorObjectsModelObserver class definition.      
*
*/



#ifndef UNIEDITOROBJECTSMODELOBSERVER_H
#define UNIEDITOROBJECTSMODELOBSERVER_H

// ========== INCLUDE FILES ================================

#include <uniobjectsmodelobserver.h>

// ========== FORWARD DECLARATIONS =========================

class CUniEditorDocument;
class CMsgAttachmentInfo;

// ========== CLASS DECLARATION ============================

/**
* CUniEditorObjectsModelObserver
*
* @lib unieditor.app
* @since 3.2
*/
class CUniEditorObjectsModelObserver : public CBase, 
                                       public MUniObjectsModelObserver
    {
    public:

        /**
        * Constructor.
        *
        * @param    aDocument   Reference to UniEditor Document class
        */
        CUniEditorObjectsModelObserver( CUniEditorDocument& aDocument,
                                        CUniEditorAppUi& aAppUi );

        /**
        * Destructor.
        */
        ~CUniEditorObjectsModelObserver();

        /**
        * From MMsgAttachmentModelObserver
        *
        * Deprecated.
        */
        void NotifyChanges( TMsgAttachmentCommand aCommand,
                            CMsgAttachmentInfo* aAttachmentInfo );

        /**
        * From MMsgAttachmentModelObserver
        *
        * Used for communication between Objects
        * view and UniEditor when adding or removing
        * objects
        *
        * @param    aCommand    Command id
        * @param    aAttachmentInfo Attachment affected by the command
        */
        void NotifyChangesL( TMsgAttachmentCommand aCommand,
                             CMsgAttachmentInfo* aAttachmentInfo );

        /**
        * From MMsgAttachmentModelObserver
        */
        RFile GetAttachmentFileL( TMsvAttachmentId aId );

        /*
        * Used for communication between Objects
        * view and UniEditor when layout is changed
        * from Objects view
        * 
        * @param    aLayout New layout, EImageFirst or ETextFirst
        */
        void NotifyOrderChangeL( TUniLayout aLayout );

    private:

        CUniEditorDocument& iDocument;
        CUniEditorAppUi& iAppUi;
    };

#endif // UNIEDITOROBJECTSMODELOBSERVER_H

// End of File
