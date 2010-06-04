/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Global DLL defines 
 *
*/


#ifndef S60QCONV_DEFINES_H
#define S60QCONV_DEFINES_H 

#include <QObject>

#ifdef BUILD_S60QCONV_DLL
#define S60QCONV_EXPORT Q_DECL_EXPORT
#else
#define S60QCONV_EXPORT Q_DECL_IMPORT
#endif

#endif // S60QCONV_DEFINES_H
