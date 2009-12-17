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
* Description: smilmedia implementation
*
*/



// INCLUDE FILES
#include "smilmedia.h"
#include "smilmediarendererinterface.h"
#include "smilpresentation.h"
#include "smilregion.h"
#include "smilrootregion.h"
#include "smiltimecontainer.h"

#include "smiltransition.h"
#include "smiltransitionfilter.h"
#include "smiltransitionfactoryinterface.h"
#include "smillinkedlist.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSmilMedia* CSmilMedia::NewL(CSmilPresentation* aPresentation)
	{
	CSmilMedia* ptr = new ( ELeave ) CSmilMedia;
	CleanupStack::PushL( ptr );
	ptr->ConstructL(aPresentation);
	CleanupStack::Pop(); //ptr
	return ptr;
	}

CSmilMedia::CSmilMedia() 
	{
	iRegion = 0;
	iRenderer = 0;
	iTransitionIn = 0;
	iTransitionOut = 0;
	iTransitionFilter = 0;
	iClipBegin = 0;
	iClipEnd = TSmilTime::KIndefinite;
	iFocus = EFalse;
	iFocusable = EFalse;
	}

// Destructor
CSmilMedia::~CSmilMedia()
	{
	if (iRenderer)
		iRenderer->Close();

	if (iTransitionFilter)
		iTransitionFilter->Close();

	iParams.ResetAndDestroy();
	iParams.Close();

	if (iRegion)
		iRegion->RemoveMedia( this );
	else if ( iPresentation && iPresentation->GetLayout() )
		iPresentation->GetLayout()->RemoveMedia( this );
	}



void CSmilMedia::ResolveL( const TSmilTime& aTps )
	{
	CSmilTimeContainer::ResolveL(aTps);	
	if(!iRenderer)
		{
		CreateRendererL(aTps);
		}
	}


CSmilArea* CSmilMedia::GetArea()
{
	for( CSmilObject* c = iFirstChild; c != NULL; c = c->NextSibling() )
	{
		if (c->IsArea()) return (CSmilArea *)c;
	}
	return NULL;          
}


void CSmilMedia::ProceedL( const TSmilTime& aTime )
	{
	TSmilTime oldBegin = iCurrentInterval.iBegin;

	// some clip semantics
	TSmilTime clip = iClipEnd - iClipBegin;
	if (clip.IsFinite() && clip>0 && 
		iRepeatBeginTime.IsFinite() && iRepeatBeginTime+clip>=aTime)
		iForcedRepeat = ETrue;

	CSmilTimeContainer::ProceedL( aTime );
		
	// process <area> elements
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
	
	// start out transition
	if (iTransitionOut)
		{
		TSmilTime assumedOutStart = PredictedRemovalTime()  - iTransitionOut->iDur;
		if (aTime>=assumedOutStart)
			{
			if (!iTransitionFilter || !iTransitionFilter->iTransOut) 
				{

				if (iTransitionFilter)
					{
					iTransitionFilter->Close();
					iTransitionFilter = 0;
					}

				MSmilTransitionFactory* tf = iPresentation->GetTransitionFactory();
				if (tf) 
					{
					iTransitionFilter = tf->CreateFilterL(iTransitionOut,this);
					}

				}
			if (iTransitionFilter && !iTransitionFilter->iIsActive)
				{
				iTransitionFilter->iTransOut = ETrue;
				iTransitionFilter->BeginTransitionL();				
				}
			}
		}

	// min/max: implement fill at end of non-minlimited duration
	if (iMinLimited && iPlaying)
		{
        if (CalcIntermediateActiveDuration()<=aTime-iCurrentInterval.iBegin)
            {
			if (iRenderer)
				{
				if (iFill == EFillRemove)				
					{
					iRenderer->HideMedia();

					if (iRenderer->IsVisual())
						GetRegion()->RemoveMedia( this );
					}
				else
					iRenderer->FreezeMedia();
				}
            }		
		}

	if (oldBegin.IsUnresolved() && iCurrentInterval.iBegin.IsFinite() && !iRenderer)
		{
		CreateRendererL(aTime);
		}
	}


void CSmilMedia::ForceRepeat( TBool aBool )
	{
	iForcedRepeat = aBool;
	}


void CSmilMedia::BeginL( const TSmilTime& aTime )
	{
	CSmilTimeContainer::BeginL( aTime );

	// test some cases where there is no need to
	// actually show anything
	if( iCurrentInterval.iBegin == iCurrentInterval.iEnd && ( iFill == EFillRemove || 
		( iFill == EFillFreeze && iParentObject && iParentObject->IsSequence()
		&& iNext && iNext->BeginTime() == iCurrentInterval.iEnd && iPresentation->Duration()!=0) ) )
		return;
	if( iParentObject && !iParentObject->IsPlaying() && iPresentation->Duration()!=0)
		return;

	if (!iRenderer)
		{
		CreateRendererL(iCurrentInterval.iBegin);
		}

	if (iRenderer)
		iRenderer->PrepareMediaL();

	if (iRenderer)
		{
		if (iRenderer->IsVisual())
			GetRegion()->AddMediaL( this );

		if (iTransitionFilter && iTransitionFilter->iTransition==iTransitionOut)
			iTransitionFilter->EndTransition();			

		if (iTransitionIn)
			{
			MSmilTransitionFactory* tf = iPresentation->GetTransitionFactory();
			if (tf) 
				{
				if (!iTransitionFilter || iTransitionFilter->iTransition != iTransitionIn)
					{				
					if (iTransitionFilter)
						{
						iTransitionFilter->Close();
						}
					iTransitionFilter = 0;
					iTransitionFilter = tf->CreateFilterL(iTransitionIn,this);
					}
				if (iTransitionFilter)
					{
					iTransitionFilter->iTransOut = EFalse;
					iTransitionFilter->BeginTransitionL();
					}
				}
			}
	
	
		iRenderer->SeekMediaL(aTime+iClipBegin);
		iRenderer->ShowMediaL();
		}
	}


TSmilTime CSmilMedia::NextActivation( const TSmilTime& aTime ) const
	{		

	TSmilTime res = CSmilTimeContainer::NextActivation(aTime);

	if (iTransitionOut)
		{
		TSmilTime assumedOutStart = PredictedRemovalTime()  - iTransitionOut->iDur;
		TSmilTime inEnd = iTransitionIn ? BeginTime()+iTransitionIn->iDur : BeginTime();
		if (assumedOutStart.IsFinite() && 
			 inEnd <= assumedOutStart ) 
			{
			if (assumedOutStart>aTime && assumedOutStart<res)
				res = assumedOutStart;
			}
		}

	if (iMinLimited)
		{
		TSmilTime minLimit = iCurrentInterval.iBegin+CalcIntermediateActiveDuration();
		if (minLimit>aTime && minLimit<res)
			res = minLimit;
		}

	return res;
	}


void CSmilMedia::RepeatL( const TSmilTime& aTime )
    {
	TSmilTime d = iSimpleDuration;
	CSmilTimeContainer::RepeatL( aTime );
	iSimpleDuration = d; // don't reset sd for media
	if (iRenderer)
		{
		iRenderer->SeekMediaL(iClipBegin);
		}
    }


void CSmilMedia::Unfreeze()
	{
	CSmilTimeContainer::Unfreeze();			

	if (!iPlaying && iRenderer)
		{		
		iRenderer->HideMedia();

		if (iTransitionFilter)
			{
			iTransitionFilter->EndTransition();	
			}

		if (iRenderer->IsVisual())
			GetRegion()->RemoveMedia(this);		
		}
	}

void CSmilMedia::EndL( const TSmilTime& aTime )
	{
	CSmilTimeContainer::EndL( aTime );

	if (!iRenderer)
		return;

	if ( iFill==EFillRemove)
		{
		iRenderer->HideMedia();

		if (iRenderer->IsVisual())
			{
			GetRegion()->RemoveMedia( this );
			}

		if (iTransitionFilter)
			{
			iTransitionFilter->EndTransition();
			}
		}
	else
		{		
		iRenderer->FreezeMedia();
		}
	
	}


void CSmilMedia::SetAltL( const TDesC &aPtr )
	{
	iAlt.SetTextL(aPtr);
	}


void CSmilMedia::SetTypeL( const TDesC &aType )
	{
	iType.SetTextL(aType);
	}

void CSmilMedia::SetSrcL( const TDesC &aPtr )
	{
	iSrc.SetTextL(aPtr);

	iPresentation->GetMediaFactory()->RequestMediaL(iSrc.Text(),this);

	if (iRenderer)
		{
		iRenderer = 0;
		}					
	}


MSmilRegion* CSmilMedia::GetRegion() const
	{ 
	if (iRegion)
		return iRegion;
	else
		return iPresentation->GetLayout();
	}


TRect CSmilMedia::GetRectangle() const
	{
	return GetRegion()->GetRectangle(this);
	}

void CSmilMedia::SetPaused(TBool aPaused)
	{
	if (iRenderer && iPlaying)
		{
		if (aPaused)
			{
			iRenderer->FreezeMedia();
			}
		else
			iRenderer->ResumeMedia();
		}
	}


void CSmilMedia::Redraw() 
	{
	GetRegion()->Redraw(GetRectangle());		
	}


TSmilTime CSmilMedia::ImplicitDuration() const
	{
	if (iRenderer)
		{
		TSmilTime clip = iClipEnd - iClipBegin;
		if (clip.IsFinite() && clip>0 && clip<iRenderer->IntrinsicDuration())
			return clip;
		else
			return iRenderer->IntrinsicDuration();
		}
	else
		return TSmilTime::KUnresolved;
	}


void CSmilMedia::RendererDurationChangedL()
	{
#ifdef MEDIA_DEBUG
	RDebug::Print(_L("CSmilMedia::RendererIntrinsicValuesChanged()"));
#endif 

	RecalcInterval(iCurrentInterval.iBegin);
	iPresentation->ScheduleNext();
	}


void CSmilMedia::RendererAtEndL()
	{
#ifdef MEDIA_DEBUG
	RDebug::Print(_L("CSmilMedia::RendererAtEnd()"));
#endif 

	iForcedRepeat = ETrue;
	iPresentation->ScheduleUpdateL(iPresentation->RealTime());
	iPresentation->ScheduleNext();
	}

void CSmilMedia::TransitionFilterEnd(CSmilTransitionFilter* /*aTrR*/)
	{
	// called by transition renderer at the end of a transition
#ifdef MEDIA_DEBUG
	RDebug::Print(_L("CSmilMedia::TransitionRenderEnd()"));
#endif 

	if (iRegion)
		{
		iRegion->MediaTransitionEnd(this);
		}
		

	if (iRenderer)
		{		
		Redraw();
		}				
	}


void CSmilMedia::CreateRendererL(const TSmilTime& aRecalcTime)
	{
	TBool oldf = IsFocusable();

	MSmilMediaFactory::TMediaFactoryError err = 
		iPresentation->GetMediaFactory()->CreateRendererL(iSrc.Text(), this, iRenderer);
	if (err==MSmilMediaFactory::EOk) 
		{
		RecalcInterval(aRecalcTime);
		}
	else if (err==MSmilMediaFactory::ENotFound && iRenderer)
		{	
		// media not found, but we still got renderer (for alt text most likely)
		RecalcInterval(aRecalcTime);
		}

	if (oldf!=IsFocusable())
		iPresentation->UpdateFocusListL();

	if (err!=MSmilMediaFactory::EOk && iPresentation->GetPlayer())
		iPresentation->GetPlayer()->PresentationEvent(MSmilPlayer::EMediaNotFound, iPresentation, Src() );
	}


TBool CSmilMedia::IsFocusable() const 
	{
	if (iFocusable)
		return ETrue;

	if (GetRegion() && iRenderer && iRenderer->IsScrollable())
		return ETrue;

	return EFalse;
	}

CSmilPresentation* CSmilMedia::Presentation() const
	{
	return CSmilObject::Presentation();
	}

void CSmilMedia::AfterL(MSmilActive* iActive, const TSmilTime& aDelay, TBool aLazy)
	{
	CSmilObject::AfterL(iActive,aDelay,aLazy);		
	}

void CSmilMedia::CancelActive(MSmilActive* aActive) 
	{ 
	CSmilObject::CancelActive(aActive); 
	}


CSmilTransitionFilter* CSmilMedia::ActiveTransitionFilter() const
	{
	if (iTransitionFilter && 
		( iTransitionFilter->iIsActive
		|| (iTransitionFilter->iTransition==iTransitionOut&&!iPlaying)))
		return iTransitionFilter;
	else
		return 0;
	}

TPtrC CSmilMedia::GetParam(const TDesC& aName) const
	{
	for (TInt n=0; n<iParams.Count();n++)
		{
		if (iParams[n]->iName->Des()==aName)
			return *(iParams[n]->iValue);
		}
	return TPtrC();
	}

TSmilTime CSmilMedia::MediaBegin() const
    {
    return BeginTime();
    }

TSmilTime CSmilMedia::MediaEnd() const
    {
    return EndTime();
    }

void CSmilMedia::AddParamL(const TDesC& aName, const TDesC& aValue)
	{
	CNameValuePair* p = new (ELeave) CNameValuePair;
	CleanupStack::PushL(p);
	p->iName = aName.AllocL();
	p->iValue = aValue.AllocL();
	User::LeaveIfError(iParams.Append(p));
	CleanupStack::Pop(p);
	}


TBool CSmilMedia::IsVisible() const
	{
	return GetRegion()->ContainsMedia(this);
	}

TBool CSmilMedia::IsScrollable() const
	{
	return GetRegion()
		&& GetRenderer() 
		&& GetRenderer()->IsScrollable();
	}

TBool CSmilMedia::IsSelected() const
	{
	return iFocus && iPresentation->MediaSelected();
	}

TSmilTime CSmilMedia::PredictedRemovalTime() const
	{
	TSmilTime parentEnd = TSmilTime::KUnresolved;
	if (iParentObject)
		{
		parentEnd = iParentObject->EndTime()-iParentObject->BeginTime();
		if ( iParentObject->IsSequence() && NextSibling()
			&& NextSibling()->BeginTime()<parentEnd )
			{
			parentEnd = NextSibling()->BeginTime();
			}
		}
	if (iFill == EFillFreeze)
		{
		return parentEnd;
		}
	else
		{
		return EndTime()<parentEnd?EndTime():parentEnd;
		}
	}












