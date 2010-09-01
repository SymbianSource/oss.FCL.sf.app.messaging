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
* Description: smilmediarendererinterface  declaration
*
*/



#ifndef SMILMEDIARENDERERINTERFACE_H
#define SMILMEDIARENDERERINTERFACE_H

#include <e32std.h>
#include <gdi.h>

#include <smiltime.h>
#include <smilmediainterface.h>
#include <smilfocusinterface.h>

class CSmilTransitionFilter;

/**
 * Interface for concrete media renderers. Each media
 * renderer class is responsible of playback of some
 * media type.
 */
class MSmilMediaRenderer
	{
	public:

		/**
		* Called by the engine to signal a renderer that the engine is
		* no more referencing it. Renderer may be deleted in this call.
		*/
		virtual void Close() = 0;

		/**
		* Returns TRUE if the media type is visual. Anything that draws 
		* its content is visual media. Audio is an example of non-visual media.
		*/
		virtual TBool IsVisual() const = 0;

		/**
		* Returns TRUE if a visual media covers every pixel of its 
		* drawing surface fully
		*/
		virtual TBool IsOpaque() const = 0;

		/**
		* Returns TRUE if a visual media is scrollable
		*/
		virtual TBool IsScrollable() const = 0;

		/**
		* Returns TRUE if the media renderer has care of updating its 
		* drawing area itself, asynchronously to  the SMIL engine. A video or 
		* audio renderer might work like this.
		*/
		virtual TBool IsControl() const = 0;

		/**
		* Returns the intrinsic (unscaled) size of a visual media object in pixels. 
		*/
		virtual TInt IntrinsicWidth() const = 0;

		/**
		* Returns the intrinsic (unscaled) size of a visual media object in pixels. 
		*/
		virtual TInt IntrinsicHeight() const = 0;

		/**
		* Returns the intrinsic duration of a media object. This value can also be 
		* Indefinite (for media that continues indefinitely, like repeating animation) 
		* or Unresolved (for media that's length is not known, like streamed video). 
		* Discrete media (like images) have intrinsic duration of zero.
		*/
		virtual TSmilTime IntrinsicDuration() const = 0;

		/**
		* Called by the engine to signal the renderer that the media is going to 
		* be played soon (this might involve reserving some resource, opening 
		* connections, decoding headers to find out intrinsic media values, etc)
		*/
		virtual void PrepareMediaL()	= 0;	

	
		/**
		* Called by the engine to seek media to given position
		* on its local timeline
		*/
		virtual void SeekMediaL(const TSmilTime& aTime) = 0;

		/**
		* Called by the engine to start media playback and to 
		* make visual media visible. The playback is started from the current 
		* position on the media timeline.
		*/
		virtual void ShowMediaL() = 0;

		/**
		* Called by the engine to stop the media playback and remove 
		* visual media from the screen. Timeline can be reset.
		*/
		virtual void HideMedia()	= 0;

		/**
		* Called by the engine to pause the media playback while keeping the 
		* visual presentation (for example the current video frame) visible.
		*/
		virtual void FreezeMedia() = 0;

		/**
		* Called by the engine to continue the media playback after freeze
		*/
		virtual void ResumeMedia() = 0;


		/**
		* The engine calls this method to make a media renderer to 
		* draw its content. The graphics context to draw to is passed as a 
		* parameter. In addition, the exact rectangle that needs updating is 
		* provided, relative to the graphics context. Renderer may use this 
		* information to optimize drawing.All drawing must be performed 
		* synchronously during this method call. 
		*
		* Renderer should do the drawing using the transition filter passed as 
		* a parameter and draw the focus indicator if needed. Both these 
		* parameters may be null.
		*/
		virtual void Draw(CGraphicsContext& aGc, const TRect& aRect, CSmilTransitionFilter* aTransitionFilter, const MSmilFocus* aFocus) = 0;


		/**
		* Move scrollable media to given direction (-1=left/up, 1=down/right)
		*/
		virtual void Scroll(TInt aDirX, TInt aDirY) = 0;

		/**
		* Set volume of audio media (0-100). 
		*/
		virtual void SetVolume(TInt aVolume) = 0;

		/**
		* Handle pointer events inside the renderer. If this method return ETrue
		* the event is consumed and not processed by the SMIL engine. If it returns
		* EFalse, the event is processed normally.
		*/
		virtual TBool ConsumePointerEventL(const TPointerEvent& /*aPointerEvent*/) {return EFalse;}


	};


#endif
