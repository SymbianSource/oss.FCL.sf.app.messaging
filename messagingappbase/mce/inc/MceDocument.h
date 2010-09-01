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
*     Defines the CMceDocument class
*
*/



#ifndef MCEDOCUMENT_H
#define MCEDOCUMENT_H

//  INCLUDES
#include <AknDoc.h>

// CLASS DECLARATION

class CMceDocument : public CAknDocument
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMceDocument* NewL( CEikApplication& aApp );
        
        /**
        * Destructor.
        */
        virtual ~CMceDocument();

        /**
        * From CEikDocument
        */
        CEikAppUi* CreateAppUiL();

    private:

        /**
        * Constructor.
        */
        CMceDocument(CEikApplication& aApp);

    };


#endif // MCEDOCUMENT_H

// End of file
