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
* Description:  
*     CMsgEditorShutter declaration file
*
*/



#ifndef __MSGEDITORSHUTTER_H
#define __MSGEDITORSHUTTER_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>
#include <MsgEditorDocument.h>  
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* CMsgEditorShutter
*
* Utility to listen Messaging editor PubSub property.
* Initialises application shutdown if somebody forces 
* open editor count to 0
*
* @lib MsgEditorAppUi.dll
* @since 3.0
*/
NONSHARABLE_CLASS( CMsgEditorShutter ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        */
        static CMsgEditorShutter* NewL(CMsgEditorDocument& aDoc);
        
        /**
        * Destructor.
        */
        virtual ~CMsgEditorShutter();

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Private C++ constructor.
        */
        CMsgEditorShutter(CMsgEditorDocument& aDoc);

    private:

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();

    private:    // Data

        RProperty iOpenAppProperty;
        RProperty iOpenAppStandAloneProperty;
        CMsgEditorDocument& iDoc; 
    };

#endif      // __MSGEDITORSHUTTER_H
            
// End of File
