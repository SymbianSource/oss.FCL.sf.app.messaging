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
*      Declaration of Scheme handler interface implementation 
*      for "sms:"scheme
*
*/



#ifndef SMS_HANDLER_H
#define SMS_HANDLER_H

// INCLUDES

#include "BaseHandler.h"
#include <e32base.h>
#include <eikdoc.h>
#include <apparc.h>


// FORWARD DECLARATION
class CSendUi;

// CLASS DECLARATION

/**
*  Scheme Handler IF implementation class for sms uri scheme
*  @since Series60_2.6
*/
class CSmsHandler : public CBaseHandler
    {

    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @param - aUrl
        * @return The created object.
        */      
		static CSmsHandler* NewL( const TDesC& aUrl );

       /**
        * Destructor.
        */      
        virtual ~CSmsHandler();

        /**
		* Url Handler with embedding
		* @param -
		*/
		void HandleUrlEmbeddedL();

		/**
		* Url Handler without embedding
		* @param -
		*/
		void HandleUrlStandaloneL();
        
	private: // Constructors

        enum TSchemeSmsFields           //<Possible sms fields
            {
            ESchemeSms,                   ///< Sms.
            ESchemeMsgBody,               ///< Message Body
            ESchemeTo,                    ///< TO.
			ESchemeNoMore
            };

        /**
        * Constructor.
        */      
		CSmsHandler();

        /**
        * Second phase constructor. Leaves on failure.
		* @param - aUrl
        */      
		void ConstructL( const TDesC& aUrl );

    private:

        /**
		* Url Handler
		* @param aLaunchEmbedded
		*/
		void HandleUrlL(TBool aLaunchEmbedded);

		/**
		* Get the appropriate filed from the url
		* @param aHeader the name of the field
        * @return Value of the header
		*/
		TPtrC GetField(const TDesC& aHeader);

		/**
		* Check if the field is in the url
		* @param aHeader the name of the field
		* @return TBool
		*/
		TBool IsHeader(const TDesC& aHeader);

		/**
		* Starting position of the specific field
		* @param aHeader the name of the field
		* @return TInt Starting position of the specific field
		*/
        TInt FieldStart(const TDesC& aHeader);

		/**
		* Ending position of the specific field
		* @param aHeader the name of the field
		* @return TInt Ending position of the specific field
		*/
		TInt FieldEnd(const TDesC& aHeader);

		/**
		* Get the enumerated position of the specific field
		* @param aHeader the name of the field
		* @return TSchemeSmsToFields
		*/
		TSchemeSmsFields GetHeaderPos(const TDesC& aHeader);

		/**
		* Get the next enumerated position of the specific field
		* @param aPos enumerated position of the field
        * @return Start position of the next field.
		*/
        TInt GetNextField( TInt aStart );

	private: // Data

        CSendUi* iSendUi;
	};

#endif /* def SMS_HANDLER_H */
