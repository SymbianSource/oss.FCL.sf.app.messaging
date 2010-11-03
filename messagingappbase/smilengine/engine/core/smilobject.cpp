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
* Description: smilobject implementation
*
*/



//#define TIMING_DEBUG

// INCLUDE FILES
#include "smilobject.h"
#include "smiltimecontainer.h"
#include "smilpresentation.h"
#include "smilevent.h"
#include "smiltimelist.h"
#include "e32math.h"
#include "smilanchor.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSmilObject::CSmilObject() :
iBeginConditions(2), iEndConditions(2)
	{

	}

// Destructor
CSmilObject::~CSmilObject()
	{
	iBeginConditions.ResetAndDestroy();
	iEndConditions.ResetAndDestroy();
	iBeginConditions.Close();
	iEndConditions.Close();

	if (iAnchor)
		iAnchor->Close();
	}


void CSmilObject::ConstructL(CSmilPresentation* aPresentation)
	{
	iPresentation = aPresentation;
	iParentObject = 0;
	iNext = 0;
	iPrevious = 0;

	iBeginList.SetObject(this);
	iEndList.SetObject(this);

	// initialise style variables
	iRepeatCount = KFloatUnspecified;	
	iRepeatDur = TSmilTime::KUnspecified;
	iDuration = TSmilTime::KUnspecified;
	iFill = EFillAuto;
	iRestart = ERestartAlways;

	iMin=0;
	iMax=TSmilTime::KIndefinite;

	Initialise();

	}	

// ---------------------------------------------------------
// initialise the calculated variables to initial values

void CSmilObject::Initialise()
	{
	iCurrentInterval = TInterval();
	iSimpleDuration = TSmilTime::KUnresolved;
	iRepeatBeginTime = TSmilTime::KUnresolved;
	iActualRepeat = 0;
	iFirstInterval = ETrue;
	iMinLimited = EFalse;
	iForcedRepeat = EFalse;
	}


void CSmilObject::ResolveL( const TSmilTime& aTps )
	{

	// connect events
	for (RPointerArray<TSmilCondition>* conds=&iBeginConditions;  ; conds=&iEndConditions) 
		{		
		TInt eventcount = conds->Count();
		for( TInt i=0; i<eventcount; i++ )
			{
			TSmilCondition* c = (*conds)[i];                
			if (c->iEventSourceObject)
				continue;
						
			CSmilObject* o = iPresentation->GetTimegraph()->FindNamedObject(c->EventSourceName());
			if (o)
				{
				c->iEventSourceObject = o;
				if (TSmilEvent::IsInteractive(c->iEventClass))
					{
					o->SetFocusable(ETrue);					
					}
				}
			}
		if (conds==&iEndConditions)
			break;
		}
	
	// get first intervals
	if( iCurrentInterval.IsNull() )
		{
		iCurrentInterval = FirstInterval();
		if (!iCurrentInterval.IsNull())
			NotifyDependents(aTps);
		}

	}

// ------------------------------------------------------------
// perform the object state changes in the given time
// 
// this may include starting, stopping, repeating media,
// getting new intervals etc.

void CSmilObject::ProceedL( const TSmilTime& aTps  )
	{
	//<Debug>
	//CSmilInterpreter::Log( _L8( "CSmilObject::Proceed()" ) );

#ifdef TIMING_DEBUG
	RDebug::Print(_L("CSmilObject(%S)::Proceed()"), ClassName());
#endif

//	if( !iPrinted ) PrintTimeLists();

	//</Debug>
	TBool notify = EFalse;
	if( iCurrentInterval.IsNull() )
		{
		iCurrentInterval = FirstInterval();

		if (iCurrentInterval.IsNull())
			return;
	
		notify = ETrue;			
		}

	// the current interval might have changed so
	// that we need to stop and restart
	if( iPlaying && iCurrentInterval.iBegin.IsFinite() && iCurrentInterval.iBegin > aTps )
		{
		TSmilTime end = aTps - iCurrentInterval.iBegin;
		EndL( end );		
		}
	TBool foundNewInterval;
	do 
		{            
		foundNewInterval = EFalse;
		if( !iPlaying )
			{
			//begin if not playing and begin time has been reached
			//and end time has not reached and parent is playing
			if( ( (aTps>=iCurrentInterval.iBegin && aTps<iCurrentInterval.iEnd) || 
				(aTps==iCurrentInterval.iBegin && iCurrentInterval.Duration()==0) )
				&& (!iParentObject || iParentObject->IsPlaying() 
				|| (iParentObject->Fill()!=EFillRemove && iCurrentInterval.iBegin==0)) )
				{
				iRepeatBeginTime = iCurrentInterval.iBegin;
				TSmilTime begin = aTps - iCurrentInterval.iBegin;
				BeginL( begin );
				}
			}
		if( iPlaying )
			{
			TSmilTime gp = TSmilTime::KUnresolved;
			if( iRestart == ERestartAlways )
				gp = iBeginList.NextInstance( iCurrentInterval.iBegin, EFalse );
			// end if active and endTime has been reached
			if( iCurrentInterval.iEnd <= aTps || gp <= aTps )
				{
				if( gp < iCurrentInterval.iEnd )
					iCurrentInterval.iEnd = gp;
				TSmilTime end = aTps - iCurrentInterval.iBegin;
				EndL( end );
				// try to find a new interval
				TInterval newInterval = NextInterval();
				if( !newInterval.IsNull() )
					{
					notify = ETrue;
					foundNewInterval = ETrue;
					iCurrentInterval = newInterval;
					}
				iFirstInterval = EFalse;
				}			
			// repeat either when the explicit simple duration is ending or
			// when the media has reached its implicit end (it sets iForcedRepeat)
			else if( !iRepeatDur.IsUnspecified() || 
						(iRepeatCount!=KFloatUnspecified && iActualRepeat<iRepeatCount ) )
				{ 
				if ( (IsMedia() && iDuration.IsFinite() && aTps >= iRepeatBeginTime + iDuration)
					|| ( !IsMedia() && iSimpleDuration.IsFinite() && aTps >= iRepeatBeginTime + iSimpleDuration ) 
					|| (iForcedRepeat && iDuration.IsUnspecified()) )
					{
					iRepeatBeginTime = aTps;
					TSmilTime rt = aTps - iCurrentInterval.iBegin;

					RepeatL( rt );
					}
				iForcedRepeat = EFalse;
				}
			}
		// continue as long as new intervals are found
		} while( foundNewInterval );

		if( notify )
			NotifyDependents( aTps );
	}



// ------------------------------------------------------------
// notify all time dependent objects on new or changed interval
//
void CSmilObject::NotifyDependents( const TSmilTime& aTime )
	{
#ifdef TIMING_DEBUG
	RDebug::Print(_L("CSmilObject(%S)::NotifyDependents()"), ClassName());
#endif

	if( iParentObject == NULL )
		return;
	if( iParentObject->IsSequence() )
		{
		if( iNext != NULL )
			iNext->RecalcInterval( aTime );
		if( iParentObject->LastChild() == this )
			{
			TSmilTime a = aTime + iParentObject->BeginTime();
			iParentObject->RecalcInterval( a );
			}
		} 
	else
		{
		TSmilTime b = aTime + iParentObject->BeginTime();
		iParentObject->RecalcInterval( b );
		}
	}



// ------------------------------------------------------------
// recalculates the current interval, or tries to establish one
// if there is none
//
// only modifies end of the current interval if its begin time has
// already passed

void CSmilObject::RecalcInterval( const TSmilTime& aTime )
    {
#ifdef TIMING_DEBUG
	RDebug::Print(_L("CSmilObject(%S)::RecalcInterval() aTime=%d"), ClassName(), aTime.Value());
#endif

	iBeginList.RecalcSyncbase();
	iEndList.RecalcSyncbase();
	TSmilTime oldBegin = iCurrentInterval.iBegin;
	TSmilTime oldEnd = iCurrentInterval.iEnd;
	if ( aTime < iCurrentInterval.iBegin )
		{
		if ( iFirstInterval )
			{
			iCurrentInterval = FirstInterval();
			}
		else
			{
			iCurrentInterval.iEnd = aTime;
			iCurrentInterval = NextInterval();                            
			}
		}
	else if ( aTime >= iCurrentInterval.iBegin && 
		( aTime < iCurrentInterval.iEnd || ( aTime == iCurrentInterval.iEnd && iCurrentInterval.Duration()==0 )))
		{
		// recalutate the end of the interval
		if( !EndSpecified() ) 
			iCurrentInterval.iEnd = CalcActiveEnd( iCurrentInterval.iBegin );
		else
			{			
			TSmilTime tempend = iEndList.NextInstance( iCurrentInterval.iBegin, ETrue );		
			if ( tempend==iCurrentInterval.iBegin )
				tempend = iEndList.NextInstance( iCurrentInterval.iEnd, EFalse );
			iCurrentInterval.iEnd = CalcActiveEnd( iCurrentInterval.iBegin, tempend );

			}
		}
	
	if( iCurrentInterval.iBegin != oldBegin || iCurrentInterval.iEnd != oldEnd )
		NotifyDependents( aTime );
    }


// ------------------------------------------------------------
// Calculates the first acceptable interval for an element
// Returns:
//    Interval if there is such an interval
//    Null interval otherwise

CSmilObject::TInterval CSmilObject::FirstInterval()
	{
#ifdef TIMING_DEBUG
	RDebug::Print(_L("CSmilObject(%S)::FirstInterval()"), ClassName());
#endif

	//PrintTimeLists();
	TSmilTime beginAfter = KMinTInt;//Integer.MIN_VALUE;
	FOREVER // loop till return
		{
		//the first value in the begin list that is >= beginAfter.
		TSmilTime tempBegin = iBeginList.NextInstance( beginAfter, ETrue);
		if( !tempBegin.IsFinite() )
			return TInterval();
		TSmilTime tempEnd;
		if( !EndSpecified() ) 
			{
			//there was no end attribute specified
			// this calculates the active end with no end constraint
			tempEnd = CalcActiveEnd( tempBegin );
			}
		else
			{
			//We have a begin value - get an end
			//the first value in the end list that is >= tempBegin.            
			tempEnd = iEndList.NextInstance( tempBegin, ETrue );
			// Allow for non-0-duration interval that begins immediately
			// after a 0-duration interval.
			if( tempEnd == tempBegin && tempEnd == beginAfter )
				{
				// tempEnd has already been used in an interval calculated in this method call
				// set tempEnd to the next value in the end list that is > tempEnd
				tempEnd = iEndList.NextInstance( tempEnd, EFalse );
				}         
			if( tempEnd.IsUnresolved() )
				{
				// Events leave the end open-ended. If there are other conditions
				// that have not yet generated instances, they must be unresolved.
				if( HasEndEvent() || iEndList.IsEmpty() )
					tempEnd.IsUnresolved();
				// if all ends are before the begin, bad interval
				else
					return TInterval();
				}
			// this calculates the active dur with an end constraint
			tempEnd = CalcActiveEnd( tempBegin, tempEnd );
			}
		// We have an end - is it after the parent simple begin?
		if( tempEnd > 0 || ( tempBegin == 0 && tempEnd ==0) )
			{
			return TInterval( tempBegin, tempEnd );
			}
		else
			// Change beginAfter to find next interval, and loop
			beginAfter = tempEnd;
		} // close while loop
	} // close getFirstInterval




// ------------------------------------------------------------
// Calculates the next acceptable interval for an element
// Returns:
//    Interval if there is such an interval
//    Null interval otherwise

CSmilObject::TInterval CSmilObject::NextInterval( )
	{
#ifdef TIMING_DEBUG
	RDebug::Print(_L("CSmilObject(%S)::NextInterval()"), ClassName());
#endif

	//PrintTimeLists();
	TSmilTime tempBegin;
	
	// Note that at this point, the just ended interval is still the "current interval"
	TSmilTime beginAfter = iCurrentInterval.iEnd;
	//the first value in the begin list that is >= beginAfter.
	if( iCurrentInterval.iEnd == iCurrentInterval.iBegin )
		tempBegin = iBeginList.NextInstance( beginAfter, EFalse );
	else
		tempBegin = iBeginList.NextInstance( beginAfter, ETrue );
	if( !tempBegin.IsFinite() )
		return TInterval();
	if( tempBegin >= ParentSimpleEnd() )
		return TInterval();

	TSmilTime tempEnd;
	if( !EndSpecified() ) 
		{
		//there was no end attribute specified
		// this calculates the active end with no end constraint
		tempEnd = CalcActiveEnd( tempBegin );
		}
	else
		{
		// We have a begin value - get an end
		//the first value in the end list that is >= tempBegin.
		tempEnd = iEndList.NextInstance( tempBegin, ETrue );
		// Allow for non-0-duration interval that begins immediately
		// after a 0-duration interval.
		if ( tempEnd == tempBegin )
			{
			// set tempEnd to the next value in the end list that is > tempEnd
			tempEnd = iEndList.NextInstance( tempEnd, EFalse );
			}                       
		if ( tempEnd.IsUnresolved() )
			{
			// Events leave the end open-ended. If there are other conditions
			// that have not yet generated instances, they must be unresolved.
			if( HasEndEvent() || iEndList.IsEmpty() )
				tempEnd = TSmilTime::KUnresolved;
			// if all ends are before the begin, bad interval
			else
				return TInterval();
			}
		// this calculates the active dur with an end constraint
		tempEnd = CalcActiveEnd( tempBegin, tempEnd );            
		}
	
	return TInterval( tempBegin, tempEnd);
	}


// ------------------------------------------------------------
// called when object begins its active duration
void CSmilObject::BeginL( const TSmilTime& aTime )
    {
	if( iPrevious && iPrevious->Fill() == EFillFreeze && iParentObject &&
		iParentObject->IsSequence() )
		{
		iPrevious->Unfreeze();	
		}

	if( iSimpleDuration.IsFinite() && iCurrentInterval.iBegin < 0 )
		{
		TInt absolute = iCurrentInterval.iBegin.Value() < 0 
			? -iCurrentInterval.iBegin.Value() 
			: iCurrentInterval.iBegin.Value();
		iActualRepeat = 1 + absolute / iSimpleDuration.Value();
		}
	else
		iActualRepeat = 1;
	
	iPlaying = ETrue;

	iPresentation->SubmitEventL( TSmilEvent( aTime, TSmilEvent::EBegin, this ), aTime );

	if (iAnchor && iAnchor->iActuate==CSmilAnchor::EOnLoad)
		{
		iPresentation->ActivateAnchorL(iAnchor);
		}
	
	}



// ------------------------------------------------------------
// called when object ends its active duration

void CSmilObject::EndL( const TSmilTime& aTime )
	{
		
	iPresentation->SubmitEventL( TSmilEvent( aTime, TSmilEvent::EEnd, this ), aTime );

	iPlaying = EFalse;
	
	}

// ------------------------------------------------------------
// called when repeats during its active duration

void CSmilObject::RepeatL( const TSmilTime& aTime )
	{
	iActualRepeat++;

	iPresentation->SubmitEventL( TSmilEvent( aTime, TSmilEvent::ERepeat, this ), aTime );
	}



void CSmilObject::ResolveSimpleDuration()
	{
	if( iDuration.IsFinite() && !iDuration.IsMedia() )
		{
		iSimpleDuration = iDuration;
		}        
	else if( iDuration.IsUnspecified() && !IsRepeating() && EndSpecified() )
		{
		iSimpleDuration = TSmilTime::KIndefinite;
		}
	else if( iDuration.IsIndefinite())
		{
		iSimpleDuration = TSmilTime::KIndefinite;
		}
	else if( iDuration.IsUnspecified() )
		{
		iSimpleDuration = ImplicitDuration();
		}
	else if( iDuration.IsMedia() )
	    {
	    iSimpleDuration = ImplicitDuration();
	    }
	}



TSmilTime CSmilObject::CalcActiveEnd( TSmilTime aB ) 
	{
	return CalcActiveEnd( aB, TSmilTime::KIndefinite );
	}


TSmilTime CSmilObject::CalcActiveEnd( TSmilTime aB, TSmilTime aEnd )
	{                
	TSmilTime PAD;
	ResolveSimpleDuration();
	if( EndSpecified() && iDuration.IsUnspecified() && !IsRepeating() )		
		{
		if ( aEnd.IsFinite() )
			PAD = aEnd - aB;
		else if( aEnd.IsIndefinite() )
			PAD = TSmilTime::KIndefinite;
		else
			PAD = TSmilTime::KUnresolved;
		}
	else if( !EndSpecified() || aEnd.IsIndefinite() )
		{
		PAD = CalcIntermediateActiveDuration();
		}
	else 
		{
		TSmilTime dd;
		if( aEnd.IsUnresolved() )
			dd = TSmilTime::KUnresolved;
		else
			dd = aEnd - aB;
		TSmilTime x = CalcIntermediateActiveDuration();
		if( x < dd )
			PAD = x;
		else
			PAD = dd;
		}

	

#ifdef TIMING_DEBUG
	RDebug::Print(_L("CSmilObject(%S)::CalcActiveEnd() PAD=%d"), ClassName(), PAD.Value());
#endif

	// minmax semantics
	TSmilTime AD;	

	TSmilTime tmin = iMin.IsMedia()?ImplicitDuration():iMin;
    TSmilTime tmax = iMax.IsMedia()?ImplicitDuration():iMax;
              	
	iMinLimited = EFalse;
	
	if (PAD>tmax)
		AD = tmax;
	else if (PAD<tmin)
		{
		AD = tmin;
		iMinLimited = ETrue;
		}
	else 
		AD = PAD;	
	
	return aB + AD;
	}


// ------------------------------------------------------------
// intermediate active duration calculation 
//
// see SMIL2 Timing Module for details
TSmilTime CSmilObject::CalcIntermediateActiveDuration() const
	{
	TSmilTime p0 = iSimpleDuration;
	TSmilTime p1; 
	if( iRepeatCount == KFloatUnspecified || iSimpleDuration.IsUnresolved() ||
		( IsTimeContainer() && !IsMedia() && iActualRepeat < iRepeatCount ) )
		p1 = TSmilTime::KUnresolved;
	else
		{
		if (iRepeatBeginTime.IsFinite())
			{
			TSmilTime d = iRepeatBeginTime - iCurrentInterval.iBegin;		
			p1 = d + iSimpleDuration * ( iRepeatCount - iActualRepeat + 1 );
			}
		else
			p1 = iSimpleDuration * iRepeatCount;
		}
	TSmilTime p2 = !iRepeatDur.IsUnspecified() ? iRepeatDur : TSmilTime::KIndefinite;
	TSmilTime res;
	if( p0 == 0)
		res = 0;
	else if( !IsRepeating() )
		res = p0;
	else 
		{
		if( p1 < p2 )
			res = p1;
		else
			res = p2;
		if( !res.IsFinite() )
			res = TSmilTime::KIndefinite;
		}
#ifdef TIMING_DEBUG
	RDebug::Print(_L("CSmilObject(%S)::CalcIntermediateActiveDuration() res=%d"), ClassName(), res.Value());
#endif
	return res;            
	}


TSmilTime CSmilObject::NextActivation( const TSmilTime& aTime ) const
	{		

	if (iCurrentInterval.iBegin>aTime)
		return iCurrentInterval.iBegin.IsFinite() ? iCurrentInterval.iBegin : TSmilTime::KUnresolved;	

	TSmilTime res = TSmilTime::KUnresolved;

	if (IsRepeating())
		{
		TSmilTime nrep = iRepeatBeginTime+iSimpleDuration;
		if (nrep>aTime)
			res = nrep;
		}

	if (iCurrentInterval.iEnd>aTime)
		{
		if (iCurrentInterval.iEnd<res)
			res = iCurrentInterval.iEnd;
		}

	return res;
	}


// ------------------------------------------------------------
// check if the event matches to conditions defined in this object
// and resolve the timings accordingly
//
void CSmilObject::PropagateEventL( const TSmilEvent& aEvent, const TSmilTime& aTime )
	{
	if( !HasEndEvent() && !HasBeginEvent() )
		return;
	TBool found = EFalse;
	TInt i = 0;
	//begin events
	if( !iPlaying || iRestart == ERestartAlways ) //handle if not playing or if restarting allowed
		{
		TInt eventcount = iBeginConditions.Count();               
		while( i < eventcount )
			{
			TSmilCondition* c = iBeginConditions[i];                
			if( c->Matches(aEvent) )
				{
				//TSmilTime t = TSmilTime::ToLocal( iParentObject, aEvent.iTime ) + c->iOffset;
				TSmilTime t = aTime + c->iOffset;
				if( t > iCurrentInterval.iBegin && t < iCurrentInterval.iEnd && iRestart == ERestartAlways )
					{
					iBeginList.Reset();
					CSmilInstanceTime* a = new ( ELeave ) CSmilInstanceTime( KEventValue, t );
					iBeginList.Add( a );
					iCurrentInterval.iEnd = t;
					}
				else if( t < iCurrentInterval.iBegin )
					{
					CSmilInstanceTime* b = new ( ELeave ) CSmilInstanceTime( KEventValue, t );
					iBeginList.Add( b );
					RecalcInterval( aTime );
					}

				else if( iRestart != ERestartNever && aTime >= iCurrentInterval.iEnd)
					{
					CSmilInstanceTime* it = new ( ELeave ) CSmilInstanceTime( KEventValue, t );
					iBeginList.Add( it );
					iCurrentInterval = NextInterval();
					if( !iCurrentInterval.IsNull() )
						{						
						NotifyDependents( aTime );
						iFirstInterval = EFalse; 
						}
					}
				found = ETrue;
				}
			i++;
			}
		if( found )
			return;
		}
		//end events
	if( iPlaying ) //handle only if playing
		{
		i = 0;
		TInt endcount = iEndConditions.Count(); 
		while( i < endcount )
			{
			TSmilCondition* c = iEndConditions[i];
			if( c->Matches( aEvent ) )
				{
				TSmilTime t = TSmilTime::ToLocal( this->iParentObject, aEvent.iTime + c->iOffset );
				CSmilInstanceTime* a = new ( ELeave ) CSmilInstanceTime( KEventValue, t );
				iEndList.Add( a );
				RecalcInterval( aTime );        
				}
			i++;
			}
		}
	}


TSmilTime CSmilObject::ParentSimpleEnd()
	{ 	
	if (!iParentObject)
		return TSmilTime::KUnresolved;

	if (iParentObject->iDuration.IsUnspecified())
		return TSmilTime::KUnresolved;
	else
		return iParentObject->iRepeatBeginTime + iParentObject->iDuration;
	}


void CSmilObject::RewindL(const TSmilTime& aTime, const TSmilTime& aTo)
	{
	if (aTo < iCurrentInterval.iBegin)
		{
		EndL(aTime);
		Unfreeze();			
		iBeginList.Reset();
		iEndList.Reset();
		iCurrentInterval.iBegin = TSmilTime::KUnresolved;
		iCurrentInterval.iEnd = TSmilTime::KUnresolved;
		}	
	else if (aTo == iCurrentInterval.iBegin)
		{
		// ### hackety hack
		EndL(aTime);
		Unfreeze();
		iEndList.Reset();
		iCurrentInterval.iEnd = TSmilTime::KUnresolved;
		RecalcInterval(aTo);
		}
	else if ( iPlaying && IsRepeating()	&& iSimpleDuration.IsFinite())
		{
		TSmilTime sd = iSimpleDuration;			
		TInt pos = (iCurrentInterval.iBegin - aTime).Value();
		TInt ar = 1 + pos / sd.Value();			
		if (IsTimeContainer() && !IsMedia() && iActualRepeat!=ar)
			{
			RepeatL(aTime); // ### to reset children, evil
			iActualRepeat = ar;
			}
		}					
	}



void CSmilObject::AfterL(MSmilActive* iActive, const TSmilTime& aDelay, TBool aLazy)
	{
	iActive->iSmilObject = this;
	iPresentation->AfterL(iActive, aDelay, aLazy);		
	}

void CSmilObject::CancelActive(MSmilActive* aActive) 
	{ 
	iPresentation->CancelActive(aActive); 
	}



TSmilTime CSmilObject::LocalToGlobal(const TSmilTime& aTime)
	{	
	if (!aTime.IsFinite())	
		return aTime;
		
	if (iParentObject)
		{
		TSmilTime t;
		if (iParentObject->iRepeatBeginTime.IsFinite())
			t = iParentObject->iRepeatBeginTime + aTime;
		else
			t = iParentObject->iCurrentInterval.iBegin + aTime;

		return iParentObject->LocalToGlobal(t);
		}		
	else 
		return aTime;		
	}

TSmilTime CSmilObject::GlobalToLocal(const TSmilTime& aTime)
	{	
	if (!aTime.IsFinite())	
		return aTime;
			
	TSmilTime t;
	if (iRepeatBeginTime.IsFinite())
		t = aTime - iRepeatBeginTime ;
	else
		t = aTime - iCurrentInterval.iBegin;

	if (iParentObject)
		return iParentObject->GlobalToLocal(t);
	else 
		return t;
		
	}


TSmilTime CSmilObject::RepeatBeginTime() const
	{
	return iRepeatBeginTime;
	}


void CSmilObject::Reset()
	{
	Initialise();
	iBeginList.Reset();
	iEndList.Reset();
    }

TBool CSmilObject::BeginSpecified()
	{
	return !iBeginList.IsEmpty() || iBeginConditions.Count() > 0;
	}


TBool CSmilObject::EndSpecified()
	{
	return !iEndList.IsEmpty() || iEndConditions.Count() > 0;
	}


TBool CSmilObject::HasEndEvent()
	{
	if( iEndConditions.Count() > 0 )
		return ETrue;
	else
		return EFalse;
	}


TBool CSmilObject::HasBeginTime()
	{
	return !iCurrentInterval.iBegin.IsUnresolved();
	}

TBool CSmilObject::HasEndTime()
	{
	return !iCurrentInterval.iEnd.IsUnresolved();
	}


TBool CSmilObject::HasBeginEvent()
	{
	if( iBeginConditions.Count() > 0 )
		return ETrue;
	else
		return EFalse;
	} 



TSmilTime CSmilObject::BeginTime() const
	{
	return iCurrentInterval.iBegin;
	}


TSmilTime CSmilObject::EndTime() const
	{
	return iCurrentInterval.iEnd;
	}


TBool CSmilObject::HasMoreIntervals()
	{
	if (iCurrentInterval.IsNull())
		return iBeginList.HasFiniteOffsets();
	else
		return !NextInterval().IsNull();
	}

TBool CSmilObject::IsRepeating() const
	{
	return !iRepeatDur.IsUnspecified() || iRepeatCount!=KFloatUnspecified;
	}


void CSmilObject::AddConditionL( const TSmilCondition& aCondition, TBool aBeginCondition )
	{

//	RDebug::Print(_L("AddConditionL %d:%S"),aCondition->Offset().Value(), aCondition->Event());

	if( aCondition.IsEventCondition() )
		{
		TSmilCondition* c = new (ELeave) TSmilCondition();
		CleanupStack::PushL(c);
		if (aCondition.EventSourceName().Length()==0)
			{
			c->iEventSourceObject = this;
			if (TSmilEvent::IsInteractive(aCondition.iEventClass))
				{
				SetFocusable(ETrue);					
				}
			}
		else
			{
			CSmilObject* o;
			if (aCondition.EventSourceName() == iId.Text())
				o = this;
			else
				o = iPresentation->GetTimegraph()->FindNamedObject(aCondition.EventSourceName());
			if (o)
				{
				c->iEventSourceObject = o;
				if (TSmilEvent::IsInteractive(aCondition.iEventClass))
					{
					o->SetFocusable(ETrue);					
					}
				}
			else
				c->SetEventSourceL(aCondition.EventSourceName());
			}

		c->iEventClass = aCondition.iEventClass;
		c->iOffset = aCondition.iOffset;

		
		if (aBeginCondition)
			User::LeaveIfError(iBeginConditions.Append( c ));
		else
			User::LeaveIfError(iEndConditions.Append( c ));
		CleanupStack::Pop();
		}
	else
		{
		CSmilInstanceTime* it = new ( ELeave ) CSmilInstanceTime( KOffsetValue, aCondition.iOffset );
		
		if (aBeginCondition)
			iBeginList.Add( it );
		else
			iEndList.Add( it );
		}
	}


void CSmilObject::ResolveBeginTimeL(TSmilTime aTime)
	{
	iBeginList.Add(new (ELeave) CSmilInstanceTime( KEventValue, aTime ));
	iCurrentInterval.iBegin = aTime;
	NotifyDependents(aTime);
	}


void CSmilObject::ResolveEndTimeL(TSmilTime aTime)
	{
	iEndList.Add(new (ELeave) CSmilInstanceTime( KEventValue, aTime ));
	iCurrentInterval.iEnd = aTime;
	NotifyDependents(aTime);
	}

TSmilTime CSmilObject::ImplicitDuration() const
	{
	return TSmilTime::KIndefinite;
	}



TReal32 CSmilObject::RepeatCount() const
	{
	return iRepeatCount;
	}


TInt CSmilObject::ActualRepeat() const
	{
	return iActualRepeat;
	}



TSmilTime CSmilObject::RepeatDur() const
	{
	return iRepeatDur;
	}



void CSmilObject::SetEndTime( const TSmilTime& aEndTime )
	{
	iCurrentInterval.iEnd = aEndTime;
	}



TFill CSmilObject::Fill() const
	{
	return iFill;
	}

void CSmilObject::SetRestart( TRestart aVal )
	{
	iRestart = aVal;
	}


void CSmilObject::SetRepeatCount( TReal32 aRC )
	{
	iRepeatCount = aRC;
	}


void CSmilObject::SetRepeatDur( TSmilTime aRD )
	{
	iRepeatDur = aRD;
	}


void CSmilObject::SetDuration( const TSmilTime& aDuration )
	{
	iDuration = aDuration;
	}

void CSmilObject::SetMax( const TSmilTime& aMax )
	{
	iMax = aMax;
	}

void CSmilObject::SetMin( const TSmilTime& aMin )
	{
	iMin = aMin;
	}


void CSmilObject::SetIdL( const TDesC& aString )
	{
	iId.SetTextL(aString);
	}

void CSmilObject::SetTitleL( const TDesC& aString )
	{
	iTitle.SetTextL(aString);
	}


void CSmilObject::SetAnchor( CSmilAnchor* aAnchor )
	{
	if( iAnchor )
		iAnchor->Close();

	iAnchor = aAnchor;

	if( iAnchor )
		{
		if ( iAnchor->Open() == KErrNone )
		    {
		    SetFocusable(ETrue);		
		    }
		}
		
	}


EXPORT_C
CSmilAnchor* CSmilObject::Anchor() const
	{
	return iAnchor;
	}



TPtrC CSmilObject::Id() const
	{
	return iId.Text();
	}



TPtrC CSmilObject::Title() const
	{
	return iTitle.Text();
	}


TBool CSmilObject::IsPlaying() const
	{
	return iPlaying;
	}



void CSmilObject::SetParent( CSmilTimeContainer* aContainer )
	{
	iParentObject = aContainer;
	}



CSmilTimeContainer* CSmilObject::ParentObject()
	{
	return iParentObject;
	}

void CSmilObject::SetPreviousSibling( CSmilObject* aPrevious )
	{
	iPrevious = aPrevious;
	}

void CSmilObject::SetNextSibling( CSmilObject* aNext )
	{
	iNext = aNext;
	}


CSmilObject* CSmilObject::NextSibling() const
	{
	return iNext;
	}


CSmilObject* CSmilObject::PreviousSibling() const
	{
	return iPrevious;
	}


CSmilObject* CSmilObject::NextAdjacent() const
	{
	if (FirstChild())
		return FirstChild();
	else if (iNext)
		return iNext;
	else 
		{
		CSmilObject* o = iParentObject;
		while (o && !o->iNext)
			{
			o = o->iParentObject;
			}
		if (o)
			return o->iNext;		
		}	
	return 0;
	}


CSmilObject* CSmilObject::PreviousAdjacent() const
	{
	CSmilObject* o = iPrevious;
	while (o && o->LastChild())
		{		
		o = o->LastChild();
		}
	return o;
	}


CSmilPresentation* CSmilObject::Presentation() const
	{
	return iPresentation;
	}




// ------------------------------------------------------
// at end of attribute parsing, some finishing touches
//
void CSmilObject::AttachedL() 
	{

	// only media can have fill="transition"
	if (iFill==EFillTransition && !IsMedia())
		{
		iFill = EFillAuto;
		}

	/* "If none of the attributes dur, end, repeatCount or repeatDur are 
	   specified on the element, then the element will have a fill behavior identical 
	   to that if it were specified as "freeze".  
		
	   Otherwise, the element will have a fill behavior identical to that if it were 
	   specified as "remove"." */

	if (iFill==EFillAuto)
		{
		if (!EndSpecified() && iDuration.IsUnspecified()
			&& iRepeatCount==KFloatUnspecified && iRepeatDur.IsUnspecified())
			iFill = EFillFreeze;
		else
			iFill = EFillRemove;
		}

	// check min<=max
	if (iMin>iMax)
		{
		iMin=0;
		iMax=TSmilTime::KIndefinite;
		}

	if (!BeginSpecified())
		{
		// excl children have indefinite default begin time, par and seq 0
		if (!iParentObject || !iParentObject->IsExclusive())
			iBeginList.Add( new (ELeave) CSmilInstanceTime(KOffsetValue,0) );
		}
	}


CSmilObject* CSmilObject::FindNamedObject(const TDesC& aName)
	{
	if (iId.Text() == aName)
		return this;
	else
		return 0;
	}


void CSmilObject::PrintTree(int d) const
	{

	TBuf<100> spacer;
	for (TInt n=0;n<d;++n) spacer.Append(_L(" "));

	TPtrC i = Id();
	RDebug::Print(_L("%S(%S) id=%S bt=%d et=%d sd=%d rbt=%d ar=%d"), &spacer, ClassName(), &i, 
		iCurrentInterval.iBegin.Value(), iCurrentInterval.iEnd.Value(), iSimpleDuration.Value(), iRepeatBeginTime.Value(), iActualRepeat);
	}

// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File  
