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
*     Server-side message editor/viewer service implementations
*
*/



#ifndef MUIUMSGEDITORSERVICESESSION_H
#define MUIUMSGEDITORSERVICESESSION_H

#include <f32file.h>
#include <eikdoc.h>
#include <aknserverapp.h>

enum TMuiuMsgEditorServiceCmds
	{
	EMuiuMsgEditorServiceCmdOpen = RApaAppServiceBase::KServiceCmdBase
	};



/**
* Server-side message editor/viewer service implementations
*/
class CMuiuMsgEditorServiceSession : public CAknAppServiceBase
	{
	public:
		/**
		* C++ Constructor
		*/
		IMPORT_C CMuiuMsgEditorServiceSession();
		
		/**
		* C++ Destructor
		*/
		~CMuiuMsgEditorServiceSession();

	protected: // from CSession2
		/**
		* Just basecalls CAknAppServiceBase
		*/
		void CreateL();
		
		/**
		* Checks the function from aMessage. If it is
		* EMuiuMsgEditorServiceCmdOpen -> calls
		* OpenEmbedded. Otherwise basecalls CAknAppServiceBase.
		*/
		void ServiceL(const RMessage2& aMessage);
		
		/**
		* Basecalls CAknAppServiceBase.
		*/
		void ServiceError(const RMessage2& aMessage,TInt aError);

	private:
		/**
		* Constructs TEditorParameters from aMessage
		* Externalizes the params to store and restore
		* CEikDocument from there (ie. starts messaging
		* editor/viewer). Completes aMessage.
		*/
		void OpenEmbeddedL( const RMessage2& aMessage );

	private:
		CEikDocument* iDoc;
	};

#endif


// End of file