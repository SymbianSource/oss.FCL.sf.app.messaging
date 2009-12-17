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
* Description: smilrootregion  declaration
*
*/



#ifndef SMILROOTREGION_H
#define SMILROOTREGION_H

//  INCLUDES
#include "smilregion.h"

// CLASS DECLARATION

class CSmilRootRegion : public CSmilRegion
    {
    public:  // Constructors and destructor

		/**
        * Two-phased constructor.
        */
        static CSmilRootRegion* NewL( CSmilPresentation* aPresentation );
        
        /**
        * Destructor.
        */
        virtual ~CSmilRootRegion();
		

    public: // New functions

		virtual TBool IsRoot() const { return ETrue; }

		virtual void Redraw();
		virtual void Redraw(TRect iUpdate);

		virtual void AddChild(CSmilRegion* aChild);

		virtual TRect GetRectangle() const;

		/**
		* Calculate the region positions
		*/
		virtual void Layout(TReal32 aScalingFactor = 1);

		// drawing

		virtual void Draw(CGraphicsContext& aGc, const TRect& aRect);

		// child management

		virtual CSmilRegion* FirstChild() const { return iFirstChild; }
		virtual CSmilRegion* LastChild() const { return iLastChild; }

		virtual void SetFirstChild(CSmilRegion* c) { iFirstChild = c; } 
		virtual void SetLastChild(CSmilRegion* c ) { iLastChild = c; } 

		CSmilRegion* FindRegion(const TDesC& aId);

		void SetDefaultLayout(TBool aDef=ETrue);

		CSmilMedia* MediaAt(const TPoint& aPoint,TBool aOnlyFocusable=EFalse);
		CSmilArea* AreaAt(const TPoint& aPoint, TBool aOnlyFocusable=EFalse);

		CSmilObject* FindNearest(const TPoint& aPoint, TDirection iDir);		
		TReal32 GetScalingFactor() ;

	protected:

		CSmilObject* FindNearest(const TPoint& aPoint, TDirection iDir, TInt& dist2);

		CSmilRootRegion(CSmilPresentation* aPresentation);

		// child management

		CSmilRegion* iFirstChild;
		CSmilRegion* iLastChild;
		
		TReal32 iScalingFactor;

    };

#endif      // ?INCLUDE_H   
            
// End of File
