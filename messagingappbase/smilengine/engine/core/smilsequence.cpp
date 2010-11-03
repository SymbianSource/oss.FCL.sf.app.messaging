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
* Description: smilsequence implementation
*
*/



// INCLUDE FILES
#include "smilsequence.h"
#include "smilobject.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSmilSequence::CSmilSequence()
	{
	}

// EPOC default constructor can leave.
void CSmilSequence::ConstructL(CSmilPresentation* aPresentation)
	{
	CSmilObject::ConstructL(aPresentation);
	}

// Two-phased constructor.
// EPOC default constructor can leave.
CSmilSequence* CSmilSequence::NewL(CSmilPresentation* aPresentation)
	{
	CSmilSequence* ptr = new ( ELeave ) CSmilSequence;
	CleanupStack::PushL( ptr );
	ptr->ConstructL(aPresentation);
	CleanupStack::Pop(); //ptr
	return ptr;
	}

// Destructor
CSmilSequence::~CSmilSequence()
	{
	}


void CSmilSequence::ProceedL( const TSmilTime& aTime )
	{
	CSmilObject::ProceedL( aTime );
	TSmilTime ta = aTime - iRepeatBeginTime;
	if( iCurrentInterval.iBegin.IsFinite() )
		{
		if( aTime >= iCurrentInterval.iBegin && aTime <= iCurrentInterval.iEnd)
			{
			for( CSmilObject* c = iFirstChild; c != NULL; c = c->NextSibling() )
				{
				if( c->BeginTime().IsFinite() && c->BeginTime() > ta )
					break;
				if( !c->EndTime().IsFinite() || ( c->BeginTime() <= ta && ta <= c->EndTime() ) )
					{
					c->ProceedL( ta );
					if( !( c->EndTime() == ta ) )
						break;
					}
				}
			}
		}
	}


void CSmilSequence::RewindL(const TSmilTime& aTime, const TSmilTime& aTo)
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


void CSmilSequence::PropagateEventL( const TSmilEvent& aEvent, const TSmilTime& aTime )
	{
	CSmilObject::PropagateEventL( aEvent, aTime );
	TSmilTime ta = aTime - iRepeatBeginTime;
	for( CSmilObject* c = iFirstChild; c != NULL; c = c->NextSibling() )
		{
		if( c->IsPlaying() )
			{
			c->PropagateEventL( aEvent, ta );
			break;
			}
		}        
	}


TSmilTime CSmilSequence::ImplicitDuration() const
	{
	if( !iLastChild )
		return 0;
	else if ( iLastChild->HasMoreIntervals() )
		return TSmilTime::KUnresolved;
	else
		return iLastChild->EndTime();
	}



// ================= OTHER EXPORTED FUNCTIONS ==============

//" Eagles may soar, but weasels don't get sucked into jet engines. "

//  End of File  











