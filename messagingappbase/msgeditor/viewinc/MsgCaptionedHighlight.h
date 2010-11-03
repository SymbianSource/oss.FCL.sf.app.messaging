/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgCaptionedHighlight  declaration
*
*/



#ifndef INC_MSGCAPTIONEDHIGHLIGHT_H
#define INC_MSGCAPTIONEDHIGHLIGHT_H

// ========== INCLUDE FILES ================================

#include <coecntrl.h>
#include <AknUtils.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgHeader;

// ========== CLASS DECLARATION ============================

class CMsgCaptionedHighlight : public CCoeControl
    {
    public:
        static CMsgCaptionedHighlight* NewL( const CMsgHeader& aParent );

    public: //From CCoeControl
        /**
         * For handling dynamic layout switch.
         */
        void HandleResourceChange( TInt aType );

    protected: // from CCoeControl
        IMPORT_C void Draw( const TRect& aRect ) const;

    private:
        CMsgCaptionedHighlight( const CMsgHeader& aParent );
        void ConstructL();
        void ResolveLayouts();
        void DrawFrame( CWindowGc& aGc, const TRect& aRect ) const;
        
        /**
        * Updates the text color from currently used skin.
        */
        void UpdateTextColor();
        
    private:
        const CMsgHeader& iParent;
        TAknLayoutRect iFrame;
        TAknLayoutRect iVertBar;
        TAknLayoutText iText;
        TInt iTextBaselineOffset;
        TInt iTextGap;
        
        /** Offset from main pane top to data pane top. */
        TInt iBaseLineOffset;
        
        /** Current text color. */
        TRgb iTextColor;
    };

#endif

// End of File
