/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgEditorObserver  declaration
*
*/



#ifndef MMSGEDITOROBSERVER_H
#define MMSGEDITOROBSERVER_H

// ========== INCLUDE FILES ================================

#include <eikedwin.h>  // for TClipboardFunc

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgBaseControl;
class TCoeHelpContext;

// ========== CLASS DECLARATION ============================

/**
*
*
*/
class MMsgEditorObserver
    {
    public:

        enum TMsgEditorObserverFunc
            {
            // arg1 = CEikEdwin::TClipboardFunc
            // arg2 = TBool ETrue = deny, EFalse = don't deny
            // arg3 not used.
            EMsgDenyClipboardOperation,

            // arg1 = TMsgFocusEvent
            // arg2 = TMsgAfterFocusEventFunc
            // arg3 = TInt (IN/OUT) - Specifies the scroll part that is wanted to be changed.
            //                        Client can determine whether this is possible and return
            //                        really changed scroll part via this parameter.
            EMsgHandleFocusChange,

            // arg1 = TInt parts
            // arg2 not used
            // arg3 not used
            EMsgScrollParts,

            // arg1 = TDes size
            // arg2 = TInt bitmask of indicators to set on (TMsgIndicatorFlags in MsgEditor.hrh)
            // arg3 not used
            EMsgGetNaviIndicators,
            
            // arg1 = CMsgBaseControl (IN) - destination control. NULL if the event occurred
            //                               outside of any control.
            // arg2 = TPointerEvent (IN) - pointer event.
            // arg3 = TBool (OUT) - specifies whether pointer event was handled or not. Default value
            //                      is that it was not handled. If pointer event is set to be handled
            //                      focus is not set to destination control and pointer event is not 
            //                      forwarded to destination control.
            EMsgControlPointerEvent,
            
            // arg1 = CMsgBaseControl (IN) - destination control. Will not be NULL.
            // arg2 not used currently
            // arg3 not used currently
            EMsgButtonEvent
            };

        enum TMsgFocusEvent
            {
            EMsgFocusNone = -1,
            EMsgFocusToBody,
            EMsgFocusToHeader,
            EMsgFocusAtBottom,
            EMsgFocusAtTop,
            EMsgFocusMovingFrom,
            EMsgFocusMovedTo
            };

        enum TMsgAfterFocusEventFunc
            {
            EMsgAfterFocusNone = -1,
            EMsgCursorToBodyBeginning,
            EMsgCursorToBodyEnd
            };

    public:

        /**
        * Create custom control.
        * @param
        * @return
        */
        virtual CMsgBaseControl* CreateCustomControlL(TInt aControlType) = 0;

        /**
        *
        *
        */
        virtual void EditorObserver(TMsgEditorObserverFunc aFunc, TAny* aArg1, TAny* aArg2, TAny* aArg3) = 0;
    };


#endif // MMSGEDITOROBSERVER_H

// End of File
