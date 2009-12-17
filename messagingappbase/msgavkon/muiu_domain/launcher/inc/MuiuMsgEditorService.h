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
* Description:  
*     Client-side service implementations
*
*/



#ifndef MUIUMSGEDITORSERVICE_H
#define MUIUMSGEDITORSERVICE_H

#include <f32file.h>
#include <AknServerApp.h>
#include <msvapi.h>
#include <MuiuMsgEditorLauncher.h>
#include <MuiuMsgEditorServiceConst.h>




/**
* Lower level client side service class
*/
class RMuiuMsgEditorService : public RAknAppServiceBase
	{
	public: 
		/**
		* Sends command EMuiuMsgEditorServiceCmdOpen to the server side
		* with parameters aParams (excluding iSpecialAppId)
		*/
		void OpenEntryL( const TEditorParameters aParams );
		
	private: // From RApaAppServiceBase
		/**
		* Returns the uid of the service
		*/
		TUid ServiceUid() const;
	};


/**
* Higher level client side service class
*/
class CMuiuMsgEditorService : public CBase
	{
	public:
		/**
		* C++ Constructor
		*/
		static CMuiuMsgEditorService* NewL( const TEditorParameters aParams, TUid aMtm, MAknServerAppExitObserver* aObserver );
		
		/**
		* C++ Destructor
		*/
		~CMuiuMsgEditorService();
		
	private:
		/**
		* C++ Constructor
		*/
		CMuiuMsgEditorService( TUid aMtm );
	
		/**
		* 2nd phase constructor
		* First calls Discover to find the right application with aParams.
		* Then calls iService to connect to right application.
		* Creates an instance of CApaServerAppExitMonitor		
		* Creates an instance of CMuiuMsgEditorService.
		* Sets itself as observer for the service.
		* @params IN:
		* 	aParams	- 	parameters for the service so that it
		*				knows which application to start
		*	aObserver -	pointer of MAknServerAppExitObserver to be passed to iMonitor
		*/
		void ConstructL( const TEditorParameters aParams, MAknServerAppExitObserver* aObserver );

		/**
		* DiscoverL
		* Finds the right application id to be started and returns it.
		* First checks if TEditorParameters.iSpecialAppId is set. If it is,
		* returns that. Then connects RApaLsSession and initializes
		* it with GetServerApps command. Then gets all the services that
		* implement the service by using GetServiceImplementationsLC. 
		* Calls TryMatchL for each service and if there's a match,
		* the appId is returned. If no implementation is found, 0 is returned.
		*/
    	TUid DiscoverL( const TEditorParameters aParams );

		/**
		* TryMatchL
		* If TEditorParameters.iFlags has either EMsgReadOnly
		* or EMsgPreviewMessage bit set, this function looks for a service
		* that implements the KMsgServiceView part of the service. Otherwise
		* looks for KMsgServiceEdit part.
		* Reads the parameters ( service type (View/Edit), MtmType) from 
		* the opaque data. If type and mtm match, sets aAppUi as
		* the application id and returns ETrue. If not found, returns EFalse.
		*/
    	TBool TryMatchL(  const TEditorParameters aParams, const TApaAppServiceInfo aInfo, TUid& aAppUid  );

	private:
		// The mtm type we are looking the service for
		TUid						iMtm;
		// Lower level client-side service implementation
		RMuiuMsgEditorService 		iService;
		// Monitor for checking events from the server side
		CApaServerAppExitMonitor* 	iMonitor;
	};

#endif


// End of file
