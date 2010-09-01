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
* Description: smilobject  declaration
*
*/



#ifndef SMILOBJECT_H
#define SMILOBJECT_H

//  INCLUDES
#include <e32base.h>

#include "smiltextbuf.h"

#include "smiltime.h"
#include "smiltimelist.h"
#include "smilinstancetime.h"
#include "smilcondition.h"

// CONSTANTS
//const TReal32 KFloatIndefinite = KMaxTReal32;
#define KFloatIndefinite KMaxTReal32
const TReal32 KFloatUnspecified = -12346;

// FORWARD DECLARATIONS
class CSmilTimeContainer;
class CSmilRegion;
class MSmilRegion;
class CSmilAnchor;
class CSmilPresentation;
class CSmilEvent;
class MSmilActive;
// CLASS DECLARATION


//< -- Fill values -- >
enum TFill  {  EFillAuto, EFillRemove, EFillFreeze, EFillHold, EFillTransition };
//< -- Restart values -->
enum TRestart { ERestartAlways, ERestartNever, ERestartWhenNotActive };
//< -- Endsync values -->
enum TEndsync { EEndsyncFirst, EEndsyncLast, EEndsyncAll, EEndsyncId };


class CSmilObject : public CBase
    {

    public:  // Constructors and destructor
		
		virtual const TDesC* ClassName() const { _LIT(KN,"CSmilObject"); return &KN; } 

		/**
        * Destructor.
        */
		virtual ~CSmilObject();

	public:	
		/**
        * Gets anchor object associated with this object.
        * @return CSmilAnchor* - pointer to the anchor object
        */
		IMPORT_C CSmilAnchor* Anchor() const;

		CSmilPresentation* Presentation() { return iPresentation; }
		CSmilObject* NextSibling() { return iNext; }
		CSmilObject* PreviousSibling() { return iPrevious; }		
		CSmilTimeContainer* ParentObject();
		virtual CSmilObject* FirstChild() const { return 0; }
		virtual CSmilObject* LastChild() const { return 0; }

		TSmilTime Duration() const { return iDuration; }				
		TRestart Restart() const { return iRestart; }
		
		TSmilTime Min() const { return iMin; }
		TSmilTime Max() const { return iMax; }
		TPtrC Id() const;
		TPtrC Title() const;
		TFill Fill() const;			
		TSmilTime RepeatDur() const;
		TReal32 RepeatCount() const;

		virtual TBool IsTimeContainer() const { return EFalse; }
		virtual TBool IsSequence() const { return EFalse; }
		virtual TBool IsParallel() const { return EFalse; }
		virtual TBool IsExclusive() const { return EFalse; }
		virtual TBool IsMedia() const { return EFalse; }
		virtual TBool IsPrefetch() const { return EFalse; }
		virtual TBool IsArea() const { return EFalse; }
		virtual TBool IsA() const { return EFalse; }

		virtual TBool IsVisible() const { return EFalse; }
		virtual TBool IsScrollable() const { return EFalse; }

		struct TInterval
			{
			TInterval()
				{
				iBegin = TSmilTime::KUnresolved;
				iEnd = TSmilTime::KUnresolved;
				}

			TInterval(const TSmilTime& aBegin, const TSmilTime& aEnd)
				{
				iBegin = aBegin;
				iEnd = aEnd;
				}

			TBool IsNull() { return iBegin.IsUnresolved(); }
			TSmilTime Duration() { return iEnd - iBegin; }

			TSmilTime iBegin;
			TSmilTime iEnd;
			};

		/**
        * Unfreeze an object.
        */
		virtual void Unfreeze() {};

		/**
        * Propagate an event throught the timing tree.
        */
		virtual void PropagateEventL( const TSmilEvent& aEvent, const TSmilTime& aTime );

		/**
        * A common "proceed" function for all timed objects.
		* Called each time a timed object receives the timing signal.
        */
		virtual void ProceedL( const TSmilTime& aTps );
		
		void ResolveL() { ResolveL(0); }
		virtual void ResolveL( const TSmilTime& aTps );

		// virtual void Seek( const TSmilTime& aTps, const TSmilTime& aTo );

		/**
        * A common "NotifyDependents" function for all timed objects.
        */
		virtual void NotifyDependents( const TSmilTime& aTime );

		/**
        * Get the implicit duration of an object.
        */
		virtual TSmilTime ImplicitDuration() const;

		/**
        * End an object & geberate an end event.
        */
		virtual void EndL( const TSmilTime& aTime );

		/**
        * Begin an object & generate a begin event.
        */
		virtual void BeginL( const TSmilTime& aTime );
		
		/**
        * Reset the timing values of an object.
        */
		virtual void Reset();
		
		/**
        * Repeat an object.
        */
		virtual void RepeatL( const TSmilTime& aTime );
		
		/**
        * Sets the default timing values of an object.
        */
		void Initialise();
		
		/**
        * Get the active time of an object.
        */
		TSmilTime ActiveTime();		
		
		/**
        * Does the object have a resolved begin time.?
        */
		TBool HasBeginTime();
		
		/**
        * Does the object have a begin condition?
        */
		TBool HasBeginEvent();

		/**
        * Does the object have a resolved end time?.
        */
		TBool HasEndTime();
		
		TBool HasEndEvent();


		void AddConditionL( const TSmilCondition& aCondition, TBool aBeginCondition );
		
			

		void SetRestart( TRestart aVal );

		/**
        * Set the repeatCount value.
        */
		void SetRepeatCount( TReal32 aRC );
		
		/**
        * Set the repeatDur value.
        */
		void SetRepeatDur( TSmilTime aRD );

		void SetFill(TFill aFill) { iFill = aFill; }

		/**
        * Set the "id" string.
        */
		void SetIdL( const TDesC& aString );
		
		/**
        * Set the "title" string.
        */
		void SetTitleL( const TDesC& aString );
		

		void SetAnchor( CSmilAnchor* aReference );
		
		void SetDuration( const TSmilTime& aDuration );
		
		void SetEndTime( const TSmilTime& aEndTime );


		void SetMax( const TSmilTime& aMax );
		void SetMin( const TSmilTime& aMin );

		/**
        * Set the parent object.
        */
		void SetParent( CSmilTimeContainer* aContainer );

		/**
        * Set the "next sibling.
        */
		void SetNextSibling( CSmilObject* aNext );

		/**
        * Set the previous sibling.
        */
		void SetPreviousSibling( CSmilObject* aPrevious );

		/**
        * Returns the next sibling.
        */
		CSmilObject* NextSibling() const;

		/**
        * Returns the previous sibling.
        */
		CSmilObject* PreviousSibling() const;

		CSmilObject* NextAdjacent() const;
		CSmilObject* PreviousAdjacent() const;
		
		CSmilPresentation* Presentation() const;

		
		virtual void SetEndsyncL(TEndsync /*	aEndsync*/, const TDesC& /*aId*/ = TPtrC()) { }

		TSmilTime EndTime() const;
		TSmilTime BeginTime() const;

		TBool IsPlaying() const;
	
		void RecalcInterval( const TSmilTime& aTime );

		virtual TSmilTime NextActivation( const TSmilTime& aTime ) const;			

		TInterval FirstInterval();
		TInterval NextInterval();
		TBool HasMoreIntervals();

		TSmilTime ParentSimpleEnd();

		TInt ActualRepeat() const;
		
		TBool EndSpecified();
		TBool BeginSpecified();
		
		TSmilTime RepeatBeginTime() const;
		
		TSmilTime CalcActiveEnd( TSmilTime aB ); 
		TSmilTime CalcActiveEnd( TSmilTime aB, TSmilTime aEnd );
		
		void ResolveSimpleDuration();
		
		TSmilTime CalcIntermediateActiveDuration() const;

		virtual void AttachedL();
		
		virtual void PrintTree(int d=0) const;		

		TBool IsRepeating() const;

		virtual void SetFocus(TBool ) { }
		virtual TBool HasFocus() const { return EFalse; }

		virtual void SetFocusable(TBool ) { }
		virtual TBool IsFocusable() const{ return EFalse; }
		

		virtual CSmilObject* FindNamedObject(const TDesC& aName);

		void ResolveBeginTimeL(TSmilTime aTime);
		void ResolveEndTimeL(TSmilTime aTime);

		TSmilTime LocalToGlobal(const TSmilTime& aTime);
		TSmilTime GlobalToLocal(const TSmilTime& aTime);

		virtual void RewindL(const TSmilTime& aTime, const TSmilTime& aTo);

		virtual void SetPaused(TBool /*aPaused*/) {};

		virtual void Redraw() {}
		virtual MSmilRegion* GetRegion() const { return 0;}
		virtual TPoint Center() const { return TPoint();}

		void AfterL(MSmilActive* iActive, const TSmilTime& aDelay, TBool aLazy = EFalse);
		void CancelActive(MSmilActive* aActive);

	protected:

		/**
        * C++ default constructor.
        */
		CSmilObject();
		
		/**
        * Base-class Constructor.
        */
		virtual void ConstructL(CSmilPresentation* aPresentation);

	protected:	// Data

		// style properties		
		TSmilTime								iDuration;
		TSmilTime								iRepeatDur;
		TReal32									iRepeatCount;
		TRestart								iRestart;
		TFill									iFill;
		TSmilTime								iMin;
		TSmilTime								iMax;
		RPointerArray<TSmilCondition>			iBeginConditions;
		RPointerArray<TSmilCondition>			iEndConditions;
		CSmilAnchor*							iAnchor;
		RSmilTextBuf							iId;
		RSmilTextBuf							iTitle;
				
		
		// object state

		TInterval								iCurrentInterval;

		TBool									iPlaying;
		TBool									iFirstInterval;		
				
		TSmilTimeList							iBeginList;
		TSmilTimeList							iEndList;

		TSmilTime								iRepeatBeginTime;
		TSmilTime								iSimpleDuration;
		TSmilTime								iActiveDuration;

		TInt									iActualRepeat;

		TBool									iMinLimited;

		TBool									iForcedRepeat;

		// tree management

		CSmilPresentation*						iPresentation;
		CSmilTimeContainer*						iParentObject;
		CSmilObject*							iPrevious;
		CSmilObject*							iNext;		

};

#include "smiltimecontainer.h"

#endif      // ?INCLUDE_H   
            
// End of File

