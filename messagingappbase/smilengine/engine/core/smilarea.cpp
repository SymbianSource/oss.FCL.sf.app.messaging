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
* Description: smilarea implementation
*
*/



// INCLUDE FILES
#include "smillength.h"
#include "smilarea.h"
#include "smilmedia.h"
#include "smilrootregion.h"
#include "smilpresentation.h"
#include "smillinkedlist.h"

#include "gdi.h"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//

CSmilArea::CSmilArea()
: CSmilObject(),
iPoints(2)
	{	
	}


// EPOC default constructor can leave.
void CSmilArea::ConstructL(CSmilPresentation* aPresentation)
	{
	CSmilObject::ConstructL(aPresentation);
	iShape = ERect;
	iActive = EFalse;
	iAnchor = new (ELeave) CSmilAnchor;
	iTimeContainer = 0;
	iCoords = new (ELeave) CLinkedList<TSmilLength>();
	}

// Two-phased constructor.
CSmilArea* CSmilArea::NewL(CSmilPresentation* aPresentation)
	{
	CSmilArea* ptr = new ( ELeave ) CSmilArea;
	CleanupStack::PushL( ptr );
	ptr->ConstructL(aPresentation);
	CleanupStack::Pop(); //ptr
	return ptr;
	}

// EPOC default constructor can leave.
CSmilArea::~CSmilArea()
	{	
	delete iCoords;
	}


TBool CSmilArea::IsFocusable() const
	{
	return iActive;
	}

TBool CSmilArea::IsVisible() const
	{
	return iActive&&iParentObject->IsVisible();
	}

void CSmilArea::BeginL( const TSmilTime& aT )
	{
	CSmilObject::BeginL(aT);
	iActive = ETrue;

	iPresentation->UpdateFocusListL();

	Redraw();
	}

void CSmilArea::EndL( const TSmilTime& aT )
	{
	CSmilObject::EndL(aT);

	iPresentation->UpdateFocusListL();

	Redraw();
	}

void CSmilArea::Unfreeze()
	{
	iActive = EFalse;
	iFocus = EFalse;

	TRAPD(err,iPresentation->UpdateFocusListL());
	if (err==KErrNone)
		Redraw();
	}


void CSmilArea::SetTimeContainer(CSmilTimeContainer* aTimeContainer)
{
	iTimeContainer = aTimeContainer;
}

CSmilMedia* CSmilArea::LinkedMedia()
{
	if (iTimeContainer->IsMedia())
		return (static_cast<CSmilMedia*>(iTimeContainer));
	else
		return 0;

}

TBool CSmilArea::Contains(const TPoint& aPoint)
{
	CLinkedList<TSmilLength>::Iter iter(*iCoords);
	TRect rect;
	TRect mrect = static_cast<CSmilMedia*>(iParentObject)->GetRectangle();

	switch (iShape)
		{		
		case ERect: 
			{
			TSmilLength l1,l2,l3,l4;
			if (iter.HasMore()) l1 = iter.Next();
			if (iter.HasMore()) l2 = iter.Next();
			if (iter.HasMore()) l3 = iter.Next();
			if (iter.HasMore()) 
				{
				l4 = iter.Next();				
				TPoint p1(l1.GetLength(mrect.Width()),l2.GetLength(mrect.Height()));
				TPoint p2(l3.GetLength(mrect.Width()),l4.GetLength(mrect.Height()));
				rect = TRect(p1+mrect.iTl,p2+mrect.iTl);
				}
			else
				{
				rect=mrect;
				}
			
			if (aPoint.iX<rect.iTl.iX || aPoint.iX>=rect.iBr.iX || aPoint.iY<rect.iTl.iY || aPoint.iY>=rect.iBr.iY)
				return(EFalse);
			else 
				return (ETrue);
			}
		case ECircle:
			{
			TInt min = mrect.Width()<mrect.Height()?mrect.Width():mrect.Height();
			TSmilLength l1,l2,r;
			if (iter.HasMore()) l1 = iter.Next();
			else break;
			if (iter.HasMore()) l2 = iter.Next();
			else break;
			if (iter.HasMore()) r = iter.Next();
			else break;
			TPoint c(l1.GetLength(mrect.Width()),l2.GetLength(mrect.Height()));
			TPoint rr(r.GetLength(min),r.GetLength(min));
			rect = TRect(c-rr+mrect.iTl,c+rr+mrect.iTl);		
			
			TInt dist = GetDistance(TPoint(aPoint.iX - mrect.iTl.iX, aPoint.iY - mrect.iTl.iY), c);
			if (dist > r.GetLength(min))
				return(EFalse);
			else 
				return (ETrue);
			}
		case EPoly:
			{
			TSmilLength l1,l2;
			TInt crossing = 0; 
			TPoint p1, p2;

			if (iter.HasMore()) l1 = iter.Next();
			else break;
			if (iter.HasMore()) l2 = iter.Next();
			else break;
					
			p1.SetXY(l1.GetLength(mrect.Width()),l2.GetLength(mrect.Height()));
		
			do
			{
				if (iter.HasMore()) l1 = iter.Next();
				else break;
				if (iter.HasMore()) l2 = iter.Next();
				else break;

				p2.SetXY(l1.GetLength(mrect.Width()),l2.GetLength(mrect.Height()));

				TInt Xmin = p1.iX<p2.iX?p1.iX:p2.iX;
				TInt Ymax = p1.iY>p2.iY?p1.iY:p2.iY;
				if ((Xmin < aPoint.iX) && (aPoint.iY < Ymax))
				{
					crossing += CrossLine(p1, p2, aPoint);
					p1.SetXY(p2.iX, p2.iY);
				}
				else break;
			
			} while (iter.HasMore());

			if ((!crossing) || (crossing % 2 == 0)) return (EFalse);
			else return (ETrue);

			}
		default:
			return ETrue;
			;
		}
	return EFalse;
}

TInt CSmilArea::CrossLine(const TPoint& aL1, const TPoint& aL2, const TPoint& aPoint)
{
	TInt yMin = aL1.iY<aL2.iY?aL1.iY:aL2.iY;
	TInt yMax = aL1.iY>aL2.iY?aL1.iY:aL2.iY;

	if (aPoint.iY>yMin && aPoint.iY<yMax)
		{
		TInt dx = aL1.iX-aL2.iX;
		TInt dy = aL1.iY-aL2.iY;
		TInt x = dx*(aPoint.iY-aL1.iY)/dy+aL1.iX;
		if (x<aPoint.iX)
			return 1;
		}

	return 0;
}



TInt CSmilArea::GetDistance(const TPoint& aPoint, const TPoint& bPoint)
{
	TReal a, b, res, a2, b2;
	a = aPoint.iY - bPoint.iY;
	b = aPoint.iX - bPoint.iX;

	Math::Pow(a2, a, 2);
	Math::Pow(b2, b, 2);
	res = a2 + b2;
	Math::Sqrt(res, res);
	return (TInt)res;
}


void CSmilArea::ResolveL( const TSmilTime& aTps )
	{
	CSmilObject::ResolveL(aTps);

	}

TPoint CSmilArea::Center() const
	{			
	return Rect().Center();	
	}

void CSmilArea::Redraw()
	{
	if (iParentObject->IsMedia())
		static_cast<CSmilMedia*>(iParentObject)->Redraw();	
	}

MSmilFocus::TShape CSmilArea::Shape() const
	{
	return iShape;
	}

const CArrayFixFlat<TPoint>* CSmilArea::PolyPoints() const
	{
	if (iShape!=EPoly)
		return 0;
		
	if (!iParentObject->IsMedia())		
		return 0;	
			
	if (iPoints.Count()==0)
		{
		TRect mrect = static_cast<CSmilMedia*>(iParentObject)->GetRectangle();
		TInt w = mrect.Width();
		TInt h = mrect.Height();
		CLinkedList<TSmilLength>::Iter iter(*iCoords);
		while (iter.HasMore())
			{								
			TInt x = iter.Next().GetLength(w);
			TInt y;
			if (iter.HasMore())
				y = iter.Next().GetLength(h);
			else
				break;

			TPoint p(x,y);
			p = p + mrect.iTl;
			TRAPD(err, const_cast<CSmilArea*>(this)->iPoints.AppendL(p));
			if (err != KErrNone)
				break;
			if (iPoints.Count()==16) 
				break;
			}
		}
	return &iPoints;
	}

TRect CSmilArea::Rect() const
	{
	TRect mrect;
	if (iParentObject->IsMedia())
		mrect = static_cast<CSmilMedia*>(iParentObject)->GetRectangle();
	else
		return TRect();

	CLinkedList<TSmilLength>::Iter iter(*iCoords);

	if (!iter.HasMore())
		return mrect;

	TRect rect;
	switch (iShape)
		{		
		case ERect: 
			{
			TSmilLength l1,l2,l3,l4;
			if (iter.HasMore()) l1 = iter.Next();
			else break;
			if (iter.HasMore()) l2 = iter.Next();
			else break;
			if (iter.HasMore()) l3 = iter.Next();
			else break;
			if (iter.HasMore()) l4 = iter.Next();
			else break;
			TPoint p1(l1.GetLength(mrect.Width()),l2.GetLength(mrect.Height()));
			TPoint p2(l3.GetLength(mrect.Width()),l4.GetLength(mrect.Height()));
			rect = TRect(p1+mrect.iTl,p2+mrect.iTl);
			break;
			}
		case ECircle:
			{
			TInt min = mrect.Width()<mrect.Height()?mrect.Width():mrect.Height();
			TSmilLength l1,l2,r;
			if (iter.HasMore()) l1 = iter.Next();
			else break;
			if (iter.HasMore()) l2 = iter.Next();
			else break;
			if (iter.HasMore()) r = iter.Next();
			else break;
			TPoint c(l1.GetLength(mrect.Width()),l2.GetLength(mrect.Height()));
			TPoint rr(r.GetLength(min),r.GetLength(min));
			rect = TRect(c-rr+mrect.iTl,c+rr+mrect.iTl);			
			break;
			}
		case EPoly:
			{			
			TInt w = mrect.Width();
			TInt h = mrect.Height();
			TInt minx=10000,miny=10000,maxx=-10000,maxy=-10000;
			while (iter.HasMore())
				{								
				TInt x = iter.Next().GetLength(w);
				TInt y;
				if (iter.HasMore())
					y = iter.Next().GetLength(h);
				else
					break;

				if (x<minx) minx=x;
				if (x>maxx) maxx=x;
				if (y<miny) miny=y;
				if (y>maxy) maxy=y;
				}
			if (minx<maxx && miny<maxy)
				{
				return TRect(TPoint(minx,miny)+mrect.iTl,TPoint(maxx,maxy)+mrect.iTl);
				}
			}
			break;
		default:
			;
		}
	return rect;
	}

// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File  



