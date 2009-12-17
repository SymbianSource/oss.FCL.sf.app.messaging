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
* Description:   This class acts as a high level wrapper around the media server
*                playback and recording functionalities.
*
*/




#ifndef AUDIOMESSAGERECORDER_H
#define AUDIOMESSAGERECORDER_H

#include <e32base.h>
#include <mdaaudiosampleeditor.h>

class MAmsVoiceObserver;
class CAudioOutput;
class CAudioInput;
class CMdaAudioRecorderUtility;
class CAudioMessageBeep;

const TInt KAmsDisplayUpdateDelay = 200000;

/**
 *	Class for control recording and playing operations
 *
 *  @lib audiomessage.exe
 *  @since S60 v3.1
 */
class CAudioMessageRecorder 
	: public CBase, public MMdaObjectStateChangeObserver
	{
	public:
		/**
		 * Default constructor
		 */
		CAudioMessageRecorder(); 

		/**
		 * Destructor
		 */
		~CAudioMessageRecorder();

	
		static CAudioMessageRecorder* NewL(const RFile& aFile, const TBool aAudioOutput );

    private:

   		void ConstructL(const RFile& aFile, const TBool aAudioOutput  );

	public:
		/**
		 * Encapsulates the interface for handling the change of state of
		 * an audio data sample object.
		 * @param aObject A pointer to the audio sample object that has
		 *				 changed state.
		 * @param aPreviousState The state before the change
		 * @param aCurrentState The state after the change
		 * @param aErrorCode If not KErrNone, then the error that
		 *					caused the state change
		 */
	    void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, 
	    	TInt aCurrentState, TInt aErrorCode);
	
		/**
		 * Registers an observer for receiving update notifications.
		 * @param aObserver The observer object.
		 */
		void SetObserver( MAmsVoiceObserver* aObserver );
	
		/**
		 * Starts recording.
		 */
		void StartRecordingL();

		/**
		 * Starts playback.
		 */
		void StartPlayingL();
		
		/**
		 * Stops playback or recording.
		 */
		void StopL(); 
		
		/**
		 * Pauses recording.
		 */
		void Pause(); 

		/**
		 * Continues recording.
		 */
		void ContinueL();
		
		/**
		 * Position for pause use
		 * @return position in ms
		 */
	    TTimeIntervalMicroSeconds GetPosition();

		/**
		 * Duration for playing use
		 * @return position in ms
		 */
	    TTimeIntervalMicroSeconds GetDuration();

		/**
		 * bitrate for size calc.
		 */
		TUint GetBitRateL();

		/**
		 * Stops and deletes the timer object.
		 */
		void StopTimer();

		/**
		 * increace vol.
		 */
		void IncrementVol();

		/**
		 * dec vol.
		 */
		void DecrementVol();

		/**
		 * Gets volume
		 * @return currentvolume
		 */
		TInt GetCurrentVolume();

		/**
		 * Sets volume
		 * @param aVal wanted volume
		 */
		void SetVol(TInt aVal);

		/**
		 * Sets audiorouting 
		 * @param EFalse earpiece,ETrue loudspkr
		 */
		TInt SetAudioOutput(TBool aOutputPublic);

		/**
		 * Initialize routing
		 */
		void InitializeAudioRoutingL();


	private:

		/**
		* Creates and starts a CPeriodic timer object
		* with a resolution of one second.
		*/
		void StartTimer();
		static TInt TimerCallBack( TAny* aMdaRecorder );
		void HandleTick();
		void ResetAudioInputL();

	private: //data
		/**
		 * Pointer to the media server audio recorder object. 
		 * Owned.
		 */
		CMdaAudioRecorderUtility* iRecUtility;

		/**
		 * CActiveScheduler wrapper object.
		 * Owned.
		 */
		CActiveSchedulerWait iActiveWait;
		
		/**
		 * Pointer to the timer object. 
		 * Owned.
		 */
		CPeriodic* iTimer;
		
		/**
		 * Pointer to the status observer. 
		 * Not owned.
		 */
		MAmsVoiceObserver* iObserver;

		/**
		 * Pointer to the audiorouting object. 
		 * Owned.
		 */
	 	CAudioOutput* iAudioOutput;
	 	CAudioInput* iAudioInput;
	 	
		/**
		 * For storing current volume
		 */
	    TInt iCurrentVolume;   

		/**
		 * True if outputrouting is supported
		 */
	    TBool iSupportAudioOutput;

		/**
		* For playing the recording start and stop tones. 
		* Owned.
		*/
	    CAudioMessageBeep* iBeepPlayer;
	    
		/**
		 * ETrue if pause
		 */
		TBool iPause;
      	TInt iErrorCode; 
	};

#endif // AUDIOMESSAGERECORDER_H

