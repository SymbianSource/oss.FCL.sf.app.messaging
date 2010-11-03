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
* Description: Logging.h
*
*
*/


#ifndef __LOGGING_H__
#define __LOGGING_H__

/*****************************************************************************
 DEBUGGING ACTIVE
*****************************************************************************/
#ifdef _DEBUG

/**
 * These macros should be defined in the component header, before including
 * this file.
 *
 * @code_start
 * #define _IMUM_LOG_NAME xxx
 * #define _IMUM_LOG_PATH xxxx
 * #define _IMUM_LOG_COMPONENT_ID 0
 * #define _IMUM_LOG_BANNER xxx
 * #include <logging.h>
 * @code_end
 */

/** The name of the file, where the logs should appear (e.g. component.txt) */
#ifndef _IMUM_LOG_NAME
#define _IMUM_LOG_NAME "Imum.txt"
#pragma message( "*** Macro _IMUM_LOG_NAME undefined, using default :" _IMUM_LOG_NAME )
#endif

/** The name of the folder under the c:\logs (e.g. component)*/
#ifndef _IMUM_LOG_PATH
#define _IMUM_LOG_PATH "Email"
#pragma message( "*** Macro _IMUM_LOG_PATH undefined, using default :" _IMUM_LOG_PATH )
#endif

/** The id the component */
#ifndef _IMUM_LOG_COMPONENT_ID
#define _IMUM_LOG_COMPONENT_ID 0
#pragma message( "*** Macro _IMUM_LOG_COMPONENT_ID undefined, using default : 0" )
#endif

/** The banner text for the logs */
#ifndef _IMUM_LOG_BANNER
#define _IMUM_LOG_BANNER "Internet Mail UI MTM %d.%d.%d"
#endif

/** LOGGING DATA */
#include "ImumLogUtils.h"

/*****************************************************************************
 NO DEBUGGING
*****************************************************************************/
#else // _DEBUG
#include "ImumLogUtils.h"
#endif

#endif // __LOGGING_H__
