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
*     General constants for MMS.
*
*/



#ifndef MMSMTMCONST_H
#define MMSMTMCONST_H

// CONSTANTS
const TUid KUidMmsEditor = {0x100058DE};
const TUid KUidMmsViewer = {0x100058DF};
const TUid KUidNotViewer = {0x101F86A0};

const TInt KMmsViewer = 0x100058DF;
const TInt KNotViewer = 0x101F86A0;

const TInt KKiloByte = 1024;

const TInt KMmsNumberOfZoomStates = 2;

_LIT(KMmsMtmResourceFile,"MMSUI");
_LIT(KNotMtmResourceFile,"NOTUI");
_LIT(KMmsMtmUiBitmapFile, "muiu.mbm");

// MACROS

#include <flogger.h>                    // HW debug logging support
                                      
#ifdef USE_LOGGER
_LIT(KMmsUiLogFile,"MMSUI.TXT");
_LIT(KMmsUiLogDir,"MMSUI");
_LIT(KMmsUiTimeFormatString,"%H:%T:%S:%*C2");

#define CREATELOGTEXT(a)    RFileLogger::Write(KMmsUiLogDir(),KMmsUiLogFile(),EFileLoggingModeOverwrite,a)
#define LOGTEXT(a)          RFileLogger::Write(KMmsUiLogDir(),KMmsUiLogFile(),EFileLoggingModeAppend,a)
#define LOGTEXT2(a,b)       RFileLogger::WriteFormat(KMmsUiLogDir(),KMmsUiLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(a),b)
#define LOGTEXT3(a,b,c)     RFileLogger::WriteFormat(KMmsUiLogDir(),KMmsUiLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(a),b,c)
#define LOGHEXDUMP(a,b,c,d) RFileLogger::HexDump(KMmsUiLogDir(),KMmsUiLogFile(),EFileLoggingModeAppend,a,b,c,d);
#define LOGTIMESTAMP(a)     {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KMmsUiTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KMmsUiLogDir, KMmsUiLogFile, EFileLoggingModeAppend, buffer); }

#else // no logger

#define CREATELOGTEXT(a)
#define LOGTEXT(a)
#define LOGTEXT2(a,b)
#define LOGTEXT3(a,b,c)
#define LOGHEXDUMP(a,b,c,d)
#define LOGTIMESTAMP(a)

#endif // USE_LOGGER

#endif // MMSMTMCONST_H
            
// End of File
