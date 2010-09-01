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
* Description:  PostcardTextDialogCustomDraw  declaration
*
*/



#ifndef CPOSTCARDTEXTDIALOGCUSTOMDRAW_H
#define CPOSTCARDTEXTDIALOGCUSTOMDRAW_H

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
class CPostcardTextDialog;

// ========== CLASS DECLARATION ============================

/**
*  Implementation of MFormCustomDraw
*       Class is used to draw editor lines to text editor.
*
* @since 3.1
*/
NONSHARABLE_CLASS(CPostcardTextDialogCustomDraw): public CBase,
                                         public MFormCustomDraw
    {
    public:

        /**
        * Static constructor.
        * @since S60 3.2
        * @param    aParentCustomDraw IN Parent Custom drawer.
        *
        * @return   pointer to instance
        */
        static CPostcardTextDialogCustomDraw* NewL(
            const MFormCustomDraw* aParentCustomDraw,
            const CPostcardTextDialog* aParentControl );

        /**
        * Destructor
        * @since S60 3.2
        */
        virtual ~CPostcardTextDialogCustomDraw();
        
        /**
        * Resolves editor line layout from LAF.
        * @since S60 3.2
        */
        void ResolveLayouts();
        
    public: // Functions from base classes
        
        /**
        * From MFormCustomDraw
        * See frmtlay.h for more information.
        * @since S60 3.2
        */
        void DrawBackground( const TParam& aParam,
                             const TRgb& aRgb,
                             TRect& aDrawn ) const;

        /**
        * From MFormCustomDraw
        * See frmtlay.h for more information.
        * @since S60 3.2
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
        * @since S60 3.2
        */
        void DrawLineGraphics( const TParam& aParam, 
                               const TLineInfo& aLineInfo ) const;
        
        /**
        * From MFormCustomDraw
        * See frmtlay.h for more information.
        * @since S60 3.2
        */
	    TRgb SystemColor( TUint aColorIndex, 
	                      TRgb aDefaultColor) const;
        
    private:

        /**
        * C++ contructor
        * @since S60 3.2
        */
        CPostcardTextDialogCustomDraw(
            const MFormCustomDraw* aParentCustomDraw,
            const CPostcardTextDialog* aParentControl );
                                     
        /**
        * 2nd phase constructor
        * @since S60 3.2
        */
        void ConstructL();
        
        /**
        * Draws editor lines.
        * @since S60 3.2
        */
        void DrawEditorLines( const TParam& aParam ) const; 

    private: // data

        const MFormCustomDraw* iParentCustomDraw;
        
        TRect iFirstLineRect;
        TInt iLineDelta;
        
        CGulIcon* iIcon;

        const CPostcardTextDialog* iParentControl;
        };


#endif // CPOSTCARDTEXTDIALOGCUSTOMDRAW_H

// End of File
