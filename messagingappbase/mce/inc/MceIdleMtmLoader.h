/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*     Active object to load mtm's background after mce start.
*
*/



#ifndef __MCEIDLEMTMLOADER_H__
#define __MCEIDLEMTMLOADER_H__

//  INCLUDES
#include <e32base.h>
#include <MsgArrays.h>
#include "MsgFunctionInfo.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CMceIdleMtmLoader
*
*
*/
class CMceIdleMtmLoader : public CActive
    {
    public:  // Constructors and destructor

        /**
        * C++ constructor.
        */
        CMceIdleMtmLoader(
            CMtmStore&      aMtmStore,
            CUidNameArray&  aMsgTypesWritePopup,
            CUidNameArray&  aMsgTypesWriteSubmenu,
            CUidNameArray&  aMsgTypesSettings,
            CMsgFuncArray&  aMTMFunctionsArray,
            CMtmUiDataRegistry& aUiRegistry );

        /**
        * Destructor.
        */
        virtual ~CMceIdleMtmLoader();

    public: // New functions

        /**
        * Starts loading of MTMs
        * @param aSession: session to message server
        * @param aAudioMsgEnabled: to check whether audio message is enabled or not
        * @param aPostcardEnabled: to check whether audio message is enabled or not
        */
        void StartL(CMsvSession& aSession, TBool aAudioMsgEnabled, TBool aPostcardEnabled); // CR : 401-1806

        /**
        * Complete synchronously. Loads rest of the MTMs immediately.
        * @param aAudioMsgEnabled: to check whether audio message is enabled or not
        * @param aPostcardEnabled: to check whether audio message is enabled or not
        */
        void FinishL(TBool aAudioMsgEnabled, TBool aPostcardEnabled); // CR : 401-1806


    private:  // Functions from CActive
        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void RunL();
        /** 
        * From CActive
        */
        TInt RunError(TInt /*aError*/);

    private: // New functions

        /**
        * Requests next mtm to be loaded.
        */
        void QueueLoad();

        /**
        * Load one mtm
        * @param aMtmType: mtm to load.
        */
        void LoadMtmL( TUid aMtmType );

        /**
        * Changes array order: sms, mms, email, others...
        */
        void SortAndChangeSmsMmsEmailFirstL();

        /**
        * Places given mtm first in the array.
        * @param aArray: reference to array
        * @param aMsgType: mtm to changed to the first.
        */
        void ChangeMsgTypeTopL( CUidNameArray& aArray, TUid aMsgType ) const;

    private: //Data
        CMtmDllRegistry& iUiRegistry;
        CMtmStore&      iMtmStore;
        CUidNameArray&  iMsgTypesWritePopup;
        CUidNameArray&  iMsgTypesWriteSubmenu;
        CUidNameArray&  iMsgTypesSettings;
        CMsgFuncArray&  iMTMFunctionsArray;
        TInt            iRegMtmIndex;
        TBool           iAudioMsgEnabled ;  // CR : 401-1806
        TBool           iPostcardEnabled ;  // CR : 401-1806
    };

#endif      // __MCEIDLEMTMLOADER_H__

// End of File
