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
*     Postcard server Application class definition.
*
*/



#ifndef POSTCARDAPPSERVER_H
#define POSTCARDAPPSERVER_H

// INCLUDES
#include <AknServerApp.h>                 // CAknServerApp

// CLASS DECLARATION

class CPostcardAppServer : public CAknAppServer
    {

    public: // from CAknAppServer

	/*
	* Creates an instance of the service aServiceType
	*/
    CApaAppServiceBase* CreateServiceL( TUid aServiceType ) const;

	/*
	* Destructor
	*/
    ~CPostcardAppServer( );

    protected: // from CPolicyServer

    /*
    * An empty implementation of this pure virtual CPolicyServer function
    */
	TCustomResult CustomFailureActionL(const RMessage2& aMsg, TInt aAction, const TSecurityInfo& aMissing);

    };

#endif      //  POSTCARDAPPSERVER_H
  
// End of File
