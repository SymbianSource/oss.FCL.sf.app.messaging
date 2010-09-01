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
* Description: smiltime implementation
*
*/



// INCLUDE FILES
#include "smiltime.h"
#include "smilobject.h"

#include <e32svr.h>

// time resolution for smil is milliseconds
#define RESOLUTION_MICROSECONDS 1000 

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C
TSmilTime::TSmilTime( TInt aValue ) : iValue( aValue )
	{
	}


// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C
TSmilTime::TSmilTime( const TDesC& aValue ) 
	{

//	RDebug::Print(_L("%S"), &aValue);

	_LIT(KSec,"s");
	_LIT(KHour,"h");
	_LIT(KMin,"min");
	_LIT(KMsec,"ms");
	_LIT(KIndef,"indefinite");
	_LIT(KMed,"media");

	iValue = KUnresolved;

	// timecount value
	TInt val = 0;
	TInt sign = 1;
	if( aValue.Length() != 0 )
		{
		TInt b=-1;
		TInt e=0;
		for (; e<aValue.Length(); e++)
			{
			TChar c = aValue[e];
			if (c=='-')
				sign *= -1;
			else if (c.IsDigit() && b==-1)
				b = e;
			if ( b==-1 && !(c==' ' || c=='+' || c=='-') )
				break;			
			if ( b>-1 && !(c.IsDigit() || c=='.') )
				break;
			}
		if (e==0)
			{			
			//indefinite or unresolved
			if (aValue==KIndef)
				iValue = KIndefinite;
			else if (aValue==KMed)
				iValue = KMedia;
			}
		else if (b>-1)
			{			
			TLex lexer( aValue.Mid(b,e-b) );
			TReal32 real;
			if (lexer.Val( real )==KErrNone)
				{			
				val = sign * (TInt)(real*RESOLUTION_MICROSECONDS);

				if (e<aValue.Length())
					{
					TPtrC u = aValue.Mid(e);
					if (u==KMsec)
						iValue=val/1000; 
					else if (u==KSec)
						iValue=val;
					else if (u==KHour)
						iValue=val*3600;
					else if (u==KMin)
						iValue=val*60;
					}
				else
					iValue = val;
				}
			}		
		}	

	// clock value
	if (iValue==KUnresolved && aValue.Length() != 0)
		{
		TInt b=0;
		TInt e=0;
		TInt res=0;
		for (; e<=aValue.Length(); e++)
			{
			TChar c = e<aValue.Length()?aValue[e]:0;
			if(c==':' || e==aValue.Length())
				{
				if (e>b)
					{
					TLex lexer( aValue.Mid(b,e-b) );
					TReal32 real;
					if (lexer.Val( real )==KErrNone)
						{
						res = res*60+(TInt)(real*RESOLUTION_MICROSECONDS);
						}
					else 
						break;
					}
				else 
					break;
				b=e+1;
				}
			else if (!(c.IsDigit() || c=='.'))
				break;
			}
		if (e==aValue.Length()+1)
			{
			iValue = res;
			}

		}
	}


// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C
TSmilTime::TSmilTime() : iValue( 0 )
	{
	}

EXPORT_C
TSmilTime TSmilTime::ToLocal( CSmilObject* aObject, TSmilTime aTime )
	{
	if( !aTime.IsFinite() )
		return aTime;
	while( aObject != NULL )
		{
		if( aObject->RepeatBeginTime().IsUnresolved() )
			return TSmilTime::KUnresolved;
		aTime = aTime - aObject->RepeatBeginTime();
		aObject = aObject->ParentObject();
		}
	return aTime;
	}

EXPORT_C
TSmilTime TSmilTime::operator*( const TSmilTime& aValue ) const
	{
	if( aValue.IsUnresolved() || IsUnresolved() )
		return TSmilTime( KUnresolved );
	if( aValue.Value() == 0 || iValue == 0 )
		return TSmilTime( 0 );
	if( !aValue.IsFinite() || iValue == KIndefinite )
		return TSmilTime( KIndefinite ) ;
	return TSmilTime( iValue * aValue.Value() );  
	}

EXPORT_C
TSmilTime TSmilTime::operator*( const TReal32 aValue ) const
	{
	if( iValue == KUnresolved || aValue == KFloatUnspecified )
		return TSmilTime( KUnresolved );
	if( iValue == 0 || aValue == 0 )
		return TSmilTime( 0 );
	if( iValue == KIndefinite || aValue == KFloatIndefinite )
		return KIndefinite;
	return TSmilTime( STATIC_CAST( TInt, iValue * aValue ) );       
	}

EXPORT_C
TBool TSmilTime::operator==( const TSmilTime& aValue ) const
	{
	if( aValue.Value() == iValue )
		return ETrue;
	else
		return EFalse;
	}

EXPORT_C
TBool TSmilTime::operator!=( const TSmilTime& aValue ) const
	{
	if( aValue.Value() != iValue )
		return ETrue;
	else
		return EFalse;
	}

EXPORT_C
TSmilTime TSmilTime::operator-( const TSmilTime& aTime ) const
	{
	if( aTime.Value() == KUnresolved || iValue == KUnresolved )
		return TSmilTime( KUnresolved );
	if( aTime.Value() == KIndefinite || iValue == KIndefinite )
		return TSmilTime( KIndefinite );
	return TSmilTime( iValue - aTime.Value() );         
	}

EXPORT_C
TBool TSmilTime::operator>( const TSmilTime& aTime ) const
	{
	if( iValue > aTime.Value() )
		return ETrue;
	else
		return EFalse;
	}

EXPORT_C
TBool TSmilTime::operator<( const TSmilTime& aTime ) const
	{
	if( iValue < aTime.Value() )
		return ETrue;
	else
		return EFalse;		
	}


EXPORT_C
TBool TSmilTime::operator>=( const TSmilTime& aTime ) const
	{
	if( iValue >= aTime.Value() )
		return ETrue;
	else
		return EFalse;	
	}

EXPORT_C
TBool TSmilTime::operator<=( const TSmilTime& aTime ) const
	{
	if( iValue <= aTime.Value() )
		return ETrue;
	else
		return EFalse;
	}

	
EXPORT_C
const TSmilTime& TSmilTime::operator++( TInt /*aX*/ )
	{
	iValue++;
	return *this;
	}

EXPORT_C
TSmilTime& TSmilTime::operator=( const TSmilTime& aTime )
	{
	iValue = aTime.Value();
	return *this;
	}


EXPORT_C
TSmilTime& TSmilTime::operator=( const TInt aValue ) 
	{
	iValue = aValue;
	return *this;
	}

EXPORT_C
TSmilTime TSmilTime::operator+( const TSmilTime& aTime ) const
	{
	if( aTime.Value() == KUnresolved || iValue == KUnresolved )
		return TSmilTime( KUnresolved );
	if ( aTime.Value() == KIndefinite || iValue == KIndefinite )
		return TSmilTime( KIndefinite );
	return TSmilTime( aTime.Value() + iValue );       
	}



EXPORT_C
TTimeIntervalMicroSeconds TSmilTime::ToMicroSeconds() const
	{
	TInt64 time = iValue;
	time*=1000000/RESOLUTION_MICROSECONDS;
	return TTimeIntervalMicroSeconds(time);
	}

EXPORT_C
TSmilTime TSmilTime::FromMicroSeconds(TTimeIntervalMicroSeconds aMicro)
	{

	return TSmilTime(I64INT(aMicro.Int64()/RESOLUTION_MICROSECONDS));
	}

EXPORT_C
TSmilTime TSmilTime::FromMicroSeconds(TInt aMicro)
	{

	return TSmilTime(aMicro/RESOLUTION_MICROSECONDS);
	}

// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File  
