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
* Description: smiltransitionfilter implementation
*
*/



#include "smiltransitionfilter.h"

#include "smilmediarendererinterface.h"
#include "smilpresentation.h"

#include "smilmedia.h"

// ----------------------------------------------------------------------------
// CSmilTransitionFilter::CSmilTransitionFilter
// ----------------------------------------------------------------------------
//
EXPORT_C CSmilTransitionFilter::CSmilTransitionFilter()
	{
	}

// ----------------------------------------------------------------------------
// CSmilTransitionFilter::ConstructL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSmilTransitionFilter::ConstructL(MSmilMedia* aMedia)
	{
	iTransOut = EFalse;
	iPercent = 0;
	iIsActive = EFalse;
	iStartTime = TSmilTime::KUnresolved;
	iMedia = static_cast<CSmilMedia*>(aMedia);
	}

// ----------------------------------------------------------------------------
// CSmilTransitionFilter::~CSmilTransitionFilter
// ----------------------------------------------------------------------------
//
EXPORT_C CSmilTransitionFilter::~CSmilTransitionFilter()
	{
	}
	
// ----------------------------------------------------------------------------
// CSmilTransitionFilter::TransitionStartingL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSmilTransitionFilter::TransitionStartingL()
	{
	}
	
// ----------------------------------------------------------------------------
// CSmilTransitionFilter::TransitionEnding
// ----------------------------------------------------------------------------
//
EXPORT_C void CSmilTransitionFilter::TransitionEnding()
	{
	}

// ----------------------------------------------------------------------------
// CSmilTransitionFilter::ActivateL
// aTime is local time (i.e. relative to current repeat begin).
// ----------------------------------------------------------------------------
//
EXPORT_C void CSmilTransitionFilter::ActivateL( const TSmilTime& aTime )
	{
	if ( !iIsActive )
	    {
	    return;
	    }
		

	if ( !iMedia )
	    {
	    return;
	    }
	    
	TSmilTime currentTime( aTime );
	    
	// We have to modify currentTime and startTime if transition is used on
	// object that is repeating.
	if ( iSmilObject &&
	     iSmilObject->RepeatBeginTime().IsFinite() &&
	     iSmilObject->IsRepeating() )
	    {
	    if ( iTransOut )
	        {
	        // Transition out.
	        if ( !iStartTime.IsFinite() )
		        {
		        // Calculate start time so that it is time from the beginning of media's
		        // active duration and not from beginning of current loop begin time.
		        iStartTime = iSmilObject->RepeatBeginTime() - iSmilObject->BeginTime() + aTime;
		        }
		                    
	        // Calculate current time so that it is time from the beginning
	        // of media's active duration and not from the beginning of current loop begin time.
	        currentTime = iSmilObject->RepeatBeginTime() - 
	                      iSmilObject->BeginTime() + 
	                      aTime;
	        }
	    else
    	    {
    	    // Transition in. 
    	    if ( !iStartTime.IsFinite() )
		        {
		        iStartTime = aTime;
		        }
		        
	        // Calculate current time so that it is time from the beginning
	        // of media's active duration and not from the beginning of current loop begin time.
	        currentTime = iSmilObject->RepeatBeginTime() - iSmilObject->BeginTime() + aTime;
	        }
	    }
	else
	    {
	    if ( !iStartTime.IsFinite() )
		    {
		    iStartTime = aTime;
		    }
	    }
	
	iTransitionTime = currentTime - iStartTime;

	TInt oldPercent = iPercent;
	
	if ( iTransitionTime < iTransition->iDur )
		{
		iMedia->AfterL( this, ResolutionMilliseconds(), ETrue );

		TInt rem = iTransition->iEndProgress - iTransition->iStartProgress;
		TInt transitionDur = iTransition->iDur.Value();
		if ( transitionDur > 0 )
			{
			if ( transitionDur > 200 )
			    {
			    transitionDur -= 200;
			    }
			    
			iPercent = iTransition->iStartProgress + iTransitionTime.Value() * rem / transitionDur;
			}
			
		if ( iPercent > iTransition->iEndProgress )
		    {
		    iPercent = iTransition->iEndProgress;
		    }
		}
	else
		{
		iPercent = iTransition->iEndProgress;

		EndTransition();
		}

	if ( oldPercent != iPercent )
	    {
	    iMedia->Redraw();
	    }
	}

// ----------------------------------------------------------------------------
// CSmilTransitionFilter::Media
// ----------------------------------------------------------------------------
//
EXPORT_C MSmilMedia* CSmilTransitionFilter::Media() const
	{	
	return iMedia;
	}

// ----------------------------------------------------------------------------
// CSmilTransitionFilter::BeginTransitionL
// ----------------------------------------------------------------------------
//
void CSmilTransitionFilter::BeginTransitionL()
	{
	iTransitionTime = 0;	
	iPercent = iTransition->iStartProgress;	

	if (!iMedia)
		return;

	if (iTransition->iDur>0)
		{				
		iMedia->AfterL(this, ResolutionMilliseconds(), ETrue );
		iIsActive = ETrue;
		TransitionStartingL();
		}
	}

// ----------------------------------------------------------------------------
// CSmilTransitionFilter::EndTransition
// ----------------------------------------------------------------------------
//
void CSmilTransitionFilter::EndTransition()
	{
	if (iIsActive)
		{
		iIsActive = EFalse;
		iStartTime = TSmilTime::KUnresolved;
		iMedia->Presentation()->CancelActive(this);
		iMedia->TransitionFilterEnd(this);
		TransitionEnding();
		}

	}

// ----------------------------------------------------------------------------
// CSmilTransitionFilter::Close
// ----------------------------------------------------------------------------
//
void CSmilTransitionFilter::Close()
	{
	iMedia->CancelActive(this);
	delete this;
	}
	
//  End of File
