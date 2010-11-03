/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Mail Viewer attachment Dialog exit observer.
*
*/


#ifndef MMSGMAILATTACHMENTDIALOGOBSERVER_H
#define MMSGMAILATTACHMENTDIALOGOBSERVER_H

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  MMsgMailAttachmentDialogObserver interface.
*
*  @since Series 60 5.0
*/
class MMsgMailAttachmentDialogObserver
    {
    public:  // destructor
        /**
        * Destructor.
        */
        virtual ~MMsgMailAttachmentDialogObserver(){};

    public: // New functions

        /**
        * @since Series 60 5.0
        * Called when exiting a attachment dialog.
        */
        virtual void AttachmentDialogExit() = 0;

    };

#endif      // MMSGMAILATTACHMENTDIALOGOBSERVER_H

// End of File

