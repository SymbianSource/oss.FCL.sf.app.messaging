/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *
 */

#ifndef MATCH_PHNUMBER_UTIL_H
#define MATCH_PHNUMBER_UTIL_H

#include "msguiutilsmanager.h"
/**
 * This class provides number of ph number digits to be matched for contact matching
 * 
 */
class MSGUI_UTILS_DLL_EXPORT MatchPhNumberUtil
    {
    
public:
    
    /**
     * Get amount of digits to be used in contact matching
     */
    static int matchDigits();
    
    };
    

    

#endif // MATCH_PHNUMBER_UTIL_H

// EOF

