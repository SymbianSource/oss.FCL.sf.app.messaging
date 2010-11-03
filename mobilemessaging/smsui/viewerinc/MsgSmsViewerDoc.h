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



#ifndef     MSGSMSVIEWERDOCUMENT_H
#define     MSGSMSVIEWERDOCUMENT_H

//  INCLUDES
#include <MsgEditorDocument.h>       // CMsgEditorDocument

// CLASS DECLARATION

/**
*  CMsgSmsviewerDocument
*/
class CMsgSmsViewerDocument : public CMsgEditorDocument
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMsgSmsViewerDocument* NewL(CEikApplication& aApp);  
        
        /**
        * Destructor.
        */
        virtual ~CMsgSmsViewerDocument();

    public:
        /**
        * From CMsgEditorDocument
        * @return TMsvId of default folder of the new sms'es.
        */
        TMsvId DefaultMsgFolder() const;

        /**
        * From CMsgEditorDocument 
        * @return TMsvId of default service.
        */
        TMsvId DefaultMsgService() const;

    private: // Functions from base classes

        /**
        * From CMsgEditorDocument : Sets the information of the new sms entry.
        * @param aService look from CMsgEditorDocument
        * @param aFolder look from CMsgEditorDocument
        * @return TMsvId look from CMsgEditorDocument
        */
        TMsvId CreateNewL(TMsvId aService,TMsvId aFolder);

        /**
        * From CMsgEditorDocument 
        */
        void EntryChangedL();
     
    private:

        /**
        * C++ constructor.
        */
        CMsgSmsViewerDocument(CEikApplication& aApp);

        /**
        * Creates a new CMsgSmsViewerAppUi
        * @param none
        * @return a pointer to CMsgSmsViewerAppUi
        */
        CEikAppUi* CreateAppUiL();
        
    };

#endif      //  MSGSMSVIEWERDOCUMENT_H
            
// End of File
