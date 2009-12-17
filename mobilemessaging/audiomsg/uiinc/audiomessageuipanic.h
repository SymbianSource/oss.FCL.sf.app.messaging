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
* Description:   UI panic part of the Ams MTM type to Symbian OS Messaging
*
*/




#ifndef __AUDIOMESSAGEUIUIPANIC_H
#define __AUDIOMESSAGEUIUIPANIC_H

#if !defined(__E32BASE_H__)
#include <e32base.h>      
#endif

enum TAMSUiPanic
    {
    EAMSNotAMessage,
    EAMSNotAService,
    EAMSWrongMtmType,
    EAMSWrongParameters
    };

// ----------------------------------------------------
// Panic
// Implemented in audiomessageui.cpp
// ----------------------------------------------------
GLREF_C void Panic(TAMSUiPanic aPanic);


#endif      // __AUDIOMESSAGEUIUIPANIC_H   
            

