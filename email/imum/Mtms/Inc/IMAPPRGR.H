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
* Description:  IMAPPRGR.H
*
*/


#ifndef __IMAPPRGR_H__
#define __IMAPPRGR_H__

//	General includes
#include <e32std.h>
#include "PROGTYPE.H"

const TInt KImap4UiProgressType = EUiProgTypeImap4Ui;	//	Use to identify this type of progress to MtmUi

class TImap4UiProgress
	{
public:
	inline TImap4UiProgress();

	TInt iType;	//	Will always be KImap4UiProgressType

	enum
		{
		ECompleted,
		EUnsubscribing,
		ESubscribing,
		ESynchronising,
		ECreatingFolder,
		ERenamingFolder,
		EDeletingFolder,
		EPreConnecting,
		EPurgingAttachment,
		EFetchingAttachment,
		EDeletingMessages
		};

	TInt iUiOperation;

	//	As used during subscription/unsubscription
	//
	TInt iFoldersToDo;
	TInt iFoldersDone;

	TInt iFoldersFailed;

	//	As used during attachment fetching
	//
	TInt iBytesToDo;
	TInt iBytesDone;

	TInt iReturnedMsvId;

	TInt iError;

	TChar iSeparatorChar;
	};

typedef TPckgBuf<TImap4UiProgress> TImap4UiProgressBuf;

#include "IMAPPRGR.INL"

#endif //__IMAPPRGR_H__
