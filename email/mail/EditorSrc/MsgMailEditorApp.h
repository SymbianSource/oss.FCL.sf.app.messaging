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


#ifndef MSGMAILEDITORAPP_H
#define MSGMAILEDITORAPP_H

// INCLUDES
#include <aknapp.h>
#include <AknServerApp.h>

// CLASS DECLARATION

/**
* CMsgMailEditorApp application class. 
*/
class CMsgMailEditorApp : public CAknApplication
    {
    public:
    	void NewAppServerL( CApaAppServer*& aAppServer );
    
    private:

        /**
        * From CApaApplication, creates CMsgMailEditorDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidMsgMailEditor).
        * @return The value of KUidMsgMailEditor.
        */
        TUid AppDllUid() const;
    };

#endif // MSGMAILEDITORAPP_H

// End of File

