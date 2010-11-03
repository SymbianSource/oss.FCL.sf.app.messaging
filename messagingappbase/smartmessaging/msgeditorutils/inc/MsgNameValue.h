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
*     This class encapsulates a name and value pair.
*
*/




#ifndef _CMSGNAMEVALUE_H_
#define _CMSGNAMEVALUE_H_

// INCLUDES

#include <e32base.h>                    // CBase

// CLASS DECLARATION

/**
 * Class holds name and value text pairs.
 */
class CMsgNameValue : public CBase
    {
    public: // construction

         /**
         * Two phased constructor.
         * @param aName The field name.
         * @param aValue The field value.
         * @return The newly constructed object.
         */
        IMPORT_C static CMsgNameValue* NewL( HBufC* aName, HBufC* aValue );
        IMPORT_C static CMsgNameValue* NewL(const TDesC& aName, const TDesC& aValue);
        IMPORT_C static CMsgNameValue* NewLC(const TDesC& aName, const TDesC& aValue);

        /// Destructor
        IMPORT_C ~CMsgNameValue();
    
    public: // new functions

        /// @return Name or KNullDesC
        IMPORT_C const TDesC& Name() const;
        /// @return Value or KNullDesC
        IMPORT_C const TDesC& Value() const;

    private: // construction

        /**
         * Second phase constructor.
         * @param aName The field name.
         * @param aValue The field value.
         */
        void ConstructL( const TDesC& aName, const TDesC& aValue );

        /**
         * Second phase constructor.
         * @param aName The field name.
         * @param aValue The field value.
         */
        void ConstructL( HBufC* aName, HBufC* aValue );

    private: // hidden

        /// Default constructor.
        CMsgNameValue();

        /// Another constructor
        CMsgNameValue( HBufC* aName, HBufC* aValue );

        /// Copy contructor prohibited.
        CMsgNameValue(const CMsgNameValue& aSource);

        /// Assignment operator prohibited.
        const CMsgNameValue& operator=(const CMsgNameValue& aSource);

    private:
    
        /// Own. Pointer to the name.
        HBufC* iName;

        /// Own. Pointer to the value.
        HBufC* iValue;
    };

#endif // _CMSGNAMEVALUE_H_

// End of file
