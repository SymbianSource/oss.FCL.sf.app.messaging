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
*     Sms Viewer
*
*/



#ifndef MSGSMSVIEWERAPP_H
#define MSGSMSVIEWERAPP_H

//  INCLUDES
#include <eikapp.h>                 // CEikApplication
#include <AknServerApp.h>

// CONSTANTS
const TUid KUidSmsViewer ={0x100058BD};
// Note that the UID values are recorded in three places:
// - CPP source file (as pragma data_seg)
// - header file (below, as a constant for AppDllUid() method)
// - MMP file (i.e. also in the makefile)

// CLASS DECLARATION

/**
*  CmsgSmsViewerApplication class
*  Needed by the EIKON framework
*/
class CMsgSmsViewerApplication : public CEikApplication
    {
    private:  
    void NewAppServerL( CApaAppServer*& aAppServer );
    CApaDocument* CreateDocumentL();    // abstract in CEikApplication
    TUid AppDllUid() const;             // abstract in CApaApplication
    };

#endif      //  MSGSMSVIEWERAPP_H
  
// End of File
