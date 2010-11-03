/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Unified Message Editor Utils definitions.
*
*/



#ifndef UNIUTILS_H
#define UNIUTILS_H

// INCLUDES

#include <e32def.h>
#include <MsgEditor.hrh>

// CONSTANTS

// UiUtils bitmaps.
_LIT( KUniUtilsMBMFileName, "uniutils.mbm" );
_LIT( KUniUtilsResourceFileName, "uniutils.rsc" );

// MACROS

// FORWARD DECLARATIONS

// DATA TYPES

enum TUniObjectTypeFlags
    {
    EUniNoneFlag    = 0x00,
    EUniAudioFlag   = 0x01,
    EUniImageFlag   = 0x02,
    EUniTextFlag    = 0x04,
    EUniVideoFlag   = 0x08,
    EUniSvgFlag     = 0x10
    };

// FUNCTION PROTOTYPES

// CLASS DECLARATION

#endif // UNIUTILS_H

// End of file
