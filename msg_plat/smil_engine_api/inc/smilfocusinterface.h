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
* Description: smilfocusinterface  declaration
*
*/




#ifndef SMILMEDIAFOCUSINTERFACE_H
#define SMILMEDIAFOCUSINTERFACE_H

#include <e32std.h>

/**
 * Interface representing user focusable area on the screen.
 */

class MSmilFocus
	{
public:
	
	enum TShape
		{
		ERect,
		ECircle,
		EPoly,
		ENone
		};

public:
	/**
	* Shape of the focused area
	*/
	virtual TShape Shape() const = 0;

	/**
	* Bounding box of the focused area if Shape()==ERect or ECircle
	*/
	virtual TRect Rect() const = 0;

	/**
	* Corners of the polygon (non-zero only if Shape()==EPoly);
	*/
	virtual const CArrayFixFlat<TPoint>* PolyPoints() const = 0;

	/**
	* True if this area is focused AND selected for keypad focus
	*/
	virtual TBool IsSelected() const = 0;

	};

#endif
