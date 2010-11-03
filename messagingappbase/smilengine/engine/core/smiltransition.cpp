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
* Description: smiltransition implementation
*
*/



#include "smiltransition.h"


CSmilTransition::CSmilTransition()
	{
		// "All of the transitions defined in the Appendix have 
		// a default duration of 1 second."
		iDur = TSmilTime::FromMicroSeconds(1000000);
		iFadeColor = TRgb(0,0,0);
		iReverse = EFalse;
		iNext = 0;
		iStartProgress = 0;
		iEndProgress = 100;
	}
