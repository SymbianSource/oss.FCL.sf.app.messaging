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
* Description: smilactiveinterface  declaration
*
*/



#ifndef SMILACTIVE_H
#define SMILACTIVE_H

//  INCLUDES
#include <e32base.h>

#include <smiltime.h>

// FORWARD DECLARATIONS
class CSmilObject;
class CSmilPresentation;

// CLASS DECLARATION
/**
 * This interface provides SMIL engine generated timing for components
 * outside SMIL engine. This allows accurate synchronization of external 
 * components to the SMIL engine clock. For example, this interface may
 * be used in media renderer to implement animation, for example 
 * animated GIFs. The interface is used inside the SMIL engine to 
 * provide timing for transition effects.
 * 
 * To use this interface, the using component inherits it and implements
 * ActivateL() method. By using MSmilMedia::AfterL() method the
 * component can request a delay of specific duration. ActivateL() is
 *  called when this duration has passed in SMIL presentation timeline.
 * In real time, this might take longer than the requested time, for example 
 * if the presentation is paused.
  */
class MSmilActive 
    {
protected:  

	/**
	* Called when the requested time has been reached. The current time, relative to the requesting media obect, is passed as parameter.
	*/
	virtual void ActivateL(const TSmilTime& aTime) = 0;

public:  

	CSmilObject* iSmilObject;
	friend class CSmilPresentation;
	};

#endif      // ?INCLUDE_H   
            
// End of File
