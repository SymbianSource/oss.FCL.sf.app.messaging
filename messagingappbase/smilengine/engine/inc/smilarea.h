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
* Description: smilarea  declaration
*
*/



#ifndef SMILAREA_H
#define SMILAREA_H

//  INCLUDES
#include <e32base.h>

#include "smilanchor.h"
#include "smilobject.h"

#include <smillength.h>
#include <smiltime.h>

#include <smilmedia.h>
#include <smilfocusinterface.h>
// FORWARD DECLARATIONS
class CGraphicsContext;
template <class T> class CLinkedList;

// CLASS DECLARATION


class CSmilArea : public CSmilObject, public MSmilFocus
    {
    public:  // Constructors and destructor

		/**
        * Destructor
        */
		virtual ~CSmilArea();

		void ConstructL(CSmilPresentation* aPresentation);
		static CSmilArea* NewL(CSmilPresentation* aPresentation);

    public: // New functions


		TBool IsFocusable() const;
		TBool IsVisible() const;
		void SetFocus(TBool aVal) { iFocus = aVal; }
		TBool HasFocus() { return iFocus; }

		void BeginL( const TSmilTime& aTime );
		void EndL( const TSmilTime& aTime );
		void Unfreeze();

		TBool IsArea() const { return ETrue; }

		void Draw(CGraphicsContext& aGc, const TRect& aRect);
		void ResolveL( const TSmilTime& aTps );

		void SetTimeContainer(CSmilTimeContainer* aTimeContainer);
		CSmilMedia* LinkedMedia();

		TPoint Center() const;

		void Redraw();

		TBool Contains(const TPoint& aPoint);
		TInt GetDistance(const TPoint& aPoint, const TPoint& bPoint);
		TInt CrossLine(const TPoint& Point1, const TPoint& Point2, const TPoint& aPoint);

	protected:

		// from MSmilFocus
		TRect Rect() const;
		TShape Shape() const;
		const CArrayFixFlat<TPoint>* PolyPoints() const;
		TBool IsSelected() const { return EFalse; }


		CSmilTimeContainer* iTimeContainer;
		/**
        * C++ default constructor.
        */
        CSmilArea();	
		
	public:
		
		TShape iShape;
		CLinkedList<TSmilLength>* iCoords;

		TBool iActive;
		TBool iFocus;

		CArrayFixFlat<TPoint> iPoints;			
		
	};

#endif      // ?INCLUDE_H   
            
// End of File
