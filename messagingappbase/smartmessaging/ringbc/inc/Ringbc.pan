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
*     Ringing tone bio control Panics
*     Defines panic codes used in Ringbc.
*
*/





#ifndef RINGBC_PAN
#define RINGBC_PAN

enum TRingBcPanic
    {
    EPlayerNull,
    EAudioFileNull
    };

GLREF_C void Panic(TRingBcPanic aPanic);


#endif //MSGEDITORUTILS_PAN

// End of file
