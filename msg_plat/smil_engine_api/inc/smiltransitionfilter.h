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
* Description: smiltransitionfilter  declaration
*
*/



#ifndef SMILTransitionFilter_H
#define SMILTransitionFilter_H

#include <e32base.h>

#include <smiltime.h>

#include <smiltransition.h>
#include <smilactiveinterface.h>

class CSmilMedia;
class MSmilMedia;

/**
 * Subclasses of this class are concrete transition effect implementations
 */
class CSmilTransitionFilter : public CBase, private MSmilActive
	{
	public:

		/**
		 * Invoke this from the subclass constructor
		 */
		IMPORT_C
		virtual void ConstructL(MSmilMedia* aMedia);		

		/**
		 * Virtual destructor
		 */
		IMPORT_C
		virtual ~CSmilTransitionFilter();		

		/**
		 * Draw the aTargetBitmap to the graphics context, using the optional aMaskBitmap,
		 * implementing a transition effect.
		 * The iPercent field, running from 0 to 100, tells the current state of the transition.
		 */
		virtual void Draw(CGraphicsContext& aGc, const TRect& aRect, 
			const CFbsBitmap* aTargetBitmap, const TPoint& aTarget, const CFbsBitmap* aMaskBitmap = 0) = 0;
				
		/** 
		 * Implement to return how often the transition should be updated, in milliseconds
		 */				
		virtual TInt ResolutionMilliseconds() const = 0;

		/**
		 * Pointer to the media object the transition is attached to
		 */
		IMPORT_C MSmilMedia* Media() const;

		void BeginTransitionL();
		void EndTransition();

		/**
		 * Internal, don't use
		 */
		IMPORT_C
		void ActivateL(const TSmilTime& aTime);
		void Close();

	protected:

		/**
		 * Override to get signaled when a transition starts (may allocate resources)
		 */
		IMPORT_C
		virtual void TransitionStartingL();

		/**
		 * Override to get signaled when a transition ens (may free resources)
		 */
		IMPORT_C
		virtual void TransitionEnding();


		/**
		 * Constructor
		 */
		IMPORT_C CSmilTransitionFilter();		

		TSmilTime iStartTime;
		TSmilTime iTransitionTime;

		CSmilMedia* iMedia;

	public:		

		CSmilTransition* iTransition;

		TBool iTransOut;

		TInt iPercent;

		TBool iIsActive;
	
	};


#include <smilmediainterface.h>

#endif
