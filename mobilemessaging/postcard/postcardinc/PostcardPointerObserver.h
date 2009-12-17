/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*   Postcard pointer (pen) event observer class
*
*/



#ifndef __POSTCARDPOINTEROBSERVER_H__
#define __POSTCARDPOINTEROBSERVER_H__

#ifdef RD_SCALABLE_UI_V2

// DATA TYPES

enum TPocaPointerEvent
    {
    // Note: code assumes these are in this order
    EPocaPointerEventUpIcon,    // backside flip icon
    EPocaPointerEventDownIcon,  // frontside flip icon
    EPocaPointerEventGreeting,  // backside greeting
    EPocaPointerEventAddress,   // backside address
    EPocaPointerEventFrontBg    // front background
    };

enum TPocaPointerEventType
    {
    // These are bits
    EPocaPointerEventTypeNone = 0,
    EPocaPointerEventTypeSingleDown = 1,
    EPocaPointerEventTypeSingleUp = 2
    };

// Pointer event observer
class MPocaPointerEventObserver
    {
public: // functions
    virtual void PocaPointerEventL( TPocaPointerEvent aEvent,
        TPocaPointerEventType aEventType ) = 0;
    };

#endif // RD_SCALABLE_UI_V2

#endif // __POSTCARDPOINTEROBSERVER_H__
