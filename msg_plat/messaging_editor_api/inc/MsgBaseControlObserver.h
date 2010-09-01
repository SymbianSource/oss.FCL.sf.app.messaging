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
* Description:  MsgBaseControlObserver  declaration
*
*/



#ifndef INC_MSGBASECONTROLOBSERVER_H
#define INC_MSGBASECONTROLOBSERVER_H

// ========== INCLUDE FILES ================================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

enum TMsgControlEventRequest
    {
    EMsgEnsureCorrectFormPosition,
    EMsgRotateFocusUp,
    EMsgRotateFocusDown,
    EMsgHeightChanged,
    EMsgScrollForm,
    EMsgCheckIfFocusIsAboveThis,
    EMsgDenyCut,
    EMsgDenyCopy,
    EMsgDenyPaste,
    EMsgUpdateScrollbar,
    EMsgStateFlags,
    EMsgGetNaviIndicators,
    EMsgButtonPressed
    };    

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgBaseControl;

// ========== CLASS DECLARATION ============================

/**
*
*/
class MMsgBaseControlObserver
    {

    public:
        
        /**
        * Returns whether view is initialized.
        */
        virtual TBool ViewInitialized() const = 0;
        
        /**
        * Returns the current view rectangle.
        * @return
        */
        virtual TRect ViewRect() const = 0;
        
        /**
        * Handles an event from a control.
        * @param aControl
        * @param aRequest
        * @param aDelta
        * @return
        */
        virtual TBool HandleBaseControlEventRequestL(
            CMsgBaseControl*        aControl,
            TMsgControlEventRequest aRequest,
            TInt                    aDelta) = 0;

        /**
        * Handles an event from a control.
        * @param aControl
        * @param aRequest
        * @return
        */
        virtual TBool HandleBaseControlEventRequestL(
            CMsgBaseControl*        aControl,
            TMsgControlEventRequest aRequest) = 0;

        /**
        * Handles an event from a control.
        * @param aControl
        * @param aRequest
        * @param aArg1
        * @param aArg2
        * @param aArg3
        * @return
        */
        virtual TInt HandleEditObserverEventRequestL(
            const CCoeControl*      aControl,
            TMsgControlEventRequest aRequest,
            TAny*                   aArg1 = 0,
            TAny*                   aArg2 = 0,
            TAny*                   aArg3 = 0
            ) = 0;
    };

#endif

// End of File
