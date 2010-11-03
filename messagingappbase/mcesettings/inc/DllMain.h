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
*     DLL internal functions and types implementation for Mce Settings.
*
*/



#ifndef     __DllMain_H__
#define     __DllMain_H__


//  INCLUDES

#include <e32std.h>

// MACROS

/**
 * Convert millimetres to twips.
 */
#define MM2TWIPS(mm) ((4000*(mm))/254)

// DATA TYPES

// FUNCTION PROTOTYPES

/**
 * Stop the program execution with module specific text and error code.
 * Call if an unrecoverable error occurs in this module's code.
 *
 * @param aCode     error code
 *
 * 
 */
void Panic( TInt aCode );


#endif // __DllMain_H__

// End of file
