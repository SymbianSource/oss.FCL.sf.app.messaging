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
*		This class encapsulates a name and value pair
*
*/



// INCLUDE FILES

#include "MsgNameValue.h"

//  MEMBER FUNCTIONS

EXPORT_C CMsgNameValue* CMsgNameValue::NewL( HBufC* aName, HBufC* aValue)
    {
    CMsgNameValue* self = new (ELeave) CMsgNameValue(aName, aValue);
    return self;
    }

EXPORT_C CMsgNameValue* CMsgNameValue::NewL(const TDesC& aName, const TDesC& aValue)
    {
    CMsgNameValue* self = CMsgNameValue::NewLC( aName, aValue );
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CMsgNameValue* CMsgNameValue::NewLC(const TDesC& aName, const TDesC& aValue)
    {
    CMsgNameValue* self = new (ELeave) CMsgNameValue;
    CleanupStack::PushL(self);
    self->ConstructL(aName, aValue);
    return self;
    }

EXPORT_C CMsgNameValue::~CMsgNameValue()
    {
    delete iName;
    delete iValue;
    }

EXPORT_C const TDesC& CMsgNameValue::Name() const
    {
    if (iName)
        {
        return *iName;
        }
    return KNullDesC;
    }

EXPORT_C const TDesC& CMsgNameValue::Value() const
    {
    if (iValue)
        {
        return *iValue;
        }
    return KNullDesC;
    }

void CMsgNameValue::ConstructL(const TDesC& aName, const TDesC& aValue)
    {
	aName.Length()==0 ? iName=KNullDesC().AllocL() : iName = aName.AllocL();
	aValue.Length()==0 ? iValue=KNullDesC().AllocL() : iValue = aValue.AllocL();
    }

CMsgNameValue::CMsgNameValue()
    {
    }

CMsgNameValue::CMsgNameValue( HBufC* aName, HBufC* aValue ) :
    iName(aName),
    iValue(aValue)
    {
    }

// end of file
