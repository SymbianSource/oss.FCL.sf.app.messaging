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
*           Postcard application's interface central repository
*
*/



// Include files

#include <centralrepository.h>
#include <messaginginternalcrkeys.h>

#include "PostcardCenRep.h"
#include "PostcardPanic.h"

// Static constant data

// True if corresponding key refers to string data
const TInt8 CPostcardCenRep::iIsStringType[KPocaKeyCount] = 
    {
    ETrue,  // KPocaKeyServiceProvider
    EFalse, // KPocaKeyMandatoryFields
    EFalse, // KPocaKeyMaxGreetingLength
    EFalse, // KPocaKeyMaxNameLength
    EFalse, // KPocaKeyMaxInfoLength
    EFalse, // KPocaKeyMaxStreetLength
    EFalse, // KPocaKeyMaxZipLength
    EFalse, // KPocaKeyMaxCityLength
    EFalse, // KPocaKeyMaxStateLength
    EFalse, // KPocaKeyMaxCountryLength
    ETrue,  // KPocaKeyServiceSpecialFormat
    ETrue,  // KPocaKeyServiceSeparator
    ETrue   // KPocaKeyServiceReplaceString
    };

// Member functions

// ---------------------------------------------------------
//  Two-phased constructor
// ---------------------------------------------------------
CPostcardCenRep* CPostcardCenRep::NewL()
    {
    CPostcardCenRep* self = new (ELeave) CPostcardCenRep();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
//  Default destructor
// ---------------------------------------------------------
CPostcardCenRep::~CPostcardCenRep()
    {
    for(TInt i = 0; i < KPocaKeyCount; i++)
        {
        if (iIsStringType[i])
            {
            delete iData[i].asString;
            }
        }
    }

// ---------------------------------------------------------
// CPostcardCenRep
// ---------------------------------------------------------
CPostcardCenRep::CPostcardCenRep()
    {
    }

// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
void CPostcardCenRep::ConstructL()
    {
    // Read data from Postcard repository
    CRepository* cenrep = CRepository::NewLC(KCRUidPostcard);

    for(TInt i = 0; i < KPocaKeyCount; i++)
        {
        TUint32 key = KPocaKeyServiceProvider + i;
        if (iIsStringType[i])
            {
            HBufC* buf = iData[i].asString = HBufC::NewL(KCRPocaMaxString);
            TPtr des = buf->Des();
            User::LeaveIfError(cenrep->Get(key, des));
            iData[i].asString = buf->ReAllocL(buf->Length());
            }
        else
            {
            User::LeaveIfError(cenrep->Get(key, iData[i].asInt));
            }
        }
    CleanupStack::PopAndDestroy( cenrep); // cenrep

    // Read data from Muiu variation repository
    cenrep = CRepository::NewLC(KCRUidMuiuVariation);
    User::LeaveIfError(cenrep->Get(KMuiuPostcardFeatures, iFeatureBits));
    CleanupStack::PopAndDestroy( cenrep ); // cenrep
    }


// ---------------------------------------------------------
// Get
// ---------------------------------------------------------
TInt CPostcardCenRep::Get(TUint32 aKey) const
    {
    TInt i = aKey - KPocaKeyServiceProvider;
    __ASSERT_DEBUG(i >= 0 && i < KPocaKeyCount, Panic(EPostcardPanicCoding));
    __ASSERT_DEBUG(!iIsStringType[i], Panic(EPostcardPanicCoding));
    return iData[i].asInt;
    }

// ---------------------------------------------------------
// GetString
// ---------------------------------------------------------
const TDesC& CPostcardCenRep::GetString(TUint32 aKey) const
    {
    TInt i = aKey - KPocaKeyServiceProvider;
    __ASSERT_DEBUG(i >= 0 && i < KPocaKeyCount, Panic(EPostcardPanicCoding));
    __ASSERT_DEBUG(iIsStringType[i], Panic(EPostcardPanicCoding));
    return *iData[i].asString;
    }

// ---------------------------------------------------------
// FeatureBits
// ---------------------------------------------------------
TInt CPostcardCenRep::FeatureBits() const
    {
    return iFeatureBits;
    }
