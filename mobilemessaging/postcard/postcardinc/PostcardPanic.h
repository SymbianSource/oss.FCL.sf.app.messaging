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
*       Postcard panic definitions.
*
*/



#ifndef __POSTCARDPANIC_H
#define __POSTCARDPANIC_H

// DATA TYPES
enum TPostcardPanic
    {
    EPostcardPanicAiwCmdId,
    EPostcardPanicAiwEvent,
    EPostcardPanicAiwEventParam,
    EPostcardPanicArgument,
    EPostcardPanicCoding,
    EPostcardPanicRepositoryFailure
    };


// FUNCTION PROTOTYPES

// ----------------------------------------------------
// Panic
// Implemented in PostcardApp.cpp
// ----------------------------------------------------
void Panic(TPostcardPanic aPanic);

#endif  // __POSTCARDPANIC_H
            
// End of File
