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
* Description:   Class that handles insert operations
*
*/




#ifndef __AUDIOMESSAGEINSERTOPERATION_H
#define __AUDIOMESSAGEINSERTOPERATION_H

#include <e32base.h>
#include <msvapi.h>

#include <MsgEditorView.h>
#include "audiomessageoperation.h"

class CMsgMediaInfo;
class CAudioMessageRecorder;

// ========== CLASS DECLARATION ============================

/**
 * insert class
 *
 * @lib AudioMessage.exe
 * @since S60 v3.1
 */
class CAudioMessageInsertOperation : public CAudioMessageOperation
    {
    protected:
	    enum TAmsInsertState
		    {
		    EAmsInit,
		    EAmsCheckAttas,
		    EAmsSetAudio,
		    EAmsSetDuration,
		    EAmsInsertSmil,
		    EAmsCommit,
		    EAmsRemoveAudio,
		    EAmsRemoveSmil,
		    EAmsInsertReady
		    };

    public: // new operations

        /**
       	 * Factory method that creates this object.
         * @param aOperationObserver
         * @param aDocument
         * @param aView
         */
       	static CAudioMessageInsertOperation* NewL( 
            MAmsOperationObserver& aOperationObserver,
            CAudioMessageDocument& aDocument,
            CMsgEditorView& aView );

        /**
         * Initializes the iState,then completes
         * @param aInfo Media type to be checked
         * @param aVal, delete/insert
         */        
        void Insert( CMsgMediaInfo* aInfo, TBool aVal, TBool aNew = ETrue );

        /**
         * Destructor
         */
       	~CAudioMessageInsertOperation();
       
        /**
        * Removes old audio.
        */
		void DoRemoveAudioL( );
        
        /**
        * Removes old audio.
        */
		void DoRemoveSmilL();
		
	// From CActive	
    protected:
        void RunL();
        void DoCancel();

    private: // new operations
        /**
         * Constructor.
         */
        CAudioMessageInsertOperation( MAmsOperationObserver& aOperationObserver,
        	CAudioMessageDocument& aDocument,
        	CMsgEditorView& aView );

        
        /**
         * Called in RunL to do the actual state transition 
         */
        void  DoInsertStepL();
        
        /**
         * Removes possible old audio.
         * Starts creating new attachment
         */
   		void DoCreateNewAudioAttaL();

       	/**
         * size limit check.
         * @return ETrue if size is ok
         */
        TBool CheckSizeForInsert();
        
        /*
        * Set the duration of the clip to the SMIL
        */
        void DoSetDurationL( );

    private: // data

        CMsgEditorView& iView;
        
        /**
         * media info
         * Own.  
		 */
        CMsgMediaInfo*  iInfo;

        /**
         * current insertion/deletion state
		 */
        TInt iState;
        
        /**
         * Pointer to the created file
         * Not own.  
		 */
        RFile* iEditFile;  
        
        //TFileName iFileName;
        //TFileName iContentLocationFileName;
        
        /**
         * remove bit
		 */
        TBool iRemove;
        TBool iNew;
    };

#endif // __AUDIOMESSAGEINSERTOPERATION_H