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
* Description: smilregioninterface  declaration
*
*/



#ifndef SMILREGIONINTERFACE
#define SMILREGIONINTERFACE

#include <e32std.h>
#include <gdi.h>


class CSmilMedia;

/**
 * Interface representing a drawing region
 */
class MSmilRegion
	{
	public:
		enum TFit
			{
			EFill,
			EMeet,	
			ESlice,	
			EScroll,
			EHidden
			};

		enum TBgMode
			{
			EAlways,
			EWhenActive
			};

		/**
		* Coordinates of the drawing area
		*/
		virtual TRect GetRectangle() const = 0;

		/**
		* Coordinates of the media object within the rectangle. 
		* Empty rect if the media is not visible on the region
		*/
		virtual TRect GetRectangle(const CSmilMedia* aMedia) const = 0;

		/**
		* z-index of the region. Regions with higher z-index are 
		* drawn on top of lower ones
		*/
		virtual TInt GetZIndex() const = 0;

		/**
		* Value of SMIL fit attribute. Values are:
		*
		* EFill - media on this regions is scaled to fill it
		* EMeet - media is scaled so that it fills the region in 
		*		one direction while maintaining the aspect ratio
		* ESlice - scale media to fill the region, while 
		*		maintaining the aspect ration and slicing off 
		*		the overflowing part 
		* EScroll - provide scrolling mechanism for the media
		* EHidden - don't scale, slice off the overflow
		*/
		virtual TFit GetFit() const = 0;

		/**
		* Foreground color
		*/
		virtual TRgb GetForeground() const = 0;

		/**
		* Background color
		*/
		virtual TRgb GetBackground() const = 0;

		/**
		* Is background transparent?
		*/
		virtual TBool BackgroundTransparent() const = 0;

		/**
		* Background mode of the region. Values match those of
		* SMIL showBackground attribute:
		* 
		* EAlways - background is always drawn
		* EWhenActive - background is drawn when some media is 
		*			active on the region
		*/
		virtual TBgMode GetBackgroundMode() const = 0;

		/**
		* Requests redrawing of this region and its visible contents
		*/
		virtual void Redraw()=0;

		/**
		* Requests redrawing of this region and its visible contents.
		* Update area is given in the coordinate space of the region
		*/
		virtual void Redraw(TRect aUpdate)=0;

		/**
		*Return the parent region
		*/
		virtual MSmilRegion* Parent() const = 0;

		/**
		* Return the root region
		*/
		virtual MSmilRegion* Root() const = 0;

		/**
		* Called to by the Engine to tell that a piece of media 
		* is currently visible on this region
		*/
		virtual void AddMediaL(CSmilMedia* aMedia)=0;

		/**
		* Called to by the Engine to tell that a piece of media is 
		* no longer visible on this region
		*/
		virtual void RemoveMedia(CSmilMedia* aMedia)=0;

		/**
		* Is the media currently visible on the region?
		*/
		virtual TBool ContainsMedia(const CSmilMedia* aMedia) const = 0;

		/**
		* True if this is the root region
		*/
		virtual TBool IsRoot() const = 0;

		/**
		* Called to by the Engine to tell that a piece of media
		* on the region has ended its transition 
		* (and fill=transition semantics should be applied)
		*/
		virtual void MediaTransitionEnd(CSmilMedia* aMedia)=0;
	};

#endif
