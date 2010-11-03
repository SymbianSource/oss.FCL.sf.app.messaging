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
* Description: smilplayerinterface  declaration
*
*/



#ifndef SMILPLAYERINTERFACE
#define SMILPLAYERINTERFACE

#include <e32std.h>
#include <gdi.h>

#include <smiltime.h>
#include <smilanchor.h>

#include <smilmediafactoryinterface.h>

class CCoeControl;
class CSmilPresentation;
class MSmilTransitionFactory;

/**
 * Callback interface between SMIL engine and player application.
 */
class MSmilPlayer
	{
	public:
		/**	
		* Return the media factory object that is responsible of 
		* creating the media renderers
		*/
		virtual MSmilMediaFactory* GetMediaFactory(const CSmilPresentation* aPres) const = 0;

		/**	
		* Return the transition factory object that is responsible of 
		* creating the transition renderers
		*/
		virtual MSmilTransitionFactory* GetTransitionFactory(const CSmilPresentation* aPres) const = 0;

		/**	
		* Evaluate a content control attribute for true or false. 
		* Attributes 
		*		systemOperatingSystem
		*		systemCPU
		*		systemScreenSize 
		*		systemScreenDepth 
		* are handled inside the SMIL engine, the rest are passed to this method.
		*/
		virtual TBool EvaluateContentControlAttribute(const TDesC& aName, const TDesC& aValue) = 0;

		/**	
		* Return the dimensions of the drawing surface
		*/
		virtual TRect GetDimensions(const CSmilPresentation* aPres)	= 0;

		/**	
		* Return the EIKON GUI control that is used as the view for the given 
		* presentation.
		*
		* SMIL Engine never calls this method. It might be used by 
		* implementations of MSmilMediaRenderer interface.
		*/
		virtual CCoeControl* GetControl(const CSmilPresentation* aPres) = 0;

		/**	
		* The engine calls this method to initiate redrawing of the given area. 
		* Implementation of this method should in turn call Draw method of the 
		* given presentation object.
		*/
		virtual void Redraw(TRect aArea, CSmilPresentation* aPres) = 0;

		/**	
		* This method is called by the engine as a result of hyperlink activation.
		* Player should open the given (relative to the presentation base) url. 
		* However, the current presentation MUST NOT be deleted during execution 
		* of this method, as this method is invoked by the current presentation,
		* 
		* aAnchor - contains addition information about the link. can be 0.
		*
		* source/destination states: 
		* EPlay - (continue) play the presentation
		* EStop - stop the source presentation or open destination presentation in stopped state
		* EPause - pause the source presentation, continue it after, destination finishes
		*
		* 3GPP profile does not require respecting these states, aAnchor may be ignored
		*/
		
		virtual void OpenDocumentL(const TDesC& aUrl, const CSmilAnchor* aAnchor, const CSmilPresentation* aPres) = 0;

		enum TEventType
			{
			EStarted,
			EStopped,
			EPaused,
			EResumed,
			ESeeked,
			EStalled,
			EMediaNotFound,
			EEndReached,
			ELastSlideReached
			};

		/**	
		* Invoked when a presentation state changes. aEvent is 
		* EStarted - presentation started playing,
		* EStopped - presentation was stopped,
		* EPaused - presentation was paused
		* EResumed - paused presentation was resumed
		* ESeeked - presentation was seeked to a new position on timeline
		* EMediaNotFound - opening media failed, aText contains the media url
		* or
		* EEndReached - presentation has reached its end time. whether it is paused or stopped depends
		* on EndAction setting.
		* one of the negative system wide error codes (from a trapped leave inside engine RunL() method). 
		* In this case the playing presentation is stopped automatically and the presentation can not
		* be restarted.
		*/

		virtual void PresentationEvent(TInt aEvent, const CSmilPresentation* aPres, const TDesC& aText = TPtrC()) = 0;


	};
#include <smiltransitionfactoryinterface.h>

#endif
