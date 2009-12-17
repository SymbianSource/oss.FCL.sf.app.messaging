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
*      "mmsto" URI scheme
*
*/



#ifndef MMS_TO_HANDLER_H
#define MMS_TO_HANDLER_H

// INCLUDES

#include "BaseHandler.h"
#include <e32base.h>
#include <eikdoc.h>
#include <apparc.h>

// FORWARD DECLARATION
class CSendUi;

// CLASS DECLARATION

/**
*  Scheme Handler IF implementation class for mmsto uri scheme
*  @since Series60_2.6
*/
class CMmsToHandler : public CBaseHandler
    {

    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @param - aUrl
        * @return The created object.
        */      
		static CMmsToHandler* NewL( const TDesC& aUrl );

       /**
        * Destructor.
        */      
        virtual ~CMmsToHandler();

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

        enum TSchemeMmsToFields           //<Possible MmsTo fields
            {
            ESchemeMmsTo,                 ///< MmsTo.
            ESchemeSubject,               ///< Subject.
            ESchemeMsgBody,               ///< Message Body.
            ESchemeTo,                    ///< TO.
            ESchemeCc,                    ///< CC.
			ESchemeNoMore
            };

        /**
        * Constructor.
        */      
		CMmsToHandler();

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
		* @return TSchemeMmsToFields
		*/
		TSchemeMmsToFields GetHeaderPos(const TDesC& aHeader);

		/**
		* Get the next enumerated position of the specific field
		* @param aPos enumerated position of the field
        * @return Start position of the next field.
		*/
		TInt GetNextField( TInt aStart );

	private: // Data

        CSendUi* iSendUi;
        RFs iFs;
	};

#endif /* def MMS_TO_HANDLER_H */
