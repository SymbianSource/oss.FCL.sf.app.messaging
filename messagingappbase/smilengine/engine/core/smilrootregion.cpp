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
* Description: smilrootregion implementation
*
*/



// INCLUDE FILES
#include "smilrootregion.h"
#include "smilpresentation.h"
#include "smilarea.h"

// ================= MEMBER FUNCTIONS =======================

CSmilRootRegion* CSmilRootRegion::NewL( CSmilPresentation* aPresentation )
	{
	CSmilRootRegion* self = new(ELeave) CSmilRootRegion(aPresentation);
//	CleanupStack::PushL(self);
	//self->ConstructL();
//	CleanupStack::Pop();	
	return self;
	}


CSmilRootRegion::CSmilRootRegion(CSmilPresentation* aPresentation)
: CSmilRegion(aPresentation)
	{
	iFirstChild=0;
	iLastChild=0;
	iDefaultLayout = ETrue;
	iBackgroundMode = EAlways;
	}
        
CSmilRootRegion::~CSmilRootRegion()
	{

	for (CSmilRegion* r = FirstChild(); r; )
		{
		CSmilRegion* del = r;
		r=r->NextSibling();
		delete del;
		}
	}


void CSmilRootRegion::AddChild(CSmilRegion* aChild)
	{
	if (iLastChild)
		{
		iLastChild->iNext = aChild;
		aChild->iPrevious = iLastChild;
		iLastChild = aChild;
		}
	else
		{
		iFirstChild = aChild;
		iLastChild = aChild;
		}
	aChild->iParent = this;
	}


void CSmilRootRegion::Redraw()
	{
	Redraw(iRect);
	}

void CSmilRootRegion::Layout(TReal32 aScalingFactor)
	{
	iRect = TRect(0,0,iWidth.GetLength(0),iHeight.GetLength(0));
	iScalingFactor=aScalingFactor;

	if (iPresentation && iPresentation->GetPlayer())
		{
		const TRect& r = iPresentation->GetPlayer()->GetDimensions(iPresentation);
		TReal32 iDownScale, iUpScale; 
		
		iDownScale = iPresentation->GetMaxDownScaling();
		iUpScale = iPresentation->GetMaxUpScaling();
		
		if (iRect.Width()==0)
			iRect.iBr.iX = r.Width();
		if (iRect.Height()==0)
			iRect.iBr.iY = r.Height();
		
		if ((iDownScale != 1.0) && (iRect.Width()>r.Width() || iRect.Height()>r.Height()))
			{
				// scale down!
				TInt w1 = iRect.Width()*r.Height()/iRect.Height();
				TInt w2 = r.Width();
				TInt w;
				if (w1<w2)
					w = w1;
				else 
					w = w2;
				iScalingFactor=(TReal32)w/iRect.Width();
				if (iScalingFactor < iDownScale)
					iScalingFactor = iDownScale;
			}
		else if ((iUpScale != 1.0) && (iRect.Width()<r.Width() && iRect.Height()<r.Height()))
			{
				// scale up!
				TInt w1 = iRect.Width()*r.Height()/iRect.Height();
				TInt w2 = r.Width();
				TInt w;
				if (w1<w2)
					w = w1;
				else 
					w = w2;
				iScalingFactor=(TReal32)w/iRect.Width();
				if (iScalingFactor > iUpScale)
					iScalingFactor = iUpScale;
			}
		}

	TBool changes;
	do	// sort by z-index
		{
		changes=EFalse;

		for (CSmilRegion* r = iFirstChild; r; )
			{
			CSmilRegion* next = r->iNext;
			if ( next && (r->GetZIndex() > next->GetZIndex()) )
				{
				if (r->iPrevious)
					r->iPrevious->iNext = next;
				else
					iFirstChild = next;

				next->iPrevious = r->iPrevious;

				r->iNext = next->iNext;
				r->iPrevious = next;

				next->iNext = r;

				if (r->iNext)
					r->iNext->iPrevious = r;
				else
					iLastChild = r;

				changes = ETrue;

				}
			else
				r=r->iNext;
			}
		} 
		while (changes);

    for (CSmilRegion* c=FirstChild(); c!=0; c=c->NextSibling())
		{
        c->Layout(iScalingFactor);
		}	
	iRect.iBr.iX = (TInt)(iScalingFactor*iRect.Width());
	iRect.iBr.iY = (TInt)(iScalingFactor*iRect.Height());
	}


void CSmilRootRegion::Redraw(TRect aUpdate)
	{
	if (iPresentation && iPresentation->GetPlayer())
		{
		iPresentation->GetPlayer()->Redraw(aUpdate, iPresentation);
		}
	}


void CSmilRootRegion::Draw(CGraphicsContext& aGc, const TRect& aRect)
	{
//	PrintTree();

	//RDebug::Print(_L("CSmilRootRegion::Draw()"));
	CSmilRegion::Draw(aGc,aRect);

	for (CSmilRegion* r = FirstChild(); r!=0; r=r->NextSibling())
		{
		if (r->GetRectangle().Intersects(aRect))
			r->Draw(aGc,aRect);
		}
	
	}


CSmilRegion* CSmilRootRegion::FindRegion(const TDesC& aId) 
	{
	for (CSmilRegion* r = iFirstChild; r!=0; r = r->NextSibling())
		{
		if (r->iName.Text()==aId)
			return r;
		}
		
	if (iId.Text()==aId)
		return this;
	
	for (CSmilRegion* r1 = iFirstChild; r1!=0; r1 = r1->NextSibling())
		{
		if (r1->iId.Text()==aId)
			return r1;
		}
	return 0;
	}

TRect CSmilRootRegion::GetRectangle() const
	{
	if (iDefaultLayout)
		return iRect;
	else
		return iRect;
	}

void CSmilRootRegion::SetDefaultLayout(TBool aDef)
	{	
	iDefaultLayout = aDef;
	}


CSmilMedia* CSmilRootRegion::MediaAt(const TPoint& aPoint, TBool aOnlyFocusable)
	{
	TInt highest_z = KMinTInt;
	CSmilMedia* highest=0;

	for (CSmilRegion* r = iLastChild; r!=0; r = r->PreviousSibling())
		{
		CSmilMedia* m = r->MediaAt(aPoint, aOnlyFocusable);
		if (m && r->iZIndex>highest_z)
		{
			highest = m;
			highest_z = r->iZIndex;
		}
	}
	if (highest)
		return highest;

	return CSmilRegion::MediaAt(aPoint, aOnlyFocusable);
	}

TReal32 CSmilRootRegion::GetScalingFactor() 
{ 
	return iScalingFactor;	
}

CSmilArea* CSmilRootRegion::AreaAt(const TPoint& aPoint, TBool aOnlyFocusable)
	{

	if (iLastChild != 0)
	{
		for (CSmilRegion* r = iLastChild; r!=0; r = r->PreviousSibling())
		{
			CSmilArea* a = r->AreaAt(aPoint, aOnlyFocusable);
			if (a && (a->LinkedMedia() == MediaAt(aPoint, EFalse)))
 				return a;
 		}
		return 0;
	}
	else 	
 		return CSmilRegion::AreaAt(aPoint, aOnlyFocusable);
	//return 0;
	}


CSmilObject* CSmilRootRegion::FindNearest(const TPoint& aPoint, TDirection iDir)
	{
	TInt dist = KMaxTInt;
	return FindNearest(aPoint, iDir, dist);
	}


CSmilObject* CSmilRootRegion::FindNearest(const TPoint& aPoint, TDirection iDir, TInt& dist2)
	{			
	CSmilObject* found = CSmilRegion::FindNearest(aPoint, iDir, dist2);

	for (CSmilRegion* r = iLastChild; r!=0; r = r->PreviousSibling())
		{
		CSmilObject* o = r->FindNearest(aPoint, iDir, dist2);
		if (o)
			found = o;
		}			
			
	return found;

	}

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  





