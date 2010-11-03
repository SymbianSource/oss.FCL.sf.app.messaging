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
* Description:   Provides audiomessageapp prototypes
*
*/




#ifndef _AUDIOMESSAGEAPP_H
#define _AUDIOMESSAGEAPP_H


#include <aknapp.h> 


class CApaAppServer;

/**
 *  CAudioMessageApplication
 *
 *  Application class
 *
 *  @lib audiomessage.exe
 *  @since S60 v3.1
 */

class CAudioMessageApplication : public CAknApplication
    {
	// From CEikApplication
	private:    
    /**
    * Create CEditorDocument document object.
    */
    CApaDocument* CreateDocumentL();    // abstract in CEikApplication

	/**
	* Called by CEikonEnv when new server application is created 
	*/
	void NewAppServerL( CApaAppServer*& aAppServer );

	// From CApaApplication (via CEikApplication)
    private:     

    /**
    * Return KUideditor.
    */
    TUid AppDllUid() const; // abstract in CApaApplication

    };

#endif  // _AUDIOMESSAGEAPP_H

