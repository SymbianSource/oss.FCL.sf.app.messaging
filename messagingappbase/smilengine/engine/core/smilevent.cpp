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
* Description: smilevent implementation
*
*/




#include "smilevent.h"

#include "smilobject.h"

_LIT(KActivate, "activateEvent");
_LIT(KBegin, "beginEvent");
_LIT(KEnd, "endEvent");
_LIT(KRepeat, "repeatEvent");
_LIT(KFocusIn, "focusInEvent");
_LIT(KFocusOut, "focusOutEvent");
_LIT(KInBounds, "inBoundsEvent");
_LIT(KOutOfBounds, "outOfBoundsEvent");


TSmilEvent::TSmilEvent( const TSmilTime& aTime, const TDesC& aClass )
	{
	iTime = aTime;
	iClass = ClassForName(aClass);

	iSourceObject=0;	
	}

TSmilEvent::TSmilEvent( const TSmilTime& aTime, const TDesC& aClass, const CSmilObject* aSource )
	{
	iTime = aTime;	

	iClass = ClassForName(aClass);
	
	iSourceObject=aSource;	
	}


TSmilEvent::TSmilEvent( const TSmilTime& aTime, TEventClass aClass, const CSmilObject* aSource )
	{
	iTime = aTime;	
	iClass = aClass;
	iSourceObject=aSource;
	}


TBool TSmilEvent::IsInteractive() const
	{
	return IsInteractive(iClass);
	}


TPtrC TSmilEvent::ClassName() const 
	{
	return NameForClass(iClass);
	}


TSmilEvent::TEventClass TSmilEvent::ClassForName(const TDesC& aName)
	{
	if (aName == KActivate) 
		return EActivate;
	if (aName == KBegin) 
		return EBegin;
	if (aName == KEnd) 
		return EEnd;
	if (aName == KFocusIn) 
		return EFocusIn;
	if (aName == KFocusOut) 
		return EFocusOut;
	if (aName == KRepeat) 
		return ERepeat;
	if (aName == KInBounds) 
		return EInBounds;
	if (aName == KOutOfBounds) 
		return EOutOfBounds;
	
	return EOther;
	}


TPtrC TSmilEvent::NameForClass(TSmilEvent::TEventClass aClass)
	{
	switch (aClass)
		{
		case EActivate:
			return TPtrC(KActivate);
		case EBegin:
			return TPtrC(KBegin);
		case EEnd:
			return TPtrC(KEnd);
		case EFocusIn:
			return TPtrC(KFocusIn);
		case EFocusOut:
			return TPtrC(KFocusOut);
		case ERepeat:
			return TPtrC(KRepeat);
		case EInBounds:
			return TPtrC(KInBounds);
		case EOutOfBounds:
			return TPtrC(KOutOfBounds);
		default: 
			return TPtrC();
		}	
	}


TBool TSmilEvent::IsInteractive(TEventClass aClass)
	{
	switch (aClass)
		{
		case EActivate:
		case EFocusIn:
		case EFocusOut:
		case EInBounds:
		case EOutOfBounds:
			return ETrue;
		default: 
			return EFalse;
		}	
	}

TPtrC TSmilEvent::SourceName() const 
	{ 
	if (iSourceObject)
		return iSourceObject->Id();
	else
		return iSourceName.Text(); 
	}

void TSmilEvent::SetSourceNameL(const TDesC& aSource)
	{
	iSourceName.SetTextL(aSource);
	}

