/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: smilprefetch implementation
*
*/



#include "smilprefetch.h"


CSmilPrefetch* CSmilPrefetch::NewL(CSmilPresentation* aPresentation)
	{
	CSmilPrefetch* ptr = new ( ELeave ) CSmilPrefetch;
	CleanupStack::PushL( ptr );
	ptr->ConstructL(aPresentation);
	CleanupStack::Pop(); //ptr
	return ptr;
	}

CSmilPrefetch::CSmilPrefetch() 
	{
	}

// Destructor
CSmilPrefetch::~CSmilPrefetch()
	{
	}


TSmilTime CSmilPrefetch::ImplicitDuration() const
	{
	return 0;
	}
