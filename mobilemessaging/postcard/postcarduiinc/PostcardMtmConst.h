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
* Description:  
*     General constants for Postcard.
*
*/



#ifndef POSTCARDMTMCONST_H
#define POSTCARDMTMCONST_H

// CONSTANTS

const TUid KUidPostcardId = {0x10207247};

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
