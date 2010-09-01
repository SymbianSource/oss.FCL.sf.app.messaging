/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       Postcard UI Panic definitions.
*
*/



#ifndef __POSTCARDUIPANIC_H
#define __POSTCARDUIPANIC_H

//  INCLUDES
#if !defined(__E32BASE_H__)
#include <e32base.h>                     // GLREF_C
#endif


// DATA TYPES

enum TPostcardUiPanic
    {
    EPostcardNotAMessage,
    EPostcardNotAService,
    EPostcardWrongMtmType,
    EPostcardWrongParameters
    };


// FUNCTION PROTOTYPES

// ----------------------------------------------------
// Panic
// Implemented in PostcardUi.cpp
// ----------------------------------------------------
GLREF_C void Panic(TPostcardUiPanic aPanic);


#endif      // __POSTCARDUIPANIC_H   
            
// End of File
