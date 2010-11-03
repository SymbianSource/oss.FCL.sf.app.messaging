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
* Description: smilmediafactoryinterface  declaration
*
*/




#ifndef SMILMEDIAFACTORYINTERFACE_H
#define SMILMEDIAFACTORYINTERFACE_H

#include <e32std.h>


class MSmilMedia;
class MSmilMediaRenderer;

/**
 *  Factory interface for constructing media renderers.
 */
class MSmilMediaFactory
{
public:
	enum TMediaFactoryError
		{
		EOk,
		ENoRenderer,
		ENotFound,
		EConnectionError
		};

	enum TPrefetchStatus
		{
		EPrefetchNone,
		EPrefetchWaiting,
		EPrefeching,
		EPrefetchReady,
		EPrefetchNotFound,
		EPrefetchConnectionError
		};

	/**
	* Sets the base URL that is to be used for resolving relative URLs
	*/
	virtual void SetBaseUrlL(const TDesC& aBaseUrl) = 0;

	/**
	* Instructs the factory that a media component from the given URL will be
	* needed later. The factory should proceed to fetch at least enough of 
	* the media to be able to instantiate a correct renderer object for it later. 
	* Some factory implementation might start downloading and caching some media 
	* types already because of this call.
	*
	* This method should return fast. If there is need for slow downloads, they should be performed asynchronously.
	* This method is called when the engine sees a new URL, typically during parsing 
	* of SMIL file.
	*/
	virtual void RequestMediaL(const TDesC& aUrl, MSmilMedia* aMedia) = 0;

	/**
	* As above, except with a strong hint that the media should be prefetched 
	* and cached.
	*/
	virtual void PrefetchMediaL(const TDesC& aUrl) = 0;

	/**
	* Creates a renderer object for a given media URL. If the method fails, 
	* an error code is returned. Media may not be fully loaded when this 
	* method completes (in case of a stream it can't be), but at least enough 
	* must be known about it to construct a correct media specific renderer 
	* object. 
	*
	* This method is synchronous and might block for a while. The 
	* ownership of the returned renderer object is moved to whoever called 
	* this method.
	*
	* Return values:
	* EOk - renderer created succefully
	* ENoRenderer - there is no renderer object available for this media type
	* ENotFound - given URL was not found
	* EConnectionError - getting the URL failed, possibly by timeout
	*/
	virtual TMediaFactoryError CreateRendererL(const TDesC& aUrl, MSmilMedia* aMedia, MSmilMediaRenderer*& aRender) = 0;

	/**
	* Returns the current status of media prefetching of given URL. 
	* This includes amount downloaded so far (aDown) and total size 
	* (aSize, if known, 0 otherwise) in bytes.
	* 
	* Return values:
	* ENone - no prefetch has been asked
	* EWaiting - prefetch is waiting, for example to establish network connection
	* EPrefeching - prefetch is in progress
	* EReady - prefetch has completed
	* ENotFound - given URL was not found
	* EConnectionError - getting the URL failed, possibly by timeout
	*/
	virtual TPrefetchStatus PrefetchStatus(const TDesC& aUrl, TInt& aDown, TInt& aSize) = 0;

	/**
	 * Called by the engine to signal the factory that a renderer is being deleted.
	 * If the factory maintains any references to the renderer, they must be removed here.
	 * (DEPRECATED)
	 */
	virtual void RendererDeleted(MSmilMediaRenderer* aRenderer) = 0;

	/**
	* The parsing of presentation has finished and initial
	* layout has been calculated.
	*/
	virtual void PresentationReady() = 0;


	/**
	* The presentation has ended (though it might restart).
	* The factory may free some resources, cut connections, etc. 
	*/
	virtual void PresentationEnd() = 0;

	/**
	* Query for supported content type. Returns ETrue if
	* the given type (for example "application/xhtml+xml") is 
	* supported by this factory implementation.
	*
	* The method is used by the SMIL engine for resolving values
	* like "ContentType:application/xhtml+xml" of the 
	* systemComponent content control attribute.
	*/
	virtual TBool QueryContentType(const TDesC& aMimeType) const = 0;

};


#endif
