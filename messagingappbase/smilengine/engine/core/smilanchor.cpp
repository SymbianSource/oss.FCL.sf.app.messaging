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
* Description: smilanchor implementation
*
*/



// INCLUDE FILES
#include "smilanchor.h"
#include "smilobject.h"

// C++ default constructor can NOT contain any code, that
// might leave.
//

CSmilAnchor::CSmilAnchor()
	{
	iSourceLevel = 100;
	iDestinationLevel = 100;
	iTabIndex = CSmilAnchor::KTabUnspecified;
	iAccessKey = 0;
	iExternal = ETrue;
	iSourcePlaystate = EStop;
	iDestinationPlaystate = EPlay;
	iShow = ENew;
	iActuate = EOnRequest;
	}


CSmilAnchor::~CSmilAnchor()
	{
	}


void CSmilAnchor::SetAltL( const TDesC& aStringValue )
	{
	iAlt.SetTextL(aStringValue);
	}


void CSmilAnchor::SetTargetL( const TDesC& aStringValue )
	{
	iTarget.SetTextL(aStringValue);
	}


void CSmilAnchor::SetHrefL( const TDesC& aStringValue )
	{
	iHref.SetTextL(aStringValue);
	}
/*
void CSmilAnchor::SetShowL( const TDesC& aStringValue )
	{
	if (aStringValue.Match(_L("ENew")))
		iShow = ENew;
	else if (aStringValue.Match(_L("EDeprecatedPause")))
		iShow = EDeprecatedPause;
	else if (aStringValue.Match(_L("EReplace")))
		iShow = EReplace;
	}

void CSmilAnchor::SetSourcePlayStateL( const TDesC& aStringValue )
	{
	if (aStringValue.Match(_L("EPlay")))
		iSourcePlaystate = EPlay;
	else if (aStringValue.Match(_L("EPause")))
		iSourcePlaystate = EPause;
	else if (aStringValue.Match(_L("EStop")))
		iSourcePlaystate = EStop;
	}
*/
TPtrC CSmilAnchor::Target() const
	{
	return iTarget.Text();
	}

EXPORT_C
TPtrC CSmilAnchor::Href() const
	{
	return iHref.Text();
	}

//
TPtrC CSmilAnchor::Alt() const
	{
	return iAlt.Text();
	}
/*
TPtrC CSmilAnchor::Show() const
	{
	if (iShow == ENew)
		return _L("ENew");
	else if (iShow == EDeprecatedPause)
		return _L("EDeprecatedPause");
	else if (iShow == EReplace)
		return _L("EReplace");
	}

TPtrC CSmilAnchor::SourcePlayState() const
	{
		if (iSourcePlaystate == EPlay)
		return _L("EPlay");
	else if (iSourcePlaystate == EPause)
		return _L("EPause");
	else if (iSourcePlaystate == EStop)
		return _L("EStop");
	}*/

// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File  



