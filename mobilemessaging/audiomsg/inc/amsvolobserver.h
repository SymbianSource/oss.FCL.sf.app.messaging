/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Observer class for speaker functions
*
*/



#ifndef __AMSVOLOBSERVER_H
#define __AMSVOLOBSERVER_H
 

// CLASS DEFINITION

/**
 *  This class defines a generic observer interface.
 *  Indicates navipane volume and speaker selection.
 *
 *  @lib audiomessge.exe
 *  @since S60 v3.1
 */
class MAmsVolumeObserver
	{
	public: // new methods

		/**
		* Called to notify a change in the observed volume state.
		*/
		virtual void UpdateNaviVol( TInt aCurrentVolumeValue ) = 0;

        virtual TInt GetVolume()= 0;
        
		/**
		* Called to notify a change in the observed speaker state.
		*/
		virtual void SetSpeakerOutput( TBool aEarpiece ) = 0;
	
	};


#endif  // __AMSVOLOBSERVER_H

