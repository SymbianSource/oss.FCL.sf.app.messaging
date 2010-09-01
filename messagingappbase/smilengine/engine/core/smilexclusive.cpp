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
* Description: smilexclusive implementation
*
*/



// INCLUDE FILES
#include "smilexclusive.h"
#include "smilmedia.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSmilExclusive::CSmilExclusive() 
	{	
	}

// EPOC default constructor can leave.
void CSmilExclusive::ConstructL(CSmilPresentation* aPresentation)
	{
	CSmilObject::ConstructL(aPresentation);
	iEndsync = EEndsyncLast;
	}

// Two-phased constructor.
CSmilExclusive* CSmilExclusive::NewL(CSmilPresentation* aPresentation)
	{
	CSmilExclusive* ptr = new ( ELeave ) CSmilExclusive;
	CleanupStack::PushL( ptr );
	ptr->ConstructL(aPresentation);
	CleanupStack::Pop(); //ptr
	return ptr;
	}		

// Destructor
CSmilExclusive::~CSmilExclusive()
	{
	}

//
TSmilTime CSmilExclusive::ImplicitDuration() const
	{
	return TimeContainerEnd();
	}
	

void CSmilExclusive::ProceedL( const TSmilTime& aTime )
	{
	CSmilObject::ProceedL( aTime );        
	TSmilTime ta = aTime - iRepeatBeginTime;
	if( iCurrentInterval.iBegin.IsFinite() )
		{
		if( aTime >= iCurrentInterval.iBegin && aTime <= iCurrentInterval.iEnd )
			{
			for( CSmilObject* c = iFirstChild; c != NULL; c = c->NextSibling() )
				{
				if( c->BeginTime().IsFinite() && c->BeginTime() > ta )
					continue;
				if( !c->EndTime().IsFinite() || ( c->BeginTime() <= ta && ta <= c->EndTime() ) )
					{
					TBool wasPlaying = c->IsPlaying();
					c->ProceedL( ta );
					if (c->BeginTime() <= ta && c->IsPlaying() && !wasPlaying)
						{
						TBool before = ETrue;
						for( CSmilObject* cc = iFirstChild; cc != NULL; cc = cc->NextSibling() )
							{
							if (cc==c)
								{
								before = EFalse;
								continue;
								}
							if ( (before||cc->BeginTime()!=ta)
								&& ( cc->BeginTime() <= ta && ta < cc->EndTime() ))
								{
								cc->SetEndTime(ta);
								cc->ProceedL(ta);
								}
							}
						}
					}				
				}
			}          
		}
	}


void CSmilExclusive::RewindL(const TSmilTime& aTime, const TSmilTime& aTo)
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


void CSmilExclusive::PropagateEventL( const TSmilEvent& aEvent, const TSmilTime& aTime )
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




