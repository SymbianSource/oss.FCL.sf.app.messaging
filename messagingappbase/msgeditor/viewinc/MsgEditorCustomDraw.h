/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgEditorCustomDraw  declaration
*
*/



#ifndef CMSGEDITORCUSTOMDRAW_H
#define CMSGEDITORCUSTOMDRAW_H

// ========== INCLUDE FILES ================================

#include <e32base.h>
#include <frmtlay.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================
class CGulIcon;
class CEikEdwin;

// ========== CLASS DECLARATION ============================

/**
*  Implementation of MFormCustomDraw
*       Class is used to draw editor lines to text editor.
*
* @since 3.1
*/
NONSHARABLE_CLASS(CMsgEditorCustomDraw): public CBase,
                                         public MFormCustomDraw
    {
    public:

        /**
        * Static constructor.
        *
        * @param    aParentCustomDraw IN Parent Custom drawer.
        *
        * @return   pointer to instance
        */
        static CMsgEditorCustomDraw* NewL( const MFormCustomDraw* aParentCustomDraw,
                                           const CEikEdwin* aParentControl,const TInt aControlType );

        /**
        * Destructor
        */
        virtual ~CMsgEditorCustomDraw();
        
        /**
        * Resolves editor line layout from LAF.
        */
        void ResolveLayouts();
        
        /**
         * Notifies custom draw that skin has changed.
         */
        void SkinChanged();
        
    public: // Functions from base classes
        
        /**
        * From MFormCustomDraw
        * See frmtlay.h for more information.
        */
        void DrawBackground( const TParam& aParam,
                             const TRgb& aRgb,
                             TRect& aDrawn ) const;

        /**
        * From MFormCustomDraw
        * See frmtlay.h for more information.
        */
        void DrawText( const TParam& aParam,
                       const TLineInfo& aLineInfo,
                       const TCharFormat& aFormat,
                       const TDesC& aText,
                       const TPoint& aTextOrigin,
                       TInt aExtraPixels ) const;
                       
        /**
        * From MFormCustomDraw
        * See frmtlay.h for more information.
        */
        
        void DrawText( const TParam& aParam, const TLineInfo& aLineInfo, 
            const TCharFormat& aFormat, const TDesC& aText, const TInt aStart, 
            const TInt aEnd, const TPoint& aTextOrigin, TInt aExtraPixels ) const;
        
        /**
        * From MFormCustomDraw
        * See frmtlay.h for more information.
        */
        void DrawLineGraphics( const TParam& aParam, 
                               const TLineInfo& aLineInfo ) const;
        
        /**
        * From MFormCustomDraw
        * See frmtlay.h for more information.
        */
	    TRgb SystemColor( TUint aColorIndex, 
	                      TRgb aDefaultColor) const;
        
    private:

        /**
        * C++ contructor
        */
        CMsgEditorCustomDraw( const MFormCustomDraw* aParentCustomDraw,
                              const CEikEdwin* aParentControl,const TInt aControlType );
                                     
        /**
        * 2nd phase constructor
        */
        void ConstructL();
        
        /**
        * Draws editor lines.
        */
        void DrawEditorLines( const TParam& aParam ) const; 

    private: // data

        const MFormCustomDraw* iParentCustomDraw;
        
        TRect iFirstLineRect;
        TInt iLineDelta;
        
        CGulIcon* iIcon;
        
        const CEikEdwin* iParentControl;
        TInt iControlType;
      
        };


#endif // CMSGEDITORCUSTOMDRAW_H

// End of File
