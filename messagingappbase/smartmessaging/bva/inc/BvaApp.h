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
*     BVA application class.
*
*/



#ifndef _BVAAPP_H_
#define _BVAAPP_H_

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
 * CBvaApp application class.
 * Provides factory to create concrete document object.
 */
NONSHARABLE_CLASS( CBvaApp ) : public CAknApplication
    {
    public: // from CEikApplication
        /**
         * This overrides the default implementation, 
		 *	that changes document name to be Application name. 
         */
        void GetDefaultDocumentFileName(TFileName& aDocumentName) const;

    private: // from CApaApplication

        /**
         * From CApaApplication, creates CBvaDocument document object.
         * @return A pointer to the created document object.
         */
        CApaDocument* CreateDocumentL();

        /**
         * From CApaApplication, returns application's UID (KUidBVA).
         * @return The value of KUidBVA.
         */
        TUid AppDllUid() const;
    };

#endif //_BVAAPP_H_

// End of File
