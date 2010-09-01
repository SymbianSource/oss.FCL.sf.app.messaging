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
* Description:  IMAPPRGR.INL
*
*/


TImap4UiProgress::TImap4UiProgress()
	:	iType(KImap4UiProgressType),
		iUiOperation(ECompleted),
		iFoldersToDo(0),
		iFoldersDone(0),
		iFoldersFailed(0),
		iBytesToDo(0),
		iBytesDone(0),
		iReturnedMsvId(0),
		iError(KErrNone),
		iSeparatorChar(0)

	{}
