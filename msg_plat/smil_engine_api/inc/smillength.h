/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: smillength  declaration
*
*/



#ifndef SMILLENGTH_H
#define SMILLENGTH_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>

// CLASS DECLARATION
/**
 * Length class. The value can be percent or absolute.
 */
class TSmilLength
{
public:

    enum TLenType { EInvalid, EFixed, EPercent, EAuto } ;

    TSmilLength();
    TSmilLength(TInt value);
    TSmilLength(TLenType type, TInt value);
    TSmilLength(const TDesC& aLen );

    TBool IsPercent() { return iType==EPercent; }
    TBool IsFixed() { return iType==EFixed; }
    TBool IsAuto() { return iType==EAuto; }
    TBool IsValid() { return iType!=EInvalid; }

    TInt GetLength(TInt relation);

    TLenType iType;
    TInt iValue;

};



#endif      // ?INCLUDE_H   
            
// End of File
