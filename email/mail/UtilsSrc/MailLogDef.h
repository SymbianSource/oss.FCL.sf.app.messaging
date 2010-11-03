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
* Description:  Logging class defines
*
*/

#ifndef MAILLOGDEF_H
#define MAILLOGDEF_H

/////////////////////////////////////////////////////////////////////////////
// #Defines that control printing of additional information to debug stream.
/////////////////////////////////////////////////////////////////////////////

// enable logging in debug builds
#ifdef _DEBUG
#define MAIL_ENABLE_LOGGING
#define MAIL_LOG_TO_FILE
#define MAIL_ENABLE_UNIT_TEST
#endif // _DEBUG

// Define this to enable alloc failure testing
//#define MAIL_ENABLE_ALLOC_TEST
// Defining this enables logging
//#define MAIL_ENABLE_LOGGING
// Defining this will direct all messages to a log file
//#define MAIL_LOG_TO_FILE
// Defining this will allow Unit Tests access to private functions & members
//#define MAIL_ENABLE_UNIT_TEST

#endif // MAILLOGDEF_H

//  End of File  
