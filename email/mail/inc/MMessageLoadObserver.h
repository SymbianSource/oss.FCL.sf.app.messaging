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
* Description:  Message load observer.
*
*/



#ifndef MMESSAGELOADOBSERVER_H
#define MMESSAGELOADOBSERVER_H


// FORWARD DECLARATIONS
class CMailMessage;

// CLASS DECLARATION

/**
*  MMessageLoadObserver interface.
*
*  @since Series 60 3.0
*/
class MMessageLoadObserver
    {
    public:  // destructor
        /**
        * Destructor.
        */
        virtual ~MMessageLoadObserver(){};

    public: // New functions

        /**
        * MessageLoadedL.
        * @since Series 60 3.0
        * Called when loading a message.
        * @param aStatus loading status
        * @param aMessage message that contains loaded parts. 
        */
        virtual void MessageLoadingL(TInt aStatus, CMailMessage& aMessage) = 0;

    };

#endif      // MMESSAGELOADOBSERVER_H

// End of File
