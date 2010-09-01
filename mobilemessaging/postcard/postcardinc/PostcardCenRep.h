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
*     Postcard application's interface to central repository
*
*/



#ifndef __POSTCARCENREP_H__
#define __POSTCARCENREP_H__

#include <e32base.h>

#include "PostcardPrivateCRKeys.h"

class CPostcardCenRep : public CBase
    {

public: // Constructors and destructor

    /**
    * Two-phased constructor.
    * @return pointer to created CPostcardCenRep
    */
    static CPostcardCenRep* NewL();

    /**
    * Destructor.
    */
    ~CPostcardCenRep();

public:

    /**
    * Get int data for a key
    * @param aKey Key to read data
    * @return Key value
    */
    TInt Get(TUint32 aKey) const;

    /**
    * Get string data for a key
    * @param aKey Key to read data
    * @return Key value
    */
    const TDesC& GetString(TUint32 aKey) const;

    /**
    * Get Postcard feature (variant) bits
    * @return Postcard variation bits
    */
    TInt FeatureBits() const;

private: // constructors
    CPostcardCenRep();

    /**
    * EPOC constructor
    */
    void ConstructL();

private: // data

    // Int and string data types are supported
    static const TInt8 iIsStringType[KPocaKeyCount];

    // Data read from Postcard central repository is cached here
    union
        {
        TInt asInt;
        HBufC* asString;
        } iData[KPocaKeyCount];

    // Postcard feature bits read from Muiu variant repository
    TInt iFeatureBits;
    };

#endif // __POSTCARCENREP_H__
