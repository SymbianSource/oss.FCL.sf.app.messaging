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
* Description: smilmedia  declaration
*
*/



#ifndef SMILMEDIA_H
#define SMILMEDIA_H

//  INCLUDES
#include <e32base.h>

#include <smilmediainterface.h>
#include <smilfocusinterface.h>

#include "smiltimecontainer.h"
#include "smilregioninterface.h"

// FORWARD DECLARATIONS
class MSmilMediaRenderer;
class CSmilTransitionFilter;
class CSmilTransition;
class CSmilArea;

// CLASS DECLARATION

class CSmilMedia : public CSmilTimeContainer, public MSmilMedia, public MSmilFocus
    {
    public:  // Constructors and destructor

		virtual const TDesC* ClassName() const { _LIT(KN,"CSmilMedia"); return &KN; }

		static CSmilMedia* NewL(CSmilPresentation* aPresentation);
        
        virtual ~CSmilMedia();
		
    public: // New functions

		/**
		* Returns the region where this media is supposed to be rendered on. May be NULL.
		*/ 
		MSmilRegion* GetRegion() const;

		/**
		* Returns the rectangle (region) where the media is supposed to be rendered on.
		*/
		TRect GetRectangle() const;

		/**
		* Invoked by renderer to ask redrawing of area covered by the media. 
		* This should be invoked when (visual) media appears, there are changes to it, and 
		* when it is removed.
		*/
		void Redraw();

		/** 
		* Invoked by a MSmillMediaRenderer object when there is a change 
		* in (known) state of its intrinsic values. For example, duration of 
		* video clip is not typically known until it is at least partially 
		* decoded. This method should be invoked by the renderer when this 
		* information becomes available.
		*/
		void RendererDurationChangedL();

		/**
		* Invoked by a MSmillMediaRenderer object when the associated media 
		* ends (end of the file or stream is reached). Renderer should also 
		* move to frozen state at this point (that is, to keep showing the last 
		* frame). 
		*/
		void RendererAtEndL();

		/**
		* Presentation
		*/
		CSmilPresentation* Presentation() const;

		/**
		 * Get name media parameter, or empty TPtrC if not found
		 */
		TPtrC GetParam(const TDesC& aName) const;
        
        /**
		 * Returns begin time of media relative to presentation begin time.
		 */
		TSmilTime MediaBegin() const;
        
        /**
		 * Returns end time of media relative to presentation begin time.
		 */
		TSmilTime MediaEnd() const;
		
		TPtrC Src() const { return iSrc.Text(); }
		TPtrC Alt() const { return iAlt.Text(); }
		TPtrC Type() const { return iType.Text(); }

		void SetAltL( const TDesC &aPtr );
		void SetSrcL( const TDesC& aString );	
		void SetTypeL( const TDesC& aString );
	
		void ForceRepeat( TBool aBool );	        		
		
		void SetRegion( MSmilRegion* aRegion ) { iRegion=aRegion; }

		MSmilMediaRenderer* GetRenderer() const { return iRenderer; }
		
		void TransitionFilterEnd(CSmilTransitionFilter* );

		void SetFocus(TBool aFocus) { iFocus = aFocus;}
		TBool HasFocus() const { return iFocus; }

		TBool IsFocusable() const;
		void SetFocusable(TBool aFocusable) { iFocusable = aFocusable;}

		void Unfreeze();	

		virtual void SetPaused(TBool aPaused);

		TPoint Center() const { return GetRectangle().Center(); }

		TSmilTime ClipBegin() { return iClipBegin; }
		TSmilTime ClipEnd() { return iClipEnd; }

		void AfterL(MSmilActive* iActive, const TSmilTime& aDelay, TBool aLazy = EFalse);
		void CancelActive(MSmilActive* aActive);

		CSmilTransitionFilter* ActiveTransitionFilter() const;

		void AddParamL(const TDesC& aName, const TDesC& aValue);

		TBool IsVisible() const;
		TBool IsScrollable() const;

		TSmilTime PredictedRemovalTime() const;

		CSmilArea* GetArea();

    protected:  // Functions from base classes

		CSmilMedia();
        
		virtual void ProceedL( const TSmilTime& aTime );
		virtual void ResolveL( const TSmilTime& aTps );

		virtual void RepeatL( const TSmilTime& aTime );
		virtual void BeginL( const TSmilTime& aTime );

		virtual void EndL( const TSmilTime& aTime );

		virtual TSmilTime ImplicitDuration() const;

		TSmilTime NextActivation( const TSmilTime& aTime ) const;

		TBool IsMedia() const { return ETrue; }			

		// from MSmilFocus
		TRect Rect() const { return GetRectangle(); }
		TShape Shape() const { return ERect; }
		const CArrayFixFlat<TPoint>* PolyPoints() const { return 0; }
		TBool IsSelected() const;

	private:
		void CreateRendererL(const TSmilTime& aRecalcTime);

	protected:
		RSmilTextBuf			iSrc;
		RSmilTextBuf			iAlt;
		RSmilTextBuf			iType;

		TBool					iFocus;
		TBool					iFocusable;

    private:    // Data
		MSmilRegion*			iRegion;
		MSmilMediaRenderer*		iRenderer;
		
		CSmilTransitionFilter*	iTransitionFilter;

	public:		
		CSmilTransition*		iTransitionIn;		
		CSmilTransition*		iTransitionOut;	

		struct CNameValuePair
			{
			~CNameValuePair() { delete iName; delete iValue; }
			HBufC* iName;
			HBufC* iValue;
			};
		RPointerArray<CNameValuePair> iParams;

		TSmilTime iClipBegin;
		TSmilTime iClipEnd;

    };

#endif      // ?INCLUDE_H   
            
// End of File
