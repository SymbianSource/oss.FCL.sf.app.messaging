/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Mail icalviewer attachment loader
*
*/


#ifndef __CICALATTALOADER_H
#define __CICALATTALOADER_H

#include <e32base.h>
#include <MMessageLoadObserver.h>

// FORWARD DECLARATIONS
class MMailAppUiInterface;

// CLASS DECLARATION

/**
*  Loads message attachments.
*/
class CICalAttaLoader : public CBase, MMessageLoadObserver
    {
    public:  // Constructors and destructor

        /**
        * NewL
        * @param aAppUI App UI call back
        */
        static CICalAttaLoader* NewL( MMailAppUiInterface& aAppUI );

        /**
        * Destructor.
        */
        ~CICalAttaLoader();
        
    public: // new functions
    
        /**
        * Starts loading attachments.
        * @param aMessage Mail message
        */
        void StartLoadingL( CMailMessage& aMessage );

        /**
        * FinishedWithAttachments.
        * @return ETrue if loading is finished.
        */        
        TBool FinishedWithAttachments();

    protected: // from MMessageLoadObserver
    
        void MessageLoadingL( TInt aStatus, CMailMessage& aMessage );

    private: // Constructors and destructor
        
        CICalAttaLoader( MMailAppUiInterface& aAppUI );
        
    private: // Data
    
        MMailAppUiInterface& iAppUI;
        
        TBool iFinishedWithAttachments;        
    };

#endif      // __CICALATTALOADER_H

// End of File
