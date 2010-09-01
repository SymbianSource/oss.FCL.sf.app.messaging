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
*     Declares main application class.
*
*/


#ifndef MSGMAILVIEWERAPP_H
#define MSGMAILVIEWERAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS

// CLASS DECLARATION

/**
* CMsgMailViewerApp application class.
* Provides factory to create concrete document object.
* 
*/
class CMsgMailViewerApp : public CAknApplication
    {
    public:
    
        void NewAppServerL( CApaAppServer*& aAppServer );
            
    private:

        /**
        * From CApaApplication, creates CMsgMailViewerDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidMsgMailViewer).
        * @return The value of KUidMsgMailViewer.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

