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
* Description:  NcnDebug  declaration
*
*/

#ifndef NCNDEBUG_H
#define NCNDEBUG_H

#include    "NcnVariant.hrh"

#include    <e32svr.h>
#include    <flogger.h>
#include    <f32file.h>

// DEFINES
#ifdef _DEBUG   // debug build environment

#ifdef _NCN_FLOG

// The log path
_LIT( KLogPath, "c:\\logs\\NcnList\\" );

// The name of the log folder
_LIT(KLogDir,"NcnList");

//The log file
_LIT(KLogFile,"NcnListLog.txt");

// Declare the FPrint function
inline void FPrint(const TRefByValue<const TDesC> aFmt, ...)
    {
    VA_LIST list;
    VA_START(list,aFmt);
    
	// Make the log directory
	RFs fs;
	if (fs.Connect() == KErrNone)
		{
		fs.MkDirAll( KLogPath );
		fs.Close();
		}
		
    RFileLogger::WriteFormat(KLogDir, KLogFile, EFileLoggingModeAppend, aFmt, list);
    }

    #define NCN_RDEBUG(X)                   { FPrint(X); }
    #define NCN_RDEBUG_INT(X,Y)             { FPrint(X,Y); }
    #define NCN_RDEBUG_INT2(X,Y,Z)          { FPrint(X,Y,Z); }

#else   // _NCN_FLOG

    #define NCN_RDEBUG(X)                   RDebug::Print(X)
    #define NCN_RDEBUG_INT(X,Y)             RDebug::Print(X,Y)
    #define NCN_RDEBUG_INT2(X,Y,Z)          RDebug::Print(X,Y,Z)

#endif  // _NCN_FLOG

#else   // _DEBUG hw environment
    #define NCN_RDEBUG(X)
    #define NCN_RDEBUG_INT(X,Y)
    #define NCN_RDEBUG_INT2(X,Y,Z)
#endif  // _DEBUG

#endif  // NCNDEBUG_H
