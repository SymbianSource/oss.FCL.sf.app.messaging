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
* Description: smiltimelist implementation
*
*/



// INCLUDE FILES
#include "smiltimelist.h"
#include "smilobject.h"
#include "smiltime.h"
#include "smilinstancetime.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
TSmilTimeList::TSmilTimeList() 
: iFirst( NULL ), iLast( NULL )
	{
	iObject = 0;
	iSyncbaseCalculated = EFalse;
	}


TSmilTimeList::~TSmilTimeList()
	{
	CSmilInstanceTime* a = iFirst;
	while( a != NULL )
		{	
		CSmilInstanceTime* del = a;
		a = a->Next();
		delete del;
		del = NULL;
		}
	}

void TSmilTimeList::Add( CSmilInstanceTime* aTime )
	{
	if (!aTime)
		return;

	aTime->SetNext(0);

	if( iFirst == NULL )
		{
		iFirst = aTime;
		iLast = aTime;
		}
	else if( !aTime->Time().IsFinite() && !iLast->Time().IsFinite() )
		{
		return;
		}
	else if( iLast->Time() <= aTime->Time() )
		{
		iLast->SetNext( aTime );
		iLast = aTime;            
		}
	else if( iFirst == iLast )
		{
		iFirst = aTime;
		aTime->SetNext( iLast );
		}
	else
		{
		CSmilInstanceTime* t = iFirst;
		while( t->Next() != NULL )
			{
			if( t->Next()->Time() > aTime->Time() )
				{
				CSmilInstanceTime* tmp = t->Next();
				t->SetNext( aTime );
				aTime->SetNext( tmp );
				}                
			t = t->Next();
			}
		}        
	}


void TSmilTimeList::SetObject(CSmilObject* aObject)
	{
	iObject = aObject;
	iSyncbaseCalculated = EFalse;
	}


void TSmilTimeList::RecalcSyncbase()
	{
	CSmilInstanceTime* t = iFirst;        
	iFirst = iLast = NULL;

	while( t != NULL )
		{
		CSmilInstanceTime* nxt = t->Next();

		// implicit syncbases
		if( !t->IsEventValue() && iObject->ParentObject() )
			{
			if( iObject->ParentObject()->IsParallel() )
				{
				t->SetSyncbase( iObject->ParentObject() );
				t->SetSyncBegin( ETrue );
				}
			else if( iObject->ParentObject()->IsSequence() )
				{
				if( !iObject->PreviousSibling())
					{
					t->SetSyncbase( iObject->ParentObject() );
					t->SetSyncBegin( ETrue );
					} 
				else
					{
					t->SetSyncbase( iObject->PreviousSibling() );
					t->SetSyncBegin( EFalse );
					} 
				}	
			}

		//TSmilTime oldtime = t->Time();
		TSmilTime sb;
		if( t->Syncbase() )
			{
			if( t->IsBeginSync() )                
				sb = t->Syncbase()->BeginTime();
			else
				sb = t->Syncbase()->EndTime();
			if ( t->Syncbase() == iObject->ParentObject() )
				{
				sb = sb - t->Syncbase()->BeginTime();
				}	 
			else if (t->Syncbase() == iObject->PreviousSibling() )
				{
				// same timespace, nothing to do
				} 
			else
				{
				// ### generic timebase conversion
				}
			t->SetTime( sb + t->Offset() );
			}                      
		Add( t );
		t = nxt;
		}

	iSyncbaseCalculated = ETrue;
	}


void TSmilTimeList::Remove( CSmilInstanceTime* aRem, CSmilInstanceTime* aPrev )
	{
	if( aPrev == NULL && iFirst != aRem )
		{
		CSmilInstanceTime* t = iFirst;
		while( t->Next() != NULL )
			{
			if( t->Next() == aRem )
				{
				aPrev = t;
				break;
				}
			t = t->Next();
			}
		}
	if( iFirst == aRem )
		{
		iFirst = aRem->Next();
		}
	else if( aPrev )
		{
		aPrev->SetNext( aRem->Next() );
		}
	if( iLast == aRem )
		iLast = aPrev;
	}


void TSmilTimeList::Reset()
	{
	CSmilInstanceTime* t = iFirst;
	CSmilInstanceTime* p = NULL;

	while( t != NULL )
		{
		if( t->IsEventValue() )
			{
			if( p == NULL )
				{
				iFirst = t->Next();
				CSmilInstanceTime* del = t;
				t = t->Next();
				delete del;
				}
			else
				{
				p->SetNext( t->Next() );
				CSmilInstanceTime* del = t;
				t = t->Next();
				delete del;
				}
			}
		else
			{
			p = t;
			t = t->Next();
			}
		}
	iLast = p;
	}


TSmilTime TSmilTimeList::NextInstance( TSmilTime& aAfter, TBool aEq )
	{ 		

	if (!iSyncbaseCalculated)
		RecalcSyncbase();

	CSmilInstanceTime* t = iFirst;

	while( t != NULL )
		{
		if( !t->Time().IsFinite() )
			return t->Time();
		else
			{
			// implicit syncbase for seq
			//                if (parent instanceof Sequence && it.isOffsetValue()&&
			//                    prev!=null && prev.endTime>=0)
			//                    t += prev.endTime;
			if( t->Time() > aAfter || ( t->Time() == aAfter && aEq ) )
				return t->Time();
			}
		t = t->Next();
		}
	return TSmilTime::KUnresolved;
	}


TBool TSmilTimeList::IsEmpty()
	{
	return iFirst == NULL; 
	}


TBool TSmilTimeList::HasFiniteOffsets()
	{
	CSmilInstanceTime* t = iFirst;
	while( t)
		{
		if (t->Offset().IsFinite())
			return ETrue;
		t = t->Next();
		}
	return EFalse;
	}







