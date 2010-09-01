/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgDummyHeaderCleaner  declaration
*
*/



#ifndef INC_MSGDUMMYHEADERCLEANER_H
#define INC_MSGDUMMYHEADERCLEANER_H

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

// This class is used to clear the area between the status/navipane
// and the uppermost control on the screen

NONSHARABLE_CLASS( CMsgDummyHeaderCleaner ) : public CCoeControl
    {
    public:
        CMsgDummyHeaderCleaner( const CMsgHeader* aParent );
        void ConstructL( );

    public: // Methods from base classes
        
        /**
        *
        */
        void HandleResourceChange( TInt aType );        
        
    protected: // from CCoeControl
        void Draw( const TRect& aRect ) const;

    private:
        const CMsgHeader* iParent; // Pointer to parent control
        TAknLayoutRect iDashBar; // Clear only around this rect, not inside

    };

#endif

// End of File
