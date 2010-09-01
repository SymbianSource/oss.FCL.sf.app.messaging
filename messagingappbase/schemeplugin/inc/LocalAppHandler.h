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
*      Declaration of Scheme handler interface implementation for 
*      "localapp:" URI scheme    
*
*/



#ifndef LOCAL_APP_HANDLER_H
#define LOCAL_APP_HANDLER_H

// INCLUDES

#include "BaseHandler.h"
#include <e32base.h>
#include <eikdoc.h>
#include <apparc.h>


// DATA TYPES

// FORWARD DECLARATION

// CLASS DECLARATION

/**
*  Scheme Handler IF implementation class for localapp uri scheme
*
*  @since Series60_2.6
*/
class CLocalAppHandler : public CBaseHandler
    {

    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @param - aUrl
        * @return The created object.
        */      
		static CLocalAppHandler* NewL( const TDesC& aUrl );

       /**
        * Destructor.
        */      
        virtual ~CLocalAppHandler();
        
	private: // Constructors

        /**
        * Constructor.
        */      
		CLocalAppHandler();

        /**
        * Second phase constructor. Leaves on failure.
		* @param - aUrl
        */      
		void ConstructL( const TDesC& aUrl );

    private:

        /**
		* Url Handler with embedding
        * All applications are always started standalone
		* @param -
		*/
		void HandleUrlEmbeddedL();

		/**
		* Url Handler without embedding
		* @param -
		*/
		void HandleUrlStandaloneL();

        /**
		* Url Handler
		* @param aLaunchEmbedded
		*/
		void HandleUrlL();

        void HandleMmsL();
    
        void HandleSmsL();
    
        void HandleCalendarL();
    
        void HandleContactsL();

        void HandleMessagingL();
        
        void HandleJamL();
    
        /**
		* Starts application identified in iAppUid
		* 
		*/
        void StartAppL();
        
	private: // Data
        CActiveSchedulerWait iWait;
        TUid                 iAppUid;
        TBool                iToday;
        TBool                iEmbedded;

	};

#endif /* def LOCAL_APP_HANDLER_H */
