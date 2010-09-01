/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgAsyncControlObserver  declaration
*
*/



#ifndef MMSGASYNCCONTROLOBSERVER_H
#define MMSGASYNCCONTROLOBSERVER_H

// ========== INCLUDE FILES ================================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// Order is important. Do not change unless you know what you do.
enum TMsgAsyncControlState
    {
    EMsgAsyncControlStateIdle = 0,
    EMsgAsyncControlStateOpening,
    EMsgAsyncControlStateBuffering,
    EMsgAsyncControlStateReady,
    EMsgAsyncControlStateAboutToPlay,
    EMsgAsyncControlStatePlaying,
    EMsgAsyncControlStatePaused,
    EMsgAsyncControlStateStopped,
    EMsgAsyncControlStateNoRights,
    EMsgAsyncControlStateCorrupt,       // cannot recover
    EMsgAsyncControlStateError
    };


// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================
class CMsgBaseControl;

// ========== CLASS DECLARATION ============================

/**
* MMsgAsyncControlObserver
*/
class MMsgAsyncControlObserver
    {
    public: // new methods

        /**
        * Call back function for reporting asynchronous control state machine state has been changed
        * to observer.
        *
        * @param aControl - Changed asynchrnous control.
        * @param aNewState - New state of the control.
        * @param aOldState - Old state of the control.
        */
        virtual void MsgAsyncControlStateChanged( CMsgBaseControl& aControl,
                                                  TMsgAsyncControlState aNewState,
                                                  TMsgAsyncControlState aOldState ) = 0;
        
        /**
        * Call back function for reporting asynchronous control resource changes to observer.
        *
        * @param aControl - Changed MediaControl.
        */                                          
        virtual void MsgAsyncControlResourceChanged( CMsgBaseControl& aControl, TInt aType ) = 0;
    };

#endif // MMSGASYNCCONTROLOBSERVER_H

// End of File
