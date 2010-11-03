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
* Description: smilpresentation.inl*
*/



inline TBool CSmilPresentation::TwoWayNavigation() const
	{ 
	return iTwoWayNavigation; 
	}

inline void CSmilPresentation::SetTwoWayNavigation(TBool aEnable) 
	{ 
	iTwoWayNavigation = aEnable;
	}

inline void CSmilPresentation::SetMaxDownUpScaling( TReal aDown, TReal aUp )
	{ 
	iDown = aDown;
	iUp = aUp;
	}
	
inline TInt CSmilPresentation::Volume() 
	{ 
	return iVolume;
	}


inline MSmilPlayer* CSmilPresentation::GetPlayer()
	{
	return iPlayer;
	}


inline
TBool CSmilPresentation::SlideModeEnabled() const
	{ 
	return iSlideModeEnabled; 
	}

inline
void CSmilPresentation::SetSlideModeEnabled(TBool aEnable)
	{ 
	iSlideModeEnabled = aEnable; 
	}

//not part of public API!

inline CSmilRootRegion* CSmilPresentation::GetLayout()
	{
	return iLayout;
	}

inline CSmilTimeContainer* CSmilPresentation::GetTimegraph()
	{
	return iTimegraph;
	}



inline void CSmilPresentation::SetEndAction(TEndAction aEndAction)
	{
	iEndAction = aEndAction;
	}

inline CSmilPresentation::TEndAction CSmilPresentation::GetEndAction() const
	{
	return iEndAction;
	}

inline void CSmilPresentation::SetStartAction(TStartAction aStartAction)
	{
	iStartAction = aStartAction;
	}

inline CSmilPresentation::TStartAction CSmilPresentation::GetStartAction() const
	{
	return iStartAction;
	}

inline void CSmilPresentation::SetRTLNavigation(TBool aRTL)
	{
	iRTL = aRTL;
	}

inline TBool CSmilPresentation::RTLNavigation() const
	{
	return iRTL;
	}

inline TBool CSmilPresentation::MediaSelected() const
	{
	return iMediaSelected;
	}

