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
* Description: smilregion implementation
*
*/



// INCLUDE FILES
#include "smilregion.h"
#include "smilpresentation.h"

#include "smilfocusinterface.h"
#include "smilmediarendererinterface.h"

#include "smilarea.h"
#include "smilmedia.h"

// ================= MEMBER FUNCTIONS =======================

CSmilRegion* CSmilRegion::NewL( CSmilPresentation* aPresentation )
	{
	CSmilRegion* self = new(ELeave) CSmilRegion(aPresentation);
//	CleanupStack::PushL(self);
	//self->ConstructL();
//	CleanupStack::Pop();
	return self;
	}


CSmilRegion::CSmilRegion(CSmilPresentation* aPresentation)
	{
	iPresentation = aPresentation;
	iTransparent = ETrue;
	iFit = EHidden;
	iBackgroundMode = EAlways;
	iNext=0;
	iPrevious=0;
	iParent=0;
	iVisibleMedia = 0;
	iDefaultLayout = EFalse;
	iForeground = TRgb(0,0,0);
	}
        
CSmilRegion::~CSmilRegion()
	{
	for (MediaListItem* i = iVisibleMedia;i;)
		{
		MediaListItem* del = i;
		i=i->iNext;
		delete del;
		}
	}


TRect CSmilRegion::GetRectangle() const
	{
	return iRect;
	}

TRect CSmilRegion::GetRectangle(const CSmilMedia* aMedia) const
	{
	if (iFit != EHidden)
		return iRect;

	MediaListItem* i = iVisibleMedia;
	for (;i;i=i->iNext)
		{
		if (i->iMedia==aMedia)
			break;
		}	

	if (i && i->iArea.Width()>0)
		return i->iArea;
	else
		return iRect;
	}

TInt CSmilRegion::GetZIndex() const
	{
	return iZIndex;
	}

MSmilRegion::TFit CSmilRegion::GetFit() const
	{
	return iFit;
	}

TRgb CSmilRegion::GetBackground() const
	{
	return iBackground;
	}

TRgb CSmilRegion::GetForeground() const
	{
	return iForeground;
	}

MSmilRegion::TBgMode CSmilRegion::GetBackgroundMode() const
	{
	return iBackgroundMode;
	}

void CSmilRegion::Redraw()
	{
	if (Parent())
		{
		Parent()->Redraw(iRect);
		}
	}


void CSmilRegion::Redraw(TRect aUpdate)
	{
	if (Parent())
		{
		Parent()->Redraw(iTransparent?aUpdate:iRect);
		}
	}


MSmilRegion* CSmilRegion::Parent() const
	{
	return iParent;
	}


MSmilRegion* CSmilRegion::Root() const
	{
	MSmilRegion* p = Parent();
	if (!p || p->IsRoot())
		return p;
	else
		return p->Root();
	}


void CSmilRegion::AddMediaL(CSmilMedia* aMedia)
	{
	// of regions with same zindex, the one with most recent media is on top

	CSmilRegion* n = iNext;
	while (n && n->iZIndex == iZIndex)
		{
		if (iPrevious)
			iPrevious->iNext = n;
		else
			iParent->SetFirstChild(n);
		
		n->iPrevious = iPrevious;

		iNext = n->iNext;
		iPrevious = n;

		n->iNext = this;
				
		if (iNext)
			iNext->iPrevious = this;
		else
			iParent->SetLastChild(this);

		n = iNext;
		};

	MediaListItem* nw = 0;
	MediaListItem* i = iVisibleMedia;

	TInt x = 0;
	
	if (iVisibleMedia)
		{
		if (iVisibleMedia->iMedia == aMedia)
			{			
			nw = iVisibleMedia;
			iVisibleMedia = i = nw->iNext;			
			}
		else
			if (i->iArea.iBr.iX>x) x = i->iArea.iBr.iX;

		while (i && i->iNext)
			{					
			if (i->iNext->iMedia==aMedia)
				{				
				nw = i->iNext;
				i->iNext = i->iNext->iNext;				
				}			
			else
				{
				if (i->iNext->iArea.iBr.iX>x) x = i->iNext->iArea.iBr.iX;
				}
			if (!i->iNext)
				break;
			i=i->iNext;
			}			
		}

	if (nw)
		nw->iNext=0;
	else
		nw = new (ELeave) MediaListItem(aMedia);

//	RDebug::Print(_L("CSmilRegion x=%d"),x);

	if (iFit==EHidden && aMedia->GetRenderer())
		{
		nw->iArea = TRect(TPoint(iRect.iTl.iX+(iDefaultLayout?x:0),iRect.iTl.iY),
			TSize(aMedia->GetRenderer()->IntrinsicWidth(),aMedia->GetRenderer()->IntrinsicHeight()));		
		nw->iArea.Intersection(iRect);
		}
		
	if (i)
		i->iNext = nw;
	else
		iVisibleMedia = nw;
	}

void CSmilRegion::RemoveMedia(CSmilMedia* aMedia)
	{
	if (iVisibleMedia)
		{
		if (iVisibleMedia->iMedia == aMedia)
			{
			MediaListItem* del = iVisibleMedia;
			iVisibleMedia = iVisibleMedia->iNext;
			delete del;
			return;
			}
			
	
		for (MediaListItem* i = iVisibleMedia;i->iNext;i=i->iNext)
			if (i->iNext->iMedia==aMedia)
				{
				MediaListItem* del = i->iNext;
				i->iNext = i->iNext->iNext;				
				delete del;
				break;
				}		
	
		}
	}

TBool CSmilRegion::ContainsMedia(const CSmilMedia* aMedia) const
	{
	for (MediaListItem* i = iVisibleMedia;i;i=i->iNext)
		if (i->iMedia==aMedia)
			return ETrue;
	return EFalse;
	}


void CSmilRegion::Layout(TReal32 aScalingFactor)
	{
    /// SMIL spec chapter 5.3.1

	TRect rec = Parent()->GetRectangle();
    TInt bbw = rec.Width();
    TInt bbh = rec.Height();

    TInt l,r,t,b,w,h;


    // vertical direction
    if (iLeft.IsAuto() && iWidth.IsAuto() && iRight.IsAuto())
		{
        l=0; w=bbw; r=0;
		}
    else if (iLeft.IsAuto() && iWidth.IsAuto())
		{
        l=0; r=iRight.GetLength(bbw); w=bbw-r;
		}
    else if (iLeft.IsAuto() && iRight.IsAuto())
		{
        l=0; w=iWidth.GetLength(bbw); r=bbw-w;
		}
    else if (iLeft.IsAuto())
		{
        w=iWidth.GetLength(bbw); r=iRight.GetLength(bbw); l=bbw-r-w;
		}
    else if (iWidth.IsAuto() && iRight.IsAuto())
		{
        l=iLeft.GetLength(bbw); w=bbw-l; r=0;
		}
    else if (iWidth.IsAuto())
		{
        l=iLeft.GetLength(bbw); r=iRight.GetLength(bbw); w=bbw-l-r;
		}
    else // right==auto and overconstrained case
		{
        l=iLeft.GetLength(bbw); w=iWidth.GetLength(bbw); r=bbw-l-w;
		}

    // horizontal direction
    if (iTop.IsAuto() && iHeight.IsAuto() && iBottom.IsAuto())
		{
        t=0; h=bbh; b=0;
		}
    else if (iTop.IsAuto() && iHeight.IsAuto())
		{
        t=0; b=iBottom.GetLength(bbh); h=bbh-b;
		}
    else if (iTop.IsAuto() && iBottom.IsAuto())
		{
        t=0; h=iHeight.GetLength(bbh); b=bbh-h;
		}
    else if (iTop.IsAuto())
		{
        h=iHeight.GetLength(bbh); b=iBottom.GetLength(bbh); t=bbh-b-h;
		}
    else if (iHeight.IsAuto() && iBottom.IsAuto())
		{
        t=iTop.GetLength(bbh);; h=bbh-t; b=0;
		}
    else if (iHeight.IsAuto())
		{
        t=iTop.GetLength(bbh); b=iBottom.GetLength(bbh); h=bbh-t-b;
		}
    else // iBottom==auto and overconstrained case
		{
        t=iTop.GetLength(bbh); h=iHeight.GetLength(bbh); b=bbh-t-h;
		}

	l=(TInt)(l*aScalingFactor);
	t=(TInt)(t*aScalingFactor);
	w=(TInt)(w*aScalingFactor);
	h=(TInt)(h*aScalingFactor);
    iRect = TRect(TPoint(l,t),TSize(w,h));

    for (CSmilRegion* c=FirstChild(); c!=0; c=c->NextSibling())
		{
        c->Layout();
		}

	}


void CSmilRegion::Draw(CGraphicsContext& aGc, const TRect& aRect)
	{
	//RDebug::Print(_L("CSmilRegion::Draw(%d,%d,%d,%d)"),
//		aRect.iTl.iX,aRect.iTl.iY,aRect.Width(),aRect.Height());

	aGc.SetClippingRect(iRect);
	// draw the background
	if (((iVisibleMedia && iBackgroundMode==EWhenActive) || iBackgroundMode==EAlways) && !iTransparent)
		{
		TRect drawRect = iRect;
		drawRect.Intersection(aRect);

		//RDebug::Print(_L("drawing background..."));
		aGc.SetBrushColor(iBackground);
		aGc.SetPenStyle(CGraphicsContext::ESolidPen);
		aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc.SetPenColor(iBackground);
		aGc.DrawRect(drawRect);
		}			

	// draw the media		
	for (MediaListItem* i = iVisibleMedia;i;i=i->iNext)
		{
		CSmilMedia* m = i->iMedia;
		if (m->GetRenderer())
			{
			//RDebug::Print(_L("drawing media..."));
			MSmilFocus* focus = 0;
			if (m->HasFocus())
				{
				focus = m;
				}
			else if (m->FirstChild())
				{
				for (CSmilObject* o = m->FirstChild(); o; o=o->NextSibling())
					{
					if (o->IsArea())
						{
						CSmilArea* area = static_cast<CSmilArea*>(o);
						if (area->HasFocus() && area->IsFocusable())
							{
							focus = area;
							break;
							}
						}
					}
				}
			m->GetRenderer()->Draw(aGc,aRect,m->ActiveTransitionFilter(),focus);
			}
		}
	aGc.CancelClippingRect();

	}

void CSmilRegion::PrintTree(int d) const
	{
		
	TBuf<100> spacer;
	for (TInt n=0;n<d;++n) spacer.Append(_L(" "));

	RDebug::Print(_L("%Sid=%S (%d,%d,%d,%d)"), &spacer, 
		&iId, iRect.iTl.iX, iRect.iTl.iY, iRect.Width(), iRect.Height());

		
	for (CSmilRegion* r = FirstChild(); r!=0; r=r->NextSibling())
		r->PrintTree(d+2);

	}

void CSmilRegion::MediaTransitionEnd(CSmilMedia* aMedia)
	{
	MediaListItem* i = iVisibleMedia;
	while (i)
		{		
		// unfreeze may remove this item
		MediaListItem* n=i->iNext;		
		CSmilMedia* m = i->iMedia;
		if (m!=aMedia && m->Fill()==EFillTransition)
			m->Unfreeze();
		i=n;
		}		
	}


CSmilMedia* CSmilRegion::MediaAt(const TPoint& aPoint, TBool aOnlyFocusable)
	{

	MediaListItem* i = iVisibleMedia;
	CSmilMedia* found = 0;
	for (;i;i=i->iNext)
		{
		if (i->iMedia->IsFocusable() || !aOnlyFocusable)
			{
			TRect r = GetRectangle(i->iMedia);
			if (r.Contains(aPoint))
				found = i->iMedia;
			}		
		}

	return found;
	}


CSmilArea* CSmilRegion::AreaAt(const TPoint& aPoint, TBool /*aOnlyFocusable*/)
	{
	MediaListItem* i = iVisibleMedia;
	CSmilArea* found = 0;
	for (;i;i=i->iNext)
		{
		TRect r = GetRectangle(i->iMedia);
		if (r.Contains(aPoint))
			{
			found = i->iMedia->GetArea();
			if (found && found->Contains(aPoint))
				return found;
			else
				return 0;
			}
		}

	return found;
	}


TInt CalcD2(TPoint cent, TPoint aPoint, CSmilRegion::TDirection aDir, TBool cone) 
	{

	TInt dx = cent.iX - aPoint.iX;
	TInt dy = cent.iY - aPoint.iY;
	TInt dxdy = Abs(dx)-Abs(dy);
	switch (aDir)
		{
		case CSmilRegion::ELeft:
			if (dx<0 && (cone==EFalse || dxdy>0)) break;
			return KMaxTInt;
		case CSmilRegion::ERight:
			if (dx>0 && (cone==EFalse || dxdy>0)) break;
			return KMaxTInt;
		case CSmilRegion::ETop:
			if (dy<0 && (cone==EFalse || dxdy<0)) break;
			return KMaxTInt;
		case CSmilRegion::EBottom:
			if (dy>0 && (cone==EFalse || dxdy<0)) break;
			return KMaxTInt;
		case CSmilRegion::EBottomTwoWay:
			if (dy>0 || (dy==0 && dx>0)) break;
			return KMaxTInt;
		case CSmilRegion::ETopTwoWay:
			if (dy<0 || (dy==0 && dx<0)) break;
			return KMaxTInt;
		default:
			;
		}
	TInt d2;
	if (aDir==CSmilRegion::EBottomTwoWay) 
		d2 = dy*1000 + cent.iX;
	else if (aDir==CSmilRegion::ETopTwoWay)
		d2 = -dy*1000 - cent.iX;
	else
		d2 = dx*dx + dy*dy;
	return d2;
	}

CSmilObject* CSmilRegion::FindNearest(const TPoint& aPoint, TDirection iDir, TInt& dist2)
	{
	TBool cone = ETrue;
	CSmilObject* found = 0;

	// try first to find media within 90 degree arc in direction, then 180 deg
	while (!found)
		{
		MediaListItem* i = iVisibleMedia;
		for (;i;i=i->iNext)
			{
			CSmilMedia* m = i->iMedia;
			if (!m->IsFocusable() && !m->FirstChild())
				continue;
			TRect r = GetRectangle(m);			
			if (m->IsFocusable())
				{				
				TPoint cent = r.Center();
				TInt d2 = CalcD2(cent,aPoint,iDir,cone);
				if (d2<dist2)
					{
					dist2 = d2;
					found = i->iMedia;
					}

				}
			else
				{
				for (CSmilObject* o = m->FirstChild(); o; o=o->NextSibling())
					{
					if (o->IsArea() && o->IsFocusable())
						{
						CSmilArea* area = static_cast<CSmilArea*>(o);
						TPoint cent = area->Center();
						TInt d2 = CalcD2(cent,aPoint,iDir,cone);
						if (d2<dist2)
							{
							dist2 = d2;
							found = o;
							}
						}					
					}				
				}
			
				
			}
		if (cone) 
			cone = EFalse;
		else
			break;
		}
	return found;

	}



// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  





