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
* Description:  App Server
*
*/



#ifndef CMSGMAILVIEWERAPPSERVER_H
#define CMSGMAILVIEWERAPPSERVER_H

//  INCLUDES
#include <AknServerApp.h>                 // CAknServerApp
#include <apaserverapp.h>


// CLASS DECLARATION

/**
*  Application server
*
*  @since Series60_30
*/
class CMsgMailViewerAppServer : public CAknAppServer
    {
    public: // from CAknAppServer
    	CApaAppServiceBase* CreateServiceL( TUid aServiceType ) const;
    	
    	~CMsgMailViewerAppServer();
    
    protected: // from CPolicyServer
		virtual TCustomResult CustomFailureActionL(
			const RMessage2& aMsg, 
			TInt aAction, 
			const TSecurityInfo& aMissing);
    };

#endif      // CMSGMAILVIEWERAPPSERVER_H
            
// End of File
