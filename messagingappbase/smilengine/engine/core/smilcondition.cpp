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
* Description: smilcondition implementation
*
*/




// INCLUDE FILES
#include "smilcondition.h"
#include "smilobject.h"
#include <e32svr.h>

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
TSmilCondition::TSmilCondition()
	{
	iOffset = 0;
	iEventSourceObject = 0;
	iEventClass = TSmilEvent::EOther;
	}


//C++ destructor
//
TSmilCondition::~TSmilCondition()
	{
	}


void TSmilCondition::SetEventClassL( const TDesC& aEventClass )
	{
	iEventClass = TSmilEvent::ClassForName(aEventClass);
	if (iEventClass==TSmilEvent::EOther)
		iEventClassName.SetTextL(aEventClass);	
	}


void TSmilCondition::SetEventSourceL( const TDesC& aEventSource )
	{
	iEventSourceName.SetTextL(aEventSource);
	}


TPtrC TSmilCondition::EventClassName() const 
	{
	return iEventClassName.Text();
	}

TPtrC TSmilCondition::EventSourceName() const 
	{
	return iEventSourceName.Text();
	}



TBool TSmilCondition::IsEventCondition() const
	{
	return iEventClass!=TSmilEvent::EOther || iEventClassName.Text().Length()>0;
	}


TBool TSmilCondition::Matches(const TSmilEvent& aEvent) const
	{
	/*TPtrC ec = aEvent.ClassName();
	TPtrC cc = iEventClassName.Text();
	RDebug::Print(_L("%S %S"), &ec, &cc);*/

	if ( ( 
		(iEventSourceObject!=0 && aEvent.iSourceObject==iEventSourceObject) || 
		(iEventSourceName.Text().Length()>0 && aEvent.SourceName()==iEventSourceName.Text() ) )
		&& 
		( aEvent.iClass == iEventClass || 
		(iEventClass==TSmilEvent::EOther && aEvent.ClassName()==iEventClassName.Text() )))
		return ETrue;
	else
		return EFalse;

	}

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  


