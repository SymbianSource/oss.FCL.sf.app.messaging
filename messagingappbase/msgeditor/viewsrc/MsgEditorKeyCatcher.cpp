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
* Description:  MsgEditorKeyCatcher implementation
*
*/



// ========== INCLUDE FILES ================================

#include <eikenv.h>
#include <eikappui.h>

#include "MsgEditorKeyCatcher.h"
#include "MsgAddressControlEditor.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgEditorKeyCatcher::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CMsgEditorKeyCatcher::ConstructL( CMsgAddressControlEditor* aEditor )
    {
    CreateWindowL();
    SetNonFocusing();

#ifdef RD_SCALABLE_UI_V2
    SetPointerCapture( EFalse );
#endif // RD_SCALABLE_UI_V2
    
    RWindow& window=Window();
    window.SetOrdinalPosition( 0, ECoeWinPriorityFep );
    TPoint fepControlPos( 0, 0 );
    SetExtent( fepControlPos, TSize( 0, 0 ) );
    window.SetNonFading( ETrue );
    iEikonEnv->EikAppUi()->AddToStackL(
        this,
        ECoeStackPriorityFep + 10,
        ECoeStackFlagRefusesFocus | ECoeStackFlagSharable );
    iEditor = aEditor;
    }

// ---------------------------------------------------------
// CMsgEditorKeyCatcher::CMsgEditorKeyCatcher
//
// Constructor.
// ---------------------------------------------------------
//
CMsgEditorKeyCatcher::CMsgEditorKeyCatcher()
    {
    }

// ---------------------------------------------------------
// CMsgEditorKeyCatcher::~CMsgEditorKeyCatcher
//
// Destructor.
// ---------------------------------------------------------
//
CMsgEditorKeyCatcher::~CMsgEditorKeyCatcher()
    {
    iEikonEnv->EikAppUi()->RemoveFromStack( this );
    }

// ---------------------------------------------------------
// CMsgEditorKeyCatcher::OfferKeyEventL
//
// Handles key event.
// ---------------------------------------------------------
//
TKeyResponse CMsgEditorKeyCatcher::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( aType == EEventKey )
        {
        // All printable characters except # which means user is changing case.
        // SCT of address control contains less characters, however.
        // There is a drawback here: pressing cancel in SCT still may add delimiters.
        if ( aKeyEvent.iCode >= 32 &&
            aKeyEvent.iCode <= 255 &&
            aKeyEvent.iCode != '#' )
            {
            iEditor->PreInsertEditorFormattingL( aKeyEvent );
            return EKeyWasNotConsumed;
            }
        }

    return EKeyWasNotConsumed;
    }

//  End of File
