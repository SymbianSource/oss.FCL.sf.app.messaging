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
* Description:   Class that handles voice progressdialog operations.
*
*/



#ifndef AUDIOMESSAGEPROGRESSDIALOG_H
#define AUDIOMESSAGEPROGRESSDIALOG_H

#include <e32base.h>
#include <AknProgressDialog.h>
#include <notecontrol.h>
#include <remconcoreapitargetobserver.h>
#include <e32debug.h>
#include "amsvoiceobserver.h"
#include <AknNaviDecoratorObserver.h>

class CAudioMessageRecorder;
class MAmsVolumeObserver;
class CRemConInterfaceSelector;
class CRemConCoreApiTarget;
class CAknVolumePopup;
class CAudioMessageDocument;

/**
 *  Shows dialog with progressbar, states and time of duration. 
 *	Controls recording and playing via AudioMessageRecoder object
 *
 *  @lib audiomessage.exe
 *  @since S60 v3.1
 */
class CAudioMessageProgressDialog : public CAknProgressDialog, 
 									public MAmsVoiceObserver,	
 									public MRemConCoreApiTargetObserver,
                                    public MAknNaviDecoratorObserver
    {
    protected:
    	enum TAmsProgressFlags
    		{
    		EAmsWorkingModeRecorder = 0x1,
    		EAmsSetExit = 0x2,
    		EAmsWsEventStop = 0x4,
    		EAmsPause = 0x8,
    		EAmsSupportAudioOutput = 0x10,
    		EAmsAudioOutputEarpiece = 0x20
    		};
    public:  // Constructors and destructor
        /**
         * Constructor
         */
        CAudioMessageProgressDialog( CAudioMessageDocument& aDocument);

        /**
         * Destructor
         */
        virtual ~CAudioMessageProgressDialog();

	    /**
	     * Factory function to create this object
	     *
	     * @param aFile filehandle for voice 
	     * @param aVolume volume level
	     * @param aSpeakerEarpiece speaker ETrue earpiece
	     * @param aRecorder, record or play working mode
	     */
       	static CAudioMessageProgressDialog* NewL( const RFile& aFile, TInt aVolume,
       		 TBool aSpeakerEarpiece, TBool aRecorder, CAudioMessageDocument& aDocument);
	
        /**
         * set observer for navipane use
         */
		void SetNaviObserver( MAmsVolumeObserver* aObserver );

        /**
         * set mode of progressdialog
         * @param ETrue recoding, EFalse playing
         */
	//	void SetMode( TBool aMode );

        /**
         * set max message size
         * @param sSize
         */
		void SetMaxMessageSize( TUint aSize );

        /**
         * stop recording/playing
         */
        void Stop();

        /**
         * Starts remove timer. Cancels running remove timer.
         * If SVK event occurs, the timer is cancelled and restarted.
         */
  		void StartRemoveTimerL( );

        /**
         * Cancels remove timer. 
         * If SVK event occurs, the timer is restarted.
         */
        void CancelRemoveTimer( );
        

        /**
         * From MAknNaviDecoratorObserver
         * Handles the navi decorator events.
         * These are created by pressing the arrows related on volumebar.
         */
        void HandleNaviDecoratorEventL( TInt aEventID );

        /**
         * From MCoeControlObserver
         * Handles the control events coming from the volumebar.
         * These events do not include the arrow-presses.
         */
        void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

        /**
         * Sets the observer for the volumecontrol
         */
        void SetVolCtrlObserver( MCoeControlObserver& aObserver );


    protected: // New functions
        /**
         * Constructor.
         */
		void ConstructL( const RFile& aFile, TInt aVolume ,
		TBool aSpeakerEarpiece, TBool aRecorder);
		
	protected:	// from MAmsRecObserver
		/**
		* Called to notify a change in the observed subject's state.
		*/
     	void PlayingStops();
 
     	void UpdateL( TTimeIntervalMicroSeconds aCurrentDuration, TInt aErrorCode );
	private:
        /**
        * Calls respective observer function
        */
        void DoChangeVolumeL( );

        /**
        * ChangeVolume is the callback function called from change volume timer.
        */
        static TInt ChangeVolume(TAny* aThis);

        /**
         * From CAknProgressDialog
         * keyevents
         */        
		TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    //from MRemConCoreApiTargertObserver
    protected: 	
        /**
         * @see MRemConCoreApiTargetObserver.
         *
         * A command has been received. 
         * @param aOperationId The operation ID of the command.
         * @param aButtonAct The button action associated with the command.
         */
		void MrccatoCommand( TRemConCoreApiOperationId   aOperationId, 
                                    TRemConCoreApiButtonAction  aButtonAct );
    // from CAknProgressDialog                                
    protected:  
        
        /**
        * From CAknProgressDialog
        * @see CAknProgressDialog for more information
        */
        void PreLayoutDynInitL();

        /**
        * From CAknProgressDialog
        * @see CAknProgressDialog for more information
        */
        void PostLayoutDynInitL();

        /**
        * From CAknProgressDialog
        * @see CAknProgressDialog for more information
        */
        TBool OkToExitL(TInt aButtonId);

    private:  // Functions from base classes

        /**
         * Called by Update 
         * updates the progress bar texts
         * @return void
         * @param text and interval
         */
       void UpdateProgressTextL( TInt aText, 
        			TTimeIntervalMicroSeconds aMicroSec );
        /**
         * Called by Update 
         * updates the progress bar
         * @return void
         * @param currrent value of timer
         */
	   	void DoTickL(TTimeIntervalMicroSeconds aCurrValue);

        /**
         * pause recording
         */
       	void PauseL();

        /**
         * continue recording.
         */
       	void ContinueL();

        /**
         * set softkeys dynamically
         * @param resouce
         */
       	void SetSoftkeys( TInt aSk );
       	
        /**
         * set icon for progressdialog
         */
       	void SetProgressIconL( TInt aIconId, TInt aIconMask );
       	
        /**
         * change outputouting
         */
       	void ToLoudspeaker();

        /**
         * change outputouting
         */
       	void ToEarpiece();
        
        /**
         * sets navipanel volume indicator via appui
         */
       	void SetNaviVolL();

        /**
         * show note when max rec time appears
         */
       	void ShowInformationNoteL( TInt aResourceID, TBool aWaiting );

   private: //data
       	CAudioMessageDocument& iDocument;
       	/** 
       	 * Observer to notify about a event. 
       	 * Not owned. 
       	 **/
		MAmsVolumeObserver* iNaviVolObserver;
       	CEikProgressInfo* iProgressInfo;

       	/** 
       	 * recorder. 
       	 * Owned. 
       	 **/
        CAudioMessageRecorder* iRecorder;

 		TTimeIntervalMicroSeconds iPausePosMicroSec;
		
		/** 
		 * max rec/play time.
		 */
 		TTimeIntervalMicroSeconds iMaxDurationMicroSec;

		/** 
		 * max size, sets by appui
		 */
        TUint iMaxMessageSize;
       	
       	/** 
       	 * for storing current volume level
       	 **/
		TInt iCurrentVolume;
        TInt iTickCount;
		
		CAknVolumePopup* iVolumePopup;
		
		/** 
		 * max rec/play value.
		 */
        TInt iFinalValue;
    	TInt iIncrement;
		/** 
		 * Remote Controller. 
		 * Owned.
		 */
	    CRemConInterfaceSelector* iInterfaceSelector;

		/** 
		 * Remote Controller. 
		 * Not owned.
		 */
	    CRemConCoreApiTarget* iCoreTarget;
	    CPeriodic* iVolumeTimer;

       	/** 
       	 * for volume up/down use 
       	 **/
        TInt iChange;
	    
       	/** 
       	 * for mode, exits, outputrouting, feature 
       	 **/
	    TInt iFlags;
      	HBufC* iTimeDuratBase;
      	HBufC* iTextRecording;
      	HBufC* iTextPlaying;
      	HBufC* iTextPaused;
      	
      	//Following are made to reduce allocation in every 1 second when
      	//the progressbar is running
      	HBufC* iLengthMax;
      	HBufC* iLengthNow;
      	
#ifdef RD_SCALABLE_UI_V2      	
      	MCoeControlObserver* iVolCtrlObserver;
#endif      	


   };

#endif  // AudioMessagePROGRESSDIALOG_H

// End of File
