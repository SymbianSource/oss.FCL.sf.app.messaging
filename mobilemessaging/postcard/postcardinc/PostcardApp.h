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
*     Postcard Application class definition.
*
*/



#ifndef _POSTCARDAPP_H
#define _POSTCARDAPP_H

// ========== INCLUDE FILES ================================

#include <aknapp.h>                     // CAknApplication

// FORWARD DECLARATION

class CApaAppServer;

/**
* CPostcardApp
*
* @lib postcard.exe
* @since 3.0
*/
class CPostcardApplication : public CAknApplication
    {

    private:     // From CEikApplication

    /**
    * Create CEditorDocument document object.
    */
    CApaDocument* CreateDocumentL();         // abstract in CEikApplication

	/**
	* Called by CEikonEnv when new server application is created 
	*/
	void NewAppServerL( CApaAppServer*& aAppServer );

    private:     // From CApaApplication (via CEikApplication)

    /**
    * Return KUideditor.
    */
    TUid AppDllUid() const;                 // abstract in CApaApplication

    };

#endif  // _POSTCARDAPP_H

// End of file
