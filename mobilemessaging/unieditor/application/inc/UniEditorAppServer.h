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
* Description:   Uni Editor server Application class definition.
*
*/



#ifndef __UNIEDITORAPPSERVER_H
#define __UNIEDITORAPPSERVER_H

// INCLUDES
#include <AknServerApp.h>

// CLASS DECLARATION

/**
* CUniEditorAppServer
*
* @since 3.2
*/
class CUniEditorAppServer : public CAknAppServer
    {
    public: // from CAknAppServer

        /*
        * Creates an instance of the service aServiceType
        */
        CApaAppServiceBase* CreateServiceL( TUid aServiceType ) const;

        /*
        * Destructor
        */
        virtual ~CUniEditorAppServer();

    protected: // from CPolicyServer

        /*
        * An empty implementation of this pure virtual CPolicyServer function
        */
        TCustomResult CustomFailureActionL( const RMessage2& aMsg, 
                                            TInt aAction, 
                                            const TSecurityInfo& aMissing );
    };

#endif      //  __UNIEDITORAPPSERVER_H
  
// End of File
