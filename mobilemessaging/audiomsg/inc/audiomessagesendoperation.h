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
* Description:   Class that handles sending operations
*
*/



#ifndef __AUDIOMESSAGESENDOPERATION_H
#define __AUDIOMESSAGESENDOPERATION_H

#include "audiomessageoperation.h"
#include "audiomessagedocument.h"
#include "audiomessagesaveoperation.h"
class CUniAddressHandler;


/**
 * class for sending 
 *
 * @lib audiomessage.exe
 * @since S60 v3.1
 */
class CAudioMessageSendOperation : public CAudioMessageOperation,
                                   public MAmsOperationObserver
    {
    protected: // data

        enum TAmsSendState
            {
            EAmsSendRemoveDuplicateAddresses,
            EAmsSendSave,
            EAmsSendMtmSend,
            EAmsSendEnd
            };

    public:  // New methods

        /**
         * Factory method that creates this object.
         *
         * @param    aAddressCtrl
         * @param    aObserver
         * @param    aDocument
         * @param    aView
         * @param    aFs
         * @return   Pointer to instance in cleanup stack
         */
        static CAudioMessageSendOperation* NewL(
        	CMsgAddressControl& aAddressCtrl,
            CUniAddressHandler& aAddressHandler,
            MAmsOperationObserver& aObserver,
            CAudioMessageDocument& aDocument,
            CMsgEditorView& aView );

        /**
         * Destructor
         */
        virtual ~CAudioMessageSendOperation();

        /**
         * Initializes the iSendState, then completes
		 */
        void Send();

    protected: // From CActive
       	void RunL();
        void DoCancel();

	// From MAmsOperationObserver
    protected: 
        void EditorOperationEvent(
            TAmsOperationType aOperation,
            TAmsOperationEvent aEvent );

    private:

        /**
         * Constructor.
         */
        CAudioMessageSendOperation(
        	CMsgAddressControl& aAddressCtrl,
            CUniAddressHandler& aAddressHandler,
            MAmsOperationObserver& aObserver,
            CAudioMessageDocument& aDocument,
            CMsgEditorView& aView );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
         * Called by RunL.
         * Calls different functions according to iSendStep
         */
        void DoSendStepL();

        /**
         * Removes dublicates off
         */
        void DoRemoveDuplicateAddressesL();

        /**
         * Saves
         */
        void DoSave();

        /**
         * Sends
         */
        void DoSendL();

    protected: // data

        CMsgAddressControl& iAddressCtrl;
        CUniAddressHandler& iAddressHandler;

        CMsgEditorView& iView;
        /**
         * pointer to sendoperation 
         * Own.  
		 */
        CAudioMessageSaveOperation* iSaveOperation;

        /**
         * pointer to sendoperation 
         * Own.  
		 */
        CMsvOperation* iMtmSendOperation;

        TAmsSendState iSendState;

    };

#endif // __AUDIOMESSAGESENDOPERATION_H



