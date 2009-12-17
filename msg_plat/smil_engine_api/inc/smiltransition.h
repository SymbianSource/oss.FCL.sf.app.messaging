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
* Description: smiltransition  declaration
*
*/



#ifndef SMILTRANSITION_H
#define SMILTRANSITION_H

#include <e32base.h>
#include <gdi.h>

#include <smiltime.h>
#include <smiltextbuf.h>

/**
 * Descripes properties of a SMIL2 transition
 */
class CSmilTransition : public CBase
	{
	public:

		CSmilTransition();

		void SetId(const TDesC& iVal) { iId.SetTextL(iVal); }
		void SetType(const TDesC& iVal) { iType.SetTextL(iVal); }
		void SetSubtype(const TDesC& iVal) { iSubtype.SetTextL(iVal); }
		const TPtrC Id() const { return iId.Text(); }
		const TPtrC Type() const { return iType.Text(); }
		const TPtrC Subtype() const { return iSubtype.Text(); }
	private:
		RSmilTextBuf iId;
		RSmilTextBuf iType;
		RSmilTextBuf iSubtype;
	public:

		TSmilTime iDur;
		TRgb iFadeColor;
		TBool iReverse;

		CSmilTransition* iNext;

		TInt8 iStartProgress;
		TInt8 iEndProgress;
	
	};

#endif
