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
* Description:   Defines class CNcnClass0Viewer.
*
*/



#ifndef NCNCLASS0VIEWER_H
#define NCNCLASS0VIEWER_H

//  INCLUDES
#include <e32base.h>
#include <msvstd.h>

// FORWARD DECLARATIONS
class CMsvOperation;
class CNcnMsvSessionHandler;

// CLASS DECLARATION

/**
 * Handles the display of Class0 SMSes.
 */
class CNcnClass0Viewer :
    public CActive
    {
    private:  // Constructors and destructor

        /**
         * C++ default constructor.
         * @param aMsvSessionHandler Msv session handler reference.
         */
        CNcnClass0Viewer( CNcnMsvSessionHandler& aMsvSessionHandler );
        
        /**
         * 2nd. phase constructor.
         */
        void ConstructL();

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aMsvSessionHandler Msv session handler reference.
         */
        static CNcnClass0Viewer* NewL( CNcnMsvSessionHandler& aMsvSessionHandler );

        /**
         * Destructor.
         */
        virtual ~CNcnClass0Viewer();

    public: // From CActive
    
        /**
         * RunL
         */
        void RunL();

        /**
         * RunError
         * @param aError the error returned
         *  @return error
         */
        TInt RunError( TInt aError );

        /**
         * DoCancel
         */
        void DoCancel();

    public:  // Other public methods
        /**
         * Shows class0 sms.
         * @param aEntry Class0 sms entry to show.
         */
        void ShowClass0SmsL( const TMsvEntry& aEntry );

    private: // Data
    
        // operation associated with entry viewing
        CMsvOperation* iMsvOperation;       
        
        // msv session handler reference
        CNcnMsvSessionHandler& iMsvSessionHandler;
    };

#endif    // NCNCLASS0VIEWER_H

// End of File
