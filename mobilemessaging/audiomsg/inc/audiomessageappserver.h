/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides server applicaion prototypes
*
*/




#ifndef AUDIOMESSAGEAPPSERVER_H
#define AUDIOMESSAGEAPPSERVER_H

#include <AknServerApp.h> 

/**
 *  CAudioMessageAppServer
 *
 *  Applicationserver class
 *
 *  @lib audiomessage.exe
 *  @since S60 v3.1
 */
class CAudioMessageAppServer : public CAknAppServer
    {
	// from CAknAppServer
    public: 
	/*
	* Creates an instance of the service aServiceType
	*/
    CApaAppServiceBase* CreateServiceL( TUid aServiceType ) const;

	/*
	* Destructor
	*/
    ~CAudioMessageAppServer( );
	
	// from CPolicyServer
    protected: 
    /*
    * An empty implementation of this pure virtual CPolicyServer function
    */
	TCustomResult CustomFailureActionL(const RMessage2& aMsg, TInt aAction, 
		const TSecurityInfo& aMissing);

    };

#endif      //  AUDIOMESSAGEAPPSERVER_H
  
