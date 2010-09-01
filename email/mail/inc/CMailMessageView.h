/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Mesasge view
*
*/



#ifndef CMAILMESSAGEVIEW_H
#define CMAILMESSAGEVIEW_H

//  INCLUDES
#include <MMailMessageView.h>
#include <ecom/ecom.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Mail message view base class.
*
*  @since Series 60 3.0
*/
class CMailMessageView : public CBase,
    public MMailMessageView
    {
    public:  // Constructors and destructor

        /**
        * NewL
        * @param aImplementationUid
        */
        static CMailMessageView* NewL(
            const TUid& aImplementationUid );

        /**
        * Destructor.
        */
        ~CMailMessageView();

    public: // New functions

        static void ListL( RImplInfoPtrArray& aImplInfoArray );

	protected: // Functions from base classes 
		// From MMailMessageView
		void ViewMessageL(
			RFileReadStream& aReadStream,
			MMailAppUiInterface& aUICallBack);
		// From MMailMessageView
        const TDesC& ResourceFile();			

	protected:

		// Default c'tor
		CMailMessageView();

    private:    // Data
        /// Implementation UID of the adapter
        TUid iDtor_ID_Key;

    };

// This includes the implementation of the instantiation functions and
// destructor
#include "CMailMessageView.inl"

#endif      // CMAILMESSAGEVIEW_H

// End of File
