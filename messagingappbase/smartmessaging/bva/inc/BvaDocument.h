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
*     Declares document for BVA application.
*
*/



#ifndef _BVADOCUMENT_H_
#define _BVADOCUMENT_H_

// INCLUDES
#include <eikdoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
 *  CBvaDocument application class.
 */
NONSHARABLE_CLASS( CBvaDocument ) : public CEikDocument
    {
    public: // Construction & destruction
        /**
         * Two-phased constructor.
         * @param Reference to the aplication.
         * @return The newly created object.
         */
        static CBvaDocument* NewL(CEikApplication& aApp);

        /// Destructor.
        virtual ~CBvaDocument();

    public: // from CEikDocument
        void OpenFileL(CFileStore*& aFileStore, RFile& aFile);

    private: // construction

        /// constructor
        CBvaDocument(CEikApplication& aApp): CEikDocument(aApp) { }

    private: //from CEikDocument

        /**
         * Creates the AppUi.
         * @return The AppUi.
         */
        CEikAppUi* CreateAppUiL();

    };

#endif //_BVADOCUMENT_H_

// End of File
