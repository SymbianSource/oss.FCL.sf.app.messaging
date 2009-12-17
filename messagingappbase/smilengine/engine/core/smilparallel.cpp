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
* Description: smilparallel implementation
*
*/



// INCLUDE FILES
#include "smilparallel.h"
#include "smilmedia.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSmilParallel::CSmilParallel() 
	{	
	}

// EPOC default constructor can leave.
void CSmilParallel::ConstructL(CSmilPresentation* aPresentation)
	{
	CSmilObject::ConstructL(aPresentation);
	iEndsync = EEndsyncLast;
	}

// Two-phased constructor.
CSmilParallel* CSmilParallel::NewL(CSmilPresentation* aPresentation)
	{
	CSmilParallel* ptr = new ( ELeave ) CSmilParallel;
	CleanupStack::PushL( ptr );
	ptr->ConstructL(aPresentation);
	CleanupStack::Pop(); //ptr
	return ptr;
	}		

// Destructor
CSmilParallel::~CSmilParallel()
	{
	}

TSmilTime CSmilParallel::ImplicitDuration() const
	{
	return TimeContainerEnd();
	}
	
void CSmilParallel::ProceedL( const TSmilTime& aTime )
	{
	CSmilObject::ProceedL( aTime );        
	TSmilTime ta = aTime - iRepeatBeginTime;
	if( iCurrentInterval.iBegin.IsFinite() )
		{
		if( aTime >= iCurrentInterval.iBegin && aTime <= iCurrentInterval.iEnd )
			{
			for( CSmilObject* c = iFirstChild; c != NULL; c = c->NextSibling() )
				{
				c->ProceedL( ta );
				}
			}          
		}
	}


void CSmilParallel::RewindL(const TSmilTime& aTime, const TSmilTime& aTo)
	{
	
	CSmilObject::RewindL( aTime, aTo );
	TSmilTime ta = aTime - iRepeatBeginTime;
	TSmilTime to = aTo - iRepeatBeginTime;
	if( iCurrentInterval.iBegin.IsFinite() && aTo<iCurrentInterval.iEnd )
		{		
		for( CSmilObject* c = iFirstChild; c != NULL; c = c->NextSibling() )
			{
			if( to < c->EndTime() )
				{
				c->RewindL(ta,to);
				}
			}
		
		}
	}


void CSmilParallel::PropagateEventL( const TSmilEvent& aEvent, const TSmilTime& aTime )
    {
	CSmilObject::PropagateEventL( aEvent, aTime );
	TSmilTime ta = aTime - iRepeatBeginTime;
	for( CSmilObject* c = iFirstChild; c != NULL; c = c->NextSibling() )
		{
		c->PropagateEventL( aEvent, ta );
		}        
	}

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  




