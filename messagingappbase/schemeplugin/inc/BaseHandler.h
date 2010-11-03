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
*      Declaration of class CBaseHandler.   
*
*/



#ifndef BASE_HANDLER_H
#define BASE_HANDLER_H

// INCLUDES

#include <schemehandler.h>
#include <e32base.h>

// FORWARD DECLARATION
class CPtrC16Array;
class CMessageData;

// CONSTS

// CLASS DECLARATION
enum TSchemePluginPanic
	{
	EUnrecognisedState			= 1
	};

/**
*  Scheme Handler IF definition class
*  @since Series60_2.6
*/
class CBaseHandler : public CSchemeHandler
    {
	public:
	
		enum TRecipientTypes
			{
			KToRecipient,
			KCcRecipient
			};
			
    public: // From base class

		/**
		* Url Handler with embedding
		* @param -
		*/
		virtual void HandleUrlEmbeddedL() = 0;

		/**
		* Url Handler without embedding
		* @param -
		*/
		virtual void HandleUrlStandaloneL() = 0;

		/**
		* Observer
		* @param - aSchemeDoc
		*/
        void Observer( MAknServerAppExitObserver* aSchemeDoc );
		
	protected:

        /**
        * Constructor.
        */      
		CBaseHandler();

       /**
        * Destructor.
        */      
        virtual ~CBaseHandler();

        /**
        * Second phase constructor. Leaves on failure.
		* BaseConstructL must be called from inherited ConstructL
		* preferably before do anything
		* @param - aUrl
        */      
		void BaseConstructL( const TDesC& aUrl );

	protected: // New functions

        /**
        * Separates items from string to array.
        * 
        * @param aSeparator Separator mark that separates items.
        * @param aString String to separate
        * @param aArray Result array
        */
        void SeparateItemsToArrayL( 
			const TDesC& aSeparator, 
			const TDesC& aString, 
			CPtrC16Array& aArray ) const;
			
			/**
        * Separates items from string to array of messageData.
        * 
        * @param aSeparator Separator mark that separates items.
        * @param aString String to separate
        * @param aMessageData Object where items are appended.
        */
        void SeparateAndAppendAddressL(
			const TDesC& aSeparator, 
			const TDesC& aString, 
			CMessageData& aMessageData,
			const TInt aRecipientType = KToRecipient ) const;	
			
        /**
        * Notify client on completion of editing of an embedded document.
        * @param.
        */
		void NotifyClient();
        
        /**
        * Returns the minimum value of the array.
        * @param.
        * @return
        */
        TInt Minimum( RArray<TInt>& aArray ) const;

        /**
        * Launch SchemeApp passing Generic Params List
        * @param -
        * @return -
        */
        void LaunchSchemeAppWithCommandLineL();


	protected: // Data

        MAknServerAppExitObserver* iSchemeDoc; ///< Used.

    protected: // Data

		// Results of parsing
		HBufC* iParsedUrl; ///< Owned.
	};

#endif /* def BASE_HANDLER_H */
