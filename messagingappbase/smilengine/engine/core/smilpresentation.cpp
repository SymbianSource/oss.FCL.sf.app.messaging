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
* Description: smilpresentation implementation
*
*/



// INCLUDE FILES

#include "smilpresentation.h"
#include "smilsequence.h"
#include "smilplayerinterface.h"

#include "smilrootregion.h"

#include "smiltransition.h"

#include "smilanchor.h"
#include "smilarea.h"

#include "smilinstancetime.h"

#include "smilactiveinterface.h"

#include "smilmediarendererinterface.h"

#include <e32keys.h>

//#define SCHEDULING_DEBUG
//#define TIMING_DEBUG

EXPORT_C CSmilPresentation* CSmilPresentation::NewL(MSmilPlayer* aPlayer)
	{
	CSmilPresentation* self = new(ELeave) CSmilPresentation;
	CleanupStack::PushL(self);
	self->ConstructL(aPlayer);
	CleanupStack::Pop();
	return self;		
	}

void CSmilPresentation::ConstructL(MSmilPlayer* aPlayer)
	{
	iPlayer = aPlayer;
	iLayout = 0;
	iTimegraph = 0;
	iClock = 0;
	iState = EStopped;
	iSchedule = 0;

	iAccumulatedTime = 0;

	iSchedulingEnabled = EFalse;
	iEventsEnabled = ETrue;
	iProcessing = EFalse;

	User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this);

	iMediaSelected = EFalse;

	iEndAction = EStopAtEnd;

	iTwoWayNavigation = ETrue;

	iLastEventTime.HomeTime();

	iVolume = JEEVES_MEDIUM_VOLUME;

	iSlideModeEnabled = EFalse;

	iStartAction = EDoNothing;

	iInBounds = 0;

	iDelayedAnchor = 0;

	iFirstFramePaused = EFalse;

	iRTL = EFalse;
	}

CSmilPresentation::CSmilPresentation()
: CActive(0)
	{
	
	}
        
CSmilPresentation::~CSmilPresentation()
	{
	Cancel();
	iTimer.Close();
	ClearTransitions();
	ClearSchedule();
	delete iTimegraph;
	delete iLayout;
	}

EXPORT_C void CSmilPresentation::Draw(CGraphicsContext& aGc, const TRect& aRect)
	{
//	RDebug::Print(_L("CSmilPresentation::Draw()"));
	if (iLayout)
		{
		iLayout->Draw(aGc,aRect);
		}
	}

void CSmilPresentation::RunL()
	{
	ProcessScheduledL();
	}

void CSmilPresentation::DoCancel()
	{
	iTimer.Cancel();
	}

void CSmilPresentation::AfterL(MSmilActive* aActive, const TSmilTime& aDelay, TBool aLazy)
	{
	TSmilTime at = iClock + aDelay;
	if (aLazy && at<RealTime())
		{
		// use lazy timing, get back to real time, but with 
		// some granularity so that things slip in sync
		TInt rt = RealTime().Value();	
		//at = rt - rt%100 + 100;
		at = rt;
		}
	ScheduleUpdateL(at, aActive);
	}

void CSmilPresentation::CancelActive(MSmilActive* aActive)
	{
	ScheduleItem* si = iSchedule;
	ScheduleItem* prev = 0;
	while (si)
		{
		if (si->iActive == aActive)
			{			
			if (prev)
				prev->iNext = si->iNext;
			else
				{
				iSchedule = si->iNext;
				ScheduleNext();
				}
			delete si;
			
			break;
			}
		prev = si;
		si = si->iNext;
		}
	}

void CSmilPresentation::ProcessScheduledL()
	{
	if (!iTimegraph)
		return;

	iProcessing = ETrue;
    
    TInt err( KErrNone );
    TBool loop( ETrue );
    
    while ( loop )
    	{ 
    	TRAP( err, ProcessScheduledRecL() );

    	if ( err != KErrNone )
    		{		
    		ClearTransitions();
    		ClearSchedule();
    		delete iTimegraph;
    		delete iLayout;
    		iTimegraph = 0;
    		iLayout = 0;
    		Cancel();
    		iPlayer->PresentationEvent( err, this );
    		loop = EFalse;
    		}
    	else if ( iSchedule && 
    	          iSchedule->iTime - RealTime() <= 0 )
    		{
    		// Process next schedule event right away
    		// if activation time is already now (or has passed)
    		loop = ETrue;
    		}
    	else
    		{
    		loop = EFalse;
    		}
    	}

	iProcessing = EFalse;

	if (iTimegraph)
		{
		if (iDelayedAnchor)
			{
			TRAP(err, ActivateAnchorL(iDelayedAnchor));
			iDelayedAnchor = 0;
			}

		if (iClock >= iTimegraph->EndTime())			
			{
			switch (iEndAction)
				{				
				case EStopAtEnd:
					StopL();
					break;
				case EPauseAtEnd:
					// handled in ProcessScheduledRecL()
					break;
				case ERepeatAtEnd:				
					StopL();
					PlayL();
					break;
				}

			iPlayer->PresentationEvent(MSmilPlayer::EEndReached,this);
			}
		}

	TRAP(err,AutoFocusL());
	}

void CSmilPresentation::ProcessScheduledRecL()
	{
#ifdef SCHEDULING_DEBUG
	RDebug::Print(_L("CSmilPresentation::ProcessScheduledRec() clock=%d"),iClock.Value());
#endif

	Cancel(); // to be sure

	MSmilActive* active = 0;

	if (iSchedule)
		{
#ifdef SCHEDULING_DEBUG		
		RDebug::Print(_L("scheduled time=%d"),iSchedule->iTime.Value());
#endif		

		iClock = iSchedule->iTime;	
	
		if (iEndAction==EPauseAtEnd && iClock >= iTimegraph->EndTime())
			{
			// pause automatically at end of the presentation if requested
			PauseInternal();
			return;
			}

		active = iSchedule->iActive;
		ScheduleItem* next = iSchedule->iNext;
		delete iSchedule;		
		iSchedule = next; // may be null		
		}	

#ifdef TIMING_DEBUG
 	RDebug::Print(_L("CLOCK: %d"),iClock);
#endif

	iSchedulingEnabled = EFalse;

	if (active)
		{
		//RDebug::Print(_L("processing active..."));
		active->ActivateL(active->iSmilObject->GlobalToLocal(iClock));
		}
	else
		{
		//RDebug::Print(_L("proceeding..."));
		iTimegraph->ProceedL(iClock);
		}

	// find the next activation time from the timegraph
	TSmilTime act = iTimegraph->NextActivation(iClock);
	ScheduleUpdateL(act);
	
	iSchedulingEnabled = ETrue;

#ifdef TIMING_DEBUG
	iTimegraph->PrintTree();
#endif

	if (iSchedule)
		{
		if (iSchedule->iTime - RealTime() > 0)
			{
			// nothing to run right now, wait for next
			ScheduleNext();
			}
		}
	}


//
// set up the timer to wait for the next item in schedule
//
void CSmilPresentation::ScheduleNext()
	{	
	if (!iSchedulingEnabled || iState!=EPlaying) //avoid re-entrancy!
		return;	

	Cancel();

	if (!iSchedule)
		return;	

	TInt delay = I64INT((iSchedule->iTime - RealTime()).ToMicroSeconds().Int64());

	if (delay<0) delay=0;

#ifdef SCHEDULING_DEBUG
	RDebug::Print(_L("scheduling next %d delay=%d"),iSchedule->iTime.Value(), delay);
#endif

	if (iSlideModeEnabled && (CurrentSlide()==SlideCount()))
		{
		iPlayer->PresentationEvent(MSmilPlayer::ELastSlideReached,this);
		}
	
	iTimer.After(iStatus, delay);		
	SetActive();
	}

void CSmilPresentation::ScheduleUpdateL(const TSmilTime& aTime, MSmilActive* aActive)
	{
#ifdef SCHEDULING_DEBUG
	RDebug::Print(_L("CSmilPresentation::ScheduleUpdate(%d) clock=%d"),aTime.Value(),iClock.Value());
#endif

	if (!iTimegraph)
		return;

	if (!aTime.IsFinite())
		return;

	TSmilTime t = aTime;

	if (t<=iClock)
		{
		if (aActive)
			t = iClock;
		else
			return;
		}

	ScheduleItem* si = iSchedule;
	if (!si || t < si->iTime )
		{
		ScheduleItem* iNew = new(ELeave) ScheduleItem;
		iNew->iNext = si;
		iNew->iTime = t;
		iNew->iActive = aActive;
		iSchedule = iNew;
		}
	else if (si->iTime!=t || si->iActive!=aActive)
		{
		while (si->iNext && (si->iNext->iTime < t || (aActive&&si->iNext->iTime == t)) )
			si = si->iNext;
		
		if ( (!si->iNext || si->iNext->iTime!=t) || si->iNext->iActive != aActive)
			{
			ScheduleItem* iNew = new(ELeave) ScheduleItem;
			iNew->iNext = si->iNext;
			iNew->iTime = t;
			iNew->iActive = aActive;

			si->iNext = iNew;
			}
		}
	}

void CSmilPresentation::ClearSchedule()
	{
	ScheduleItem* si = iSchedule;
	while (si)
		{
		ScheduleItem* del = si;
		si = si->iNext;
		delete del;
		}
	iSchedule = 0;
	}

TSmilTime CSmilPresentation::RealTime()
	{
	TTime t;
	t.HomeTime();
	TTimeIntervalMicroSeconds inter = t.MicroSecondsFrom(iStartTime);
	TSmilTime res = TSmilTime::FromMicroSeconds(inter) + iAccumulatedTime;
	//RDebug::Print(_L("RealTime(): %d"),res.Value());
	return res;
	}

EXPORT_C void CSmilPresentation::PlayL()
	{
	iProcessing = ETrue;
	if (iState==EStopped && iTimegraph)
		{
		iSchedulingEnabled = EFalse;

		iAccumulatedTime = 0;
		iClock = 0;
		if (iFirstFramePaused)
			{
			// already resolved...
			iTimegraph->SetPaused(EFalse);
			iFirstFramePaused = EFalse;
			}
		else
			{
			iTimegraph->ResolveL(0);
			}

		iState = EPlaying;

		iStartTime.HomeTime();

		iTimegraph->ProceedL(0);

		AutoFocusL();

		ScheduleUpdateL(iTimegraph->NextActivation(iClock));

		iSchedulingEnabled = ETrue;

		iPlayer->PresentationEvent(MSmilPlayer::EStarted,this);
		
		ScheduleNext();	
		}
	iProcessing = EFalse;

	// maybe we already got an anchor to follow?
	if (iDelayedAnchor)
		{
		ActivateAnchorL(iDelayedAnchor);
		iDelayedAnchor = 0;
		}

	if (Duration()==0 && iState == EPlaying && !iSchedule)
		{
		// absolutely nothing to do, get out
		StopL();
		iPlayer->PresentationEvent(MSmilPlayer::EEndReached,this);
		}
	}


EXPORT_C void CSmilPresentation::StopL()
	{
	iSchedulingEnabled = EFalse;	
	if (iTimegraph)
		{
		iTimegraph->EndL(RealTime());
		Cancel();
		ClearSchedule();
		iTimegraph->Reset();

		if (GetMediaFactory())
			GetMediaFactory()->PresentationEnd();		

		iPlayer->PresentationEvent(MSmilPlayer::EStopped, this);		
		}
	iState = EStopped;
	iClock = 0;	
	iAccumulatedTime = 0;
	if (iFocusedObject)
		iFocusedObject->SetFocus(EFalse);
	iFocusedObject=0;
	iMediaSelected=EFalse;

	if (iStartAction==EShowFirstFrame && iTimegraph)
		{
		
		iTimegraph->ResolveL(0);
		iStartTime.HomeTime();
		iTimegraph->ProceedL(0);
		iTimegraph->SetPaused(ETrue);
		iFirstFramePaused = ETrue;
		}
	}

EXPORT_C void CSmilPresentation::Pause()
	{
	if (iState==EPlaying)
		{
		iClock = RealTime();
		PauseInternal();
		}

	}

void CSmilPresentation::PauseInternal()
	{
	iSchedulingEnabled = EFalse;
	Cancel();
	iTimegraph->SetPaused(ETrue);
	iState = EPaused;	
	iPlayer->PresentationEvent(MSmilPlayer::EPaused,this);
	}

EXPORT_C void CSmilPresentation::Resume()
	{
	if (iState==EPaused)
		{
		iSchedulingEnabled = ETrue;
		iState = EPlaying;
		iAccumulatedTime = iClock;
		iStartTime.HomeTime();
		iTimegraph->SetPaused(EFalse);		

		TRAPD(err,ScheduleUpdateL(iTimegraph->NextActivation(iClock)));
		if (err!=KErrNone)
			return;
		ScheduleNext();
		iPlayer->PresentationEvent(MSmilPlayer::EResumed,this);
		}
	}

EXPORT_C TInt CSmilPresentation::SlideCount() const
	{
	if (!SlideModeEnabled())
		return 0;
	TInt c=0;
	CSmilObject* o = iTimegraph->FirstChild();
		if (o && o->IsSequence())
			o = o->FirstChild();
	for (;o;o=o->NextSibling())
		{
		c++;
		}
	return c;
	}

EXPORT_C TInt CSmilPresentation::CurrentSlide() const
	{
	if (!SlideModeEnabled())
		return 0;
	TInt c=1;
	CSmilObject* o = iTimegraph->FirstChild();
	if (o && o->IsSequence())
		o = o->FirstChild();
	for (;o;o=o->NextSibling())
		{
		if (o->EndTime()<=iClock)
			c++;
		else
			break;
		}
	return c;
	}

EXPORT_C TBool CSmilPresentation::NextSlideL() 
	{
	if (!SlideModeEnabled())
		return EFalse;
	if (iState!=EStopped)
		{
		CSmilObject* o = iTimegraph->FirstChild();
		if (o && o->IsSequence())
			o = o->FirstChild();
		for (;o;o=o->NextSibling())
			{
			TSmilTime t = o->EndTime();
			if (t.IsFinite() && t>iClock)
				{	
				Pause();
				SeekL(t);
				Resume();
				return ETrue;
				}
			}
		}
	return EFalse;
	}

EXPORT_C TBool CSmilPresentation::PreviousSlideL() 
	{
	if (!SlideModeEnabled())
		return EFalse;
	if (iState!=EStopped)
		{
		CSmilObject* o = iTimegraph->LastChild();
		if (o && o->IsSequence())
			o = o->LastChild();
		for (;o;o=o->PreviousSibling())
			{
			TSmilTime t = o->EndTime();
			if (t.IsFinite() && t<=iClock)
				{	
				Pause();
				SeekL(o->BeginTime());
				Resume();
				return ETrue;
				}
			}
		}
	return EFalse;
	}

EXPORT_C TBool CSmilPresentation::SeekL(TSmilTime aTo)
	{
//	if (aTo==iClock)
//		return ETrue;

	if (aTo > iClock)
		{
		iEventsEnabled = EFalse;
		iSchedulingEnabled = EFalse;
		
		Cancel();

		ScheduleUpdateL(aTo);

		while (iClock < aTo)
			{
			if (iSchedule)
				{
				iClock = iSchedule->iTime;
				ScheduleItem* next = iSchedule->iNext;
				delete iSchedule;		
				iSchedule = next; // may be null		
				}	

#ifdef SCHEDULING_DEBUG
	 		RDebug::Print(_L("Seeking, CLOCK: %d"),iClock);
#endif
			
			iTimegraph->ProceedL(iClock);			

			iPlayer->PresentationEvent(MSmilPlayer::ESeeked,this);
			}		

		iAccumulatedTime = iClock;
		iStartTime.HomeTime();

		iEventsEnabled = ETrue;

		if (iState==EPlaying)
			{
			iSchedulingEnabled = ETrue;
			ScheduleNext();
			}
		else
			iTimegraph->SetPaused(ETrue);
		}

	else
		{
		iEventsEnabled = EFalse;
		iSchedulingEnabled = EFalse;	

		Cancel();

		TSmilTime t = iClock;
		iClock = aTo-1; // to allow schedule collecting
		
		iTimegraph->RewindL(t, aTo);		

		iClock = aTo;

		iAccumulatedTime = aTo;

		iStartTime.HomeTime();

		iTimegraph->ProceedL(iClock);

		iPlayer->PresentationEvent(MSmilPlayer::ESeeked,this);

		iEventsEnabled = ETrue;

		if (iState==EPlaying)
			{			
			iSchedulingEnabled = ETrue;
			ScheduleNext();
			}
		else
			iTimegraph->SetPaused(ETrue);

		}

	AutoFocusL();

	return EFalse;	
	}

TBool CSmilPresentation::SeekL(CSmilObject* aTo)
	{
	if (aTo->IsPlaying())
		return SeekL(aTo->LocalToGlobal(aTo->BeginTime()));
	else if (aTo->BeginTime().IsFinite())
		return SeekL(aTo->LocalToGlobal(aTo->FirstInterval().iBegin));
	else
		{
		TSmilTime newBegin = RealTime();
		CSmilTimeContainer* p; 
		CSmilObject* c; 
		do
			{
			p = aTo->ParentObject();
			c = aTo;
			while (p && !p->BeginTime().IsFinite())
				{
				c = p;
				p = p->ParentObject();
				}
			if (!p)
				break;
			TSmilTime restime = 0;
			if (p->IsSequence())
				{				
				for (CSmilObject* o=p->FirstChild(); o&&o!=c; o=o->NextSibling())
					{
					if (o->BeginTime().IsFinite()) 
						restime = o->BeginTime();
					else
						o->ResolveBeginTimeL(restime);
					if (o->EndTime().IsFinite()) 
						restime = o->EndTime();
					else
						{
						restime = TSmilTime::ToLocal(p,newBegin);
						o->ResolveEndTimeL(restime);
						}
					}
				}
			c->ResolveBeginTimeL(restime);
			} 
			while(c!=aTo);

		return SeekL(aTo->LocalToGlobal(aTo->BeginTime()));
		}

	}

EXPORT_C TPtrC CSmilPresentation::BaseUrl() const
	{
	return iBaseUrl.Text();
	}


EXPORT_C void CSmilPresentation::SetBaseUrlL(const TDesC& aUrl)
	{
	iBaseUrl.SetTextL(aUrl);
	}

EXPORT_C TSmilTime CSmilPresentation::CurrentTime()
	{
	if (iState==EPlaying)
		return RealTime();
	else
		return iClock;
	}

EXPORT_C CSmilPresentation::TPresentationState CSmilPresentation::State()
	{
	return iState;
	}

EXPORT_C TSmilTime CSmilPresentation::Duration() const
	{
	if (!iTimegraph)
		return TSmilTime::KUnresolved;

	return iTimegraph->EndTime();
	}

EXPORT_C void CSmilPresentation::SubmitEventL(const TDesC& aName, const TDesC& aSource)
	{
	TSmilTime rt = RealTime();
	TSmilEvent ev(rt,aName);
	ev.SetSourceNameL(aSource);
	SubmitEventL(ev, rt);
	}

EXPORT_C void CSmilPresentation::SubmitEventL(const TDesC& aName, CSmilObject* aSource)
	{
	TSmilTime rt = RealTime();

	SubmitEventL(TSmilEvent(rt,aName,aSource), rt);	
	}

void CSmilPresentation::SubmitEventL(const TSmilEvent& aEvent, TSmilTime aTime)
	{
	if (!iEventsEnabled)
		return;

	if (iTimegraph && (iState==EPlaying || !aEvent.IsInteractive()))
		{

		if (!aTime.IsFinite())
			aTime = aEvent.iTime;

		if (aTime<iClock) aTime = iClock; //should not happen			
		
		iTimegraph->PropagateEventL(aEvent, aTime);

		ScheduleUpdateL(iTimegraph->NextActivation(iClock));
				
		ScheduleNext();		

		}
	}

EXPORT_C void CSmilPresentation::SetVolume(const TInt aVolume)
	{ 
	CSmilObject* o = iTimegraph;
	while (o)
		{
		if(o->IsMedia())
			{
				MSmilMediaRenderer* r = static_cast<CSmilMedia*>(o)->GetRenderer();
				if (r)
					r->SetVolume(aVolume);
	
			}
		if (o->FirstChild())
			o = o->FirstChild();
		else
			{
			while (o && !o->NextSibling())
				o = o->ParentObject();
			if (o)
				o = o->NextSibling();
			}			
		}
	iVolume = aVolume;
	}

void CSmilPresentation::ActivateAnchorL(const CSmilAnchor* anchor)
	{
	if (iProcessing)
		{
		iDelayedAnchor = anchor;
		return;
		}

	const TDesC& url = anchor->Href();
	if (url.Length()>0)
		{
		if (url[0]=='#')
			{
			if (url.Length()>1 && iTimegraph)
				{
				CSmilObject* o = iTimegraph->FindNamedObject(url.Mid(1));
				if (o)
					{
					SeekL(o);
					}						
				}
			}					
		else
			if (iPlayer)
				iPlayer->OpenDocumentL(url,anchor,this);
		}
	}

EXPORT_C void CSmilPresentation::MouseClickedL(const TPoint& aPoint)
	{
// 	RDebug::Print(_L("mouse clicked at %d,%d"), aPoint.iX, aPoint.iY);

	if (iLayout)
		{
		CSmilMedia* m = iLayout->MediaAt(aPoint,ETrue);

		if (m)
			{			
			SetFocusedObjectL(m);	
			ActivateFocusedL();			
			}
		else
			{
			CSmilArea* a = iLayout->AreaAt(aPoint,ETrue);
			if (a)
	    		{			
				SetFocusedObjectL(a);
				ActivateFocusedL();
		    	}
		    }
	    }
    }

EXPORT_C void CSmilPresentation::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	if (!iLayout)
		return;

	CSmilMedia* m = iLayout->MediaAt(aPointerEvent.iPosition,EFalse);
	if(m && m->GetRenderer() && 
		m->GetRenderer()->ConsumePointerEventL(aPointerEvent))
		{
		return;
		}
		
	TSmilTime rt = RealTime();
	if (iInBounds && iInBounds!=m)
		{
		SubmitEventL(TSmilEvent(rt, TSmilEvent::EOutOfBounds,iInBounds));	
		}
        
    CSmilObject* object = m;
    
    if ( object )
        {
        SubmitEventL( TSmilEvent( rt+1, TSmilEvent::EInBounds, object ) );           
        }
    else 
        {
        object = iLayout->AreaAt( aPointerEvent.iPosition, ETrue );                
        }
     
    if ( object )
        {
        if (    aPointerEvent.iType == TPointerEvent::EButton1Down
            &&  object->IsFocusable() )
            {
            SetFocusedObjectL( object );   
            }
        else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
            {
            ActivateFocusedL();         
            }
        }
    
	iInBounds = m;
	}

EXPORT_C TKeyResponse CSmilPresentation::HandleKeyEventL( const TKeyEvent& aKeyEvent,
                                                          TEventCode /*aType*/ )
    {
//	RDebug::Print(_L("KEY %d"),aKeyEvent);

	// discard events during processing
	if (iProcessing)
		return EKeyWasNotConsumed;

	// discards events coming too fast to prevent
	// sequential smilevents with the same timestamp
	switch (aKeyEvent.iCode)
		{
	case EKeyLeftArrow:
	case EKeyRightArrow:
	case EKeyUpArrow:
	case EKeyDownArrow:
	case EKeyEnter:
	case EKeyDevice3:
	case '1': case '2': case '3': case '4': case '5': case '6':
	case '7': case '8': case '9': case '0': case '*': case '#':
		{
		TTime t;
		t.HomeTime();
		TTimeIntervalMicroSeconds inter = t.MicroSecondsFrom(iLastEventTime);
		iLastEventTime = t;
		if (inter.Int64()<100000)
			return EKeyWasNotConsumed;		
		}
		break;
	default:
		return EKeyWasNotConsumed;
		}

	TBool consumed = EFalse;
	switch (aKeyEvent.iCode)
		{
	case EKeyLeftArrow:
		consumed = MoveDirectionL(-1,0);
		break;
	case EKeyRightArrow:
		consumed = MoveDirectionL(1,0);
		break;
	case EKeyUpArrow:
		consumed = MoveDirectionL(0,-1);
		break;
	case EKeyDownArrow:
		consumed = MoveDirectionL(0,1);
		break;
	case EKeyEnter:
	case EKeyDevice3:
	case EKeyDevice7:
		consumed = SelectionKeyPressedL();
		break;	
	case '1': case '2': case '3': case '4': case '5': case '6':
	case '7': case '8': case '9': case '0': case '*': case '#':
		{
		CSmilObject* o = iTimegraph;
		while (o)
			{
			if (o->IsPlaying() && o->Anchor() && o->Anchor()->iAccessKey==aKeyEvent.iCode)
				break;
			if (o->FirstChild())
				o = o->FirstChild();
			else
				{
				while (o && !o->NextSibling())
					o = o->ParentObject();
				if (o)
					o = o->NextSibling();
				}				
			}
			
		if (o)
			{
			SetFocusedObjectL(o);	
			ActivateFocusedL();
			consumed = ETrue;
			}
		break;
		}
	default: 
		;
		}	
	
	if (iState==EPlaying && iSchedule && iSchedule->iTime - RealTime() <= 0)
		{
		ProcessScheduledL();
		}

	return consumed?EKeyWasConsumed:EKeyWasNotConsumed;
    }

TBool CSmilPresentation::MoveDirectionL(TInt aX, TInt aY)
	{
	if (!iFocusedObject)
		AutoFocusL();

	if (!iFocusedObject)
		return EFalse;

	CSmilMedia* m = 0;
	if (iFocusedObject->IsMedia())
		m = static_cast<CSmilMedia*>(iFocusedObject);
	if (m && iMediaSelected)
		{
		if (m->GetRenderer() && m->GetRenderer()->IsScrollable())
			{
			m->GetRenderer()->Scroll(aX,aY);
			return ETrue;
			}
		}
	else 
		{
		if (!iTwoWayNavigation || aX==0)
			{
			MoveFocusL(aX,aY);
			return ETrue;
			}
		}
	return EFalse;
	}

EXPORT_C void CSmilPresentation::MoveFocusL(TInt aX, TInt aY)
	{
	if (!iFocusedObject)
		return;

//	RDebug::Print(_L("moving focus %d,%d"), aX, aY);

	CSmilObject* f = 0;

	if (aX==0)
		{
		if (aY<0)
			{
			if (iTwoWayNavigation)
				{
				//f = iLayout->FindNearest(fp, CSmilRegion::ETopTwoWay);
				CLinkedList<CSmilObject*>::Iter it(iFocusOrder);
				CSmilObject* p = 0;
				while(it.HasMore())
					{
					CSmilObject* o = it.Next();
					if (o==iFocusedObject && p)
						break;
					if (o->IsVisible())
						p = o;
					}
				if (p)
					f = p;
				}
			else
				{
				f = iLayout->FindNearest(iFocusedObject->Center(), CSmilRegion::ETop);						
				}
			}
		else
			{
			if (iTwoWayNavigation)
				{
				//f = iLayout->FindNearest(fp, CSmilRegion::EBottomTwoWay);
				CLinkedList<CSmilObject*>::Iter it(iFocusOrder);
				TBool found = EFalse;
				CSmilObject* first = 0;
				while(it.HasMore())
					{
					CSmilObject* o = it.Next();
					if (!first && o->IsVisible())
						first = o;
					if (o==iFocusedObject)
						found = ETrue;
					else if (found && o->IsVisible())
						{
						f = o;
						break;
						}
					}
				if (!f)
					f = first;
				}
			else
				f = iLayout->FindNearest(iFocusedObject->Center(), CSmilRegion::EBottom);
			}
		}
	else if (aX<0)
		f = iLayout->FindNearest(iFocusedObject->Center(), CSmilRegion::ELeft);
	else
		f = iLayout->FindNearest(iFocusedObject->Center(), CSmilRegion::ERight);
	
	if (f && f!=iFocusedObject)
		{
		SetFocusedObjectL(f);	
		}
	}

EXPORT_C void CSmilPresentation::CancelFocusL()
	{
	if (iFocusedObject) 
		{
		iFocusedObject->SetFocus(EFalse);
		iFocusedObject->Redraw();

		SubmitEventL(TSmilEvent(RealTime(),TSmilEvent::EFocusOut,iFocusedObject));	
		}
	iFocusedObject = 0;
	iMediaSelected=EFalse;
	}

void CSmilPresentation::AutoFocusL()
	{
	if (iFocusedObject && !iFocusedObject->IsVisible())
		{
		iFocusedObject->SetFocus(EFalse);
		iFocusedObject=0;
		}
	if (!iFocusedObject)
		{
		iMediaSelected=EFalse;
		CSmilObject* f=0;
		CLinkedList<CSmilObject*>::Iter it(iFocusOrder);
		TInt visibleCount=0;
		while(it.HasMore())
			{
			CSmilObject* o = it.Next();
			if (o->IsVisible())
				{
				if (!f)
					f = o;
				++visibleCount;
				}
			}
		if (f)
			{
			SetFocusedObjectL(f);
			}
		}
	}

EXPORT_C CSmilObject* CSmilPresentation::FocusedObject() const
	{
	return iFocusedObject;
	}

void CSmilPresentation::SetFocusedObjectL(CSmilObject* aFocus)
	{
	// is it already focused?
	if (iFocusedObject==aFocus && iFocusedObject->HasFocus())
		return;

	TSmilTime rt = RealTime();
	if (iFocusedObject) 
		{		
		SubmitEventL(TSmilEvent(rt,TSmilEvent::EFocusOut,iFocusedObject));	
		iFocusedObject->SetFocus(EFalse);
		iFocusedObject->Redraw();
		}
	iFocusedObject = aFocus;
	iFocusedObject->SetFocus(ETrue);
	iFocusedObject->Redraw();

	SubmitEventL(TSmilEvent(rt+1,TSmilEvent::EFocusIn,iFocusedObject));	
	}

EXPORT_C void CSmilPresentation::ActivateFocusedL()
	{

	if (iFocusedObject) 
		{
		SubmitEventL(TSmilEvent(RealTime(),TSmilEvent::EActivate,iFocusedObject));			
		if (iFocusedObject->Anchor())
			{
			ActivateAnchorL(iFocusedObject->Anchor());			
			}
		}
	}

TBool CSmilPresentation::SelectionKeyPressedL()
	{
	if (iFocusedObject) 
		{
		if (iFocusedObject->IsScrollable())
			{
			iMediaSelected = !iMediaSelected;
			iFocusedObject->GetRegion()->Redraw();
			}

		ActivateFocusedL();
		return ETrue;
		}
	return EFalse;
	}

EXPORT_C TBool CSmilPresentation::IsTransparent() const
	{
	return !iLayout || iLayout->iTransparent;
	}

MSmilMediaFactory* CSmilPresentation::GetMediaFactory()
	{
	return iPlayer->GetMediaFactory(this);
	}

MSmilTransitionFactory* CSmilPresentation::GetTransitionFactory()
	{
	return iPlayer->GetTransitionFactory(this);
	}

void CSmilPresentation::SetLayout(CSmilRootRegion* aRoot)
	{
	iLayout = aRoot;
	}

void CSmilPresentation::SetTimegraph(CSmilTimeContainer* aBody)
	{	
	iTimegraph = aBody;
	}

void CSmilPresentation::AddTransitionL(CSmilTransition* aTransition)
	{
	if (iTransitions)
		aTransition->iNext = iTransitions;
	iTransitions = aTransition;		
	}

void CSmilPresentation::ClearTransitions()
	{
	CSmilTransition* tr = iTransitions;
	while (tr)
		{
		CSmilTransition* del = tr;
		tr = tr->iNext;
		delete del;
		}
	iTransitions = 0;
	}

CSmilTransition* CSmilPresentation::FindTransition(const TDesC& aName)
	{

	TInt b = 0;
	TInt e = 0;

	while (e<aName.Length())
		{
		while (e<aName.Length() && aName[e]==' ')
			e++;
		b=e;
		while (e<aName.Length() && aName[e]!=';')
			e++;
		TInt te = e-1;
		while (te>b && aName[te]==' ')
			te--;
		if (te>=b)
			{
			TPtrC tran = aName.Mid(b,te-b+1);

	//		RDebug::Print(_L("tran=%S"),&tran);

			for (CSmilTransition* tr = iTransitions; tr; tr = tr->iNext)
				{
				MSmilTransitionFactory* tf = GetPlayer()->GetTransitionFactory(this);
				if (tr->Id()==tran && tf && tf->IsSupportedType(tr->Type()))
					return tr;
				}
			}
		e++;		
		}

	
	return 0;
	}

void CSmilPresentation::PrintDebug()
    {
	if (iLayout)
	    {
		RDebug::Print(_L("Layout:\n"));
		iLayout->PrintTree();
	    }
		
	if (iTimegraph)
	    {
		RDebug::Print(_L("Timegraph:\n"));
		iTimegraph->PrintTree();
	    }
    }

void CSmilPresentation::ReadyL()
	{
	if (iTimegraph && iLayout)
		{
		iProcessing = ETrue;
		iLayout->Layout();
		iTimegraph->ResolveL();
		UpdateFocusListL();
		GetMediaFactory()->PresentationReady();
		iProcessing = EFalse;
		}

	}

TBool FocusOrderCompareLTR(CSmilObject* l,CSmilObject* r)
	{
	TInt lo = (l->Anchor()&&l->Anchor()->iTabIndex!=CSmilAnchor::KTabUnspecified)
		? l->Anchor()->iTabIndex
		: 0xfffffff;
	TInt ro = (r->Anchor()&&r->Anchor()->iTabIndex!=CSmilAnchor::KTabUnspecified)
		? r->Anchor()->iTabIndex
		: 0xfffffff;
	if (lo==ro)
		{
		TPoint lc = l->Center();
		TPoint rc = r->Center();
		lo = lc.iY;
		ro = rc.iY;
		if (lo==ro)
			{
			// left to right navigation order
			lo = lc.iX;
			ro = rc.iX;
			}
		}
	return lo<=ro;
	}

TBool FocusOrderCompareRTL(CSmilObject* l,CSmilObject* r)
	{
	TInt lo = (l->Anchor()&&l->Anchor()->iTabIndex!=CSmilAnchor::KTabUnspecified)
		? l->Anchor()->iTabIndex
		: 0xfffffff;
	TInt ro = (r->Anchor()&&r->Anchor()->iTabIndex!=CSmilAnchor::KTabUnspecified)
		? r->Anchor()->iTabIndex
		: 0xfffffff;
	if (lo==ro)
		{
		TPoint lc = l->Center();
		TPoint rc = r->Center();
		lo = lc.iY;
		ro = rc.iY;
		if (lo==ro)
			{
			// right to left navigation order
			ro = lc.iX;
			lo = rc.iX;
			}
		}
	return lo<=ro;
	}

void CSmilPresentation::UpdateFocusListL()
	{
	iFocusOrder.Empty();
	for (CSmilObject* o = iTimegraph; o; o = o->NextAdjacent())
		{
		if (o->IsFocusable())
			{
			iFocusOrder.AddL(o);
			}
		else if (o==iFocusedObject)
            {
             // if currently focused object is removed from the list,
             // unfocus it
             CancelFocusL();
		}
		}
	if (iRTL)
		iFocusOrder.Sort(&FocusOrderCompareRTL);
	else
		iFocusOrder.Sort(&FocusOrderCompareLTR);
	}

EXPORT_C TReal32 CSmilPresentation::GetScalingFactor()
    {
	return iLayout->GetScalingFactor();
    }

EXPORT_C TRect CSmilPresentation::RootRegionRect() const
    {
    return iLayout->GetRectangle();
    }

TReal32 CSmilPresentation::GetMaxDownScaling()
    {
	return (TReal32)iDown;
    }
    
TReal32 CSmilPresentation::GetMaxUpScaling()
    {
	return (TReal32)iUp;
    }

EXPORT_C TBool CSmilPresentation::IsInteractive() const
	{
	if (iFocusOrder.IsEmpty())
		return EFalse;
	CLinkedList<CSmilObject*>::Iter it(iFocusOrder);
	TInt count=0;
	while(it.HasMore())
		{
		CSmilObject* o = it.Next();
		// scrollables don't make the presentation interactive
		// ### warning, may not be good enough
		if (o->IsScrollable())
			{
			if (o->IsVisible())
				++count;
			if (count>1)
				return ETrue;
			}
		else
			return ETrue;
		}
	return EFalse;
	}

// End of file
