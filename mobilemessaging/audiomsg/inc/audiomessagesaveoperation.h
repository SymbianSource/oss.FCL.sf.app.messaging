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
* Description:   Class that handles saving operations
*
*/





#ifndef __AUDIOMESSAGESAVEOPERATION_H
#define __AUDIOMESSAGESAVEOPERATION_H

#include <e32base.h>
#include <e32std.h>
#include <mmsmsventry.h>

#include "audiomessageoperation.h"
#include "audiomessagedocument.h"


/**
 * class for saving 
 *
 * @lib audiomessage.exe
 * @since S60 v3.1
 */
class CAudioMessageSaveOperation : public CAudioMessageOperation
    {
    protected:
       	enum TAmsSaveState
           	{
            EAmsSaveCheck,
            EAmsSaveFinalize,
            EAmsSaveEnd
            };

    public:  // New methods

        /**
         * Factory method that creates this object.
         *
         * @param    aAddressCtrl
         * @param    aObserver for appui use
         * @param    aDocument
         * @param    aView
         * @return   Pointer to instance in cleanup stack
         */
        static CAudioMessageSaveOperation* NewL(
        	CMsgAddressControl& aAddressCtrl,
            MAmsOperationObserver& aObserver,
            CAudioMessageDocument& aDocument,
            CMsgEditorView& aView );

        /**
         * Destructor
         */
        virtual ~CAudioMessageSaveOperation();

        /**
         * Initializes the iSaveState, then completes
		 */
        void Save();
        
    // From CActive    
    protected: 
        void DoCancel();
        void RunL();
  
    private:
        /**
         * Constructor.
         */
        CAudioMessageSaveOperation(
            CMsgAddressControl& aAddressCtrl,
            MAmsOperationObserver& aObserver,
            CAudioMessageDocument& aDocument,
            CMsgEditorView& aView );


        /**
         * Called by RunL.
         * Calls different functions according to iSaveStep
         */
        void DoSaveStepL();

        /**
         * Inserts addresses from address control to clientMtm
         */
        void DoSaveChecksL();

        /**
        * finalize, some checks for recipients
        */
        void DoFinalizeSaveL();

        /**
        * ETrue if ok
        */
        TBool CheckDiskSpaceL();

       	/**
        * Removes characters from string that confuses address 
        * control or clientMtm. Note that function does not make diffrence 
        * whether string is phone number or email address.
        *
        * @param aString
        * @return ETrue, chars were removed, EFalse otherwise 
        */
		TBool RemoveIllegalAddrChars( TDes& aString );

        /**
        * Check that character is not something that confuses address
        * control or clientMtm.
        *
        * @param aChar
        * @return ETrue, char is legal, EFalse otherwise 
        */
		TBool IsValidAddrChar( const TChar& aChar );

    protected: // data
 
        CMsgEditorView& iView;
        CMsgAddressControl& iAddressCtrl;
        TAmsSaveState iSaveState;

    };


#endif // __AUDIOMESSAGESAVEOPERATION_H
