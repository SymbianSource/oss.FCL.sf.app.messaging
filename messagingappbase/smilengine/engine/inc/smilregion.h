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
* Description: smilregion  declaration
*
*/



#ifndef SMILREGION_H
#define SMILREGION_H

//  INCLUDES
#include <e32def.h>
#include <e32base.h>

#include "smiltextbuf.h"

#include "smillength.h"
#include "smilregioninterface.h"

// FORWARD DECLARATIONS
class CSmilPresentation;
class CSmilEvent;
class CSmilObject;
class CSmilArea;

// CLASS DECLARATION

class CSmilRegion : public CBase, public MSmilRegion
    {
    public:  // Constructors and destructor

		friend class CSmilRootRegion;

		/**
        * Two-phased constructor.
        */
        static CSmilRegion* NewL( CSmilPresentation* aPresentation );
        
        /**
        * Destructor.
        */
        virtual ~CSmilRegion();
		

    public: // New functions

		/**
		* Following methods are an implementation of MSmilRegion interface
		*/
		virtual TRect GetRectangle() const;
		virtual TRect GetRectangle(const CSmilMedia* aMedia) const;

		virtual TInt GetZIndex() const;
		virtual TFit GetFit() const;
		virtual TRgb GetBackground() const;
		virtual TRgb GetForeground() const;
		virtual TBgMode GetBackgroundMode() const;
		virtual TBool BackgroundTransparent() const { return iTransparent; }

		virtual void Redraw();
		virtual void Redraw(TRect iUpdate);

		virtual MSmilRegion* Parent() const;
		virtual MSmilRegion* Root() const;

		virtual void AddMediaL(CSmilMedia* aMedia);
		virtual void RemoveMedia(CSmilMedia* aMedia);
		virtual TBool ContainsMedia(const CSmilMedia* aMedia) const;

		virtual TBool IsRoot() const { return EFalse; }

		virtual void MediaTransitionEnd(CSmilMedia* aMedia);


		virtual CSmilMedia* MediaAt(const TPoint& aPoint,TBool aOnlyFocusable=EFalse);
		virtual CSmilArea* AreaAt(const TPoint& aPoint, TBool aOnlyFocusable=EFalse);

		enum TDirection { ELeft, ERight, ETop, EBottom, ETopTwoWay, EBottomTwoWay, EAll };

		/**
		* Calculate the region positions
		*/
		virtual void Layout(TReal32 aScalingFactor = 1);

		// drawing

		virtual void Draw(CGraphicsContext& aGc, const TRect& aRect);

		// child management

		virtual void AddChild(CSmilRegion* ) { }
		virtual CSmilRegion* FirstChild() const { return 0; } 
		virtual CSmilRegion* LastChild() const { return 0; }

		virtual void SetFirstChild(CSmilRegion*) { } 
		virtual void SetLastChild(CSmilRegion*) { } 

		virtual CSmilRegion* NextSibling() const { return iNext; }
		virtual CSmilRegion* PreviousSibling() const { return iPrevious; }


		//debug

		virtual void PrintTree(int d=0) const;


		// style attributes
		TSmilLength iWidth;
		TSmilLength iHeight;
		TSmilLength iTop;
		TSmilLength iBottom;
		TSmilLength iLeft;
		TSmilLength iRight;

		TInt iZIndex;

		TFit iFit;

		TRgb iBackground;
		TRgb iForeground;
		TBgMode iBackgroundMode;
		TBool iTransparent;

		TPtrC Id() const { return iId.Text(); }
		TPtrC Name() const { return iName.Text(); }
		void SetId(const TDesC& aId) { iId.SetTextL(aId); }
		void SetName(const TDesC& aName) { iName.SetTextL(aName); }

		RSmilTextBuf iId;
		RSmilTextBuf iName;		

	protected:

		virtual CSmilObject* FindNearest(const TPoint& aPoint, TDirection iDir, TInt& dist2);

		CSmilRegion(CSmilPresentation* aPresentation);
		       
		//other

		CSmilPresentation* iPresentation;
		TRect iRect;

		TBool iDefaultLayout;

		struct MediaListItem
			{
			MediaListItem(CSmilMedia* aMedia) { iMedia=aMedia; iNext=0; }
			CSmilMedia* iMedia;
			MediaListItem* iNext;
			TRect iArea;
			};

		MediaListItem* iVisibleMedia;

		// child management

		CSmilRegion* iNext;
		CSmilRegion* iPrevious;
		CSmilRegion* iParent;


    };

#endif      // ?INCLUDE_H   
            
// End of File
