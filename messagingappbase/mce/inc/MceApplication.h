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
*     Application class for Mce.
*
*/



#ifndef MCEAPPLICATION_H
#define MCEAPPLICATION_H

//  INCLUDES
#include <aknapp.h>     // CEikApplication

// CLASS DECLARATION

/**
*  Application class for Mce.
*/
class CMceApplication : public CAknApplication
    {
    public: // Functions from base classes

        /**
        * From CApaApplication 
        */
        TUid AppDllUid() const;

    private: // Functions from base classes
    
        /**
        * From CEikApplication 
        */
        CApaDocument* CreateDocumentL();        

    };


#endif

// End of file
