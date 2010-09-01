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
*     MMS Notification Viewer Application Server class definition.
*
*/



#ifndef NOTVIEWERAPPSERVER_H
#define NOTVIEWERAPPSERVER_H

// INCLUDES
#include <AknServerApp.h>                 // CAknServerApp

// CLASS DECLARATION

class CNotViewerAppServer : public CAknAppServer
    {
    public: // from CAknAppServer
    
    /*
    * Creates an instance of the service that this server apps supports
    */
    CApaAppServiceBase* CreateServiceL( TUid aServiceType ) const;
    
	/*
	* Destructor
	*/
    ~CNotViewerAppServer( );
    
    protected: // from CPolicyServer
    
    /*
    * An empty implementation of this pure virtual CPolicyServer function
    */
    virtual TCustomResult CustomFailureActionL(const RMessage2& aMsg, TInt aAction, const TSecurityInfo& aMissing);
    };

#endif      //  NOTVIEWERAPPSERVER_H
  
// End of File
