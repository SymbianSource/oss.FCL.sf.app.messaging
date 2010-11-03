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
* Description:   Class that handles launching operations
*
*/



#ifndef __AUDIOMESSAGELAUNCHOPERATION_H
#define __AUDIOMESSAGELAUNCHOPERATION_H

#include "audiomessageoperation.h"
#include "audiomessagedocument.h"

class CMsgImageControl;
class CAudioMessageRecorder;

/**
 * class for application launching 
 *
 * @lib audiomessage.exe
 * @since S60 v3.1
 */
class CAudioMessageLaunchOperation : public CAudioMessageOperation
    {
   	protected: 
        enum TAmsLaunchState
            {
            EAmsLaunchInitialize = 0,
			EAmsCheckAttach,
			EAmsHandleAudio,
            EAmsLaunchPrepareContent,
            EAmsLaunchAddNeededSmil,
            EAmsLaunchUnInitialize,
            EAmsLaunchPrepareBody,
            EAmsLaunchEnd,
            EAmsLaunchErrorReport
            };

    public:  // New methods
        /**
         * Sets icon in body.
         * @param aType 	EAudioInsert/EAudioInserted	
         */
      	void DoPrepareBodyL( TAmsRecordIconStatus aType );

        /**
        * Factory method that creates this object.
        *
        * @param    aControlObserver for imagecontrol use
        * @param    aOperationObserver for appui use
        * @param    aDocument
        * @param    aView
        * @param    aAppUi
        * @return   Pointer to instance in cleanup stack
        */
        static CAudioMessageLaunchOperation* NewL(
            MAmsOperationObserver& aOperationObserver,
            CAudioMessageDocument& aDocument,
            CMsgEditorView& aView );

        /**
         * Destructor
         */
        virtual ~CAudioMessageLaunchOperation();

        /**
         * Initializes the iLaunchState,then completes
		 */
        void Launch();

        /**
        * Sets the mimetypes and content location.
        * Stores headers, checks is smil exists
        */
        void DoPrepareContentL();

        /**
        * Gets the framesize
        */
        TSize FrameSize();
	
	// From CActive
    protected: 
        void DoCancel();
        void RunL();

    private: 

        /**
        * Constructor.
        */
        CAudioMessageLaunchOperation(
            MAmsOperationObserver& aOperationObserver,
            CAudioMessageDocument& aDocument,
            CMsgEditorView& aView );

        /**
         * Called by RunL.
         * Calls different functions according to iLaunchStep
         */
        void DoLaunchStepL();

        /**
         * Calls DoPrepareBodyL with argument
         */
        void DoPrepareBodyL();

        /**
         * initialize, sets store and makes some checks
         */
        void DoInitializeL();

        /**
         * checks is smil in message
         * @return ETrue there is. 
         */
        TBool SmilExists();

        /**
         * sets smil flag
         * @param aSel ETrue/EFalse. 
         */
        void SetSmilExists(TBool aSel);
        void DoHandleAudioL( );
      
protected:


        CMsgEditorView& iView;

        /**
         * current insertion/deletion state
		 */
        TInt iLaunchState;
        
        /**
         * smilexist bit
		 */
        TBool iSmilExist;

        /**
         * control for main audio clip icon 
         * Own.  
		 */
        CMsgImageControl* iImageControl;
        
        //TFileName iContentLocationFileName;

    };

#endif // __AUDIOMESSEAGELAUNCHOPERATION_H


