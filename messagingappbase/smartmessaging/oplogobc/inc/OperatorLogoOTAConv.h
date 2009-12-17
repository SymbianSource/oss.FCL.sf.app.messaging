/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Image conversion
*
*/



#ifndef COPERATORLOGOOTACONV_H
#define COPERATORLOGOOTACONV_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CActiveSchedulerWait;
class CFbsBitmap;

// CLASS DECLARATION

/**
* Image convertor class for operatorlogo biocontrol
*/
NONSHARABLE_CLASS( COperatorLogoOTAConv ) : public CActive
    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static COperatorLogoOTAConv* NewL();

        /**
        * Two-phased constructor.
        */
        static COperatorLogoOTAConv* NewLC();

        /**
        * Destructor.
        */
        virtual ~COperatorLogoOTAConv();

    public: // New functions

        /**
         * Converts OTA-image to mbm.
         * @param aFile path and filename of the image
         * @param aBitmap bitmap to be created
         */
        void ConvertImageL( const TDesC& aFile,
                            CFbsBitmap& aBitmap );

    protected:  // New functions

        void StartWait();

        void StopWait();


    protected:  // Functions from base classes

        void DoCancel();

        void RunL();

    private:

        /**
        * C++ default constructor.
        */
        COperatorLogoOTAConv();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:    // Data
        
        CActiveSchedulerWait iWait;

    };
#endif // COPERATORLOGOOTACONV_H

// End of File
