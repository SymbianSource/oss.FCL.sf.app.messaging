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
* Description: smilmediainterface  declaration
*
*/



#ifndef SMILMEDIAINTERFACE_H
#define SMILMEDIAINTERFACE_H

//  INCLUDES
#include <e32base.h>

#include <smilregioninterface.h>
#include <smilpresentation.h>

// FORWARD DECLARATIONS
class MSmilActive;

// CLASS DECLARATION

/**
*  Abstract base class for media objects
*/
class MSmilMedia
    {
    public:  

		virtual CSmilPresentation* Presentation() const = 0;

		/**
		* Returns the region where this media is supposed to be rendered on. May be NULL.
		*/ 
		virtual MSmilRegion* GetRegion() const = 0;

		/**
		* Returns the rectangle (region) where the media is supposed to be rendered on.
		*/
		virtual TRect GetRectangle() const = 0;

		/**
		* Invoked by renderer to ask redrawing of area covered by the media. 
		* This should be invoked when (visual) media appears, there are changes to it, and 
		* when it is removed.
		*/
		virtual void Redraw() = 0;

		/** 
		* Invoked by a MSmillMediaRenderer object when there is a change 
		* in (known) state of its intrinsic values. For example, duration of 
		* video clip is not typically known until it is at least partially 
		* decoded. This method should be invoked by the renderer when this 
		* information becomes available.
		*/
		virtual void RendererDurationChangedL() = 0;

		/**
		* Invoked by a MSmillMediaRenderer object when the associated media 
		* ends (end of the file or stream is reached). Renderer should also 
		* move to frozen state at this point (that is, to keep showing the last 
		* frame). 
		*/
		virtual void RendererAtEndL() = 0;

		/**
		 * Media URL
		 */
		virtual TPtrC Src() const = 0;

		/**
		 * Alt tag
		 */
		virtual TPtrC Alt() const = 0;
		
		/**
		 * Type attribute. Should be used to override the media type 
		 * checking in the media factory, for example
		 * <img src="jee.jpg" type="image/gif"/>
		 * should cause factory to try opening the image as gif.
		 */
		virtual TPtrC Type() const = 0;

		/**
		 * Invoke the ActivateL() method of iActive when delay aDelay has passed.
		 */
		virtual void AfterL(MSmilActive* iActive, const TSmilTime& aDelay, TBool aLazy = EFalse) = 0;

		/**
		 * Cancel a pending request
		 */
		virtual void CancelActive(MSmilActive* aActive) = 0;

		/**
		 * Get media parameter, or empty TPtrC if not found
		 */
		virtual TPtrC GetParam(const TDesC& aName) const = 0;
		
		/**
		 * Returns begin time of media relative to presentation begin time.
		 */
		virtual TSmilTime MediaBegin() const = 0;
        
        /**
		 * Returns end time of media relative to presentation begin time.
		 */
		virtual TSmilTime MediaEnd() const = 0;
    };

#endif      // ?INCLUDE_H   
            
// End of File

