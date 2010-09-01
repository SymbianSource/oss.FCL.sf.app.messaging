/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       SMS Details Ecom Plugin definition.
*
*/



#ifndef __SMSDETAILSPLUGIN_H
#define __SMSDETAILSPLUGIN_H


// INCLUDES

#include <e32std.h>
#include <csmsgetdetdescinterface.h>

// FORWARD DECLARATIONS

class RFs;
class CSmsMessage;
class CContactMatcher;
class CStringResourceReader;

// CLASS DECLARATION

/**
* An ECOM plugin to create the details and description values for an SMS
* Overrides the default implementation by Symbian.
*
* @since S60 3.2
*/ 
class CSmsDetailsPlugin : public CSmsGetDetDescInterface
	{
    public:

        /**
        * Constructor
        */
    	static CSmsDetailsPlugin* NewL();
    	
    	/**
    	* Destructor
    	*/
    	virtual ~CSmsDetailsPlugin();

        /**
        * From CSmsGetDetDescInterface
        */
    	virtual TInt GetDetails(RFs& aFs, const CSmsMessage& aMessage, TDes& aDetails, TInt aMaxLength);

        /**
        * From CSmsGetDetDescInterface
        */
    	virtual TInt GetDetails(RFs& aFs, const TDesC& aFromAddress, TDes& aDetails, TInt aMaxLength);

        /**
        * From CSmsGetDetDescInterface
        */
    	virtual TInt GetDescription(const CSmsMessage& aMessage, TDes& aDescription, TInt aMaxLength);

    private:

        /**
        * Constructor
        */
    	CSmsDetailsPlugin();

        /**
        * CreateResourceReaderL
        */
        void CreateResourceReaderL( RFs* aFs );    

        /**
        * DoGetDetailsL
        */
        void DoGetDetailsL(RFs& aFs, const TDesC& aFromAddress, TDes& aDetails, TInt aMaxLength);

        /**
        * DoGetDescriptionL
        */
        TBool DoGetDescriptionL(const CSmsMessage& aMessage, TDes& aDescription, TInt aMaxLength);

        /**
        * ExtractDescriptionFromMessage
        */
        void ExtractDescriptionFromMessage(const CSmsMessage& aMessage, TDes& aDescription, TInt aMaxLength);

    private: // Threading related
        static TInt CntMatchThreadFunction( TAny* aRequestData );
        static void CntMatchThreadFunctionL( TAny* aRequestData );

    private:
        CStringResourceReader* iResourceReader;
        CContactMatcher* iContactMatcher;
        RThread iCntMatchThread;
	};

#endif // __SMSDETAILSPLUGIN_H
