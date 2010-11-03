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
* Description:  App Server
*
*/



#ifndef CMSGMAILEDITORAPPSERVER_H
#define CMSGMAILEDITORAPPSERVER_H

//  INCLUDES
#include <AknServerApp.h>                 // CAknServerApp
#include <apaserverapp.h>


// CLASS DECLARATION

/**
*  Application server
*
*  @since Series60_30
*/
class CMsgMailEditorAppServer : public CAknAppServer
    {
    public: // from CAknAppServer
    	CApaAppServiceBase* CreateServiceL( TUid aServiceType ) const;
    	
    	~CMsgMailEditorAppServer();
    
    protected: // from CPolicyServer
		virtual TCustomResult CustomFailureActionL(
			const RMessage2& aMsg, 
			TInt aAction, 
			const TSecurityInfo& aMissing);
    };

#endif      // CMSGMAILEDITORAPPSERVER_H
            
// End of File
