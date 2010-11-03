/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Observer class for voice functions
*
*/



#ifndef __AMSVOICEOBSERVER_H
#define __AMSVOICEOBSERVER_H
 

// CLASS DEFINITION

/**
 *  This class defines a generic observer interface.
 *  Updates timer for duration, tells when playing stops
 *
 *  @lib audiomessge.exe
 *  @since S60 v3.1
 */
class MAmsVoiceObserver 
	{
	public: // new methods

		/**
		 * Called to notify a change in the observed timer state.
		 */
		virtual void UpdateL( TTimeIntervalMicroSeconds aCurrentDuration, TInt aErrorCode ) = 0;

		/**
		 * Called to notify a change in the observed playing state.
		 */
		virtual void PlayingStops() = 0;
	
		
	};

#endif  // __AMSVOICEOBSERVER_H
