/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMailViewFactory can be used to create mail view plugins.
*
*/


#ifndef CMAILVIEWFACTORY_H
#define CMAILVIEWFACTORY_H

// INCLUDES
#include <e32base.h>
#include <CMailMessageView.h>

// FORWARD DECLARATIONS
class CImplementationInformation;

//  CLASS DEFINITION

/**
*  Factory class for mail view plugins.
*
*  @lib 
*  @since 3.0
*/
class MailViewFactory 
    {
    public:
        /**
        * Creates all adapters and stores them in an array.
        * @return Array of adapters. Ownership is transferred.
        */
        IMPORT_C static CArrayPtr<CMailMessageView>* CreateViewPluginsL();

    private:

        /**
        * ResetAndDestroy() cleanup for an RImplInfoPtrArray.
        * @param aAny Array
        */
        static void CleanupImplArray( TAny* aAny );

        /**
        * ResetAndDestroy() cleanup for a CArrayPtr<CWPAdapter>.
        * @param aAny Array
        */
        static void CleanupAdapterArray( TAny* aAny );

        /** 
        * Comparator for two implementation information entries.
        * Compares the opaque data as a string.
        * @param aImpl1 First implementation to compare
        * @param aImpl2 Second implementation to compare
        */
        static TInt Compare( 
            const CImplementationInformation& aImpl1, 
            const CImplementationInformation& aImpl2 );
    };

#endif /* CMAILVIEWFACTORY_H */

// End of File
