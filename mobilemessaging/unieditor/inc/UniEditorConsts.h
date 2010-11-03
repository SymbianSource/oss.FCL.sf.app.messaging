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
* Description: UniEditorConsts  declaration
*
*/


 
#ifndef __UNIEDITORCONSTS_H
#define __UNIEDITORCONSTS_H

//  INCLUDES

// CONSTANTS

//-----------------------------------------------
// Flag bits in iMtmData1
// The default value for all bits is 0
//-----------------------------------------------

const TUint32 KUniMessageCurrentTypeMask =    0x00000003;
const TUint32 KUniMessageCharSetSupportMask = 0x0000000C;
const TInt KUniMessageCharSetSupportShift = 2;

const TUint32 KUniMessageTypeLocked =         0x00000010;
const TUint32 KUniMessageForwarded =          0x00000020;
const TUint32 KUniMessageEditorOriented =     0x00000040;

// DATA TYPES

enum TUniMessageCurrentType
    {
    EUniMessageCurrentTypeSms = 0,
    EUniMessageCurrentTypeMms,
    EUniMessageCurrentTypeReserved1,
    EUniMessageCurrentTypeReserved2
    };


enum TUniMessageCharSetSupport
    {
    EUniMessageCharSetFull = 0,
    EUniMessageCharSetReduced,
    EUniMessageCharSetFullLocked,
    EUniMessageCharSetReducedLocked
    };

#endif      // __UNIEDITORCONSTS_H
            
// End of File
