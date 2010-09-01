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
*     MMS Notification Viewer Application class definition.
*
*/



#ifndef NOTVIEWERAPP_H
#define NOTVIEWERAPP_H

// INCLUDES
#include <aknapp.h>                 // CAknApplication

// CONSTANTS
const TUid KUidNotViewer = { 0x101F86A0 }; 
// Note that the UID values are recorded in three places:
// - CPP source file (as pragma data_seg)
// - header file (below, as a constant for AppDllUid() method)
// - MMP file (i.e. also in the makefile)

// FORWARD DECLARATION

class CApaAppServer;

// CLASS DECLARATION

/**
*  CNotViewerApplication class
*  Needed by the EIKON framework
*/
class CNotViewerApplication : public CAknApplication
    {
    private:
        
        /**
        * From CApaApplication.
        */
        CApaDocument* CreateDocumentL();

        /**
        * From CApaApplication.
        */
        TUid AppDllUid() const;
        
        /**
        * Called by CEikonEnv when new server application is created 
        */
        void NewAppServerL( CApaAppServer*& aAppServer );

    };

#endif      //  NOTVIEWERAPP_H
  
// End of File
