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
* Description: smillength implementation
*
*/



#include "smillength.h"

TSmilLength::TSmilLength() 
	{
	iType = EAuto;
	iValue = 0;	
	}


TSmilLength::TSmilLength(TInt value) 
	{
	iType = EFixed;
	iValue = value;	
	}


TSmilLength::TSmilLength(TLenType type, TInt value)
	{
	iType = type;
	iValue = value;	
	}
 


TSmilLength::TSmilLength(const TDesC& aPtr )
    {
	_LIT(KAuto, "auto");

    if (aPtr.Length()==0)
        {
		iType=EInvalid;
        iValue=0;
        }
    else if (aPtr.Compare(KAuto)==0)
        {
        iType=EAuto;
        iValue=0;
        }
	else if (aPtr[aPtr.Length()-1]=='%')
        {

		TPtrC parse = aPtr.Left( aPtr.Length() - 1 );	

		TLex lex( parse );
		TInt val = 0;
		if( lex.Val( val ) == KErrNone )
			{
			iValue = val;
			iType = EPercent;
			}
		else
			iType = EInvalid;
		

        }
    else
        {
		_LIT(px, "px");
		TLex lex;
		if (aPtr.Length()>2 && aPtr.Right(2)==px)
			lex.Assign( aPtr.Left(aPtr.Length()-2) ) ;
		else
			lex.Assign( aPtr );
		TInt val = 0;
		if( lex.Val( val ) == KErrNone )
			{
			iValue = val;
			iType = EFixed;
			}
		else
			iType = EInvalid;
        
		}
    }


TInt TSmilLength::GetLength(TInt relation) 
	{
	TInt res;
	if (iType==EFixed)
		res = iValue;
	else if (iType==EPercent)
		res = relation*iValue/100;
	else
		res = 0;
	return res;
    }
