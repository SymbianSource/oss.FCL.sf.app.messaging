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
* Description:   Uni Editor Application class definition.   
*
*/



#ifndef __UNIEDITORAPP_H
#define __UNIEDITORAPP_H

// INCLUDE FIlES

#include <aknapp.h>

// FORWARD DECLARATION

class CApaAppServer;

/**
* Application class definitions
*
* @since 3.2
*/
class CUniEditorApplication : public CAknApplication
    {
    private:     // From CEikApplication

        /**
        * Create CEditorDocument document object.
        */
        CApaDocument* CreateDocumentL();         // abstract in CEikApplication

        /**
        * Called by CEikonEnv when new server application is created 
        */
        void NewAppServerL( CApaAppServer*& aAppServer );

    private:

        /**
        * Return KUideditor.
        */
        TUid AppDllUid() const;                 // abstract in CApaApplication

    };

#endif  // __UNIEDITORAPP_H

// End of file
