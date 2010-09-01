/*
* Copyright (c) 1999 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PROGTYPE.H
*
*/


#ifndef __PROGTYPE_H__
#define __PROGTYPE_H__

// Common between POP3 and IMAP4 MTMs.
enum TMsvUiProgressType
	{
	EUiProgTypeBase = 0x200000,
	EUiProgTypeEditing,
	EUiProgTypeConnecting,
	EUiProgTypeImap4Ui,
	EUiProgTypeSizingMsg,
	EUiProgTypeLast = 0x300000
	};

class TEmailEditProgress
	{
public:
	inline TEmailEditProgress() : iProgType(EUiProgTypeEditing), iError(0), iId(0) {}
public:
	TInt iProgType;
	TInt iError;
	TMsvId iId;
	};

#endif

