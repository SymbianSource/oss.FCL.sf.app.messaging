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
* Description:   Logger facility for the SendUi.
*
*/




#ifndef SENDUILOG_H
#define SENDUILOG_H

// MACROS

#include <flogger.h>                    // HW debug logging support

//
#ifdef USE_LOGGER
//

_LIT(KSendUiLogFile,"SENDUI.TXT");
_LIT(KSendUiLogDir,"SENDUI");
_LIT(KSendUiTimeFormatString,"%H:%T:%S:%*C2");

#define CREATELOGTEXT(a)    RFileLogger::Write(KSendUiLogDir(),KSendUiLogFile(),EFileLoggingModeOverwrite,a)
#define LOGTEXT(a)          RFileLogger::Write(KSendUiLogDir(),KSendUiLogFile(),EFileLoggingModeAppend,a)
#define LOGTEXT2(a,b)       RFileLogger::WriteFormat(KSendUiLogDir(),KSendUiLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(a),b)
#define LOGTEXT3(a,b,c)     RFileLogger::WriteFormat(KSendUiLogDir(),KSendUiLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(a),b,c)
#define LOGHEXDUMP(a,b,c,d) RFileLogger::HexDump(KSendUiLogDir(),KSendUiLogFile(),EFileLoggingModeAppend,a,b,c,d);
#define LOGTIMESTAMP(a)     {_LIT(temp, a); TTime time; time.HomeTime(); TBuf<256> buffer; time.FormatL( buffer, KSendUiTimeFormatString ); buffer.Insert(0, temp); RFileLogger::Write(KSendUiLogDir, KSendUiLogFile, EFileLoggingModeAppend, buffer); }

#else // no logger

#define CREATELOGTEXT(a)
#define LOGTEXT(a)
#define LOGTEXT2(a,b)
#define LOGTEXT3(a,b,c)
#define LOGHEXDUMP(a,b,c,d)
#define LOGTIMESTAMP(a)

#endif // USE_LOGGER

#endif // SENDUILOG_H
            
// End of File
