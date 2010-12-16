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
* Description: smilpresentation  declaration
*
*/



#ifndef SMILPRESENTATION_H
#define SMILPRESENTATION_H

//  INCLUDES
#include <e32base.h>
#include <coedef.h>
#include <w32std.h>

#include <smiltextbuf.h>

#include <smiltime.h>
#include <smilevent.h>
#include <smillinkedlist.h>
#include <smilregioninterface.h>
#include <smilplayerinterface.h>

#include <smilmediafactoryinterface.h>

#define JEEVES_MEDIUM_VOLUME 50
#define JEEVES_MAX_VOLUME 100

// FORWARD DECLARATIONS
class CSmilRootRegion;
class CSmilSequence;
class CSmilTimeContainer;
class CSmilTransition;
class CSmilAnchor;
class MSmilTransitionFactory;
class MSmilActive;

// CLASS DECLARATION

/**
 *  A SMIL presentation
 */
class CSmilPresentation : public CActive
    {
public:  // Constructors and destructor
    
	/**
    * Two-phased constructor.
    */
    IMPORT_C static CSmilPresentation* NewL(MSmilPlayer* aPlayer);	
    
    /**
    * Destructor.
    */
    virtual ~CSmilPresentation();


public: // New functions

	/**
	* Returns the MSmilPlayer object associated with this presentation
	*/
	inline MSmilPlayer* GetPlayer();

	/**
	* Called from outside of the engine to update the given 
	* region within the presentation area, using the given
	* graphics context The engine draws the region 
	* backgrounds and the media objects that are not 
	* responsible for performing their own drawing.
	*
	* This method should ultimately be called by the Draw 
	* method of the CCoeControl that contains the SMIL 
	* presentation. It should also be called as a result 
	* of MSmilPlayer::Redraw() call.
	*/

	IMPORT_C void Draw(CGraphicsContext& aGc, const TRect& aRect);

	/**
	* Starts the presentation playback.
	*/
    IMPORT_C void PlayL();

	
	/**
	* Stops the presentation playback, resetting the 
	* timeline to the beginning and possibly freeing some 
	* allocated resouces.
	*/
    IMPORT_C void StopL();


	/**
	* Pauses the presentation playback.
	*/
    IMPORT_C void Pause();


	/**
	* Resumes the presentation playback.
	*/
    IMPORT_C void Resume();
	

	/**
	* Tries to seek the presentation to the given position
	* in the timeline. If this fails, the presentation is 
	* stopped.
	*
	* Returns TRUE if succesful.
	*/
	IMPORT_C TBool SeekL(TSmilTime aTo);


	/**
	* Returns the base URL of the presentation
	*/
	IMPORT_C TPtrC BaseUrl() const;

	
	/**
	* Sets the base URL of the presentation
	*/
	IMPORT_C void SetBaseUrlL(const TDesC& aUrl);


	/**
	* Current position on presentation timeline
	*/
	IMPORT_C TSmilTime CurrentTime();


	enum TPresentationState
	{
		EPlaying,
		EStopped,
		EPaused
	};

	/**
	* State of the presentation:
	*
	* EStopped
	* EPlaying
	* EPaused
	*/
	IMPORT_C TPresentationState State();


	IMPORT_C TSmilTime Duration() const;

	/** 
	* Submits an event to the Engine. Events are 
	* characterised by event name and event source.
	*
	* Event source is a name of a source element. If 
	* the source is within the timegraph the name can 
	* be obtained using TSmilObject::GetName() method.
	* 
	* Event name is a name of an event class, for example 
	* "activateEvent".
	*/
	IMPORT_C void SubmitEventL(const TDesC& aName, const TDesC& aSource);

	IMPORT_C void SubmitEventL(const TDesC& aName, CSmilObject* aSource);

	/** 
	 * Convenience method for generating UI events
	 */
	IMPORT_C void MouseClickedL(const TPoint& aPoint);

	/** 
	 * Move media focus maintained by the SMIL engine to given direction.
	 * HandleKeyEvent replaces this method in most cases.
	 */
	IMPORT_C void MoveFocusL(TInt aX, TInt aY);

	/**
	 * Remove focus if exists
	 */
	IMPORT_C void CancelFocusL();

	/**
	 * Return currently focused object, or null
	 */
	IMPORT_C CSmilObject* FocusedObject() const;

	/** 
	 * Generate activate event for currently focused media (if there is one)
	 */
	IMPORT_C void ActivateFocusedL();

	/**
	 * Set to true to enable slide mode used for playing back Conformance
	 * Document MMS messages. Use NextSlide(), CurrentSlide(), SlideCount() to navigate.
	 *
	 */
	inline void SetSlideModeEnabled(TBool aEnable);

	inline TBool SlideModeEnabled() const;

	/**
	 * Handle Symbian OS key event. This may generate SMIL events, may move key focus
	 * and may cause media to scroll
	 */
	IMPORT_C TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

	/**
	 * Handle Symbian OS mouse event. This may generate SMIL events, may move key focus
	 * and may cause media to scroll
	 */
	IMPORT_C void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	/**
	 * Return the sequence number (starting from 1) of current slide (top level <body> child element)
	 */
	IMPORT_C TInt CurrentSlide() const;

	/**
	 * Return total number of slides in the presentation (top level <body> child elements)
	 */
	IMPORT_C TInt SlideCount() const;

	/**
	 * Go to next slide in the presentation:
	 */
	IMPORT_C TBool NextSlideL();

	/**
	 * Go to previous slide in the presentation:
	 */
	IMPORT_C TBool PreviousSlideL();

	/**
	 * Returns true if presentation background is transparent
	 */
	IMPORT_C TBool IsTransparent() const;

	/**
	 * If true, using two-way (up-down) navigation mode, four-way otherwise
	 */
	inline TBool TwoWayNavigation() const;

	/**
	 * If set to true (default), use two-way (up-down) navigation mode, four-way otherwise
	 */
	inline void SetTwoWayNavigation(TBool aEnable);

	/**
	 * If set to true (default), scale down oversized SMIL layouts
	 */
	inline TBool ScaleLayout() const;

	/**
	 * If set to true (default), scale down oversized SMIL layouts
	 */
	inline void SetScaleLayout(TBool aScale);

	/**
	 * Volume getter  function. SetVolume notifies renderes about changes.
	 * Range: [0..100]
	 */
	inline TInt Volume(); 

	/**
	 * Volume setter function. SetVolume notifies renderes about changes.
	 * Range: [0..100]
	 */
	IMPORT_C void SetVolume(const TInt aVolume);


	enum TEndAction { EStopAtEnd, EPauseAtEnd, ERepeatAtEnd };

	/**
	 * What to do at the end of the presentation. 
	 * Values: EStopAtEnd, EPauseAtEnd, ERepeatAtEnd
	 */
	inline void SetEndAction(TEndAction aEndAction);

	/**
	 * What to do at the end of the presentation. 
	 * Values: EStopAtEnd, EPauseAtEnd, ERepeatAtEnd
	 */
	inline TEndAction GetEndAction() const;


	enum TStartAction { EDoNothing, EShowFirstFrame };

	/**
	 * What to do when then the presentation is in the beginning
	 * (stopped) and not playing.
	 * Values: EDoNothing, EShowFirstFrame
	 */
	inline void SetStartAction(TStartAction aStartAction);

	/**
	 * What to do when then the presentation is in the beginning
	 * (stopped) and not playing.
	 * Values: EDoNothing, EShowFirstFrame
	 */
	inline TStartAction GetStartAction() const;


	/**
	 * Sets the keyboards focus navigation order
	 * EFalse: from left to right
	 * ETrue: from right to left
	 */
	inline void SetRTLNavigation(TBool aRTL);


	/**
	 * Gets the keyboards focus navigation order
	 * EFalse: from left to right
	 * ETrue: from right to left
	 */
	inline TBool RTLNavigation() const;

	inline void SetMaxDownUpScaling( TReal aDown, TReal aUp );

	TReal32 GetMaxDownScaling();

	TReal32 GetMaxUpScaling();

	/**
	 * Returns true if the presentation contains interactive elements
	 * (besides scrollable media)
	 */
	IMPORT_C TBool IsInteractive() const;
 
	IMPORT_C TReal32 GetScalingFactor();
	
	/**
	 * Returns root region rectangle.
	 */
	IMPORT_C TRect RootRegionRect() const;

public:

	inline CSmilRootRegion* GetLayout();
	void SetLayout(CSmilRootRegion* aRoot);

	inline CSmilTimeContainer* GetTimegraph();
	void SetTimegraph(CSmilTimeContainer* aBody);

	
	MSmilMediaFactory* GetMediaFactory(); 
	MSmilTransitionFactory* GetTransitionFactory(); 
	
	void SetFocusedObjectL(CSmilObject* aFocus);

	void AddTransitionL(CSmilTransition* aTransition);
	void ClearTransitions();
	CSmilTransition* FindTransition(const TDesC& aName);

	void ProcessScheduledL();
	void ProcessScheduledRecL();
	void ScheduleNext();
	void ClearSchedule();
	void ScheduleUpdateL(const TSmilTime& aTime, MSmilActive* aActive = 0);

	void PrintDebug();	

	TSmilTime RealTime();

	TBool SeekL(CSmilObject* aTo);

	void SubmitEventL(const TSmilEvent& aEvent, TSmilTime aNow = TSmilTime::KUnresolved);

	void ReadyL();

	void ActivateAnchorL(const CSmilAnchor* anchor);	

	void AfterL(MSmilActive* aActive, const TSmilTime& aDelay, TBool aLazy);
	void CancelActive(MSmilActive* aActive);

	inline TBool MediaSelected() const;

	void UpdateFocusListL();

protected:
	

	friend class CSmilParser;

	/** constructor */
	CSmilPresentation(); 
	void ConstructL(MSmilPlayer* aPlayer);

	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError( TInt /*aError*/ );
	
	void PauseInternal();

	TBool iSlideModeEnabled;

	void AutoFocusL();
	TBool SelectionKeyPressedL();
	TBool MoveDirectionL(TInt aX, TInt aY);


private:

	CSmilRootRegion* iLayout;
	CSmilTimeContainer* iTimegraph;

	RSmilTextBuf iBaseUrl;

	MSmilPlayer* iPlayer;

	TSmilTime iClock;

	TPresentationState iState;

	RTimer iTimer;


	TTime iStartTime;
	TSmilTime iAccumulatedTime;

	struct ScheduleItem
		{
		ScheduleItem* iNext;
		TSmilTime iTime;
		MSmilActive* iActive;
		};

	ScheduleItem* iSchedule;		

	CSmilTransition* iTransitions;
	CSmilObject* iFocusedObject; 
	CSmilMedia* iInBounds;

	TInt iVolume;

	TBool iMediaSelected;
	
	TBool iEventsEnabled;
	TBool iSchedulingEnabled;		
	TBool iProcessing;

	TEndAction iEndAction;
	TStartAction iStartAction;

	TTime iLastEventTime;

	TBool iTwoWayNavigation;

	CLinkedList<CSmilObject*> iFocusOrder;

	const CSmilAnchor* iDelayedAnchor;

	TBool iFirstFramePaused;

	TBool iRTL;
	
		TReal iDown;
		TReal iUp;
};

#include <smilpresentation.inl>

#endif      // ?INCLUDE_H   
            
// End of File
